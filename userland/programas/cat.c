/* cat (ver) - mostra arquivos; sem argumentos, copia a entrada para a saída. */
#include "sagui.h"

static int copiar(int fd)
{
    char buf[512];
    int n;
    while ((n = ler(fd, buf, sizeof buf)) > 0)
        if (escrever_tudo(SAIDA, buf, (unsigned)n) < 0) return 1;
    return n < 0;
}

int principal(int argc, char **argv)
{
    if (argc < 2) return copiar(ENTRADA);
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        int fd = abrir(argv[i], ABRIR_LER);
        if (fd < 0) { imprimir_em(ERROS, "cat: %s: %s\n", argv[i], nome_erro(fd)); erros++; continue; }
        erros += copiar(fd);
        fechar(fd);
    }
    return erros ? 1 : 0;
}
