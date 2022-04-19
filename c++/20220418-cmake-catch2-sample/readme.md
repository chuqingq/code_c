
```
> cd build
> cmake ..
...


chuqq@chuqq-r7000p /m/d/t/c/build [2]> make
[ 94%] Built target Catch2
[ 97%] Built target Catch2WithMain
Consolidate compiler generated dependencies of target tests
[ 98%] Building CXX object CMakeFiles/tests.dir/test.cpp.o
[100%] Linking CXX executable tests
[100%] Built target tests
chuqq@chuqq-r7000p /m/d/t/c/build> ./tests

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tests is a Catch v3.0.0-preview.4 host application.
Run with -? for options

Randomness seeded to: 959821262

-------------------------------------------------------------------------------
Fibonacci
-------------------------------------------------------------------------------
/mnt/d/temp/cmake-sample/test.cpp:18
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
Fibonacci 20                                   100             2     6.0588 ms
                                        30.1991 us    30.0569 us     30.552 us
                                        1.07142 us    543.178 ns    2.20306 us

Fibonacci 25                                   100             1    36.5293 ms
                                        336.676 us    335.933 us    337.672 us
                                        4.34666 us    3.34625 us    5.93076 us

Fibonacci 30                                   100             1    370.863 ms
                                        3.79719 ms    3.74689 ms    3.88899 ms
                                        337.757 us    218.016 us    550.955 us

Fibonacci 35                                   100             1     4.13957 s
                                        41.5433 ms    41.4501 ms    41.7126 ms
                                        623.356 us    369.823 us    1.00675 ms


===============================================================================
All tests passed (6 assertions in 2 test cases)

```
