# Como gravar e testar o SaguiOS

Cada fase entrega dois arquivos em `entrega/faseX/` (e a versão final em `entrega/final/`):

| Arquivo | Para quê |
| --- | --- |
| `saguios.img` | Imagem crua do disco. É o que vai, byte a byte, para o pendrive de boot do G470. |
| `saguios.vdi` | A mesma imagem convertida para o formato do VirtualBox. |
| `SHA256SUMS.txt` | Impressões digitais dos dois arquivos, para conferir que o download não corrompeu nada. |

**Ordem de teste:** primeiro no VirtualBox (`.vdi`). Se passar, no G470 (`.img`).

> **Atenção:** gravar a imagem **apaga tudo** que estiver no pendrive. Use um pendrive sem nada importante.
> O SaguiOS nunca grava no disco interno do notebook.

---

## 1. Conferir o arquivo baixado (opcional, recomendado)

- **Windows (PowerShell):** `Get-FileHash .\saguios.img -Algorithm SHA256`
- **Linux:** `sha256sum saguios.img`

O resultado tem que ser igual ao que está no `SHA256SUMS.txt`.

---

## 2. Testar no VirtualBox (`saguios.vdi`)

### Criar a VM (só na primeira vez)

1. **Máquina > Nova**.
2. Nome: `SaguiOS`. Tipo: **Other**. Versão: **Other/Unknown** (32 bits).
3. Memória: **512 MB** (qualquer valor de 64 MB a 2 GB serve).
4. Disco rígido: **Usar um arquivo de disco rígido virtual existente** e escolher o `saguios.vdi`.
5. Antes de ligar, em **Configurações > Sistema > Placa-mãe**, deixe **Habilitar EFI desmarcado**. O SaguiOS dá boot pela BIOS tradicional.
6. Em **Configurações > Armazenamento**, o disco deve estar no controlador **IDE** (é o padrão para "Other").

### Atualizar a VM numa fase nova

O VirtualBox lembra de cada `.vdi` pelo identificador interno, e cada fase gera um arquivo novo:

1. Com a VM desligada, **Configurações > Armazenamento**, selecione o disco antigo e clique em **Remover anexo**.
2. **Arquivo > Ferramentas > Gerenciador de Mídias Virtuais**: selecione o `saguios.vdi` antigo e clique em **Remover** (escolha **Manter** o arquivo, se perguntar; ou apague, tanto faz).
3. Volte em **Armazenamento**, clique no controlador IDE, **Adicionar disco rígido** e escolha o `saguios.vdi` novo.

### Ver o log do kernel no VirtualBox (opcional)

O kernel escreve um log na porta serial. Para ver: **Configurações > Portas seriais > Porta 1**, marque **Habilitar**, modo **Arquivo bruto** e escolha um caminho, por exemplo `C:\temp\sagui-serial.txt`. Depois de ligar a VM, abra esse arquivo no Bloco de Notas.

---

## 3. Gravar o pendrive de boot (`saguios.img`)

Qualquer pendrive serve (o SaguiOS usa menos de 2 MB). Escolha **um** dos programas abaixo.

### Opção A: Rufus (Windows)

1. Abra o Rufus e escolha o pendrive em **Dispositivo**.
2. Em **Seleção de boot**, clique em **SELECIONAR** e escolha o `saguios.img`.
   (Se o filtro de arquivos mostrar só `.iso`, mude o filtro para **Todos os arquivos** ou **\*.img**.)
3. O Rufus reconhece o `.img` como imagem de disco crua e desativa as opções de partição. Isso é o esperado.
4. Clique em **INICIAR**. **Se ele perguntar o modo de gravação, escolha "Gravar em modo de imagem DD".**
5. Confirme o aviso de que os dados do pendrive serão apagados.

> No projeto anterior o Rufus falhou com uma ISO. Com o `.img` cru, em modo DD, ele copia a imagem sem alterar nada.
> Se mesmo assim der problema, use o balenaEtcher.

### Opção B: balenaEtcher (Windows, macOS, Linux)

1. **Flash from file** e escolha o `saguios.img`.
2. **Select target** e escolha o pendrive.
3. **Flash!** O Etcher grava e confere a gravação no final.

### Opção C: `dd` (Linux)

```bash
lsblk                          # descubra qual é o pendrive, ex.: /dev/sdb (NÃO uma partição como /dev/sdb1)
sudo dd if=saguios.img of=/dev/sdX bs=4M conv=fsync status=progress
sync
```

> Confira três vezes o `/dev/sdX`. Errar aqui apaga o disco errado.

### Depois de gravar

O Windows pode dizer que o pendrive "precisa ser formatado" ou que não reconhece a partição. **Não formate**: isso é normal, a partição do SaguiOS tem um formato próprio. Para usar o pendrive para outra coisa depois, basta formatá-lo de novo.

---

## 4. Dar boot no Lenovo G470

1. Desligue o notebook. Espete o pendrive numa porta USB.
2. Ligue e aperte **F2** repetidamente para entrar no **Setup da BIOS**.
   - Em **Boot**, confira que o **Boot Mode** é **Legacy Support** / **Legacy** (se existir a opção).
   - Se houver **Secure Boot**, deixe **Disabled** (o G470, de 2011, normalmente nem tem essa opção).
   - **Save & Exit** (F10).
3. Ligue de novo e aperte **F12** repetidamente para abrir o **menu de boot**.
4. Escolha o pendrive (aparece como **USB HDD: <nome do pendrive>**).
5. O SaguiOS deve começar a carregar.

Se travar ou aparecer erro, **fotografe a tela inteira**. As mensagens `[1/6]` a `[6/6]` mostram em que etapa parou.

### Mensagens de erro do carregador

| Mensagem | O que significa |
| --- | --- |
| `Erro: BIOS sem INT13 LBA` | A BIOS não oferece leitura por LBA para o pendrive. |
| `Erro: cabecalho invalido` | O LBA 1 não tem o cabeçalho do SaguiOS: gravação incompleta ou feita em modo ISO. Grave de novo em modo DD. |
| `Erro de leitura` / `ERRO: falha de leitura do disco` | A BIOS não conseguiu ler o pendrive. Tente outra porta USB ou outro pendrive. |
| `ERRO: dados lidos estao corrompidos` | Leu, mas o conteúdo não bate com a soma do build. Grave de novo; se repetir, tente outro pendrive. |
| `ERRO: nao consegui ligar a linha A20` | Nenhum dos três métodos de A20 funcionou nesse hardware. |
| `ERRO: BIOS nao deu o mapa de memoria` | A BIOS não respondeu ao E820. |

---

## 5. Pendrive de dados (fase 6)

O SaguiOS lê e grava pendrives **FAT32** em `/usb`. Dá para usar o **mesmo pendrive** do boot, porque o sistema roda inteiro na RAM:

1. Dê boot pelo pendrive com o `saguios.img`.
2. Com a área de trabalho no ar, tire o pendrive (o sistema continua rodando).
3. Num PC, formate o pendrive em **FAT32**:
   - **Windows, até 32 GB:** clique com o botão direito no pendrive > **Formatar** > Sistema de arquivos **FAT32**.
   - **Mais de 32 GB:** o Windows não oferece FAT32. Use o **Rufus**: em "Seleção de boot" escolha **Não inicializável** e em "Sistema de arquivos" escolha **FAT32**.
   - **Linux:** `sudo mkfs.vfat -F 32 -n MEUPEN /dev/sdX1` (a partição).
4. Espete o pendrive de volta com o SaguiOS ligado: ele aparece na área de trabalho.

> Formatar apaga o SaguiOS do pendrive: para o próximo boot, grave o `saguios.img` de novo.
>
> **Alternativa sem apagar o boot:** depois de gravar o `saguios.img`, crie uma partição **FAT32** no espaço livre do pendrive (no Windows, **Gerenciamento de Disco** > botão direito no espaço não alocado > **Novo Volume Simples** > FAT32; no Linux, `gparted`). O SaguiOS dá boot pela primeira partição e monta a FAT32 em `/usb`.
> Antes de tirar o pendrive, use **Remover com segurança** (ou `desmontar` no Terminal).
