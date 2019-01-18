ENTRY(_payload)

SECTIONS {
    /* Interrupt table is handled by boot loader */
    . = 0x8045D930;
    .text : {
        __start = .;
        *(.text)
        __text_end = .;
    }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
     }
     .sbss : {
         *(.sbss)
         *(.sbss.*)
      }
     __payload_end = .;
}