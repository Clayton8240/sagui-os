# Diário do SaguiOS

Log cronológico do que foi feito. Uma entrada por bloco de trabalho.

## 2026-11-03

- **H01.** Leitura do PRD. Instalados NASM 2.16, ImageMagick 6.9 e `gcc-multilib`;
  QEMU 8.2.2, Python 3 e mtools já estavam na VPS. Validado o GCC do sistema com
  `-m32 -ffreestanding` (gera ELF32 i386). Criada a estrutura de pastas da seção 5,
  o repositório git, o `status.md`, o painel e a ferramenta `tools/status.py`.
- **H02.** MBR de 512 bytes: salto + BPB falso reservado, leitura por LBA com
  3 tentativas, cabeçalho de boot no LBA 1 como única posição fixa, tabela com
  uma partição ativa. Testado isolado (estágio 2 de mentira que só para) em IDE
  e em USB/EHCI: a mensagem `SaguiOS: carregando...` aparece nos dois.
- **H03.** Estágio 2 com as 6 etapas na tela, unreal mode para copiar acima de
  1 MB, soma de verificação do kernel e do ramdisk, procura do modo VESA. Kernel
  em C: BSS, pilha, cópia da boot_info, saudação e memória. A memória mostrada
  acompanha a RAM da VM (511 MB com 512, 127 MB com 128), prova de que vem do E820.
- **H04.** `.vdi` pelo `qemu-img`; o teste dá boot no próprio `.vdi` e confere
  que ele é idêntico ao `.img`. Imagem mínima de 1 MB (D004). `como-gravar.md`
  e `teste-fase1.md` escritos. Nenhum erro de execução nesta fase: todos os
  testes passaram na primeira rodada completa (ver `docs/erros.md`).
- **Fim da fase 1.** Todos os testes (H01 a H04) passando. Imagens em
  `entrega/fase1/`. Parada obrigatória: aguardando o teste do Eli.
