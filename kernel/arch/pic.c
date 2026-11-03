/* pic.c - os dois controladores de interrupção 8259 (mestre e escravo).
 *
 * Depois do boot, o PIC entrega as IRQ 0-7 nos vetores 8-15, que no modo
 * protegido são exceções da CPU (8 é "falta dupla"!). Por isso o primeiro
 * trabalho é reprogramá-los: IRQ 0-7 em 0x20-0x27 e IRQ 8-15 em 0x28-0x2F.
 * Começam todas mascaradas; cada driver habilita a sua com pic_habilitar(). */
#include "cpu.h"
#include "portas.h"

#define PIC1_CMD  0x20
#define PIC1_DADO 0x21
#define PIC2_CMD  0xA0
#define PIC2_DADO 0xA1

static void escreve(uint16_t porta, uint8_t v)
{
    porta_escreve8(porta, v);
    porta_pausa();   /* os 8259 antigos precisavam de um tempo entre comandos */
}

void pic_iniciar(void)
{
    /* Sequência de inicialização (ICW1 a ICW4) definida pela folha de dados do 8259. */
    escreve(PIC1_CMD, 0x11);          /* ICW1: vai ter ICW4, modo cascata */
    escreve(PIC2_CMD, 0x11);
    escreve(PIC1_DADO, VETOR_IRQ0);   /* ICW2: vetor base */
    escreve(PIC2_DADO, VETOR_IRQ0 + 8);
    escreve(PIC1_DADO, 0x04);         /* ICW3: escravo ligado na linha 2 do mestre */
    escreve(PIC2_DADO, 0x02);         /*        identidade do escravo = 2 */
    escreve(PIC1_DADO, 0x01);         /* ICW4: modo 8086 */
    escreve(PIC2_DADO, 0x01);
    escreve(PIC1_DADO, 0xFB);         /* tudo mascarado, menos a linha 2 (cascata) */
    escreve(PIC2_DADO, 0xFF);
}

void pic_habilitar(uint8_t irq)
{
    uint16_t porta = irq < 8 ? PIC1_DADO : PIC2_DADO;
    porta_escreve8(porta, porta_le8(porta) & ~(1 << (irq & 7)));
}

void pic_desabilitar(uint8_t irq)
{
    uint16_t porta = irq < 8 ? PIC1_DADO : PIC2_DADO;
    porta_escreve8(porta, porta_le8(porta) | (1 << (irq & 7)));
}

void pic_fim(uint8_t irq)
{
    if (irq >= 8)
        porta_escreve8(PIC2_CMD, 0x20);
    porta_escreve8(PIC1_CMD, 0x20);
}

/* IRQ 7 e 15 podem chegar "de mentira" (ruído na linha). Lendo o registrador
 * de serviço (ISR) dá para saber: se o bit não está ligado, é espúria e não
 * leva EOI (no escravo, o mestre ainda precisa do EOI da linha 2). */
bool pic_espuria(uint8_t irq)
{
    if (irq != 7 && irq != 15)
        return false;
    uint16_t cmd = irq == 7 ? PIC1_CMD : PIC2_CMD;
    porta_escreve8(cmd, 0x0B);
    if (porta_le8(cmd) & 0x80)
        return false;
    if (irq == 15)
        porta_escreve8(PIC1_CMD, 0x20);
    return true;
}
