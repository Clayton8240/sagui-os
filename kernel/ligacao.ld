/* ligacao.ld - mapa de memória do kernel do SaguiOS.
 *
 * O estágio 2 copia o kernel.bin (binário plano) para 1 MB. Por isso tudo é
 * ligado a partir de 0x100000 e a seção .entrada vem primeiro. Os símbolos
 * _kernel_inicio e _kernel_fim são lidos pelo tools/mkimagem.py para decidir
 * onde o ramdisk fica (depois da BSS, que não existe no arquivo). */
ENTRY(_inicio)

SECTIONS
{
    . = 0x100000;
    _kernel_inicio = .;

    .text : { *(.entrada) *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    .bss : {
        _bss_inicio = .;
        *(.bss .bss.*) *(COMMON)
        _bss_fim = .;
    }
    _kernel_fim = .;

    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
