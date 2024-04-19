SECTIONS
{
    . = 0x60000;
    __start__ = .;
    . = ALIGN(8);
    .text : { *(.text*) }
    
    .data : {
        . = ALIGN(8);
        *(.data)
        *(.data*)
     }
    .bss : {
        . = ALIGN(8);
        _bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    }
    . = ALIGN(8);
    __image_end = .;
    __simple_malloc_start = .;

    __simple_malloc_end_no_paging = . +0x18000;
    _end = .;
}
_bss_size = (_bss_end - _bss_start)>>3;      /* 8 byte */
_whole_size = (_end- __start__) >>3;