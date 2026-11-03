# Status do SaguiOS

_Atualizado em 2026-11-03T19:03:23_

- **Situação:** Trabalhando na H23: Arquivos
- **Fase atual:** 5
- **História atual:** H23
- **Progresso geral:** 70.6%
- **Bloqueio:** nenhum
- **Imagem:** 347648 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 903, kernel 6740, userland 2715

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 100.0% | feito |
| 4 | Gráfico e mouse | 100.0% | feito |
| 5 | Janelas e programas | 55.6% | em andamento |
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
| H21 | Gerenciador de janelas | feito | 7 |
| H22 | Kit gráfico da libsagui | feito | 3 |
| H23 | Arquivos | em andamento |  |
| H24 | Terminal em janela | feito | 3 |
| H25 | Editor de texto | feito | 3 |
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

- 2026-11-03T19:03:23: Início da H23: Arquivos
- 2026-11-03T19:03:23: H25 concluída: Editor com Novo/Abrir/Salvar, Ctrl+S, título com *, aviso ao fechar
- 2026-11-03T19:00:05: Início da H25: Editor de texto
- 2026-11-03T19:00:05: H24 concluída: Terminal em janela rodando o sagsh por pipes; comandos que abrem os programas gráficos
- 2026-11-03T18:56:46: Início da H24: Terminal em janela
- 2026-11-03T18:56:46: H22 concluída: Kit gráfico na libsagui: botão, caixa, área de texto com rolagem, lista, diálogos
- 2026-11-03T18:53:31: Início da H22: Kit gráfico da libsagui
- 2026-11-03T18:53:31: H21 concluída: Gerenciador de janelas no kernel, memória compartilhada, eventos por janela; arrastar e fechar testados
- 2026-11-03T18:53:30: Erro corrigido: Rastros ao arrastar janelas
- 2026-11-03T18:46:46: Início da H21: Gerenciador de janelas
- 2026-11-03T18:46:46: H20 concluída: Área de trabalho com degradê do Brasil, mascote em pixel art, barra com relógio e ícones
- 2026-11-03T18:46:46: Erro corrigido: Boot não terminava com a área de trabalho no ar
- 2026-11-03T18:38:00: Início da H20: Área de trabalho
- 2026-11-03T18:37:28: Eli confirmou: fase 4 ok
- 2026-11-03T18:27:00: Fase 4 entregue. Aguardando teste do Eli.
