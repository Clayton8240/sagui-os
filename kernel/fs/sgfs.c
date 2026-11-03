/* sgfs.c - leitura do ramdisk SGFS.
 *
 * O SGFS é uma tabela de entradas com caminhos completos, ordenada, seguida
 * pelos dados. Não há estrutura de árvore: uma pasta é só uma entrada do tipo
 * pasta, e os filhos dela são as entradas cujo caminho começa com
 * "pasta/" e não tem outra barra depois. Isso deixa o leitor muito simples
 * e é suficiente para um ramdisk somente leitura (a escrita entra na H14,
 * numa camada na RAM por cima deste). */
#include "sgfs.h"
#include "console.h"
#include "texto.h"

typedef struct EMPACOTADO {
    char     assinatura[4];
    uint16_t versao;
    uint16_t tam_entrada;
    uint32_t quantidade;
    uint32_t desloc_tabela;
    uint32_t desloc_dados;
    uint32_t tamanho_total;
    uint32_t soma;
    uint32_t reservado;
} sgfs_cabecalho_t;

static const uint8_t *base;
static const sgfs_entrada_t *tabela;
static uint32_t quantidade, total;
static const sgfs_entrada_t raiz = { .caminho = "", .tipo = SGFS_PASTA };

bool sgfs_iniciar(const void *inicio, uint32_t bytes)
{
    const sgfs_cabecalho_t *c = inicio;
    if (bytes < sizeof *c || memcmp(c->assinatura, "SGFS", 4) != 0) {
        klog("[SGFS] assinatura nao encontrada\n");
        return false;
    }
    if (c->versao != 1 || c->tam_entrada != sizeof(sgfs_entrada_t) || c->tamanho_total > bytes ||
        c->desloc_tabela + c->quantidade * sizeof(sgfs_entrada_t) > c->tamanho_total) {
        klog("[SGFS] cabecalho incoerente\n");
        return false;
    }
    uint32_t soma = 0;
    for (uint32_t i = sizeof *c; i < c->tamanho_total; i++)
        soma += ((const uint8_t *)inicio)[i];
    if (soma != c->soma) {
        klog("[SGFS] soma de verificacao nao confere\n");
        return false;
    }
    base = inicio;
    tabela = (const sgfs_entrada_t *)(base + c->desloc_tabela);
    quantidade = c->quantidade;
    total = c->tamanho_total;
    /* Recusa entradas com dados fora da imagem ou caminho sem terminador. */
    for (uint32_t i = 0; i < quantidade; i++) {
        const sgfs_entrada_t *e = &tabela[i];
        if (e->caminho[SGFS_MAX_CAMINHO - 1] != 0 || (e->tamanho && e->desloc + e->tamanho > total)) {
            klog("[SGFS] entrada %u invalida\n", i);
            quantidade = 0;
            return false;
        }
    }
    return true;
}

uint32_t sgfs_quantidade(void) { return quantidade; }

const sgfs_entrada_t *sgfs_entrada(uint32_t i)
{
    return i < quantidade ? &tabela[i] : NULL;
}

const sgfs_entrada_t *sgfs_procurar(const char *caminho)
{
    while (*caminho == '/')
        caminho++;
    if (!*caminho)
        return &raiz;
    /* A tabela está ordenada, mas com poucas dezenas de entradas uma busca
     * linear é mais simples e igualmente rápida. */
    for (uint32_t i = 0; i < quantidade; i++)
        if (txt_compara(tabela[i].caminho, caminho) == 0)
            return &tabela[i];
    return NULL;
}

const sgfs_entrada_t *sgfs_proximo_filho(const char *pasta, uint32_t *pos)
{
    while (*pasta == '/')
        pasta++;
    size_t n = txt_tamanho(pasta);
    for (; *pos < quantidade; (*pos)++) {
        const char *c = tabela[*pos].caminho;
        const char *resto;
        if (n == 0) {
            resto = c;
        } else {
            if (txt_compara_n(c, pasta, n) != 0 || c[n] != '/')
                continue;
            resto = c + n + 1;
        }
        bool direto = true;
        for (const char *p = resto; *p; p++)
            if (*p == '/') { direto = false; break; }
        if (direto && *resto)
            return &tabela[(*pos)++];
    }
    return NULL;
}

uint32_t sgfs_ler(const sgfs_entrada_t *e, uint32_t desloc, void *destino, uint32_t n)
{
    if (!e || e->tipo == SGFS_PASTA || desloc >= e->tamanho)
        return 0;
    if (n > e->tamanho - desloc)
        n = e->tamanho - desloc;
    memcpy(destino, base + e->desloc + desloc, n);
    return n;
}

const char *sgfs_nome(const sgfs_entrada_t *e)
{
    const char *nome = e->caminho;
    for (const char *p = e->caminho; *p; p++)
        if (*p == '/') nome = p + 1;
    return nome;
}

const uint8_t *sgfs_dados(const sgfs_entrada_t *e)
{
    return base + e->desloc;
}
