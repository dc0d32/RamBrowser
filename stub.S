/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
*/

/*selects the assembly code portions from the following headers*/
#define _STUB_S_ 1

/*for multiboot header values*/
#include <multiboot.h>
#include <asm.h>


.section .text

.align	4	/*must be aligned to dword boundary*/
/* Multiboot header.  */
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

/*kernel entry point*/
.globl	asmstart
asmstart:
	cli

	/* the pointer to the Multiboot information structure.  */
	.extern multiboot_info_ptr
	movl	%ebx, multiboot_info_ptr

	/* the magic value. Ensures that boot was successful */
	.extern multiboot_bootloader_magic
	movl	%eax, multiboot_bootloader_magic

	/* setup stack */
	movl	init_kernel_stack, %esp

	/*
	Reset EFLAGS. this ensures that NT is not set
	and iret does a normal iret instead of a
	task backlink switch
	also clears direction flag, interrupts and IOPL
	*/
	pushl	$0
	popf


	/*
	disable NMI [play safe]
	this IS possible on the pc
	*/
	inb	$0x70, %al		/*the bit7 on cmos port 70h is*/
	orb	$0x80, %al		/*known to control the NMI*/
	outb	%al, $0x70		/*	--R Brown ports list*/


	/*move on to the C entry point*/
	pushl	$endless /*just in case the C kernel returns*/
	.extern main
	jmp	main	/*go, fly*/
endless:
        cli
        hlt
        jmp     endless

