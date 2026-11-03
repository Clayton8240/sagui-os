/* arquivos [pasta] - o gerenciador de arquivos do SaguiOS (H23).
 *
 * Mostra uma pasta como lista com ícones (pastas primeiro). Duplo clique:
 * pasta entra, arquivo de texto abre no Editor, programa executa. Botões
 * para voltar, criar pasta e apagar (com confirmação). */
#include "sagui.h"
#include "gui.h"

#define LARG 620
#define ALT  440
#define MAX_ITENS 128

static jan_t *j;
static char pasta[160] = "/";
static char nomes[MAX_ITENS][64];
static uint8_t tipos[MAX_ITENS];
static uint32_t tamanhos[MAX_ITENS];
static lista_t lista;

/* Ícones 16x16 em pixel art. */
static const char *const ic_pasta[] = {
    "................", "................", ".PPPPP..........", "PYYYYYP.........",
    "PYYYYYYPPPPPPPP.", "PYYYYYYYYYYYYYYP", "PPPPPPPPPPPPPPPP", "PAAAAAAAAAAAAAAP",
    "PAAAAAAAAAAAAAAP", "PAAAAAAAAAAAAAAP", "PAAAAAAAAAAAAAAP", "PAAAAAAAAAAAAAAP",
    "PAAAAAAAAAAAAAAP", "PPPPPPPPPPPPPPPP", "................", "................",
};
static const char *const ic_arquivo[] = {
    "..PPPPPPPPP.....", "..PBBBBBBBPP....", "..PBBBBBBBPBP...", "..PBBBBBBBPPPP..",
    "..PBCCCCCBBBBP..", "..PBBBBBBBBBBP..", "..PBCCCCCCCCBP..", "..PBBBBBBBBBBP..",
    "..PBCCCCCCCBBP..", "..PBBBBBBBBBBP..", "..PBCCCCCCCCBP..", "..PBBBBBBBBBBP..",
    "..PBCCCCCBBBBP..", "..PBBBBBBBBBBP..", "..PPPPPPPPPPPP..", "................",
};
static const char *const ic_programa[] = {
    "................", ".PPPPPPPPPPPPPP.", ".PVVVVVVVVVVVVP.", ".PPPPPPPPPPPPPP.",
    ".PDDDDDDDDDDDDP.", ".PDGDDDDDDDDDDP.", ".PDDGDDDDDDDDDP.", ".PDDDGDDDDDDDDP.",
    ".PDDGDDDDDDDDDP.", ".PDGDDGGGGDDDDP.", ".PDDDDDDDDDDDDP.", ".PDDDDDDDDDDDDP.",
    ".PPPPPPPPPPPPPP.", "................", "................", "................",
};

static uint32_t paleta(char c)
{
    switch (c) {
    case 'P': return 0x202428;
    case 'Y': return 0xFFE070;
    case 'A': return 0xF2C230;
    case 'B': return 0xFFFFFF;
    case 'C': return 0x9AA3AC;
    case 'V': return 0x2A6FD8;
    case 'D': return 0x182030;
    case 'G': return 0x3CDC6E;
    default:  return 0xFF000000;
    }
}

static void icone(jan_t *jj, int x, int y, int i)
{
    const char *const *d = tipos[i] == TIPO_PASTA ? ic_pasta : tipos[i] == TIPO_PROGRAMA ? ic_programa : ic_arquivo;
    g_desenho(jj, x, y, d, 16, 16, paleta, 1);
}

static void caminho_de(int i, char *saida, size_t max)
{
    copia_texto(saida, pasta, max);
    if (compara_texto(pasta, "/") != 0) junta_texto(saida, "/", max);
    junta_texto(saida, nomes[i], max);
}

static void desenhar_topo(void)
{
    g_retangulo(j, 0, 40, LARG, 26, COR_FUNDO);
    char t[200];
    formatar_texto(t, sizeof t, "Pasta: %s", pasta);
    g_texto_n(j, 10, 46, t, (LARG - 20) / LETRA_L, COR_TEXTO);
    jan_mostrar_area(j, 0, 40, LARG, 26);
}

static void desenhar_rodape(const char *msg)
{
    g_retangulo(j, 0, ALT - 24, LARG, 24, 0xE6E9EC);
    g_retangulo(j, 0, ALT - 24, LARG, 1, COR_BORDA);
    g_texto_n(j, 10, ALT - 20, msg, (LARG - 20) / LETRA_L, COR_TEXTO_FRACO);
    jan_mostrar_area(j, 0, ALT - 24, LARG, 24);
}

/* Lê a pasta: primeiro as pastas, depois o resto (o kernel já devolve em
 * ordem alfabética). */
static void carregar(void)
{
    entrada_pasta_t e;
    int n = 0;
    for (int passo = 0; passo < 2; passo++)
        for (int i = 0; n < MAX_ITENS && listar(pasta, i, &e) == 1; i++) {
            bool eh_pasta = e.tipo == TIPO_PASTA;
            if ((passo == 0) != eh_pasta) continue;
            copia_texto(nomes[n], e.nome, 64);
            tipos[n] = (uint8_t)e.tipo;
            tamanhos[n] = e.tamanho;
            n++;
        }
    lista.qtd = n;
    lista.sel = n ? 0 : -1;
    lista.topo = 0;
    desenhar_topo();
    lista_desenhar(j, &lista);
    char msg[64];
    formatar_texto(msg, sizeof msg, "%d itens. Duplo clique para abrir.", n);
    desenhar_rodape(msg);
    registrar("arquivos: pasta %s (%d itens)", pasta, n);
}

static void entrar(const char *nova)
{
    info_arquivo_t inf;
    if (info_arquivo(nova, &inf) < 0 || inf.tipo != TIPO_PASTA) return;
    copia_texto(pasta, nova, sizeof pasta);
    carregar();
}

static void voltar(void)
{
    if (compara_texto(pasta, "/") == 0) return;
    char *b = acha_ultimo_char(pasta, '/');
    if (b == pasta) pasta[1] = 0; else *b = 0;
    carregar();
}

static void abrir_item(int i)
{
    if (i < 0 || i >= lista.qtd) return;
    char c[200];
    caminho_de(i, c, sizeof c);
    if (tipos[i] == TIPO_PASTA) { entrar(c); return; }
    char *argv[3];
    int pid;
    if (tipos[i] == TIPO_PROGRAMA) {
        argv[0] = nomes[i]; argv[1] = NULL;
        pid = executar(c, argv);
        registrar("arquivos: executando %s (pid %d)", c, pid);
    } else {
        argv[0] = "editor"; argv[1] = c; argv[2] = NULL;
        pid = executar("/bin/editor", argv);
        registrar("arquivos: abrindo no editor %s (pid %d)", c, pid);
    }
    char msg[200];
    formatar_texto(msg, sizeof msg, pid > 0 ? "Abrindo %s..." : "Não consegui abrir %s", nomes[i]);
    desenhar_rodape(msg);
}

static void nova_pasta(void)
{
    char nome[64] = "";
    if (!pedir_texto("Nova pasta", "Nome da nova pasta:", nome, sizeof nome) || !nome[0]) return;
    char c[200];
    copia_texto(c, pasta, sizeof c);
    if (compara_texto(pasta, "/") != 0) junta_texto(c, "/", sizeof c);
    junta_texto(c, nome, sizeof c);
    int r = criar_pasta(c);
    if (r < 0) { char m[200]; formatar_texto(m, sizeof m, "Não consegui criar a pasta:\n%s", nome_erro(r)); mensagem("Arquivos", m); return; }
    registrar("arquivos: pasta criada %s", c);
    carregar();
}

static void apagar_item(void)
{
    int i = lista.sel;
    if (i < 0 || i >= lista.qtd) return;
    char c[200], pergunta[200];
    caminho_de(i, c, sizeof c);
    formatar_texto(pergunta, sizeof pergunta, "Tem certeza que quer apagar\n%s%s?", nomes[i], tipos[i] == TIPO_PASTA ? "/" : "");
    if (!confirmar("Apagar", pergunta)) return;
    int r = tipos[i] == TIPO_PASTA ? remover_pasta(c) : apagar(c);
    if (r < 0) { char m[200]; formatar_texto(m, sizeof m, "Não consegui apagar:\n%s", nome_erro(r)); mensagem("Arquivos", m); return; }
    registrar("arquivos: apagado %s", c);
    carregar();
}

int principal(int argc, char **argv)
{
    if (argc > 1) copia_texto(pasta, argv[1], sizeof pasta);
    j = jan_abrir("Arquivos", LARG, ALT);
    if (!j) return 1;
    botao_t bt[3] = {
        { 8, 7, 90, 26, "Voltar", false }, { 106, 7, 110, 26, "Nova pasta", false }, { 224, 7, 90, 26, "Apagar", false },
    };
    g_retangulo(j, 0, 0, LARG, 40, 0xE6E9EC);
    g_retangulo(j, 0, 39, LARG, 1, COR_BORDA);
    for (int i = 0; i < 3; i++) botao_desenhar(j, &bt[i]);
    lista = (lista_t){ 8, 70, LARG - 16, ALT - 70 - 30, nomes, 0, -1, 0, icone };
    carregar();
    jan_mostrar(j);
    registrar("arquivos: pronto");

    evento_t e;
    for (;;) {
        if (proximo_evento(j->id, &e, true) != 1) continue;
        int c;
        while (recolher(&c) > 0) ;   /* programas abertos daqui que já terminaram */
        if (e.tipo == EV_FECHAR) break;
        if (botao_tratar(j, &bt[0], &e)) voltar();
        if (botao_tratar(j, &bt[1], &e)) nova_pasta();
        if (botao_tratar(j, &bt[2], &e)) apagar_item();
        if (e.tipo == EV_TECLA && e.tecla == '\b') { voltar(); continue; }
        if (e.tipo == EV_TECLA && e.tecla == TECLA_DELETE) { apagar_item(); continue; }
        int r = lista_tratar(j, &lista, &e);
        if (r == LISTA_ATIVOU) abrir_item(lista.sel);
        else if (r == LISTA_SELECIONOU && lista.sel >= 0) {
            char msg[100];
            if (tipos[lista.sel] == TIPO_PASTA) formatar_texto(msg, sizeof msg, "%s/ (pasta)", nomes[lista.sel]);
            else formatar_texto(msg, sizeof msg, "%s: %u bytes%s", nomes[lista.sel], tamanhos[lista.sel],
                                tipos[lista.sel] == TIPO_PROGRAMA ? " (programa)" : "");
            desenhar_rodape(msg);
        }
    }
    jan_fechar(j);
    return 0;
}
