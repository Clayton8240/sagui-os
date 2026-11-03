```
Eli, terminei a fase 5: Janelas e programas. Pode testar!

O que foi feito: o SaguiOS liga direto numa área de trabalho nas cores do
Brasil, com o sagui em pixel art, relógio na barra e ícones. O kernel cuida
das janelas (molduras, arrastar, foco, fechar, cliques e teclas) e cada
programa desenha o próprio conteúdo no anel 3: Arquivos, Terminal (com o
sagsh dentro, abre sozinho ao ligar), Editor de texto, Calculadora (ponto
fixo) e o jogo Sagui Pega Banana, além da tela Sobre. Duplo clique nos
ícones abre os programas.
Histórias concluídas: H20 a H28
Imagem: entrega/fase5/saguios.vdi (VirtualBox, testar primeiro) e saguios.img (pendrive, G470)
Roteiro de teste: docs/teste-fase5.md
O que você deve ver no G470: a área de trabalho com o Terminal aberto; o
touchpad arrastando janelas e o duplo clique abrindo os programas; o Sagui
Pega Banana jogável com as setas.
Problemas conhecidos: o botão direito ainda não tem função (fase 6, pendrive).
Os arquivos criados somem ao desligar (o pendrive de dados é a fase 6).

Quando testar, me diga "fase 5 ok" ou descreva o que aconteceu.
```

# Status do SaguiOS

_Atualizado em 2026-11-03T19:24:19_

- **Situação:** Aguardando teste do Eli: fase 5
- **Fase atual:** 5
- **História atual:** -
- **Progresso geral:** 82.4%
- **Bloqueio:** nenhum
- **Imagem:** 429056 bytes úteis (meta 1474560 bytes = 1,44 MB)
- **Linhas de código:** boot 903, kernel 6764, userland 3628

## Fases

| Fase | Nome | Progresso | Estado |
| --- | --- | --- | --- |
| 1 | Boot | 100.0% | feito |
| 2 | Kernel e terminal | 100.0% | feito |
| 3 | Processos e programas | 100.0% | feito |
| 4 | Gráfico e mouse | 100.0% | feito |
| 5 | Janelas e programas | 100.0% | aguardando teste |
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
| H20 | Área de trabalho | aguardando teste | 9 |
| H21 | Gerenciador de janelas | aguardando teste | 7 |
| H22 | Kit gráfico da libsagui | aguardando teste | 3 |
| H23 | Arquivos | aguardando teste | 4 |
| H24 | Terminal em janela | aguardando teste | 3 |
| H25 | Editor de texto | aguardando teste | 3 |
| H26 | Calculadora | aguardando teste | 2 |
| H27 | Sagui Pega Banana | aguardando teste | 2 |
| H28 | Acabamento | aguardando teste | 2 |
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

- 2026-11-03T19:24:19: Fase 5 entregue. Aguardando teste do Eli.
- 2026-11-03T19:24:19: Todos os testes passaram (H01 a H28, 28/28)
- 2026-11-03T19:14:10: H28 concluída: Ícones que abrem os programas, tela Sobre; imagem com 428 KB (29% da meta)
- 2026-11-03T19:11:50: Início da H28: Acabamento
- 2026-11-03T19:11:50: H27 concluída: Jogo Sagui Pega Banana com pixel art, vidas, velocidade crescente e recorde
- 2026-11-03T19:09:30: Início da H27: Sagui Pega Banana
- 2026-11-03T19:09:30: H26 concluída: Calculadora em ponto fixo com botões e teclado; avisos de divisão por zero e número grande
- 2026-11-03T19:07:26: Início da H26: Calculadora
- 2026-11-03T19:07:26: H23 concluída: Gerenciador de arquivos com ícones; abre no Editor, cria e apaga pastas
- 2026-11-03T19:03:23: Início da H23: Arquivos
- 2026-11-03T19:03:23: H25 concluída: Editor com Novo/Abrir/Salvar, Ctrl+S, título com *, aviso ao fechar
- 2026-11-03T19:00:05: Início da H25: Editor de texto
- 2026-11-03T19:00:05: H24 concluída: Terminal em janela rodando o sagsh por pipes; comandos que abrem os programas gráficos
- 2026-11-03T18:56:46: Início da H24: Terminal em janela
- 2026-11-03T18:56:46: H22 concluída: Kit gráfico na libsagui: botão, caixa, área de texto com rolagem, lista, diálogos
