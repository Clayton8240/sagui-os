/* mem - memória total, usada e livre. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    info_memoria_t m;
    if (info_sistema(INFO_MEMORIA, &m, sizeof m) < 0) return 1;
    imprimir("Memória RAM total (E820):   %u MB\n", m.ram_total_kb / 1024);
    imprimir("Gerenciada pelo kernel:     %u MB\n", m.gerida_kb / 1024);
    imprimir("Usada:                      %u KB\n", m.usada_kb);
    imprimir("Livre:                      %u MB (%u KB)\n", m.livre_kb / 1024, m.livre_kb);
    return 0;
}
