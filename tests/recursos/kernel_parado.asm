; Kernel de mentira para o teste da H03: para a máquina sem limpar a tela,
; para que as mensagens [1/6] a [6/6] do estágio 2 fiquem visíveis.
section .entrada
global _inicio
_inicio:
        cli
.l:     hlt
        jmp .l
