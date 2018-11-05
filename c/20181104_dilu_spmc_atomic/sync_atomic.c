// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
// 发展方向 __sync_xxx（老gcc） -> __atomic_xxx（新gcc，带内存模型） -> atomic_xxx（C11标准）

#include <stdio.h>

int main() {
    int a = 0;
    
    int b = __atomic_load_n(&a, __ATOMIC_SEQ_CST);
    printf("b: %d\n", b);

    b = 1;
    b = __sync_sub_and_fetch(&a, 0);
    printf("b: %d\n", b);
    return 0;
}