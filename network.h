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

#include <stdio.h>

typedef struct msg_queue {
  char *msg;
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
int sendfmsg(int sfd, char *msg);
int recmsg(int sfd, char **msg);
int selectfd(net_t *net, int usec);
int resize(net_t *net, int expand);

#endif /*NETWORK_H*/
