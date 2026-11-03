/* console.h - saída de texto do kernel (tela + espelho na serial). */
#ifndef SAGUI_CONSOLE_H
#define SAGUI_CONSOLE_H
#include "tipos.h"

/* Cores do modo texto VGA (4 bits). */
enum {
    COR_PRETO, COR_AZUL, COR_VERDE, COR_CIANO, COR_VERMELHO, COR_MAGENTA, COR_MARROM, COR_CINZA,
    COR_CINZA_ESCURO, COR_AZUL_CLARO, COR_VERDE_CLARO, COR_CIANO_CLARO, COR_VERMELHO_CLARO,
    COR_MAGENTA_CLARO, COR_AMARELO, COR_BRANCO
};

void vga_iniciar(void);
void vga_limpar(void);
void vga_cor(uint8_t frente, uint8_t fundo);
void vga_char(char c);

void serial_iniciar(void);
void serial_char(char c);
void serial_texto(const char *s);

/* kprintf escreve na tela e na serial; klog só na serial (log de testes). */
void kprintf(const char *fmt, ...);
void klog(const char *fmt, ...);
int  formatar(char *dst, size_t max, const char *fmt, va_list ap);
int  snformatar(char *dst, size_t max, const char *fmt, ...);

#endif
