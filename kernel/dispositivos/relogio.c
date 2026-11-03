/* relogio.c - o PIT (temporizador programável) e o relógio CMOS.
 *
 * PIT: o canal 0 recebe um relógio fixo de 1.193.182 Hz e conta de N até 0,
 * gerando a IRQ 0 a cada volta. Com N = 11932 temos ~100 interrupções por
 * segundo, a "batida" do sistema: conta o tempo desde o boot e, na fase 3,
 * é quem tira a CPU de um processo para dar a outro.
 *
 * CMOS: o chip de relógio da placa-mãe guarda data e hora mesmo desligado.
 * Os registradores são lidos pelas portas 0x70 (índice) e 0x71 (dado). */
#include "relogio.h"
#include "cpu.h"
#include "portas.h"

#define PIT_FREQ   1193182u
#define PIT_CANAL0 0x40
#define PIT_CMD    0x43

static volatile uint32_t tiques;

static quadro_t *ao_tique(quadro_t *q)
{
    tiques++;
    return q;
}

void relogio_iniciar(void)
{
    uint32_t divisor = (PIT_FREQ + TIQUES_POR_SEGUNDO / 2) / TIQUES_POR_SEGUNDO;
    porta_escreve8(PIT_CMD, 0x34);              /* canal 0, byte baixo e alto, modo 2 (gerador de taxa) */
    porta_escreve8(PIT_CANAL0, divisor & 0xFF);
    porta_escreve8(PIT_CANAL0, divisor >> 8);
    irq_registrar(0, ao_tique);
}

uint32_t relogio_tiques(void)   { return tiques; }
uint32_t relogio_segundos(void) { return tiques / TIQUES_POR_SEGUNDO; }

void relogio_dormir_ms(uint32_t ms)
{
    uint32_t alvo = tiques + (ms * TIQUES_POR_SEGUNDO + 999) / 1000;
    while ((int32_t)(alvo - tiques) > 0)
        cpu_espera();   /* dorme até a próxima interrupção */
}

/* ------------------------------------------------------------ CMOS */

static uint8_t cmos_le(uint8_t reg)
{
    porta_escreve8(0x70, reg | 0x80);   /* bit 7 = mantém a NMI desligada durante o acesso */
    return porta_le8(0x71);
}

static bool cmos_atualizando(void)
{
    return cmos_le(0x0A) & 0x80;
}

static uint8_t de_bcd(uint8_t v)
{
    return (uint8_t)((v >> 4) * 10 + (v & 0x0F));
}

static void ler_bruto(uint8_t r[7])
{
    while (cmos_atualizando())
        ;
    r[0] = cmos_le(0x00); r[1] = cmos_le(0x02); r[2] = cmos_le(0x04);
    r[3] = cmos_le(0x07); r[4] = cmos_le(0x08); r[5] = cmos_le(0x09);
    r[6] = cmos_le(0x32);   /* século, quando a placa guarda (ACPI) */
}

void cmos_ler_data(data_hora_t *d)
{
    /* O chip atualiza os registradores uma vez por segundo; uma leitura no
     * meio disso pode misturar valores. Lemos até duas leituras seguidas
     * darem o mesmo resultado. */
    uint8_t a[7], b[7];
    ler_bruto(b);
    do {
        for (int i = 0; i < 7; i++) a[i] = b[i];
        ler_bruto(b);
    } while (a[0] != b[0] || a[1] != b[1] || a[2] != b[2] || a[3] != b[3] || a[4] != b[4] || a[5] != b[5]);

    uint8_t status_b = cmos_le(0x0B);
    bool bcd = !(status_b & 0x04);
    bool h12 = !(status_b & 0x02);
    uint8_t pm = b[2] & 0x80;
    b[2] &= 0x7F;
    if (bcd)
        for (int i = 0; i < 7; i++) b[i] = de_bcd(b[i]);
    if (h12) {
        if (pm && b[2] < 12) b[2] += 12;
        if (!pm && b[2] == 12) b[2] = 0;
    }
    d->segundo = b[0]; d->minuto = b[1]; d->hora = b[2];
    d->dia = b[3]; d->mes = b[4];
    uint32_t seculo = (b[6] >= 19 && b[6] <= 21) ? b[6] : 20;
    d->ano = (uint16_t)(seculo * 100 + b[5]);
}
