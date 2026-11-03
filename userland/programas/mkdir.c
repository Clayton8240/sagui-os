/* mkdir (criar-pasta) - cria pastas. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc < 2) { imprimir_em(ERROS, "uso: mkdir <pasta...>\n"); return 1; }
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        int r = criar_pasta(argv[i]);
        if (r < 0) { imprimir_em(ERROS, "mkdir: %s: %s\n", argv[i], nome_erro(r)); erros++; }
    }
    return erros ? 1 : 0;
}
