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
  struct sockaddr_in groupcastaddr, addr;
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
  groupcastaddr.sin_addr.s_addr = inet_addr(argv[1]);  // 224-239
  groupcastaddr.sin_port = htons(atoi(argv[2]));

  // 第三步:将套接字与服务器网络信息结构体绑定
  if (bind(sockfd, (struct sockaddr *)&groupcastaddr, addrlen) < 0) {
    errlog("fail to bind");
  }

  // 加入多播组，允许数据链路层处理指定数据包
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) <
      0) {
    errlog("fail to setsockopt");
  }

  ssize_t bytes;
  while (1) {
    if ((bytes = recvfrom(sockfd, buf, N, 0, (struct sockaddr *)&addr,
                          &addrlen)) < 0) {
      errlog("fail to recvfrom");
    } else {
      printf("ip: %s, port: %d\n", inet_ntoa(addr.sin_addr),
             ntohs(addr.sin_port));

      printf("groupcast : %s\n", buf);
    }
  }

  close(sockfd);

  return 0;
}
