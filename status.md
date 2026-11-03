# Status do SaguiOS

_Atualizado em 2026-11-03T20:18:39_

- **Situação:** Trabalhando na H33: FAT32: gravação
- **Fase atual:** 6
- **História atual:** H33
- **Progresso geral:** 94.1%
- **Bloqueio:** nenhum
- **Imagem:** 491520 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 903, kernel 9161, userland 3769

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 100.0% | feito |
| 4 | Gráfico e mouse | 100.0% | feito |
| 5 | Janelas e programas | 100.0% | feito |
| 6 | Pendrive de dados | 66.7% | em andamento |
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
| H21 | Gerenciador de janelas | feito | 7 |
| H22 | Kit gráfico da libsagui | feito | 3 |
| H23 | Arquivos | feito | 4 |
| H24 | Terminal em janela | feito | 3 |
| H25 | Editor de texto | feito | 3 |
| H26 | Calculadora | feito | 2 |
| H27 | Sagui Pega Banana | feito | 2 |
| H28 | Acabamento | feito | 2 |
| H29 | PCI e controlador EHCI | feito | 1 |
| H30 | Enumeração USB, hubs e conexão a quente | feito | 1 |
| H31 | Armazenamento em massa | feito | 1 |
| H32 | FAT32: leitura | feito | 2 |
| H33 | FAT32: gravação | em andamento |  |
| H34 | Integração do pendrive | a fazer |  |
| B1 | Bônus: enxergar o HD interno | a fazer |  |
| B2 | Bônus: boot por UEFI | a fazer |  |
| B3 | Bônus: bip | a fazer |  |

## Últimas ações

- 2026-11-03T20:18:39: Início da H33: FAT32: gravação
- 2026-11-03T20:18:39: H32 concluída: FAT32 com nomes longos e acentos montado em /usb; ls e cat funcionando
- 2026-11-03T20:16:48: Início da H32: FAT32: leitura
- 2026-11-03T20:16:48: H31 concluída: Bulk-Only + SCSI; setores lidos e gravados conferidos byte a byte na VPS
- 2026-11-03T20:16:48: H30 concluída: Enumeração, hubs (validação no G470), conexão e remoção a quente
- 2026-11-03T20:16:48: H29 concluída: PCI, EHCI assumido da BIOS, transferências; comando usb
- 2026-11-03T20:16:48: Início da H31: Armazenamento em massa
- 2026-11-03T20:16:48: Início da H30: Enumeração USB, hubs e conexão a quente
- 2026-11-03T20:16:48: Início da H29: PCI e controlador EHCI
- 2026-11-03T20:04:57: Início da H29: PCI e controlador EHCI
- 2026-11-03T20:00:46: Eli confirmou: fase 5 ok
- 2026-11-03T19:24:19: Fase 5 entregue. Aguardando teste do Eli.
- 2026-11-03T19:24:19: Todos os testes passaram (H01 a H28, 28/28)
- 2026-11-03T19:14:10: H28 concluída: Ícones que abrem os programas, tela Sobre; imagem com 428 KB (29% da meta)
- 2026-11-03T19:11:50: Início da H28: Acabamento
