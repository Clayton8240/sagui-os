# Status do SaguiOS

_Atualizado em 2026-11-03T16:12:46_

- **Situação:** Trabalhando na H12: Ring 3 e chamadas de sistema
- **Fase atual:** 3
- **História atual:** H12
- **Progresso geral:** 32.4%
- **Bloqueio:** nenhum
- **Imagem:** 47104 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 899, kernel 3252, userland 0

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 28.6% | em andamento |
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
| H05 | Interrupções e relógio | feito | 4 |
| H06 | Memória física | feito | 4 |
| H07 | Teclado PS/2 com ABNT2 | feito | 6 |
| H08 | Sistema de arquivos SGFS | feito | 2 |
| H09 | Terminal provisório do kernel | feito | 2 |
| H10 | Paginação | feito | 3 |
| H11 | Processos e escalonador | feito | 3 |
| H12 | Ring 3 e chamadas de sistema | em andamento |  |
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

- 2026-11-03T16:12:46: Início da H12: Ring 3 e chamadas de sistema
- 2026-11-03T16:12:46: H11 concluída: Escalonador round-robin preemptivo; tarefas A e B alternam a cada 20 ms sem cooperar
- 2026-11-03T16:10:05: Início da H11: Processos e escalonador
- 2026-11-03T16:10:05: H10 concluída: Paginação de 2 níveis, espaços por processo, tela de falta de página
- 2026-11-03T16:07:26: Início da H10: Paginação
- 2026-11-03T16:05:16: Eli confirmou: fase 2 ok
- 2026-11-03T15:45:24: Fase 2 entregue. Aguardando teste do Eli.
- 2026-11-03T15:45:16: Todos os testes passaram (H01 a H09, 9/9), IDE e USB
- 2026-11-03T15:41:51: H09 concluída: Terminal provisório com ajuda, ls, cd, cat (paginado), limpar, mem, data, tempo, reiniciar
- 2026-11-03T15:41:51: Erro corrigido: cd ../arte chegava como cd ..;arte
- 2026-11-03T15:39:25: Início da H09: Terminal provisório do kernel
- 2026-11-03T15:39:25: H08 concluída: mksgfs.py + leitor no kernel; lista igual ao rootfs/ e leia-me.txt idêntico
- 2026-11-03T15:39:25: Erro corrigido: Saudação e memória quebravam linha
- 2026-11-03T15:39:25: Erro corrigido: Árvore do SGFS saiu como %*s na tela
- 2026-11-03T15:37:48: Início da H08: Sistema de arquivos SGFS
