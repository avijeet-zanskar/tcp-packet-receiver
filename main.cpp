#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>

#define SERVER_IP_ADDR "localhost"
#define SERVER_TCP_PORT 31415

int main() {
    int fd, listenfd, one = 1;
    sockaddr_in sa;
    socklen_t slen;
    ssize_t len;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0) {
        std::cerr << "socket error\n" << strerror(errno);
        return -1;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        std::cerr << "setsockopt error\n" << strerror(errno);
        return -1;
    }

    std::memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton(SERVER_IP_ADDR, &sa.sin_addr);
    sa.sin_port = htons(SERVER_TCP_PORT);
    if (bind(listenfd, (struct sockaddr *)&sa, sizeof(sa))) {
        std::cerr << "bind error\n" << strerror(errno);
        return -1;
    }

    if (listen(listenfd, 1)) {
        std::cerr << "listen error\n" << strerror(errno);
        return -1;
    }

    slen = sizeof(sa);
    std::memset(&sa, 0, sizeof(sa));
    fd = accept(listenfd, (struct sockaddr *)&sa, &slen);
    if (fd < 0) {
        std::cerr << "accept error\n" << strerror(errno);
        return -1;
    }
    int packetSize;
    recv(fd, &packetSize, sizeof(int), 0);
    auto packet = std::make_unique<unsigned char[]>(packetSize);
    while (true) {
        len = recv(fd, packet.get(), packetSize, 0);
        if (len <= 0) {
            break;
        }
        std::cout << "Packet received\n";
    }
    close(fd);
    close(listenfd);
}