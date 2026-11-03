/* caminho.h - montar e simplificar caminhos ("/", ".", ".."). */
#ifndef SAGUI_CAMINHO_H
#define SAGUI_CAMINHO_H
#include "tipos.h"

#define CAMINHO_MAX 128

/* Resolve 'alvo' a partir da pasta 'atual' (absoluta) e grava em 'saida' um
 * caminho absoluto simplificado, sem "." nem "..", sem barra no fim (exceto
 * a raiz "/"). Devolve false se não couber. */
bool caminho_resolver(const char *atual, const char *alvo, char *saida, size_t max);

#endif
