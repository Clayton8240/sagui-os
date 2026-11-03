/* vga_texto.c - console em modo texto VGA (80x25), usado nas fases 1 a 3.
 *
 * A placa mostra o conteúdo de 0xB8000: cada célula tem 2 bytes, o caractere
 * e o atributo (cor de frente nos 4 bits baixos, fundo nos 4 altos). O
 * cursor piscante é posicionado pelos registradores 0x0E/0x0F do controlador
 * CRT, acessados pelas portas 0x3D4 (índice) e 0x3D5 (dado). */
#include "console.h"
#include "portas.h"
#include "texto.h"

#define COLUNAS 80
#define LINHAS  25

/* O texto do SaguiOS é Latin-1, mas a fonte gravada na placa VGA segue a
 * página de código 437, que tem outra numeração e não tem todas as letras
 * (faltam ã, õ e as maiúsculas acentuadas, exceto Ç, É e poucas outras).
 * Esta tabela leva cada caractere Latin-1 de 0xA0 a 0xFF ao equivalente da
 * placa ou, se não houver, à letra sem acento. Na fase 4 a fonte própria
 * resolve isso de vez. */
static const uint8_t latin1_para_placa[96] = {
    0xFF, 0xAD, 0x9B, 0x9C, '*',  0x9D, '|',  0x15, '"',  'c',  0xA6, 0xAE, 0xAA, '-',  'r',  '-',
    0xF8, 0xF1, 0xFD, '3',  '\'', 0xE6, 0x14, 0xFA, ',',  '1',  0xA7, 0xAF, 0xAC, 0xAB, '3',  0xA8,
    'A',  'A',  'A',  'A',  0x8E, 0x8F, 0x92, 0x80, 'E',  0x90, 'E',  'E',  'I',  'I',  'I',  'I',
    'D',  0xA5, 'O',  'O',  'O',  'O',  0x99, 'x',  'O',  'U',  'U',  'U',  0x9A, 'Y',  'P',  0xE1,
    0x85, 0xA0, 0x83, 'a',  0x84, 0x86, 0x91, 0x87, 0x8A, 0x82, 0x88, 0x89, 0x8D, 0xA1, 0x8C, 0x8B,
    'd',  0xA4, 0x95, 0xA2, 0x93, 'o',  0x94, 0xF6, 'o',  0x97, 0xA3, 0x96, 0x81, 'y',  'p',  0x98,
};

static volatile uint16_t *const tela = (volatile uint16_t *)0xB8000;
static int col, lin;
static uint8_t atributo = (COR_PRETO << 4) | COR_CINZA;

static void mover_cursor(void)
{
    uint16_t pos = (uint16_t)(lin * COLUNAS + col);
    porta_escreve8(0x3D4, 0x0F);
    porta_escreve8(0x3D5, pos & 0xFF);
    porta_escreve8(0x3D4, 0x0E);
    porta_escreve8(0x3D5, pos >> 8);
}

void vga_cor(uint8_t frente, uint8_t fundo)
{
    atributo = (uint8_t)((fundo << 4) | (frente & 0x0F));
}

void vga_limpar(void)
{
    for (int i = 0; i < COLUNAS * LINHAS; i++)
        tela[i] = (uint16_t)(atributo << 8) | ' ';
    col = lin = 0;
    mover_cursor();
}

void vga_iniciar(void)
{
    vga_cor(COR_CINZA, COR_PRETO);
    vga_limpar();
}

/* Sobe tudo uma linha e limpa a última. */
static void rolar(void)
{
    for (int i = 0; i < COLUNAS * (LINHAS - 1); i++)
        tela[i] = tela[i + COLUNAS];
    for (int i = COLUNAS * (LINHAS - 1); i < COLUNAS * LINHAS; i++)
        tela[i] = (uint16_t)(atributo << 8) | ' ';
    lin = LINHAS - 1;
}

void vga_char(char c)
{
    switch (c) {
    case '\n': col = 0; lin++; break;
    case '\r': col = 0; break;
    case '\t': col = (col + 8) & ~7; break;
    case '\b': if (col > 0) { col--; tela[lin * COLUNAS + col] = (uint16_t)(atributo << 8) | ' '; } break;
    default: {
        uint8_t b = (uint8_t)c;
        if (b >= 0xA0)
            b = latin1_para_placa[b - 0xA0];
        tela[lin * COLUNAS + col] = (uint16_t)(atributo << 8) | b;
    }
        col++;
    }
    if (col >= COLUNAS) { col = 0; lin++; }
    if (lin >= LINHAS) rolar();
    mover_cursor();
}
