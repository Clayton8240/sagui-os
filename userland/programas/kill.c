/* kill (matar) - encerra um processo pelo PID. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc != 2) { imprimir_em(ERROS, "uso: kill <pid>\n"); return 1; }
    int pid = texto_para_int(argv[1]);
    int r = matar(pid);
    if (r < 0) { imprimir_em(ERROS, "kill: %d: %s\n", pid, nome_erro(r)); return 1; }
    imprimir("Processo %d encerrado.\n", pid);
    return 0;
}
