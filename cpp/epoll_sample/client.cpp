#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
  int sock;
  struct sockaddr_in dest1, dest2;
  std::ostringstream message;

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  dest1.sin_family = AF_INET;
  dest2.sin_family = AF_INET;

  inet_pton(AF_INET, "127.0.0.1", &dest1.sin_addr.s_addr);
  inet_pton(AF_INET, "127.0.0.1", &dest2.sin_addr.s_addr);

  dest1.sin_port = htons(11111);
  dest2.sin_port = htons(22222);

  message.str("");
  message << "data to port 11111\n";
  sendto(sock, message.str().c_str(), strlen(message.str().c_str()), 0,
         (struct sockaddr *)&dest1, sizeof(dest1));

  message.str("");
  message << "data to port 22222\n";
  sendto(sock, message.str().c_str(), strlen(message.str().c_str()), 0,
         (struct sockaddr *)&dest2, sizeof(dest2));

  close(sock);

  return 0;
}
