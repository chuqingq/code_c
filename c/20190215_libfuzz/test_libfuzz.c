#include <stdint.h>
#include <stddef.h>

int VulnerableFunction1(const uint8_t* data, size_t size) {
  int result = 0;
  if (size >= 3) {
    result = data[0] == 'F' &&
             data[1] == 'U' &&
             data[2] == 'Z' &&
             data[3] == 'Z';
  }

  return result;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  VulnerableFunction1(data, size);
  return 0;
}

/*
0.先安装libfuzz，包含在clang中：
sudo apt install clang

1.编译：
chuqq@cqq-dilu /m/e/t/c/c/20190215_libfuzz> clang -g -fsanitize=address,fuzzer test_libfuzz.c  -o test_libfuzz

2.运行：
chuqq@cqq-dilu /m/e/t/c/c/20190215_libfuzz> ./test_libfuzz
INFO: Seed: 2370746163
INFO: Loaded 1 modules   (7 inline 8-bit counters): 7 [0x787f60, 0x787f67),
INFO: Loaded 1 PC tables (7 PCs): 7 [0x565c18,0x565c88),
INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 4096 bytes
INFO: A corpus is not provided, starting from an empty corpus
#2      INITED cov: 3 ft: 3 corp: 1/1b exec/s: 0 rss: 37Mb
#3      NEW    cov: 4 ft: 4 corp: 2/49b exec/s: 0 rss: 37Mb L: 48/48 MS: 1 InsertRepeatedBytes-
#37     REDUCE cov: 4 ft: 4 corp: 2/33b exec/s: 0 rss: 38Mb L: 32/32 MS: 4 ChangeByte-ChangeBit-CopyPart-EraseBytes-
#44     REDUCE cov: 4 ft: 4 corp: 2/32b exec/s: 0 rss: 38Mb L: 31/31 MS: 2 ChangeByte-EraseBytes-
#46     REDUCE cov: 4 ft: 4 corp: 2/18b exec/s: 0 rss: 38Mb L: 17/17 MS: 2 InsertByte-EraseBytes-
#54     REDUCE cov: 4 ft: 4 corp: 2/13b exec/s: 0 rss: 38Mb L: 12/12 MS: 3 ChangeByte-InsertByte-EraseBytes-
#65     REDUCE cov: 4 ft: 4 corp: 2/12b exec/s: 0 rss: 38Mb L: 11/11 MS: 1 EraseBytes-
#86     REDUCE cov: 4 ft: 4 corp: 2/10b exec/s: 0 rss: 38Mb L: 9/9 MS: 1 EraseBytes-
#124    REDUCE cov: 4 ft: 4 corp: 2/9b exec/s: 0 rss: 38Mb L: 8/8 MS: 3 CopyPart-ChangeBinInt-EraseBytes-
#150    REDUCE cov: 4 ft: 4 corp: 2/5b exec/s: 0 rss: 38Mb L: 4/4 MS: 1 EraseBytes-
#182    REDUCE cov: 4 ft: 4 corp: 2/4b exec/s: 0 rss: 38Mb L: 3/3 MS: 2 EraseBytes-CopyPart-
#3080   NEW    cov: 5 ft: 5 corp: 3/65b exec/s: 0 rss: 41Mb L: 61/61 MS: 3 CopyPart-CopyPart-InsertRepeatedBytes-
#3205   REDUCE cov: 5 ft: 5 corp: 3/46b exec/s: 0 rss: 41Mb L: 42/42 MS: 5 CrossOver-CopyPart-CopyPart-ShuffleBytes-EraseBytes-
#3211   REDUCE cov: 5 ft: 5 corp: 3/29b exec/s: 0 rss: 41Mb L: 25/25 MS: 1 EraseBytes-
#3255   REDUCE cov: 5 ft: 5 corp: 3/21b exec/s: 0 rss: 42Mb L: 17/17 MS: 4 CopyPart-EraseBytes-ChangeBinInt-EraseBytes-
#3316   REDUCE cov: 5 ft: 5 corp: 3/13b exec/s: 0 rss: 42Mb L: 9/9 MS: 1 EraseBytes-
#3334   REDUCE cov: 5 ft: 5 corp: 3/11b exec/s: 0 rss: 42Mb L: 7/7 MS: 3 InsertByte-ChangeBinInt-EraseBytes-
#3345   REDUCE cov: 5 ft: 5 corp: 3/8b exec/s: 0 rss: 42Mb L: 4/4 MS: 1 EraseBytes-
#3351   REDUCE cov: 5 ft: 5 corp: 3/7b exec/s: 0 rss: 42Mb L: 3/3 MS: 1 EraseBytes-
#20248  REDUCE cov: 6 ft: 6 corp: 4/11b exec/s: 0 rss: 60Mb L: 4/4 MS: 2 EraseBytes-CMP- DE: "U\x00"-
#20325  REDUCE cov: 6 ft: 6 corp: 4/10b exec/s: 0 rss: 60Mb L: 3/3 MS: 2 PersAutoDict-EraseBytes- DE: "U\x00"-
#39817  REDUCE cov: 7 ft: 7 corp: 5/17b exec/s: 0 rss: 80Mb L: 7/7 MS: 2 ShuffleBytes-CMP- DE: "Z\x00\x00\x00"-
#40043  REDUCE cov: 7 ft: 7 corp: 5/14b exec/s: 0 rss: 80Mb L: 4/4 MS: 1 EraseBytes-
=================================================================
==3999==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x6020000c16b3 at pc 0x00000054a826 bp 0x7fffebceea20 sp 0x7fffebceea18
READ of size 1 at 0x6020000c16b3 thread T0
    #0 0x54a825  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x54a825)
    #1 0x54a8c4  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x54a8c4)
    #2 0x42e9f7  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x42e9f7)
    #3 0x439264  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x439264)
    #4 0x43a8cf  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x43a8cf)
    #5 0x429c8c  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x429c8c)
    #6 0x41cb52  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x41cb52)
    #7 0x7f3189c91b96  (/lib/x86_64-linux-gnu/libc.so.6+0x21b96)
    #8 0x41cbc9  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x41cbc9)

0x6020000c16b3 is located 0 bytes to the right of 3-byte region [0x6020000c16b0,0x6020000c16b3)
allocated by thread T0 here:
    #0 0x5120e0  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x5120e0)
    #1 0x7f318ad031a7  (/usr/lib/x86_64-linux-gnu/libstdc++.so.6+0x931a7)
    #2 0x439264  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x439264)
    #3 0x43a8cf  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x43a8cf)
    #4 0x429c8c  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x429c8c)
    #5 0x41cb52  (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x41cb52)
    #6 0x7f3189c91b96  (/lib/x86_64-linux-gnu/libc.so.6+0x21b96)

SUMMARY: AddressSanitizer: heap-buffer-overflow (/mnt/e/temp/code_c/c/20190215_libfuzz/test_libfuzz+0x54a825)
Shadow bytes around the buggy address:
  0x0c0480010280: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fa
  0x0c0480010290: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fa
  0x0c04800102a0: fa fa fd fa fa fa fd fd fa fa fd fd fa fa fd fa
  0x0c04800102b0: fa fa fd fd fa fa fd fd fa fa fd fd fa fa fd fa
  0x0c04800102c0: fa fa fd fa fa fa fd fa fa fa fd fd fa fa fd fd
=>0x0c04800102d0: fa fa fd fa fa fa[03]fa fa fa fa fa fa fa fa fa
  0x0c04800102e0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c04800102f0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c0480010300: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c0480010310: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c0480010320: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==3999==ABORTING
MS: 2 InsertByte-EraseBytes-; base unit: 0bcd9049c8bf7995250c791aebe6d9704226c9e6
0x46,0x55,0x5a,
FUZ
artifact_prefix='./'; Test unit written to ./crash-0eb8e4ed029b774d80f2b66408203801cb982a60
Base64: RlVa
⏎

3.分析上述错误信息，注意到“READ of size 1 at 0x6020000c16b3 thread T0 #0 0x54a825”是非法读取的地方，通过gdb查看代码：
(gdb) list *0x54a825
0x54a825 is in VulnerableFunction1 (test_libfuzz.c:10).
5         int result = 0;
6         if (size >= 3) {
7           result = data[0] == 'F' &&
8                    data[1] == 'U' &&
9                    data[2] == 'Z' &&
10                   data[3] == 'Z';
11        }
12
13        return result;
14      }
(gdb)

结论：第10行越界读取。

*/

