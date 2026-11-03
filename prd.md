# PR<F2><F2><F2><F2>D: SaguiOS, um sistema operacional escrito do zero por IA

> Nome do sistema: **SaguiOS**.
> Hardware alvo: **Lenovo G470 (modelo 20078)** e máquina virtual (QEMU / VirtualBox).
> Ambiente de trabalho: VPS Hostinger KVM 8 com Ubuntu 26.04, acessada por SSH, sessão tmux.

---

## 0. Como usar este documento (leia primeiro, agente)

1. Este PRD é a única especificação do projeto. Siga a ordem das fases e das histórias.
2. Antes de cada história, releia a seção dela e os critérios de aceite. Só marque como feita quando todos os critérios passarem nos testes automáticos.
3. Registre tudo (seção 5). O Eli acompanha pelo `status.md` e pelo dashboard, e vai mostrar isso num vídeo.
4. Trabalhe em modo autônomo (seção 10) **dentro de cada fase**. Ao terminar uma fase, **pare** e avise o Eli (seção 8). Só comece a fase seguinte depois que ele testar e liberar.
5. Toda decisão que este PRD não cobre: escolha a opção mais simples que funcione no G470, anote em `docs/decisoes.md` com o motivo e siga em frente.

---

## 1. Objetivo

Escrever, do zero, um sistema operacional mínimo para PCs x86 que:

1. **Dá boot** num pendrive, no Lenovo G470 real, com carregador de boot próprio.
2. Tem um **kernel de verdade**: interrupções, gerenciamento de memória com paginação, processos com escalonador, chamadas de sistema e programas rodando em modo usuário (ring 3), carregados de arquivos ELF.
3. Mostra um **terminal** onde dá para digitar comandos e navegar em arquivos. Os comandos são programas separados, não funções do kernel.
4. Abre um **modo gráfico simples**, com mouse (touchpad), janelas e **programas gráficos em ring 3**: Arquivos, Terminal, Editor de texto, Calculadora e o jogo Sagui Pega Banana.
5. **Lê e grava arquivos num pendrive FAT32**, com driver USB próprio, para os arquivos sobreviverem ao desligar.

Não precisa ser bonito nem completo. A meta é provar que a IA consegue escrever as partes fundamentais de um sistema operacional.

**Meta de tamanho:** a imagem final deve caber em **1,44 MB** (o tamanho de um disquete, como referência). Não é obrigatório, mas é o objetivo.

### 1.1 O que torna isso um sistema operacional (critério do Tanenbaum)

O público vai cobrar que isto seja um sistema operacional de verdade, não uma distribuição. A referência é o livro *Modern Operating Systems*, de Andrew Tanenbaum, usado nos cursos de computação do mundo todo. Para ele, o sistema operacional tem dois papéis:

1. **Máquina estendida:** esconde os detalhes do hardware e oferece abstrações mais simples aos programas (processos, espaços de endereço, arquivos).
2. **Gerente de recursos:** divide a CPU, a memória e os dispositivos entre os programas, com proteção entre eles.

O sistema operacional, nesse sentido, é o software que roda em **modo kernel**. O shell e a interface gráfica são programas que *usam* o sistema operacional. Por isso, neste projeto, o kernel é o que importa, e o shell roda como programa comum em ring 3.

Ao final, todos os itens abaixo precisam estar implementados e demonstrados por teste e captura de tela. A organização segue os conceitos básicos do capítulo 1 do livro e os assuntos dos capítulos seguintes:

| Conceito do livro | O que o SaguiOS implementa | História |
| --- | --- | --- |
| Modo kernel e modo usuário | Kernel em ring 0, programas em ring 3 | H03, H12 |
| Processos | Criação, término, estados, PID, processo pai e filho | H11, H13 |
| Escalonamento | Round-robin preemptivo no relógio | H11 |
| Comunicação entre processos | Pipes entre programas (`ls \| conta`) | H15 |
| Espaços de endereço | Paginação com um espaço por processo | H10 |
| Gerenciamento de memória | Memória física, alocador do kernel, memória dos processos | H06, H10, H13 |
| Arquivos e diretórios | Abrir, ler, escrever, criar, apagar arquivos e pastas, na RAM e num pendrive FAT32 | H08, H14, H32, H33 |
| Entrada e saída | Interrupções e drivers próprios: teclado, mouse, vídeo, relógio, USB e armazenamento | H05, H07, H17, H19, H29 a H31 |
| Proteção | Um processo não acessa a memória do outro nem o kernel | H12, H16 |
| Chamadas de sistema | Processos, arquivos, diretórios e diversas (seção 4.5) | H12 |
| O shell | `sagsh`, um programa em ring 3, com redirecionamento e pipes | H16 |
| Carregamento de programas | Executáveis ELF lidos do sistema de arquivos | H13 |
| Interface gráfica (extra) | Gerenciador de janelas no kernel; programas gráficos em ring 3 | H20 a H28 |

Fora do escopo, por serem assuntos avançados do livro: threads, múltiplos processadores, memória virtual com troca para disco, virtualização e usuários com permissões.

---

## 2. Regras do desafio

Estas regras existem porque o vídeo vai ser cobrado pelo público. Siga à risca.

### Pode

- Usar **ferramentas** na VPS: compilador (GCC), assembler (NASM), linker, `make`, Python para scripts de build, QEMU para testar, ImageMagick para converter capturas de tela. Ferramentas não vão para dentro do sistema.
- Ler **especificações e manuais oficiais** para entender como o hardware e os formatos funcionam: manuais da Intel (Software Developer's Manual), especificação VESA BIOS Extensions, especificação do formato ELF, documentação do controlador PS/2 (i8042), do PIT, do PIC e do CMOS, especificação PCI, especificação USB 2.0, especificação EHCI da Intel, especificação USB Mass Storage (Bulk-Only Transport), comandos SCSI básicos e a especificação do sistema de arquivos FAT32 da Microsoft.
- Escrever scripts auxiliares que rodam na VPS (por exemplo, o empacotador do sistema de arquivos).

### Não pode

- **Acessar código-fonte de outros sistemas operacionais.** Não abrir, clonar, ler nem pesquisar repositórios de nenhum sistema operacional, kernel ou carregador de boot, em nenhum site (GitHub, GitLab, Codeberg, SourceForge etc.). Isso inclui projetos pessoais e de hobby, o Linux, o Minix, o KolibriOS e qualquer projeto citado nos comentários do vídeo. Na dúvida se um link é código de sistema operacional, não abra.
- **Copiar código** de qualquer lugar: tutoriais, wikis (incluindo a OSDev Wiki), respostas de fórum, blogs. Todo código dentro da imagem é escrito neste projeto. Documentação conceitual pode ser lida; trechos de código nunca são copiados.
- Usar **bibliotecas prontas** dentro do sistema (nem libc). Tudo que o kernel e os programas usarem é escrito aqui, incluindo a biblioteca dos programas de usuário (`libsagui`).
- Usar **GRUB, Limine, Syslinux** ou qualquer carregador de boot pronto. O carregador é nosso.
- Usar **fontes ou imagens prontas**. A fonte bitmap e qualquer desenho (inclusive o mascote sagui) são criados pela IA neste projeto, como dados no código.
- Usar nomes de componentes de outros projetos. Os nomes do SaguiOS são próprios e em português (ex.: "Arquivos", "Terminal", `sagsh`).
- Criar programas ou jogos que imitem os de projetos citados nos comentários do vídeo (por exemplo: cobrinha, arkanoid, space invaders). Os programas do SaguiOS são só os definidos neste PRD.

Se em algum momento parecer necessário quebrar uma regra, **não quebre**: registre em `docs/decisoes.md` e procure outra saída.

### 2.1 Princípio de autoria

Este é o ponto mais importante do PRD. O SaguiOS tem que ser **autoral**: só os **conceitos** são aprendidos de fora (o que é um processo, como funciona a paginação, o que um pipe faz). A **implementação** é inteiramente nossa.

- **Não reproduzir código de memória.** Você conhece código de projetos famosos (Linux, Minix, xv6, exemplos de livros e tutoriais). Não escreva nada a partir dessa lembrança. Para cada parte, pense na solução a partir do conceito e da especificação do hardware, e escreva a sua versão.
- **Livros didáticos valem só pelos conceitos.** O livro do Tanenbaum traz o código do Minix, e outros livros trazem o código do xv6. Esse código não pode ser usado nem como modelo.
- **Estrutura, nomes e comentários próprios.** Organização de pastas, nomes de funções, de estruturas e de variáveis, e comentários são criados neste projeto. Comentários em português, explicando o porquê de cada decisão.
- **O que é igual por obrigação, tudo bem.** Valores definidos pelo hardware ou por especificações (endereços de portas, números de interrupções, formato dos cabeçalhos ELF, bits das tabelas de páginas, a assinatura 0x55AA) são iguais em qualquer sistema. Isso não é cópia: é o hardware exigindo.
- **Registrar o raciocínio.** Para cada parte central (carregador de boot, paginação, escalonador, chamadas de sistema, sistema de arquivos, pipes, gerenciador de janelas, driver USB, FAT32), escrever em `docs/decisoes.md` como a solução foi pensada e por que foi escolhida. Isso é a prova de autoria para o público.
- **Na dúvida, faça diferente.** Se uma solução parecer muito igual a algo conhecido, mude a abordagem e registre.

---

## 3. Hardware alvo e ambiente de teste

### 3.1 Lenovo G470 (modelo 20078)

Notebook de 2011, plataforma Intel Sandy Bridge. Premissas (a confirmar no teste real):

| Peça | Premissa | Como o sistema usa |
| --- | --- | --- |
| Firmware | BIOS Insyde com boot tradicional (legacy) | Carregador de boot próprio, em modo real |
| Boot pelo pendrive | A BIOS emula o pendrive como disco rígido | Leitura com `INT 13h` estendido (AH=42h) |
| Teclado interno | Controlador PS/2 (i8042), teclado **ABNT2** | Driver PS/2 próprio, IRQ1, layout ABNT2 |
| Touchpad | Modo mouse PS/2 padrão | Driver de mouse PS/2 próprio, IRQ12 |
| Vídeo | Intel HD Graphics com VESA VBE na BIOS | Framebuffer linear via VBE |
| Memória | 2 GB ou mais | Mapa de memória via `INT 15h, E820` |
| Disco interno | SATA, modo IDE ou AHCI na BIOS | Só no bônus, e **somente leitura** |
| USB | Apenas USB 2.0 (controladores EHCI do chipset Intel série 6, com hub interno em cada controlador) | Fase 6: driver EHCI próprio, só para um **pendrive de dados** |

### 3.2 Estratégia central: tudo na RAM, mais um pendrive de dados

O carregador de boot usa a BIOS (`INT 13h`) para copiar o kernel e o sistema de arquivos (ramdisk) do pendrive de boot para a memória **antes** de entrar no modo protegido. Depois disso, o sistema inteiro roda da RAM e nunca mais lê o pendrive de boot. Isso permite que as fases 1 a 5 funcionem sem nenhum driver USB.

Na fase 6, o kernel ganha um driver USB próprio para um **segundo pendrive, de dados**, formatado em FAT32 (por exemplo, no Windows). Nele o usuário salva e abre arquivos que sobrevivem ao desligar.

**Segurança dos dados:** o sistema nunca grava no disco interno do notebook. Só grava em pendrives USB que o usuário montou de forma explícita com `montar`. Antes de apagar qualquer coisa no pendrive, os programas pedem confirmação.

### 3.3 Ambiente de teste na VPS

- QEMU sem KVM (a VPS não tem virtualização aninhada). É lento, mas suficiente para um sistema pequeno.
- Rodar sempre sem janela (`-display none`), com saída serial para arquivo e o monitor do QEMU para enviar teclas, mexer o mouse e tirar capturas de tela (`screendump`).
- Testar **dois caminhos de boot**:
  1. Imagem como disco IDE: `-drive format=raw,file=build/saguios.img`
  2. Imagem como **pendrive USB** (simula o G470): controlador EHCI + `usb-storage`, boot pela BIOS do QEMU.
- A partir da fase 6, um **segundo dispositivo USB** de dados: uma imagem FAT32 criada na VPS com `mkfs.vfat` (ferramenta), ligada ao mesmo controlador EHCI. Depois do teste, conferir o conteúdo gravado com `mtools` na VPS.

---

## 4. Arquitetura técnica (decisões fixas)

| Item | Decisão | Motivo |
| --- | --- | --- |
| Arquitetura | x86 32 bits, modo protegido | Mais simples que 64 bits, roda no G470 e em qualquer VM |
| Linguagens | Assembly (NASM) no boot e onde for necessário; C (freestanding) no resto | Padrão para kernels pequenos |
| Compilador | Preferência: compilador cruzado `i686-elf` (binutils + GCC compilados do código-fonte na VPS). Alternativa: GCC do sistema com `-m32 -ffreestanding -nostdlib -fno-pic -fno-stack-protector` | O compilador é ferramenta, não vai para a imagem |
| Formato da imagem | Imagem de disco crua (`.img`) com MBR e tabela de partição, mais uma cópia convertida para `.vdi` | O `.img` é o que a BIOS do G470 aceita do pendrive; o `.vdi` é para o VirtualBox |
| Gravação no pendrive | Rufus (com `.img` ele grava byte a byte, no modo DD), balenaEtcher ou `dd` | No projeto anterior o Rufus falhou com uma ISO; com `.img` cru ele não altera a imagem. Se der problema, usar o balenaEtcher |
| Modo de execução | Kernel em ring 0; programas em ring 3 a partir da fase 3 | Separação real entre kernel e programas |
| Paginação | Obrigatória a partir da fase 3: paginação de 2 níveis (4 KB), um diretório de páginas por processo, kernel mapeado em todos | Isolamento entre processos |
| Multitarefa | Preemptiva, round-robin, no tique do PIT | Escalonador simples e demonstrável |
| Chamadas de sistema | `int 0x80`, número no EAX, argumentos em EBX, ECX, EDX (seção 4.5) | Simples de depurar |
| Programas | ELF32 estático, ligado no endereço virtual 0x40000000 | Formato padrão, carregador escrito por nós |
| Interface gráfica | Gerenciador de janelas no kernel; todos os programas gráficos em ring 3, usando chamadas de sistema de janela | O kernel só desenha as molduras e entrega os eventos; os programas são programas de verdade |
| Números na calculadora e no jogo | Inteiros e ponto fixo, sem usar a unidade de ponto flutuante do processador | Evita ter que salvar o estado da FPU a cada troca de processo |
| USB | Driver EHCI próprio (fase 6), com suporte a hub, para armazenamento em massa | Só para o pendrive de dados; nada de teclado ou mouse USB |

### 4.1 Layout da imagem de disco

| Região | Conteúdo |
| --- | --- |
| LBA 0 | **MBR**: código do estágio 1 + tabela de partição com 1 partição ativa cobrindo a imagem. Reservar a área de um BPB falso no início (algumas BIOSes escrevem nessa área ao dar boot por USB). |
| LBA 1 | **Cabeçalho de boot** do SaguiOS: assinatura, LBA e tamanho do estágio 2, do kernel e do ramdisk |
| A seguir | **Estágio 2** do carregador (assembly, modo real) |
| A seguir | **Kernel** (binário plano) |
| A seguir | **Ramdisk** (sistema de arquivos SGFS, seção 4.3) |

Todos os offsets são gerados pelo script de build. Nada fixo no código além do LBA 1.

### 4.2 Sequência de boot

1. **Estágio 1 (MBR, 512 bytes):** guarda o número do drive recebido em `DL`, monta pilha e segmentos, lê o cabeçalho (LBA 1) e o estágio 2 com `INT 13h AH=42h`, pula para o estágio 2. Mostra mensagem curta de erro se a leitura falhar.
2. **Estágio 2 (modo real):**
   1. Habilita a linha A20 (tentar o método rápido pela porta 0x92 e o pelo controlador de teclado, e verificar se funcionou).
   2. Obtém o mapa de memória (`INT 15h, E820`).
   3. Lê o kernel e o ramdisk com `INT 13h AH=42h`, em blocos, mostrando progresso na tela.
   4. Consulta os modos VESA (`INT 10h AX=4F00/4F01`) e escolhe, nesta ordem: 1024x768, 800x600, 640x480, com 32 ou 24 bits por pixel e framebuffer linear. Guarda as informações do modo. **Até a fase 3 não troca de modo**: fica em modo texto.
   5. Monta a GDT, entra em modo protegido e pula para o kernel, passando uma estrutura `boot_info` (mapa de memória, drive, endereço e tamanho do ramdisk, dados do modo VESA).
3. **Carregamento acima de 1 MB:** o `INT 13h` só escreve abaixo de 1 MB. Estratégia recomendada: carregar em blocos num buffer baixo e copiar para cima de 1 MB usando *unreal mode*. Alternativa aceita: se kernel + ramdisk couberem com folga abaixo de 0x9F000, carregar direto em 0x10000. Registrar a escolha.

### 4.3 Sistema de arquivos SGFS (formato próprio)

Formato simples, criado neste projeto. A imagem no pendrive é somente leitura; por cima dela, o kernel mantém uma **camada gravável na RAM** (história H14), onde é possível criar, alterar e apagar arquivos e pastas durante a sessão. Ao desligar, as alterações se perdem, como num sistema live.

Uma **camada de arquivos virtual** no kernel une tudo num caminho só: `/` é o SGFS com a camada da RAM, e `/usb` é o pendrive de dados quando montado (fase 6). Os programas não precisam saber onde o arquivo está.

- Cabeçalho: assinatura `SGFS`, versão, número de entradas.
- Tabela de entradas: caminho completo (ex.: `docs/sobre.txt`, até 56 bytes), tipo (arquivo, pasta ou programa), tamanho, deslocamento dos dados.
- Área de dados.

Um script Python na VPS (`tools/mksgfs.py`) monta o SGFS a partir da pasta `rootfs/`. Conteúdo do `rootfs/`, escrito pela IA:

- `leia-me.txt`: boas-vindas ao SaguiOS e como usar os comandos.
- `sobre.txt`: como o sistema foi feito, por quem, e as regras do desafio.
- `historia/linux.txt`: um resumo da história do Minix, do Linux e do GNU.
- `historia/saguios.txt`: o diário resumido deste projeto, gerado no fim.
- `arte/sagui.txt`: o mascote em arte ASCII.
- `bin/`: os programas ELF da fase 3, gerados a partir da pasta `userland/` no build.
- `tmp/`: pasta vazia, para testes de gravação.

### 4.4 Texto, acentos e teclado

- Sistema todo em **português do Brasil**.
- A codificação interna do texto é **Latin-1 (ISO-8859-1)**, para ter acentos sem implementar UTF-8. Os arquivos do `rootfs/` são convertidos para Latin-1 no build.
- Nas fases 1 a 3 (modo texto VGA) os acentos podem aparecer errados por causa da fonte da placa de vídeo. Isso é aceitável. Na fase 4, a fonte própria já tem os acentos.
- **Layout ABNT2** com scan code set 1 (o padrão traduzido pelo controlador). Mapeamento das teclas que mudam em relação ao americano:

| Scan code | Normal | Com Shift |
| --- | --- | --- |
| 0x29 | ' | " |
| 0x07 (tecla 6) | 6 | ¨ (tecla morta) |
| 0x1A | ´ (tecla morta) | ` (tecla morta) |
| 0x1B | [ | { |
| 0x27 | ç | Ç |
| 0x28 | ~ (tecla morta) | ^ (tecla morta) |
| 0x2B | ] | } |
| 0x56 | \ | \| |
| 0x33 | , | < |
| 0x34 | . | > |
| 0x35 | ; | : |
| 0x73 | / | ? |

- Teclas mortas combinam com vogais: á é í ó ú, à, â ê ô, ã õ, ü. Combinação inválida mostra o acento seguido da letra.
- Se algum mapeamento estiver errado no G470, o Eli informa no teste real e a correção vira uma entrada em `docs/erros.md`.

### 4.5 Chamadas de sistema (mínimo)

Organizadas nos mesmos grupos que o livro do Tanenbaum usa:

| Grupo | Chamadas | Função |
| --- | --- | --- |
| Processos | `executar`, `esperar`, `sair`, `pid`, `matar` | Criar processo a partir de um ELF, esperar o filho, encerrar, consultar PID, encerrar outro processo |
| Arquivos | `abrir`, `ler`, `escrever`, `fechar`, `posicionar`, `info_arquivo` | Ler e gravar arquivos; o terminal também é um "arquivo" (entrada 0, saída 1) |
| Diretórios | `criar_pasta`, `remover_pasta`, `apagar`, `listar`, `mudar_pasta` | Manipular pastas e entradas |
| Comunicação | `pipe`, `duplicar` | Ligar a saída de um processo à entrada de outro |
| Diversas | `dormir`, `tempo`, `data`, `info_sistema` | Pausa, relógio, memória e lista de processos |
| Janelas (fase 5) | `criar_janela`, `fechar_janela`, `atualizar_janela`, `proximo_evento` | O programa desenha na própria área de memória e pede ao kernel para mostrar; recebe cliques, movimentos e teclas da sua janela |
| Dispositivos (fase 6) | `listar_discos`, `montar`, `desmontar` | Ver pendrives e partições, montar em `/usb`, gravar tudo e liberar para remover |

Convenção: `int 0x80`, número da chamada no EAX, argumentos em EBX, ECX, EDX, retorno no EAX (negativo indica erro). Os nomes no código podem ser em inglês; os mostrados ao usuário são em português. Ponteiros vindos do ring 3 são sempre validados pelo kernel antes do uso.

---

## 5. Estrutura do repositório e documentação

```
saguios/
├── prd.md                 # este documento
├── status.md              # resumo vivo: fase, história atual, progresso, bloqueios
├── Makefile
├── boot/                  # estágio 1 e estágio 2 (NASM)
├── kernel/                # código do kernel (C + NASM)
├── userland/              # libsagui e os programas de usuário (sagsh, ls, cat, editor, calculadora, jogo...)
├── tools/                 # mksgfs.py, scripts de teste, build da imagem
├── rootfs/                # arquivos que vão para o ramdisk
├── build/                 # saída do build (saguios.img)
├── tests/                 # scripts de teste no QEMU por história
├── docs/
│   ├── historias/         # uma página por história: objetivo, critérios, resultado
│   ├── decisoes.md        # toda decisão tomada e o motivo
│   ├── diario.md          # log cronológico do que foi feito
│   ├── erros.md           # cada erro: sintoma, causa, correção
│   ├── capturas/          # PNGs das telas do QEMU, nomeados por história
│   ├── como-gravar.md     # como gravar o pendrive e testar no G470
│   └── teste-faseX.md     # roteiro de teste de cada fase para o Eli
├── dashboard/
│   ├── index.html         # painel visual (sem dependências externas)
│   └── status.json        # dados do painel, atualizados a cada história
└── entrega/               # imagens finais por fase + SHA-256
```

- Inicializar um repositório **git** e fazer commit ao fim de cada história, com mensagem `H0X: descrição`.
- `status.md` e `dashboard/status.json` são atualizados **ao fim de cada história** e sempre que houver bloqueio.
- As capturas de tela do QEMU são importantes: o Eli vai usá-las no vídeo.

---

## 6. Fases e histórias

Progresso do dashboard: cada história vale o mesmo peso. As histórias bônus não contam para os 100%.

São 6 fases. **Cada fase termina numa parada obrigatória**: o agente entrega a imagem, avisa o Eli e espera ele testar no G470 antes de começar a próxima (seção 8). Os bugs que aparecerem nesses testes fazem parte do projeto e do vídeo: não esconda nem contorne, registre e corrija.

| Fase | Objetivo | Histórias |
| --- | --- | --- |
| 1. Boot | Dar boot no G470 com carregador próprio e mostrar a mensagem do kernel | H01 a H04 |
| 2. Kernel e terminal | Interrupções, relógio, memória, teclado ABNT2, arquivos e um terminal provisório | H05 a H09 |
| 3. Processos e programas | Paginação, processos, ring 3, chamadas de sistema, ELF, arquivos graváveis, pipes e o shell | H10 a H16 |
| 4. Gráfico e mouse | Modo gráfico, fonte própria e touchpad | H17 a H19 |
| 5. Janelas e programas | Área de trabalho, janelas e os programas: Arquivos, Terminal, Editor, Calculadora e Sagui Pega Banana | H20 a H28 |
| 6. Pendrive de dados | Driver USB e FAT32 para salvar e abrir arquivos num pendrive | H29 a H34 |

### Fase 1: Boot (dar sinal de vida)

**Objetivo:** dar boot no G470 com carregador de boot próprio e mostrar a primeira mensagem do kernel.

**H01. Preparar o ambiente.** Instalar NASM, QEMU, ImageMagick, Python. Montar o compilador cruzado ou validar o GCC do sistema com as flags freestanding. Criar a estrutura de pastas, o git, o `status.md` e o dashboard vazio.
*Aceite:* `make` roda sem erro com um alvo vazio; dashboard abre no navegador.

**H02. Estágio 1 (MBR).** Código de 512 bytes com assinatura 0x55AA, tabela de partição válida com partição ativa e área de BPB reservada.
*Aceite:* no QEMU (IDE e USB), aparece a mensagem `SaguiOS: carregando...`.

**H03. Estágio 2 e entrada no modo protegido.** A20, mapa E820, leitura do kernel e do ramdisk, GDT, modo protegido, salto para o kernel com `boot_info`.
*Aceite:* o kernel em C escreve na tela em modo texto VGA: `Ola! Eu sou o SaguiOS.` e a quantidade de memória detectada.

**H04. Imagem para o pendrive e para o VirtualBox.** Script de build gera `build/saguios.img` com todas as regiões e o SHA-256, e converte a mesma imagem para `build/saguios.vdi` com `qemu-img convert -f raw -O vdi` (o VirtualBox não abre `.img` direto). Escrever `docs/como-gravar.md` com: gravação no pendrive (balenaEtcher no Windows, `dd` no Linux), boot no G470 (desativar Secure Boot se existir, escolher boot legacy pelo pendrive, F12 no menu de boot) e teste no VirtualBox (nova VM do tipo "Other", sem EFI, usando o `.vdi` como disco).
*Aceite:* boot funciona nos dois modos do QEMU; o `.vdi` é gerado sem erro; arquivos de entrega em `entrega/fase1/`. O `como-gravar.md` explica a gravação com Rufus (escolher a imagem `.img`; se ele perguntar o modo, escolher DD) e com balenaEtcher como alternativa.

> **PARADA OBRIGATÓRIA: fim da fase 1.** Ver seção 8.

### Fase 2: Kernel e terminal

**Objetivo:** o kernel reage ao hardware (interrupções e relógio), organiza a memória, lê o teclado ABNT2, encontra arquivos e oferece um terminal provisório para testar tudo.

**H05. Interrupções e relógio.** IDT, tratamento das exceções da CPU (mostrando nome da exceção e registradores na tela), remapeamento do PIC, timer (PIT) a 100 Hz, contador de tempo desde o boot.
*Aceite:* teste que provoca divisão por zero mostra a tela de erro do SaguiOS; o contador de segundos avança.

**H06. Memória física.** Gerenciador de memória física a partir do mapa E820 (bitmap de páginas de 4 KB) e um alocador do kernel (`kmalloc`/`kfree`).
*Aceite:* teste aloca e libera blocos de vários tamanhos sem corromper; comando de diagnóstico mostra memória total e livre.

**H07. Teclado PS/2 com ABNT2.** Driver no IRQ1, buffer de teclas, Shift, Caps Lock, Ctrl, teclas mortas e o mapa da seção 4.4.
*Aceite:* no QEMU, teclas enviadas pelo monitor aparecem corretas na tela, incluindo `ç`.

**H08. Sistema de arquivos SGFS.** `tools/mksgfs.py` + leitor no kernel: listar pasta, abrir arquivo, ler conteúdo.
*Aceite:* teste interno lista as entradas do `rootfs/` e lê o `leia-me.txt` inteiro.

**H09. Terminal provisório do kernel.** Prompt `sagui:/$` rodando dentro do kernel, só para validar teclado e arquivos antes dos processos: `ajuda`, `ls`, `cd`, `cat`, `limpar`, `mem`, `data` (relógio CMOS), `reiniciar` (reset pelo controlador de teclado).
*Aceite:* roteiro de teste digita cada comando no QEMU e confere a saída pela captura de tela e pela serial.
<F2>
> **PARADA OBRIGATÓRIA: fim da fase 2.** Ver seção 8.

### Fase 3: Processos e programas em modo usuário

**Objetivo:** rodar vários programas ao mesmo tempo, cada um isolado no seu espaço de memória e em ring 3, pedindo serviços ao kernel por chamadas de sistema. Esta é a fase que cobre o núcleo da definição do Tanenbaum (seção 1.1). Não pular nem simplificar.

**H10. Paginação.** Diretório e tabelas de páginas, kernel mapeado em todos os espaços de endereço, tratamento de falta de página (page fault) com mensagem clara, criação e destruição de espaços de endereço.
*Aceite:* teste cria dois espaços de endereço, escreve no mesmo endereço virtual em cada um e confirma valores diferentes; acesso inválido gera a tela de page fault.

**H11. Processos e escalonador.** Estrutura de processo (PID, estado, registradores, espaço de endereço, pilha do kernel), troca de contexto em assembly, escalonador round-robin preemptivo no tique do PIT, processo ocioso.
*Aceite:* duas tarefas de teste imprimem `A` e `B` alternados na serial sem cooperar entre si.

**H12. Ring 3 e chamadas de sistema.** TSS com a pilha do kernel, segmentos de usuário na GDT, entrada em ring 3, `int 0x80` com as chamadas da seção 4.5, validação de ponteiros.
*Aceite:* um programa de teste em ring 3 escreve `Ola do ring 3!` pela chamada `escrever` e sai com `sair`; uma instrução privilegiada (`cli`) no ring 3 encerra só aquele processo.

**H13. Carregador ELF e libsagui.** Leitor de ELF32 estático (cabeçalhos de programa, mapeamento das seções no espaço do processo, pilha do usuário com argumentos). Biblioteca `libsagui` escrita neste projeto: chamadas de sistema, funções de texto e memória, um `printf` simples.
*Aceite:* o kernel carrega `bin/ola` do SGFS e o programa imprime os argumentos recebidos.

**H14. Arquivos e pastas graváveis.** Camada gravável na RAM por cima do SGFS: criar, escrever, sobrescrever, apagar arquivos; criar e remover pastas; tabela de arquivos abertos por processo; o terminal como entrada 0 e saída 1 de cada processo.
*Aceite:* teste em ring 3 cria `tmp/teste.txt`, escreve, fecha, reabre, lê o mesmo conteúdo, apaga e confirma que sumiu; o mesmo com uma pasta.

**H15. Pipes (comunicação entre processos).** Chamadas `pipe` e `duplicar`, buffer no kernel, bloqueio de quem lê quando o pipe está vazio e de quem escreve quando está cheio, fim de arquivo quando o escritor fecha.
*Aceite:* teste liga dois processos por um pipe e transfere um texto maior que o buffer sem perder nem duplicar dados.

**H16. Shell e comandos como programas.** O kernel inicia `bin/sagsh` como primeiro processo. O terminal provisório da H09 é removido. O shell suporta redirecionamento (`>`, `>>`, `<`), pipe (`|`), histórico com seta para cima e completar nomes com Tab.

Os comandos clássicos usam os nomes que qualquer usuário de Linux conhece, e cada um tem também um apelido em português. Os comandos próprios do SaguiOS têm nome em português.

| Comando | Apelido | Tipo | Função |
| --- | --- | --- | --- |
| `ls [pasta]` | `listar` | programa | Lista arquivos e pastas |
| `cd <pasta>` | `entrar` | interno | Entra numa pasta |
| `pwd` | `onde` | interno | Mostra a pasta atual |
| `cat <arquivo>` | `ver` | programa | Mostra o conteúdo de um arquivo |
| `echo <texto>` | `eco` | programa | Escreve um texto (com `>` grava em arquivo) |
| `cp <origem> <destino>` | `copiar` | programa | Copia arquivo (também entre a RAM e o pendrive) |
| `mv <origem> <destino>` | `mover` | programa | Move ou renomeia |
| `rm <arquivo>` | `apagar` | programa | Apaga arquivo (pede confirmação no pendrive) |
| `mkdir <pasta>` | `criar-pasta` | programa | Cria pasta |
| `rmdir <pasta>` | `remover-pasta` | programa | Remove pasta vazia |
| `wc` | `conta` | programa | Conta linhas, palavras e letras |
| `ps` | `processos` | programa | Lista os processos em execução |
| `kill <pid>` | `matar` | programa | Encerra um processo |
| `clear` | `limpar` | interno | Limpa a tela |
| `ajuda [comando]` | | interno | Lista todos os comandos com uma linha de explicação em português, ou explica um comando |
| `mem` | | programa | Memória total, usada e livre |
| `data` | | programa | Data e hora |
| `sobre` | | programa | Versão e créditos do SaguiOS |
| `ola` | | programa | Imprime os argumentos recebidos (teste) |
| `contador` | | programa | Conta segundos, para mostrar multitarefa |
| `falha` | | programa | Provoca um erro de propósito, para mostrar a proteção |
| `reiniciar` | | interno | Reinicia o computador |
| `sair` | | interno | Fecha o shell |

Na fase 5 entram os comandos que abrem os programas gráficos (`arquivos`, `editor [arquivo]`, `calculadora`, `banana`). Na fase 6, `discos`, `montar` e `desmontar`.

*Aceite:* roteiro executa todos os comandos e apelidos, incluindo `echo ola > tmp/a.txt`, `cat tmp/a.txt`, `cp tmp/a.txt tmp/b.txt`, `ls bin | conta` e `contador` rodando junto com outro comando; `falha` derruba só o próprio processo e o shell continua; `ps` mostra os processos; `ajuda` lista tudo. Captura de tela de cada caso em `docs/capturas/`.

> **PARADA OBRIGATÓRIA: fim da fase 3.** Ver seção 8.

### Fase 4: Modo gráfico e mouse

**Objetivo:** trocar o modo texto por uma tela gráfica desenhada pixel a pixel, com fonte própria e o touchpad funcionando.

**H17. Framebuffer VESA.** O estágio 2 troca para o modo VESA escolhido. O kernel desenha num buffer na memória e copia para a tela (double buffer). Primitivas: pixel, retângulo, linha, retângulo com borda.
*Aceite:* captura mostra fundo colorido e formas desenhadas na resolução detectada. Se o VESA falhar, o sistema volta para o modo texto e avisa.

**H18. Fonte própria.** Fonte bitmap 8x16 desenhada neste projeto, com ASCII completo e os caracteres acentuados do português em Latin-1. O terminal passa a rodar no modo gráfico.
*Aceite:* captura mostra o terminal gráfico com o texto `Ação, vovô, pão, ÇÃO, à é í ó ú` correto.

**H19. Mouse / touchpad PS/2.** Habilitar a porta auxiliar do i8042, IRQ12, pacotes de 3 bytes, cursor desenhado por nós, botões esquerdo e direito.
*Aceite:* movimentos enviados pelo monitor do QEMU movem o cursor; clique é detectado e registrado na serial.

> **PARADA OBRIGATÓRIA: fim da fase 4.** Ver seção 8.

### Fase 5: Janelas e programas

**Objetivo:** área de trabalho com janelas e os programas gráficos em ring 3: Arquivos, Terminal, Editor de texto, Calculadora e Sagui Pega Banana.

**H20. Área de trabalho.** Fundo com degradê em cores do Brasil, o mascote sagui em pixel art desenhado pela IA no código, barra superior com nome do sistema e relógio (CMOS).
*Aceite:* captura da área de trabalho em `docs/capturas/`.

**H21. Gerenciador de janelas e chamadas de sistema gráficas.** No kernel: lista de janelas, moldura com barra de título e botão fechar, arrastar com o mouse, janela ativa por cima, foco do teclado. Chamadas da seção 4.5 (grupo Janelas): cada programa desenha numa área de memória própria e pede ao kernel para mostrar; o kernel entrega a cada programa só os eventos da sua janela.
*Aceite:* dois programas de teste em ring 3 abrem uma janela cada; o teste arrasta uma, fecha a outra, e o programa fechado termina sozinho.

**H22. Kit gráfico da libsagui.** Componentes desenhados pelo próprio programa, em ring 3: texto com a fonte própria, botão, rótulo, caixa de texto de uma linha, área de texto com várias linhas e rolagem, lista com seleção, caixa de confirmação ("Tem certeza?").
*Aceite:* programa de demonstração mostra todos os componentes funcionando com mouse e teclado; captura em `docs/capturas/`.

**H23. Arquivos.** Programa em ring 3 que lista pastas com ícones de pasta, arquivo e programa. Duplo clique: pasta entra, `.txt` abre no Editor, programa executa. Botões para voltar, criar pasta e apagar (com confirmação).
*Aceite:* teste navega até `historia/`, abre `linux.txt` no Editor, cria uma pasta em `tmp/` e a apaga.

**H24. Terminal em janela.** Programa em ring 3 que abre uma janela e roda o `sagsh` dentro dela, ligado por pipes. Do terminal, os comandos `arquivos`, `editor [arquivo]`, `calculadora` e `banana` abrem os programas gráficos.
*Aceite:* captura com o terminal em janela rodando `ps` e `ls bin | conta`.

**H25. Editor de texto.** Programa em ring 3: novo, abrir, salvar, salvar como; digitação com acentos (ABNT2), setas, Home, End, Backspace, Delete, Enter, rolagem; Ctrl+S salva; título da janela mostra o nome do arquivo e um asterisco quando há alteração não salva; aviso ao fechar sem salvar.
*Aceite:* teste cria `tmp/nota.txt` com o texto `Olá, SaguiOS! Ação e coração.`, salva, fecha, reabre pelo Arquivos e confere o conteúdo.

**H26. Calculadora.** Programa em ring 3 com botões clicáveis e teclado: dígitos, vírgula decimal, soma, subtração, multiplicação, divisão, igual, limpar e apagar último dígito. Números em ponto fixo (seção 4), com aviso de divisão por zero e de número grande demais.
*Aceite:* teste calcula `12,5 × 4 = 50`, `7 ÷ 2 = 3,5`, `1 ÷ 0` mostra aviso; captura em `docs/capturas/`.

**H27. Sagui Pega Banana.** Jogo autoral em ring 3: o sagui anda na parte de baixo da janela com as setas, bananas caem do alto, cada banana pega vale um ponto, banana que cai no chão tira uma vida (3 vidas), a velocidade aumenta com a pontuação. Placar, tela de início, tela de fim de jogo e recorde da sessão. Todos os desenhos em pixel art feitos pela IA no código. Se houver pendrive montado (fase 6), o recorde é salvo em `/usb/sagui-recorde.txt`.
*Aceite:* teste automático joga alguns segundos enviando setas pelo monitor do QEMU; capturas da tela de jogo e da tela de fim de jogo.

**H28. Acabamento.** Ícones na área de trabalho para Arquivos, Terminal, Editor, Calculadora e Sagui Pega Banana. Tela "Sobre o SaguiOS" com versão, tamanho da imagem, número de processos ativos e créditos.
*Aceite:* captura com três programas abertos ao mesmo tempo; imagem medida e registrada (meta: até 1,44 MB).

> **PARADA OBRIGATÓRIA: fim da fase 5.** Ver seção 8.

### Fase 6: Pendrive de dados (USB e FAT32)

**Objetivo:** salvar, abrir e editar arquivos num pendrive FAT32, com driver USB próprio, e o pendrive aparecendo sozinho quando é conectado. Esta é a parte de driver mais difícil do projeto. Ir com calma, testar cada camada antes da próxima e registrar tudo em `docs/erros.md`.

**H29. PCI e controlador EHCI.** Varrer o barramento PCI e encontrar os controladores USB 2.0 (EHCI). Assumir o controlador que a BIOS estava usando (procedimento de passagem de controle da especificação EHCI), reiniciar o controlador e ligar as portas. Lembrar que o sistema já está todo na RAM, então a BIOS não precisa mais do pendrive de boot.
*Aceite:* comando de diagnóstico lista os controladores EHCI encontrados e as portas com dispositivo conectado, no QEMU.

**H30. Enumeração USB, hubs e conexão a quente.** Transferências de controle, leitura dos descritores, endereçamento dos dispositivos. Suporte a hub: no chipset do G470, cada controlador EHCI tem um hub interno, e todo dispositivo fica atrás dele. Detectar quando um pendrive é conectado ou removido com o sistema já rodando, consultando as mudanças de estado das portas e dos hubs.
*Aceite:* no QEMU, com um hub USB virtual entre o controlador e o pendrive, o sistema lista fabricante, produto e classe do dispositivo; conectar e remover o pendrive pelo monitor do QEMU gera os eventos de conexão e remoção, registrados na serial.

**H31. Armazenamento em massa.** Protocolo Bulk-Only Transport com os comandos SCSI básicos: identificar, capacidade, ler e gravar blocos. Tratamento de erros e de remoção do dispositivo.
*Aceite:* teste lê e grava setores numa imagem de teste no QEMU e confere byte a byte com a VPS.

**H32. Partições e FAT32: leitura.** Tabela de partições MBR, leitura do FAT32 (nomes curtos e nomes longos), montagem em `/usb` pela camada de arquivos virtual.
*Aceite:* pendrive de teste criado na VPS com pastas, acentos e nomes longos é listado e lido corretamente pelo `ls` e pelo `cat`.

**H33. FAT32: gravação.** Criar, sobrescrever, aumentar, apagar arquivos; criar e remover pastas; atualizar as duas cópias da tabela FAT e as informações de espaço livre; gravar tudo pendente no `desmontar`.
*Aceite:* teste grava arquivos e pastas no QEMU; depois do `desmontar`, a VPS confere com `mtools` e com `fsck.vfat` que o sistema de arquivos está íntegro e com o conteúdo certo.

**H34. Integração: o pendrive aparece no sistema.**

Quando o usuário espeta o pendrive com o sistema rodando:

1. Aparece uma notificação no canto da tela: `Pendrive conectado: <nome> (<tamanho>)`.
2. Surge um ícone "Pendrive" na área de trabalho e na lateral do Arquivos.
3. Clicar no ícone monta o pendrive em `/usb` e abre o Arquivos nele. O Editor passa a abrir e salvar arquivos lá.
4. Clique com o botão direito no ícone (ou o botão "Remover com segurança" no Arquivos) grava tudo o que estiver pendente, desmonta e mostra `Pode remover o pendrive`.
5. Se o pendrive for removido sem desmontar, o sistema avisa que dados podem ter se perdido e retira o ícone.

Pelo terminal, o mesmo fluxo:

```
sagui:/$ discos
usb0  <fabricante> <produto>  16 GB  FAT32  "MEUPEN"  (não montado)
sagui:/$ montar usb0
Pendrive montado em /usb
sagui:/$ ls /usb
sagui:/$ cp /leia-me.txt /usb/
sagui:/$ desmontar usb0
Pode remover o pendrive.
```

Pendrive que não seja FAT32 aparece em `discos` com o formato detectado e a mensagem `formato não suportado`, sem montar.

*Aceite:* roteiro completo no QEMU, pelos dois caminhos (interface e terminal): conectar o pendrive com o sistema rodando, montar, criar uma nota no Editor, salvar em `/usb`, copiar um arquivo pelo terminal, desmontar, remover; a VPS confere o conteúdo com `mtools`. Capturas de cada etapa.

> **PARADA OBRIGATÓRIA: fim da fase 6 (final).** Ver seção 8.

### Bônus (só depois dos 100%, se houver tempo e limite)

**B1. Enxergar o HD interno, sem abrir nem gravar.** Driver de disco SATA só para leitura, para o comando `discos` mostrar que o HD existe, o tamanho e as partições com o tipo de cada uma (por exemplo, `ext3 (Linux): formato não suportado`). O sistema não monta, não abre e **nunca grava** no disco interno. Testar primeiro na VM com um disco virtual. No G470 o HD usa ext3 e o Eli não quer que ele seja alterado de forma alguma.

**B2. Boot por UEFI.** Segundo carregador para UEFI (vídeo via GOP), para rodar em PCs mais novos.

**B3. Bip.** Som simples pelo alto-falante do PC (canal 2 do PIT): um bip ao pegar uma banana no Sagui Pega Banana e outro ao perder uma vida, mais um comando `bip`. No QEMU, testar com o alto-falante emulado. No G470 pode não tocar, dependendo de como o notebook liga esse sinal ao alto-falante; registrar o resultado.

---

## 7. Testes automáticos no QEMU

- Um script por história em `tests/`, executado por `make test`.
- O kernel escreve um log na **porta serial COM1**. Os testes leem esse log para conferir resultados.
- O script usa o **monitor do QEMU** para: enviar teclas (`sendkey`), mover o mouse (`mouse_move`, `mouse_button`) e capturar a tela (`screendump`, convertida para PNG com ImageMagick).
- **Olhe as capturas de tela** para confirmar visualmente o que o critério pede. Salve as boas em `docs/capturas/HXX-descricao.png`.
- Todo teste roda nos dois modos de boot (IDE e USB) quando a história mexer no boot.
- Ao fim de cada fase, rodar **todos** os testes anteriores de novo, para garantir que nada quebrou.
- Tempo máximo por execução de teste: 3 minutos. Se travar, matar o QEMU e registrar em `docs/erros.md`.

---

## 8. Testes no hardware real (com o Eli)

O G470 não tem saída de depuração. Se travar, a tela simplesmente fica preta. Por isso:

1. **Mensagens de progresso** em cada etapa do boot (`[1/6] A20 ok`, `[2/6] memoria ok`...). Se travar, o Eli fotografa a tela e a foto mostra onde parou.
2. **Ao fim de cada fase**, o agente:
   1. Roda todos os testes automáticos de todas as fases anteriores.
   2. Gera em `entrega/faseX/` a imagem `.img` (pendrive) e a `.vdi` (VirtualBox), com SHA-256.
   3. Atualiza `docs/como-gravar.md`.
   4. Escreve `docs/teste-faseX.md` com o **roteiro de teste** da fase, em duas etapas, nesta ordem:
      1. **Máquina virtual (VirtualBox, com o `.vdi`):** como criar ou atualizar a VM, o que fazer, o que deve aparecer na tela.
      2. **G470 físico (pendrive com o `.img`):** como gravar, como dar boot, o que deve aparecer, o que fotografar se der errado.
      O Eli testa primeiro na VM. Se der erro, manda o print e o agente corrige antes do teste físico. Se passar na VM, ele testa no G470.
   5. Marca o dashboard como "Aguardando teste do Eli: fase X".
   6. **Para e escreve a mensagem abaixo**, no terminal e no topo do `status.md`.
3. **Mensagem de fim de fase** (modelo):

```
Eli, terminei a fase X: <nome da fase>. Pode testar!

O que foi feito: <resumo em 3 a 5 linhas, em português simples>
Histórias concluídas: HXX a HYY
Imagem: entrega/faseX/saguios.vdi (VirtualBox, testar primeiro) e saguios.img (pendrive, G470)
Roteiro de teste: docs/teste-faseX.md
O que você deve ver no G470: <1 a 3 itens>
Problemas conhecidos: <lista, ou "nenhum">

Quando testar, me diga "fase X ok" ou descreva o que aconteceu.
```

4. **Depois do teste:**
   - Se o Eli disser "fase X ok", rodar `tools/confirmar-hardware.sh "fase X ok"`, que registra a confirmação no diário e no dashboard, e começar a fase seguinte.
   - Se o Eli relatar um problema, criar uma história de correção (`HXX-fix`), registrar em `docs/erros.md` com sintoma, causa e correção, gerar uma nova imagem da fase e enviar a mesma mensagem de novo, dizendo o que foi corrigido. Só seguir depois do "fase X ok".
5. **No teste da fase 6**, o Eli usa um pendrive de dados sem nada importante, formatado em FAT32.

---

## 9. Dashboard

- `dashboard/index.html` em HTML, CSS e JS puros, sem nenhuma dependência externa. O Eli baixa a pasta por FTP e abre localmente.
- Lê `dashboard/status.json` e mostra:
  - Título `SaguiOS: um sistema operacional escrito do zero por IA`.
  - Barra de progresso com a porcentagem geral e por fase.
  - Lista de histórias com estado (a fazer, em andamento, feito, aguardando teste).
  - A tabela da seção 1.1 (critério do Tanenbaum), com cada item marcado conforme for implementado.
  - Últimas ações, erros e correções recentes (causa e correção).
  - Galeria com as capturas de tela mais recentes.
  - Tamanho atual da imagem comparado à meta de 1,44 MB.
  - Tempo gasto por história e número de linhas de código escritas (kernel, boot, userland).

---

## 10. Modo autônomo, limites e economia de tokens

- O agente roda com `/loop` no Claude Code, numa assinatura dedicada a este projeto, dentro de uma sessão tmux.
- Ao bater limite de uso, **esperar o limite renovar e continuar** de onde parou. Ao retomar, ler `status.md` primeiro.
- **Economia de tokens:**
  - Este projeto escreve muito código, então gasta mais que o anterior. Planeje cada história antes de escrever.
  - Não leia arquivos inteiros sem necessidade; prefira buscar trechos.
  - Não fique monitorando processos longos; rode, espere e leia só o resultado.
  - Quando um erro se repetir mais de 3 vezes com a mesma abordagem, pare, registre e troque de estratégia.
- Nunca apagar trabalho validado. Antes de mudanças grandes no boot, na paginação, no escalonador ou no driver USB, criar uma tag no git.

---

## 11. Entregáveis

1. `entrega/fase1/` a `entrega/fase6/`: para cada fase, a imagem `.img` (pendrive) e a `.vdi` (VirtualBox), com SHA-256.
2. `entrega/final/`: `saguios.img` e `saguios.vdi` da versão final.
3. Todo o código-fonte e scripts de build no repositório, prontos para publicação.
4. `docs/` completo: histórias, decisões, diário, erros e capturas.
5. `docs/resumo-final.md`: o que foi feito, tempo de cada fase, número de ciclos de correção, linhas de código, tamanho final da imagem, o que funcionou no G470 e o que ficou de fora, e a tabela da seção 1.1 preenchida.

---

## 12. Definição de sucesso

| Nível | O que acontece no G470 |
| --- | --- |
| Mínimo | Dá boot com carregador próprio e mostra a mensagem do kernel |
| Bom | Terminal com teclado ABNT2 e navegação nos arquivos |
| Muito bom | Todos os conceitos da tabela 1.1: programas ELF em ring 3, multitarefa, proteção de memória, arquivos graváveis e pipes |
| Ótimo | Modo gráfico com mouse |
| Excelente | Área de trabalho com janelas e os programas: Arquivos, Terminal, Editor, Calculadora e Sagui Pega Banana |
| Completo | Salva e abre arquivos num pendrive FAT32, que continuam lá depois de desligar |
| Bônus | Enxerga o HD interno sem mexer nele, dá boot por UEFI ou faz bip no jogo |

