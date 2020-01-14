#include "network.h"

net_t *getnett()
{
  net_t *net;
  RPTEST(net= malloc(sizeof(net_t)), NULL);
  net->fd = NULL;
  net->addr = NULL;
  net->n = 0;
  net->size = 0;
  net->nfds = -1;
  net->q_head = NULL;
  net->q_tail = NULL;
  FD_ZERO(&(net->set));
  return net;
}

void delqueue(msg_queue_t *queue)
{
  msg_queue_t *ptr = queue;
  while(ptr != NULL) {
    ptr = queue->next;
    free(queue->msg->str);
    free(queue->msg);
    free(queue);
    queue = ptr;
  }
}

void delnett(net_t **net)
{
  for(int i=(*net)->n-1; i>=0; i--) {
    if((*net)->addr[i] != NULL) {
      closesfd(*net, i);
    }
  }
  
  delqueue((*net)->q_head);
  free((*net)->fd);
  for(int i=(*net)->n-1; i>=0; i--) {
    free((*net)->addr[i]);
  }
  free((*net)->addr);
  
  free(*net);
  net = NULL;
}

int acceptsfd(net_t *net, int fd_i)
{
  int sd;
  int addr_len;
  struct sockaddr_in *addr;

  RPTEST(addr = malloc(sizeof(struct sockaddr_in)), -1);
  addr_len = sizeof(*addr);
  RTEST(sd = accept(net->fd[fd_i],
        (struct sockaddr*)(addr),
        (socklen_t*)(&addr_len)), -1);

  return addfd(net, sd, addr);
}

int closesfd(net_t *net, int fd_i)
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
  //new nfds
  return 0;
}

int creatsfd(net_t *net, char *ip)
{
  int sd;
  struct sockaddr_in *addr;
  
  RTEST(sd = socket(AF_INET, SOCK_STREAM, 0), -1);
  RPTEST(addr = malloc(sizeof(struct sockaddr_in)), -1);
  
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(55555);
  if(ip == NULL) {
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    RTEST(bind(sd, (struct sockaddr*)(addr),
          sizeof(struct sockaddr_in)), -1);
    RTEST(listen(sd, 10), -1);
  } else {
    addr->sin_addr.s_addr = inet_addr(ip);
    RTEST(connect(sd, (struct sockaddr*)(addr),
          sizeof(struct sockaddr_in)), -1);
  }

  return addfd(net, sd, addr);
}

int addfd(net_t *net, int sd, struct sockaddr_in *addr)
{
  if(net->n == net->size) {
    RTEST(resize(net, 1), -1);
  }
  net->fd[net->n] = sd;
  net->addr[net->n] = addr;
  net->n++;
  if(sd > net->nfds) {
    net->nfds = sd;
  }
  return net->n - 1;
}

int sendfmsg(int sfd, str_t *msg)
{
  char *fmsg;
  RPTEST(fmsg = malloc((msg->size+1)*sizeof(char)), -1);
  fmsg[0] = msg->size;
  memcpy(fmsg+1, msg->str, msg->size);
  return send(sfd, fmsg, msg->size+1, 0);
}

int recmsg(int sfd, str_t **msg)
{
  char chr;
  recv(sfd, &chr, 1, 0);
  if(((*msg) = malloc(sizeof(str_t))) == NULL 
  || ((*msg)->str = malloc(chr*sizeof(char))) == NULL)
  {
    (*msg)->size = -1;
  } else {
    (*msg)->size = recv(sfd, (*msg)->str, chr, 0);
    //if msg->size != chr => error
  }
  return (*msg)->size;
}

int selectfd(net_t *net, int usec)
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
    acceptsfd(net, 0);
  }
  for(int i=net->n-1; i>0; i--) {
    if(FD_ISSET(net->fd[i], &(net->set))) {
      msg_queue_t *msg;
      RPTEST(msg = malloc(sizeof(msg_queue_t)), -1);
      RPTEST(msg->msg = malloc(sizeof(str_t)), -1);
      recmsg(net->fd[i], &msg->msg);
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
      msg->next = NULL;
  
      if(net->q_head == NULL) {
        net->q_head = msg;
        net->q_tail = msg;
      }
      else {
        net->q_tail->next = msg;
        net->q_tail = msg;
      }
//    wprintf(L"r%d: %s\n", i, net->q_tail->msg);
    }
  }
  return s;
}

int resize(net_t *net, int expand)
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

str_t *mapserialize(map_t *map) {
  str_t *str;

  RPTEST(str = malloc(sizeof(str_t)), NULL);
  str->size = 2*(map->w+1)*map->h;
  RPTEST(str->str = malloc((str->size+2)*sizeof(char)), NULL);
  str->str[0] = map->h;
  str->str[1] = map->w + 1;
  for(int i=str->size/2-1; i>=0; i--) {
    str->str[2*i+2] = map->data[0][i]%256;
    str->str[2*i+3] = map->data[0][i]/256;
//  if(str->str[2*i+1] == '\0') str->str[2*i+1] = '-';
//  else                        str->str[2*i+1] = '+';
//  if(str->str[2*i  ] == '\0') str->str[2*i  ] = '-';
//  else                        str->str[2*i  ] = '+';
  }
//str->str[str->size-1] = '\0';
  return str;
}

map_t *mapdeserialize(str_t *str) {
  map_t *map;
  int h = str->str[0];
  int w = str->str[1];

  RPTEST(map = mapget(0, h, w-1), NULL);
  for(int i=w*h-1; i>=0; i--) {
    map->data[0][i] = str->str[2*i+3]*256 + str->str[2*i+2];
  }
  return map;
}

