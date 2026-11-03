/* energia.c - reiniciar a máquina. */
#include "cpu.h"
#include "console.h"
#include "portas.h"
#include "relogio.h"

SEM_RETORNO void maquina_reiniciar(void)
{
    klog("[SAGUI] reiniciando\n");
    int_desliga();
    /* O controlador de teclado tem uma linha ligada ao reset da CPU: o
     * comando 0xFE dá um pulso nela. */
    for (int i = 0; i < 100000 && (porta_le8(0x64) & 2); i++) ;
    porta_escreve8(0x64, 0xFE);
    for (volatile int i = 0; i < 1000000; i++) ;
    /* Se não funcionou: IDT vazia + interrupção = falta tripla, que também reinicia. */
    struct EMPACOTADO { uint16_t limite; uint32_t base; } vazia = { 0, 0 };
    __asm__ volatile("lidt %0; int3" : : "m"(vazia));
    parar_maquina();
}
