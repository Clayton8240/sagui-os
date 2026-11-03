/* autoteste.c - testes que o kernel executa sobre si mesmo.
 *
 * A imagem normal nunca roda nada daqui. Os testes automáticos (tests/)
 * geram uma imagem com o nome de um teste gravado no cabeçalho de boot
 * (tools/mkimagem.py --teste NOME); o kernel o recebe na boot_info e chama
 * autoteste_executar(). O resultado vai para a serial, que o teste lê:
 * "[TESTE] <nome> ok" ou "[TESTE] <nome> FALHOU: motivo". */
#include "autoteste.h"
#include "console.h"
#include "cpu.h"
#include "relogio.h"
#include "texto.h"

static int falhas;

static void confere(bool cond, const char *o_que)
{
    if (!cond) {
        falhas++;
        klog("[TESTE] falha: %s\n", o_que);
    }
}

/* H05: o kernel provoca uma divisão por zero; a tela de erro tem que aparecer. */
static void teste_divisao(void)
{
    kprintf("Autoteste: vou dividir por zero de propósito...\n");
    volatile int zero = 0;
    volatile int r = 10 / zero;
    (void)r;
    klog("[TESTE] divisao FALHOU: a CPU nao gerou a excecao\n");
}

/* H05: o contador de tempo avança, e na velocidade certa (comparado ao CMOS). */
static void teste_relogio(void)
{
    uint32_t t0 = relogio_tiques();
    data_hora_t d0, d1;
    cmos_ler_data(&d0);
    for (int i = 1; i <= 3; i++) {
        relogio_dormir_ms(1000);
        klog("[TESTE] segundos desde o boot: %u (tiques %u)\n", relogio_segundos(), relogio_tiques());
    }
    cmos_ler_data(&d1);
    uint32_t dt = relogio_tiques() - t0;
    confere(dt >= 300 && dt <= 310, "3 segundos devem dar ~300 tiques");
    int s0 = d0.minuto * 60 + d0.segundo, s1 = d1.minuto * 60 + d1.segundo;
    int passou = (s1 - s0 + 3600) % 3600;
    klog("[TESTE] CMOS andou %d s enquanto o PIT contou %u tiques\n", passou, dt);
    confere(passou >= 2 && passou <= 4, "o relógio CMOS deve andar ~3 s");
}

typedef struct {
    const char *nome;
    void (*funcao)(void);
} autoteste_t;

static const autoteste_t testes[] = {
    { "divisao", teste_divisao },
    { "relogio", teste_relogio },
};

void autoteste_executar(const char *nome)
{
    for (unsigned i = 0; i < sizeof testes / sizeof testes[0]; i++) {
        if (txt_compara(nome, testes[i].nome) == 0) {
            klog("[TESTE] inicio %s\n", nome);
            falhas = 0;
            testes[i].funcao();
            if (falhas)
                klog("[TESTE] %s FALHOU (%d)\n", nome, falhas);
            else
                klog("[TESTE] %s ok\n", nome);
            return;
        }
    }
    klog("[TESTE] desconhecido: %s\n", nome);
}
