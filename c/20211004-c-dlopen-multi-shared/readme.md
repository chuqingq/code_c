gcc mylib1.c -fPIC -shared -o libmy1.so
gcc mylib2.c -fPIC -shared -o libmy2.so
gcc main.c -o main -l dl -g

./main
11111111
22222222

