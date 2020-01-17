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

typedef struct 
{
  int    type;
  char  *str;
  size_t size;
} Msg;

typedef struct MsgQueue 
{
  Msg             *msg;
  int              fd_i;
  struct MsgQueue *next;
} MsgQueue;

typedef struct 
{
  int                 *fd;
  struct sockaddr_in **addr;
  int                  n;
  int                  size;
  int                  nfds;
  fd_set               set;
  MsgQueue            *q_head;
} Net;

Net *net_create();
void msg_delete(Msg **msg);
void msg_queue_delete(MsgQueue **queue);
void net_delete(Net **net);
int net_accept_client(Net *net, int fd_i);
int net_close_sfd(Net *net, int fd_i);
int _net_add_sfd(int *sd, struct sockaddr_in **addr);
int net_add_client(Net *net, char *ip);
int net_add_server(Net *net);
int net_add_fd(Net *net, int sd, struct sockaddr_in *addr);
int net_send_msg(int sfd, Msg *msg);
int net_recive_msg(int sfd, Msg **msg);
int net_listen(Net *net, int usec);
int _net_resize(Net *net, int expand);

Msg *map_serialize(Map *map);
Map *map_deserialize(Msg *str);

#endif /*NETWORK_H*/
