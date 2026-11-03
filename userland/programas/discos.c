/* discos - lista os pendrives: nome, fabricante, produto, tamanho, formato,
 * rótulo e se está montado (H34). */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    info_disco_t d;
    int n = 0;
    for (int i = 0; listar_discos(i, &d) == 1; i++, n++) {
        unsigned mb = d.setores / 2048;
        char tam[24];
        if (mb >= 1024) formatar_texto(tam, sizeof tam, "%u,%u GB", mb / 1024, (mb % 1024) * 10 / 1024);
        else formatar_texto(tam, sizeof tam, "%u MB", mb);
        if (d.suportado)
            imprimir("%-5s %s %s  %s  FAT32  \"%s\"  (%s)\n", d.nome, d.fabricante, d.produto, tam, d.rotulo,
                     d.montado ? "montado em /usb" : "não montado");
        else
            imprimir("%-5s %s %s  %s  %s\n", d.nome, d.fabricante, d.produto, tam, d.formato);
    }
    if (n == 0) imprimir("Nenhum pendrive conectado.\n");
    return 0;
}
