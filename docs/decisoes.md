# Decisões do SaguiOS

Toda decisão que o PRD não fixa, com o motivo. Também registra como cada parte
central foi pensada (prova de autoria, PRD seção 2.1).

## D001. Compilador: GCC do sistema com `-m32` freestanding (H01)

**Decisão:** usar o GCC 13 do Ubuntu com `-m32 -ffreestanding -nostdlib -fno-pic
-fno-stack-protector -mgeneral-regs-only`, e o `ld` com `-m elf_i386`, em vez de
compilar um `i686-elf-gcc`.

**Motivo:** o PRD aceita as duas opções. O GCC do sistema já está instalado,
gera código i386 correto com essas flags e evita uma hora de compilação de
binutils + GCC. O risco conhecido (o compilador assumir coisas do Linux) é
coberto pelas flags: sem libc, sem PIC, sem proteção de pilha, sem SSE/x87
(`-mgeneral-regs-only` garante que o kernel nunca toca a FPU, o que combina com a
decisão do PRD de não usar ponto flutuante).

## D002. Painel lido por `status.js`, não só por `status.json` (H01)

**Decisão:** `tools/status.py` grava `dashboard/status.json` (fonte única) e uma
cópia `dashboard/status.js` (`window.SAGUI_STATUS = {...}`).

**Motivo:** o Eli abre o painel direto do disco (baixado por FTP). Navegadores
bloqueiam `fetch()` de arquivos locais (`file://`), mas carregam `<script src>`.
O painel tenta o `.js` e, se não houver, cai para `fetch("status.json")`.

## D003. Carregador de boot: como foi pensado (H02, H03)

**Problema.** A BIOS só traz 512 bytes para a memória, em modo real, e só a
BIOS sabe ler o pendrive. O kernel precisa estar acima de 1 MB e rodar em modo
protegido, onde a BIOS não existe mais.

**Divisão em dois estágios.** O MBR faz o mínimo que cabe em 356 bytes de
código: guarda o disco (DL), lê um *cabeçalho de boot* no LBA 1 e, com ele, o
estágio 2. Todo o resto vai para o estágio 2, que pode crescer até 32 KB. O
cabeçalho é a única posição fixa; os demais LBAs, tamanhos e endereços são
calculados pelo `tools/mkimagem.py`. Assim mexer no kernel nunca exige mexer
no MBR.

**Formato do MBR.** Os primeiros 90 bytes imitam o início de um setor com BPB
(salto curto + NOP + área zerada). Motivo: algumas BIOS, ao dar boot por USB,
"corrigem" a geometria escrevendo nessa área; se houvesse código ali, ele
seria corrompido. A tabela de partições tem uma única partição ativa (0x80)
cobrindo a imagem a partir do LBA 1, porque várias BIOS só tratam um pendrive
como disco rígido (e não como disquete) quando encontram uma partição ativa
válida. O tipo escolhido é 0xDA ("dados sem sistema de arquivos"), para que
nenhum sistema tente montar o pendrive de boot.

**Leitura por LBA.** Só `INT 13h AH=42h`. CHS seria mais compatível com BIOS
muito antigas, mas o G470 (2011) e as VMs têm as extensões, e CHS dependeria
da geometria inventada pela BIOS para o pendrive, uma fonte clássica de
problemas. O MBR verifica as extensões (AH=41h) e avisa na tela se faltarem.
Cada leitura tem 3 tentativas com reset do disco entre elas.

**Acima de 1 MB: unreal mode.** O estágio 2 lê blocos de 32 KB num buffer em
0x10000 e copia cada um para o destino final. Para copiar acima de 1 MB sem
sair do modo real (precisamos da BIOS para o próximo bloco), entramos no modo
protegido só pelo tempo de carregar DS e ES com um descritor de 4 GB e
voltamos. O processador mantém o limite em cache. Isso é refeito antes de cada
cópia, porque uma rotina da BIOS pode ter recarregado os segmentos. A outra
opção (carregar tudo abaixo de 0x9F000) limitaria kernel + ramdisk a ~570 KB,
e a fase 5 vai passar disso.

**Onde fica o ramdisk.** Logo depois do fim da BSS do kernel, alinhado em
4 KB. O build lê o símbolo `_kernel_fim` do ELF. Se o ramdisk ficasse logo
depois do *arquivo* do kernel, o kernel apagaria o começo do ramdisk ao zerar
a própria BSS.

**Integridade.** O cabeçalho guarda uma soma simples dos bytes do kernel e do
ramdisk; o estágio 2 confere antes de saltar. No G470 não há serial: um
"dados lidos estão corrompidos" na tela é muito mais útil que uma tela preta.

**Progresso na tela.** `[1/6]` a `[6/6]`, como pede a seção 8 do PRD.

**A20.** Primeiro verifica se já está ligada (comparando 0x000500 com
0x100500); depois tenta, em ordem, o serviço da BIOS, a porta 0x92 e o
controlador de teclado, conferindo após cada um. O método usado vai na
boot_info e aparece na tela do kernel, para sabermos qual funcionou no G470.

**VESA.** O estágio 2 já percorre a lista de modos e escolhe o melhor
(1024x768 > 800x600 > 640x480; 32 bits > 24 bits; exige framebuffer linear e
cor direta), mas só troca de modo se o bit `OPC_VESA` do cabeçalho estiver
ligado, o que o build fará a partir da fase 4. A troca é a última coisa antes
do modo protegido, porque depois dela o texto da BIOS some.
