#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define N 128
#define errlog(errmsg)                                    \
  do {                                                    \
    perror(errmsg);                                       \
    printf("%s--%s--%d\n", __FILE__, __func__, __LINE__); \
    exit(1);                                              \
  } while (0)

int main(int argc, const char *argv[]) {
  int sockfd;
  struct sockaddr_in groupcastaddr;
  socklen_t addrlen = sizeof(groupcastaddr);
  char buf[N] = {};

  if (argc < 3) {
    fprintf(stderr, "Usage: %s ip port\n", argv[0]);
    exit(1);
  }

  // 第一步:创建套接字
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    errlog("fail to socket");
  }

  // 第二步:填充组播网络信息结构体
  // inet_addr:将点分十进制ip地址转化为网络字节序的整型数据
  // htons:将主机字节序转化为网络字节序
  // atoi:将数字型字符串转化为整型数据
  groupcastaddr.sin_family = AF_INET;
  // 224.x.x.x - 239.x.x.x
  groupcastaddr.sin_addr.s_addr = inet_addr(argv[1]);
  groupcastaddr.sin_port = htons(atoi(argv[2]));

  while (1) {
    fgets(buf, N, stdin);
    buf[strlen(buf) - 1] = '\0';

    if (sendto(sockfd, buf, N, 0, (struct sockaddr *)&groupcastaddr, addrlen) <
        0) {
      errlog("fail to sendto");
    }
  }

  close(sockfd);

  return 0;
}
// 在IPv4英特网域（AF_INET）中，组播地址结构体用如下结构体ip_mreq表示。
// typedef  uint32_t  in_addr_t;
// struct in_addr{
// in_addr_t s_addr;
// };
// struct ip_mreq  {
//      struct in_addr imr_multiaddr;   /*组播的IP地址*/
//      struct in_addr imr_interface;   /*本机的IP地址*/
//  };
