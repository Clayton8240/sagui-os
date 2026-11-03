/* contador [segundos] - conta segundos, para mostrar a multitarefa. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    int total = argc > 1 ? texto_para_int(argv[1]) : 10;
    if (total <= 0) total = 10;
    int pid = meu_pid();
    for (int i = 1; i <= total; i++) {
        dormir(1000);
        imprimir("[contador %d] %d de %d segundos\n", pid, i, total);
    }
    return 0;
}
