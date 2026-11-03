/* cp (copiar) - copia um arquivo. Se o destino for uma pasta, copia para
 * dentro dela com o mesmo nome. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    if (argc != 3) { imprimir_em(ERROS, "uso: cp <origem> <destino>\n"); return 1; }
    char destino[160];
    info_arquivo_t inf;
    copia_texto(destino, argv[2], sizeof destino);
    if (info_arquivo(argv[2], &inf) == 0 && inf.tipo == TIPO_PASTA) {
        const char *nome = acha_ultimo_char(argv[1], '/');
        nome = nome ? nome + 1 : argv[1];
        size_t n = tam_texto(destino);
        if (n && destino[n - 1] != '/') junta_texto(destino, "/", sizeof destino);
        junta_texto(destino, nome, sizeof destino);
    }
    int o = abrir(argv[1], ABRIR_LER);
    if (o < 0) { imprimir_em(ERROS, "cp: %s: %s\n", argv[1], nome_erro(o)); return 1; }
    int d = abrir(destino, ABRIR_ESCREVER | ABRIR_CRIAR | ABRIR_TRUNCAR);
    if (d < 0) { imprimir_em(ERROS, "cp: %s: %s\n", destino, nome_erro(d)); fechar(o); return 1; }
    char buf[1024];
    int n, total = 0;
    while ((n = ler(o, buf, sizeof buf)) > 0) {
        int w = escrever_tudo(d, buf, (unsigned)n);
        if (w < 0) { imprimir_em(ERROS, "cp: %s: %s\n", destino, nome_erro(w)); break; }
        total += n;
    }
    fechar(o);
    fechar(d);
    return n < 0 ? 1 : 0;
}
