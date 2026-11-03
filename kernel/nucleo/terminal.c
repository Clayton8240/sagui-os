/* terminal.c - terminal provisório dentro do kernel (H09).
 *
 * Existe só para testar teclado, arquivos, memória e relógio antes de haver
 * processos. Na H16 ele é removido e o shell passa a ser um programa comum
 * (sagsh), rodando no anel 3. Por isso aqui tudo é direto e simples: uma
 * linha lida do teclado, separada em palavras, e uma tabela de comandos. */
#include "terminal.h"
#include "caminho.h"
#include "console.h"
#include "cpu.h"
#include "memoria.h"
#include "portas.h"
#include "relogio.h"
#include "sgfs.h"
#include "teclado.h"
#include "texto.h"

#define MAX_LINHA    120
#define MAX_ARGS     8
#define LINHAS_TELA  25
#define COLUNAS_TELA 80

static char pasta_atual[CAMINHO_MAX] = "/";

/* ------------------------------------------------------------ paginação */

/* Conta as linhas mostradas desde o último comando. A tela não tem rolagem
 * para trás, então textos longos param a cada tela cheia. */
static int linhas_mostradas, coluna;
static bool parar_saida;

static bool pausa_mais(void)
{
    vga_cor(COR_PRETO, COR_CINZA);
    kprintf("-- mais: Espaço ou Enter continua, q sai --");
    vga_cor(COR_CINZA, COR_PRETO);
    int t;
    do t = teclado_ler(); while (t != ' ' && t != '\n' && t != 'q' && t != 'Q' && t != TECLA_ESC);
    kprintf("\r                                           \r");
    linhas_mostradas = 0;
    return t == ' ' || t == '\n';
}

static void saida_char(char c)
{
    if (parar_saida) return;
    if (c == '\r') return;
    kprintf("%c", c);
    if (c == '\n' || ++coluna >= COLUNAS_TELA) {
        coluna = 0;
        if (++linhas_mostradas >= LINHAS_TELA - 2 && !pausa_mais())
            parar_saida = true;
    }
}

static void saida(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    formatar(buf, sizeof buf, fmt, ap);
    va_end(ap);
    for (char *p = buf; *p; p++) saida_char(*p);
}

static void erro(const char *fmt, const char *arg)
{
    vga_cor(COR_VERMELHO_CLARO, COR_PRETO);
    saida(fmt, arg);
    vga_cor(COR_CINZA, COR_PRETO);
}

/* ------------------------------------------------------------ comandos */

typedef struct {
    const char *nome;
    const char *uso;
    const char *descricao;
    void (*executar)(int argc, char **argv);
} comando_t;

static const comando_t comandos[];

static void cmd_ajuda(int argc, char **argv)
{
    (void)argc; (void)argv;
    saida("Comandos do terminal provisório do SaguiOS:\n");
    for (const comando_t *c = comandos; c->nome; c++) {
        vga_cor(COR_AMARELO, COR_PRETO);
        saida("  %-16s", c->uso);
        vga_cor(COR_CINZA, COR_PRETO);
        saida("%s\n", c->descricao);
    }
    saida("Teclado ABNT2: acentos com as teclas de acento, depois a letra.\n");
}

/* Resolve um argumento contra a pasta atual e procura no SGFS. */
static const sgfs_entrada_t *achar(const char *arg, char *caminho)
{
    if (!caminho_resolver(pasta_atual, arg, caminho, CAMINHO_MAX))
        return NULL;
    return sgfs_procurar(caminho);
}

static void cmd_ls(int argc, char **argv)
{
    char caminho[CAMINHO_MAX];
    const sgfs_entrada_t *p = achar(argc > 1 ? argv[1] : ".", caminho);
    if (!p) { erro("ls: não existe: %s\n", argc > 1 ? argv[1] : "."); return; }
    if (p->tipo != SGFS_PASTA) { saida("%s  %u bytes\n", sgfs_nome(p), p->tamanho); return; }
    uint32_t pos = 0, n = 0;
    const sgfs_entrada_t *e;
    while ((e = sgfs_proximo_filho(caminho, &pos))) {
        if (e->tipo == SGFS_PASTA) {
            vga_cor(COR_AZUL_CLARO, COR_PRETO);
            saida("  %-30s", sgfs_nome(e));
            vga_cor(COR_CINZA, COR_PRETO);
            saida("  <pasta>\n");
        } else {
            vga_cor(e->tipo == SGFS_PROGRAMA ? COR_VERDE_CLARO : COR_CINZA, COR_PRETO);
            saida("  %-30s", sgfs_nome(e));
            vga_cor(COR_CINZA, COR_PRETO);
            saida("  %u bytes\n", e->tamanho);
        }
        n++;
    }
    if (n == 0) saida("  (pasta vazia)\n");
}

static void cmd_cd(int argc, char **argv)
{
    char caminho[CAMINHO_MAX];
    const char *alvo = argc > 1 ? argv[1] : "/";
    const sgfs_entrada_t *e = achar(alvo, caminho);
    if (!e) { erro("cd: não existe: %s\n", alvo); return; }
    if (e->tipo != SGFS_PASTA) { erro("cd: não é uma pasta: %s\n", alvo); return; }
    txt_copia_n(pasta_atual, caminho, sizeof pasta_atual);
}

static void cmd_cat(int argc, char **argv)
{
    if (argc < 2) { erro("uso: cat <arquivo>%s\n", ""); return; }
    for (int i = 1; i < argc && !parar_saida; i++) {
        char caminho[CAMINHO_MAX];
        const sgfs_entrada_t *e = achar(argv[i], caminho);
        if (!e) { erro("cat: não existe: %s\n", argv[i]); continue; }
        if (e->tipo == SGFS_PASTA) { erro("cat: é uma pasta: %s\n", argv[i]); continue; }
        char bloco[128];
        uint32_t desloc = 0, n;
        while (!parar_saida && (n = sgfs_ler(e, desloc, bloco, sizeof bloco)) > 0) {
            for (uint32_t k = 0; k < n; k++) saida_char(bloco[k]);
            desloc += n;
        }
        if (coluna) saida_char('\n');
    }
}

static void cmd_limpar(int argc, char **argv)
{
    (void)argc; (void)argv;
    vga_limpar();
}

static void cmd_mem(int argc, char **argv)
{
    (void)argc; (void)argv;
    estat_memoria_t e;
    memoria_estatisticas(&e);
    uint32_t gerida_kb = e.paginas_geridas * 4, livre_kb = e.paginas_livres * 4;
    saida("Memória RAM total (mapa E820):     %u MB\n", (uint32_t)(e.ram_total >> 20));
    saida("Gerenciada pelo kernel:            %u MB (%u páginas de 4 KB)\n", gerida_kb / 1024, e.paginas_geridas);
    saida("Usada:                             %u KB\n", gerida_kb - livre_kb);
    saida("Livre:                             %u MB (%u KB)\n", livre_kb / 1024, livre_kb);
    saida("Alocador do kernel (kmalloc):      %u blocos em uso, %u grandes, %u págs.\n",
          e.heap_blocos_usados, e.heap_grandes, e.heap_paginas);
}

static void cmd_data(int argc, char **argv)
{
    (void)argc; (void)argv;
    static const char *semana[] = { "domingo", "segunda-feira", "terça-feira", "quarta-feira",
                                    "quinta-feira", "sexta-feira", "sábado" };
    data_hora_t d;
    cmos_ler_data(&d);
    int ds = d.ano >= 2000 ? dia_da_semana(d.ano, d.mes, d.dia) : 0;
    saida("%s, %02u/%02u/%u %02u:%02u:%02u (relógio da placa-mãe)\n",
          semana[ds], d.dia, d.mes, d.ano, d.hora, d.minuto, d.segundo);
}

static void cmd_tempo(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint32_t s = relogio_segundos();
    saida("Ligado há %u h %02u min %02u s (%u tiques do relógio a 100 Hz)\n",
          s / 3600, (s / 60) % 60, s % 60, relogio_tiques());
}

SEM_RETORNO void maquina_reiniciar(void);

static void cmd_reiniciar(int argc, char **argv)
{
    (void)argc; (void)argv;
    saida("Reiniciando...\n");
    relogio_dormir_ms(200);
    maquina_reiniciar();
}

static const comando_t comandos[] = {
    { "ajuda",     "ajuda",          "mostra esta lista",                         cmd_ajuda },
    { "ls",        "ls [pasta]",     "lista arquivos e pastas",                   cmd_ls },
    { "cd",        "cd <pasta>",     "entra numa pasta (cd .. volta)",            cmd_cd },
    { "cat",       "cat <arquivo>",  "mostra o conteúdo de um arquivo",           cmd_cat },
    { "limpar",    "limpar",         "limpa a tela",                              cmd_limpar },
    { "mem",       "mem",            "memória total, usada e livre",              cmd_mem },
    { "data",      "data",           "data e hora do relógio da placa-mãe",      cmd_data },
    { "tempo",     "tempo",          "quanto tempo o sistema está ligado",        cmd_tempo },
    { "reiniciar", "reiniciar",      "reinicia o computador",                     cmd_reiniciar },
    { NULL, NULL, NULL, NULL },
};

/* ------------------------------------------------------------ leitura da linha */

static void prompt(void)
{
    vga_cor(COR_VERDE_CLARO, COR_PRETO);
    kprintf("sagui:%s$ ", pasta_atual);
    vga_cor(COR_CINZA, COR_PRETO);
}

/* Lê uma linha com eco. Backspace apaga, Ctrl+C cancela, Ctrl+L limpa a tela. */
static int ler_linha(char *linha)
{
    int n = 0;
    for (;;) {
        int t = teclado_ler();
        if (t == '\n') { kprintf("\n"); linha[n] = 0; return n; }
        if (t == '\b') { if (n > 0) { n--; kprintf("\b"); } continue; }
        if (t == 3) { kprintf("^C\n"); return -1; }
        if (t == 12) { vga_limpar(); prompt(); linha[n] = 0; kprintf("%s", linha); continue; }
        if (t == '\t') t = ' ';
        if (t >= 0x20 && t < 0x100 && t != 0x7F && n < MAX_LINHA - 1) {
            linha[n++] = (char)t;
            kprintf("%c", t);
        }
    }
}

static int separar(char *linha, char **argv)
{
    int argc = 0;
    char *p = linha;
    while (*p && argc < MAX_ARGS) {
        while (*p == ' ') *p++ = 0;
        if (!*p) break;
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
    }
    return argc;
}

void terminal_executar(void)
{
    char linha[MAX_LINHA];
    char *argv[MAX_ARGS];
    vga_cor(COR_AMARELO, COR_PRETO);
    kprintf("\nTerminal provisório do kernel. Digite ajuda para ver os comandos.\n");
    vga_cor(COR_CINZA, COR_PRETO);
    klog("[SAGUI] terminal pronto\n");
    for (;;) {
        prompt();
        if (ler_linha(linha) <= 0)
            continue;
        int argc = separar(linha, argv);
        if (argc == 0)
            continue;
        linhas_mostradas = coluna = 0;
        parar_saida = false;
        const comando_t *c;
        for (c = comandos; c->nome; c++)
            if (txt_compara(c->nome, argv[0]) == 0)
                break;
        if (c->nome)
            c->executar(argc, argv);
        else
            erro("Comando desconhecido: %s. Digite ajuda.\n", argv[0]);
        klog("[SAGUI] fim do comando\n");
    }
}
