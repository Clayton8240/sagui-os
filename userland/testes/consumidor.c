/* consumidor N - lê a entrada 0 até o fim e confere se veio exatamente o
 * padrão do gerador, com N bytes. Sai com 0 se estiver tudo certo. */
#include "sagui.h"
#include "padrao.h"

int principal(int argc, char **argv)
{
    unsigned esperado = argc > 1 ? (unsigned)texto_para_int(argv[1]) : 1000;
    char buf[100];
    unsigned total = 0, erros = 0;
    int n;
    while ((n = ler(ENTRADA, buf, sizeof buf)) > 0) {
        for (int i = 0; i < n; i++)
            if (buf[i] != byte_do_padrao(total + (unsigned)i)) erros++;
        total += (unsigned)n;
    }
    imprimir("consumidor: recebi %u bytes (esperava %u), %u diferentes\n", total, esperado, erros);
    return (total == esperado && erros == 0) ? 0 : 1;
}
