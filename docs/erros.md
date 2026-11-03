# Erros do SaguiOS

Cada erro encontrado: sintoma, causa e correção. Nada é escondido (PRD seção 6).

| # | História | Sintoma | Causa | Correção |
| --- | --- | --- | --- | --- |
| 1 | H04-fix | No G470 a saudação `Ola! Eu sou o SaguiOS.` e a linha `Memoria detectada` não aparecem na foto. | O G470 informa ~20 regiões no E820 (o QEMU, 6). O kernel imprimia todas na tela de 25 linhas, que rolou ~8 linhas e levou o topo junto. | Mapa completo só na serial; na tela, um resumo de uma linha. Também mostra separadamente a RAM utilizável abaixo de 4 GB (o kernel é 32 bits). Teste com mapa grande simulado. |
| 2 | H07 | Na captura do teclado, a faixa da saudação e a linha de memória quebravam em duas linhas. | Os textos passavam de 80 colunas (a versão "0.2 (fase 2)" e a memória com 4 dígitos). | Faixa montada com larguras fixas que somam 80; linha de memória encurtada. |
| 3 | H08 | Na captura do SGFS, a árvore saiu como `%*s` repetido. | O `formatar()` do kernel não aceitava largura vinda dos argumentos (`*`) e os argumentos ficaram desalinhados. | `formatar()` passou a aceitar `*` (com valor negativo = alinhar à esquerda). |
| 4 | H09 | `cd ../arte` chegava ao kernel como `cd ..;arte`. | Erro do harness de testes, não do kernel: ele digitava `/` pela tecla `slash` do layout americano, que no ABNT2 é o `;`. | O harness passou a usar as posições ABNT2 (`/` = tecla `ro`, `;` = `slash`, `'` = `grave_accent`...). |
| 5 | H14 | Regressão no teste da H12: `Ola do ring 3!` não aparecia mais (o resto do teste passava). | Com a H14, a saída 1 virou um descritor de verdade. Os programinhas da H12 são criados direto pelo autoteste, sem o carregador, e ficaram sem descritores; o `escrever(1)` devolvia "descritor inválido". | O autoteste dá a eles o terminal como 0, 1 e 2 (`arquivos_herdar(p, NULL)`). O commit da H14 foi feito antes de eu notar a falha; a correção vai no commit seguinte. |
