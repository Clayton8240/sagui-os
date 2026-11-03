/* rmdir (remover-pasta) - remove pastas vazias. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc < 2) { imprimir_em(ERROS, "uso: rmdir <pasta...>\n"); return 1; }
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        int r = remover_pasta(argv[i]);
        if (r < 0) { imprimir_em(ERROS, "rmdir: %s: %s\n", argv[i], nome_erro(r)); erros++; }
    }
    return erros ? 1 : 0;
}
