#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "macro.h"

typedef struct {
  int *fd;
  struct sockaddr_in **addr;
  int n;
  int size;
//int maxfd;
} net_t;

net_t *getnett();
void delnett(net_t **net);
int acceptsfd(net_t *net, int fd_i);
int closesfd(net_t *net, int fd_i);
int creatsfd(net_t *net, char *ip);
int addfd(net_t *net, int sd, struct sockaddr_in *addr);
int resize(net_t *net, int expand);

#endif /*NETWORK_H*/
