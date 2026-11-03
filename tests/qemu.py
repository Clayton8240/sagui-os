"""
qemu.py - controle do QEMU para os testes automáticos do SaguiOS.

Cada teste cria uma Maquina, que:
  - liga o QEMU sem janela (-display none), sem KVM;
  - manda a serial COM1 para um arquivo (o log do kernel);
  - abre o monitor do QEMU num socket Unix, usado para enviar teclas, mover o
    mouse, ler a memória e tirar capturas de tela (screendump -> PNG).

Dois modos de boot (PRD 3.3):
  "ide" - a imagem como disco IDE;
  "usb" - a imagem como pendrive num controlador EHCI (simula o G470).

Tempo máximo de qualquer espera: 3 minutos (PRD seção 7).
"""
import os
import re
import socket
import subprocess
import time

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMAGEM = os.path.join(RAIZ, "build", "saguios.img")
SAIDA = os.path.join(RAIZ, "build", "testes")
CAPTURAS = os.path.join(RAIZ, "docs", "capturas")
TEMPO_MAX = 180

# Nomes de tecla do QEMU para cada caractere digitável (teclado americano do
# QEMU; o mapa ABNT2 do kernel traduz os scan codes, ver tecla_abnt2).
TECLAS = {" ": "spc", "\n": "ret", "\t": "tab", "-": "minus", "=": "equal", ".": "dot",
          ",": "comma", "/": "slash", ";": "semicolon", "'": "apostrophe", "\\": "backslash",
          "[": "bracket_left", "]": "bracket_right", "`": "grave_accent",
          "_": "shift-minus", "+": "shift-equal", ">": "shift-dot", "<": "shift-comma",
          "?": "shift-slash", ":": "shift-semicolon", '"': "shift-apostrophe", "|": "shift-backslash",
          "!": "shift-1", "@": "shift-2", "#": "shift-3", "$": "shift-4", "%": "shift-5",
          "&": "shift-7", "*": "shift-8", "(": "shift-9", ")": "shift-0"}


class FalhaTeste(Exception):
    pass


class Maquina:
    def __init__(self, nome, modo="ide", imagem=IMAGEM, memoria="512M", extras=None, formato="raw"):
        self.nome = f"{nome}-{modo}"
        self.modo = modo
        self.imagem = imagem
        self.memoria = memoria
        self.extras = extras or []
        self.formato = formato
        os.makedirs(SAIDA, exist_ok=True)
        self.arq_serial = os.path.join(SAIDA, self.nome + ".serial")
        self.sock_monitor = os.path.join(SAIDA, self.nome + ".mon")
        self.proc = None
        self.mon = None

    # ------------------------------------------------------------ ciclo de vida
    def ligar(self):
        for f in (self.arq_serial, self.sock_monitor):
            if os.path.exists(f):
                os.remove(f)
        cmd = ["qemu-system-i386", "-m", self.memoria, "-display", "none",
               "-serial", "file:" + self.arq_serial,
               "-monitor", f"unix:{self.sock_monitor},server,nowait",
               "-no-reboot"]
        if self.modo == "ide":
            cmd += ["-drive", f"format={self.formato},file={self.imagem},if=ide,index=0,snapshot=on"]
        elif self.modo == "usb":
            cmd += ["-device", "usb-ehci,id=ehci",
                    "-drive", f"if=none,id=pendrive,format={self.formato},file={self.imagem},snapshot=on",
                    "-device", "usb-storage,bus=ehci.0,drive=pendrive,bootindex=0"]
        else:
            raise ValueError(self.modo)
        cmd += self.extras
        self.proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        for _ in range(100):
            if os.path.exists(self.sock_monitor):
                break
            time.sleep(0.05)
        self.mon = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.mon.connect(self.sock_monitor)
        self.mon.settimeout(10)
        self._ler_monitor()  # banner do monitor
        return self

    def desligar(self):
        try:
            if self.mon:
                self.mon.sendall(b"quit\n")
                self.mon.close()
        except OSError:
            pass
        if self.proc:
            try:
                self.proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.proc.kill()

    def __enter__(self):
        return self.ligar()

    def __exit__(self, *a):
        self.desligar()

    # ------------------------------------------------------------ monitor
    def _ler_monitor(self):
        dados = b""
        while not dados.endswith(b"(qemu) "):
            parte = self.mon.recv(65536)
            if not parte:
                break
            dados += parte
        return dados.decode("utf-8", "replace")

    def monitor(self, comando):
        self.mon.sendall(comando.encode() + b"\n")
        resp = self._ler_monitor()
        # Remove o eco do comando e o prompt; tira sequências de terminal.
        resp = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", resp)
        linhas = resp.replace("\r", "").split("\n")
        return "\n".join(l for l in linhas[1:] if not l.startswith("(qemu)"))

    # ------------------------------------------------------------ serial
    def serial(self):
        if not os.path.exists(self.arq_serial):
            return ""
        with open(self.arq_serial, "rb") as f:
            return f.read().decode("latin-1")

    def esperar_serial(self, texto, tempo=TEMPO_MAX):
        fim = time.time() + tempo
        while time.time() < fim:
            if texto in self.serial():
                return True
            if self.proc.poll() is not None:
                raise FalhaTeste(f"QEMU terminou antes de aparecer {texto!r}")
            time.sleep(0.2)
        raise FalhaTeste(f"tempo esgotado esperando {texto!r} na serial")

    # ------------------------------------------------------------ tela
    def tela_texto(self):
        """Lê as 25 linhas do modo texto VGA direto da memória (0xB8000)."""
        saida = self.monitor("xp /2000hx 0xb8000")
        celulas = []
        for linha in saida.splitlines():
            if ":" not in linha:
                continue
            for palavra in linha.split(":", 1)[1].split():
                celulas.append(int(palavra, 16))
        chars = "".join(chr(c & 0xFF) if 32 <= (c & 0xFF) < 127 else " " for c in celulas[:2000])
        return [chars[i:i + 80].rstrip() for i in range(0, len(chars), 80)]

    def esperar_tela(self, texto, tempo=60):
        fim = time.time() + tempo
        while time.time() < fim:
            if any(texto in l for l in self.tela_texto()):
                return True
            time.sleep(0.3)
        raise FalhaTeste(f"tempo esgotado esperando {texto!r} na tela")

    def captura(self, nome, guardar=True):
        """Tira uma captura de tela. guardar=True salva em docs/capturas/."""
        ppm = os.path.join(SAIDA, self.nome + "-" + nome + ".ppm")
        destino = os.path.join(CAPTURAS if guardar else SAIDA, nome + ".png")
        self.monitor(f"screendump {ppm}")
        for _ in range(50):
            if os.path.exists(ppm) and os.path.getsize(ppm) > 0:
                break
            time.sleep(0.1)
        time.sleep(0.2)
        subprocess.run(["convert", ppm, destino], check=True)
        os.remove(ppm)
        return destino

    # ------------------------------------------------------------ teclado
    def tecla(self, nome, espera=0.08):
        self.monitor(f"sendkey {nome}")
        time.sleep(espera)

    def digitar(self, texto, espera=0.08):
        for c in texto:
            if c in TECLAS:
                self.tecla(TECLAS[c], espera)
            elif c.isupper():
                self.tecla("shift-" + c.lower(), espera)
            else:
                self.tecla(c, espera)


def verificar(condicao, mensagem):
    if not condicao:
        raise FalhaTeste(mensagem)
    print("  ok:", mensagem)
