/* usb - diagnóstico: controladores USB 2.0, portas com dispositivo e
 * dispositivos reconhecidos (H29, H30). */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    static char texto[4096];
    int n = info_sistema(INFO_USB, texto, sizeof texto - 1);
    if (n < 0) { imprimir_em(ERROS, "usb: %s\n", nome_erro(n)); return 1; }
    texto[n] = 0;
    escrever_texto(SAIDA, texto);
    return 0;
}
