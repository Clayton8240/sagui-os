#!/usr/bin/env python3
"""H09 - terminal provisório do kernel.

Critério: roteiro digita cada comando no QEMU e confere a saída pela captura
de tela e pela serial: ajuda, ls, cd, cat, limpar, mem, data, reiniciar.
"""
import datetime
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from qemu import Maquina, verificar, RAIZ  # noqa: E402

FIM = "[SAGUI] fim do comando"


def comando(m, texto, paginas=False):
    """Digita o comando, aperta Enter e espera o terminal terminar. Se houver
    paginação, aperta Espaço a cada '-- mais'. Devolve a saída na serial."""
    antes = m.serial()
    n = antes.count(FIM)
    m.digitar(texto + "\n", 0.05)
    fim = time.time() + 60
    while m.serial().count(FIM) <= n:
        if time.time() > fim:
            raise AssertionError(f"comando {texto!r} não terminou")
        if paginas and any("-- mais" in l for l in m.tela_texto()):
            m.tecla("spc")
        time.sleep(0.3)
    return m.serial()[len(antes):].replace("\r", "")


def main():
    for modo in ("ide", "usb"):
        with Maquina("h09", modo) as m:
            m.esperar_serial("[SAGUI] terminal pronto", 120)
            verificar("sagui:/$" in "\n".join(m.tela_texto()), f"[{modo}] prompt sagui:/$ na tela")
            if modo == "usb":
                out = comando(m, "ls")
                verificar("leia-me.txt" in out, "[usb] terminal funciona com boot pelo pendrive")
                continue

            out = comando(m, "ajuda")
            for c in ("ajuda", "ls", "cd", "cat", "limpar", "mem", "data", "reiniciar"):
                verificar(re.search(r"^\s+" + c, out, re.M), f"ajuda lista o comando {c}")
            m.captura("H09-ajuda")

            comando(m, "limpar")
            out = comando(m, "ls")
            for nome in ("arte", "bin", "historia", "leia-me.txt", "sobre.txt", "tmp"):
                verificar(nome in out, f"ls mostra {nome}")
            verificar("<pasta>" in out and "bytes" in out, "ls diferencia pastas e arquivos")

            comando(m, "cd historia")
            verificar("sagui:/historia$" in "\n".join(m.tela_texto()), "cd historia muda o prompt")
            out = comando(m, "ls")
            verificar("linux.txt" in out and "saguios.txt" in out, "ls dentro de historia/")
            m.captura("H09-ls-cd")

            out = comando(m, "cat linux.txt", paginas=True)
            original = open(os.path.join(RAIZ, "rootfs", "historia", "linux.txt"), encoding="utf-8").read()
            linhas = [l for l in original.splitlines() if l.strip()]
            faltando = [l for l in linhas if l not in out]
            verificar(not faltando, f"cat mostra todas as {len(linhas)} linhas do linux.txt (com paginação)")
            verificar("-- mais" in out, "cat pagina textos maiores que a tela")

            out = comando(m, "cd ../arte")
            out = comando(m, "cat sagui.txt")
            verificar("S A G U I O S" in out, "cd ../arte e cat sagui.txt (caminho relativo com ..)")
            m.captura("H09-cat-sagui")

            out = comando(m, "cd /nao-existe")
            verificar("não existe" in out, "cd para pasta inexistente avisa")
            out = comando(m, "cat /bin")
            verificar("é uma pasta" in out, "cat numa pasta avisa")
            out = comando(m, "xyz")
            verificar("Comando desconhecido" in out, "comando desconhecido avisa")
            comando(m, "cd /")

            comando(m, "limpar")
            tela = [l for l in m.tela_texto() if l.strip()]
            verificar(len(tela) == 1 and tela[0].startswith("sagui:/$"), "limpar deixa só o prompt na tela")

            out = comando(m, "mem")
            total = int(re.search(r"total \(mapa E820\):\s+(\d+) MB", out).group(1))
            livre = int(re.search(r"Livre:\s+(\d+) MB", out).group(1))
            verificar(500 <= total <= 512 and 400 < livre <= total, f"mem: total {total} MB, livre {livre} MB")

            out = comando(m, "data")
            hoje = datetime.datetime.now(datetime.timezone.utc)
            verificar(hoje.strftime("%d/%m/%Y") in out, f"data mostra a data de hoje ({hoje:%d/%m/%Y}, relógio UTC do QEMU)")
            dia = ["segunda-feira", "terça-feira", "quarta-feira", "quinta-feira", "sexta-feira", "sábado", "domingo"][hoje.weekday()]
            verificar(dia in out, f"data mostra o dia da semana ({dia})")
            out = comando(m, "tempo")
            verificar("Ligado h" in out, "tempo mostra quanto tempo o sistema está ligado")
            m.captura("H09-mem-data")

            # Acentos digitados no terminal (teclas mortas) chegam ao comando.
            out = comando(m, "cat ação")
            verificar("ação" in out, "acentos digitados chegam ao terminal (cat ação)")

            m.digitar("reiniciar\n", 0.05)
            m.esperar_serial("[SAGUI] reiniciando", 30)
            try:
                m.proc.wait(timeout=30)
                reiniciou = True
            except Exception:
                reiniciou = False
            # Com -no-reboot, o QEMU termina quando a máquina reinicia.
            verificar(reiniciou, "reiniciar reinicia a máquina (QEMU encerrou no reset)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
