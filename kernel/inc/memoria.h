/* memoria.h - páginas físicas e o alocador do kernel. */
#ifndef SAGUI_MEMORIA_H
#define SAGUI_MEMORIA_H
#include "tipos.h"

#define PAGINA        4096u
#define MEMFIS_LIMITE 0x38000000u   /* ver docs/decisoes.md, D008 */

void     memfisica_iniciar(void);
uint32_t memfisica_alocar(void);              /* 0 se acabou */
uint32_t memfisica_alocar_seguidas(uint32_t n);
void     memfisica_liberar(uint32_t endereco);
void     memfisica_liberar_seguidas(uint32_t endereco, uint32_t n);
uint32_t memfisica_livres(void);
uint32_t memfisica_geridas(void);

typedef struct {
    uint64_t ram_total;        /* toda a RAM livre segundo o E820 (inclusive acima de 4 GB) */
    uint32_t paginas_geridas;  /* páginas que o bitmap pode entregar */
    uint32_t paginas_livres;
    uint32_t heap_paginas;     /* páginas usadas pelas classes pequenas do kmalloc */
    uint32_t heap_blocos_usados;
    uint32_t heap_grandes;     /* blocos grandes em uso */
} estat_memoria_t;

void memoria_estatisticas(estat_memoria_t *e);

void *kmalloc(size_t n);
void *kcalloc(size_t n);
void  kfree(void *p);

#endif
