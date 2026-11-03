# Roteiro de teste: fase 3 (Processos e programas)

**Arquivos:** `entrega/fase3/saguios.vdi` (VirtualBox) e `entrega/fase3/saguios.img` (pendrive).
**Como gravar e configurar:** `docs/como-gravar.md` (para trocar o `.vdi` da VM, veja "Atualizar a VM numa fase nova").

Teste primeiro na máquina virtual. Se der erro, mande o print antes de ir para o G470.

---

## O que mudou desde a fase 2

O terminal provisório do kernel foi removido. Agora:

- o kernel liga a **paginação** (cada programa tem o próprio espaço de memória);
- o primeiro programa é o **shell `sagsh`**, que roda no **anel 3** (sem privilégios);
- cada comando (`ls`, `cat`, `cp`, `ps`...) é um **programa separado** em `/bin`;
- vários programas rodam **ao mesmo tempo** (o relógio troca de programa a cada 20 ms);
- dá para **criar, gravar e apagar** arquivos e pastas (na memória: somem ao desligar);
- dá para ligar programas com **pipes** (`ls bin | conta`) e **redirecionar** para arquivos (`>`, `>>`, `<`).

---

## Etapa 1: VirtualBox (`saguios.vdi`)

1. Troque o disco da VM pelo `saguios.vdi` novo e ligue.
2. **Tela inicial esperada:** a faixa verde com `Ola! Eu sou o SaguiOS.` e `versao 0.3 (fase 3)`, seis linhas `[ ok ]` (agora com **Paginação** e **Processos**), a linha `sagsh: o shell do SaguiOS (processo 1)` e o prompt `sagui:/$`.
3. **Digite os comandos abaixo**, na ordem:

| Comando | O que deve aparecer |
| --- | --- |
| `ajuda` | Duas colunas com todos os comandos e apelidos (`ls/listar`, `cat/ver`, ...). |
| `ajuda conta` | Explicação do comando `wc` (apelido `conta`). |
| `ls` | As pastas (`arte/`, `bin/`, `historia/`, `testes/`, `tmp/`) e os arquivos com tamanho. |
| `listar bin` | Os 18 programas, cada um marcado como `programa`. |
| `ls bin \| conta` | `18 linhas, ...` (a saída do `ls` passa pelo pipe e o `wc` conta). A barra vertical fica com Shift + a tecla `\` ao lado do Shift esquerdo. |
| `echo ola > tmp/a.txt` | Nada (o texto foi para o arquivo). |
| `cat tmp/a.txt` | `ola` |
| `eco "mais uma" >> tmp/a.txt` e depois `ver tmp/a.txt` | `ola` e `mais uma`. |
| `cp tmp/a.txt tmp/b.txt` e depois `ls tmp` | `a.txt` e `b.txt`. |
| `conta < tmp/a.txt` | `2 linhas, 3 palavras, ...` |
| `mkdir tmp/pasta`, `ls tmp`, `rmdir tmp/pasta` | A pasta aparece e depois some. |
| `contador 10 &` | `[N] contador em segundo plano`. Nos próximos 10 segundos aparecem linhas `[contador N] 1 de 10 segundos`... **enquanto você continua digitando**. |
| `ps` (logo depois) | A lista de processos: `ocioso`, `sagsh`, `contador` e o próprio `ps`, com memória e tempo de CPU. |
| `falha` | `[SaguiOS] Processo N (falha) encerrado: Falta de página...` e **o shell continua** (o prompt volta). |
| `falha instrucao` | `... encerrado: Proteção geral...` e o shell continua. |
| `mem`, `data`, `sobre` | Memória, data e hora, e os créditos. |
| Seta para cima | Volta o último comando. |
| `cat /his` e aperte **Tab** | Completa para `cat /historia/`. Digite `li` e Tab de novo: `linux.txt`. Enter. |
| `sair` | `O shell foi fechado. Abrindo um novo...` e um prompt novo. |
| `reiniciar` | A VM reinicia. |

---

## Etapa 2: Lenovo G470 (`saguios.img` no pendrive)

1. Grave o `saguios.img` no pendrive e dê boot (F12, **USB HDD**).
2. **O que você deve ver no G470:**
   1. As seis linhas `[ ok ]`, incluindo **Paginação** e **Processos**, e o prompt `sagui:/$` do sagsh.
   2. `contador 10 &` contando sozinho **enquanto** você digita `ps` e outros comandos (é a multitarefa).
   3. `falha` encerrando só o programa, sem tela vermelha, e o shell continuando.
3. Rode a mesma lista da etapa 1. No G470, preste atenção principalmente em:
   - **paginação e processos:** é a primeira vez que o notebook roda com paginação ligada e com programas no anel 3. Se aparecer uma **tela vermelha** logo no boot, a foto dela é essencial;
   - **teclado no shell:** setas (histórico), Tab, Backspace, `|` (Shift + `\`), `>` (Shift + `.`), `<` (Shift + `,`), `&` (Shift + `7`) e as **aspas** (Shift + a tecla `'` ao lado do 1);
   - **acentos:** se puder, teste agora as teclas mortas que faltaram na fase 2: `echo ação vovô pão à é ü` (lembre que no modo texto `ã` e `õ` aparecem sem til; a fonte própria vem na fase 4).
4. **Fotografe** a tela depois do `ps` com o contador rodando e a tela depois do `falha`.

**Se der errado**, fotografe a tela e me diga:

- **Tela vermelha:** a foto mostra o nome do erro, o endereço e os registradores.
- **O shell não aparece** (para depois das linhas `[ ok ]`): me diga qual é a última linha na tela.
- **O contador não conta** enquanto você digita: me diga o que o `ps` mostra.

Quando testar, me diga **"fase 3 ok"** ou descreva o que aconteceu.
