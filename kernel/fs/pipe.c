/* pipe.c - comunicação entre processos por um buffer circular no kernel.
 *
 * Um pipe tem duas pontas, cada uma um arquivo aberto: quem escreve numa
 * ponta, outro lê na outra. Regras (H15):
 *   - ler com o pipe vazio: se ainda há quem escreva, o leitor dorme; se não
 *     há mais nenhum escritor, devolve 0 (fim do arquivo);
 *   - escrever com o pipe cheio: o escritor dorme até alguém ler; escreve o
 *     que couber e devolve quanto escreveu (a libsagui insiste no resto);
 *   - escrever sem nenhum leitor: erro "pipe quebrado".
 * Leitores e escritores dormem no próprio pipe como canal; qualquer mudança
 * acorda todos, e cada um repete a chamada (D010) e confere de novo. */
#include "pipe.h"
#include "abi.h"
#include "memoria.h"
#include "processo.h"
#include "sistema.h"

pipe_t *pipe_criar(void)
{
    pipe_t *p = kcalloc(sizeof(pipe_t));
    if (p) { p->leitores = 1; p->escritores = 1; }
    return p;
}

int32_t pipe_ler(pipe_t *p, char *destino, uint32_t n)
{
    if (n == 0) return 0;
    if (p->qtd == 0) {
        if (p->escritores == 0) return 0;
        bloquear(p);
        return SIS_REPETIR;
    }
    uint32_t k = 0;
    while (k < n && p->qtd) {
        destino[k++] = (char)p->buf[p->ini];
        p->ini = (p->ini + 1) % PIPE_TAM;
        p->qtd--;
    }
    acordar(p);   /* abriu espaço: escritores podem continuar */
    return (int32_t)k;
}

int32_t pipe_escrever(pipe_t *p, const char *origem, uint32_t n)
{
    if (p->leitores == 0) return ERRO_PIPE_QUEBRADO;
    if (n == 0) return 0;
    if (p->qtd == PIPE_TAM) {
        bloquear(p);
        return SIS_REPETIR;
    }
    uint32_t k = 0;
    while (k < n && p->qtd < PIPE_TAM) {
        p->buf[(p->ini + p->qtd) % PIPE_TAM] = (uint8_t)origem[k++];
        p->qtd++;
    }
    acordar(p);   /* chegaram dados: leitores podem continuar */
    return (int32_t)k;
}

void pipe_fechar_ponta(pipe_t *p, bool escrita)
{
    if (escrita) p->escritores--; else p->leitores--;
    acordar(p);   /* um leitor esperando precisa descobrir que acabou */
    if (p->leitores <= 0 && p->escritores <= 0)
        kfree(p);
}
