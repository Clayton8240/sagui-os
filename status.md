# Status do SaguiOS

_Atualizado em 2026-11-03T15:24:42_

- **Situação:** Fase 1 confirmada no hardware. Seguindo para a fase 2.
- **Fase atual:** 1
- **História atual:** -
- **Progresso geral:** 11.8%
- **Bloqueio:** nenhum
- **Imagem:** 14848 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 893, kernel 728, userland 0

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 0.0% | a fazer |
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
| H05 | Interrupções e relógio | a fazer |  |
| H06 | Memória física | a fazer |  |
| H07 | Teclado PS/2 com ABNT2 | a fazer |  |
| H08 | Sistema de arquivos SGFS | a fazer |  |
| H09 | Terminal provisório do kernel | a fazer |  |
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

- 2026-11-03T15:24:42: H04-fix concluída: mapa E820 só na serial, saudação fica no topo; memória abaixo de 4 GB mostrada à parte
- 2026-11-03T15:23:28: Início da H04-fix: saudação some no G470 por causa do mapa E820 grande
- 2026-11-03T15:22:25: Eli confirmou: fase 1 ok
- 2026-11-03T14:27:18: Fase 1 entregue. Aguardando teste do Eli.
- 2026-11-03T14:27:10: Todos os testes da fase 1 passaram (H01 a H04, IDE e USB)
- 2026-11-03T14:27:10: H04 concluída: Imagem .img e .vdi com SHA-256; boot do .img (IDE e USB) e do .vdi no QEMU; como-gravar.md
- 2026-11-03T14:22:52: Início da H04: Imagem para pendrive e VirtualBox
- 2026-11-03T14:22:52: H03 concluída: Estágio 2 completo e kernel em C em modo protegido; testado em IDE e USB com 512 e 128 MB
- 2026-11-03T14:22:05: Início da H03: Estágio 2 e modo protegido
- 2026-11-03T14:22:05: H02 concluída: MBR de 512 bytes com BPB reservado e partição ativa; mensagem aparece em IDE e USB
- 2026-11-03T14:14:29: Início da H02: Estágio 1 (MBR)
- 2026-11-03T14:14:17: H01 concluída: Ambiente pronto: NASM, GCC -m32, QEMU, ImageMagick; estrutura, git e painel criados
- 2026-11-03T14:12:34: Início da H01: Preparar o ambiente
