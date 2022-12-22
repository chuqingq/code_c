package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"time"
)

func main() {
	// num_cores := runtime.NumCPU()
	// runtime.GOMAXPROCS(num_cores)

	localaddr := flag.String("local", "", "local addr")
	addr := flag.String("server", "127.0.0.1:20000", "server")
	c := flag.Int("c", 1, "connections")
	count := flag.Int("count", 10000, "count")
	recv := flag.Bool("recv", true, "recv")
	flag.Parse()

	local_addr, err := net.ResolveUDPAddr("udp", *localaddr)
	if err != nil {
		log.Fatalf("localaddr resolve error: %v\n", err)
	}

	server_addr, err := net.ResolveUDPAddr("udp", *addr)
	if err != nil {
		log.Fatalf("addr resolve error: %v\n", err)
	}

	log.Printf("cores: %d, connections: %d, recv: %v, server: %s\n", 1, *c, *recv, *addr)

	for i := 0; i < *c; i++ {
		go func() {
			conn, err := net.ListenUDP("udp", local_addr)
			if err != nil {
				log.Fatalf("dialudp error: %v\n", err)
			}

			buf := make([]byte, 64*1024-1)
			start := time.Now()

			for i := 0; i < *count; i++{
				_, err = conn.WriteTo(buf, server_addr)
				if err != nil {
					log.Printf("write error: %v\n", err)
					continue
				}

				if *recv {
					n, err := conn.Read(buf)
					if err != nil {
						log.Printf("read error: %v\n", err)
					} else if n != len(buf) {
						log.Printf("read len error: %v\n", n)
					}
				}
			}

			duration := time.Now().Sub(start)
			log.Printf("Total: %v for %v loops; average: %v ns/loop", duration, *count, duration.Nanoseconds()/ int64(*count))
		}()
	}
	var wait string
	fmt.Scanf("%s", &wait)
}

/*
2022/12/02 17:58:31 Total: 3.79103183s for 10000 loops; average: 379103 ns/loop
*/
