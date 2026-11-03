/* entrada.c - leitura de linhas. */
#include "sagui.h"

/* Lê até o fim da linha (o '\n' não entra no buffer). Devolve o tamanho
 * da linha, ou -1 se o arquivo acabou antes de ler qualquer coisa. Lê um
 * byte por vez: assim nunca consome o que vem depois da linha, o que
 * importa quando outro programa vai ler o resto do mesmo pipe. */
int ler_linha(int fd, char *buf, unsigned max)
{
    unsigned n = 0;
    for (;;) {
        char c;
        int r = ler(fd, &c, 1);
        if (r <= 0) {
            if (n == 0) return -1;
            break;
        }
        if (c == '\n') break;
        if (n + 1 < max) buf[n++] = c;
    }
    buf[n] = 0;
    return (int)n;
}
