/* vfs.c - camada de arquivos virtual. (Provisória: só leitura do SGFS, até a H14.) */
#include "vfs.h"
#include "memoria.h"
#include "sgfs.h"

void vfs_iniciar(void) { }

int vfs_ler_inteiro(const char *caminho, uint8_t **dados, uint32_t *tam)
{
    const sgfs_entrada_t *e = sgfs_procurar(caminho);
    if (!e) return ERRO_NAO_EXISTE;
    if (e->tipo == SGFS_PASTA) return ERRO_E_PASTA;
    uint8_t *buf = kmalloc(e->tamanho ? e->tamanho : 1);
    if (!buf) return ERRO_SEM_MEMORIA;
    *tam = sgfs_ler(e, 0, buf, e->tamanho);
    *dados = buf;
    return 0;
}
