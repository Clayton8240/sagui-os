/* jogo.h - Sagui 3D (bônus B4): tipos e constantes compartilhados.
 *
 * O jogo é dividido em arquivos pequenos, cada um com um assunto:
 *   contas.c   ponto fixo e a tabela de seno (o programa não tem números
 *              de ponto flutuante: -mgeneral-regs-only)
 *   arte.c     paleta, texturas e personagens (pixel art feita aqui)
 *   mapa.c     leitura do arquivo de fase e consultas geométricas
 *   tela3d.c   o desenho em perspectiva
 *   mundo.c    regras: movimento, macacos-prego, sementes, bananas
 *   sagui3d.c  janela, laço principal, telas, barra de status e sons
 * Ver a seção "Sagui 3D" em docs/decisoes.md. */
#ifndef SAGUI3D_JOGO_H
#define SAGUI3D_JOGO_H
#include "sagui.h"
#include "gui.h"

/* ------------------------------------------------------------ ponto fixo */
/* "fixo": número com 16 bits inteiros e 16 de fração. 1,0 = 65536.
 * As coordenadas do mapa são em "palmos": o sagui tem o olho a 40 palmos
 * do chão e uma sala comum tem 128 de altura. */
typedef int32_t fixo;
#define UM        65536
#define FX(n)     ((fixo)((n) * UM))
#define INTEIRO(f) ((f) >> 16)

static inline fixo fmul(fixo a, fixo b) { return (fixo)(((int64_t)a * b) >> 16); }
fixo muldiv(fixo a, fixo b, fixo c);   /* a * b / c com 64 bits no meio, saturado */
fixo fdiv(fixo a, fixo b);             /* a / b em ponto fixo */
int32_t raiz(uint32_t n);              /* raiz quadrada inteira */

/* Ângulos: 4096 passos por volta. 0 = leste, 1024 = norte (y cresce para o norte). */
#define VOLTA      4096
#define QUARTO     1024
#define ANG(a)     ((a) & (VOLTA - 1))
void  contas_iniciar(void);
fixo  seno(int a);
fixo  cosseno(int a);
int   angulo_de(fixo dx, fixo dy);     /* direção de um vetor */
uint32_t sorteio(void);                /* números pseudoaleatórios do jogo */

/* ------------------------------------------------------------ tela */
#define VL 320           /* a vista 3D é desenhada em 320x200 ... */
#define VA 200
#define ESCALA 2         /* ... e ampliada 2x na janela */
#define BARRA 48         /* barra de status embaixo */
#define JAN_L (VL * ESCALA)
#define JAN_A (VA * ESCALA + BARRA)
#define FOCO 176         /* distância focal em pixels: ~85 graus de visão */
#define HORIZONTE (VA / 2)
extern uint32_t vista[VA][VL];

/* ------------------------------------------------------------ arte */
#define TEX 32                          /* texturas de 32x32 */
enum { T_PEDRA, T_CIPO, T_MADEIRA, T_GRAMA, T_TERRA, T_LAJOTA, T_FOLHAS, T_ROCHA, T_CEU, QTD_TEXTURAS };
extern uint8_t texturas[QTD_TEXTURAS][TEX][TEX];   /* índices da paleta */
#define NIVEIS_LUZ 16
extern uint32_t sombra[NIVEIS_LUZ][96];            /* cor de cada índice em cada nível de luz */
int  textura_por_nome(const char *nome);           /* -1 se não existe */

typedef struct {
    int l, a;
    const uint8_t *px;     /* l*a índices; 0 = transparente */
} figura_t;
enum { F_MACACO, F_MACACO_TONTO, F_BANANA, F_CACHO, F_CASCA, F_SEMENTE, F_POTE,
       F_PORTEIRA, F_PORTEIRA_ABERTA, F_ESTRELA, F_CORACAO, F_ESTILINGUE, F_ESTILINGUE_PUXADO,
       F_SAGUI, QTD_FIGURAS };
extern figura_t figuras[QTD_FIGURAS];
extern const uint32_t paleta[96];
void arte_iniciar(void);
int      altura_arvores(int angulo);
uint32_t cor_arvore(int angulo);
uint32_t degrade_ceu(int y);

/* ------------------------------------------------------------ mapa */
#define MAX_PONTOS  256
#define MAX_SALAS   64
#define MAX_LADOS   12      /* lados por sala */
#define MAX_COISAS  96

typedef struct { fixo x, y; } ponto_t;

typedef struct {
    int  a, b;          /* índices dos pontos; a sala fica à esquerda de a->b */
    int  vizinha;       /* -1 = parede */
    bool secreta;       /* parece parede, mas dá para atravessar */
    int  textura;
    fixo comprimento;
    fixo nx, ny;        /* normal de comprimento 1 apontando para dentro da sala */
} lado_t;

typedef struct {
    int    n;
    lado_t lados[MAX_LADOS];
    fixo   chao, teto;
    int    luz;            /* 0 (breu) a 15 (sol) */
    int    piso, forro;    /* texturas; forro T_CEU = céu aberto */
} sala_t;

enum { C_BANANA, C_CACHO, C_POTE, C_PORTEIRA, C_CASCA, C_MACACO, C_SEMENTE_VOANDO };

typedef struct {
    int  tipo;
    bool ativa;
    fixo x, y;
    int  sala;
    /* macacos */
    int  estado, tempo, acertos, ang, desvio;
    fixo alvo_x, alvo_y;
    bool tem_alvo;
    /* sementes voando */
    fixo vx, vy, z;
} coisa_t;

typedef struct {
    char    nome[48];
    int     minimo;                /* bananas para abrir a porteira */
    int     qtd_pontos, qtd_salas, qtd_coisas;
    ponto_t pontos[MAX_PONTOS];
    sala_t  salas[MAX_SALAS];
    coisa_t coisas[MAX_COISAS];
    fixo    inicio_x, inicio_y;
    int     inicio_ang, inicio_sala;
    int     total_bananas, passagens, paredes;
} mapa_t;

extern mapa_t mapa;
bool mapa_carregar(const char *caminho, char *erro, int tam_erro);
int  sala_do_ponto(fixo x, fixo y, int palpite);   /* -1 = fora do mapa */
bool dentro_da_sala(int s, fixo x, fixo y);
/* Distância (com sinal: positiva dentro) do ponto à reta do lado. */
fixo distancia_ao_lado(const sala_t *s, int l, fixo x, fixo y);
bool linha_livre(int sala, fixo ax, fixo ay, fixo bx, fixo by, fixo altura);

/* ------------------------------------------------------------ desenho 3D */
typedef struct {
    fixo x, y, z;      /* z = altura do olho */
    int  ang;
    int  sala;
} camera_t;
void tela3d_desenhar(const camera_t *c, int relogio);

/* ------------------------------------------------------------ mundo */
#define TIQUE_MS 30                /* a lógica anda em passos de 30 ms */
#define OLHO     40                /* altura do olho acima do chão */
#define ALTURA_SAGUI 56
#define DEGRAU   24                /* o maior degrau que dá para subir */
#define RAIO_SAGUI  14
#define RAIO_MACACO 16

enum { M_QUIETO, M_CACANDO, M_PARADO, M_RECUANDO, M_TONTO, M_ESCORREGANDO, M_FUGINDO, M_FORA };

typedef struct {
    fixo x, y, z;
    int  ang, sala;
    int  vidas, bananas, comidas, cascas, sementes, pontos;
    int  invulneravel, recarga, puxado;
    bool cacho;
    int  espantados;
} sagui_t;

extern sagui_t sagui;
extern int  fase_atual;
extern char aviso[64];
extern int  aviso_tempo;

typedef struct {
    bool frente, tras, esquerda, direita, atirar, correr;
    int  giro_mouse;
} comandos_t;

enum { R_NADA, R_FASE_CONCLUIDA, R_FIM_DE_JOGO };
void mundo_iniciar_fase(void);
int  mundo_tique(const comandos_t *cmd);
void mundo_largar_casca(void);
bool porteira_aberta(void);
void mundo_mostrar_aviso(const char *t);

/* ------------------------------------------------------------ sons (bip) */
void som_tiro(void);
void som_banana(void);
void som_acerto(void);
void som_vida_perdida(void);
void som_fase_concluida(void);
void som_casca(void);
void som_escorregou(void);
void som_cacho(void);
void som_porteira_fechada(void);

#endif
