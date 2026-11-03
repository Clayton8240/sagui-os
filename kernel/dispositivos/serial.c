/* serial.c - porta serial COM1 (0x3F8), o "log" do kernel.
 *
 * No QEMU a serial vai para um arquivo, que os testes automáticos leem. No
 * G470 não há conector serial; escrever nela não faz mal (os bytes se perdem).
 * Configuração: 38400 bauds, 8 bits, sem paridade, 1 bit de parada. */
#include "console.h"
#include "portas.h"

#define COM1 0x3F8

static bool serial_ok;

void serial_iniciar(void)
{
    porta_escreve8(COM1 + 1, 0x00);   /* sem interrupções da serial */
    porta_escreve8(COM1 + 3, 0x80);   /* DLAB=1: próximos dois bytes são o divisor */
    porta_escreve8(COM1 + 0, 3);      /* 115200 / 3 = 38400 bauds */
    porta_escreve8(COM1 + 1, 0);
    porta_escreve8(COM1 + 3, 0x03);   /* 8N1, DLAB=0 */
    porta_escreve8(COM1 + 2, 0xC7);   /* FIFO ligada e limpa */
    porta_escreve8(COM1 + 4, 0x03);   /* DTR + RTS */
    /* Se não há UART (o registrador de linha volta 0xFF), não esperamos por ela. */
    serial_ok = porta_le8(COM1 + 5) != 0xFF;
}

void serial_char(char c)
{
    if (!serial_ok)
        return;
    if (c == '\n')
        serial_char('\r');
    /* Espera o registrador de transmissão esvaziar, com limite. */
    for (int i = 0; i < 100000 && !(porta_le8(COM1 + 5) & 0x20); i++)
        ;
    porta_escreve8(COM1, (uint8_t)c);
}

void serial_texto(const char *s)
{
    while (*s)
        serial_char(*s++);
}
