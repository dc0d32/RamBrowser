OUTPUT_FORMAT("elf32-i386");
OUTPUT_ARCH(i386);
ENTRY(asmstart);
SEARCH_DIR("");


KERNEL_PHYS_ADDR = 0x00100000;
KERNEL_VIRT_ADDR = 0x00100000;


SECTIONS
{
	.text KERNEL_VIRT_ADDR : AT(KERNEL_PHYS_ADDR)
	{
		. = ALIGN(4096);

		_text_section = .;

		_text = .;
		*(.text);
		*(.text.*);
		*(.gnu.linkonce.t.*);
		_text_end = .;

		. = ALIGN(4096);
		_text_section_end = .;
 }


 .data : AT(_text_section_end - KERNEL_VIRT_ADDR + KERNEL_PHYS_ADDR)
 {
	 _data_section = .;

	 _data = .;
	 *(.data);
	 _data_end = .; 

	 _rodata = .;
	 *(.rodata);
	 *(.rodata.*);
	 *(.got);
	 *(.symtab);
	 *(.strtab);
	 *(.shstrtab);
	 *(.gnu.linkonce.r.*);
	 *(.eh_frame);
	 _rodata_end = .;

	 . = ALIGN(4096);
	 _data_section_end = .;
 }


 .bss : AT(_data_section_end - KERNEL_VIRT_ADDR + KERNEL_PHYS_ADDR)
 {

	 _bss_section = .;

	 _bss = .;
	 *(.bss);
	 *(COMMON);
	 . = ALIGN(4096);
	 _bss_end = .;

	 . = ALIGN(4096);
	 _bss_section_end = .;
 }


 /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.comment) }
}
