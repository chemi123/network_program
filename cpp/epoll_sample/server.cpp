#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

int main() {
  int sock1, sock2;
  //  struct sockaddr_in {
  //     u_char  sin_family;    （アドレスファミリ．今回はAF_INETで固定）
  //     u_short sin_port;    （ポート番号）
  //     struct  in_addr sin_addr;    （IPアドレス）
  //     char    sin_zero[8];    （無視してもよい．「詰め物」のようなもの）
  //  };
  //
  //   struct in_addr {
  //      u_int32_t s_addr;
  //   };
  struct sockaddr_in addr1, addr2;
  int epfd;
  struct epoll_event ev;
  char buf[2048];
  int i;
  int nfds; // epfdが監視しているソケットがイベントを受け取った数
  int n;

  sock1 = socket(AF_INET, SOCK_DGRAM, 0);
  sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock1 < 0 || sock2 < 0 ) {
    std::cerr << "failed to create socket" << std::endl;
    return 1;
  } 
  memset(&addr1, 0, sizeof(addr1));
  memset(&addr2, 0, sizeof(addr1));
  addr1.sin_family = AF_INET;
  addr2.sin_family = AF_INET;

  // IPv4/IPv6 アドレスをテキスト形式からバイナリ形式に変換する 
  if (inet_pton(AF_INET, "127.0.0.1", &addr1.sin_addr.s_addr) != 1 ||
      inet_pton(AF_INET, "127.0.0.1", &addr2.sin_addr.s_addr) != 1) {
    std::cerr << "failed to transfer the address to binary" << std::endl;
    return 1;
  }

  addr1.sin_port = htons(11111);
  addr2.sin_port = htons(22222);

  if (bind(sock1, (struct sockaddr *)&addr1, sizeof(addr1)) < 0 ||
      bind(sock2, (struct sockaddr *)&addr2, sizeof(addr2)) < 0) {
    std::cerr << "failed to bind socket" << std::endl;
    return 1;
  }

  epfd = epoll_create(1);
  if (epfd < 0) {
    std::cerr << "epoll_create failed" << std::endl;
    return 1;
  }

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = sock1;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock1, &ev) < 0) {
    std::cerr << "epoll_ctl failed" << std::endl;
    return 1;
  }

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = sock2;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock2, &ev) < 0) {
    std::cerr << "epoll_ctl failed" << std::endl;
    return 1;
  }

  while (1) {
    std::cout << "before epoll_wait" << std::endl;
    memset(&ev, 0, sizeof(ev));
    nfds = epoll_wait(epfd, &ev, 1, 5);
    if (nfds <= 0) {
      std::cerr << "epoll_wait failed" << std::endl;
      return 1;
    }
    std::cout << "after epoll_wait" << std::endl;
    
    for (int i = 0; i < nfds; ++i) {
      n = recv(ev.data.fd, buf, sizeof(buf), 0);
      write(1, buf, n);
    }
  }

  close(sock1);
  close(sock2);

  return 0;
}
