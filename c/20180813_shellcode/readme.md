nasm shellcode3.s
gcc -o shellcode_test shellcode_test.c -z execstack
./shellcode_test "$(cat shellcode3)"
nc 127.0.0.1 4444

