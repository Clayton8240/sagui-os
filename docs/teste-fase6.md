# Roteiro de teste: fase 6 (Pendrive de dados: USB e FAT32)

**Arquivos:** `entrega/fase6/saguios.vdi` (VirtualBox) e `entrega/fase6/saguios.img` (pendrive). As mesmas imagens estão em `entrega/final/`.
**Como gravar e configurar:** `docs/como-gravar.md`.

Teste primeiro na máquina virtual. Se der erro, mande o print antes de ir para o G470.

---

## Correção depois do primeiro teste no G470

O pendrive não aparecia por dois motivos (erros 17 e 18):

1. O controlador USB do G470 trabalha com **endereços de 64 bits** e exige estruturas maiores que as que o driver usava. O QEMU usa um controlador de 32 bits, por isso os testes daqui passavam. **Esta correção só pode ser confirmada no G470.**
2. Só a **primeira partição** do pendrive era examinada. Agora a FAT32 é achada em qualquer uma das quatro, então o pendrive com a partição do SaguiOS + a FAT32 de 3 GB que você criou funciona (testado no QEMU com esse mesmo arranjo).

Se ainda não aparecer, rode **`usb`** no Terminal e fotografe: a saída agora diz se o controlador é de 64 bits, se a BIOS o soltou e **em que passo** a conversa com o pendrive parou em cada porta.

## O que mudou desde a fase 5

- O SaguiOS tem um **driver USB 2.0 (EHCI) próprio** e lê e grava **pendrives em FAT32**.
- Ao **espetar** um pendrive com o sistema ligado: aparece a notificação `Pendrive conectado: <nome> (<tamanho>)` no canto e o **ícone do pendrive** na área de trabalho.
- **Duplo clique no ícone:** monta em `/usb` e abre o Arquivos lá. O Editor salva e abre arquivos em `/usb`.
- **Botão direito no ícone** (ou **Remover com segurança** na lateral do Arquivos): grava tudo e mostra `Pode remover o pendrive.`
- Pelo Terminal: `discos`, `montar`, `desmontar`, e todos os comandos (`ls`, `cat`, `cp`, `mv`, `rm`...) funcionam em `/usb`. O `rm` no pendrive pede confirmação.
- O Sagui Pega Banana grava o recorde em `/usb/sagui-recorde.txt` quando há pendrive montado.
- Correção: os corações das vidas aparecem na tela de início do jogo.

---

## Etapa 1: VirtualBox (`saguios.vdi`)

O SaguiOS só lê pendrives **USB** (um segundo disco virtual comum não serve). No VirtualBox, o pendrive de verdade do PC é repassado para a VM:

1. **Configurações > USB**: marque **Habilitar controlador USB** e escolha **USB 2.0 (EHCI)**. (No VirtualBox 6/7 o USB 2.0 precisa do **Extension Pack**; se ele não estiver instalado, pule esta etapa e teste direto no G470.)
2. Ligue a VM. Espete um pendrive **FAT32** no PC e, no menu da VM, **Dispositivos > USB**, marque o pendrive. Ele passa para a VM como se fosse espetado nela.
3. Siga a lista da etapa 2 abaixo (é a mesma).

---

## Etapa 2: Lenovo G470 (com um pendrive só)

Você vai usar o **mesmo pendrive** para dar boot e para os dados (o sistema roda todo da RAM):

1. Grave o `saguios.img` no pendrive (Rufus em modo DD, balenaEtcher ou `dd`) e dê boot no G470 (F12, **USB HDD**).
2. Na tela inicial, confira a linha **`[ ok ] USB: N controlador(es) EHCI`** (o G470 deve ter **2**).
3. Com a área de trabalho no ar, **tire o pendrive**. O sistema continua rodando.
4. No PC, **formate o pendrive em FAT32** (até 32 GB pelo Windows; acima disso, pelo Rufus: "Não inicializável", sistema de arquivos FAT32). Se quiser, copie algum arquivo de texto e uma pasta com nome acentuado para ele.
5. **Espete o pendrive de volta no G470**, com o SaguiOS ligado.

**O que você deve ver:**

| O que fazer | O que deve acontecer |
| --- | --- |
| Espetar o pendrive | Notificação `Pendrive conectado: <nome> (<tamanho>)` no canto de cima e o ícone do pendrive embaixo, na coluna de ícones. |
| Duplo clique no ícone do pendrive | Notificação `Pendrive montado em /usb` e o Arquivos abre mostrando o conteúdo do pendrive (nomes longos e acentos certos). |
| Abrir um `.txt` que veio do PC | Abre no Editor com os acentos certos. |
| Duplo clique em **Editor**, digitar uma nota, **Ctrl+S**, apagar o `/tmp/` e digitar `/usb/nota.txt` | Salva no pendrive. |
| No Arquivos, **Nova pasta** e **Apagar** dentro do pendrive | Cria e apaga (com "Tem certeza?"). |
| No Terminal: `discos` | `usb0  <fabricante> <produto>  <tamanho>  FAT32  "<nome>"  (montado em /usb)`. |
| No Terminal: `cp /leia-me.txt /usb/` e `ls /usb` | O arquivo aparece. |
| Jogar uma partida do **Banana** | O recorde fica salvo em `/usb/sagui-recorde.txt`. |
| **Botão direito** no ícone do pendrive (ou **Remover com segurança** no Arquivos) | `Pode remover o pendrive.` |
| Tirar o pendrive | `Pendrive removido.` e o ícone some. |
| No PC: abrir o pendrive | A `nota.txt`, o `leia-me.txt` e o `sagui-recorde.txt` estão lá. O Windows não deve pedir para "verificar e corrigir" o pendrive. |
| Espetar de novo, montar (duplo clique) e **arrancar sem desmontar** | Aviso `O pendrive foi removido sem desmontar: dados podem ter se perdido.` |

**Atenção especial no G470:**

- **O hub interno do chipset.** No G470, todo dispositivo USB fica atrás de um hub de alta velocidade que existe dentro do chipset. O QEMU não tem um hub equivalente, então **esta é a primeira vez que o caminho do hub roda de verdade** (D024). Se o pendrive não for reconhecido, rode `usb` no Terminal e **fotografe a saída**: ela mostra os controladores, as portas e os dispositivos (o hub deve aparecer como "classe hub").
- **A passagem do controlador da BIOS para o SaguiOS** também só acontece de verdade no G470. Se o teclado ou o touchpad pararem logo depois da linha `USB` na inicialização, me diga.
- **Portas:** teste mais de uma porta USB do notebook.

**Se der errado**, fotografe a tela e me diga:

- **Nenhuma notificação ao espetar:** rode `usb` e `discos` no Terminal e fotografe.
- **Aparece, mas não monta:** a mensagem diz o motivo (por exemplo, "formato não suportado" se estiver em exFAT ou NTFS).
- **Tela vermelha:** a foto mostra o erro e os registradores.

Quando testar, me diga **"fase 6 ok"** ou descreva o que aconteceu.
