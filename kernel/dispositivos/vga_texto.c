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
    default:
        tela[lin * COLUNAS + col] = (uint16_t)(atributo << 8) | (uint8_t)c;
        col++;
    }
    if (col >= COLUNAS) { col = 0; lin++; }
    if (lin >= LINHAS) rolar();
    mover_cursor();
}
