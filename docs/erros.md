# Erros do SaguiOS

Cada erro encontrado: sintoma, causa e correção. Nada é escondido (PRD seção 6).

| # | História | Sintoma | Causa | Correção |
| --- | --- | --- | --- | --- |
| 1 | H04-fix | No G470 a saudação `Ola! Eu sou o SaguiOS.` e a linha `Memoria detectada` não aparecem na foto. | O G470 informa ~20 regiões no E820 (o QEMU, 6). O kernel imprimia todas na tela de 25 linhas, que rolou ~8 linhas e levou o topo junto. | Mapa completo só na serial; na tela, um resumo de uma linha. Também mostra separadamente a RAM utilizável abaixo de 4 GB (o kernel é 32 bits). Teste com mapa grande simulado. |
| 2 | H07 | Na captura do teclado, a faixa da saudação e a linha de memória quebravam em duas linhas. | Os textos passavam de 80 colunas (a versão "0.2 (fase 2)" e a memória com 4 dígitos). | Faixa montada com larguras fixas que somam 80; linha de memória encurtada. |
| 3 | H08 | Na captura do SGFS, a árvore saiu como `%*s` repetido. | O `formatar()` do kernel não aceitava largura vinda dos argumentos (`*`) e os argumentos ficaram desalinhados. | `formatar()` passou a aceitar `*` (com valor negativo = alinhar à esquerda). |
