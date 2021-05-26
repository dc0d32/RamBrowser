#!/bin/sh

qemu-system-x86_64 -m 64M -fda floppy.img -gdb tcp::5022

