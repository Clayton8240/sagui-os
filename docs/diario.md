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

## 2026-11-03 15:22

- **Confirmação do Eli no hardware:** "fase 1 ok". Fase 1 validada no VirtualBox e no G470.
- Foto do Eli no G470: `testes_eli/fase_1_no_notebook.jpg` (cópia em
  `docs/capturas/G470-fase1-tela-final.png`). Boot pelo pendrive (disco 0x80),
  22 setores lidos, A20 já ligada pela BIOS, VESA 1024x768x32 (modo 0x118),
  ramdisk íntegro. Mapa E820 com ~20 regiões e ~8 GB de RAM.
- **Problema visto na foto:** o mapa E820 do G470 é grande e rolou a tela, e a
  saudação e a linha de memória saíram do topo. Correção combinada com o Eli:
  fazer no começo da fase 2 (H04-fix).
- **H04-fix.** Mapa E820 completo só na serial; a tela mostra o resumo e,
  separada, a memória que um kernel de 32 bits consegue usar. Testado com
  6 GB no QEMU (6143 MB no total, 3071 MB abaixo de 4 GB).
- **Fase 2 iniciada.** Mecanismo de autotestes pelo cabeçalho de boot (D006).
- **H05.** GDT e TSS próprias, 256 vetores gerados por `%rep`, despachante que
  devolve o quadro a retomar (D007), tela de erro em português, PIC
  remapeado, PIT a 100 Hz conferido contra o relógio CMOS. Kernel passou a ser
  compilado com strings em Latin-1, traduzidas para a fonte da placa VGA.
- **H06.** Bitmap de páginas até 896 MB e `kmalloc` por classes de tamanho
  (D008). Bug achado na revisão antes do primeiro teste: o elo da lista livre
  sobrescrevia a assinatura do bloco; corrigido usando o campo do cabeçalho.
  Autoteste com 450 blocos passou de primeira com 128 MB e 1 GB.
- **H07.** Teclado ABNT2 completo (D009). O teste digita 25 grupos de teclas e
  compara byte a byte. Descoberta: no QEMU o AltGr é `alt_r`, não `altgr`.
  Captura revelou a faixa da saudação passando de 80 colunas (erro 2).
- **H08.** Leitor do SGFS e resolução de caminhos. Captura revelou que o
  `formatar()` não aceitava largura `*` (erro 3).
- **H09.** Terminal provisório com paginação. O harness digitava `/` com a
  tecla americana (erro 4, do teste, não do kernel).
- **Fim da fase 2.** 9/9 testes passando (H01 a H09). Imagens em
  `entrega/fase2/`. Parada obrigatória: aguardando o teste do Eli.

## 2026-11-03 16:05

- **Confirmação do Eli no hardware:** "fase 2 ok". Fase 2 validada no VirtualBox e no G470.
- Foto do Eli no G470: `testes_eli/fase_2_no_notebook.jpg` (cópia em
  `docs/capturas/G470-fase2-tela-inicial.png`). Saudação no topo (correção
  confirmada), 8135 MB no total, 3017 MB abaixo de 4 GB, 892 MB gerenciados,
  21 regiões no E820. Todos os comandos funcionaram, sem tela vermelha; `/`,
  `?` e `ç` conferidos. Acentos com teclas mortas ainda não testados
  isoladamente no notebook (o Eli avisa se der erro).
- **Fase 3 iniciada** (tag `pre-fase3`). Plano de arquitetura em D010: mapa de
  endereços, chamadas reiniciáveis, herança só de 0-2.
- **H10.** Paginação de 2 níveis, tabelas do kernel compartilhadas, janelas
  para dispositivos, tela de falta de página explicada.
- **H11.** Escalonador round-robin: `AABBAABB...` com duas tarefas que nunca
  cedem a vez. Truque usado: o quadro inicial de uma tarefa do kernel guarda
  o endereço de retorno e o argumento nos campos que o `iret` do anel 0 não lê.
- **H12.** Anel 3, TSS e `int 0x80` com validação de ponteiros; `cli` e
  leitura do kernel no anel 3 encerram só o processo.
- **H13.** Carregador ELF, libsagui, build da userland; `/bin/ola um dois três`.
- **H14.** Árvore de arquivos na RAM com cópia na escrita, descritores,
  terminal como arquivo (modos linha e bruto). Regressão encontrada na H12 e
  corrigida (erro 5); o commit tinha passado porque o script encadeava o
  commit depois de um `grep` (passei a conferir PASSOU explicitamente).
- **H15.** Pipes: 20.000 bytes num buffer de 512, nos dois sentidos.
- **H16.** sagsh com redirecionamento, pipes, `&`, histórico e Tab; 18
  programas com os apelidos; terminal provisório removido; teste da H09
  adaptado ao shell. Achado: o GCC não divide em `1 / x` (erro 6).
- **Fim da fase 3.** 16/16 testes passando. Imagem com 223 KB úteis. Imagens
  em `entrega/fase3/`. Parada obrigatória: aguardando o teste do Eli.

## 2026-11-03 17:21

- **Confirmação do Eli no hardware:** "fase 3 ok". Fase 3 validada no VirtualBox e no G470.
- Foto do Eli no G470: `testes_eli/fase_3_no_notebook.jpg` (cópia em
  `docs/capturas/G470-fase3-contador-ps.png`): `contador 10 &` contando
  enquanto `ps` roda, quatro processos, shell no anel 3. Setas para cima e
  para baixo (histórico) e acentos com teclas mortas conferidos. Pedidos do
  Eli: as setas para os lados não editam a linha (não estava implementado:
  vai na H18, com o terminal gráfico) e o texto de um programa em segundo
  plano se mistura com a linha que está sendo digitada (também na H18).
- **Fase 4 iniciada** (tag `pre-fase4`). Pedidos do Eli registrados.
- **H17.** Console separado em camada (`console_*`) com dois motores. Vídeo
  VESA com buffer duplo de 32 bits, cópia só do que mudou, primitivas.
- **H18.** Fonte própria desenhada em texto (`tools/fonte/sagui8x16.txt`),
  acentuadas compostas pelo gerador; terminal gráfico 128x48; boot
  reordenado para a tela ficar gráfica logo após ligar a memória. O harness
  passou a ler a tela gráfica reconhecendo as células pela própria fonte
  (sem OCR), e todos os testes antigos seguiram valendo. Pedidos do Eli:
  edição com setas no shell e redesenho da linha quando outro processo
  escreve.
- **H19.** Mouse PS/2 (IRQ 12) com cursor próprio. Corrigido um problema
  latente: o tratador do teclado descartava bytes do mouse.
- **Fim da fase 4.** 19/19 testes passando. Imagens em `entrega/fase4/`.
  Parada obrigatória: aguardando o teste do Eli.
- **Teste do Eli no G470 (fase 4):** o touchpad não respondeu e o teclado
  parou. **H19-fix:** módulo `i8042.c` único, fila esvaziada também pelo
  relógio, configuração conferida, porta do mouse desligada se ele não
  responder, diagnóstico na tela (erro 9, D020). Um teste novo com a IRQ 12
  bloqueada reproduz o defeito no QEMU com o código antigo e passa com o
  novo. Na regressão apareceram duas disputas de tempo em testes (erro 10),
  corrigidas. 19/19 passando. Nova imagem em `entrega/fase4/`.

## 2026-11-03 18:37

- **Confirmação do Eli no hardware:** "fase 4 ok". Fase 4 validada no VirtualBox e no G470.
- Foto do Eli no G470 depois da correção: `testes_eli/fase_4_no_notebook.jpg`
  (cópia em `docs/capturas/G470-fase4-grafico-touchpad.png`). Modo gráfico
  1024x768x32 ligado, fonte própria com acentos (`Ação, vovô, pão, ÇÃO`),
  touchpad respondendo (linha `[ ok ] Mouse/touchpad PS/2`) e seta na tela,
  teclado funcionando. A correção H19-fix resolveu.
