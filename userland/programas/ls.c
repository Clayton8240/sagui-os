/* ls (listar) - lista arquivos e pastas. Uma entrada por linha, para que
 * "ls bin | conta" conte os programas. */
#include "sagui.h"

static int listar_pasta(const char *pasta)
{
    entrada_pasta_t e;
    int i = 0, r;
    while ((r = listar(pasta, i, &e)) == 1) {
        if (e.tipo == TIPO_PASTA) imprimir("%s/\n", e.nome);
        else if (e.tipo == TIPO_PROGRAMA) imprimir("%-28s %7u bytes  programa\n", e.nome, e.tamanho);
        else imprimir("%-28s %7u bytes\n", e.nome, e.tamanho);
        i++;
    }
    if (r < 0) {
        info_arquivo_t inf;
        if (r == ERRO_NAO_E_PASTA && info_arquivo(pasta, &inf) == 0) {   /* ls de um arquivo */
            imprimir("%-28s %7u bytes\n", pasta, inf.tamanho);
            return 0;
        }
        imprimir_em(ERROS, "ls: %s: %s\n", pasta, nome_erro(r));
        return 1;
    }
    return 0;
}

int principal(int argc, char **argv)
{
    if (argc < 2) return listar_pasta(".");
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        if (argc > 2) imprimir("%s:\n", argv[i]);
        erros += listar_pasta(argv[i]);
    }
    return erros ? 1 : 0;
}
