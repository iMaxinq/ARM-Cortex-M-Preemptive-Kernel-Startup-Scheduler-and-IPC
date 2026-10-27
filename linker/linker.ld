/* The > RAM and > FLASH do not allocate or copy any data to the memories, they are only used internally to get the bookmarks (_estack, _etext etc)
The linker.ld basically just creates the bookmarks that are the boundaries of each memory area. These are then burnt into the .elf file */

/* Tell the debugger/ELF header that execution starts at Reset_Handler */
ENTRY(Reset_Handler)

/* physical memory regions for QEMU mps2-an385 */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 4M
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 4M
}

/* The Cortex-M stack grows downward (from high address to low address).
Set the initial Main Stack Pointer (_estack) to the very top of RAM (0x20400000). */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* CODE AND CONSTANTS (in FLASH) */
    .text :
    {
        . = ALIGN(4);

        /* vector table MUST be at 0x00000000 */
        KEEP(*(.isr_vector))
        
        /* All functions (.text) and read-only constants (.rodata) from all .o files */
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* _sidata stores the Flash address (LMA) where the initial values are kept */
    _sidata = LOADADDR(.data);

    /* GLOBALS AND INITIALIZED VARIABLES */
    .data :
    {
        . = ALIGN(4);

        _sdata = .;        /* RAM start address of .data */
        *(.data)
        *(.data*)
        . = ALIGN(4);

        _edata = .;        /* RAM end address of .data */
    } > RAM AT> FLASH      /* "> RAM" = VMA (runtime address), "AT> FLASH" = LMA (storage address) */

    /* UNINITIALIZED GLOBAL/STATIC VARIABLES (in RAM)*/
    .bss :
    {
        . = ALIGN(4);

        _sbss = .;         /* RAM start address of .bss */
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* RAM end address of .bss */
    } > RAM
}