/* ola - imprime os argumentos recebidos (teste do carregador, H13). */
#include "sagui.h"

int principal(int argc, char **argv)
{
    imprimir("Olá! Sou o programa %s, processo %d, rodando no anel 3.\n", argv[0], meu_pid());
    imprimir("Recebi %d argumento(s):\n", argc);
    for (int i = 0; i < argc; i++)
        imprimir("  argv[%d] = \"%s\"\n", i, argv[i]);
    return 0;
}
