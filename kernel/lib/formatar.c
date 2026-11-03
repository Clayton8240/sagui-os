/* formatar.c - formatação de texto no estilo printf, escrita para o SaguiOS.
 *
 * Suporta: %s %c %d %i %u %x %X %p %%, com largura, alinhamento à esquerda
 * ('-') e preenchimento com zeros ('0'). O modificador 'l' é aceito e
 * ignorado (long tem 32 bits aqui). Não há ponto flutuante de propósito: o
 * kernel não usa a FPU.
 *
 * A ideia central: cada conversão produz primeiro os dígitos num buffer local
 * (de trás para frente), e só depois decide o preenchimento. Assim uma única
 * rotina, emitir_campo(), cuida de largura e alinhamento para todos os tipos. */
#include "console.h"
#include "texto.h"

typedef struct {
    char  *dst;
    size_t max;   /* capacidade, incluindo o zero final */
    size_t pos;   /* quantos caracteres já seriam escritos (pode passar de max) */
} saida_t;

static void poe(saida_t *s, char c)
{
    if (s->pos + 1 < s->max)
        s->dst[s->pos] = c;
    s->pos++;
}

static void emitir_campo(saida_t *s, const char *txt, size_t n, int largura, bool esquerda, char preenche)
{
    int falta = largura > (int)n ? largura - (int)n : 0;
    /* Com zeros, o sinal de menos vem antes do preenchimento: -0042. */
    if (!esquerda && preenche == '0' && n && txt[0] == '-') {
        poe(s, '-'); txt++; n--;
    }
    if (!esquerda) while (falta--) poe(s, preenche);
    while (n--) poe(s, *txt++);
    if (esquerda) while (falta-- > 0) poe(s, ' ');
}

/* Escreve v na base dada no fim de buf; devolve o início. */
static char *numero(char *fim, uint32_t v, unsigned base, bool maiusculo)
{
    const char *dig = maiusculo ? "0123456789ABCDEF" : "0123456789abcdef";
    *fim = 0;
    do {
        *--fim = dig[v % base];
        v /= base;
    } while (v);
    return fim;
}

int formatar(char *dst, size_t max, const char *fmt, va_list ap)
{
    saida_t s = { dst, max, 0 };
    char buf[16];

    for (; *fmt; fmt++) {
        if (*fmt != '%') { poe(&s, *fmt); continue; }
        fmt++;
        bool esquerda = false;
        char preenche = ' ';
        int largura = 0;
        for (;; fmt++) {
            if (*fmt == '-') esquerda = true;
            else if (*fmt == '0') preenche = '0';
            else break;
        }
        while (*fmt >= '0' && *fmt <= '9')
            largura = largura * 10 + (*fmt++ - '0');
        while (*fmt == 'l') fmt++;

        char *ini;
        switch (*fmt) {
        case 's': {
            const char *t = va_arg(ap, const char *);
            if (!t) t = "(nulo)";
            emitir_campo(&s, t, txt_tamanho(t), largura, esquerda, ' ');
            break;
        }
        case 'c':
            buf[0] = (char)va_arg(ap, int);
            emitir_campo(&s, buf, 1, largura, esquerda, ' ');
            break;
        case 'd': case 'i': {
            int32_t v = va_arg(ap, int32_t);
            /* Converte o módulo como sem sinal: funciona até para -2^31. */
            ini = numero(buf + sizeof buf - 1, v < 0 ? 0u - (uint32_t)v : (uint32_t)v, 10, false);
            if (v < 0) *--ini = '-';
            emitir_campo(&s, ini, txt_tamanho(ini), largura, esquerda, preenche);
            break;
        }
        case 'u':
            ini = numero(buf + sizeof buf - 1, va_arg(ap, uint32_t), 10, false);
            emitir_campo(&s, ini, txt_tamanho(ini), largura, esquerda, preenche);
            break;
        case 'x': case 'X':
            ini = numero(buf + sizeof buf - 1, va_arg(ap, uint32_t), 16, *fmt == 'X');
            emitir_campo(&s, ini, txt_tamanho(ini), largura, esquerda, preenche);
            break;
        case 'p':
            ini = numero(buf + sizeof buf - 1, (uint32_t)va_arg(ap, void *), 16, false);
            poe(&s, '0'); poe(&s, 'x');
            emitir_campo(&s, ini, txt_tamanho(ini), 8, false, '0');
            break;
        case '%':
            poe(&s, '%');
            break;
        case 0:
            fmt--;  /* '%' no fim do texto: para sem ler além */
            break;
        default:
            poe(&s, '%'); poe(&s, *fmt);
        }
    }
    if (max)
        dst[s.pos < max ? s.pos : max - 1] = 0;
    return (int)s.pos;
}

int snformatar(char *dst, size_t max, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = formatar(dst, max, fmt, ap);
    va_end(ap);
    return n;
}
