/* mv (mover) - move ou renomeia um arquivo ou pasta. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc != 3) { imprimir_em(ERROS, "uso: mv <origem> <destino>\n"); return 1; }
    int r = renomear(argv[1], argv[2]);
    if (r < 0) { imprimir_em(ERROS, "mv: %s: %s\n", argv[1], nome_erro(r)); return 1; }
    return 0;
}
