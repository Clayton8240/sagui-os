/* caminho.c - resolução de caminhos.
 *
 * Em vez de manipular texto com ".." no meio, o caminho é tratado como uma
 * pilha de componentes: cada nome empilha, ".." desempilha, "." e barras
 * repetidas são ignorados. A pilha é a própria string de saída, e
 * "desempilhar" é voltar até a barra anterior. */
#include "caminho.h"
#include "texto.h"

static bool empilhar_componentes(char *saida, size_t max, size_t *tam, const char *s)
{
    while (*s) {
        while (*s == '/') s++;
        if (!*s) break;
        const char *ini = s;
        while (*s && *s != '/') s++;
        size_t n = (size_t)(s - ini);
        if (n == 1 && ini[0] == '.')
            continue;
        if (n == 2 && ini[0] == '.' && ini[1] == '.') {
            while (*tam > 0 && saida[*tam - 1] != '/') (*tam)--;   /* apaga o nome */
            if (*tam > 0) (*tam)--;                                 /* e a barra */
            continue;
        }
        if (*tam + 1 + n + 1 > max)
            return false;
        saida[(*tam)++] = '/';
        memcpy(saida + *tam, ini, n);
        *tam += n;
    }
    return true;
}

bool caminho_resolver(const char *atual, const char *alvo, char *saida, size_t max)
{
    if (max < 2)
        return false;
    size_t tam = 0;
    if (alvo[0] != '/' && !empilhar_componentes(saida, max, &tam, atual))
        return false;
    if (!empilhar_componentes(saida, max, &tam, alvo))
        return false;
    if (tam == 0)
        saida[tam++] = '/';
    saida[tam] = 0;
    return true;
}
