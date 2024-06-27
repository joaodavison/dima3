GCC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCPY = aarch64-linux-gnu-objcopy

install: entry kernel
	$(LD) -T linker.ld -o kernel.elf entry.o kernel.o
	$(OBJCPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-aarch64 kernel.elf kernel.efi
	python replace.py	

entry: 
	$(GCC) -c -o entry.o entry.S

kernel:
	$(GCC) -c -o kernel.o kernel.c

clean:
	rm *.o
	rm *.elf
	rm *.efi
