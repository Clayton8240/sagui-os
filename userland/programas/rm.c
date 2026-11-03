/* rm (apagar) - apaga arquivos. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc < 2) { imprimir_em(ERROS, "uso: rm <arquivo...>\n"); return 1; }
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        int r = apagar(argv[i]);
        if (r < 0) {
            imprimir_em(ERROS, "rm: %s: %s%s\n", argv[i], nome_erro(r), r == ERRO_E_PASTA ? " (use rmdir)" : "");
            erros++;
        }
    }
    return erros ? 1 : 0;
}
