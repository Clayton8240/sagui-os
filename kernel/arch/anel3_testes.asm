; anel3_testes.asm - três programinhas para o teste da H12.
;
; Ainda não há carregador de ELF (H13). Estes trechos são independentes de
; posição: o autoteste copia cada um para 0x40000000 num espaço de endereço
; novo e o roda no anel 3. Números das chamadas: kernel/inc/abi.h.

SIS_SAIR     equ 3
SIS_PID      equ 4
SIS_ESCREVER equ 12

section .rodata
bits 32

; 1) Escreve "Ola do ring 3!" pela chamada escrever e sai. Antes de sair,
;    tenta escrever a partir de um endereço do kernel (1 MB): o kernel tem que
;    recusar. Código de saída 42 = recusou (certo); 1 = aceitou (errado).
global anel3_ola, anel3_ola_fim
anel3_ola:
        call .aqui
.aqui:  pop esi                         ; ESI = endereço de .aqui (sem endereços fixos)
        mov eax, SIS_ESCREVER
        mov ebx, 1
        lea ecx, [esi + (.msg - .aqui)]
        mov edx, .msg_fim - .msg
        int 0x80
        mov eax, SIS_ESCREVER
        mov ebx, 1
        mov ecx, 0x00100000             ; memória do kernel
        mov edx, 16
        int 0x80
        mov ebx, 1
        test eax, eax
        jns .sai
        mov ebx, 42
.sai:   mov eax, SIS_SAIR
        int 0x80
.msg:   db "Ola do ring 3!", 10
.msg_fim:
anel3_ola_fim:

; 2) Executa uma instrução privilegiada (cli). No anel 3 isso gera proteção
;    geral, e o kernel tem que encerrar só este processo.
global anel3_cli, anel3_cli_fim
anel3_cli:
        cli
        mov eax, SIS_SAIR
        xor ebx, ebx
        int 0x80
anel3_cli_fim:

; 3) Tenta ler a memória do kernel diretamente. As páginas do kernel não têm
;    o bit de usuário: falta de página e o processo é encerrado.
global anel3_espiao, anel3_espiao_fim
anel3_espiao:
        mov eax, [0x00100000]
        mov eax, SIS_SAIR
        xor ebx, ebx
        int 0x80
anel3_espiao_fim:
