/* arte.c - paleta, texturas e personagens do Sagui 3D.
 *
 * Tudo foi desenhado para este jogo:
 *   - os personagens e objetos são pixel art escrita como texto, uma letra
 *     por pixel (a tabela "letras" diz a cor de cada letra);
 *   - as texturas de 32x32 são geradas por regras simples na hora em que o
 *     jogo abre: fiadas de pedra com juntas desencontradas, cipós ondulando
 *     pela tabela de seno, tábuas com veios e pregos, grama com florzinhas.
 *     Gerar em vez de guardar economiza espaço na imagem (cada textura
 *     guardada seria 1 KB) e deixa cada regra explicada no código.
 * A paleta (até 96 cores) mistura o verde do SaguiOS, os marrons do sagui e do macaco-prego
 * e os tons de pedra e mata. A luz não muda a paleta: a tabela "sombra" tem
 * cada cor em 16 níveis, escurecendo na direção de um verde quase preto,
 * como a sombra dentro da mata (e não um cinza). */
#include "jogo.h"

uint8_t  texturas[QTD_TEXTURAS][TEX][TEX];
uint32_t sombra[NIVEIS_LUZ][96];
figura_t figuras[QTD_FIGURAS];

/* ------------------------------------------------------------ paleta */
enum {
    /* personagens (1 a 27) */
    P_TRANSP = 0, P_CONTORNO, P_BONE, P_PELO, P_PELO_CLARO, P_ROSTO, P_BRANCO,
    P_BANANA, P_BANANA_SOMBRA, P_MADEIRA_ESC, P_MADEIRA, P_OURO, P_OURO_ESC,
    P_JABUTICABA, P_JABUTICABA_BRILHO, P_VERMELHO, P_CINZA, P_CINZA_ESC, P_ELASTICO,
    P_VERDE, P_VERDE_CLARO, P_VERDE_ESC, P_LARANJA, P_SAGUI_ESC, P_SAGUI_ROSTO,
    P_SAGUI_PELO, P_SAGUI_CORPO, P_SAGUI_CLARO,
    /* texturas (28 a 63) */
    P_PEDRA1, P_PEDRA2, P_PEDRA3, P_PEDRA4, P_JUNTA, P_PEDRA_LUZ, P_MUSGO, P_MUSGO_ESC,
    P_TABUA1, P_TABUA2, P_TABUA3, P_TABUA4, P_VEIO, P_PREGO,
    P_GRAMA1, P_GRAMA2, P_GRAMA3, P_GRAMA4, P_GRAMA5,
    P_TERRA1, P_TERRA2, P_TERRA3, P_TERRA4,
    P_LAJE1, P_LAJE2, P_LAJE3, P_REJUNTE,
    P_FOLHA1, P_FOLHA2, P_FOLHA3, P_FOLHA4,
    P_ROCHA1, P_ROCHA2, P_ROCHA3, P_ROCHA4,
    P_FLOR_AMARELA, P_FLOR_BRANCA, P_PEDRINHA, P_CIPO_ESC, P_CIPO_CLARO,
    QTD_CORES
};
_Static_assert(QTD_CORES <= 96, "a paleta tem até 96 cores");

const uint32_t paleta[96] = {
    0x000000, 0x1E1A16, 0x3A2A1E, 0x6B4A2E, 0x8C6640, 0xE8D2A8, 0xFFFFFF,
    0xF4D03F, 0xC9A227, 0x5C3A1A, 0x8B5A2B, 0xFFD23F, 0xB8860B,
    0x3B1E4A, 0x8A5C9E, 0xE04848, 0xA8A29A, 0x5E5A55, 0x2B2420,
    0x0B7A3E, 0x5FB33A, 0x0A4D27, 0xE88A2D, 0x3C2D23, 0xAA7864,
    0x6E553C, 0x968C7D, 0xF5F5F0,
    0x6E6A60, 0x7F7A6E, 0x8F8A7C, 0x5A564E, 0x403C36, 0xA29D90, 0x65705A, 0x4F5A45,
    0x7A4E26, 0x8E5C2E, 0x6A421F, 0x9E6B38, 0x4E3216, 0x3C3C3C,
    0x3E7D2E, 0x4A8F35, 0x356B27, 0x5AA040, 0x2C5A20,
    0x7A5A3A, 0x6A4C30, 0x8A6844, 0x5A3E26,
    0x8A8478, 0x9A9488, 0x7A7468, 0x55504A,
    0x1F4D24, 0x2A6230, 0x173B1B, 0x3A7A3A,
    0x4E4A44, 0x5C5852, 0x423E39, 0x67625B,
    0xF2D54B, 0xF5F0E6, 0xA09A90, 0x2E6B2A, 0x4E9A3C,
};

/* Letra de cada cor nos desenhos. */
static int letra(char c)
{
    switch (c) {
    case 'k': return P_CONTORNO;   case 'p': return P_BONE;        case 'm': return P_PELO;
    case 'M': return P_PELO_CLARO; case 'c': return P_ROSTO;       case 'w': return P_BRANCO;
    case 'y': return P_BANANA;     case 'Y': return P_BANANA_SOMBRA; case 'B': return P_MADEIRA_ESC;
    case 'b': return P_MADEIRA;    case 'd': return P_OURO;        case 'D': return P_OURO_ESC;
    case 'j': return P_JABUTICABA; case 'J': return P_JABUTICABA_BRILHO; case 'r': return P_VERMELHO;
    case 's': return P_CINZA;      case 'S': return P_CINZA_ESC;   case 'e': return P_ELASTICO;
    case 'g': return P_VERDE;      case 'G': return P_VERDE_CLARO; case 'h': return P_VERDE_ESC;
    case 'o': return P_LARANJA;    case 'q': return P_SAGUI_ESC;   case 't': return P_SAGUI_ROSTO;
    case 'n': return P_SAGUI_PELO; case 'u': return P_SAGUI_CORPO; case 'v': return P_SAGUI_CLARO;
    default:  return P_TRANSP;
    }
}

/* Cada nível de luz aproxima a cor do "escuro da mata" (verde quase preto). */
static void preparar_sombra(void)
{
    const int er = 0x0C, eg = 0x1A, eb = 0x10;
    for (int n = 0; n < NIVEIS_LUZ; n++) {
        int f = (n + 1) * 16;   /* 16 a 256 */
        for (int i = 0; i < 96; i++) {
            int r = (paleta[i] >> 16) & 0xFF, g = (paleta[i] >> 8) & 0xFF, b = paleta[i] & 0xFF;
            r = er + ((r - er) * f >> 8);
            g = eg + ((g - eg) * f >> 8);
            b = eb + ((b - eb) * f >> 8);
            sombra[n][i] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
    }
}

/* ------------------------------------------------------------ personagens */

static const char *const macaco[32] = {
    "........kkkkkkkk........",
    "......kkppppppppkk......",
    ".....kppppppppppppk.....",
    "....kppppppppppppppk....",
    "....kpppMMMMMMMMpppk....",
    "...kkMMccccccccccMMkk...",
    "..kcMMccccccccccccMMck..",
    "..kcMcckkcccccckkccMck..",
    "...kMcckwccccccwkccMk...",
    "....kcccccccccccccck....",
    "....kcccccckkcccccck....",
    ".....kcccccccccccck.....",
    "......kkMMMMMMMMkk......",
    ".....kkmmmmmmmmmmkk.....",
    "....kmmkmmmmmmmmkmmk....",
    "...kmmkmmMMMMMMmmkmmk...",
    "...kmmkmMMMMMMMMmkmmk...",
    "...kmmkmMMMMMMMMmkmmk...",
    "...kmmkmMMMMMMMMmkmmk...",
    "...kmmkmmMMMMMMmmkmmk...",
    "...kcckmmmmmmmmmmkcck...",
    "....kk.kmmmmmmmmk.kkmk..",
    ".......kmmmmmmmmk..kmmk.",
    ".......kmmmkkmmmk...kmmk",
    "......kmmmk..kmmmk..kmmk",
    "......kmmmk..kmmmk.kmmmk",
    "......kmmmk..kmmmkkmmkmk",
    "......kmmmk..kmmmkkmkkmk",
    ".....kmmmmk..kmmmmkkmmk.",
    ".....kpppk....kpppk.kk..",
    ".....kkkkk....kkkkk.....",
    "........................",
};

/* O mesmo macaco com os olhos rodando (tonto). */
static const char *const macaco_tonto_olhos[2] = {
    "..kcMcwkwccccccwkwcMck..",
    "...kMckwkcccccckwkcMk...",
};

static const char *const banana[12] = {
    "..........k.",
    ".........kBk",
    "........kyk.",
    ".......kyyk.",
    "......kyyYk.",
    ".....kyyyYk.",
    "...kkyyyYk..",
    ".kkyyyyYk...",
    "kyyyyyYk....",
    "kYyyyYkk....",
    ".kkkkk......",
    "............",
};

static const char *const cacho[16] = {
    "......kkkk......",
    "......kBBk......",
    ".....kkBBkk.....",
    "....kdkddkdk....",
    "...kddkddkddk...",
    "..kdwdkddkdddk..",
    "..kdddkddkdddk..",
    ".kdddDkddkDdddk.",
    ".kddDkdwdDkDddk.",
    ".kddDkdddDkDddk.",
    "kdddDkddDDkDdddk",
    "kddDkkddDDkkDddk",
    "kdDk.kdDDk.kDdDk",
    ".kk..kDDk...kkk.",
    "......kk........",
    "................",
};

static const char *const casca[7] = {
    "......kk......",
    ".....kYYk.....",
    "..kkkYyyYkkk..",
    ".kyyYkyykYyyk.",
    "kyyk.kyyk.kyyk",
    "kYk...kk...kYk",
    ".k..........k.",
};

static const char *const semente[6] = {
    "..kk..",
    ".kjjk.",
    "kjJjjk",
    "kjjjjk",
    ".kjjk.",
    "..kk..",
};

/* Pote de jabuticabas: munição para o estilingue. */
static const char *const pote[12] = {
    "....kk...kk.....",
    "...kJjk.kJjk....",
    "..kjjjkkjjjkkk..",
    ".kkjjkjJkjkJjjk.",
    ".kjkkjjjjkkjjjk.",
    "kBBBBBBBBBBBBBBk",
    "kbbbbbbbbbbbbbbk",
    ".kbbbbbbbbbbbbk.",
    ".kBbbbbbbbbbbBk.",
    "..kBbbbbbbbbBk..",
    "...kkkkkkkkkk...",
    "................",
};

static const char *const estrela[5] = {
    "..d..",
    "..d..",
    "ddwdd",
    "..d..",
    "..d..",
};

static const char *const coracao[7] = {
    ".kk.kk.",
    "krrkrrk",
    "krrrrrk",
    "krrrrrk",
    ".krrrk.",
    "..krk..",
    "...k...",
};

/* O sagui do SaguiOS (o mesmo do Sagui Pega Banana), para as telas. */
static const char *const desenho_sagui[16] = {
    "................",
    "....qqqqqqqq....",
    ".vvqqqqvvqqqqvv.",
    "vvvqqttttttqqvvv",
    "vvvqtkttttktqvvv",
    ".vvqttttttttqvv.",
    "....qttkkttq....",
    ".....qqqqqq.....",
    "..n.nnnnnnnn.n..",
    "..nnnuuuuuunnn..",
    "...nnusssuunn...",
    "....nusssuun...q",
    "....nuuuuuun..qu",
    "....nnnnnnnn.qu.",
    "....qq.nn.qqqu..",
    "...qqq....qq....",
};

/* Metade esquerda do estilingue visto pelo sagui (a direita é o espelho),
 * com as mãozinhas cinzentas segurando o cabo. 20 colunas, 30 linhas. */
static const char *const estilingue_metade[30] = {
    "...kk...............",
    "..kbbk..............",
    "..kbbk..............",
    "..kbBk..............",
    "..kbBk..............",
    "..kbBk..............",
    "...kbBk.............",
    "...kbBk.............",
    "....kbBk............",
    "....kbBk............",
    ".....kbBk...........",
    "......kbBk..........",
    ".......kbBkk........",
    "........kbbBkkk.....",
    ".........kbbbBBkkkkk",
    "..........kkbbbbbbbb",
    "............kbbbbbbb",
    "............kbbBbbbb",
    "...........kssskbbbb",
    "..........kssssskbbb",
    ".........kssssssskbb",
    ".........kssssSssskb",
    "........ksssssSsssk.",
    "........kssssssSssk.",
    "........ksssssssssk.",
    ".......ksssssssssk..",
    ".......kssssssssk...",
    "......ksssssssk.....",
    "......kssssssk......",
    "......kkkkkkk.......",
};

/* ------------------------------------------------------------ montagem das figuras */

static uint8_t memoria_figuras[16384];
static int usado_figuras;

static uint8_t *reservar(int n)
{
    if (n < 0 || usado_figuras + n > (int)sizeof memoria_figuras) return NULL;
    uint8_t *p = memoria_figuras + usado_figuras;
    usado_figuras += n;
    return p;
}

static void figura_de_texto(int id, const char *const *linhas, int l, int a)
{
    uint8_t *px = reservar(l * a);
    if (!px) return;
    for (int y = 0; y < a; y++)
        for (int x = 0; x < l; x++)
            px[y * l + x] = (uint8_t)letra(linhas[y][x]);
    figuras[id] = (figura_t){ l, a, px };
}

static void pintar(figura_t *f, int x, int y, int cor)
{
    if (x >= 0 && y >= 0 && x < f->l && y < f->a) ((uint8_t *)f->px)[y * f->l + x] = (uint8_t)cor;
}

/* Linha reta de pixels (para o elástico do estilingue). */
static void riscar(figura_t *f, int x0, int y0, int x1, int y1, int cor)
{
    int dx = x1 > x0 ? x1 - x0 : x0 - x1, dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int passos = dx > dy ? dx : dy;
    for (int i = 0; i <= passos; i++) {
        int x = x0 + (passos ? (x1 - x0) * i / passos : 0);
        int y = y0 + (passos ? (y1 - y0) * i / passos : 0);
        pintar(f, x, y, cor);
    }
}

static void montar_estilingue(int id, bool puxado)
{
    const int l = 40, a = 30;
    uint8_t *px = reservar(l * a);
    if (!px) return;
    for (int y = 0; y < a; y++)
        for (int x = 0; x < 20; x++) {
            int c = letra(estilingue_metade[y][x]);
            px[y * l + x] = (uint8_t)c;
            px[y * l + (l - 1 - x)] = (uint8_t)c;
        }
    figura_t *f = &figuras[id];
    *f = (figura_t){ l, a, px };
    if (!puxado) {
        riscar(f, 4, 3, 35, 3, P_ELASTICO);          /* elástico solto entre as pontas */
    } else {
        /* Elástico puxado para perto do sagui, com a jabuticaba no meio. */
        riscar(f, 4, 3, 18, 12, P_ELASTICO);
        riscar(f, 35, 3, 21, 12, P_ELASTICO);
        for (int y = 10; y <= 14; y++)
            for (int x = 17; x <= 22; x++)
                pintar(f, x, y, (x == 18 && y == 11) ? P_JABUTICABA_BRILHO : P_JABUTICABA);
    }
}

/* A porteira da banana: dois mourões, uma placa com a banana em cima e,
 * fechada, três tábuas com uma trava em diagonal. Aberta, só os mourões e a
 * placa, com folhas enroladas (dá para ver o caminho entre eles). */
static void montar_porteira(int id, bool aberta)
{
    const int l = 32, a = 40;
    uint8_t *px = reservar(l * a);
    if (!px) return;
    for (int i = 0; i < l * a; i++) px[i] = P_TRANSP;
    figura_t *f = &figuras[id];
    *f = (figura_t){ l, a, px };
    for (int y = 2; y < a; y++)                      /* mourões */
        for (int x = 0; x < 4; x++) {
            pintar(f, x, y, x == 0 ? P_CONTORNO : (x == 3 ? P_MADEIRA_ESC : P_TABUA2));
            pintar(f, l - 1 - x, y, x == 0 ? P_CONTORNO : (x == 3 ? P_MADEIRA_ESC : P_TABUA2));
        }
    for (int y = 0; y < 11; y++)                     /* placa */
        for (int x = 3; x < l - 3; x++)
            pintar(f, x, y, (y == 0 || y == 10 || x == 3 || x == l - 4) ? P_CONTORNO : P_TABUA4);
    for (int y = 0; y < 9; y++)                      /* banana desenhada na placa */
        for (int x = 0; x < 12; x++) {
            int c = letra(banana[y + 1][x]);
            if (c) pintar(f, 10 + x, 1 + y, c);
        }
    if (!aberta) {
        int tabuas[3] = { 14, 23, 32 };
        for (int t = 0; t < 3; t++)
            for (int y = tabuas[t]; y < tabuas[t] + 4; y++)
                for (int x = 4; x < l - 4; x++)
                    pintar(f, x, y, y == tabuas[t] ? P_TABUA4 : (y == tabuas[t] + 3 ? P_CONTORNO : P_TABUA1));
        for (int k = -1; k <= 1; k++)
            riscar(f, 4, 35 + k, l - 5, 15 + k, k == 0 ? P_TABUA3 : P_MADEIRA_ESC);
    } else {
        for (int y = 12; y < a; y += 5) {            /* folhas subindo pelos mourões */
            pintar(f, 4, y, P_VERDE_CLARO); pintar(f, 5, y + 1, P_VERDE);
            pintar(f, l - 5, y + 2, P_VERDE_CLARO); pintar(f, l - 6, y + 3, P_VERDE);
        }
    }
}

/* ------------------------------------------------------------ texturas */

static uint32_t ruido(int x, int y, int s)
{
    uint32_t h = (uint32_t)x * 0x27D4EB2Du + (uint32_t)y * 0x165667B1u + (uint32_t)s * 0x9E3779B9u;
    h ^= h >> 15;
    h *= 0x85EBCA6Bu;
    h ^= h >> 13;
    return h;
}

/* Pedra: quatro fiadas de 8 pixels; em cada uma, três pedras de larguras
 * sorteadas que somam 32 (para a textura emendar sem costura), começando
 * num ponto diferente em cada fiada para as juntas não alinharem. A borda
 * de cima e da esquerda de cada pedra é mais clara, a de baixo mais escura,
 * o que dá um relevo leve. Algumas pedras têm musgo. */
static void gerar_pedra(uint8_t t[TEX][TEX], int semente)
{
    for (int fiada = 0; fiada < 4; fiada++) {
        int y0 = fiada * 8;
        uint32_t h = ruido(fiada, 7, semente);
        int l1 = 8 + (int)(h % 6), l2 = 8 + (int)((h >> 8) % 6);
        int inicios[4] = { 0, l1, l1 + l2, 32 };
        int desloca = (int)((h >> 16) % 32);
        for (int p = 0; p < 3; p++) {
            uint32_t hp = ruido(fiada, p, semente + 3);
            static const int tons[5] = { P_PEDRA1, P_PEDRA2, P_PEDRA3, P_PEDRA4, P_MUSGO };
            int base = tons[hp % 5];
            for (int xx = inicios[p]; xx < inicios[p + 1]; xx++)
                for (int y = y0; y < y0 + 8; y++) {
                    int x = (xx + desloca) % 32;
                    int c = base;
                    if (y == y0 + 7 || xx == inicios[p]) c = P_JUNTA;
                    else if (y == y0 || xx == inicios[p] + 1) c = P_PEDRA_LUZ;
                    else if (y == y0 + 6) c = P_PEDRA4;
                    else {
                        uint32_t r = ruido(x, y, semente + 11) % 13;
                        if (r == 0) c = P_JUNTA;
                        else if (r < 3) c = base == P_MUSGO ? P_MUSGO_ESC : P_PEDRA4;
                    }
                    t[y][x] = (uint8_t)c;
                }
        }
    }
}

/* Cipó: a mesma pedra com dois cipós descendo em onda (a posição de cada
 * linha sai da tabela de seno) e uma folha a cada 8 linhas, alternando o lado. */
static void gerar_cipo(uint8_t t[TEX][TEX])
{
    gerar_pedra(t, 5);
    static const int base[2] = { 7, 22 };
    for (int v = 0; v < 2; v++)
        for (int y = 0; y < TEX; y++) {
            int x = base[v] + INTEIRO(seno(y * (VOLTA / TEX) + v * 900) * 3);
            t[y][(x + TEX) % TEX] = P_CIPO_ESC;
            t[y][(x + 1 + TEX) % TEX] = P_CIPO_CLARO;
            if ((y + v * 4) % 8 == 3) {
                int lado = ((y / 8) + v) % 2 ? 2 : -3;
                for (int k = 0; k < 2; k++) {
                    t[y][(x + lado + k + TEX) % TEX] = P_VERDE_CLARO;
                    t[(y + 1) % TEX][(x + lado + k + TEX) % TEX] = P_VERDE;
                }
            }
        }
}

/* Madeira: quatro tábuas de 8 pixels em pé, cada uma num tom, com veios
 * (traços curtos sorteados), uma junta escura e dois pregos. */
static void gerar_madeira(uint8_t t[TEX][TEX])
{
    static const int tons[4] = { P_TABUA1, P_TABUA2, P_TABUA3, P_TABUA4 };
    for (int x = 0; x < TEX; x++)
        for (int y = 0; y < TEX; y++) {
            int tabua = x / 8;
            int c = tons[ruido(tabua, 0, 21) % 4];
            if (x % 8 == 0) c = P_VEIO;
            else if (ruido(x, y / 4, 22) % 9 == 0) c = P_VEIO;
            else if (ruido(x, y, 23) % 17 == 0) c = P_TABUA3;
            if ((y == 3 || y == 28) && x % 8 == 4) c = P_PREGO;
            t[y][x] = (uint8_t)c;
        }
}

static void gerar_grama(uint8_t t[TEX][TEX])
{
    static const int tons[5] = { P_GRAMA1, P_GRAMA2, P_GRAMA3, P_GRAMA4, P_GRAMA5 };
    for (int y = 0; y < TEX; y++)
        for (int x = 0; x < TEX; x++) {
            uint32_t r = ruido(x, y, 31);
            int c = tons[r % 5];
            if (r % 97 == 0) c = P_FLOR_AMARELA;
            else if (r % 89 == 0) c = P_FLOR_BRANCA;
            t[y][x] = (uint8_t)c;
        }
}

static void gerar_terra(uint8_t t[TEX][TEX])
{
    for (int y = 0; y < TEX; y++)
        for (int x = 0; x < TEX; x++) {
            uint32_t r = ruido(x / 2, y / 2, 41) % 7;
            int c = r < 3 ? P_TERRA1 : (r < 5 ? P_TERRA2 : (r < 6 ? P_TERRA3 : P_TERRA4));
            if (ruido(x, y, 42) % 41 == 0) c = P_PEDRINHA;
            t[y][x] = (uint8_t)c;
        }
}

/* Lajota: quatro placas de 16x16 separadas por rejunte, cada uma num tom. */
static void gerar_lajota(uint8_t t[TEX][TEX])
{
    static const int tons[3] = { P_LAJE1, P_LAJE2, P_LAJE3 };
    for (int y = 0; y < TEX; y++)
        for (int x = 0; x < TEX; x++) {
            int c = tons[ruido(x / 16, y / 16, 51) % 3];
            if (x % 16 == 0 || y % 16 == 0) c = P_REJUNTE;
            else if (ruido(x, y, 52) % 15 == 0) c = P_LAJE3;
            t[y][x] = (uint8_t)c;
        }
}

/* Folhas (teto de mata fechada): fundo escuro com folhas ovais espalhadas
 * numa grade de 8x8 com o centro sorteado. */
static void gerar_folhas(uint8_t t[TEX][TEX])
{
    for (int y = 0; y < TEX; y++)
        for (int x = 0; x < TEX; x++) t[y][x] = P_FOLHA3;
    static const int tons[3] = { P_FOLHA1, P_FOLHA2, P_FOLHA4 };
    for (int gy = 0; gy < 4; gy++)
        for (int gx = 0; gx < 4; gx++) {
            uint32_t h = ruido(gx, gy, 61);
            int cx = gx * 8 + (int)(h % 6), cy = gy * 8 + (int)((h >> 4) % 6);
            int cor = tons[(h >> 8) % 3];
            for (int dy = -2; dy <= 2; dy++)
                for (int dx = -3; dx <= 3; dx++)
                    if (dx * dx * 4 + dy * dy * 9 <= 36)
                        t[(cy + dy + TEX) % TEX][(cx + dx + TEX) % TEX] = (uint8_t)cor;
        }
}

static void gerar_rocha(uint8_t t[TEX][TEX])
{
    static const int tons[4] = { P_ROCHA1, P_ROCHA2, P_ROCHA3, P_ROCHA4 };
    for (int y = 0; y < TEX; y++)
        for (int x = 0; x < TEX; x++) {
            int c = tons[ruido(x / 3, y / 3, 71) % 4];
            if (ruido(x, y, 72) % 23 == 0) c = P_JUNTA;
            t[y][x] = (uint8_t)c;
        }
}

int textura_por_nome(const char *nome)
{
    static const char *const nomes[QTD_TEXTURAS] = {
        "pedra", "cipo", "madeira", "grama", "terra", "lajota", "folhas", "rocha", "ceu",
    };
    for (int i = 0; i < QTD_TEXTURAS; i++)
        if (compara_texto(nomes[i], nome) == 0) return i;
    return -1;
}

/* Céu das salas abertas: degradê do azul para o claro no horizonte e, na
 * altura do horizonte, o contorno das árvores da mata, que gira junto com o
 * sagui (a altura das árvores numa direção é uma soma de ondas do ângulo).
 * O desenho pede uma vez por quadro a altura de cada coluna e a cor de
 * cada linha, em vez de calcular por pixel. */
int altura_arvores(int a)
{
    return 9 + INTEIRO(seno(a * 3) * 4 + seno(a * 7 + 777) * 3 + seno(a * 13 + 123) * 2);
}

uint32_t cor_arvore(int a) { return (seno(a * 29) > 0) ? 0x1F4A28 : 0x183D20; }

uint32_t degrade_ceu(int y)
{
    int t = y * 256 / HORIZONTE;
    int r = 0x4F + ((0xC8 - 0x4F) * t >> 8), g = 0x9C + ((0xE8 - 0x9C) * t >> 8), b = 0xD0 + ((0xF0 - 0xD0) * t >> 8);
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

void arte_iniciar(void)
{
    preparar_sombra();
    gerar_pedra(texturas[T_PEDRA], 1);
    gerar_cipo(texturas[T_CIPO]);
    gerar_madeira(texturas[T_MADEIRA]);
    gerar_grama(texturas[T_GRAMA]);
    gerar_terra(texturas[T_TERRA]);
    gerar_lajota(texturas[T_LAJOTA]);
    gerar_folhas(texturas[T_FOLHAS]);
    gerar_rocha(texturas[T_ROCHA]);
    gerar_grama(texturas[T_CEU]);   /* não é usada como textura: o céu é desenhado à parte */

    figura_de_texto(F_MACACO, macaco, 24, 32);
    figura_de_texto(F_MACACO_TONTO, macaco, 24, 32);
    for (int x = 0; x < 24; x++) {
        ((uint8_t *)figuras[F_MACACO_TONTO].px)[7 * 24 + x] = (uint8_t)letra(macaco_tonto_olhos[0][x]);
        ((uint8_t *)figuras[F_MACACO_TONTO].px)[8 * 24 + x] = (uint8_t)letra(macaco_tonto_olhos[1][x]);
    }
    figura_de_texto(F_BANANA, banana, 12, 12);
    figura_de_texto(F_CACHO, cacho, 16, 16);
    figura_de_texto(F_CASCA, casca, 14, 7);
    figura_de_texto(F_SEMENTE, semente, 6, 6);
    figura_de_texto(F_POTE, pote, 16, 12);
    figura_de_texto(F_ESTRELA, estrela, 5, 5);
    figura_de_texto(F_CORACAO, coracao, 7, 7);
    figura_de_texto(F_SAGUI, desenho_sagui, 16, 16);
    montar_porteira(F_PORTEIRA, false);
    montar_porteira(F_PORTEIRA_ABERTA, true);
    montar_estilingue(F_ESTILINGUE, false);
    montar_estilingue(F_ESTILINGUE_PUXADO, true);
}
