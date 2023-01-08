#include <uv.h>

int main() {
  uv_interface_address_t *address;
  int count;
  int r = uv_interface_addresses(&address, &count);
  if (r < 0) {
    perror("error");
    return r;
  }

  char buf[512];

  for (int i = 0; i < count; i++) {
    auto interface = address[i];
    printf("name: %s\n", interface.name);
    if (interface.address.address4.sin_family == AF_INET) {
      uv_ip4_name(&interface.address.address4, buf, sizeof(buf));
    } else if (interface.address.address4.sin_family == AF_INET6) {
      continue;
      uv_ip6_name(&interface.address.address6, buf, sizeof(buf));
    }
  }

  printf("ip: %s\n", buf);
  uv_free_interface_addresses(address, count);
  return 0;
}

/*
$ g++ -o libuv_interface_address{,.cpp} -g -Wall -luv
$ ./libuv_interface_address
name: lo
ip: 127.0.0.1
name: eth0
ip: 172.18.49.178
name: lo
ip: ::1
name: eth0
ip: fe80::215:5dff:fe50:11b0
*/