#ifndef NETWORK_H
#define NETWORK_H

#define _XOPEN_SOURCE_EXTENDED 

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "macro.h"
#include "map.h"

#include <stdio.h>

typedef struct {
  char *str;
  size_t size;
} str_t;

typedef struct msg_queue {
  str_t *msg;
  int fd_i;
  struct msg_queue *next;
} msg_queue_t;

typedef struct {
  int *fd;
  struct sockaddr_in **addr;
  int n;
  int size;
  int nfds;
  fd_set set;
  msg_queue_t *q_head;
  msg_queue_t *q_tail;
} net_t;

net_t *getnett();
void delnett(net_t **net);
int acceptsfd(net_t *net, int fd_i);
int closesfd(net_t *net, int fd_i);
int creatsfd(net_t *net, char *ip);
int addfd(net_t *net, int sd, struct sockaddr_in *addr);
int sendfmsg(int sfd, str_t *msg);
int recmsg(int sfd, str_t **msg);
int selectfd(net_t *net, int usec);
int resize(net_t *net, int expand);

str_t *mapserialize(map_t *map);
map_t *mapdeserialize(str_t *str);

#endif /*NETWORK_H*/
