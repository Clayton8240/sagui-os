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

## D004. Tamanho dos arquivos de imagem (H04)

**Decisão:** o `saguios.img` tem no mínimo 1 MB (e depois cresce em múltiplos
de 32 KB); o disco do `saguios.vdi` é arredondado para múltiplos de 1 MB.

**Motivo:** a primeira imagem tinha 32 KB. BIOS e VirtualBox inventam uma
geometria (cilindros, cabeças, setores) a partir do tamanho do disco, e um
disco menor que um cilindro é um caso pouco testado neles. 1 MB evita isso e
continua abaixo da meta de 1,44 MB. A meta do PRD é medida pelos **bytes
úteis** (MBR + cabeçalho + estágio 2 + kernel + ramdisk), que o build grava em
`build/tamanho.txt` e o painel mostra.

## D005. `.vdi` gerado pelo `qemu-img`, testado no QEMU (H04)

**Decisão:** o `.vdi` é convertido com `qemu-img convert -f raw -O vdi`, como
pede o PRD, e o teste da H04 dá boot no próprio `.vdi` pelo QEMU
(`-drive format=vdi`).

**Motivo:** a VPS não tem virtualização aninhada, então não há VirtualBox
para testar aqui. Dar boot no `.vdi` prova que a conversão preservou a
imagem byte a byte; o teste no VirtualBox de verdade fica no roteiro do Eli.

## D006. Autotestes ligados pelo cabeçalho de boot (fase 2)

**Decisão:** o cabeçalho de boot tem um campo de 16 bytes (`CAB_TESTE`) com o
nome de um autoteste. O estágio 2 copia para a boot_info e o kernel, se o
campo não estiver vazio, roda `autoteste_executar(nome)` depois de iniciar.
Os testes geram imagens próprias com `mkimagem.py --teste NOME`.

**Motivo:** critérios como "dividir por zero mostra a tela de erro" ou
"alocar e liberar sem corromper" precisam de código que nunca deve rodar na
imagem do Eli. Recompilar o kernel para cada teste seria lento e testaria um
binário diferente do entregue. Assim o kernel testado é exatamente o mesmo;
só muda um campo do cabeçalho.

## D007. Interrupções: como foi pensado (H05)

**Um trecho por vetor, gerado por repetição.** A CPU não informa qual vetor
disparou, e só algumas exceções empilham código de erro. Cada um dos 256
vetores tem um trecho de 2-3 instruções, gerado por `%rep` no NASM, que iguala
o formato (empilha 0 quando não há código de erro) e empilha o número do
vetor. Um único trecho comum salva o resto. A tabela com o endereço de cada
trecho é exportada para o C, que monta a IDT num laço.

**O despachante devolve o quadro a retomar.** `tratar_interrupcao(quadro)`
retorna um ponteiro de quadro, e o assembly faz `mov esp, eax` antes de
restaurar os registradores. Hoje sempre devolve o mesmo quadro. Na fase 3, a
troca de processo vai ser só o escalonador devolver o quadro de outro
processo: não precisa de uma rotina separada de troca de contexto.

**Portas de interrupção, não de "trap".** IF é desligada na entrada, então o
kernel não é interrompido no meio de outra interrupção. É mais simples de
raciocinar e o kernel é pequeno o bastante para isso não atrasar nada.

**PIC.** Remapeado para 0x20-0x2F (os vetores 8-15 originais colidem com
exceções da CPU), tudo mascarado, cada driver habilita a sua linha. IRQ 7 e 15
espúrias são reconhecidas pelo registrador de serviço e não recebem EOI.

**Tela de erro.** Nome da exceção em português, código de erro, todos os
registradores e CR0/CR2/CR3, e um pedido para fotografar: no G470 a foto é a
única ferramenta de depuração.

**Texto em Latin-1 também no modo texto.** O kernel é compilado com
`-fexec-charset=ISO-8859-1`, então as mensagens podem ter acentos. A fonte
da placa VGA segue a página de código 437; o driver traduz o que existe nela
(ç, é, á, ó, ...) e troca pela letra sem acento o que não existe (ã, õ).

## D008. Memória física e alocador do kernel: como foi pensado (H06)

**Até onde o kernel gerencia.** O kernel é de 32 bits sem PAE, então nunca
enxerga memória acima de 4 GB (no G470, com ~8 GB, metade fica de fora; a
tela mostra os dois números). Indo além: na fase 3, com a paginação, os
programas vão morar a partir de 0x40000000 (PRD 4), e o kernel vai ficar
mapeado 1:1 (endereço virtual = físico) abaixo disso em todos os espaços de
endereço, para poder ler e escrever qualquer página física sem truques. A
faixa 0x38000000-0x3FFFFFFF fica reservada para o kernel mapear coisas que
estão em endereços físicos altos (framebuffer de vídeo, registradores do
USB). Por isso o gerenciador de páginas só entrega memória abaixo de
**0x38000000 (896 MB)**. É muito mais do que o SaguiOS precisa, e em troca
o kernel nunca precisa de mapeamento temporário para tocar numa página.

**Bitmap de páginas de 4 KB.** Um bit por página, 1 = ocupada. Começa tudo
ocupado; liberamos só as regiões "livres" do E820 (arredondadas para dentro,
para nunca entregar meia página de uma região reservada); depois marcamos de
novo como ocupados o primeiro 1 MB (BIOS, tabelas, nossas estruturas de
boot), o kernel com a BSS e o ramdisk. 896 MB / 4 KB = 229.376 bits = 28 KB
de bitmap, que fica na BSS. A busca olha 32 páginas de uma vez (uma palavra
inteira cheia = nada livre ali) e continua de onde parou da última vez.

**`kmalloc` em duas faixas.**
- *Pequenos (até 2 KB):* oito classes de tamanho em potência de 2 (16 a
  2048 bytes, cabeçalho incluído). Cada classe tem uma lista de blocos
  livres; quando esvazia, o alocador pega uma página e a corta em blocos
  daquela classe. Liberar devolve o bloco à lista da classe, em tempo
  constante. As páginas das classes não voltam para o bitmap: a memória de
  estruturas pequenas do kernel tende a ser reutilizada pela mesma classe.
- *Grandes (mais de 2 KB):* páginas contíguas direto do bitmap, devolvidas ao
  bitmap no `kfree`.

Cada bloco tem um cabeçalho de 8 bytes com uma assinatura e a classe (ou o
número de páginas). O `kfree` confere a assinatura e troca por outra ao
liberar: liberar duas vezes ou liberar um ponteiro que não veio do `kmalloc`
é detectado e registrado, em vez de corromper as listas em silêncio.

Outras opções pensadas: lista única de blocos com divisão e junção de
vizinhos (gasta menos memória, mas cada `kfree` precisa percorrer a lista e
um erro corrompe tudo) e alocador por "buddy" (mais complexo do que um
kernel deste tamanho precisa).
