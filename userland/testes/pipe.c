/* pipe - teste da H15: dois processos ligados por um pipe, transferindo um
 * texto muito maior que o buffer do kernel (512 bytes), nos dois sentidos. */
#include "sagui.h"
#include "verifica.h"
#include "padrao.h"

#define TOTAL 20000

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    char num[16];
    formatar_texto(num, sizeof num, "%d", TOTAL);
    imprimir("Teste de pipes (H15): %d bytes por um pipe de 512\n", TOTAL);

    /* 1. Filho escreve, este processo lê. */
    int p[2];
    verifica(criar_pipe(p) == 0, "cria o pipe");
    int guarda = duplicar(SAIDA, 10);           /* guarda o terminal */
    duplicar(p[1], SAIDA);                      /* a saída do filho vai ser o pipe */
    char *arg1[] = { "gerador", num, NULL };
    int filho = executar("/testes/gerador", arg1);
    duplicar(guarda, SAIDA);                    /* o terminal volta */
    fechar(guarda);
    fechar(p[1]);                               /* sem isso o fim do arquivo nunca chegaria */
    verifica(filho > 0, "executa o gerador com a saída no pipe");
    char buf[128];
    int n, total = 0, erros = 0, leituras = 0;
    while ((n = ler(p[0], buf, sizeof buf)) > 0) {
        for (int i = 0; i < n; i++)
            if (buf[i] != byte_do_padrao((unsigned)(total + i))) erros++;
        total += n;
        leituras++;
    }
    fechar(p[0]);
    int codigo = -1;
    verifica(esperar(filho, &codigo) == filho && codigo == 0, "o gerador terminou bem");
    imprimir("  recebidos %d bytes em %d leituras, %d diferentes\n", total, leituras, erros);
    verifica(total == TOTAL && erros == 0, "todos os bytes chegaram, sem perda nem duplicação");
    verifica(n == 0, "fim do arquivo quando o escritor fechou");

    /* 2. Este processo escreve, o filho lê (o escritor bloqueia com o pipe cheio). */
    verifica(criar_pipe(p) == 0, "cria outro pipe");
    guarda = duplicar(ENTRADA, 10);
    duplicar(p[0], ENTRADA);
    char *arg2[] = { "consumidor", num, NULL };
    filho = executar("/testes/consumidor", arg2);
    duplicar(guarda, ENTRADA);
    fechar(guarda);
    fechar(p[0]);
    unsigned inicio = tempo_ms();
    for (int feito = 0; feito < TOTAL;) {
        int k = 0;
        while (k < (int)sizeof buf && feito + k < TOTAL) { buf[k] = byte_do_padrao((unsigned)(feito + k)); k++; }
        if (escrever_tudo(p[1], buf, (unsigned)k) != k) break;
        feito += k;
    }
    fechar(p[1]);
    verifica(esperar(filho, &codigo) == filho && codigo == 0, "o consumidor recebeu tudo certo");
    imprimir("  levou %u ms\n", tempo_ms() - inicio);

    /* 3. Escrever num pipe sem leitor é erro. */
    criar_pipe(p);
    fechar(p[0]);
    verifica(escrever(p[1], "x", 1) == ERRO_PIPE_QUEBRADO, "escrever sem ninguém lendo dá 'pipe quebrado'");
    fechar(p[1]);
    return resultado("pipe");
}
