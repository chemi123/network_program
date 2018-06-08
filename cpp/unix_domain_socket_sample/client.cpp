#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <iostream>

const std::string SERVER_SOCKET = "server_socket";

int main() {
  int fd;
  struct sockaddr_un unix_addr;
  char buf[256];

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    std::cerr << "client: socket error" << std::endl;
    std::cerr << strerror(errno) << std::endl;
    return 1;
  }
  std::cout << "client: socket" << std::endl;

  memset(&unix_addr, 0, sizeof(unix_addr));
  unix_addr.sun_family = AF_UNIX;
  strncpy(unix_addr.sun_path, SERVER_SOCKET.c_str(), sizeof(unix_addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*) &unix_addr, sizeof(unix_addr)) < 0) {
    std::cerr << "client: connect error" << std::endl;
    std::cerr << strerror(errno) << std::endl;
    return 1;
  }
  std::cout << "client: connect" << std::endl;

  std::string message = "request";
  strncpy(buf, message.c_str(), sizeof(buf));
  if (write(fd, buf, sizeof(buf)) < 0) {
    std::cerr << "client: send error" << std::endl;
    return 1;
  }
  std::cout << "client: send" << std::endl;

  memset(&buf, 0, sizeof(buf));
  if (read(fd, buf, sizeof(buf)) < 0) {
    std::cerr << "client: recv failed" << std::endl;
    return 1;
  }
  const std::string res(buf, sizeof(buf));
  std::cout << "response: " << res << std::endl;

  close(fd);

  return 0;
}
