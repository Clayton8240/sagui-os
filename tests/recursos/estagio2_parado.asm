; Estágio 2 de mentira, só para o teste da H02: para a máquina sem tocar na
; tela, para que a mensagem do MBR fique visível na captura.
bits 16
org 0x8000
        cli
.l:     hlt
        jmp .l
