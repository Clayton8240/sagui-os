/* duas-janelas - teste da H21: abre dois programas com uma janela cada e
 * espera os dois terminarem. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    char *a[] = { "janela", "Janela A", "3368652", NULL };   /* 0x3366CC, azul */
    char *b[] = { "janela", "Janela B", "13382451", NULL };  /* 0xCC3333, vermelho */
    int pa = executar("/testes/janela", a);
    dormir(300);
    int pb = executar("/testes/janela", b);
    imprimir("duas-janelas: processos %d e %d\n", pa, pb);
    for (int i = 0; i < 2; i++) {   /* na ordem em que terminarem */
        int c, pid = esperar(-1, &c);
        imprimir("duas-janelas: %s terminou\n", pid == pa ? "A" : "B");
    }
    return 0;
}
