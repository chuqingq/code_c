asm(".long 0x1badb002, 0, (-(0x1badb002 + 0))");

unsigned char *videobuf = (unsigned char*)0xb8000;
const char *str = "Hello, World !! ";

int start_entry(void)
{
	int i;
	for (i = 0; str[i]; i++) {
		videobuf[i * 2 + 0] = str[i];
		videobuf[i * 2 + 1] = 0x17;
	}
	for (; i < 80 * 25; i++) {
		videobuf[i * 2 + 0] = ' ';
		videobuf[i * 2 + 1] = 0x17;
	}
	while (1) { }
	return 0;
}

/*
gcc -c -fno-builtin -ffreestanding -nostdlib -m32 miniboot.c -o miniboot.o

ld -e start_entry -m elf_i386 -Ttext-seg=0x100000 miniboot.o -o miniboot.elf

sudo pacman -S qemu-arch-extra

qemu-system-i386 -kernel miniboot.elf

*/
