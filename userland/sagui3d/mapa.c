/* mapa.c - arquivo de fase do Sagui 3D e consultas geométricas.
 *
 * Formato (texto, uma instrução por linha, # começa comentário):
 *
 *   nome A clareira de pedra
 *   minimo 5                         bananas para abrir a porteira
 *   ponto 1 0 0                      ponto número 1 em x=0, y=0 (palmos)
 *   sala chao 0 teto 160 luz 15 piso grama forro ceu parede pedra : 1 2 3 4
 *   lado 3 4 cipo                    textura própria da parede entre 3 e 4
 *   secreta 5 6                      passagem que parece parede de cipó
 *   sagui 64 128 0                   começo: x, y e direção em graus (0 = leste, 90 = norte)
 *   banana 200 100   cacho 30 40   pote 10 20   porteira 900 500   macaco 600 120
 *
 * Cada sala é um polígono convexo dado pelos seus cantos. Quando duas salas
 * usam os mesmos dois pontos num lado, esse lado vira passagem entre elas
 * (com degrau, se o chão ou o teto forem diferentes); um lado que só uma
 * sala usa é parede. Sala convexa é a escolha que simplifica tudo: de
 * dentro dela, cada direção bate em exatamente um lado, e um ponto está
 * dentro se estiver do lado de dentro de todos os lados. Formas côncavas
 * (um L, uma sala com pilar) são feitas juntando salas convexas. */
#include "jogo.h"

mapa_t mapa;

/* ------------------------------------------------------------ geometria */

fixo distancia_ao_lado(const sala_t *s, int l, fixo x, fixo y)
{
    const lado_t *d = &s->lados[l];
    const ponto_t *a = &mapa.pontos[d->a];
    return fmul(x - a->x, d->nx) + fmul(y - a->y, d->ny);
}

bool dentro_da_sala(int si, fixo x, fixo y)
{
    const sala_t *s = &mapa.salas[si];
    for (int l = 0; l < s->n; l++)
        if (distancia_ao_lado(s, l, x, y) < -UM / 16) return false;
    return true;
}

int sala_do_ponto(fixo x, fixo y, int palpite)
{
    if (palpite >= 0 && palpite < mapa.qtd_salas) {
        if (dentro_da_sala(palpite, x, y)) return palpite;
        const sala_t *s = &mapa.salas[palpite];
        for (int l = 0; l < s->n; l++)
            if (s->lados[l].vizinha >= 0 && dentro_da_sala(s->lados[l].vizinha, x, y))
                return s->lados[l].vizinha;
    }
    for (int i = 0; i < mapa.qtd_salas; i++)
        if (dentro_da_sala(i, x, y)) return i;
    return -1;
}

/* Dá para ver (ou atirar) de A até B em linha reta, na altura dada?
 * Anda de sala em sala pelo segmento: dentro de uma sala convexa, o
 * segmento sai pelo primeiro lado que ele cruza de dentro para fora (o de
 * menor fração t = dA / (dA - dB) entre os lados com B do lado de fora).
 * Se esse lado é parede, o caminho está fechado; se é passagem, a altura
 * precisa caber no vão entre os dois chãos e os dois tetos. */
bool linha_livre(int si, fixo ax, fixo ay, fixo bx, fixo by, fixo altura)
{
    for (int volta = 0; volta < 32 && si >= 0; volta++) {
        if (dentro_da_sala(si, bx, by)) return true;
        const sala_t *s = &mapa.salas[si];
        int saida = -1;
        fixo melhor = 0x7FFFFFFF;
        for (int l = 0; l < s->n; l++) {
            fixo da = distancia_ao_lado(s, l, ax, ay), db = distancia_ao_lado(s, l, bx, by);
            if (db >= 0 || da < 0) continue;
            fixo t = fdiv(da, da - db);
            if (t < melhor) { melhor = t; saida = l; }
        }
        if (saida < 0) return false;
        const lado_t *d = &s->lados[saida];
        if (d->vizinha < 0 || d->secreta) return false;
        const sala_t *n = &mapa.salas[d->vizinha];
        fixo chao = s->chao > n->chao ? s->chao : n->chao;
        fixo teto = s->teto < n->teto ? s->teto : n->teto;
        if (altura <= chao || altura >= teto) return false;
        si = d->vizinha;
    }
    return false;
}

/* ------------------------------------------------------------ leitura */

static int ids[MAX_PONTOS];
static struct { int a, b, textura; bool secreta; } ajustes[64];
static int qtd_ajustes;

static int ponto_por_id(int id)
{
    for (int i = 0; i < mapa.qtd_pontos; i++)
        if (ids[i] == id) return i;
    return -1;
}

/* Separa a linha em palavras (no próprio buffer). */
static int palavras(char *linha, char **p, int max)
{
    int n = 0;
    while (*linha && n < max) {
        while (*linha == ' ' || *linha == '\t' || *linha == '\r') *linha++ = 0;
        if (!*linha) break;
        p[n++] = linha;
        while (*linha && *linha != ' ' && *linha != '\t' && *linha != '\r') linha++;
    }
    return n;
}

static bool numero(const char *s, int *v)
{
    const char *t = s;
    if (*t == '-') t++;
    if (*t < '0' || *t > '9') return false;
    for (; *t; t++) if (*t < '0' || *t > '9') return false;
    *v = texto_para_int(s);
    return true;
}

#define ERRO(...) do { formatar_texto(erro, (size_t)tam_erro, __VA_ARGS__); return false; } while (0)

static bool coisa_nova(int tipo, int x, int y, char *erro, int tam_erro, int linha)
{
    if (mapa.qtd_coisas >= MAX_COISAS) ERRO("linha %d: coisas demais", linha);
    coisa_t *c = &mapa.coisas[mapa.qtd_coisas++];
    memset(c, 0, sizeof *c);
    c->tipo = tipo;
    c->ativa = true;
    c->x = FX(x);
    c->y = FX(y);
    c->sala = -1;
    return true;
}

static bool ler_sala(char **p, int n, char *erro, int tam_erro, int linha)
{
    if (mapa.qtd_salas >= MAX_SALAS) ERRO("linha %d: salas demais", linha);
    sala_t *s = &mapa.salas[mapa.qtd_salas];
    memset(s, 0, sizeof *s);
    int chao = 0, teto = 128, luz = 12, piso = T_LAJOTA, forro = T_ROCHA, parede = T_PEDRA;
    int i = 1;
    for (; i + 1 < n && compara_texto(p[i], ":") != 0; i += 2) {
        int v = 0;
        if (compara_texto(p[i], "chao") == 0 && numero(p[i + 1], &v)) chao = v;
        else if (compara_texto(p[i], "teto") == 0 && numero(p[i + 1], &v)) teto = v;
        else if (compara_texto(p[i], "luz") == 0 && numero(p[i + 1], &v)) luz = v;
        else if (compara_texto(p[i], "piso") == 0) piso = textura_por_nome(p[i + 1]);
        else if (compara_texto(p[i], "forro") == 0) forro = textura_por_nome(p[i + 1]);
        else if (compara_texto(p[i], "parede") == 0) parede = textura_por_nome(p[i + 1]);
        else ERRO("linha %d: não entendi \"%s %s\"", linha, p[i], p[i + 1]);
        if (piso < 0 || forro < 0 || parede < 0) ERRO("linha %d: textura desconhecida \"%s\"", linha, p[i + 1]);
    }
    if (i >= n || compara_texto(p[i], ":") != 0) ERRO("linha %d: falta o \":\" antes dos pontos da sala", linha);
    int cantos[MAX_LADOS], qtd = 0;
    for (i++; i < n; i++) {
        int id;
        if (!numero(p[i], &id)) ERRO("linha %d: ponto \"%s\" inválido", linha, p[i]);
        if (qtd >= MAX_LADOS) ERRO("linha %d: a sala tem mais de %d cantos", linha, MAX_LADOS);
        int k = ponto_por_id(id);
        if (k < 0) ERRO("linha %d: o ponto %d não foi definido", linha, id);
        cantos[qtd++] = k;
    }
    if (qtd < 3) ERRO("linha %d: uma sala precisa de pelo menos 3 cantos", linha);
    if (teto - chao < 16) ERRO("linha %d: sala baixa demais", linha);
    if (luz < 0) luz = 0;
    if (luz > 15) luz = 15;

    /* Se os cantos vieram no sentido horário, inverte: a sala sempre fica à
     * esquerda de cada lado (a conta da área com sinal diz o sentido). */
    int64_t area = 0;
    for (int k = 0; k < qtd; k++) {
        const ponto_t *a = &mapa.pontos[cantos[k]], *b = &mapa.pontos[cantos[(k + 1) % qtd]];
        area += (int64_t)INTEIRO(a->x) * INTEIRO(b->y) - (int64_t)INTEIRO(b->x) * INTEIRO(a->y);
    }
    if (area < 0)
        for (int k = 0; k < qtd / 2; k++) { int t = cantos[k]; cantos[k] = cantos[qtd - 1 - k]; cantos[qtd - 1 - k] = t; }

    for (int k = 0; k < qtd; k++) {
        lado_t *d = &s->lados[k];
        d->a = cantos[k];
        d->b = cantos[(k + 1) % qtd];
        d->vizinha = -1;
        d->textura = parede;
        const ponto_t *a = &mapa.pontos[d->a], *b = &mapa.pontos[d->b];
        int dx = INTEIRO(b->x - a->x), dy = INTEIRO(b->y - a->y);
        int comp = raiz((uint32_t)(dx * dx + dy * dy));
        if (comp == 0) ERRO("linha %d: dois cantos seguidos no mesmo lugar", linha);
        d->comprimento = FX(comp);
        d->nx = -dy * UM / comp;    /* normal para a esquerda = para dentro */
        d->ny = dx * UM / comp;
        /* Convexidade: o canto seguinte não pode ficar à direita deste lado. */
        const ponto_t *c = &mapa.pontos[cantos[(k + 2) % qtd]];
        int64_t giro = (int64_t)dx * INTEIRO(c->y - b->y) - (int64_t)dy * INTEIRO(c->x - b->x);
        if (giro < 0) ERRO("linha %d: a sala não é convexa (no ponto %d)", linha, ids[d->b]);
    }
    s->n = qtd;
    s->chao = FX(chao);
    s->teto = FX(teto);
    s->luz = luz;
    s->piso = piso;
    s->forro = forro;
    mapa.qtd_salas++;
    return true;
}

static bool interpretar(char *linha, int num, char *erro, int tam_erro)
{
    char *p[40];
    for (char *c = linha; *c; c++) if (*c == '#') { *c = 0; break; }
    int n = palavras(linha, p, 40);
    if (n == 0) return true;
    int v[3] = { 0, 0, 0 };
    for (int i = 1; i < n && i <= 3; i++) numero(p[i], &v[i - 1]);
    const char *cmd = p[0];
    if (compara_texto(cmd, "nome") == 0) {
        mapa.nome[0] = 0;
        for (int i = 1; i < n; i++) {
            if (i > 1) copia_texto(mapa.nome + tam_texto(mapa.nome), " ", sizeof mapa.nome - tam_texto(mapa.nome));
            copia_texto(mapa.nome + tam_texto(mapa.nome), p[i], sizeof mapa.nome - tam_texto(mapa.nome));
        }
    } else if (compara_texto(cmd, "minimo") == 0 && n == 2) {
        mapa.minimo = v[0];
    } else if (compara_texto(cmd, "ponto") == 0 && n == 4) {
        if (mapa.qtd_pontos >= MAX_PONTOS) ERRO("linha %d: pontos demais", num);
        if (ponto_por_id(v[0]) >= 0) ERRO("linha %d: o ponto %d já existe", num, v[0]);
        ids[mapa.qtd_pontos] = v[0];
        mapa.pontos[mapa.qtd_pontos].x = FX(v[1]);
        mapa.pontos[mapa.qtd_pontos].y = FX(v[2]);
        mapa.qtd_pontos++;
    } else if (compara_texto(cmd, "sala") == 0) {
        return ler_sala(p, n, erro, tam_erro, num);
    } else if ((compara_texto(cmd, "lado") == 0 && n == 4) || (compara_texto(cmd, "secreta") == 0 && n == 3)) {
        if (qtd_ajustes >= 64) ERRO("linha %d: ajustes demais", num);
        int a = ponto_por_id(v[0]), b = ponto_por_id(v[1]);
        if (a < 0 || b < 0) ERRO("linha %d: ponto não definido", num);
        ajustes[qtd_ajustes].a = a;
        ajustes[qtd_ajustes].b = b;
        ajustes[qtd_ajustes].secreta = cmd[0] == 's';
        ajustes[qtd_ajustes].textura = cmd[0] == 's' ? T_CIPO : textura_por_nome(p[3]);
        if (ajustes[qtd_ajustes].textura < 0) ERRO("linha %d: textura desconhecida", num);
        qtd_ajustes++;
    } else if (compara_texto(cmd, "sagui") == 0 && n == 4) {
        mapa.inicio_x = FX(v[0]);
        mapa.inicio_y = FX(v[1]);
        mapa.inicio_ang = ANG(v[2] * VOLTA / 360);
    } else if (compara_texto(cmd, "banana") == 0 && n == 3) {
        mapa.total_bananas++;
        return coisa_nova(C_BANANA, v[0], v[1], erro, tam_erro, num);
    } else if (compara_texto(cmd, "cacho") == 0 && n == 3) {
        return coisa_nova(C_CACHO, v[0], v[1], erro, tam_erro, num);
    } else if (compara_texto(cmd, "pote") == 0 && n == 3) {
        return coisa_nova(C_POTE, v[0], v[1], erro, tam_erro, num);
    } else if (compara_texto(cmd, "porteira") == 0 && n == 3) {
        return coisa_nova(C_PORTEIRA, v[0], v[1], erro, tam_erro, num);
    } else if (compara_texto(cmd, "macaco") == 0 && n == 3) {
        return coisa_nova(C_MACACO, v[0], v[1], erro, tam_erro, num);
    } else {
        ERRO("linha %d: não entendi \"%s\"", num, cmd);
    }
    return true;
}

bool mapa_carregar(const char *caminho, char *erro, int tam_erro)
{
    static char texto[16384];
    memset(&mapa, 0, sizeof mapa);
    qtd_ajustes = 0;
    int fd = abrir(caminho, ABRIR_LER);
    if (fd < 0) ERRO("não consegui abrir %s", caminho);
    int n = ler(fd, texto, sizeof texto - 1);
    fechar(fd);
    if (n <= 0) ERRO("%s está vazio", caminho);
    texto[n] = 0;

    int num = 1;
    for (char *linha = texto; *linha; num++) {
        char *fim = linha;
        while (*fim && *fim != '\n') fim++;
        bool ultima = !*fim;
        *fim = 0;
        if (!interpretar(linha, num, erro, tam_erro)) return false;
        if (ultima) break;
        linha = fim + 1;
    }
    if (mapa.qtd_salas == 0) ERRO("a fase não tem salas");

    /* Passagens: o lado a->b de uma sala casa com o lado b->a de outra. */
    for (int i = 0; i < mapa.qtd_salas; i++) {
        sala_t *s = &mapa.salas[i];
        for (int l = 0; l < s->n; l++) {
            lado_t *d = &s->lados[l];
            for (int j = 0; j < mapa.qtd_salas && d->vizinha < 0; j++) {
                if (j == i) continue;
                const sala_t *t = &mapa.salas[j];
                for (int k = 0; k < t->n; k++)
                    if (t->lados[k].a == d->b && t->lados[k].b == d->a) { d->vizinha = j; break; }
            }
            if (d->vizinha >= 0) mapa.passagens++; else mapa.paredes++;
        }
    }
    mapa.passagens /= 2;   /* cada passagem foi contada pelas duas salas */
    for (int k = 0; k < qtd_ajustes; k++) {
        bool achou = false;
        for (int i = 0; i < mapa.qtd_salas; i++)
            for (int l = 0; l < mapa.salas[i].n; l++) {
                lado_t *d = &mapa.salas[i].lados[l];
                if ((d->a == ajustes[k].a && d->b == ajustes[k].b) || (d->a == ajustes[k].b && d->b == ajustes[k].a)) {
                    d->textura = ajustes[k].textura;
                    if (ajustes[k].secreta) {
                        if (d->vizinha < 0) ERRO("secreta %d %d: esse lado não é passagem", ids[ajustes[k].a], ids[ajustes[k].b]);
                        d->secreta = true;
                    }
                    achou = true;
                }
            }
        if (!achou) ERRO("lado %d %d: nenhuma sala tem esse lado", ids[ajustes[k].a], ids[ajustes[k].b]);
    }

    mapa.inicio_sala = sala_do_ponto(mapa.inicio_x, mapa.inicio_y, -1);
    if (mapa.inicio_sala < 0) ERRO("o sagui começa fora de todas as salas");
    for (int i = 0; i < mapa.qtd_coisas; i++) {
        coisa_t *c = &mapa.coisas[i];
        c->sala = sala_do_ponto(c->x, c->y, -1);
        if (c->sala < 0) ERRO("uma coisa em (%d, %d) está fora das salas", INTEIRO(c->x), INTEIRO(c->y));
    }
    if (mapa.minimo > mapa.total_bananas) mapa.minimo = mapa.total_bananas;
    if (!mapa.nome[0]) copia_texto(mapa.nome, "Fase sem nome", sizeof mapa.nome);
    return true;
}
