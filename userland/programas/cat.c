/* cat (ver) - mostra arquivos; sem argumentos, copia a entrada para a saída. */
#include "sagui.h"

/* Copia para a saída convertendo UTF-8 em Latin-1 (D027). Um começo de
 * sequência que caia no fim de um bloco espera o bloco seguinte. */
static int copiar(int fd)
{
    char buf[513];
    size_t pendente = 0;
    int n;
    while ((n = ler(fd, buf + pendente, (unsigned)(sizeof buf - 1 - pendente))) > 0) {
        size_t total = pendente + (size_t)n, sobra;
        size_t m = utf8_para_latin1(buf, total, &sobra);
        if (escrever_tudo(SAIDA, buf, (unsigned)(m - sobra)) < 0) return 1;
        if (sobra) buf[0] = buf[m - 1];
        pendente = sobra;
    }
    if (pendente) escrever_tudo(SAIDA, buf, (unsigned)pendente);
    return n < 0;
}

int principal(int argc, char **argv)
{
    if (argc < 2) return copiar(ENTRADA);
    int erros = 0;
    for (int i = 1; i < argc; i++) {
        int fd = abrir(argv[i], ABRIR_LER);
        if (fd < 0) { imprimir_em(ERROS, "cat: %s: %s\n", argv[i], nome_erro(fd)); erros++; continue; }
        erros += copiar(fd);
        fechar(fd);
    }
    return erros ? 1 : 0;
}
