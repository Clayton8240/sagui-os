# Status do SaguiOS

_Atualizado em 2026-11-03T18:46:46_

- **Situação:** Trabalhando na H21: Gerenciador de janelas
- **Fase atual:** 5
- **História atual:** H21
- **Progresso geral:** 58.8%
- **Bloqueio:** nenhum
- **Imagem:** 257024 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 903, kernel 6695, userland 1591

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 100.0% | feito |
| 4 | Gráfico e mouse | 100.0% | feito |
| 5 | Janelas e programas | 11.1% | em andamento |
| 6 | Pendrive de dados | 0.0% | a fazer |
| 7 | Bônus | 0% | a fazer |

## Histórias

| ID | Título | Estado | Minutos |
| --- | --- | --- | --- |
| H01 | Preparar o ambiente | feito | 2 |
| H02 | Estágio 1 (MBR) | feito | 8 |
| H03 | Estágio 2 e modo protegido | feito | 1 |
| H04 | Imagem para pendrive e VirtualBox | feito | 4 |
| H05 | Interrupções e relógio | feito | 4 |
| H06 | Memória física | feito | 4 |
| H07 | Teclado PS/2 com ABNT2 | feito | 6 |
| H08 | Sistema de arquivos SGFS | feito | 2 |
| H09 | Terminal provisório do kernel | feito | 2 |
| H10 | Paginação | feito | 3 |
| H11 | Processos e escalonador | feito | 3 |
| H12 | Ring 3 e chamadas de sistema | feito | 4 |
| H13 | Carregador ELF e libsagui | feito | 5 |
| H14 | Arquivos e pastas graváveis | feito | 4 |
| H15 | Pipes | feito | 2 |
| H16 | Shell e comandos como programas | feito | 14 |
| H17 | Framebuffer VESA | feito | 2 |
| H18 | Fonte própria | feito | 8 |
| H19 | Mouse / touchpad PS/2 | feito | 54 |
| H20 | Área de trabalho | feito | 9 |
| H21 | Gerenciador de janelas | em andamento |  |
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

- 2026-11-03T18:46:46: Início da H21: Gerenciador de janelas
- 2026-11-03T18:46:46: H20 concluída: Área de trabalho com degradê do Brasil, mascote em pixel art, barra com relógio e ícones
- 2026-11-03T18:46:46: Erro corrigido: Boot não terminava com a área de trabalho no ar
- 2026-11-03T18:38:00: Início da H20: Área de trabalho
- 2026-11-03T18:37:28: Eli confirmou: fase 4 ok
- 2026-11-03T18:27:00: Fase 4 entregue. Aguardando teste do Eli.
- 2026-11-03T18:27:00: H19 concluída: Mouse/touchpad PS/2; H19-fix: módulo i8042 único, teclado independente do touchpad
- 2026-11-03T18:12:39: Erro corrigido: G470: teclado parou e o touchpad não respondeu (fase 4)
- 2026-11-03T18:08:12: Bloqueio: G470: teclado não responde na fase 4 e o touchpad não respondeu (correção em andamento: H19-fix)
- 2026-11-03T17:40:18: Fase 4 entregue. Aguardando teste do Eli.
- 2026-11-03T17:40:18: Todos os testes passaram (H01 a H19, 19/19)
- 2026-11-03T17:38:51: H19 concluída: Mouse/touchpad PS/2 com IRQ 12, cursor próprio, cliques na serial
- 2026-11-03T17:32:50: Início da H19: Mouse / touchpad PS/2
- 2026-11-03T17:32:50: H18 concluída: Fonte 8x16 própria com acentos, terminal gráfico 128x48, edição com setas
- 2026-11-03T17:24:26: Início da H18: Fonte própria
