/* gerador N - escreve N bytes de um padrão conhecido na saída 1. */
#include "sagui.h"
#include "padrao.h"

int principal(int argc, char **argv)
{
    unsigned n = argc > 1 ? (unsigned)texto_para_int(argv[1]) : 1000;
    char bloco[300];   /* blocos de tamanho "estranho" para cruzar a borda do buffer do pipe */
    unsigned feito = 0;
    while (feito < n) {
        unsigned k = 0;
        while (k < sizeof bloco && feito + k < n) { bloco[k] = byte_do_padrao(feito + k); k++; }
        if (escrever_tudo(SAIDA, bloco, k) != (int)k) return 1;
        feito += k;
    }
    return 0;
}
