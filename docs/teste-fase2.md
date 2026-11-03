# Roteiro de teste: fase 2 (Kernel e terminal)

**Arquivos:** `entrega/fase2/saguios.vdi` (VirtualBox) e `entrega/fase2/saguios.img` (pendrive).
**Como gravar e configurar:** `docs/como-gravar.md` (para trocar o `.vdi` da VM, veja "Atualizar a VM numa fase nova").

Teste primeiro na máquina virtual. Se der erro, mande o print antes de ir para o G470.

---

## O que mudou desde a fase 1

- **Correção (H04-fix):** a saudação não sai mais do topo. O mapa de memória completo foi para o log; a tela mostra uma linha de resumo.
- O kernel agora trata interrupções, conta o tempo, controla a memória, lê o teclado ABNT2 e os arquivos do ramdisk, e abre um **terminal provisório** onde você digita comandos.

---

## Etapa 1: VirtualBox (`saguios.vdi`)

1. Troque o disco da VM pelo `saguios.vdi` novo e ligue.
2. **Tela inicial esperada** (veja `docs/capturas/H09-tela-inicial-ide.png`):
   - Faixa verde: `Ola! Eu sou o SaguiOS.` e, à direita, `versao 0.2 (fase 2)`.
   - `Memoria detectada: ... MB livres | usável (abaixo de 4 GB): ... MB`.
   - Quatro linhas com `[ ok ]`: Interrupções, Memória física, Teclado, Arquivos.
   - `Terminal provisório do kernel...` e o prompt `sagui:/$`.
3. **Digite os comandos abaixo**, um por vez, e aperte Enter:

| Comando | O que deve aparecer |
| --- | --- |
| `ajuda` | A lista de comandos com uma linha de explicação cada. |
| `ls` | `arte`, `bin`, `historia`, `tmp` (como `<pasta>`, em azul), `leia-me.txt` e `sobre.txt` com o tamanho. |
| `cat leia-me.txt` | O texto de boas-vindas. Como é maior que a tela, para em `-- mais --`: aperte Espaço para continuar ou `q` para sair. |
| `cd historia` | O prompt muda para `sagui:/historia$`. |
| `ls` | `linux.txt` e `saguios.txt`. |
| `cat linux.txt` | A história do Minix, do Linux e do GNU, com paginação. |
| `cd ../arte` | O prompt muda para `sagui:/arte$`. |
| `cat sagui.txt` | O mascote em arte ASCII. |
| `cd /` | Volta para `sagui:/$`. |
| `mem` | Memória total, gerenciada, usada e livre. |
| `data` | Dia da semana, data e hora (o VirtualBox usa a hora do seu PC). |
| `tempo` | Há quanto tempo o sistema está ligado. Rode duas vezes: o número aumenta. |
| `limpar` | A tela fica vazia, só com o prompt. |
| `reiniciar` | A VM reinicia e o SaguiOS carrega de novo. |

4. **Teste o teclado ABNT2:** digite (não precisa apertar Enter, ou aperte e veja o "Comando desconhecido"):
   `ação vovô pão ÇÃO à é í ó ú ü / ? ; : [ ] \ |`
   - Os acentos usam as teclas de acento: `´` e depois a letra dá `á`; `~` e depois `a` dá `ã`; Shift + `~` (que é o `^`) e depois `o` dá `ô`; Shift + `´` (que é o crase) e depois `a` dá `à`; Shift + `6` (que é o trema) e depois `u` dá `ü`.
   - **Limitação conhecida e aceita pelo PRD:** no modo texto a fonte é a da placa de vídeo, que não tem `ã`, `õ` nem as maiúsculas acentuadas (exceto `Ç` e `É`). Elas aparecem **sem o acento** na tela, mas o caractere certo chega ao sistema. A fonte própria (fase 4) resolve isso.

---

## Etapa 2: Lenovo G470 (`saguios.img` no pendrive)

1. Grave o `saguios.img` no pendrive (Rufus em modo DD, balenaEtcher ou `dd`) e dê boot (F12, **USB HDD**).
2. **O que você deve ver no G470:**
   1. A faixa verde com `Ola! Eu sou o SaguiOS.` **no topo** (a correção da fase 1).
   2. `Memoria detectada: ~8100 MB livres | usável (abaixo de 4 GB): ~3000 MB`, e a linha `Memória física: ~890 MB gerenciados...` (o kernel só usa os primeiros 896 MB de propósito; isso está explicado em `docs/decisoes.md`, D008).
   3. O prompt `sagui:/$` aceitando o **teclado do notebook**, com `ç` e acentos.
3. **Rode a mesma lista de comandos da etapa 1.** O mais importante no G470:
   - **Teclado:** digite `ação vovô ÇÃO / ? ; : [ ] \ |` e confira cada tecla, principalmente `/` e `?` (tecla ao lado do Shift direito), `ç`, `~`/`^` e `´`/`` ` ``. Se alguma tecla sair errada, me diga **qual tecla você apertou e o que apareceu** (foto ajuda).
   - **Caps Lock:** a luz do Caps Lock deve acender e apagar.
   - **`data`:** deve mostrar a data e a hora do relógio do notebook. A hora é a que estiver na BIOS (normalmente a hora local, se o Windows já foi usado nele).
   - **`tempo`:** rode, espere uns 10 segundos contando no relógio, rode de novo. A diferença deve ser de ~10 segundos.
   - **`reiniciar`:** o notebook deve reiniciar.
4. **Fotografe** a tela inicial (logo depois de ligar) e a tela depois de `mem` e `data`.

**Se der errado**, fotografe a tela e me diga:

- Uma **tela vermelha** "O SaguiOS encontrou um erro e precisou parar": a foto dela mostra o nome do erro e os registradores; é o que preciso para achar a causa.
- O sistema **não responde ao teclado**: me diga se o Caps Lock acende ao ser apertado.
- O **relógio parado** (`tempo` sempre 0): o temporizador não está gerando interrupções no G470.

Quando testar, me diga **"fase 2 ok"** ou descreva o que aconteceu.
