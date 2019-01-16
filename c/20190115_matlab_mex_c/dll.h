/*
gcc dll.c -shared -o libdll.dll -Wl,--out-implib,libdll.lib
*/

int add(int a, int b);
