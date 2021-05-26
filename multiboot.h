/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

/*

Standard multiboot spec. compliant structures, to be used with
GRUB for now

*/



#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#ifdef KERNEL



/*some common definitions*/

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the Multiboot header.  */
# define MULTIBOOT_HEADER_FLAGS		0x00000003

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002






/*these macros must only be available in the assembly stub*/
#ifdef _STUB_S_
/*only asm specific portion*/


#else
/*multiboot standard structures, C portion*/

/* Types.  */
#include<types.h>



/* The Multiboot header.  */
typedef struct multiboot_header
{
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
}multiboot_header_t;


/* The section header table for ELF kernel.  */
typedef struct TAGelf_section_header_table_t
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct TAGmultiboot_info_t
{
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  elf_section_header_table_t elf_sec;
  uint32_t mmap_length;
  uint32_t mmap_addr;
} multiboot_info_t;



/* The module structure.  */
typedef struct TAGmultiboot_module_t
{
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
} multiboot_module_t;



typedef struct TAGmemory_map_t
{
  uint32_t size;
  uint32_t base_addr_low;
  uint32_t base_addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t type;
} memory_map_t;


/*our memory map structure*/
typedef struct TAGmem_map_t{
	uint32_t	base_addr;
	uint32_t	length;
	uint32_t	type;	//this should rather be enum of memory types
} __PACKED mem_map_t;


#endif /* ! _STUB_S_*/

#endif /* ! KERNEL */

#endif /* ! _MULTIBOOT_H_*/

