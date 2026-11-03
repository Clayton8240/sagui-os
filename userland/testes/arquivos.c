/* arquivos - teste da H14 (arquivos e pastas graváveis), rodando no anel 3. */
#include "sagui.h"
#include "verifica.h"

static const char texto[] = "Primeira linha do SaguiOS.\nAção, coração e pão.\n";

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    char buf[256];
    imprimir("Teste de arquivos (H14), processo %d\n", meu_pid());

    /* Entrada 0 e saída 1 são o terminal. */
    verifica(escrever(SAIDA, "", 0) == 0, "a saída 1 (terminal) aceita escrita");

    /* Arquivo: criar, escrever, fechar, reabrir, ler, apagar. */
    int fd = abrir("/tmp/teste.txt", ABRIR_ESCREVER | ABRIR_CRIAR);
    verifica(fd >= 3, "cria /tmp/teste.txt (descritor >= 3)");
    verifica(escrever(fd, texto, sizeof texto - 1) == (int)sizeof texto - 1, "escreve o texto");
    verifica(fechar(fd) == 0, "fecha");
    info_arquivo_t inf;
    verifica(info_arquivo("/tmp/teste.txt", &inf) == 0 && inf.tamanho == sizeof texto - 1 && inf.tipo == TIPO_ARQUIVO,
             "info_arquivo mostra o tamanho certo");
    fd = abrir("/tmp/teste.txt", ABRIR_LER);
    int n = ler(fd, buf, sizeof buf);
    verifica(n == (int)sizeof texto - 1 && memcmp(buf, texto, (size_t)n) == 0, "reabre e lê o mesmo conteúdo");
    verifica(ler(fd, buf, sizeof buf) == 0, "depois do fim, ler devolve 0");
    verifica(posicionar(fd, 9, POS_INICIO) == 9 && ler(fd, buf, 5) == 5 && memcmp(buf, "linha", 5) == 0,
             "posicionar e ler do meio");
    fechar(fd);
    verifica(escrever(fd, "x", 1) == ERRO_DESCRITOR, "descritor fechado não serve mais");

    /* Sobrescrever, acrescentar. */
    fd = abrir("/tmp/teste.txt", ABRIR_ESCREVER | ABRIR_TRUNCAR);
    escrever(fd, "novo", 4);
    fechar(fd);
    fd = abrir("/tmp/teste.txt", ABRIR_ACRESCENTAR);
    escrever(fd, "+mais", 5);
    fechar(fd);
    fd = abrir("tmp/../tmp/./teste.txt", ABRIR_LER);   /* caminho relativo com . e .. */
    n = ler(fd, buf, sizeof buf);
    fechar(fd);
    verifica(n == 9 && memcmp(buf, "novo+mais", 9) == 0, "sobrescrever (truncar) e acrescentar");

    verifica(apagar("/tmp/teste.txt") == 0, "apaga o arquivo");
    verifica(abrir("/tmp/teste.txt", ABRIR_LER) == ERRO_NAO_EXISTE, "o arquivo sumiu");

    /* Pasta: criar, usar, não remover cheia, remover vazia. */
    verifica(criar_pasta("/tmp/pasta") == 0, "cria /tmp/pasta");
    verifica(criar_pasta("/tmp/pasta") == ERRO_JA_EXISTE, "criar de novo avisa que já existe");
    fd = abrir("/tmp/pasta/dentro.txt", ABRIR_CRIAR);
    escrever(fd, "oi", 2);
    fechar(fd);
    entrada_pasta_t e;
    verifica(listar("/tmp/pasta", 0, &e) == 1 && compara_texto(e.nome, "dentro.txt") == 0 && e.tamanho == 2,
             "listar mostra o arquivo dentro da pasta");
    verifica(listar("/tmp/pasta", 1, &e) == 0, "e só ele");
    verifica(remover_pasta("/tmp/pasta") == ERRO_NAO_VAZIA, "não remove pasta com arquivos");
    verifica(mudar_pasta("/tmp/pasta") == 0 && pasta_atual(buf, sizeof buf) > 0 && compara_texto(buf, "/tmp/pasta") == 0,
             "mudar_pasta e pasta_atual");
    verifica(abrir("dentro.txt", ABRIR_LER) >= 0, "abre com caminho relativo à pasta atual");
    verifica(remover_pasta("/tmp/pasta") == ERRO_OCUPADO, "não remove a pasta em que o processo está");
    mudar_pasta("/");
    verifica(renomear("/tmp/pasta/dentro.txt", "/tmp/fora.txt") == 0 && info_arquivo("/tmp/fora.txt", &inf) == 0,
             "renomear move o arquivo para outra pasta");
    verifica(remover_pasta("/tmp/pasta") == 0, "remove a pasta vazia");
    verifica(info_arquivo("/tmp/pasta", &inf) == ERRO_NAO_EXISTE, "a pasta sumiu");
    apagar("/tmp/fora.txt");

    /* Arquivo do ramdisk: muda na RAM (cópia na escrita), o original não é tocado. */
    fd = abrir("/sobre.txt", ABRIR_ACRESCENTAR);
    escrever(fd, "FIM-TESTE", 9);
    fechar(fd);
    fd = abrir("/sobre.txt", ABRIR_LER);
    info_arquivo("/sobre.txt", &inf);
    posicionar(fd, -9, POS_FIM);
    n = ler(fd, buf, 9);
    fechar(fd);
    verifica(n == 9 && memcmp(buf, "FIM-TESTE", 9) == 0, "arquivo do ramdisk alterado na camada da RAM");

    /* Erros e proteção. */
    verifica(abrir("/nao/existe.txt", ABRIR_CRIAR) == ERRO_NAO_EXISTE, "criar dentro de pasta inexistente falha");
    verifica(abrir("/bin", ABRIR_LER) == ERRO_E_PASTA, "abrir uma pasta como arquivo falha");
    verifica(ler(0, (void *)0x00100000, 4) == ERRO_ENDERECO, "ler para a memória do kernel é recusado");
    verifica(abrir((const char *)0x00100000, ABRIR_LER) == ERRO_ENDERECO, "caminho na memória do kernel é recusado");
    return resultado("arquivos");
}
