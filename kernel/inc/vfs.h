/* vfs.h - camada de arquivos virtual: um caminho só para tudo (PRD 4.3). */
#ifndef SAGUI_VFS_H
#define SAGUI_VFS_H
#include "tipos.h"
#include "abi.h"

struct processo;

void vfs_iniciar(void);
/* Lê um arquivo inteiro para um buffer novo do kernel (liberar com kfree). */
int  vfs_ler_inteiro(const char *caminho, uint8_t **dados, uint32_t *tam);

void arquivos_herdar(struct processo *filho, struct processo *pai);
void arquivos_fechar_todos(struct processo *p);

#endif
