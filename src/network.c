#include "../include/network.h"

const uint16_t kPort = 55555;

const int kOther = 0;
const int kArea = 1;

Net *
net_create()
{
  Net *net;
  RPTEST(net= malloc(sizeof(Net)), NULL);
  
  net->fd = NULL;
  net->addr = NULL;
  net->n = 0;
  net->size = 0;
  net->nfds = -1;
  net->q_head = NULL;

  FD_ZERO(&(net->set));
  return net;
}

void 
msg_delete(Msg **msg) 
{
  free((*msg)->str);
  free((*msg));
  msg = NULL;
}

void 
msg_queue_delete(MsgQueue **queue)
{
  MsgQueue **ptr = queue;
  while(*ptr != NULL) {
    ptr = &(*queue)->next;
    msg_delete(&(*queue)->msg);
    free(*queue);
    *queue = *ptr;
  }
  queue = NULL;
}

void 
net_delete(Net **net)
{
  for(int i=(*net)->n-1; i>=0; i--) {
    if((*net)->addr[i] != NULL) {
      net_close_sfd(*net, i);
    }
  }
  
  msg_queue_delete(&(*net)->q_head);
  
  free((*net)->fd);
  for(int i=(*net)->n-1; i>=0; i--) {
    free((*net)->addr[i]);
  }
  free((*net)->addr);
  free(*net);

  net = NULL;
}

int 
net_accept_client(Net *net, 
                  int  fd_i)
{
  int sd;
  int addr_len;
  struct sockaddr_in *addr;

  RPTEST(addr = malloc(sizeof(struct sockaddr_in)), -1);
  addr_len = sizeof(*addr);
  RTEST(sd = accept(net->fd[fd_i],
        (struct sockaddr*)(addr),
        (socklen_t*)(&addr_len)), -1);

  return net_add_fd(net, sd, addr);
}

int 
net_close_sfd(Net *net, 
              int  fd_i)
{
  RTEST(shutdown(net->fd[fd_i], 2), -1);
  RTEST(close(net->fd[fd_i]), -1);

  free(net->addr[fd_i]);
  net->addr[fd_i] = NULL;

  net->n--;
  if(fd_i < net->n) {
    net->fd[fd_i] = net->fd[net->n];
    net->addr[fd_i] = net->addr[net->n];
  }
  //calc new nfds
  return 0;
}

int 
_net_add_sfd(int *sd, 
             struct sockaddr_in **addr)
{
  RTEST(*sd = socket(AF_INET, SOCK_STREAM, 0), -1);
  RPTEST(*addr = malloc(sizeof(struct sockaddr_in)), -1);
  
  memset(*addr, 0, sizeof(struct sockaddr_in));
  (*addr)->sin_family = AF_INET;
  (*addr)->sin_port = htons(kPort);

  return 0;
}

int 
net_add_client(Net  *net, 
            char *ip)
{
  int sd;
  struct sockaddr_in *addr;
 
  RTEST(_net_add_sfd(&sd, &addr), -1);

  addr->sin_addr.s_addr = inet_addr(ip);
  RTEST(connect(sd, (struct sockaddr*)(addr),
        sizeof(struct sockaddr_in)), -1);

  return net_add_fd(net, sd, addr);
}

int 
net_add_server(Net  *net)
{
  int sd;
  struct sockaddr_in *addr;
    
  RTEST(_net_add_sfd(&sd, &addr), -1);
  
  addr->sin_addr.s_addr = htonl(INADDR_ANY);
  RTEST(bind(sd, (struct sockaddr*)(addr),
        sizeof(struct sockaddr_in)), -1);
  RTEST(listen(sd, 10), -1);
  
  return net_add_fd(net, sd, addr);
}

int 
net_add_fd(Net                *net, 
           int                 sd, 
           struct sockaddr_in *addr)
{
  if(net->n == net->size) {
    RTEST(_net_resize(net, 1), -1);
  }
  net->fd[net->n] = sd;
  net->addr[net->n] = addr;
  net->n++;
  if(sd > net->nfds) {
    net->nfds = sd;
  }
  return net->n - 1;
}

int 
net_send_msg(int  sfd, 
             Msg *msg)
{
  char *fmsg;
  RPTEST(fmsg = malloc((msg->size+2)*sizeof(char)), -1);
  fmsg[0] = msg->size;
  fmsg[1] = msg->type;
  memcpy(fmsg+2, msg->str, msg->size);
  return send(sfd, fmsg, msg->size+2, 0);
}

int 
net_recive_msg(int   sfd, 
               Msg **msg)
{
  char chr[2];
  
  if(recv(sfd, &chr, 2, 0) < 2) {
    return -1;
  }

  if(((*msg) = malloc(sizeof(Msg))) == NULL 
  || ((*msg)->str = malloc(chr[0]*sizeof(char))) == NULL)
  {
    (*msg)->size = -1;
  } else {
    (*msg)->size = recv(sfd, (*msg)->str, chr[0], 0);
    if((*msg)->size != chr[0]) {
      msg_delete(msg);
      return -1;
    }
  }
  (*msg)->type = chr[1];
  
  return (*msg)->size;
}

int 
net_listen(Net *net, 
           int  usec)
{
  int s;
  for(int i=net->n-1; i>=0; i--) {
    FD_SET(net->fd[i], &(net->set));
  }
  if(usec > 0) {
    static struct timeval timeout;
    timeout.tv_usec = usec;
    RTEST(s = select(net->nfds+1, &net->set, NULL, NULL, &timeout), -1);
    if(s == 0) {
      return 0;
    }
  } else {
    RTEST(s = select(net->nfds+1, &net->set, NULL, NULL, NULL), -1);
  }

  if(FD_ISSET(net->fd[0], &(net->set))) {
    net_accept_client(net, 0);
  }
  for(int i=net->n-1; i>0; i--) {
    if(FD_ISSET(net->fd[i], &(net->set))) {
      MsgQueue *msg;
      RPTEST(msg = malloc(sizeof(MsgQueue)), -1);
      RPTEST(msg->msg = malloc(sizeof(Msg)), -1);
      net_recive_msg(net->fd[i], &msg->msg);
//    if(recmsg(net->fd[i], &(msg->msg)) == 0)
//    {
//      closesfd(net, i);
//      free(msg);
//      continue;
//    }
      if(msg->msg->size == -1) {
        free(msg);
        continue;
      }
      msg->fd_i = i; 
      if(net->q_head == NULL) { msg->next = NULL;        } 
      else                    { msg->next = net->q_head; }
      net->q_head = msg;
    }
  }
  return s;
}

int 
_net_resize(Net *net, 
            int  expand)
{
  int i = 0;
  int *tmp_fd;
  struct sockaddr_in **tmp_addr;
  RPTEST(tmp_fd = malloc((net->size+expand)*sizeof(int)), -1);
  RPTEST(tmp_addr = malloc(
        (net->size+expand)*sizeof(struct sockaddr_in*) ), -1);
  for(; i<net->size; i++) {
    tmp_fd[i] = net->fd[i];
    tmp_addr[i] = net->addr[i];
  }
  net->size += expand;
  for(; i<net->size; i++) {
    tmp_fd[i] = 0;
    tmp_addr[i] = NULL;
  }
  free(net->fd);
  free(net->addr);
  net->fd = tmp_fd;
  net->addr = tmp_addr;
  return 0;
}

Msg *
map_serialize(Map *map) 
{
  Msg *str;

  RPTEST(str = malloc(sizeof(Msg)), NULL);
  str->type = kArea;
  str->size = 2*(map->w+1)*map->h;
  RPTEST(str->str = malloc((str->size+2)*sizeof(char)), NULL);
  str->str[0] = map->h;
  str->str[1] = map->w + 1;
  for(int i=str->size/2-1; i>=0; i--) {
    str->str[2*i+2] = map->data[0][i]%256;
    str->str[2*i+3] = map->data[0][i]/256;
  }
  return str;
}

Map *
map_deserialize(Msg *str) 
{
  Map *map;
  int h = str->str[0];
  int w = str->str[1];

  RPTEST(map = map_create(0, h, w-1), NULL);
  for(int i=w*h-1; i>=0; i--) {
    map->data[0][i] = str->str[2*i+3]*256 + str->str[2*i+2];
  }
  return map;
}

