/* rm (apagar) - apaga arquivos. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc < 2) { imprimir_em(ERROS, "uso: rm <arquivo...>\n"); return 1; }
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        /* No pendrive, apagar é para valer: pede confirmação (PRD 3.2). */
        char c[160];
        pasta_atual(c, sizeof c);
        bool no_pendrive = comeca_com(argv[i], "/usb/") || (argv[i][0] != '/' && (compara_texto(c, "/usb") == 0 || comeca_com(c, "/usb/")));
        if (no_pendrive) {
            imprimir("Apagar %s do pendrive? (s/n) ", argv[i]);
            char resp[8];
            if (ler_linha(ENTRADA, resp, sizeof resp) < 0 || (resp[0] != 's' && resp[0] != 'S')) {
                imprimir("Não apagado.\n");
                continue;
            }
        }
        int r = apagar(argv[i]);
        if (r < 0) {
            imprimir_em(ERROS, "rm: %s: %s%s\n", argv[i], nome_erro(r), r == ERRO_E_PASTA ? " (use rmdir)" : "");
            erros++;
        }
    }
    return erros ? 1 : 0;
}
