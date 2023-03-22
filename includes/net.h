#ifndef __NETH__
#define __NETH__

int default_sock();

int sock_with_address(const char*ip,short port);

void set_nonblock(int fd);

int sock_accept(int listenfd);

#endif