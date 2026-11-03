/* tela3d.c - o desenho em perspectiva do Sagui 3D.
 *
 * A ideia, a partir da geometria:
 *
 * 1. Câmera. Cada ponto do mapa vira duas medidas em relação ao sagui:
 *    "fundo" (quanto está à frente, na direção do olhar) e "lado" (quanto
 *    está à direita). Na tela, por semelhança de triângulos, a coluna é
 *    centro + lado * FOCO / fundo, e uma altura h aparece na linha
 *    horizonte - (h - olho) * FOCO / fundo.
 *
 * 2. Sem olho de peixe. O "fundo" é a distância medida ao longo da direção
 *    do olhar, e não a distância em linha reta até o ponto. Uma parede reta
 *    tem o mesmo "fundo" que o plano da tela enxerga, então continua reta
 *    na tela. Se usasse a distância em linha reta, as colunas dos cantos da
 *    tela (que estão mais longe em linha reta) encolheriam e a parede
 *    ficaria curva.
 *
 * 3. Coluna por coluna, com uma "fresta". Cada coluna da tela guarda a
 *    faixa de linhas que ainda não foi pintada (topo e fundo da fresta).
 *    O desenho começa na sala do sagui. Para cada lado da sala que está de
 *    frente para ele, cada coluna que esse lado ocupa recebe o teto da sala
 *    acima dele, o chão abaixo e:
 *      - se o lado é parede, a parede inteira, e a coluna fecha;
 *      - se é passagem, só os degraus (o pedaço de parede entre os dois
 *        tetos e entre os dois chãos), e a fresta encolhe para o vão da
 *        passagem. Depois a sala vizinha é desenhada só nessas colunas,
 *        dentro da fresta que sobrou.
 *    Como as salas são convexas, os lados de uma sala nunca se cobrem na
 *    tela, então a ordem em que os lados são visitados não importa, e nada
 *    é pintado duas vezes. Uma sala vista por uma passagem é desenhada
 *    depois da sala da frente, mas só no buraco que a da frente deixou:
 *    não precisa ordenar paredes por distância.
 *
 * 4. Onde a parede cai na coluna. Para cada coluna, o raio que sai do olho
 *    é cruzado com o segmento do lado (uma conta de reta contra reta), o
 *    que dá o "fundo" exato e a posição ao longo da parede para a textura.
 *
 * 5. Chão e teto. Numa linha da tela abaixo do horizonte, o chão de altura
 *    h está a fundo = (olho - h) * FOCO / (linha - horizonte), e com o
 *    fundo e a direção da coluna sai o ponto do mapa, que diz o pixel da
 *    textura. FOCO / (linha - horizonte) é calculado uma vez por linha.
 *
 * 6. Coisas (macacos, bananas...). Depois das paredes, cada coisa vira um
 *    retângulo de pé, desenhado de trás para a frente e recortado pela
 *    fresta que a sala dela tinha em cada coluna (guardada no passo 3):
 *    assim um macaco atrás de uma passagem some atrás do batente. */
#include "jogo.h"

uint32_t vista[VA][VL];

#define PERTO (UM / 2)          /* nada mais perto que meio palmo é desenhado */
#define MAX_PROFUNDIDADE 32     /* salas vistas umas através das outras */

static int16_t fresta_topo[VL], fresta_fundo[VL];
static int32_t fresta_da_sala[MAX_SALAS][VL];   /* topo | fundo << 16, ou -1 */
static fixo raio[VL];             /* (coluna - centro) / FOCO */
static fixo dir_x[VL], dir_y[VL]; /* direção de cada coluna no mapa (fundo = 1) */
static fixo inverso[VA];          /* FOCO / (horizonte - centro da linha), com sinal */
static int  arvores[VL];
static uint32_t cor_arvores[VL];
static uint32_t ceu[VA];
static const camera_t *cam;
static fixo cs, sn;

static inline int teto_fixo(fixo v) { return (v + UM - 1) >> 16; }

/* Limita uma distância na tela a ±4096 pixels antes de somar ao centro:
 * um ponto quase encostado no olho daria um número que não cabe em 32 bits
 * (visto no primeiro teste: a parede da direita sumia). */
static inline fixo limitar(fixo v)
{
    return v > FX(4096) ? FX(4096) : (v < -FX(4096) ? -FX(4096) : v);
}

static inline fixo coluna_do_ponto(fixo lado, fixo fundo)
{
    return FX(VL / 2) + limitar(muldiv(lado, FX(FOCO), fundo));
}

/* Linha da tela onde fica a altura h vista a uma distância "fundo"
 * (em fixo, para arredondar como a coluna). */
static inline fixo linha_da_altura(fixo h, fixo fundo)
{
    return FX(HORIZONTE) - limitar(muldiv(h - cam->z, FX(FOCO), fundo));
}

/* Primeira linha cujo centro fica abaixo da fronteira y. */
static inline int primeira_abaixo(fixo y) { return teto_fixo(y - UM / 2); }

static inline int nivel_de_luz(int luz, fixo fundo)
{
    int n = luz - INTEIRO(fundo) / 80;
    return n < 0 ? 0 : (n > 15 ? 15 : n);
}

/* Teto (ou céu) e chão de uma sala numa coluna, entre as linhas y0 e y1. */
static void pintar_plano(const sala_t *s, int x, int y0, int y1, bool de_cima)
{
    if (y0 > y1) return;
    fixo h = de_cima ? s->teto : s->chao;
    int tex = de_cima ? s->forro : s->piso;
    if (tex == T_CEU) {
        for (int y = y0; y <= y1; y++)
            vista[y][x] = (HORIZONTE - y <= arvores[x]) ? cor_arvores[x] : ceu[y];
        return;
    }
    fixo altura = h - cam->z;
    const uint8_t (*t)[TEX] = texturas[tex];
    for (int y = y0; y <= y1; y++) {
        fixo inv = inverso[y];
        /* O plano só aparece do lado certo do horizonte: um teto acima do
         * olho em cima, um chão abaixo do olho embaixo. */
        if ((altura > 0) != (inv > 0)) { vista[y][x] = sombra[0][1]; continue; }
        fixo fundo = fmul(altura, inv);
        fixo wx = cam->x + fmul(fundo, dir_x[x]);
        fixo wy = cam->y + fmul(fundo, dir_y[x]);
        int c = t[(wy >> 17) & (TEX - 1)][(wx >> 17) & (TEX - 1)];
        vista[y][x] = sombra[nivel_de_luz(s->luz, fundo)][c];
    }
}

/* Um pedaço de parede numa coluna: linhas y0..y1, textura alinhada pelo
 * alto do pedaço (altura "alto"), posição u ao longo da parede. */
static void pintar_parede(int tex, int luz, int x, int y0, int y1, fixo alto, fixo u, fixo fundo)
{
    if (y0 > y1) return;
    const uint8_t (*t)[TEX] = texturas[tex];
    int tu = (u >> 17) & (TEX - 1);
    const uint32_t *cor = sombra[nivel_de_luz(luz, fundo)];
    /* Altura (no mundo) do centro da linha y0, e quanto ela desce por linha. */
    fixo passo = fundo / FOCO;
    fixo z = cam->z + fmul(FX(HORIZONTE) - FX(y0) - UM / 2, passo);
    for (int y = y0; y <= y1; y++) {
        fixo v = alto - z;
        vista[y][x] = cor[t[(v >> 17) & (TEX - 1)][tu]];
        z -= passo;
    }
}

static void desenhar_sala(int si, int x0, int x1, int profundidade)
{
    if (x0 > x1 || profundidade > MAX_PROFUNDIDADE) return;
    const sala_t *s = &mapa.salas[si];
    for (int x = x0; x <= x1; x++)
        if (fresta_topo[x] <= fresta_fundo[x])
            fresta_da_sala[si][x] = (int32_t)((uint16_t)fresta_topo[x] | ((uint32_t)(uint16_t)fresta_fundo[x] << 16));

    for (int l = 0; l < s->n; l++) {
        const lado_t *d = &s->lados[l];
        if (distancia_ao_lado(s, l, cam->x, cam->y) <= 0) continue;   /* de costas para o sagui */
        /* De dentro da sala, o canto b fica à esquerda na tela e o a à direita. */
        const ponto_t *pe = &mapa.pontos[d->b], *pd = &mapa.pontos[d->a];
        fixo dxe = pe->x - cam->x, dye = pe->y - cam->y, dxd = pd->x - cam->x, dyd = pd->y - cam->y;
        fixo fe = fmul(dxe, cs) + fmul(dye, sn), le = fmul(dxe, sn) - fmul(dye, cs);
        fixo fd = fmul(dxd, cs) + fmul(dyd, sn), ld = fmul(dxd, sn) - fmul(dyd, cs);
        if (fe < PERTO && fd < PERTO) continue;   /* todo atrás do olho */
        /* Corta o pedaço atrás do olho para achar as colunas que o lado ocupa. */
        fixo cfe = fe, cle = le, cfd = fd, cld = ld;
        if (cfe < PERTO) { fixo t = fdiv(PERTO - fe, fd - fe); cle = le + fmul(t, ld - le); cfe = PERTO; }
        if (cfd < PERTO) { fixo t = fdiv(PERTO - fd, fe - fd); cld = ld + fmul(t, le - ld); cfd = PERTO; }
        fixo xe = coluna_do_ponto(cle, cfe);
        fixo xd = coluna_do_ponto(cld, cfd);
        int ca = teto_fixo(xe - UM / 2), cb = teto_fixo(xd - UM / 2) - 1;
        if (ca < x0) ca = x0;
        if (cb > x1) cb = x1;
        if (ca > cb) continue;

        bool passagem = d->vizinha >= 0 && !d->secreta;
        const sala_t *n = passagem ? &mapa.salas[d->vizinha] : NULL;
        int abertas = 0;
        for (int x = ca; x <= cb; x++) {
            int topo = fresta_topo[x], fundo = fresta_fundo[x];
            if (topo > fundo) continue;
            /* Raio da coluna contra o segmento (do canto esquerdo ao direito). */
            fixo k = raio[x];
            fixo num = fmul(k, fe) - le;
            fixo den = (ld - le) - fmul(k, fd - fe);
            fixo t = den ? fdiv(num, den) : 0;
            if (t < 0) t = 0;
            if (t > UM) t = UM;
            fixo prof = fe + fmul(t, fd - fe);
            if (prof < PERTO) prof = PERTO;
            fixo u = fmul(t, d->comprimento);

            int lt = primeira_abaixo(linha_da_altura(s->teto, prof));
            int lc = primeira_abaixo(linha_da_altura(s->chao, prof));
            pintar_plano(s, x, topo, (lt - 1 < fundo ? lt - 1 : fundo), true);
            pintar_plano(s, x, (lc > topo ? lc : topo), fundo, false);
            if (!passagem) {
                pintar_parede(d->textura, s->luz, x, lt > topo ? lt : topo, lc - 1 < fundo ? lc - 1 : fundo, s->teto, u, prof);
                fresta_topo[x] = 1;
                fresta_fundo[x] = 0;
                continue;
            }
            int nt = lt, nc = lc;
            if (n->teto < s->teto) {
                nt = primeira_abaixo(linha_da_altura(n->teto, prof));
                pintar_parede(d->textura, s->luz, x, lt > topo ? lt : topo, nt - 1 < fundo ? nt - 1 : fundo, s->teto, u, prof);
            }
            if (n->chao > s->chao) {
                nc = primeira_abaixo(linha_da_altura(n->chao, prof));
                pintar_parede(d->textura, s->luz, x, nc > topo ? nc : topo, lc - 1 < fundo ? lc - 1 : fundo, n->chao, u, prof);
            }
            int novo_topo = nt > topo ? nt : topo, novo_fundo = nc - 1 < fundo ? nc - 1 : fundo;
            fresta_topo[x] = (int16_t)novo_topo;
            fresta_fundo[x] = (int16_t)novo_fundo;
            if (novo_topo <= novo_fundo) abertas++;
        }
        if (passagem && abertas) desenhar_sala(d->vizinha, ca, cb, profundidade + 1);
    }
}

/* ------------------------------------------------------------ coisas */

typedef struct {
    fixo fundo, lado, base;   /* base = altura do pé no mundo */
    int  figura, sala, luz;
    fixo largura, altura;     /* em palmos (fixo) */
    bool espelho;
} desenho_t;

static desenho_t lista[MAX_COISAS + 32];
static int qtd_lista;

static void anotar(fixo x, fixo y, fixo base, int sala, int figura, int larg, int alt, bool espelho)
{
    if (qtd_lista >= (int)(sizeof lista / sizeof lista[0]) || sala < 0) return;
    fixo dx = x - cam->x, dy = y - cam->y;
    fixo f = fmul(dx, cs) + fmul(dy, sn);
    if (f < FX(4)) return;
    desenho_t *d = &lista[qtd_lista++];
    d->fundo = f;
    d->lado = fmul(dx, sn) - fmul(dy, cs);
    d->base = base;
    d->sala = sala;
    d->figura = figura;
    d->largura = FX(larg);
    d->altura = FX(alt);
    d->espelho = espelho;
    d->luz = mapa.salas[sala].luz < 5 ? 5 : mapa.salas[sala].luz;   /* coisas nunca somem de todo no escuro */
}

static void pintar_coisa(const desenho_t *d)
{
    const figura_t *f = &figuras[d->figura];
    if (!f->px) return;
    fixo esc = fdiv(FX(FOCO), d->fundo);          /* pixels por palmo */
    fixo meio = coluna_do_ponto(d->lado, d->fundo);
    fixo larg = fmul(d->largura, esc), alt = fmul(d->altura, esc);
    if (larg < UM / 4 || alt < UM / 4) return;
    fixo esq = meio - larg / 2;
    fixo pe = linha_da_altura(d->base, d->fundo), cabeca = pe - alt;
    int x0 = primeira_abaixo(esq), x1 = primeira_abaixo(esq + larg) - 1;
    int y0 = primeira_abaixo(cabeca), y1 = primeira_abaixo(pe) - 1;
    if (x0 < 0) x0 = 0;
    if (x1 >= VL) x1 = VL - 1;
    const uint32_t *cor = sombra[nivel_de_luz(d->luz, d->fundo)];
    fixo passo_u = fdiv(FX(f->l), larg), passo_v = fdiv(FX(f->a), alt);
    for (int x = x0; x <= x1; x++) {
        int32_t fr = fresta_da_sala[d->sala][x];
        if (fr < 0) continue;
        int ftopo = (int16_t)(fr & 0xFFFF), ffundo = (int16_t)((uint32_t)fr >> 16);
        int u = INTEIRO(fmul(FX(x) + UM / 2 - esq, passo_u));
        if (u < 0) u = 0;
        if (u >= f->l) u = f->l - 1;
        if (d->espelho) u = f->l - 1 - u;
        int ya = y0 > ftopo ? y0 : ftopo, yb = y1 < ffundo ? y1 : ffundo;
        for (int y = ya; y <= yb; y++) {
            int v = INTEIRO(fmul(FX(y) + UM / 2 - cabeca, passo_v));
            if (v < 0 || v >= f->a) continue;
            uint8_t p = f->px[v * f->l + u];
            if (p) vista[y][x] = cor[p];
        }
    }
}

static void desenhar_coisas(int relogio)
{
    qtd_lista = 0;
    for (int i = 0; i < mapa.qtd_coisas; i++) {
        const coisa_t *c = &mapa.coisas[i];
        if (!c->ativa) continue;
        fixo chao = mapa.salas[c->sala].chao;
        switch (c->tipo) {
        case C_BANANA: {
            fixo pulo = seno(relogio * 40 + i * 500) * 3;     /* flutua devagar */
            anotar(c->x, c->y, chao + FX(6) + pulo, c->sala, F_BANANA, 14, 14, false);
            break;
        }
        case C_CACHO:
            anotar(c->x, c->y, chao + FX(6) + seno(relogio * 40) * 3, c->sala, F_CACHO, 22, 22, false);
            break;
        case C_POTE:     anotar(c->x, c->y, chao, c->sala, F_POTE, 18, 14, false); break;
        case C_CASCA:    anotar(c->x, c->y, chao, c->sala, F_CASCA, 16, 8, false); break;
        case C_PORTEIRA: anotar(c->x, c->y, chao, c->sala, porteira_aberta() ? F_PORTEIRA_ABERTA : F_PORTEIRA, 64, 80, false); break;
        case C_SEMENTE_VOANDO: anotar(c->x, c->y, c->z, c->sala, F_SEMENTE, 5, 5, false); break;
        case C_MACACO: {
            if (c->estado == M_FORA) break;
            bool tonto = c->estado == M_TONTO || c->estado == M_ESCORREGANDO;
            /* Escorregando, ele gira: a figura vira de lado a lado. */
            bool espelho = c->estado == M_ESCORREGANDO ? (relogio / 3) % 2 : (c->estado == M_FUGINDO);
            anotar(c->x, c->y, chao, c->sala, tonto ? F_MACACO_TONTO : F_MACACO, 30, 40, espelho);
            if (tonto)   /* estrelinhas rodando em volta da cabeça */
                for (int e = 0; e < 3; e++) {
                    int a = relogio * 120 + e * (VOLTA / 3);
                    anotar(c->x + seno(a + QUARTO) * 12, c->y + seno(a) * 12, chao + FX(42), c->sala, F_ESTRELA, 6, 6, false);
                }
            break;
        }
        }
    }
    /* De trás para a frente (ordenação por inserção: são poucas coisas). */
    for (int i = 1; i < qtd_lista; i++) {
        desenho_t t = lista[i];
        int j = i - 1;
        while (j >= 0 && lista[j].fundo < t.fundo) { lista[j + 1] = lista[j]; j--; }
        lista[j + 1] = t;
    }
    for (int i = 0; i < qtd_lista; i++) pintar_coisa(&lista[i]);
}

/* ------------------------------------------------------------ quadro */

void tela3d_desenhar(const camera_t *c, int relogio)
{
    cam = c;
    cs = cosseno(c->ang);
    sn = seno(c->ang);
    for (int x = 0; x < VL; x++) {
        fresta_topo[x] = 0;
        fresta_fundo[x] = VA - 1;
        raio[x] = (FX(x - VL / 2) + UM / 2) / FOCO;
        /* Ponto a fundo 1 na coluna x: frente + raio * direita, com direita = (sn, -cs). */
        dir_x[x] = cs + fmul(raio[x], sn);
        dir_y[x] = sn - fmul(raio[x], cs);
        int a = c->ang - (x - VL / 2) * 3;   /* ~3 passos de ângulo por coluna */
        arvores[x] = altura_arvores(a);
        cor_arvores[x] = cor_arvore(a);
    }
    for (int y = 0; y < VA; y++) {
        fixo d = FX(HORIZONTE) - FX(y) - UM / 2;
        inverso[y] = fdiv(FX(FOCO), d);
        ceu[y] = y < HORIZONTE ? degrade_ceu(y) : 0;
    }
    for (int s = 0; s < mapa.qtd_salas; s++)
        for (int x = 0; x < VL; x++) fresta_da_sala[s][x] = -1;
    if (c->sala >= 0) desenhar_sala(c->sala, 0, VL - 1, 0);
    /* Colunas que nada fechou (não deveria acontecer): pinta de escuro. */
    for (int x = 0; x < VL; x++)
        for (int y = fresta_topo[x]; y <= fresta_fundo[x]; y++) vista[y][x] = sombra[0][1];
    desenhar_coisas(relogio);
}
