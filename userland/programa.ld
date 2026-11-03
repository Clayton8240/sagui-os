/* programa.ld - como os programas do SaguiOS são ligados.
 * Endereço 0x40000000 (PRD 4); dados numa página separada do código. */
ENTRY(_inicio)
SECTIONS
{
    . = 0x40000000;
    .text : { *(.text.inicio) *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    . = ALIGN(4096);
    .data : { *(.data .data.*) }
    .bss : { *(.bss .bss.*) *(COMMON) }
    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
