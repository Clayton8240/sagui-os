/* desmontar [disco] - grava tudo o que estiver pendente e libera o pendrive. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    char nome[16] = "";
    if (argc > 1) copia_texto(nome, argv[1], sizeof nome);
    else {
        info_disco_t d;
        for (int i = 0; listar_discos(i, &d) == 1; i++)
            if (d.montado) { copia_texto(nome, d.nome, sizeof nome); break; }
        if (!nome[0]) { imprimir_em(ERROS, "desmontar: nenhum pendrive montado\n"); return 1; }
    }
    int r = desmontar(nome);
    if (r < 0) { imprimir_em(ERROS, "desmontar: %s: %s\n", nome, nome_erro(r)); return 1; }
    imprimir("Pode remover o pendrive.\n");
    return 0;
}
