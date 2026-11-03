/* contas.c - ponto fixo, seno e ângulos do Sagui 3D.
 *
 * Os programas do SaguiOS não usam números de ponto flutuante (a FPU não é
 * salva na troca de processos), então tudo é inteiro: "fixo" tem 16 bits
 * de fração. Multiplicar dois fixos dá 32 bits de fração, que cabem num
 * inteiro de 64 bits; o resultado volta a 16 com um deslocamento.
 * Dividir 64 por 32 bits pediria uma função da biblioteca do compilador,
 * que não usamos: a instrução "idiv" do x86 faz exatamente isso (EDX:EAX
 * dividido por um registrador), desde que o quociente caiba em 32 bits;
 * por isso o muldiv confere antes e satura. */
#include "jogo.h"

#define MAXIMO 0x7FFFFFFF

fixo muldiv(fixo a, fixo b, fixo c)
{
    int64_t p = (int64_t)a * b;
    bool negativo = (p < 0) != (c < 0);
    if (c == 0) return negativo ? -MAXIMO : MAXIMO;
    uint64_t ap = p < 0 ? (uint64_t)(-p) : (uint64_t)p;
    uint32_t ac = c < 0 ? (uint32_t)(-(int64_t)c) : (uint32_t)c;
    if ((ap >> 31) >= ac) return negativo ? -MAXIMO : MAXIMO;   /* não caberia */
    uint32_t lo = (uint32_t)p, hi = (uint32_t)((uint64_t)p >> 32);
    int32_t q, r;
    __asm__("idivl %4" : "=a"(q), "=d"(r) : "0"(lo), "1"(hi), "rm"(c) : "cc");
    (void)r;
    return q;
}

fixo fdiv(fixo a, fixo b) { return muldiv(a, UM, b); }

int32_t raiz(uint32_t n)
{
    /* Método de ir acertando um bit por vez, do mais alto para o mais baixo. */
    uint32_t r = 0;
    for (uint32_t bit = 1u << 15; bit; bit >>= 1) {
        uint32_t t = r | bit;
        if (t * t <= n) r = t;
    }
    return (int32_t)r;
}

/* ------------------------------------------------------------ seno */

/* Tabela de um quarto de volta (0 a 90 graus, 1025 valores), com 30 bits de
 * fração durante a conta. Começa no vetor (1, 0) e gira um passo de cada vez
 * pela rotação de um ângulo pequeno d:
 *     x' = x cos d - y sin d,    y' = x sin d + y cos d
 * cos d e sin d (d = 2pi/4096) foram calculados à mão uma vez e entram aqui
 * como constantes. O erro acumulado em 1024 passos fica abaixo de 1/65536,
 * que é a precisão do fixo. O resto da volta sai por simetria. */
static fixo quarto[QUARTO + 1];

void contas_iniciar(void)
{
    const int64_t COS_D = 1073740561;   /* cos(2pi/4096) * 2^30 */
    const int64_t SEN_D = 1647099;      /* sen(2pi/4096) * 2^30 */
    int64_t x = (int64_t)1 << 30, y = 0;
    for (int i = 0; i <= QUARTO; i++) {
        quarto[i] = (fixo)((y + (1 << 13)) >> 14);
        int64_t nx = (x * COS_D - y * SEN_D + ((int64_t)1 << 29)) >> 30;
        int64_t ny = (x * SEN_D + y * COS_D + ((int64_t)1 << 29)) >> 30;
        x = nx;
        y = ny;
    }
    quarto[QUARTO] = UM;
}

fixo seno(int a)
{
    a = ANG(a);
    if (a <= QUARTO) return quarto[a];
    if (a <= 2 * QUARTO) return quarto[2 * QUARTO - a];
    if (a <= 3 * QUARTO) return -quarto[a - 2 * QUARTO];
    return -quarto[VOLTA - a];
}

fixo cosseno(int a) { return seno(a + QUARTO); }

/* Ângulo de um vetor: reduz ao primeiro oitavo (0 a 45 graus) trocando
 * sinais e eixos, acha o ângulo cuja tangente é |menor|/|maior| por busca
 * binária na tabela e desfaz as trocas. */
int angulo_de(fixo dx, fixo dy)
{
    if (dx == 0 && dy == 0) return 0;
    fixo ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
    bool troca = ay > ax;
    fixo t = troca ? fdiv(ax, ay) : fdiv(ay, ax);   /* 0 a 1 */
    int lo = 0, hi = QUARTO / 2;
    while (lo < hi) {
        int meio = (lo + hi + 1) / 2;
        fixo tg = fdiv(quarto[meio], quarto[QUARTO - meio]);
        if (tg <= t) lo = meio; else hi = meio - 1;
    }
    int a = troca ? QUARTO - lo : lo;                /* primeiro quadrante */
    if (dx < 0) a = 2 * QUARTO - a;
    if (dy < 0) a = VOLTA - a;
    return ANG(a);
}

/* ------------------------------------------------------------ sorteio */
static uint32_t estado_sorteio = 0x5A6B1C2Du;

uint32_t sorteio(void)
{
    /* Embaralhamento por deslocamentos e ou-exclusivo: rápido e suficiente
     * para escolher para onde um macaco olha. */
    uint32_t x = estado_sorteio;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    estado_sorteio = x;
    return x;
}
