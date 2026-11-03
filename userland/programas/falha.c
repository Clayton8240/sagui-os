/* falha [tipo] - provoca um erro de propósito para mostrar a proteção: só
 * este processo é encerrado pelo kernel; o shell e o sistema continuam. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    const char *tipo = argc > 1 ? argv[1] : "memoria";
    imprimir("falha: vou provocar um erro do tipo \"%s\" (processo %d)...\n", tipo, meu_pid());
    if (compara_texto(tipo, "kernel") == 0) {
        *(volatile int *)0x00100000 = 1;           /* escrever na memória do kernel */
    } else if (compara_texto(tipo, "divisao") == 0) {
        /* Dividendo "grande" de propósito: com 1 / x o GCC troca a divisão
         * por comparações (o resultado só pode ser -1, 0 ou 1) e a CPU nunca
         * chega a dividir. */
        volatile int zero = 0, dividendo = 12345;
        volatile int x = dividendo / zero;
        (void)x;
    } else if (compara_texto(tipo, "instrucao") == 0) {
        __asm__ volatile("cli");                   /* instrução privilegiada */
    } else {
        *(volatile int *)0 = 1;                    /* ponteiro nulo */
    }
    imprimir("falha: o erro não aconteceu?!\n");
    return 1;
}
