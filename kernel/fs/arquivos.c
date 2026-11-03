/* arquivos.c - arquivos abertos por processo. (Provisório até a H14: só a
 * saída para a tela funciona.) */
#include "processo.h"
#include "sistema.h"
#include "console.h"

void arquivos_fechar_todos(processo_t *p) { (void)p; }

#define SEM_USO(...) (void)a; (void)b; (void)c; (void)d; (void)e; return ERRO_INVALIDO
int32_t sis_abrir(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_ler(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_fechar(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_posicionar(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_info_arquivo(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_criar_pasta(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_remover_pasta(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_apagar(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_listar(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_mudar_pasta(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_pasta_atual(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_renomear(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_pipe(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_duplicar(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_modo_terminal(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }
int32_t sis_limpar_tela(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { SEM_USO(); }

int32_t sis_escrever(uint32_t fd, uint32_t buf, uint32_t n, uint32_t d, uint32_t e)
{
    (void)d; (void)e;
    if (fd != 1 && fd != 2) return ERRO_DESCRITOR;
    if (!usuario_ok((const void *)buf, n, false)) return ERRO_ENDERECO;
    const char *s = (const char *)buf;
    for (uint32_t i = 0; i < n; i++) { vga_char(s[i]); serial_char(s[i]); }
    return (int32_t)n;
}
