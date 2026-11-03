#!/usr/bin/env python3
"""
mkfonte.py - gera o C da fonte 8x16 do SaguiOS a partir de tools/fonte/sagui8x16.txt.

Uso: mkfonte.py <sagui8x16.txt> <saida.c>

Saída: um vetor "fonte_sagui[256][16]" (uint8_t, um byte por linha, bit 7 =
coluna da esquerda), indexado pelo código Latin-1. O mesmo arquivo serve
para o kernel e para a libsagui (fase 5).

As letras acentuadas do Latin-1 são compostas aqui: letra base + acento.
  - maiúsculas: acento nas linhas 0 e 1 da célula (acima da letra);
  - minúsculas: acento nas linhas 4 e 5 (acima da altura do "x"); no "i",
    a base é o "i sem ponto";
  - ç e Ç: cedilha nas linhas 13 e 14.
Caracteres sem desenho (de controle, ou símbolos raros) usam a "caixa".
"""
import sys

LINHAS_ARQUIVO = 13   # linhas 2 a 14 da célula
PRIMEIRA = 2


def ler(caminho):
    blocos, nome, linhas = {}, None, []
    for bruto in open(caminho, encoding="utf-8"):
        linha = bruto.rstrip("\n")
        if linha.startswith("#") and nome is None:
            continue
        if linha.startswith("@"):
            if nome is not None:
                blocos[nome] = linhas
            nome, linhas = linha[1:], []
            continue
        if linha.startswith("#") and (len(linha) != 8 or any(c not in "#." for c in linha)):
            continue   # comentário no meio do arquivo
        if nome is not None and linha:
            if len(linha) != 8 or any(c not in "#." for c in linha):
                sys.exit(f"mkfonte: linha inválida em @{nome}: {linha!r}")
            linhas.append(linha)
    if nome is not None:
        blocos[nome] = linhas
    return blocos


def para_bytes(linhas):
    return [int(l.replace("#", "1").replace(".", "0"), 2) for l in linhas]


def celula(bloco):
    if len(bloco) != LINHAS_ARQUIVO:
        raise ValueError(f"esperava {LINHAS_ARQUIVO} linhas, veio {len(bloco)}")
    c = [0] * 16
    for i, v in enumerate(para_bytes(bloco)):
        c[PRIMEIRA + i] = v
    return c


# Letras acentuadas do Latin-1: código -> (base, acento)
COMPOSTAS = {
    0xC0: ("A", "grave"), 0xC1: ("A", "agudo"), 0xC2: ("A", "circunflexo"), 0xC3: ("A", "til"),
    0xC4: ("A", "trema"), 0xC5: ("A", "anel"), 0xC7: ("C", "cedilha"),
    0xC8: ("E", "grave"), 0xC9: ("E", "agudo"), 0xCA: ("E", "circunflexo"), 0xCB: ("E", "trema"),
    0xCC: ("I", "grave"), 0xCD: ("I", "agudo"), 0xCE: ("I", "circunflexo"), 0xCF: ("I", "trema"),
    0xD1: ("N", "til"), 0xD2: ("O", "grave"), 0xD3: ("O", "agudo"), 0xD4: ("O", "circunflexo"),
    0xD5: ("O", "til"), 0xD6: ("O", "trema"), 0xD9: ("U", "grave"), 0xDA: ("U", "agudo"),
    0xDB: ("U", "circunflexo"), 0xDC: ("U", "trema"), 0xDD: ("Y", "agudo"),
    0xE0: ("a", "grave"), 0xE1: ("a", "agudo"), 0xE2: ("a", "circunflexo"), 0xE3: ("a", "til"),
    0xE4: ("a", "trema"), 0xE5: ("a", "anel"), 0xE7: ("c", "cedilha"),
    0xE8: ("e", "grave"), 0xE9: ("e", "agudo"), 0xEA: ("e", "circunflexo"), 0xEB: ("e", "trema"),
    0xEC: ("i_sem_ponto", "grave"), 0xED: ("i_sem_ponto", "agudo"), 0xEE: ("i_sem_ponto", "circunflexo"),
    0xEF: ("i_sem_ponto", "trema"), 0xF1: ("n", "til"), 0xF2: ("o", "grave"), 0xF3: ("o", "agudo"),
    0xF4: ("o", "circunflexo"), 0xF5: ("o", "til"), 0xF6: ("o", "trema"), 0xF9: ("u", "grave"),
    0xFA: ("u", "agudo"), 0xFB: ("u", "circunflexo"), 0xFC: ("u", "trema"), 0xFD: ("y", "agudo"),
    0xFF: ("y", "trema"),
}


def compor(blocos, base, acento):
    c = celula(blocos[base])
    peca = para_bytes(blocos[acento])
    maiuscula = base[0].isupper()
    if acento == "cedilha":
        linhas = (13, 14)
    elif maiuscula:
        linhas = (0, 1)
    else:
        linhas = (4, 5)
        c[4] = c[5] = 0   # nas minúsculas, abre espaço caso a letra suba até ali
    for i, l in enumerate(linhas):
        c[l] |= peca[i]
    return c


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    blocos = ler(sys.argv[1])
    caixa = celula(blocos["caixa"])
    fonte = [list(caixa) for _ in range(256)]
    fonte[0x20] = celula(blocos["espaco"])
    fonte[0xA0] = celula(blocos["espaco"])          # espaço não separável
    faltando = []
    for nome, bloco in blocos.items():
        if len(nome) != 1:
            continue
        codigo = nome.encode("latin-1")[0]
        fonte[codigo] = celula(bloco)
    for codigo, (base, acento) in COMPOSTAS.items():
        fonte[codigo] = compor(blocos, base, acento)
    for c in range(0x21, 0x7F):
        if fonte[c] == caixa:
            faltando.append(chr(c))
    if faltando:
        sys.exit(f"mkfonte: faltam caracteres ASCII: {''.join(faltando)}")
    with open(sys.argv[2], "w") as f:
        f.write("/* Gerado por tools/mkfonte.py a partir de tools/fonte/sagui8x16.txt. Não editar. */\n")
        f.write("const unsigned char fonte_sagui[256][16] = {\n")
        for c in range(256):
            f.write("    {" + ", ".join(f"0x{v:02X}" for v in fonte[c]) + f"}},  /* 0x{c:02X} */\n")
        f.write("};\n")
    desenhados = sum(1 for c in range(256) if fonte[c] != caixa)
    print(f"mkfonte: {desenhados} caracteres desenhados -> {sys.argv[2]}")


if __name__ == "__main__":
    main()
