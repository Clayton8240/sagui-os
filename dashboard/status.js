// Gerado por tools/status.py a partir de status.json. Não editar.
window.SAGUI_STATUS = {
  "titulo": "SaguiOS: um sistema operacional escrito do zero por IA",
  "atualizado": "2026-11-03T15:39:25",
  "situacao": "Trabalhando na H09: Terminal provisório do kernel",
  "fase_atual": 2,
  "historia_atual": "H09",
  "bloqueio": "",
  "mensagem_fase": "",
  "fases": [
    {
      "numero": 1,
      "nome": "Boot",
      "estado": "feito",
      "progresso": 100.0,
      "confirmado": "2026-11-03T15:22:25"
    },
    {
      "numero": 2,
      "nome": "Kernel e terminal",
      "estado": "em andamento",
      "progresso": 80.0
    },
    {
      "numero": 3,
      "nome": "Processos e programas",
      "estado": "a fazer",
      "progresso": 0.0
    },
    {
      "numero": 4,
      "nome": "Gráfico e mouse",
      "estado": "a fazer",
      "progresso": 0.0
    },
    {
      "numero": 5,
      "nome": "Janelas e programas",
      "estado": "a fazer",
      "progresso": 0.0
    },
    {
      "numero": 6,
      "nome": "Pendrive de dados",
      "estado": "a fazer",
      "progresso": 0.0
    },
    {
      "numero": 7,
      "nome": "Bônus",
      "estado": "a fazer",
      "progresso": 0
    }
  ],
  "historias": [
    {
      "id": "H01",
      "fase": 1,
      "titulo": "Preparar o ambiente",
      "objetivo": "Instalar NASM, QEMU, ImageMagick, Python; validar o GCC freestanding; criar pastas, git, status.md e dashboard.",
      "aceite": "`make` roda sem erro com um alvo vazio; dashboard abre no navegador.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T14:12:34",
      "fim": "2026-11-03T14:14:17",
      "minutos": 2,
      "resultado": "Ambiente pronto: NASM, GCC -m32, QEMU, ImageMagick; estrutura, git e painel criados"
    },
    {
      "id": "H02",
      "fase": 1,
      "titulo": "Estágio 1 (MBR)",
      "objetivo": "Código de 512 bytes com assinatura 0x55AA, tabela de partição válida com partição ativa e área de BPB reservada.",
      "aceite": "No QEMU (IDE e USB), aparece a mensagem `SaguiOS: carregando...`.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T14:14:29",
      "fim": "2026-11-03T14:22:05",
      "minutos": 8,
      "resultado": "MBR de 512 bytes com BPB reservado e partição ativa; mensagem aparece em IDE e USB"
    },
    {
      "id": "H03",
      "fase": 1,
      "titulo": "Estágio 2 e modo protegido",
      "objetivo": "A20, mapa E820, leitura do kernel e do ramdisk, GDT, modo protegido, salto para o kernel com boot_info.",
      "aceite": "O kernel em C escreve em modo texto VGA `Ola! Eu sou o SaguiOS.` e a memória detectada.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T14:22:05",
      "fim": "2026-11-03T14:22:52",
      "minutos": 1,
      "resultado": "Estágio 2 completo e kernel em C em modo protegido; testado em IDE e USB com 512 e 128 MB"
    },
    {
      "id": "H04",
      "fase": 1,
      "titulo": "Imagem para pendrive e VirtualBox",
      "objetivo": "Build gera build/saguios.img com SHA-256 e build/saguios.vdi; docs/como-gravar.md.",
      "aceite": "Boot nos dois modos do QEMU; .vdi gerado; arquivos em entrega/fase1/; como-gravar.md com Rufus (DD) e balenaEtcher.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T14:22:52",
      "fim": "2026-11-03T14:27:10",
      "minutos": 4,
      "resultado": "Imagem .img e .vdi com SHA-256; boot do .img (IDE e USB) e do .vdi no QEMU; como-gravar.md"
    },
    {
      "id": "H05",
      "fase": 2,
      "titulo": "Interrupções e relógio",
      "objetivo": "IDT, exceções da CPU com nome e registradores, remapeamento do PIC, PIT a 100 Hz, contador de tempo.",
      "aceite": "Divisão por zero mostra a tela de erro do SaguiOS; o contador de segundos avança.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T15:24:57",
      "fim": "2026-11-03T15:28:28",
      "minutos": 4,
      "resultado": "IDT com 256 vetores, tela de erro, PIC remapeado, PIT a 100 Hz, CMOS"
    },
    {
      "id": "H06",
      "fase": 2,
      "titulo": "Memória física",
      "objetivo": "Bitmap de páginas de 4 KB a partir do E820 e alocador kmalloc/kfree.",
      "aceite": "Teste aloca e libera blocos de vários tamanhos sem corromper; diagnóstico mostra memória total e livre.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T15:28:28",
      "fim": "2026-11-03T15:32:06",
      "minutos": 4,
      "resultado": "Bitmap de páginas a partir do E820 e kmalloc por classes; autoteste de 450 blocos sem corrupção"
    },
    {
      "id": "H07",
      "fase": 2,
      "titulo": "Teclado PS/2 com ABNT2",
      "objetivo": "Driver no IRQ1, buffer, Shift, Caps Lock, Ctrl, teclas mortas e o mapa ABNT2.",
      "aceite": "Teclas enviadas pelo monitor do QEMU aparecem corretas, incluindo ç.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T15:32:06",
      "fim": "2026-11-03T15:37:48",
      "minutos": 6,
      "resultado": "Driver PS/2 com ABNT2 completo, teclas mortas, AltGr, Caps Lock; 25 grupos de teclas conferidos byte a byte"
    },
    {
      "id": "H08",
      "fase": 2,
      "titulo": "Sistema de arquivos SGFS",
      "objetivo": "tools/mksgfs.py e leitor no kernel: listar pasta, abrir e ler arquivo.",
      "aceite": "Teste interno lista as entradas do rootfs/ e lê o leia-me.txt inteiro.",
      "estado": "feito",
      "bonus": false,
      "inicio": "2026-11-03T15:37:48",
      "fim": "2026-11-03T15:39:25",
      "minutos": 2,
      "resultado": "mksgfs.py + leitor no kernel; lista igual ao rootfs/ e leia-me.txt idêntico"
    },
    {
      "id": "H09",
      "fase": 2,
      "titulo": "Terminal provisório do kernel",
      "objetivo": "Prompt sagui:/$ com ajuda, ls, cd, cat, limpar, mem, data, reiniciar.",
      "aceite": "Roteiro digita cada comando no QEMU e confere pela captura e pela serial.",
      "estado": "em andamento",
      "bonus": false,
      "inicio": "2026-11-03T15:39:25",
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H10",
      "fase": 3,
      "titulo": "Paginação",
      "objetivo": "Diretório e tabelas de páginas, kernel em todos os espaços, page fault com mensagem clara.",
      "aceite": "Dois espaços de endereço com valores diferentes no mesmo endereço virtual; acesso inválido gera a tela de page fault.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H11",
      "fase": 3,
      "titulo": "Processos e escalonador",
      "objetivo": "PCB, troca de contexto em assembly, round-robin preemptivo no PIT, processo ocioso.",
      "aceite": "Duas tarefas imprimem A e B alternados na serial sem cooperar.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H12",
      "fase": 3,
      "titulo": "Ring 3 e chamadas de sistema",
      "objetivo": "TSS, segmentos de usuário, ring 3, int 0x80, validação de ponteiros.",
      "aceite": "Programa em ring 3 escreve `Ola do ring 3!` e sai; `cli` no ring 3 encerra só aquele processo.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H13",
      "fase": 3,
      "titulo": "Carregador ELF e libsagui",
      "objetivo": "Leitor de ELF32 estático, pilha do usuário com argumentos, libsagui com printf.",
      "aceite": "O kernel carrega bin/ola do SGFS e ele imprime os argumentos.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H14",
      "fase": 3,
      "titulo": "Arquivos e pastas graváveis",
      "objetivo": "Camada gravável na RAM sobre o SGFS, tabela de arquivos por processo, terminal como 0 e 1.",
      "aceite": "Teste em ring 3 cria, escreve, relê e apaga tmp/teste.txt; o mesmo com uma pasta.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H15",
      "fase": 3,
      "titulo": "Pipes",
      "objetivo": "pipe e duplicar, buffer no kernel, bloqueio de leitor e escritor, EOF.",
      "aceite": "Dois processos transferem por pipe um texto maior que o buffer sem perda nem duplicação.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H16",
      "fase": 3,
      "titulo": "Shell e comandos como programas",
      "objetivo": "bin/sagsh como primeiro processo, redirecionamento, pipe, histórico, Tab, comandos e apelidos.",
      "aceite": "Roteiro executa todos os comandos e apelidos; falha derruba só o processo; capturas em docs/capturas/.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H17",
      "fase": 4,
      "titulo": "Framebuffer VESA",
      "objetivo": "Troca para o modo VESA, double buffer e primitivas de desenho.",
      "aceite": "Captura com fundo colorido e formas; se o VESA falhar, volta ao modo texto e avisa.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H18",
      "fase": 4,
      "titulo": "Fonte própria",
      "objetivo": "Fonte bitmap 8x16 própria com ASCII e acentos Latin-1; terminal gráfico.",
      "aceite": "Captura mostra `Ação, vovô, pão, ÇÃO, à é í ó ú` correto.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H19",
      "fase": 4,
      "titulo": "Mouse / touchpad PS/2",
      "objetivo": "Porta auxiliar do i8042, IRQ12, pacotes de 3 bytes, cursor próprio, botões.",
      "aceite": "Movimentos do monitor do QEMU movem o cursor; clique registrado na serial.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H20",
      "fase": 5,
      "titulo": "Área de trabalho",
      "objetivo": "Degradê nas cores do Brasil, mascote sagui em pixel art, barra superior com relógio.",
      "aceite": "Captura da área de trabalho em docs/capturas/.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H21",
      "fase": 5,
      "titulo": "Gerenciador de janelas",
      "objetivo": "Janelas com moldura, arrastar, foco; chamadas de janela para programas em ring 3.",
      "aceite": "Dois programas abrem janelas; o teste arrasta uma, fecha a outra e o programa termina.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H22",
      "fase": 5,
      "titulo": "Kit gráfico da libsagui",
      "objetivo": "Texto, botão, rótulo, caixa de texto, área de texto, lista, confirmação.",
      "aceite": "Demonstração com todos os componentes funcionando; captura.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H23",
      "fase": 5,
      "titulo": "Arquivos",
      "objetivo": "Gerenciador de arquivos em ring 3 com ícones, duplo clique, voltar, criar e apagar pasta.",
      "aceite": "Navega até historia/, abre linux.txt no Editor, cria e apaga pasta em tmp/.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H24",
      "fase": 5,
      "titulo": "Terminal em janela",
      "objetivo": "Janela que roda o sagsh ligado por pipes; comandos que abrem os programas gráficos.",
      "aceite": "Captura do terminal em janela rodando ps e ls bin | conta.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H25",
      "fase": 5,
      "titulo": "Editor de texto",
      "objetivo": "Novo, abrir, salvar, salvar como, acentos, setas, Ctrl+S, asterisco, aviso ao fechar.",
      "aceite": "Cria tmp/nota.txt com `Olá, SaguiOS! Ação e coração.`, reabre pelo Arquivos e confere.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H26",
      "fase": 5,
      "titulo": "Calculadora",
      "objetivo": "Botões e teclado, ponto fixo, avisos de divisão por zero e número grande.",
      "aceite": "12,5 × 4 = 50; 7 ÷ 2 = 3,5; 1 ÷ 0 mostra aviso; captura.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H27",
      "fase": 5,
      "titulo": "Sagui Pega Banana",
      "objetivo": "Jogo autoral: sagui pega bananas, 3 vidas, velocidade crescente, recorde.",
      "aceite": "Teste joga com setas pelo monitor; capturas do jogo e do fim de jogo.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H28",
      "fase": 5,
      "titulo": "Acabamento",
      "objetivo": "Ícones na área de trabalho e tela Sobre o SaguiOS.",
      "aceite": "Captura com três programas abertos; tamanho da imagem medido.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H29",
      "fase": 6,
      "titulo": "PCI e controlador EHCI",
      "objetivo": "Varredura PCI, passagem de controle da BIOS, reset do EHCI, portas.",
      "aceite": "Diagnóstico lista controladores EHCI e portas com dispositivo no QEMU.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H30",
      "fase": 6,
      "titulo": "Enumeração USB, hubs e conexão a quente",
      "objetivo": "Transferências de controle, descritores, endereços, hubs, conexão e remoção.",
      "aceite": "Com hub virtual, lista fabricante, produto e classe; eventos de conexão e remoção na serial.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H31",
      "fase": 6,
      "titulo": "Armazenamento em massa",
      "objetivo": "Bulk-Only Transport e SCSI: identificar, capacidade, ler e gravar.",
      "aceite": "Lê e grava setores numa imagem de teste e confere byte a byte na VPS.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H32",
      "fase": 6,
      "titulo": "FAT32: leitura",
      "objetivo": "Partições MBR, FAT32 com nomes curtos e longos, montagem em /usb.",
      "aceite": "Pendrive de teste com pastas, acentos e nomes longos é lido por ls e cat.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H33",
      "fase": 6,
      "titulo": "FAT32: gravação",
      "objetivo": "Criar, sobrescrever, aumentar, apagar; pastas; duas FATs; FSInfo; desmontar grava tudo.",
      "aceite": "Após desmontar, mtools e fsck.vfat confirmam integridade e conteúdo.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "H34",
      "fase": 6,
      "titulo": "Integração do pendrive",
      "objetivo": "Notificação, ícone Pendrive, montar e remover com segurança, fluxo pelo terminal.",
      "aceite": "Roteiro completo pela interface e pelo terminal; VPS confere com mtools; capturas.",
      "estado": "a fazer",
      "bonus": false,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "B1",
      "fase": 7,
      "titulo": "Bônus: enxergar o HD interno",
      "objetivo": "Driver SATA somente leitura para listar o HD e partições.",
      "aceite": "discos mostra o HD e partições sem montar nem gravar.",
      "estado": "a fazer",
      "bonus": true,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "B2",
      "fase": 7,
      "titulo": "Bônus: boot por UEFI",
      "objetivo": "Carregador UEFI com GOP.",
      "aceite": "Boot em máquina UEFI.",
      "estado": "a fazer",
      "bonus": true,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    },
    {
      "id": "B3",
      "fase": 7,
      "titulo": "Bônus: bip",
      "objetivo": "Alto-falante do PC pelo canal 2 do PIT.",
      "aceite": "Bip no jogo e comando bip.",
      "estado": "a fazer",
      "bonus": true,
      "inicio": null,
      "fim": null,
      "minutos": null,
      "resultado": ""
    }
  ],
  "tanenbaum": [
    {
      "conceito": "Modo kernel e modo usuário",
      "implementa": "Kernel em ring 0, programas em ring 3",
      "historias": "H03, H12",
      "estado": "parcial"
    },
    {
      "conceito": "Processos",
      "implementa": "Criação, término, estados, PID, pai e filho",
      "historias": "H11, H13",
      "estado": "a fazer"
    },
    {
      "conceito": "Escalonamento",
      "implementa": "Round-robin preemptivo no relógio",
      "historias": "H11",
      "estado": "a fazer"
    },
    {
      "conceito": "Comunicação entre processos",
      "implementa": "Pipes entre programas (ls | conta)",
      "historias": "H15",
      "estado": "a fazer"
    },
    {
      "conceito": "Espaços de endereço",
      "implementa": "Paginação com um espaço por processo",
      "historias": "H10",
      "estado": "a fazer"
    },
    {
      "conceito": "Gerenciamento de memória",
      "implementa": "Memória física, alocador do kernel, memória dos processos",
      "historias": "H06, H10, H13",
      "estado": "parcial"
    },
    {
      "conceito": "Arquivos e diretórios",
      "implementa": "Abrir, ler, escrever, criar, apagar, na RAM e no pendrive FAT32",
      "historias": "H08, H14, H32, H33",
      "estado": "a fazer"
    },
    {
      "conceito": "Entrada e saída",
      "implementa": "Interrupções e drivers próprios: teclado, mouse, vídeo, relógio, USB",
      "historias": "H05, H07, H17, H19, H29-H31",
      "estado": "parcial"
    },
    {
      "conceito": "Proteção",
      "implementa": "Um processo não acessa a memória do outro nem o kernel",
      "historias": "H12, H16",
      "estado": "a fazer"
    },
    {
      "conceito": "Chamadas de sistema",
      "implementa": "Processos, arquivos, diretórios e diversas",
      "historias": "H12",
      "estado": "a fazer"
    },
    {
      "conceito": "O shell",
      "implementa": "sagsh em ring 3, com redirecionamento e pipes",
      "historias": "H16",
      "estado": "a fazer"
    },
    {
      "conceito": "Carregamento de programas",
      "implementa": "Executáveis ELF lidos do sistema de arquivos",
      "historias": "H13",
      "estado": "a fazer"
    },
    {
      "conceito": "Interface gráfica (extra)",
      "implementa": "Gerenciador de janelas no kernel; programas gráficos em ring 3",
      "historias": "H20-H28",
      "estado": "a fazer"
    }
  ],
  "acoes": [
    {
      "data": "2026-11-03T15:39:25",
      "texto": "Início da H09: Terminal provisório do kernel"
    },
    {
      "data": "2026-11-03T15:39:25",
      "texto": "H08 concluída: mksgfs.py + leitor no kernel; lista igual ao rootfs/ e leia-me.txt idêntico"
    },
    {
      "data": "2026-11-03T15:39:25",
      "texto": "Erro corrigido: Saudação e memória quebravam linha"
    },
    {
      "data": "2026-11-03T15:39:25",
      "texto": "Erro corrigido: Árvore do SGFS saiu como %*s na tela"
    },
    {
      "data": "2026-11-03T15:37:48",
      "texto": "Início da H08: Sistema de arquivos SGFS"
    },
    {
      "data": "2026-11-03T15:37:48",
      "texto": "H07 concluída: Driver PS/2 com ABNT2 completo, teclas mortas, AltGr, Caps Lock; 25 grupos de teclas conferidos byte a byte"
    },
    {
      "data": "2026-11-03T15:32:06",
      "texto": "Início da H07: Teclado PS/2 com ABNT2"
    },
    {
      "data": "2026-11-03T15:32:06",
      "texto": "H06 concluída: Bitmap de páginas a partir do E820 e kmalloc por classes; autoteste de 450 blocos sem corrupção"
    },
    {
      "data": "2026-11-03T15:28:28",
      "texto": "Início da H06: Memória física"
    },
    {
      "data": "2026-11-03T15:28:28",
      "texto": "H05 concluída: IDT com 256 vetores, tela de erro, PIC remapeado, PIT a 100 Hz, CMOS"
    },
    {
      "data": "2026-11-03T15:24:57",
      "texto": "Início da H05: Interrupções e relógio"
    },
    {
      "data": "2026-11-03T15:24:42",
      "texto": "H04-fix concluída: mapa E820 só na serial, saudação fica no topo; memória abaixo de 4 GB mostrada à parte"
    },
    {
      "data": "2026-11-03T15:23:28",
      "texto": "Início da H04-fix: saudação some no G470 por causa do mapa E820 grande"
    },
    {
      "data": "2026-11-03T15:22:25",
      "texto": "Eli confirmou: fase 1 ok"
    },
    {
      "data": "2026-11-03T14:27:18",
      "texto": "Fase 1 entregue. Aguardando teste do Eli."
    },
    {
      "data": "2026-11-03T14:27:10",
      "texto": "Todos os testes da fase 1 passaram (H01 a H04, IDE e USB)"
    },
    {
      "data": "2026-11-03T14:27:10",
      "texto": "H04 concluída: Imagem .img e .vdi com SHA-256; boot do .img (IDE e USB) e do .vdi no QEMU; como-gravar.md"
    },
    {
      "data": "2026-11-03T14:22:52",
      "texto": "Início da H04: Imagem para pendrive e VirtualBox"
    },
    {
      "data": "2026-11-03T14:22:52",
      "texto": "H03 concluída: Estágio 2 completo e kernel em C em modo protegido; testado em IDE e USB com 512 e 128 MB"
    },
    {
      "data": "2026-11-03T14:22:05",
      "texto": "Início da H03: Estágio 2 e modo protegido"
    },
    {
      "data": "2026-11-03T14:22:05",
      "texto": "H02 concluída: MBR de 512 bytes com BPB reservado e partição ativa; mensagem aparece em IDE e USB"
    },
    {
      "data": "2026-11-03T14:14:29",
      "texto": "Início da H02: Estágio 1 (MBR)"
    },
    {
      "data": "2026-11-03T14:14:17",
      "texto": "H01 concluída: Ambiente pronto: NASM, GCC -m32, QEMU, ImageMagick; estrutura, git e painel criados"
    },
    {
      "data": "2026-11-03T14:12:34",
      "texto": "Início da H01: Preparar o ambiente"
    }
  ],
  "erros": [
    {
      "data": "2026-11-03T15:39:25",
      "sintoma": "Saudação e memória quebravam linha",
      "causa": "Textos com mais de 80 colunas",
      "correcao": "Larguras fixas que somam 80"
    },
    {
      "data": "2026-11-03T15:39:25",
      "sintoma": "Árvore do SGFS saiu como %*s na tela",
      "causa": "formatar() não aceitava largura com *",
      "correcao": "Suporte a * no formatar()"
    }
  ],
  "capturas": [
    {
      "arquivo": "docs/capturas/H08-sgfs-lista.png",
      "nome": "H08-sgfs-lista.png",
      "data": "2026-11-03T15:39:06"
    },
    {
      "arquivo": "docs/capturas/H07-teclado-abnt2.png",
      "nome": "H07-teclado-abnt2.png",
      "data": "2026-11-03T15:37:28"
    },
    {
      "arquivo": "docs/capturas/H04-fix-memoria-6g.png",
      "nome": "H04-fix-memoria-6g.png",
      "data": "2026-11-03T15:37:19"
    },
    {
      "arquivo": "docs/capturas/H03-kernel-usb.png",
      "nome": "H03-kernel-usb.png",
      "data": "2026-11-03T15:37:07"
    },
    {
      "arquivo": "docs/capturas/H03-kernel-ide.png",
      "nome": "H03-kernel-ide.png",
      "data": "2026-11-03T15:36:56"
    },
    {
      "arquivo": "docs/capturas/H03-estagio2-usb.png",
      "nome": "H03-estagio2-usb.png",
      "data": "2026-11-03T15:36:51"
    },
    {
      "arquivo": "docs/capturas/H03-estagio2-ide.png",
      "nome": "H03-estagio2-ide.png",
      "data": "2026-11-03T15:36:45"
    },
    {
      "arquivo": "docs/capturas/H06-autoteste-memoria.png",
      "nome": "H06-autoteste-memoria.png",
      "data": "2026-11-03T15:31:45"
    },
    {
      "arquivo": "docs/capturas/H05-tela-de-erro-divisao.png",
      "nome": "H05-tela-de-erro-divisao.png",
      "data": "2026-11-03T15:27:54"
    },
    {
      "arquivo": "docs/capturas/G470-fase1-tela-final.png",
      "nome": "G470-fase1-tela-final.png",
      "data": "2026-11-03T15:22:25"
    },
    {
      "arquivo": "docs/capturas/H04-boot-vdi.png",
      "nome": "H04-boot-vdi.png",
      "data": "2026-11-03T14:26:38"
    },
    {
      "arquivo": "docs/capturas/H02-mbr-usb.png",
      "nome": "H02-mbr-usb.png",
      "data": "2026-11-03T14:25:48"
    },
    {
      "arquivo": "docs/capturas/H02-mbr-ide.png",
      "nome": "H02-mbr-ide.png",
      "data": "2026-11-03T14:25:42"
    }
  ],
  "linhas": {
    "boot": 899,
    "kernel": 2371,
    "userland": 0,
    "ferramentas": 1164
  },
  "imagem": {
    "bytes_util": 34816,
    "bytes_arquivo": 1048576,
    "meta_bytes": 1474560
  },
  "progresso_geral": 23.5
};
