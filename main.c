/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#include <types.h>
#include <string.h>
#include <multiboot.h>
#include <asm.h>
#include <kb.h>
#include <screen.h>

/*the multiboot information structure's address passed by GRUB is stored here in stub code*/
multiboot_info_t *multiboot_info_ptr = 0;
/*the magic number passed by the loader, ensuring that loading was successful*/
uint32_t multiboot_bootloader_magic = 0;

int8_t *kernel_cmdline; //saved cmdline
multiboot_module_t secret_file_module;
mem_map_t mem_map[16]; 
size_t   mem_map_size;

static uint32_t	_init_kernel_stack[128 * 1024];	// should be enough
uint32_t	init_kernel_stack = (uint32_t)&_init_kernel_stack[sizeof(_init_kernel_stack)];

static int8_t	search_buff[33];
static uint16_t	query_len = 0;

static int8_t	address_buff[17];

/* Check if the bit in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))


int	get_memory_area(uint32_t	addr){
	unsigned int	i = 0;
	for(i = 0; i < mem_map_size; i++){
		if(mem_map[i].base_addr <= addr && addr < mem_map[i].base_addr + mem_map[i].length)
			return i;
	}
	return -1;
}


void print_mem_map(){
	size_t	i;
	printf("Memory Map:\n");
	for(i = 0; i < mem_map_size; i++){
		printf("[Type=%d  0x%x-0x%x (%d KBytes)] ", mem_map[i].type, mem_map[i].base_addr, mem_map[i].base_addr + mem_map[i].length, mem_map[i].length / 1024);
	}
	putchar('\n');
}

int main()
{

	//GDT and IDT already initialized; this is a very limited functionality kernel

	cls();

	printf("RAM_BROWSER : built using GCC GNU %d, ", __GNUC__);
	printf("on %s at %s\n", __DATE__, __TIME__);


	if(multiboot_bootloader_magic != MULTIBOOT_BOOTLOADER_MAGIC)
		printf("WARNING : bootloader magic dword invalid. [got %x instead of %x]\n", multiboot_bootloader_magic, MULTIBOOT_BOOTLOADER_MAGIC);

	/* Are mem_* valid?  */
	if (CHECK_FLAG (multiboot_info_ptr->flags, 0))
		printf ("memory: Conventional = %uKB, Extended = %uMB \n",
				(uint32_t) (multiboot_info_ptr->mem_lower), (uint32_t)(multiboot_info_ptr->mem_upper)/1024);

	/* Are mmap_* valid? */
	if (CHECK_FLAG (multiboot_info_ptr->flags, 6))
	{
		uint32_t i = 0;
		memory_map_t *mmap;
		for (mmap = (memory_map_t *) multiboot_info_ptr->mmap_addr;
				(uint32_t) mmap < multiboot_info_ptr->mmap_addr + multiboot_info_ptr->mmap_length &&( i < ((sizeof(mem_map)/sizeof(mem_map[0]))));
				mmap = (memory_map_t *) ((uint32_t) mmap
					+ mmap->size + sizeof (mmap->size)), i++){

			*(uint32_t *)&(mem_map[i].base_addr) = mmap->base_addr_low;
			*(uint32_t *)&(mem_map[i].length) = mmap->length_low;
			*(uint32_t *)&(mem_map[i].type) = mmap->type;

		}

		//record the size
		*(size_t*)&mem_map_size = i;

		print_mem_map();
	}
	else{
		printf("PANIC : invalid Memory Map [as given by bootloader].\n");
		return 0;
	}

	/* is the module loaded? */
	if (CHECK_FLAG (multiboot_info_ptr->flags, 3))
	{
		memcpy((void *)&secret_file_module, (void *)multiboot_info_ptr->mods_addr, sizeof(multiboot_module_t));
		printf("file provided.\n");
	}
	else{
		printf("file NOT provided.\n");
	}

	/* Bits 4 and 5 are mutually exclusive!  */
	if (CHECK_FLAG (multiboot_info_ptr->flags, 4) && CHECK_FLAG (multiboot_info_ptr->flags, 5))
	{
		printf ("PANIC : Both bits 4 and 5 are set.\n");
		/*TODO panic here :P */
		return 0;
	}

	/* was command line passed?  */
	if (CHECK_FLAG (multiboot_info_ptr->flags, 2)){
		kernel_cmdline = (int8_t *)(multiboot_info_ptr->cmdline);
		printf ("Command Line: %s\n", kernel_cmdline);
	}
	else{
		printf("\n Command line not provided; weird!");
		return 1;
	}

#if 1

	int8_t	*p = kernel_cmdline;
	while(*p++ != ' ');
	if(*p == 'r'){	// read to memory
		p += 2;
		uint32_t	read_addr = hatoi(p);
		uint32_t	fileSize = secret_file_module.mod_end - secret_file_module.mod_start;
		memcpy((void*)read_addr, (void*)secret_file_module.mod_start, fileSize);
		printf("file copied to memory at address %x. Reset, or power off and quickly turn it back on and try dumping values at this address.\n press any key to cold reboot instead.\n", read_addr);
		kb_get_cooked();
		kb_reboot();
	}
	else if(*p == 'b'){	// browse memory
		uint32_t	read_addr = 0x00100000;
		printf("will try to read contents of memory from %x, pageful at a time. \n Press 'z' key to go to next page, 'a' to go to previous page, 's' to go one byte less, 'x' for next byte, '/' and '?' to search, 'n' and 'N' to search again, 'G' to go to a specific address, 'R' to reset the view to 1M, 'Q' to reboot.\n* This prompt will not appear again. *\n", read_addr);
		kb_get_cooked();
		cls();
		uint8_t	*v = (uint8_t*)VIDEO;
		uint8_t	*p = (uint8_t*)read_addr;
		while(true){
			uint32_t	numBytes = COLUMNS * LINES;
			uint32_t	i = 0;
			for(i = 0; i < numBytes; i++){
				*(v + (i * 2) + 0) = *(p + i);
				*(v + (i * 2) + 1) = ATTRIBUTE;
			}
			uint8_t	c = kb_get_cooked();
			bool	found = false;
			switch(c){
				case 'a' :
					p -= numBytes;
					break;
				case 'z' :
					p += numBytes;
					break;
				case '/' :
					cls();
					printf("highly crippled (bruteforce, max 32) search\n => ");
					query_len = kb_get_string(search_buff, sizeof(search_buff));
					/* fall through */
				case 'n' :
					if(!query_len)
						break;
					if(c == 'n') cls();
					printf("searching ...");
					{
						int	ma = get_memory_area((uint32_t)p);
						uint32_t m = (uint32_t)p;
						if(c == 'n') m++;
#if 1
						uint8_t	*hit = memfind((void*)m, mem_map[ma].length - (m - mem_map[ma].base_addr), search_buff, query_len);
						if(hit != NULL){
							p = hit;
							found = true;
							break;
						}
#else						
						while(m < (mem_map[ma].base_addr + mem_map[ma].length - query_len)){
							if(m % (4*1024*1024) == 0) printf("\r%dM    ", m / (1024 * 1024));
							if(memcmp((int8_t*)m, search_buff, query_len) == 0){
								p = (uint8_t*) m;
								found = true;
								break;
							}
							if(!~m)
								break;
							m++;
						}
#endif
						putchar('\n');
					}
					if(!found){
						printf("query '%s' not found.\n", search_buff);
						kb_get_cooked();
					}
					break;

				case '?' :
					cls();
					printf("highly crippled (bruteforce, no backspace, max 32) rev search\n => ");
					query_len = kb_get_string(search_buff, sizeof(search_buff));
					/* fall through */
				case 'N' :
					if(!query_len)
						break;
					if(c == 'N') cls();
					printf("reverse searching ...");
					{
						int	ma = get_memory_area((uint32_t)p);
						uint32_t m = (uint32_t)p;
#if 1
						uint8_t	*hit = memfind_r((void*)mem_map[ma].base_addr, m - mem_map[ma].base_addr, search_buff, query_len);
						if(hit != NULL){
							p = hit;
							found = true;
							break;
						}
#else						
						do{
							if(m % (4*1024*1024) == 0) printf("\r%dM    ", m / (1024 * 1024));
							if(memcmp((int8_t*)m, search_buff, query_len) == 0){
								p = (uint8_t*) m;
								found = true;
								break;
							}
							if(m == mem_map[ma].base_addr || !m)
								break;
							--m;
						}while(true);
#endif
						putchar('\n');
					}
					if(!found){
						printf("query '%s' not found.\n", search_buff);
						kb_get_cooked();
					}
					break;
				case 'G':
					cls();
					uint32_t	addr = 0;
					while(true){
						print_mem_map();
						printf("address (hex) : ");
						kb_get_string(address_buff, sizeof(address_buff));
						addr = hatoi(address_buff);
						if(get_memory_area(addr) < 0){
							printf("Invalid address!\n");
							continue;
						}
						break;
					}
					p = (uint8_t*)addr;
					break;
				case 'R' :
					p = (uint8_t*)0x00100000;
					break;
				case 'Q' :
					kb_reboot();
					break;
				case 's' : ++p; break;
				case 'x' : --p; break;
			}
		}
	}
	else{
		printf("command not understood :P Press any key to reboot\n");
		kb_reboot();
	}

#endif

	return(0);
}


