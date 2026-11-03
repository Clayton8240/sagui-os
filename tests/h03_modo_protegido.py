#!/usr/bin/env python3
"""H03 - estágio 2 e entrada no modo protegido.

Critério: o kernel em C escreve na tela, em modo texto VGA,
`Ola! Eu sou o SaguiOS.` e a quantidade de memória detectada.
Roda nos dois modos de boot e com duas quantidades de RAM, para provar que a
memória vem mesmo do mapa E820 e não de um número fixo.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from qemu import Maquina, verificar  # noqa: E402


def main():
    for modo in ("ide", "usb"):
        for mem_mb in (512, 128):
            with Maquina("h03", modo, memoria=f"{mem_mb}M") as m:
                m.esperar_serial("[SAGUI] boot ok", 120)
                tela = m.tela_texto()
                texto = "\n".join(tela)
                verificar("Ola! Eu sou o SaguiOS." in texto, f"[{modo} {mem_mb}M] saudação na tela VGA")
                achou = re.search(r"Memoria detectada: (\d+) MB", texto)
                verificar(achou is not None, f"[{modo} {mem_mb}M] memória detectada na tela")
                mb = int(achou.group(1))
                # O QEMU reserva um pouco (BIOS, VGA, ACPI): aceitamos até 2 MB a menos.
                verificar(mem_mb - 2 <= mb <= mem_mb, f"[{modo} {mem_mb}M] {mb} MB livres, coerente com {mem_mb} MB")
                verificar("Ramdisk SGFS" in texto, f"[{modo} {mem_mb}M] ramdisk encontrado depois do kernel")
                verificar("Fase 1 concluida" in texto, f"[{modo} {mem_mb}M] kernel chegou ao fim da inicialização")
                if mem_mb == 512:
                    m.captura(f"H03-kernel-{modo}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
