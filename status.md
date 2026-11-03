```
Eli, terminei a fase 2: Kernel e terminal. Pode testar!

O que foi feito: corrigi a saudação que sumia no G470 (o mapa de memória agora
vai para o log). O kernel passou a tratar interrupções e erros da CPU (com uma
tela vermelha que mostra o erro), conta o tempo, controla a memória página a
página, lê o teclado ABNT2 com ç e acentos, lê os arquivos do ramdisk e abre um
terminal provisório com os comandos ajuda, ls, cd, cat, limpar, mem, data,
tempo e reiniciar.
Histórias concluídas: H04-fix e H05 a H09
Imagem: entrega/fase2/saguios.vdi (VirtualBox, testar primeiro) e saguios.img (pendrive, G470)
Roteiro de teste: docs/teste-fase2.md
O que você deve ver no G470: a saudação no topo; o prompt sagui:/$ respondendo
ao teclado do notebook (com ç e acentos); ls, cat e data funcionando.
Problemas conhecidos: no modo texto, ã, õ e maiúsculas acentuadas aparecem sem
acento (a fonte da placa de vídeo não tem essas letras; a fonte própria da fase
4 resolve). O kernel usa só os primeiros 896 MB de RAM, de propósito (D008).

Quando testar, me diga "fase 2 ok" ou descreva o que aconteceu.
```

# Status do SaguiOS

_Atualizado em 2026-11-03T15:45:24_

- **Situação:** Aguardando teste do Eli: fase 2
- **Fase atual:** 2
- **História atual:** -
- **Progresso geral:** 26.5%
- **Bloqueio:** nenhum
- **Imagem:** 41472 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 899, kernel 2686, userland 0

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | aguardando teste |
| 3 | Processos e programas | 0.0% | a fazer |
| 4 | Gráfico e mouse | 0.0% | a fazer |
| 5 | Janelas e programas | 0.0% | a fazer |
| 6 | Pendrive de dados | 0.0% | a fazer |
| 7 | Bônus | 0% | a fazer |

## Histórias

| ID | Título | Estado | Minutos |
| --- | --- | --- | --- |
| H01 | Preparar o ambiente | feito | 2 |
| H02 | Estágio 1 (MBR) | feito | 8 |
| H03 | Estágio 2 e modo protegido | feito | 1 |
| H04 | Imagem para pendrive e VirtualBox | feito | 4 |
| H05 | Interrupções e relógio | aguardando teste | 4 |
| H06 | Memória física | aguardando teste | 4 |
| H07 | Teclado PS/2 com ABNT2 | aguardando teste | 6 |
| H08 | Sistema de arquivos SGFS | aguardando teste | 2 |
| H09 | Terminal provisório do kernel | aguardando teste | 2 |
| H10 | Paginação | a fazer |  |
| H11 | Processos e escalonador | a fazer |  |
| H12 | Ring 3 e chamadas de sistema | a fazer |  |
| H13 | Carregador ELF e libsagui | a fazer |  |
| H14 | Arquivos e pastas graváveis | a fazer |  |
| H15 | Pipes | a fazer |  |
| H16 | Shell e comandos como programas | a fazer |  |
| H17 | Framebuffer VESA | a fazer |  |
| H18 | Fonte própria | a fazer |  |
| H19 | Mouse / touchpad PS/2 | a fazer |  |
| H20 | Área de trabalho | a fazer |  |
| H21 | Gerenciador de janelas | a fazer |  |
| H22 | Kit gráfico da libsagui | a fazer |  |
| H23 | Arquivos | a fazer |  |
| H24 | Terminal em janela | a fazer |  |
| H25 | Editor de texto | a fazer |  |
| H26 | Calculadora | a fazer |  |
| H27 | Sagui Pega Banana | a fazer |  |
| H28 | Acabamento | a fazer |  |
| H29 | PCI e controlador EHCI | a fazer |  |
| H30 | Enumeração USB, hubs e conexão a quente | a fazer |  |
| H31 | Armazenamento em massa | a fazer |  |
| H32 | FAT32: leitura | a fazer |  |
| H33 | FAT32: gravação | a fazer |  |
| H34 | Integração do pendrive | a fazer |  |
| B1 | Bônus: enxergar o HD interno | a fazer |  |
| B2 | Bônus: boot por UEFI | a fazer |  |
| B3 | Bônus: bip | a fazer |  |

## Últimas ações

- 2026-11-03T15:45:24: Fase 2 entregue. Aguardando teste do Eli.
- 2026-11-03T15:45:16: Todos os testes passaram (H01 a H09, 9/9), IDE e USB
- 2026-11-03T15:41:51: H09 concluída: Terminal provisório com ajuda, ls, cd, cat (paginado), limpar, mem, data, tempo, reiniciar
- 2026-11-03T15:41:51: Erro corrigido: cd ../arte chegava como cd ..;arte
- 2026-11-03T15:39:25: Início da H09: Terminal provisório do kernel
- 2026-11-03T15:39:25: H08 concluída: mksgfs.py + leitor no kernel; lista igual ao rootfs/ e leia-me.txt idêntico
- 2026-11-03T15:39:25: Erro corrigido: Saudação e memória quebravam linha
- 2026-11-03T15:39:25: Erro corrigido: Árvore do SGFS saiu como %*s na tela
- 2026-11-03T15:37:48: Início da H08: Sistema de arquivos SGFS
- 2026-11-03T15:37:48: H07 concluída: Driver PS/2 com ABNT2 completo, teclas mortas, AltGr, Caps Lock; 25 grupos de teclas conferidos byte a byte
- 2026-11-03T15:32:06: Início da H07: Teclado PS/2 com ABNT2
- 2026-11-03T15:32:06: H06 concluída: Bitmap de páginas a partir do E820 e kmalloc por classes; autoteste de 450 blocos sem corrupção
- 2026-11-03T15:28:28: Início da H06: Memória física
- 2026-11-03T15:28:28: H05 concluída: IDT com 256 vetores, tela de erro, PIC remapeado, PIT a 100 Hz, CMOS
- 2026-11-03T15:24:57: Início da H05: Interrupções e relógio
