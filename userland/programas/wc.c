/* wc (conta) - conta linhas, palavras e letras (bytes) de um arquivo ou da entrada. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    int fd = ENTRADA;
    if (argc > 1) {
        fd = abrir(argv[1], ABRIR_LER);
        if (fd < 0) { imprimir_em(ERROS, "wc: %s: %s\n", argv[1], nome_erro(fd)); return 1; }
    }
    char buf[512];
    unsigned linhas = 0, palavras = 0, letras = 0;
    bool dentro = false;
    int n;
    while ((n = ler(fd, buf, sizeof buf)) > 0) {
        for (int i = 0; i < n; i++) {
            letras++;
            if (buf[i] == '\n') linhas++;
            if (eh_espaco(buf[i])) dentro = false;
            else if (!dentro) { dentro = true; palavras++; }
        }
    }
    imprimir("%u linhas, %u palavras, %u letras\n", linhas, palavras, letras);
    return 0;
}
