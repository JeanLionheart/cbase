#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <net.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int default_sock();

int sock_with_address(const char* ip, short port);

void set_nonblock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    fl |= O_NONBLOCK;
    fcntl(fd, F_SETFL, fl);
}

int default_sock()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}
#include <stdio.h>
int sock_with_address(const char* ip, short port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        return -1;

    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr(ip);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    printf("%s:%d\n", ip, port);

    int ret = bind(fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret == -1)
        return -2;

    return fd;
}

int sock_accept(int listenfd)
{
    return accept(listenfd, 0, 0);
}