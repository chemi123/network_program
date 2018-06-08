#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <signal.h>

void initSignals();
void sigHandler(int signo);
bool initListener(int &listen_fd);
bool run(int listen_fd);

const std::string SERVER_SOCKET = "server_socket";
bool done = false;

void sigHandler(int signo) {
  signal(signo, SIG_IGN);
  done = true;
}

void initSignals() {
  // なぜかwriteでSIGPIPEが呼ばれたり呼ばれなかったりするため無視する
  signal(SIGPIPE, SIG_IGN);
  // ctrl+cでソケットをクローズして行儀よく終了する
  // signal(SIGINT, sigHandler);
}

bool initListener(int &listen_fd) {
  struct sockaddr_un unix_addr;
  unlink(SERVER_SOCKET.c_str()); 
  // 参考
  // AF_UNIX:  Unix Domain Socket通信
  // AF_INET:  IPv4による通信
  // AF_INET6: IPv6による通信
  //
  // SOCK_STREAM: 双方向通信(TCP)
  // SOCK_DGRAM:  一方通行通信(UDP)
  listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    std::cerr << "server: socket error" << std::endl;
    return false;
  }
  std::cout << "server: socket" << std::endl;

  memset(&unix_addr, 0, sizeof(unix_addr));
  unix_addr.sun_family = AF_UNIX;
  strncpy(unix_addr.sun_path, SERVER_SOCKET.c_str(), sizeof(unix_addr.sun_path)-1);

  if (bind(listen_fd, (struct sockaddr *) &unix_addr, sizeof(unix_addr))) {
    std::cerr << "server: bind error" << std::endl;
    return false;
  }
  std::cout << "server: bind" << std::endl;

  // 第二引数はリッスンキューの数
  if (listen(listen_fd, 5) < 0) {
    std::cerr << "server: listen error" << std::endl;
    return false;
  }
  std::cout << "server: listen" << std::endl;

  return true;
}

bool run(int listen_fd) {
  char buf[256];

  while(!done) {
    int conn = accept(listen_fd, NULL, NULL);
    if (conn < 0) {
      std::cerr << "server: accept error" << std::endl;
      return false;
    } 
    std::cout << "server: accept" << std::endl;

    if (read(conn, buf, sizeof(buf)) < 0) {
      std::cerr << "server: accept error" << std::endl;
      close(conn);
      continue;
    }

    const std::string req(buf, sizeof(buf));
    std::cout << "request: " << req << std::endl; 

    if (write(conn, req.data(), req.size()) < 0) {
      std::cerr << "server: write error" << std::endl;
    }

    close(conn);
  }

  close(listen_fd);
  std::cout << "server close" << std::endl;

  return true;
}

int main() {
  int listen_fd;
  char buf[256];

  initSignals();

  if (!initListener(listen_fd)) {
    std::cerr << "initialize listener failed" << std::endl;
    return 1;
  }

  if (!run(listen_fd)) {
    std::cerr << "run error" << std::endl;
    return 1;
  }

  return 0;
}
