/**
 *  \file network.h
 *  \file network.c
 *
 *  \todo
 *  mv struct to net_struct.h
 *  mv struct-net & net to net_comm.h
 *  mv utils to net_utils.h
 *  add wrapers
 */

#ifndef NETWORK_H
#define NETWORK_H

#ifndef _XOPEN_SOURCE_EXTENDED 
  #define _XOPEN_SOURCE_EXTENDED 
#endif

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
#include "msg.h"

#include <stdio.h>

#define net_add_client(net, ip) _net_add_sfd(net, ip)
#define net_add_server(net)     _net_add_sfd(net, NULL)

enum { SRC_ERROR, SRC_SERVER, SRC_CLIENT, SRC_STDIN };

typedef struct 
{
  int      *fd;
  int      *id;
  void    **opt;
  char     *type;

  int       n;
  int       nfds;
  int       size;
  int       next_id;

  fd_set    set;
  MsgQueue *q_head;
} Net;

//struct
Net *net_create();
void net_delete(Net **net);
int net_find(Net *net, int id);
int net_add_fd(Net *net, int fd, void *opt, char type);
int _net_resize(Net *net, int expand);
//struct-net
int _net_add_sfd(Net *net, char *ip);
int net_accept_client(Net *net, int fd_i);
int net_close_sfd(Net *net, int fd_i);
//net
int net_send_msg(int sfd, Msg *msg);
int net_recive_msg(int sfd, Msg **msg);
int net_listen(Net *net, int usec);
//util
Msg *map_serialize(Map *map);
Map *map_deserialize(Msg *str);

#endif /*NETWORK_H*/
