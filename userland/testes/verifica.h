/* verifica.h - pequeno apoio para os programas de teste. */
#ifndef VERIFICA_H
#define VERIFICA_H
#include "sagui.h"

static int falhas_teste;

static void verifica(bool cond, const char *o_que)
{
    if (cond) imprimir("  [ok] %s\n", o_que);
    else { imprimir("  [FALHA] %s\n", o_que); falhas_teste++; }
}

static int resultado(const char *nome)
{
    imprimir("RESULTADO %s: %s (%d falhas)\n", nome, falhas_teste ? "FALHOU" : "SUCESSO", falhas_teste);
    return falhas_teste;
}
#endif
