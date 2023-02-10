#pragma once

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

// CanSocket 物理层面的Can收发。不包含上层协议编解码
class CanSocket {
 public:
  int Open(const char *device) {
    int s, ret;
    struct sockaddr_can addr;
    struct ifreq ifr;

    // create socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
      perror("socket");
      return -1;
    }

    // get interface index by name
    strncpy(ifr.ifr_name, device, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if (!ifr.ifr_ifindex) {
      perror("if_nametoindex");
      return -1;
    }
    // printf("index: %d\n", ifr.ifr_ifindex);

    // set interface index into addr
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // bind
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
      perror("bind");
      return -1;
    }

    can_sock_ = s;
    return 0;
  }
  void Close() { close(can_sock_); }
  ~CanSocket() { Close(); }

  int Send(const struct canfd_frame *frame) {
    int ret = write(can_sock_, frame, CAN_MTU);
    if (ret != sizeof(*frame)) {
      perror("write");
    }
    return ret;
  }

  int Receive(struct canfd_frame *frame) {
    int ret = read(can_sock_, frame, CAN_MTU);
    if (ret != sizeof(*frame)) {
      perror("read");
    }
    return ret;
  }

 private:
  int can_sock_;
};
