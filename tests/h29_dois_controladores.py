#!/usr/bin/env python3
"""H29-fix - dois controladores EHCI ao mesmo tempo (como no G470).

O G470 tem dois EHCI (00:1a.0 e 00:1d.0), cada um com um hub interno no
endereço 1. Quando os dois compartilhavam o mesmo anel de transferências,
cada pergunta a um dispositivo era feita também pelo outro controlador, no
outro barramento, e a enumeração falhava no meio (erro 19).
Critério: com um pendrive em cada controlador, os dois são enumerados e
lidos; e um pendrive conectado a quente no segundo controlador aparece.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from qemu import Maquina, verificar, imagem_console, criar_pendrive  # noqa: E402


def main():
    a = criar_pendrive("h29d-a", rotulo="PRIMEIRO")
    b = criar_pendrive("h29d-b", rotulo="SEGUNDO")
    c = criar_pendrive("h29d-c", rotulo="QUENTE")
    extras = ["-device", "usb-ehci,id=ehci",
              "-device", "usb-ehci,id=ehci2",
              "-drive", f"if=none,id=pa,format=raw,file={a}",
              "-device", "usb-storage,bus=ehci.0,drive=pa,removable=on",
              "-drive", f"if=none,id=pb,format=raw,file={b}",
              "-device", "usb-storage,bus=ehci2.0,drive=pb,removable=on"]
    with Maquina("h29d", "ide", imagem=imagem_console(), extras=extras) as m:
        m.esperar_serial("sagui:/$", 120)
        out = m.comando("usb")
        verificar(out.count("reconhecido") >= 2, "usb: os dois controladores enumeram o seu pendrive")
        out = m.comando("discos")
        verificar("PRIMEIRO" in out and "SEGUNDO" in out, "discos: os dois pendrives são lidos")
        m.monitor(f"drive_add 0 if=none,id=pc,format=raw,file={c}")
        m.monitor("device_add usb-storage,bus=ehci2.0,drive=pc,id=pen-c,removable=on")
        m.esperar_serial('rotulo "QUENTE"', 15)
        verificar(True, "conexão a quente no segundo controlador, com o primeiro ocupado")
        m.captura("H29-dois-controladores")
    return 0


if __name__ == "__main__":
    sys.exit(main())
