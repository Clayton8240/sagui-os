/* echo (eco) - escreve os argumentos separados por espaço. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        imprimir("%s%s", argv[i], i < argc - 1 ? " " : "");
    imprimir("\n");
    return 0;
}
