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
