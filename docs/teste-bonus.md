# Roteiro de teste: fase bônus (B3 bip e B4 Sagui 3D)

**Arquivos:** `entrega/bonus/saguios.vdi` (VirtualBox) e `entrega/bonus/saguios.img` (pendrive).
As mesmas imagens estão em `entrega/final/`. Como gravar: `docs/como-gravar.md`.

B1 (HD interno) e B2 (UEFI) foram cancelados por você e aparecem assim no painel.

---

## B3: bip

| O que fazer | O que deve acontecer |
| --- | --- |
| No Terminal: `bip` | Um bip de 880 Hz por 0,2 s. |
| `bip 440 1000` | Um lá grave por 1 segundo. |
| `bip escala` | Dó-ré-mi-fá-sol-lá-si-dó. |
| Sagui Pega Banana: pegar uma banana / deixar uma cair | Bip agudo curto / dois tons descendo. |

**VirtualBox:** o alto-falante do PC costuma sair pelo som do computador (se não sair, tudo bem: o PRD já previa).
**G470:** o PRD avisa que o notebook pode não ligar esse sinal ao alto-falante. Me diga se tocou ou não, para eu registrar.

---

## B4: Sagui 3D

Abra pelo ícone **Sagui 3D** na área de trabalho (ou `sagui3d` no Terminal).

**Controles:** setas andam e giram; **Shift** corre; **Ctrl** ou **espaço** atira com o estilingue;
**B** larga uma casca de banana; **Esc** pausa (na pausa, **Q** sai); o mouse sobre a vista gira o sagui e o clique atira.

**O jogo:** três fases (A clareira de pedra, O rio seco, O templo do sagui). Junte bananas: a barra de baixo mostra
`atuais/mínimo` da porteira (fica verde quando dá para sair). Cada banana vale 10 pontos e deixa uma casca;
cinco bananas dão uma vida. Os macacos-prego vêm pegar bananas do seu bolso (tiram uma vida): três sementes os deixam
tontos e eles fogem; uma casca no caminho faz o macaco escorregar e girar. Em cada fase há um **cacho dourado**
atrás de uma parede de cipó que dá para atravessar (dica: procure cipó onde não deveria ter).

**O que conferir:**

| O que fazer | O que deve acontecer |
| --- | --- |
| Andar pelas salas | Paredes com textura de pedra, cipó e madeira; salas com céu, salas escuras e claras. |
| Subir a escada da fase 1 | O sagui sobe degrau por degrau até a plataforma. |
| Chegar à porteira com poucas bananas | `A porteira pede N bananas (faltam M).` e um som grave. |
| Atirar num macaco três vezes | Ele fica tonto (estrelinhas) e depois foge. |
| Comer uma banana, apertar **B** com um macaco vindo | Ele escorrega, gira e fica tonto. |
| Juntar o mínimo e voltar à porteira | Melodia de fim de fase e a tela de vitória com o cacho dourado "encontrado / não encontrado". |
| Perder as três vidas | Tela de fim de jogo. |
| Terminar as três fases | Tela "Liberdade!" com os pontos e quantos cachos dourados achou. |
| Jogar com o bip | Sons diferentes para tiro, banana, acerto, vida perdida, casca, escorregão e fim de fase, sem o jogo travar. |

**Fluidez:** no QEMU o jogo desenha 80 a 110 quadros por segundo; no G470 deve ser mais. Se ficar lento, me diga.

Quando testar, me diga **"bônus ok"** ou o que aconteceu (fotos ajudam).
