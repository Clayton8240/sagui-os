/* area_trabalho.c - o fundo, o mascote, os ícones e a barra superior (H20, H28).
 *
 * O fundo não muda: é desenhado uma vez numa superfície guardada (degradê
 * verde, amarelo e azul, o sagui no canto e os ícones), e a cada
 * redesenho só a parte necessária é copiada. A barra superior é desenhada
 * por cima, porque o relógio muda. */
#include "gui.h"
#include "arte.h"
#include "console.h"
#include "memoria.h"
#include "processo.h"
#include "relogio.h"
#include "texto.h"
#include "video.h"

static uint32_t *fundo;
static int larg, alt;
static int minuto_mostrado = -1;

typedef struct {
    const char *nome;
    const char *programa;
    const desenho_t *desenho;
} icone_t;

static const icone_t icones[] = {
    { "Arquivos",    "/bin/arquivos",    &arte_icone_arquivos },
    { "Terminal",    "/bin/terminal",    &arte_icone_terminal },
    { "Editor",      "/bin/editor",      &arte_icone_editor },
    { "Calculadora", "/bin/calculadora", &arte_icone_calculadora },
    { "Banana",      "/bin/banana",      &arte_icone_banana },
    { "Sobre",       "/bin/sobre-o-sistema",   &arte_icone_sobre },
};
#define QTD_ICONES (int)(sizeof icones / sizeof icones[0])
#define ICONE_X 20
#define ICONE_Y0 (BARRA_SUPERIOR + 16)
#define ICONE_PASSO 92
#define ICONE_L 96
#define ICONE_A 88

int area_qtd_icones(void) { return QTD_ICONES; }

void area_icone_retangulo(int i, int *x, int *y, int *l, int *a)
{
    *x = ICONE_X; *y = ICONE_Y0 + i * ICONE_PASSO; *l = ICONE_L; *a = ICONE_A;
}

/* Interpolação de um canal em inteiros. */
static uint32_t mistura(uint32_t a, uint32_t b, int num, int den)
{
    uint32_t r = 0;
    for (int s = 0; s <= 16; s += 8) {
        int ca = (int)((a >> s) & 255), cb = (int)((b >> s) & 255);
        r |= (uint32_t)(ca + (cb - ca) * num / den) << s;
    }
    return r;
}

static void texto_com_sombra(superficie_t *s, int x, int y, const char *t, uint32_t cor)
{
    sup_texto(s, x + 1, y + 1, t, 0x101010);
    sup_texto(s, x, y, t, cor);
}

static void desenhar_fundo(void)
{
    superficie_t s;
    sup_iniciar(&s, fundo, larg, alt);
    /* Degradê vertical nas cores do Brasil: verde em cima, amarelo no meio,
     * azul embaixo, em tons mais suaves que os da bandeira para o texto e os
     * ícones continuarem legíveis. */
    const uint32_t verde = 0x0B7A3E, amarelo = 0xE0B92A, azul = 0x14337E;
    for (int y = 0; y < alt; y++) {
        uint32_t c = y < alt / 2 ? mistura(verde, amarelo, y, alt / 2)
                                 : mistura(amarelo, azul, y - alt / 2, alt - alt / 2);
        uint32_t *p = fundo + y * larg;
        for (int x = 0; x < larg; x++) p[x] = c;
    }
    /* O mascote, grande, no canto de baixo à direita. */
    int esc = larg >= 1000 ? 5 : 3;
    int mx = larg - 32 * esc - 30, my = alt - 32 * esc - 40;
    sup_desenho(&s, mx, my, &arte_sagui, esc);
    texto_com_sombra(&s, mx + 16 * esc - texto_largura("SaguiOS") / 2, my + 32 * esc + 6, "SaguiOS", 0xFFFFFF);
    /* Ícones na coluna da esquerda, com o nome embaixo. */
    for (int i = 0; i < QTD_ICONES; i++) {
        int x, y, l, a;
        area_icone_retangulo(i, &x, &y, &l, &a);
        sup_desenho(&s, x + (l - 64) / 2, y, icones[i].desenho, 2);
        texto_com_sombra(&s, x + (l - texto_largura(icones[i].nome)) / 2, y + 68, icones[i].nome, 0xFFFFFF);
    }
}

bool area_iniciar(void)
{
    larg = (int)video_largura();
    alt = (int)video_altura();
    fundo = kmalloc((uint32_t)(larg * alt) * 4);
    if (!fundo) return false;
    desenhar_fundo();
    return true;
}

void area_fundo(superficie_t *s)
{
    sup_copiar(s, 0, 0, fundo, larg, alt, larg);
}

bool area_relogio_mudou(void)
{
    data_hora_t d;
    cmos_ler_data(&d);
    return d.minuto != minuto_mostrado;
}

void area_barra(superficie_t *s)
{
    static const char *semana[] = { "dom", "seg", "ter", "qua", "qui", "sex", "sáb" };
    sup_retangulo(s, 0, 0, larg, BARRA_SUPERIOR, 0x18202A);
    sup_retangulo(s, 0, BARRA_SUPERIOR - 1, larg, 1, 0x0B7A3E);
    /* Cabecinha do sagui (as linhas 2 a 17 do ícone "sobre") e o nome. */
    desenho_t cabeca = { arte_icone_sobre.linhas + 2, 32, 16 };
    sup_desenho(s, 6, 4, &cabeca, 1);
    sup_texto(s, 44, 4, "SaguiOS", 0xFFDF4A);
    sup_texto(s, 112, 4, "um sistema operacional escrito do zero por IA", 0x9AA8B8);
    data_hora_t d;
    cmos_ler_data(&d);
    minuto_mostrado = d.minuto;
    char hora[48];
    snformatar(hora, sizeof hora, "%s %02u/%02u/%u  %02u:%02u", semana[dia_da_semana(d.ano, d.mes, d.dia)],
               d.dia, d.mes, d.ano, d.hora, d.minuto);
    sup_texto(s, larg - texto_largura(hora) - 10, 4, hora, 0xFFFFFF);
    klog("[GUI] relogio: %s\n", hora);
}

int area_icone_em(int x, int y)
{
    for (int i = 0; i < QTD_ICONES; i++) {
        int ix, iy, l, a;
        area_icone_retangulo(i, &ix, &iy, &l, &a);
        if (x >= ix && x < ix + l && y >= iy && y < iy + a) return i;
    }
    return -1;
}

/* Abre o programa do ícone (H28). Ele não tem pai: ninguém espera por ele. */
void area_icone_abrir(int i)
{
    if (i < 0 || i >= QTD_ICONES) return;
    char *argv[] = { (char *)icones[i].programa + 5, NULL };
    int pid = processo_executar(icones[i].programa, 1, argv, NULL);
    klog("[GUI] icone %s -> %s (pid %d)\n", icones[i].nome, icones[i].programa, pid);
}
