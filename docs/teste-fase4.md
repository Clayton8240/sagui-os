# Roteiro de teste: fase 4 (Gráfico e mouse)

**Arquivos:** `entrega/fase4/saguios.vdi` (VirtualBox) e `entrega/fase4/saguios.img` (pendrive).
**Como gravar e configurar:** `docs/como-gravar.md` (para trocar o `.vdi` da VM, veja "Atualizar a VM numa fase nova").

Teste primeiro na máquina virtual. Se der erro, mande o print antes de ir para o G470.

---

## Correção depois do primeiro teste no G470 (H19-fix)

No primeiro teste desta fase, o touchpad "não respondeu" e **o teclado parou de funcionar**. A causa foi minha: a inicialização do touchpad mexia no mesmo chip do teclado e podia deixar o teclado desligado ou com a fila travada (detalhes no erro 9 de `docs/erros.md`). Agora:

- o teclado **não depende mais** do touchpad: mesmo que o touchpad não responda, o teclado tem que funcionar;
- se o touchpad não responder, aparece um aviso com **números de diagnóstico**, por exemplo `(cfg 47->45, F6=fff FF=fff F4=fff)`. **Fotografe essa linha**: ela me diz o que o chip e o touchpad responderam.

## O que mudou desde a fase 3

- A tela agora é **gráfica (1024x768)**, desenhada pixel a pixel pelo SaguiOS, com uma **fonte própria** que tem todos os acentos (`ã`, `õ`, `Ç`, `É`... aparecem certos).
- O **touchpad/mouse** move uma **seta** desenhada pelo SaguiOS.
- **Pedidos seus da fase 3:**
  - as setas **← →**, **Home**, **End** e **Delete** editam no meio da linha do shell;
  - o texto de um programa em segundo plano (`contador 10 &`) não se mistura mais com o que você está digitando: o shell reescreve o prompt e a sua linha logo abaixo.

Esta fase ainda não tem janelas nem área de trabalho: isso é a fase 5. O mouse já se move e os cliques são detectados, mas ainda não há onde clicar.

---

## Etapa 1: VirtualBox (`saguios.vdi`)

1. Troque o disco da VM pelo `saguios.vdi` novo e ligue.
   - Se a VM abrir numa janela pequena, use **Exibir > Tela cheia** ou aumente a janela: a tela do SaguiOS é 1024x768.
   - O VirtualBox pode "capturar" o mouse ao clicar na janela; a tecla **Ctrl da direita** solta.
2. **Tela inicial esperada** (veja `docs/capturas/H19-cursor-movido.png`):
   - fundo azul-escuro, faixa verde no topo com `Ola! Eu sou o SaguiOS.` e `versão 0.4 (fase 4)`;
   - **oito** linhas `[ ok ]`, incluindo **Vídeo: 1024x768, 32 bits, fonte própria** e **Mouse/touchpad PS/2**;
   - o prompt `sagui:/$` e uma **seta branca** no meio da tela.
3. Teste:

| O que fazer | O que deve acontecer |
| --- | --- |
| Mexer o mouse | A seta acompanha, sem deixar rastro, e para nas bordas. |
| `echo Ação, vovô, pão, ÇÃO, à é í ó ú` | O texto aparece **com todos os acentos certos** (inclusive `ã`, `õ`, `Ç`, `Ã`). |
| `cat /historia/linux.txt` | O texto rola na tela, com acentos. |
| Digite `echo abc`, aperte **←** duas vezes, digite `X`, aperte **End**, digite `!` e Enter | Sai `aXbc!` (a edição no meio da linha funcionou). |
| Digite `cho oi`, aperte **Home**, digite `e` e Enter | Sai `oi`. |
| `contador 5 &` e comece a digitar `echo fim` devagar | A cada segundo aparece `[contador N] ...` numa linha própria e o prompt é redesenhado com o que você já tinha digitado. |
| `ls`, `ps`, `ajuda` | Como na fase 3, agora em tela gráfica. |
| `falha` | O processo é encerrado com a mensagem, o shell continua. |

---

## Etapa 2: Lenovo G470 (`saguios.img` no pendrive)

1. Grave o `saguios.img` no pendrive e dê boot (F12, **USB HDD**).
2. **O que você deve ver no G470:**
   1. As linhas `[1/6]` a `[6/6]` em modo texto e, logo depois, a **tela gráfica** com a faixa verde, a fonte nova e as oito linhas `[ ok ]`.
   2. A **seta do mouse** mexendo com o **touchpad** (e com os botões do touchpad sem travar nada).
   3. `echo Ação, vovô, pão, ÇÃO` com os acentos certos.
3. Atenção especial no G470:
   - **Vídeo:** é a primeira vez que o notebook liga o modo gráfico VESA (a placa Intel informou 1024x768x32, modo 0x118, na fase 1). Se a tela ficar **preta** depois do `[6/6]`, ou com **cores trocadas** (por exemplo, a faixa azul em vez de verde), fotografe: isso diz se é o modo ou a ordem das cores.
   - **Touchpad:** veja se a velocidade da seta é razoável (muito lenta ou muito rápida eu ajusto).
   - **Velocidade:** veja se o texto aparece rápido ao digitar e no `cat` (o vídeo do G470 pode ser mais lento que o da VM).
4. **Fotografe** a tela inicial com a seta visível e a tela depois do `echo` com acentos.

**Se der errado**, fotografe a tela e me diga:

- **O teclado não funciona:** fotografe a tela inicial inteira (as linhas `[ ok ]` e qualquer `[aviso]`) e me diga se a luz do Caps Lock acende ao apertar.

- **Tela preta** depois do `[6/6]`: o modo gráfico não ligou ou o kernel travou antes de desenhar. Me diga se a luz do Caps Lock acende ao apertar a tecla (isso mostra se o sistema está vivo).
- **Voltou para o modo texto com o aviso** `Modo gráfico indisponível`: o estágio 2 não conseguiu ligar o VESA; o resto do sistema deve funcionar normalmente.
- **A seta não mexe:** veja se aparece `[aviso] Mouse/touchpad PS/2 não respondeu` na tela inicial.

Quando testar, me diga **"fase 4 ok"** ou descreva o que aconteceu.
