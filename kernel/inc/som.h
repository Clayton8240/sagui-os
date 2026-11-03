/* som.h - o alto-falante do PC (bônus B3). */
#ifndef SAGUI_SOM_H
#define SAGUI_SOM_H
#include "tipos.h"
#include "abi.h"

void som_tique(void);   /* chamado a cada tique do relógio: avança a fila de notas */
void som_parar(void);
int32_t sis_bip(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#endif
