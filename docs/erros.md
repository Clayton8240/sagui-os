# Erros do SaguiOS

Cada erro encontrado: sintoma, causa e correção. Nada é escondido (PRD seção 6).

| # | História | Sintoma | Causa | Correção |
| --- | --- | --- | --- | --- |
| 1 | H04-fix | No G470 a saudação `Ola! Eu sou o SaguiOS.` e a linha `Memoria detectada` não aparecem na foto. | O G470 informa ~20 regiões no E820 (o QEMU, 6). O kernel imprimia todas na tela de 25 linhas, que rolou ~8 linhas e levou o topo junto. | Mapa completo só na serial; na tela, um resumo de uma linha. Também mostra separadamente a RAM utilizável abaixo de 4 GB (o kernel é 32 bits). Teste com mapa grande simulado. |
