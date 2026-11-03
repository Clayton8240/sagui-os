/* banana - Sagui Pega Banana, o jogo do SaguiOS (H27).
 *
 * O sagui anda na parte de baixo com as setas; bananas caem do alto. Cada
 * banana pega vale um ponto; banana que chega ao chão tira uma vida (são 3).
 * A velocidade aumenta com os pontos. Tela de início, tela de fim de jogo e
 * recorde da sessão (e no pendrive, se houver um montado em /usb: fase 6).
 *
 * Só inteiros: posições em "subpixels" (x16) para a velocidade poder
 * crescer aos poucos sem ponto flutuante. Para não redesenhar a janela
 * inteira a cada quadro, o fundo fica guardado numa cópia, e cada objeto
 * que se move apaga o próprio rastro copiando o pedaço de fundo de volta. */
#include "sagui.h"
#include "gui.h"

#define LARG 480
#define ALT  400
#define TOPO 28                   /* barra de pontos */
#define CHAO 24
#define CHAO_Y (ALT - CHAO)
#define ESC_SAGUI 3
#define SAGUI_L (16 * ESC_SAGUI)
#define SAGUI_A (16 * ESC_SAGUI)
#define ESC_BANANA 2
#define BANANA_L (8 * ESC_BANANA)
#define BANANA_A (12 * ESC_BANANA)
#define MAX_BANANAS 6
#define QUADRO_MS 40
#define PASSO_SAGUI 16

static const char *const sprite_sagui[16] = {
    "................",
    "....EEEEEEEE....",
    ".BBEEEEBBEEEEBB.",
    "BBBEERRRRRREEBBB",
    "BBBERPRRRRPREBBB",
    ".BBERRRRRRRREBB.",
    "....ERRPPRRE....",
    ".....EEEEEE.....",
    "..M.MMMMMMMM.M..",
    "..MMMCCCCCCMMM..",
    "...MMCcccCCMM...",
    "....MCcccCCM...E",
    "....MCCCCCCM..EC",
    "....MMMMMMMM.EC.",
    "....EE.MM.EEEC..",
    "...EEE....EE....",
};

static const char *const sprite_banana[12] = {
    "......PE",
    ".....PEP",
    "....PYP.",
    "...PYYP.",
    "..PYYYP.",
    ".PYYYAP.",
    "PYYYYAP.",
    "PYYYAP..",
    "PYYAP...",
    ".PAAP...",
    "..PP....",
    "........",
};

static const char *const sprite_coracao[7] = {
    ".RR.RR.", "RRRRRRR", "RRRRRRR", ".RRRRR.", "..RRR..", "...R...", ".......",
};

static uint32_t paleta(char c)
{
    switch (c) {
    case 'P': return 0x141210; case 'E': return 0x3C2D23; case 'M': return 0x6E553C;
    case 'C': return 0x968C7D; case 'c': return 0xBEB4A5; case 'B': return 0xF5F5F0;
    case 'R': return 0xAA7864; case 'Y': return 0xFFD52E; case 'A': return 0xD9A400;
    default: return 0xFF000000;
    }
}

static uint32_t paleta_coracao(char c) { return c == 'R' ? 0xE03C3C : 0xFF000000; }

typedef struct { bool viva; int x, y16; } banana_t;   /* y em 1/16 de pixel */

static jan_t *j;
static uint32_t *fundo;
static banana_t bananas[MAX_BANANAS];
static int sagui_x, pontos, vidas, recorde;
static uint32_t semente;

static uint32_t aleatorio(void)
{
    semente = semente * 1103515245u + 12345u;
    return semente >> 8;
}

/* ------------------------------------------------------------ desenho */

static void desenhar_fundo(void)
{
    for (int y = TOPO; y < ALT; y++) {
        uint32_t c;
        if (y >= CHAO_Y) c = y < CHAO_Y + 4 ? 0x2E9E44 : 0x7A5230;          /* grama e terra */
        else {
            int t = (y - TOPO) * 255 / (CHAO_Y - TOPO);                        /* céu: azul para quase branco */
            c = ((uint32_t)(120 + t * 100 / 255) << 16) | ((uint32_t)(190 + t * 50 / 255) << 8) | 245;
        }
        for (int x = 0; x < LARG; x++) fundo[y * LARG + x] = c;
    }
    /* Duas nuvens simples. */
    for (int k = 0; k < 2; k++) {
        int cx = 90 + k * 260, cy = 70 + k * 30;
        for (int y = -14; y <= 14; y++)
            for (int x = -40; x <= 40; x++)
                if (x * x / 4 + y * y * 4 < 420 && cy + y >= TOPO)
                    fundo[(cy + y) * LARG + cx + x] = 0xFAFCFF;
    }
}

static void restaurar(int x, int y, int l, int a)
{
    for (int yy = y; yy < y + a; yy++) {
        if (yy < TOPO || yy >= ALT) continue;
        for (int xx = x; xx < x + l; xx++)
            if (xx >= 0 && xx < LARG) j->px[yy * LARG + xx] = fundo[yy * LARG + xx];
    }
}

static void desenhar_barra(void)
{
    g_retangulo(j, 0, 0, LARG, TOPO, 0x18202A);
    char t[64];
    formatar_texto(t, sizeof t, "Pontos: %d", pontos);
    g_texto(j, 10, 6, t, 0xFFDF4A);
    formatar_texto(t, sizeof t, "Recorde: %d", recorde);
    g_texto(j, 170, 6, t, 0xD0D4D8);
    g_texto(j, LARG - 120, 6, "Vidas:", 0xD0D4D8);
    for (int i = 0; i < vidas; i++)
        g_desenho(j, LARG - 66 + i * 20, 8, sprite_coracao, 7, 7, paleta_coracao, 2);
    jan_mostrar_area(j, 0, 0, LARG, TOPO);
}

static void desenhar_sagui(void)
{
    g_desenho(j, sagui_x, CHAO_Y - SAGUI_A, sprite_sagui, 16, 16, paleta, ESC_SAGUI);
}

static void texto_centro(int y, const char *t, uint32_t cor)
{
    g_texto(j, (LARG - g_largura_texto(t)) / 2, y, t, cor);
}

static void tela_inicio(void)
{
    memcpy(j->px + TOPO * LARG, fundo + TOPO * LARG, (size_t)(LARG * (ALT - TOPO)) * 4);
    desenhar_barra();
    g_desenho(j, LARG / 2 - 48, 90, sprite_sagui, 16, 16, paleta, 6);
    g_desenho(j, LARG / 2 + 70, 120, sprite_banana, 8, 12, paleta, 4);
    texto_centro(56, "SAGUI PEGA BANANA", 0x14337E);
    texto_centro(200, "Use as setas para mover o sagui.", 0x1E2328);
    texto_centro(220, "Pegue as bananas antes que caiam no chão!", 0x1E2328);
    texto_centro(260, "Aperte Enter (ou clique) para começar", 0x0B7A3E);
    jan_mostrar(j);
}

static void tela_fim(void)
{
    g_retangulo(j, 60, 120, LARG - 120, 140, 0x18202A);
    g_moldura(j, 60, 120, LARG - 120, 140, 0xFFDF4A);
    texto_centro(138, "FIM DE JOGO", 0xFFDF4A);
    char t[64];
    formatar_texto(t, sizeof t, "Você pegou %d banana%s.", pontos, pontos == 1 ? "" : "s");
    texto_centro(170, t, 0xFFFFFF);
    formatar_texto(t, sizeof t, "Recorde: %d", recorde);
    texto_centro(192, t, 0xD0D4D8);
    texto_centro(228, "Enter para jogar de novo", 0x3CDC6E);
    jan_mostrar_area(j, 60, 120, LARG - 120, 140);
}

/* ------------------------------------------------------------ recorde no pendrive (fase 6) */

static void ler_recorde(void)
{
    int fd = abrir("/usb/sagui-recorde.txt", ABRIR_LER);
    if (fd < 0) return;
    char b[16] = { 0 };
    ler(fd, b, sizeof b - 1);
    fechar(fd);
    recorde = texto_para_int(b);
}

static void gravar_recorde(void)
{
    int fd = abrir("/usb/sagui-recorde.txt", ABRIR_ESCREVER | ABRIR_CRIAR | ABRIR_TRUNCAR);
    if (fd < 0) return;
    char b[16];
    int n = formatar_texto(b, sizeof b, "%d\n", recorde);
    escrever(fd, b, (unsigned)n);
    fechar(fd);
    registrar("banana: recorde %d salvo no pendrive", recorde);
}

/* ------------------------------------------------------------ jogo */

static int velocidade16(void) { return 32 + pontos * 4; }           /* 2 px por quadro, mais 1/4 por ponto */
static int intervalo_bananas(void) { int q = 45 - pontos; return q < 14 ? 14 : q; }   /* em quadros */

static void nova_banana(void)
{
    for (int i = 0; i < MAX_BANANAS; i++)
        if (!bananas[i].viva) {
            bananas[i].viva = true;
            bananas[i].x = 8 + (int)(aleatorio() % (LARG - 16 - BANANA_L));
            bananas[i].y16 = TOPO * 16;
            registrar("banana: nova banana em x=%d", bananas[i].x);
            return;
        }
}

static void mover_sagui(int dx)
{
    int novo = sagui_x + dx;
    if (novo < 0) novo = 0;
    if (novo > LARG - SAGUI_L) novo = LARG - SAGUI_L;
    if (novo == sagui_x) return;
    restaurar(sagui_x, CHAO_Y - SAGUI_A, SAGUI_L, SAGUI_A);
    jan_mostrar_area(j, sagui_x, CHAO_Y - SAGUI_A, SAGUI_L, SAGUI_A);
    sagui_x = novo;
    desenhar_sagui();
    jan_mostrar_area(j, sagui_x, CHAO_Y - SAGUI_A, SAGUI_L, SAGUI_A);
    registrar("banana: sagui x=%d", sagui_x);
}

/* Devolve false quando a janela foi fechada. */
static bool jogar(void)
{
    pontos = 0;
    vidas = 3;
    sagui_x = (LARG - SAGUI_L) / 2;
    for (int i = 0; i < MAX_BANANAS; i++) bananas[i].viva = false;
    memcpy(j->px + TOPO * LARG, fundo + TOPO * LARG, (size_t)(LARG * (ALT - TOPO)) * 4);
    desenhar_barra();
    desenhar_sagui();
    jan_mostrar(j);
    registrar("banana: inicio");
    int ate_proxima = 1;
    evento_t e;
    while (vidas > 0) {
        while (proximo_evento(j->id, &e, false) == 1) {
            if (e.tipo == EV_FECHAR) return false;
            if (e.tipo == EV_TECLA && e.tecla == TECLA_ESQUERDA) mover_sagui(-PASSO_SAGUI);
            if (e.tipo == EV_TECLA && e.tecla == TECLA_DIREITA) mover_sagui(PASSO_SAGUI);
        }
        if (--ate_proxima <= 0) { nova_banana(); ate_proxima = intervalo_bananas(); }
        bool barra = false;
        for (int i = 0; i < MAX_BANANAS; i++) {
            banana_t *b = &bananas[i];
            if (!b->viva) continue;
            int y_ant = b->y16 / 16;
            b->y16 += velocidade16();
            int y = b->y16 / 16;
            restaurar(b->x, y_ant, BANANA_L, BANANA_A);
            /* Pegou: a banana encostou no sagui (contando só a parte de cima dele). */
            bool pegou = y + BANANA_A >= CHAO_Y - SAGUI_A + 6 && y < CHAO_Y - 8 &&
                         b->x + BANANA_L > sagui_x + 4 && b->x < sagui_x + SAGUI_L - 4;
            if (pegou || y + BANANA_A >= CHAO_Y) {
                b->viva = false;
                if (pegou) { pontos++; registrar("banana: pegou! pontos %d", pontos); }
                else { vidas--; registrar("banana: caiu no chao, vidas %d", vidas); }
                barra = true;
                desenhar_sagui();
                jan_mostrar_area(j, b->x, y_ant, BANANA_L, y + BANANA_A - y_ant);
                continue;
            }
            g_desenho(j, b->x, y, sprite_banana, 8, 12, paleta, ESC_BANANA);
            jan_mostrar_area(j, b->x, y_ant, BANANA_L, y + BANANA_A - y_ant);
        }
        if (barra) desenhar_barra();
        dormir(QUADRO_MS);
    }
    bool novo_recorde = pontos > recorde;
    if (novo_recorde) recorde = pontos;
    registrar("banana: fim de jogo, pontos %d, recorde %d", pontos, recorde);
    desenhar_barra();
    tela_fim();
    if (novo_recorde) gravar_recorde();
    return true;
}

static bool esperar_enter(void)
{
    evento_t e;
    for (;;) {
        if (proximo_evento(j->id, &e, true) != 1) continue;
        if (e.tipo == EV_FECHAR) return false;
        if ((e.tipo == EV_TECLA && (e.tecla == '\n' || e.tecla == ' ')) || e.tipo == EV_CLIQUE) return true;
    }
}

int principal(int argc, char **argv)
{
    j = jan_abrir("Sagui Pega Banana", LARG, ALT);
    fundo = aloca((size_t)(LARG * ALT) * 4);
    if (!j || !fundo) return 1;
    semente = argc > 1 ? (uint32_t)texto_para_int(argv[1]) : tempo_ms() | 1;
    desenhar_fundo();
    ler_recorde();
    tela_inicio();
    registrar("banana: tela de inicio");
    while (esperar_enter() && jogar())
        ;
    jan_fechar(j);
    return 0;
}
