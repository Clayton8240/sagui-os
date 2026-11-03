/* principal.c - ponto de entrada em C do kernel do SaguiOS.
 *
 * Confere que o carregador fez o trabalho dele (assinatura e boot_info),
 * mostra a saudação e a memória, e liga as partes do kernel em ordem:
 * interrupções e relógio, memória física e alocador, teclado, sistema de
 * arquivos e, por fim, o terminal. Tudo cabe em poucas linhas na tela: no
 * G470 o mapa de memória grande empurrava a saudação para fora (erro 1). */
#include "bootinfo.h"
#include "console.h"
#include "texto.h"
#include "cpu.h"
#include "relogio.h"
#include "autoteste.h"
#include "memoria.h"
#include "teclado.h"
#include "sgfs.h"
#include "terminal.h"

#define SAGUI_VERSAO "0.2 (fase 2)"

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
    klog("Ramdisk SGFS: %u bytes em %p, %u entradas\n", info_boot.ramdisk_bytes, rd, entradas);
}

/* Uma linha de status da inicialização: "[ ok ] texto". */
static void etapa(const char *texto)
{
    vga_cor(COR_VERDE_CLARO, COR_PRETO);
    kprintf("[ ok ] ");
    vga_cor(COR_CINZA, COR_PRETO);
    kprintf("%s\n", texto);
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
    info_boot.teste[15] = 0;
    if (info_boot.mapa_qtd > MAPA_MAX)
        info_boot.mapa_qtd = MAPA_MAX;
    memcpy(mapa_memoria, (const void *)info_boot.mapa_end, info_boot.mapa_qtd * sizeof(regiao_e820_t));

    vga_cor(COR_AMARELO, COR_VERDE);
    kprintf("                                                                                ");
    kprintf("  %-56s%20s  ", "Ola! Eu sou o SaguiOS.", "versão " SAGUI_VERSAO);   /* exatamente 80 colunas */
    kprintf("                                                                                ");
    vga_cor(COR_CINZA, COR_PRETO);

    uint64_t livre = memoria_livre(false);
    uint64_t usavel = memoria_livre(true);
    vga_cor(COR_BRANCO, COR_PRETO);
    kprintf("Memoria detectada: %u MB livres", (uint32_t)(livre >> 20));
    vga_cor(COR_CINZA, COR_PRETO);
    kprintf(" | usável (abaixo de 4 GB): %u MB\n", (uint32_t)(usavel >> 20));
    registrar_mapa();
    static const char *metodos_a20[] = { "ja ligada", "BIOS", "porta 0x92", "teclado" };
    kprintf("E820: %u regioes | A20: %s | disco 0x%x | VESA: ", info_boot.mapa_qtd,
            info_boot.a20_metodo < 4 ? metodos_a20[info_boot.a20_metodo] : "?", info_boot.disco);
    if (info_boot.vesa_ok)
        kprintf("%ux%ux%u\n", info_boot.vesa_largura, info_boot.vesa_altura, info_boot.vesa_bpp);
    else
        kprintf("nenhum\n");
    klog("[SAGUI] memoria_mb=%u usavel_mb=%u\n", (uint32_t)(livre >> 20), (uint32_t)(usavel >> 20));

    /* H05: segmentos próprios, interrupções e relógio. */
    gdt_iniciar();
    idt_iniciar();
    pic_iniciar();
    relogio_iniciar();
    int_liga();
    etapa("Interrupções: GDT e IDT próprias, PIC remapeado, relógio a 100 Hz");

    /* H06: páginas físicas e alocador do kernel. */
    memfisica_iniciar();
    {
        char linha[96];
        snformatar(linha, sizeof linha, "Memória física: %u MB gerenciados em páginas de 4 KB, kmalloc pronto",
                   memfisica_livres() / 256);
        etapa(linha);
    }

    /* H07: teclado. */
    teclado_iniciar();
    etapa("Teclado PS/2 com layout ABNT2 (acentos com teclas mortas, ç)");

    /* H08: sistema de arquivos do ramdisk. */
    conferir_ramdisk();
    if (sgfs_iniciar((const void *)info_boot.ramdisk_inicio, info_boot.ramdisk_bytes)) {
        char linha[96];
        snformatar(linha, sizeof linha, "Arquivos: ramdisk SGFS com %u entradas (%u KB)",
                   sgfs_quantidade(), (info_boot.ramdisk_bytes + 1023) / 1024);
        etapa(linha);
    } else {
        vga_cor(COR_VERMELHO_CLARO, COR_PRETO);
        kprintf("[ERRO] ramdisk SGFS invalido: o sistema segue sem arquivos\n");
        vga_cor(COR_CINZA, COR_PRETO);
    }
    klog("[SAGUI] boot ok\n");

    if (info_boot.teste[0])
        autoteste_executar(info_boot.teste);

    /* H09: terminal provisório (vira o programa sagsh na H16). */
    terminal_executar();
}
