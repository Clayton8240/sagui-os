/* pci.c - o barramento PCI (H29).
 *
 * Cada função de cada dispositivo tem 256 bytes de configuração: quem fez,
 * que tipo de coisa é (classe, subclasse, interface) e onde ficam os seus
 * registradores (BARs). O acesso é pelo "mecanismo 1" da especificação PCI:
 * escreve-se o endereço (barramento, dispositivo, função, registrador) na
 * porta 0xCF8 e lê-se o valor na 0xCFC. */
#include "pci.h"
#include "portas.h"

static uint32_t endereco(uint8_t b, uint8_t d, uint8_t f, uint8_t reg)
{
    return 0x80000000u | ((uint32_t)b << 16) | ((uint32_t)d << 11) | ((uint32_t)f << 8) | (reg & 0xFC);
}

static uint32_t ler(uint8_t b, uint8_t d, uint8_t f, uint8_t reg)
{
    porta_escreve32(0xCF8, endereco(b, d, f, reg));
    return porta_le32(0xCFC);
}

uint32_t pci_le32(const pci_disp_t *d, uint8_t reg)
{
    return ler(d->barramento, d->dispositivo, d->funcao, reg);
}

void pci_escreve32(const pci_disp_t *d, uint8_t reg, uint32_t v)
{
    porta_escreve32(0xCF8, endereco(d->barramento, d->dispositivo, d->funcao, reg));
    porta_escreve32(0xCFC, v);
}

int pci_procurar(uint8_t classe, uint8_t subclasse, uint8_t interface, pci_disp_t *achados, int max)
{
    int n = 0;
    for (int b = 0; b < 256; b++)
        for (int d = 0; d < 32; d++) {
            uint32_t id = ler((uint8_t)b, (uint8_t)d, 0, 0);
            if ((id & 0xFFFF) == 0xFFFF) continue;                    /* ninguém aqui */
            bool multifuncao = (ler((uint8_t)b, (uint8_t)d, 0, 0x0C) >> 16) & 0x80;
            for (int f = 0; f < (multifuncao ? 8 : 1); f++) {
                uint32_t idf = ler((uint8_t)b, (uint8_t)d, (uint8_t)f, 0);
                if ((idf & 0xFFFF) == 0xFFFF) continue;
                uint32_t cl = ler((uint8_t)b, (uint8_t)d, (uint8_t)f, 0x08);
                if ((cl >> 24) == classe && ((cl >> 16) & 0xFF) == subclasse && ((cl >> 8) & 0xFF) == interface && n < max)
                    achados[n++] = (pci_disp_t){ (uint8_t)b, (uint8_t)d, (uint8_t)f, (uint16_t)idf, (uint16_t)(idf >> 16),
                                                 classe, subclasse, interface };
            }
        }
    return n;
}
