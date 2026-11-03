/* janela <nome> <cor> - teste da H21: abre uma janela, pinta, e registra
 * na saída cada evento que recebe. Termina sozinho ao receber "fechar". */
#include "sagui.h"

int principal(int argc, char **argv)
{
    const char *nome = argc > 1 ? argv[1] : "teste";
    uint32_t cor = argc > 2 ? (uint32_t)texto_para_int(argv[2]) : 0x3366CC;
    uint32_t *px;
    int id = criar_janela(nome, 300, 200, &px);
    if (id < 0) { registrar("janela %s: erro %d\n", nome, id); return 1; }
    for (int i = 0; i < 300 * 200; i++) px[i] = cor;
    atualizar_janela(id, NULL);
    registrar("janela %s: aberta (id %d)\n", nome, id);
    evento_t e;
    for (;;) {
        if (proximo_evento(id, &e, true) != 1) continue;
        if (e.tipo == EV_FECHAR) {
            registrar("janela %s: recebi fechar, terminando\n", nome);
            return 0;
        }
        if (e.tipo == EV_CLIQUE)
            registrar("janela %s: clique em (%d,%d) botao %u cliques %u\n", nome, e.x, e.y, e.botoes, e.cliques);
        else if (e.tipo == EV_TECLA)
            registrar("janela %s: tecla %x\n", nome, e.tecla);
        else if (e.tipo == EV_SOLTOU)
            registrar("janela %s: soltou em (%d,%d)\n", nome, e.x, e.y);
    }
}
