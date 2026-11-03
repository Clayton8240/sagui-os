/* texto.h - funções de memória e de texto escritas para o kernel. */
#ifndef SAGUI_TEXTO_H
#define SAGUI_TEXTO_H
#include "tipos.h"

/* O GCC pode gerar chamadas a memcpy/memset/memmove/memcmp por conta própria
 * (cópia de estruturas, por exemplo), então esses quatro nomes precisam existir. */
void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int v, size_t n);
int   memcmp(const void *a, const void *b, size_t n);

size_t txt_tamanho(const char *s);
int    txt_compara(const char *a, const char *b);
int    txt_compara_n(const char *a, const char *b, size_t n);
char  *txt_copia_n(char *dst, const char *src, size_t n);
bool   txt_comeca_com(const char *s, const char *prefixo);

#endif
