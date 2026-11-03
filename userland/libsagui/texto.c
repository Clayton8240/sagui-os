/* texto.c - memória e texto para os programas. */
#include "sagui.h"

void *memcpy(void *d, const void *s, size_t n)
{
    uint8_t *a = d;
    const uint8_t *b = s;
    while (n--) *a++ = *b++;
    return d;
}

void *memmove(void *d, const void *s, size_t n)
{
    uint8_t *a = d;
    const uint8_t *b = s;
    if (a <= b || a >= b + n) return memcpy(d, s, n);
    a += n; b += n;
    while (n--) *--a = *--b;
    return d;
}

void *memset(void *d, int v, size_t n)
{
    uint8_t *a = d;
    while (n--) *a++ = (uint8_t)v;
    return d;
}

int memcmp(const void *x, const void *y, size_t n)
{
    const uint8_t *a = x, *b = y;
    for (; n; n--, a++, b++)
        if (*a != *b) return *a - *b;
    return 0;
}

size_t tam_texto(const char *s)
{
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

int compara_texto(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return (uint8_t)*a - (uint8_t)*b;
}

int compara_texto_n(const char *a, const char *b, size_t n)
{
    for (; n; n--, a++, b++) {
        if (*a != *b) return (uint8_t)*a - (uint8_t)*b;
        if (!*a) return 0;
    }
    return 0;
}

char *copia_texto(char *dst, const char *src, size_t max)
{
    size_t i = 0;
    if (!max) return dst;
    for (; i + 1 < max && src[i]; i++) dst[i] = src[i];
    dst[i] = 0;
    return dst;
}

char *junta_texto(char *dst, const char *src, size_t max)
{
    size_t n = tam_texto(dst);
    if (n < max) copia_texto(dst + n, src, max - n);
    return dst;
}

char *acha_char(const char *s, int c)
{
    for (; *s; s++)
        if (*s == (char)c) return (char *)s;
    return c == 0 ? (char *)s : NULL;
}

char *acha_ultimo_char(const char *s, int c)
{
    const char *achou = NULL;
    for (; *s; s++)
        if (*s == (char)c) achou = s;
    return (char *)achou;
}

bool comeca_com(const char *s, const char *p)
{
    while (*p)
        if (*s++ != *p++) return false;
    return true;
}

bool eh_espaco(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int texto_para_int(const char *s)
{
    int sinal = 1, v = 0;
    while (eh_espaco(*s)) s++;
    if (*s == '-') { sinal = -1; s++; } else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0');
    return v * sinal;
}
