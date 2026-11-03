/* padrao.h - o texto conhecido que o gerador escreve e o consumidor confere. */
#ifndef PADRAO_H
#define PADRAO_H
static inline char byte_do_padrao(unsigned i)
{
    return (i % 64 == 63) ? '\n' : (char)('a' + (i * 7 + i / 64) % 26);
}
#endif
