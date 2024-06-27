GCC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCPY = aarch64-linux-gnu-objcopy

install: entry kernel console
	$(LD) -T linker.ld -o kernel.elf entry.o kernel.o console.o -Map=mapfile.map
	$(OBJCPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-aarch64 kernel.elf kernel.efi
	python replace.py	

entry: 
	$(GCC) -c -o entry.o entry.S

kernel:
	$(GCC) -c -o kernel.o kernel.c

console:
	$(GCC) -c -o console.o console_driver/console.c 

convert:
	$(OBJCPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-aarch64 kernel.elf kernel.efi
magic:
	python replace.py	
offset:
	$(OBJCPY) --change-start=0x80000000 kernel.elf

installoff: offset convert magic
	echo "done with offset"

clean:
	rm *.o
	rm *.elf
	rm *.efi
