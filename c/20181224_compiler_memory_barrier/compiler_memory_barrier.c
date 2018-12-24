int a;
int b;

int main() {
    a = 1;
    a = 2;
    b = a + 1;
    asm volatile("" ::: "memory");
    return b;
}

/*
chuqq@cqq-ubu-dilu:~/temp$ objdump -S test.o 

test.o：     文件格式 elf64-x86-64


Disassembly of section .text.startup:

0000000000000000 <main>:
   0:   c7 05 00 00 00 00 02    movl   $0x2,0x0(%rip)        # a <main+0xa>
   7:   00 00 00 
   a:   c7 05 00 00 00 00 03    movl   $0x3,0x0(%rip)        # 14 <main+0x14>
  11:   00 00 00 
  14:   b8 03 00 00 00          mov    $0x3,%eax
  19:   c3                      retq   
chuqq@cqq-ubu-dilu:~/temp$ vi test.c 
chuqq@cqq-ubu-dilu:~/temp$ 
chuqq@cqq-ubu-dilu:~/temp$ 
chuqq@cqq-ubu-dilu:~/temp$ gcc -c test.c -O3
chuqq@cqq-ubu-dilu:~/temp$ objdump -S test.o 

test.o：     文件格式 elf64-x86-64


Disassembly of section .text.startup:

0000000000000000 <main>:
   0:   c7 05 00 00 00 00 02    movl   $0x2,0x0(%rip)        # a <main+0xa>
   7:   00 00 00 
   a:   c7 05 00 00 00 00 03    movl   $0x3,0x0(%rip)        # 14 <main+0x14>
  11:   00 00 00 
  14:   8b 05 00 00 00 00       mov    0x0(%rip),%eax        # 1a <main+0x1a>
  1a:   c3                      retq   
chuqq@cqq-ubu-dilu:~/temp$

*/
