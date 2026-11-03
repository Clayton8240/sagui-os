/* portas.h - acesso às portas de E/S do x86 (instruções in/out). */
#ifndef SAGUI_PORTAS_H
#define SAGUI_PORTAS_H
#include "tipos.h"

static inline void porta_escreve8(uint16_t porta, uint8_t v)
{
    __asm__ volatile("outb %0, %1" : : "a"(v), "Nd"(porta));
}

static inline uint8_t porta_le8(uint16_t porta)
{
    uint8_t v;
    __asm__ volatile("inb %1, %0" : "=a"(v) : "Nd"(porta));
    return v;
}

static inline void porta_escreve16(uint16_t porta, uint16_t v)
{
    __asm__ volatile("outw %0, %1" : : "a"(v), "Nd"(porta));
}

static inline uint16_t porta_le16(uint16_t porta)
{
    uint16_t v;
    __asm__ volatile("inw %1, %0" : "=a"(v) : "Nd"(porta));
    return v;
}

static inline void porta_escreve32(uint16_t porta, uint32_t v)
{
    __asm__ volatile("outl %0, %1" : : "a"(v), "Nd"(porta));
}

static inline uint32_t porta_le32(uint16_t porta)
{
    uint32_t v;
    __asm__ volatile("inl %1, %0" : "=a"(v) : "Nd"(porta));
    return v;
}

/* Pequena pausa: a porta 0x80 é a de diagnóstico do POST, sem efeito. */
static inline void porta_pausa(void)
{
    porta_escreve8(0x80, 0);
}

#endif
