/* excecoes.c - a tela de erro do SaguiOS.
 *
 * Quando o kernel comete um erro que a CPU detecta (dividir por zero, acessar
 * memória que não existe...), não há como continuar com segurança. A tela
 * mostra o nome da exceção em português e o estado dos registradores, que é o
 * que permite achar a causa a partir de uma foto. */
#include "cpu.h"
#include "console.h"

static const char *const nomes[32] = {
    "Divisão por zero", "Depuração", "Interrupção não mascarável", "Ponto de parada",
    "Estouro (INTO)", "Índice fora dos limites (BOUND)", "Instrução inválida", "Coprocessador indisponível",
    "Falta dupla", "Segmento do coprocessador", "TSS inválida", "Segmento ausente",
    "Falha na pilha", "Proteção geral", "Falta de página", "Reservada (15)",
    "Erro de ponto flutuante (x87)", "Verificação de alinhamento", "Verificação de máquina", "Erro de ponto flutuante (SIMD)",
    "Virtualização", "Proteção de controle", "Reservada (22)", "Reservada (23)",
    "Reservada (24)", "Reservada (25)", "Reservada (26)", "Reservada (27)",
    "Injeção do hipervisor", "Comunicação VMM", "Segurança", "Reservada (31)",
};

const char *nome_excecao(uint32_t vetor)
{
    return vetor < 32 ? nomes[vetor] : "Interrupção desconhecida";
}

SEM_RETORNO void parar_maquina(void)
{
    for (;;)
        __asm__ volatile("cli; hlt");
}

SEM_RETORNO void excecao_fatal(quadro_t *q)
{
    int_desliga();
    uint32_t cr2 = le_cr2();
    klog("\n[SAGUI] EXCECAO %u (%s) erro=%x eip=%x\n", q->vetor, nome_excecao(q->vetor), q->erro, q->eip);

    vga_cor(COR_BRANCO, COR_VERMELHO);
    vga_limpar();
    kprintf("\n  O SaguiOS encontrou um erro e precisou parar.\n\n");
    vga_cor(COR_AMARELO, COR_VERMELHO);
    kprintf("  Exceção %u: %s\n", q->vetor, nome_excecao(q->vetor));
    vga_cor(COR_BRANCO, COR_VERMELHO);
    kprintf("  Código de erro: 0x%08x\n\n", q->erro);
    kprintf("  EIP=%08x  CS=%04x  EFLAGS=%08x\n", q->eip, q->cs, q->eflags);
    kprintf("  EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x\n", q->eax, q->ebx, q->ecx, q->edx);
    kprintf("  ESI=%08x  EDI=%08x  EBP=%08x  ESP=%08x\n", q->esi, q->edi, q->ebp, q->esp_kernel + 20);
    kprintf("  DS=%04x  ES=%04x  FS=%04x  GS=%04x\n", q->ds, q->es, q->fs, q->gs);
    kprintf("  CR0=%08x  CR2=%08x  CR3=%08x\n", le_cr0(), cr2, le_cr3());
    if (q->vetor == 14) {
        /* Bits do código de erro da falta de página (manual da Intel):
         * 0 = página presente (violação de proteção) ou ausente,
         * 1 = escrita ou leitura, 2 = em modo usuário ou kernel. */
        kprintf("\n  Falta de página: %s no endereço 0x%08x,\n", q->erro & 2 ? "escrita" : "leitura", cr2);
        kprintf("  %s, em modo %s.\n", q->erro & 1 ? "página protegida" : "página não mapeada",
                q->erro & 4 ? "usuário" : "kernel");
    }
    kprintf("\n  Fotografe esta tela para ajudar a encontrar o problema.\n");
    kprintf("  Desligue ou reinicie o computador.\n");
    parar_maquina();
}
