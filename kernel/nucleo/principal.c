/* principal.c - ponto de entrada em C do kernel do SaguiOS.
 *
 * Fase 1: provar que o carregador fez o trabalho dele. O kernel confere a
 * assinatura deixada pelo estágio 2, guarda a boot_info, mostra a saudação e
 * a memória detectada, e confere que o ramdisk chegou inteiro. */
#include "bootinfo.h"
#include "console.h"
#include "texto.h"

#define SAGUI_VERSAO "0.1 (fase 1)"

info_boot_t info_boot;
regiao_e820_t mapa_memoria[MAPA_MAX];

static const char *nome_tipo_e820(uint32_t t)
{
    switch (t) {
    case 1: return "livre";
    case 2: return "reservada";
    case 3: return "ACPI (recuperavel)";
    case 4: return "ACPI NVS";
    case 5: return "defeituosa";
    default: return "outra";
    }
}

static void parar_com_erro(const char *msg)
{
    vga_cor(COR_BRANCO, COR_VERMELHO);
    kprintf("\n SaguiOS: %s \n", msg);
    for (;;)
        __asm__ volatile("cli; hlt");
}

/* Soma a RAM livre (tipo 1) do mapa E820. Entradas com o atributo estendido
 * presente mas com o bit 0 desligado devem ser ignoradas (ACPI 3.0).
 * Com abaixo_de_4g, conta só a parte que um kernel de 32 bits sem PAE
 * consegue endereçar. */
static uint64_t memoria_livre(bool abaixo_de_4g)
{
    const uint64_t limite = 0x100000000ull;
    uint64_t total = 0;
    for (uint32_t i = 0; i < info_boot.mapa_qtd; i++) {
        regiao_e820_t *r = &mapa_memoria[i];
        if (r->tipo != E820_LIVRE || !(r->extra & 1))
            continue;
        uint64_t ini = r->base, fim = r->base + r->tamanho;
        if (abaixo_de_4g) {
            if (ini >= limite) continue;
            if (fim > limite) fim = limite;
        }
        total += fim - ini;
    }
    return total;
}

/* O mapa completo vai só para a serial. No G470 ele tem ~20 regiões, e
 * mostrá-lo na tela de 25 linhas empurrava a saudação para fora (erro 1). */
static void registrar_mapa(void)
{
    klog("Mapa de memoria (E820), %u regioes:\n", info_boot.mapa_qtd);
    for (uint32_t i = 0; i < info_boot.mapa_qtd; i++) {
        regiao_e820_t *r = &mapa_memoria[i];
        uint64_t fim = r->base + r->tamanho - 1;
        klog("  %08x%08x - %08x%08x  %s\n",
             (uint32_t)(r->base >> 32), (uint32_t)r->base,
             (uint32_t)(fim >> 32), (uint32_t)fim, nome_tipo_e820(r->tipo));
    }
}

/* Confere o cabeçalho do SGFS (o leitor completo vem na H08). */
static void conferir_ramdisk(void)
{
    const uint8_t *rd = (const uint8_t *)info_boot.ramdisk_inicio;
    if (info_boot.ramdisk_bytes < 32 || memcmp(rd, "SGFS", 4) != 0) {
        kprintf("Ramdisk: NAO encontrado em %p\n", rd);
        return;
    }
    uint32_t entradas = *(const uint32_t *)(rd + 8);
    kprintf("Ramdisk SGFS: %u bytes em %p, %u entradas\n", info_boot.ramdisk_bytes, rd, entradas);
}

void inicio_kernel(uint32_t magico, info_boot_t *bi)
{
    serial_iniciar();
    vga_iniciar();
    klog("\n[SAGUI] kernel iniciado\n");

    if (magico != BOOT_MAGICO || bi->assinatura != BOOTINFO_ASSIN)
        parar_com_erro("nao fui carregado pelo estagio 2 do SaguiOS");

    /* A boot_info e o mapa estão na memória baixa, que mais tarde vira área
     * livre. Copiamos para dentro do kernel antes de qualquer outra coisa. */
    info_boot = *bi;
    if (info_boot.mapa_qtd > MAPA_MAX)
        info_boot.mapa_qtd = MAPA_MAX;
    memcpy(mapa_memoria, (const void *)info_boot.mapa_end, info_boot.mapa_qtd * sizeof(regiao_e820_t));

    vga_cor(COR_AMARELO, COR_VERDE);
    kprintf("                                                                                ");
    kprintf("  Ola! Eu sou o SaguiOS.                                                        ");
    kprintf("                                                                                ");
    vga_cor(COR_CINZA, COR_PRETO);
    kprintf("\nVersao %s - kernel em %p (%u bytes)\n", SAGUI_VERSAO,
            (void *)info_boot.kernel_inicio, info_boot.kernel_bytes);

    uint64_t livre = memoria_livre(false);
    uint64_t usavel = memoria_livre(true);
    vga_cor(COR_BRANCO, COR_PRETO);
    kprintf("Memoria detectada: %u MB livres\n", (uint32_t)(livre >> 20));
    vga_cor(COR_CINZA, COR_PRETO);
    kprintf("  utilizavel pelo SaguiOS (32 bits, abaixo de 4 GB): %u MB\n", (uint32_t)(usavel >> 20));
    kprintf("  mapa E820 com %u regioes (lista completa no log serial)\n", info_boot.mapa_qtd);
    registrar_mapa();

    static const char *metodos_a20[] = { "ja estava ligada", "BIOS", "porta 0x92", "controlador de teclado" };
    kprintf("Linha A20: %s\n", info_boot.a20_metodo < 4 ? metodos_a20[info_boot.a20_metodo] : "?");
    kprintf("Disco de boot: 0x%x, %u setores lidos\n", info_boot.disco, info_boot.setores_lidos);
    if (info_boot.vesa_ok)
        kprintf("Video VESA disponivel: %ux%u, %u bits (modo 0x%x, ainda em modo texto)\n",
                info_boot.vesa_largura, info_boot.vesa_altura, info_boot.vesa_bpp, info_boot.vesa_modo);
    else
        kprintf("Video VESA: nenhum modo aceitavel encontrado\n");
    conferir_ramdisk();

    vga_cor(COR_VERDE_CLARO, COR_PRETO);
    kprintf("\nFase 1 concluida: o kernel esta no controle da maquina.\n");
    vga_cor(COR_CINZA, COR_PRETO);
    klog("[SAGUI] memoria_mb=%u usavel_mb=%u\n", (uint32_t)(livre >> 20), (uint32_t)(usavel >> 20));
    klog("[SAGUI] boot ok\n");

    for (;;)
        __asm__ volatile("hlt");
}
