#include "../include/network.h"

const uint16_t kPort = 55555;

Net *
net_create()
{
  Net *net;
  RPTEST(net= malloc(sizeof(Net)), NULL);
  
  net->fd = NULL;
  net->id = NULL;
  net->opt = NULL;
  net->type = NULL;

  net->n = 0;
  net->nfds = -1;
  net->size = 0;
  net->next_id = 0;

  net->q_head = NULL;

  FD_ZERO(&(net->set));
  return net;
}

void
net_delete(Net **net)
{
  for(int i=(*net)->n-1; i>=0; i--) {
    if((*net)->type[i] == SRC_SERVER
    || (*net)->type[i] == SRC_CLIENT) 
    {
      free((*net)->opt[i]);
      net_close_sfd(*net, i);
    }
  }
  
  msg_queue_delete(&(*net)->q_head);
  
  free((*net)->fd);
  free((*net)->id);
  free((*net)->type);
  free((*net)->opt);
  free(*net);

  net = NULL;
}

int 
net_find(Net *net, 
         int  id)
{
  for(int fd_i=net->n-1; fd_i>=0; fd_i--) {
    if(net->id[fd_i] == id) {
      return fd_i;
    }
  }
  return -1;
}

int 
net_add_fd(Net  *net, 
           int   fd, 
           void *opt,
           char  type)
{
  if(net->n == net->size) {
    RTEST(_net_resize(net, 1), -1);
  }
  net->fd[net->n] = fd;
  net->id[net->n] = net->next_id;
  net->opt[net->n] = opt;
  net->type[net->n] = type;
  net->n++;
  net->next_id++;
  if(fd > net->nfds) {
    net->nfds = fd;
  }
  return net->n - 1;
}

int 
_net_resize(Net *net, 
            int  expand)
{
  int i = 0;
  int   *tmp_fd;
  int   *tmp_id;
  void **tmp_opt;
  char  *tmp_type;

  RPTEST(tmp_fd = malloc((net->size+expand)*sizeof(int)), -1);
  if((tmp_id = malloc((net->size+expand)*sizeof(int))) == NULL) {
    free(tmp_fd);
  }
  if((tmp_opt = malloc((net->size+expand)*sizeof(void*))) == NULL) {
    free(tmp_id);
    free(tmp_fd);
  }
  if((tmp_type = malloc((net->size+expand)*sizeof(char))) == NULL) {
    free(tmp_opt);
    free(tmp_id);
    free(tmp_fd);
  }

  for(; i<net->size; i++) {
    tmp_fd[i]   = net->fd[i];
    tmp_id[i]   = net->id[i];
    tmp_opt[i]  = net->opt[i];
    tmp_type[i] = net->type[i];
  }
  net->size += expand;
  for(; i<net->size; i++) {
    tmp_fd[i]   = 0;
    tmp_id[i]   = 0;
    tmp_opt[i]  = NULL;
    tmp_type[i] = 0;
  }
  free(net->fd);
  free(net->id);
  free(net->opt);
  free(net->type);
  net->fd   = tmp_fd;
  net->id   = tmp_id;
  net->opt  = tmp_opt;
  net->type = tmp_type;
  return 0;
}

int 
_net_add_sfd(Net  *net, 
             char *ip)
{
  int sd;
  struct sockaddr_in *addr;
  
  RTEST(sd = socket(AF_INET, SOCK_STREAM, 0), -1);
  RPTEST(addr = malloc(sizeof(struct sockaddr_in)), -1);
  
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(kPort);

  if(ip == NULL) {
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sd, (struct sockaddr*)(addr),
          sizeof(struct sockaddr_in)) < 0
        || listen(sd, 10) < 0)
    {
      free(addr);
      return -1;
    }
  }
  else {
    addr->sin_addr.s_addr = inet_addr(ip);
    if((connect(sd, (struct sockaddr*)(addr),
            sizeof(struct sockaddr_in))) <0)
    {
      free(addr);
      return -1;
    }
  }

  return net_add_fd(net, sd, addr, SRC_CLIENT);
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
  if((sd = accept(net->fd[fd_i],
        (struct sockaddr*)(addr),
        (socklen_t*)(&addr_len))) < 0)
  {
    free(addr);
    return -1;
  }

  return net_add_fd(net, sd, addr, SRC_CLIENT);
}

int 
net_close_sfd(Net *net, 
              int  fd_i)
{
  if(net->type[fd_i] != SRC_SERVER
  && net->type[fd_i] != SRC_CLIENT)
  {
    return 0;
  }

  RTEST(shutdown(net->fd[fd_i], 2), -1);
  RTEST(close(net->fd[fd_i]), -1);

  free(net->opt[fd_i]);
  net->opt[fd_i] = NULL;

  net->n--;
  if(fd_i < net->n) {
    net->fd[fd_i] = net->fd[net->n];
    net->id[fd_i] = net->id[net->n];
    net->opt[fd_i] = net->opt[net->n];
    net->type[fd_i] = net->type[net->n];
  }

  net->nfds = net->fd[0];
  for(int i=net->n-1; i>0; i--) {
    if(net->fd[i] > net->nfds) {
      net->nfds = net->fd[i];
    }
  }
  return 0;
}

int 
net_send_msg(int  sfd, 
             Msg *msg)
{
  char *fmsg;
  RPTEST(fmsg = malloc((msg->size+4)*sizeof(char)), -1);
  fmsg[0] = msg->size;
  fmsg[1] = msg->response;
  fmsg[2] = msg->id;
  fmsg[3] = msg->type;
  memcpy(fmsg+4, msg->str, msg->size);
  return send(sfd, fmsg, msg->size+4, 0);
}

int 
net_recive_msg(int   sfd, 
               Msg **msg)
{
  char chr[4];
  
  if(recv(sfd, &chr, 4, 0) < 4) {
    return -1;
  }
  RPTEST((*msg) = msg_create(chr[2], chr[3], chr[0]), -1);
  (*msg)->response = chr[1];
    
  return recv(sfd, (*msg)->str, chr[0], 0);
}

int 
net_listen(Net *net, 
           int  usec)
{
  int s;
  for(int fd_i=net->n-1; fd_i>=0; fd_i--) {
    FD_SET(net->fd[fd_i], &(net->set));
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

  Msg *msg = NULL;
  char chr;
  for(int fd_i=0; fd_i<net->n; fd_i++) {
    if(FD_ISSET(net->fd[fd_i], &(net->set))) {
      switch(net->type[fd_i])
      {
        case SRC_ERROR:
          break;
        case SRC_SERVER:
          net_accept_client(net, fd_i);
          break;
        case SRC_CLIENT:
          if(net_recive_msg(net->fd[fd_i], &msg) < 0){
            continue;
          }
          msg_queue_add(&net->q_head, msg, net->id[fd_i]);
          break;
        case SRC_STDIN:
          chr = ((int (*)())net->opt[fd_i])();
          if(chr < 32) {
            continue;
          }
          if((msg = msg_create(0, MSG_TEXT, 1)) == NULL) {
            continue;
          }
          msg->str[0] = chr;
          
          msg_queue_add(&net->q_head, msg, net->id[fd_i]);
          break;
      }
    }
  }
  return s;
}

Msg *
map_serialize(Map *map) 
{
  Msg *str;
  
  RPTEST(str = msg_create(0, MSG_AREA, 2*(map->w+1)*map->h+2), NULL);
  str->size -= 2;
  
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

