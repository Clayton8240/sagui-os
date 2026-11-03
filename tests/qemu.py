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

# Como digitar cada caractere num teclado ABNT2 pelo QEMU. O QEMU nomeia as
# teclas pela posição do teclado americano; o kernel aplica o mapa ABNT2. Por
# isso o "/" é a tecla "ro" (0x73), o ";" é a tecla "slash" etc.
TECLAS = {" ": "spc", "\n": "ret", "\t": "tab", "-": "minus", "=": "equal", ".": "dot",
          ",": "comma", "/": "ro", "?": "shift-ro", ";": "slash", ":": "shift-slash",
          "'": "grave_accent", '"': "shift-grave_accent", "[": "bracket_right", "{": "shift-bracket_right",
          "]": "backslash", "}": "shift-backslash", "\\": "less", "|": "shift-less",
          "_": "shift-minus", "+": "shift-equal", ">": "shift-dot", "<": "shift-comma",
          "!": "shift-1", "@": "shift-2", "#": "shift-3", "$": "shift-4", "%": "shift-5",
          "&": "shift-7", "*": "shift-8", "(": "shift-9", ")": "shift-0"}


def cor_do_pixel(png, x, y):
    """Lê a cor de um pixel de uma captura (via ImageMagick): (r, g, b)."""
    saida = subprocess.run(["convert", png, "-crop", f"1x1+{x}+{y}", "-depth", "8", "txt:-"],
                           capture_output=True, text=True, check=True).stdout
    m = re.search(r"\((\d+),(\d+),(\d+)", saida.splitlines()[-1])
    return tuple(int(v) for v in m.groups())


def tamanho_png(png):
    saida = subprocess.run(["identify", "-format", "%w %h", png], capture_output=True, text=True, check=True).stdout
    l, a = saida.split()
    return int(l), int(a)


def imagem_de_teste(teste, extras=()):
    """Monta build/testes/teste-<nome>.img: a imagem normal com um autoteste
    do kernel ligado pelo cabeçalho de boot (campo CAB_TESTE)."""
    b = os.path.join(RAIZ, "build")
    os.makedirs(SAIDA, exist_ok=True)
    seguro = re.sub(r"[^A-Za-z0-9_.-]+", "_", teste).strip("_")[:40]
    destino = os.path.join(SAIDA, f"teste-{seguro}.img")
    subprocess.run(["python3", os.path.join(RAIZ, "tools", "mkimagem.py"),
                    "--mbr", os.path.join(b, "boot", "mbr.bin"), "--estagio2", os.path.join(b, "boot", "estagio2.bin"),
                    "--kernel", os.path.join(b, "kernel.bin"), "--kernel-elf", os.path.join(b, "kernel.elf"),
                    "--ramdisk", os.path.join(b, "ramdisk.sgfs"), "--saida", destino, "--teste", teste, *extras],
                   check=True, stdout=subprocess.DEVNULL)
    return destino


# Como digitar letras acentuadas num teclado ABNT2 pelo QEMU (que nomeia as
# teclas pela posição americana): tecla morta + letra, ou a tecla do ç.
ACENTOS_ABNT2 = {"ç": ["semicolon"], "Ç": ["shift-semicolon"]}
for _letra, _mortas in (("aeiou", [("´", "bracket_left"), ("`", "shift-bracket_left"),
                                   ("^", "shift-apostrophe")]), ("ao", [("~", "apostrophe")])):
    for _acento, _tecla in _mortas:
        for _l in _letra:
            _comb = {"´": "\u0301", "`": "\u0300", "^": "\u0302", "~": "\u0303"}[_acento]
            import unicodedata as _ud
            ACENTOS_ABNT2[_ud.normalize("NFC", _l + _comb)] = [_tecla, _l]
            ACENTOS_ABNT2[_ud.normalize("NFC", _l.upper() + _comb)] = [_tecla, "shift-" + _l]


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

    # ------------------------------------------------------------ shell
    def comando(self, texto, tempo=60, teclas_finais=("ret",)):
        """Digita um comando no sagsh, aperta Enter e espera o próximo prompt.
        Devolve o que saiu na serial durante o comando (sem '\r')."""
        antes = self.serial()
        n = antes.count("sagui:")
        self.digitar(texto, 0.04)
        for t in teclas_finais:
            self.tecla(t, 0.04)
        fim = time.time() + tempo
        while self.serial().count("sagui:") <= n:
            if time.time() > fim:
                raise FalhaTeste(f"o comando {texto!r} não voltou ao prompt")
            if self.proc.poll() is not None:
                break
            time.sleep(0.2)
        return self.serial()[len(antes):].replace("\r", "")

    # ------------------------------------------------------------ teclado
    def tecla(self, nome, espera=0.08):
        self.monitor(f"sendkey {nome}")
        time.sleep(espera)

    def digitar(self, texto, espera=0.08):
        for c in texto:
            if c in ACENTOS_ABNT2:
                for t in ACENTOS_ABNT2[c]:
                    self.tecla(t, espera)
            elif c in TECLAS:
                self.tecla(TECLAS[c], espera)
            elif c.isupper():
                self.tecla("shift-" + c.lower(), espera)
            else:
                self.tecla(c, espera)


def verificar(condicao, mensagem):
    if not condicao:
        raise FalhaTeste(mensagem)
    print("  ok:", mensagem)
