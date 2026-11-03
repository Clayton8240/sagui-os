# Roteiro de teste: fase 1 (Boot)

**Arquivos:** `entrega/fase1/saguios.vdi` (VirtualBox) e `entrega/fase1/saguios.img` (pendrive).
**Como gravar e configurar:** `docs/como-gravar.md`.

Teste primeiro na máquina virtual. Se der erro, mande o print antes de ir para o G470.

---

## Etapa 1: VirtualBox (`saguios.vdi`)

1. Crie a VM como em `docs/como-gravar.md`, seção 2 (tipo **Other**, sem EFI, disco `saguios.vdi`).
2. Ligue a VM.

**O que deve aparecer** (rápido, menos de 1 segundo cada):

1. A tela da BIOS do VirtualBox e, por um instante, as linhas do carregador:
   ```
   SaguiOS: carregando...
   SaguiOS - estagio 2 do carregador
   [1/6] A20 ok
   [2/6] memoria ok: N regioes, M MB livres
   [3/6] kernel . ok
   [4/6] ramdisk . ok
   [5/6] video: 1024x768x32
   [6/6] entrando no modo protegido
   ```
2. Logo depois, a tela do kernel (veja `docs/capturas/H03-kernel-ide.png`):
   - Faixa verde no topo com **`Ola! Eu sou o SaguiOS.`** em amarelo.
   - **`Memoria detectada: N MB livres`**, com N perto da memória que você deu para a VM.
   - O mapa de memória, a linha A20, o disco de boot, o modo de vídeo VESA disponível e `Ramdisk SGFS: ... 9 entradas`.
   - Em verde: **`Fase 1 concluida: o kernel esta no controle da maquina.`**
3. O sistema fica parado nessa tela. Isso é o esperado na fase 1 (o teclado só chega na fase 2).

**Anote para mim:** a quantidade de memória mostrada e a linha `Video VESA disponivel`.

---

## Etapa 2: Lenovo G470 (`saguios.img` no pendrive)

1. Grave o `saguios.img` no pendrive (Rufus em modo DD, balenaEtcher ou `dd`), como em `docs/como-gravar.md`, seção 3.
2. Dê boot pelo pendrive: **F2** para conferir o modo Legacy na BIOS, depois **F12** e escolha **USB HDD**.

**O que você deve ver no G470:**

1. As linhas `[1/6]` a `[6/6]` passando rápido.
2. A faixa verde com **`Ola! Eu sou o SaguiOS.`**
3. **`Memoria detectada: ... MB livres`** com um valor perto da RAM do notebook (ex.: ~1900 MB se tiver 2 GB, ~3800 MB se tiver 4 GB; parte da memória fica reservada para o vídeo integrado e a BIOS).

**Fotografe**, mesmo se der certo, a tela final do kernel inteira: ela mostra o mapa de memória real do G470, o método da A20 que funcionou e o modo VESA que a placa Intel oferece. Isso me ajuda a preparar as fases seguintes.

**Se der errado, fotografe a tela** e me diga:

- Se a tela ficou preta sem nenhuma mensagem: o pendrive nem foi escolhido como boot, ou a BIOS não aceitou o MBR. Confira o modo Legacy e se o pendrive aparece no F12.
- Se parou numa das linhas `[n/6]`: a foto mostra a etapa exata.
- Se apareceu uma mensagem `Erro`/`ERRO`: a tabela no fim do `docs/como-gravar.md` explica cada uma.

Quando testar, me diga **"fase 1 ok"** ou descreva o que aconteceu.
