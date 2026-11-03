/* sobre - versão e créditos do SaguiOS. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    static info_processo_t v[64];
    int procs = info_sistema(INFO_PROCESSOS, v, 64);
    info_memoria_t m;
    info_sistema(INFO_MEMORIA, &m, sizeof m);
    imprimir("SaguiOS 1.0: um sistema operacional escrito do zero por IA.\n");
    imprimir("Kernel de 32 bits com paginação, processos no anel 3, multitarefa\n");
    imprimir("preemptiva, chamadas de sistema, arquivos na RAM e pipes.\n");
    imprimir("Escrito por Claude (Anthropic) para o projeto do Eli Rigobeli, sem código de\n");
    imprimir("outros sistemas e sem bibliotecas prontas.\n");
    imprimir("Agora: %d processos, %u MB livres de memória.\n", procs, m.livre_kb / 1024);
    imprimir("Leia /sobre.txt e /historia/saguios.txt. O mascote está em /arte/sagui.txt.\n");
    return 0;
}
