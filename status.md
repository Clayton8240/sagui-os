# Status do SaguiOS

_Atualizado em 2026-11-03T17:21:51_

- **Situação:** Fase 3 confirmada no hardware. Seguindo para a fase 4.
- **Fase atual:** 3
- **História atual:** -
- **Progresso geral:** 47.1%
- **Bloqueio:** nenhum
- **Imagem:** 224256 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 899, kernel 4659, userland 1552

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 100.0% | feito |
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
| H12 | Ring 3 e chamadas de sistema | feito | 4 |
| H13 | Carregador ELF e libsagui | feito | 5 |
| H14 | Arquivos e pastas graváveis | feito | 4 |
| H15 | Pipes | feito | 2 |
| H16 | Shell e comandos como programas | feito | 14 |
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

- 2026-11-03T17:21:51: Eli confirmou: fase 3 ok
- 2026-11-03T16:45:51: Fase 3 entregue. Aguardando teste do Eli.
- 2026-11-03T16:45:51: Todos os testes passaram (H01 a H16, 16/16)
- 2026-11-03T16:41:25: H16 concluída: sagsh no anel 3 com redirecionamento, pipes, histórico e Tab; 18 programas com apelidos; terminal provisório removido
- 2026-11-03T16:41:24: Erro corrigido: falha divisao não dividia
- 2026-11-03T16:27:11: Início da H16: Shell e comandos como programas
- 2026-11-03T16:27:11: H15 concluída: Pipes com bloqueio e fim de arquivo; 20.000 bytes num pipe de 512, nos dois sentidos
- 2026-11-03T16:25:45: Erro corrigido: Regressão: 'Ola do ring 3!' sumiu no teste da H12
- 2026-11-03T16:25:17: Início da H15: Pipes
- 2026-11-03T16:25:17: H14 concluída: Árvore na RAM com cópia na escrita, descritores por processo, terminal como 0/1; 28 verificações no anel 3
- 2026-11-03T16:21:08: Início da H14: Arquivos e pastas graváveis
- 2026-11-03T16:21:08: H13 concluída: Carregador ELF32 e libsagui; /bin/ola imprime os argumentos
- 2026-11-03T16:16:35: Início da H13: Carregador ELF e libsagui
- 2026-11-03T16:16:35: H12 concluída: Programas no anel 3, int 0x80 com validação de ponteiros; cli e acesso ao kernel encerram só o processo
- 2026-11-03T16:12:46: Início da H12: Ring 3 e chamadas de sistema
