env; hp_lubuntu
$ ./wrk -c 100 -t 10 -d 10 http://127.0.0.1:8081/hello
\Running 10s test @ http://127.0.0.1:8081/hello
  10 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   197.95us  346.16us  19.81ms   97.55%
    Req/Sec    53.35k     4.47k   89.28k    95.53%
  5340664 requests in 10.10s, 677.40MB read
Requests/sec: 528795.89
Transfer/sec:     67.07MB

env: lubuntu@home
chuqq@cqq-lubuntu:~$ wrk -c 100 -d 20 -t 4 http://127.0.0.1:8081/hello
Running 20s test @ http://127.0.0.1:8081/hello
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   693.15us    1.94ms  33.29ms   92.16%
    Req/Sec   208.19k    17.86k  236.37k    72.25%
  16576951 requests in 20.01s, 2.05GB read
Requests/sec: 828484.18
Transfer/sec:    105.08MB

====

换用rust-event的测试方法：服务器只用writable，不用readable，且writable中循环返回响应，直到写入失败。
在虚拟机上（性能是lubuntu@home的10分之一），性能是54w RPS。

chuqq@chuqq-vb:~/rust-event/src$ wrk -c 100 -d 10 -t 4 http://127.0.0.1:8081/
Running 10s test @ http://127.0.0.1:8081/
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   515.28ms  451.14ms   1.97s    68.05%
    Req/Sec   137.04k    66.63k  394.10k    82.80%
  5463940 requests in 10.10s, 693.05MB read
  Socket errors: connect 0, read 120, write 43, timeout 160
Requests/sec: 540872.66
Transfer/sec:     68.60MB