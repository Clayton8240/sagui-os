/* console.c - a saída de texto do kernel: kprintf, klog e o console.
 *
 * O console tem dois motores: o modo texto da placa VGA (fases 1 a 3, e
 * reserva se o modo gráfico não ligar) e o terminal gráfico desenhado com a
 * fonte própria (fase 4 em diante). Quem escreve não precisa saber qual. */
#include "console.h"

void textovga_iniciar(void);
void textovga_limpar(void);
void textovga_cor(uint8_t frente, uint8_t fundo);
void textovga_char(char c);

bool termgraf_iniciar(void);
void termgraf_limpar(void);
void termgraf_cor(uint8_t frente, uint8_t fundo);
void termgraf_char(char c);
void termgraf_mostrar(void);

static bool grafico;

void console_iniciar(void)
{
    textovga_iniciar();
}

/* Chamado depois que a memória está pronta (o terminal gráfico precisa do
 * kmalloc para o buffer de trás). Devolve se o modo gráfico ligou. */
bool console_grafico(void)
{
    grafico = termgraf_iniciar();
    return grafico;
}

bool console_em_grafico(void) { return grafico; }

void console_limpar(void)
{
    if (grafico) termgraf_limpar(); else textovga_limpar();
}

void console_cor(uint8_t frente, uint8_t fundo)
{
    if (grafico) termgraf_cor(frente, fundo); else textovga_cor(frente, fundo);
}

void console_char(char c)
{
    if (grafico) termgraf_char(c); else textovga_char(c);
}

/* No modo gráfico o desenho fica no buffer de trás até isto ser chamado. */
void console_mostrar(void)
{
    if (grafico) termgraf_mostrar();
}

void kprintf(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    formatar(buf, sizeof buf, fmt, ap);
    va_end(ap);
    for (char *p = buf; *p; p++) {
        console_char(*p);
        serial_char(*p);
    }
    console_mostrar();
}

void klog(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    formatar(buf, sizeof buf, fmt, ap);
    va_end(ap);
    serial_texto(buf);
}
