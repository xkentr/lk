OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)
SECTIONS
{
	. = %ROMBASE%;

	/* text/read-only data */
	.text.boot.vectab1 : { KEEP(*(.text.boot.vectab1)) }
	.text.boot.vectab2 : { KEEP(*(.text.boot.vectab2)) }
	.text.boot : { KEEP(*(.text.boot)) }
	.text :	{ *(.text .text.* .glue_7* .gnu.linkonce.t.* .eh_frame) }

	.interp : { *(.interp) }
	.hash : { *(.hash) }
	.dynsym : { *(.dynsym) }
	.dynstr : { *(.dynstr) }
	.rel.text : { *(.rel.text) *(.rel.gnu.linkonce.t*) }
	.rela.text : { *(.rela.text) *(.rela.gnu.linkonce.t*) }
	.rel.data : { *(.rel.data) *(.rel.gnu.linkonce.d*) }
	.rela.data : { *(.rela.data) *(.rela.gnu.linkonce.d*) }
	.rel.rodata : { *(.rel.rodata) *(.rel.gnu.linkonce.r*) }
	.rela.rodata : { *(.rela.rodata) *(.rela.gnu.linkonce.r*) }
	.rel.got : { *(.rel.got) }
	.rela.got : { *(.rela.got) }
	.rel.ctors : { *(.rel.ctors) }
	.rela.ctors : { *(.rela.ctors) }
	.rel.dtors : { *(.rel.dtors) }
	.rela.dtors : { *(.rela.dtors) }
	.rel.init : { *(.rel.init) }
	.rela.init : { *(.rela.init) }
	.rel.fini : { *(.rel.fini) }
	.rela.fini : { *(.rela.fini) }
	.rel.bss : { *(.rel.bss) }
	.rela.bss : { *(.rela.bss) }
	.rel.plt : { *(.rel.plt) }
	.rela.plt : { *(.rela.plt) }
	.init : { *(.init) }
	.plt : { *(.plt) }

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	__exidx_start = .;
	.ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
	__exidx_end = .;

	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)

		. = ALIGN(4);
	    	KEEP(*(.init))

	    . = ALIGN(4);
	    __preinit_array_start = .;
	    KEEP (*(.preinit_array))
	    __preinit_array_end = .;

	    . = ALIGN(4);
	    __init_array_start = .;
	    KEEP (*(SORT(.init_array.*)))
	    KEEP (*(.init_array))
	    __init_array_end = .;

	    . = ALIGN(4);
	    KEEP(*(.fini))

	    . = ALIGN(4);
	    __fini_array_start = .;
	    KEEP (*(.fini_array))
	    KEEP (*(SORT(.fini_array.*)))
	    __fini_array_end = .;

	    . = ALIGN(0x4);
	    KEEP (*crtbegin.o(.ctors))
	    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
	    KEEP (*(SORT(.ctors.*)))
	    KEEP (*crtend.o(.ctors))

	    . = ALIGN(0x4);
	    KEEP (*crtbegin.o(.dtors))
	    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
	    KEEP (*(SORT(.dtors.*)))
	    KEEP (*crtend.o(.dtors))

		__rodata_end = . ;
	}

	/* writable data  */
	__data_start_rom = .;
	. = %MEMBASE%;
	__data_start = .;
	.data : AT ( ADDR (.rodata) + SIZEOF (.rodata) ){
	    *(.got.plt) *(.got)
		*(.shdata)
		*(.data .data.* .gnu.linkonce.d.*)
		. = ALIGN(4);
		__commands_start = .;
		KEEP (*(.commands))
		__commands_end = .;
		. = ALIGN(4);
		__apps_start = .;
		KEEP (*(.apps))
		__apps_end = .;
	}

	__data_end = .;

	/* unintialized data (in same segment as writable data) */
	.bss.initial_stack : SUBALIGN(4) { KEEP(*(.bss.initial_stack)) }
	.bss : SUBALIGN(4) {
		__bss_start = .;
		*(.shbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	}
	. = ALIGN(4);
	__bss_end = .;

	_end = .;

	. = %MEMBASE% + %MEMSIZE%;
	_end_of_ram = .;

	/* Strip unnecessary stuff */
	/DISCARD/ : { *(.comment .note) }
}
