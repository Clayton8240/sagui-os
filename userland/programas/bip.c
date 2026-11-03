/* bip [frequência [duração]] - toca uma nota no alto-falante do PC (B3).
 * Sem argumentos: 880 Hz por 200 ms. "bip escala" toca dó-ré-mi-fá-sol-lá-si-dó. */
#include "sagui.h"

static int numero(const char *s, int *v)
{
    int n = 0;
    if (!*s) return 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9') return 0;
        n = n * 10 + (*s - '0');
        if (n > 100000) return 0;
    }
    *v = n;
    return 1;
}

int principal(int argc, char **argv)
{
    if (argc > 1 && compara_texto(argv[1], "escala") == 0) {
        static const nota_t escala[] = {
            {523, 150}, {587, 150}, {659, 150}, {698, 150}, {784, 150}, {880, 150}, {988, 150}, {1047, 300},
        };
        tocar(escala, 8, BIP_SUBSTITUIR);
        imprimir("bip: escala de dó maior\n");
        dormir(1500);
        return 0;
    }
    int freq = 880, ms = 200;
    if ((argc > 1 && !numero(argv[1], &freq)) || (argc > 2 && !numero(argv[2], &ms)) ||
        freq < 20 || freq > 20000 || ms < 10 || ms > 10000) {
        imprimir_em(ERROS, "uso: bip [frequência 20-20000 Hz [duração 10-10000 ms]] ou bip escala\n");
        return 1;
    }
    bip(freq, ms);
    imprimir("bip: %d Hz por %d ms\n", freq, ms);
    dormir((unsigned)ms);   /* o comando só termina quando o som acaba */
    return 0;
}
