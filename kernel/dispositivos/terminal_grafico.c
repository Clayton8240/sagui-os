/* terminal_grafico.c - terminal desenhado no modo gráfico (H18). Provisório na H17. */
#include "console.h"
#include "video.h"

bool termgraf_iniciar(void) { return false; }
void termgraf_limpar(void) { }
void termgraf_cor(uint8_t f, uint8_t b) { (void)f; (void)b; }
void termgraf_char(char c) { (void)c; }
void termgraf_mostrar(void) { }
