CC = gcc
LD = ld

#-Wextra for extra warnings
ASMFLAGS = -Wall -g0 -Wextra -Werror -I. -c -O2 -fno-exceptions -m32 -march=i686 -nostartfiles -nostdlib -ffreestanding -fno-leading-underscore -fomit-frame-pointer -DKERNEL
#the only difference in these is -save-temps
CFLAGS   = $(ASMFLAGS)

LDFLAGS = -nostdlib -verbose --print-map -Map kernel.map --no-omagic -T linker.ld --oformat elf32-i386

# yuk. Need a better way to enumerate source files.
# Meh. small project. Who cares?
KERNELSOURCES_C = $(shell ls *.c | grep .)
KERNELSOURCES_ASM = $(shell ls *.S | grep .)

KERNEL = rambo

OBJS = $(addsuffix .o,$(basename $(KERNELSOURCES_ASM))) $(addsuffix .o,$(basename $(KERNELSOURCES_C))) 

$(KERNEL):$(OBJS)
	$(LD) $(LDFLAGS)  -o $@ $^ > verbose.txt
	objdump -xSd $@ >> kernel.map
	readelf -a $@ > elfdump.txt
# skip the gzip stage while we debug
#	gzip -9 -f $@
#	mv $@.gz $@


%.o:%.S
	$(CC) $(ASMFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $^

#this should not delete the source .s files!
clean:
	-rm -f $(OBJS) $(KERNEL) $(addsuffix .i,$(basename $(KERNELSOURCES_C))) $(addsuffix .s,$(basename $(KERNELSOURCES_C))) 
	-rm -fR *.c~ *.h~

image: $(KERNEL)
	sudo mount floppy.img /mnt/tmp -o loop
	sudo cp -v $(KERNEL) /mnt/tmp/boot
	sudo cp -v kernel.map /mnt/tmp/boot
	sudo umount /mnt/tmp

