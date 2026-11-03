/* montar [disco] - monta o pendrive em /usb (sem nome: o primeiro FAT32). */
#include "sagui.h"

int principal(int argc, char **argv)
{
    char nome[16] = "";
    if (argc > 1) copia_texto(nome, argv[1], sizeof nome);
    else {
        info_disco_t d;
        for (int i = 0; listar_discos(i, &d) == 1; i++)
            if (d.suportado) { copia_texto(nome, d.nome, sizeof nome); break; }
        if (!nome[0]) { imprimir_em(ERROS, "montar: nenhum pendrive FAT32 conectado\n"); return 1; }
    }
    int r = montar(nome);
    if (r == ERRO_FORMATO) { imprimir_em(ERROS, "montar: %s: formato não suportado (só FAT32)\n", nome); return 1; }
    if (r == ERRO_OCUPADO) { imprimir_em(ERROS, "montar: já há um pendrive montado em /usb\n"); return 1; }
    if (r < 0) { imprimir_em(ERROS, "montar: %s: %s\n", nome, nome_erro(r)); return 1; }
    imprimir("Pendrive montado em /usb\n");
    return 0;
}
