# SaguiOS

**Um sistema operacional escrito do zero por IA**, para PCs x86 de 32 bits.
Alvo real: um notebook **Lenovo G470** (2011), além de VirtualBox e QEMU.

Tudo dentro da imagem é escrito neste projeto: carregador de boot, kernel,
drivers, biblioteca dos programas (`libsagui`), shell, fonte e desenhos. Sem
GRUB, sem libc, sem código de outros sistemas. A especificação completa está
em [`prd.md`](prd.md) e cada decisão de projeto, com o raciocínio, está em
[`docs/decisoes.md`](docs/decisoes.md).

## Situação

Veja [`status.md`](status.md) ou abra [`dashboard/index.html`](dashboard/index.html)
no navegador (funciona direto do disco, sem servidor).

| Fase | Conteúdo |
| --- | --- |
| 1. Boot | Carregador próprio (MBR + estágio 2), modo protegido, kernel em C |
| 2. Kernel e terminal | Interrupções, relógio, memória física, teclado ABNT2, SGFS, terminal provisório |
| 3. Processos e programas | Paginação, escalonador, ring 3, chamadas de sistema, ELF, pipes, shell `sagsh` |
| 4. Gráfico e mouse | VESA, fonte própria com acentos, touchpad PS/2 |
| 5. Janelas e programas | Área de trabalho, janelas, Arquivos, Terminal, Editor, Calculadora, Sagui Pega Banana |
| 6. Pendrive de dados | Driver USB EHCI próprio e FAT32 com leitura e gravação |

## Estrutura

```
boot/        estágio 1 (MBR) e estágio 2 do carregador, em NASM
kernel/      kernel em C freestanding + NASM
userland/    libsagui e programas de usuário (a partir da fase 3)
rootfs/      arquivos que vão para o ramdisk SGFS
tools/       scripts de build (mksgfs.py, mkimagem.py), status.py
tests/       testes automáticos no QEMU, um por história
docs/        histórias, decisões, diário, erros, capturas, roteiros de teste
dashboard/   painel do projeto (HTML puro + status.json)
entrega/     imagens .img e .vdi de cada fase, com SHA-256
```

## Compilar e testar

Requisitos (Ubuntu): `nasm gcc gcc-multilib binutils make python3 qemu-system-x86 qemu-utils imagemagick mtools dosfstools`.

```bash
make            # build/saguios.img
make vdi        # build/saguios.vdi (VirtualBox)
make test       # todos os testes no QEMU (sem janela)
make rodar      # abre no QEMU com janela
```

Para gravar num pendrive e dar boot num PC real, veja [`docs/como-gravar.md`](docs/como-gravar.md).
