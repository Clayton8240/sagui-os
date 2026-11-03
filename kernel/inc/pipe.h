/* pipe.h - canos entre processos (H15). */
#ifndef SAGUI_PIPE_H
#define SAGUI_PIPE_H
#include "tipos.h"

#define PIPE_TAM 512   /* pequeno de propósito: o teste passa muito mais que isso */

typedef struct pipe {
    uint8_t  buf[PIPE_TAM];
    uint32_t ini, qtd;
    int      leitores, escritores;
} pipe_t;

pipe_t *pipe_criar(void);
int32_t pipe_ler(pipe_t *p, char *destino, uint32_t n);
int32_t pipe_escrever(pipe_t *p, const char *origem, uint32_t n);
void    pipe_fechar_ponta(pipe_t *p, bool escrita);

#endif
