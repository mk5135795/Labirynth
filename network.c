#include "network.h"

net_t *getnett()
{
  net_t *tmp;
  RPTEST(tmp = malloc(sizeof(net_t)), NULL);
  tmp->fd = NULL;
  tmp->addr = NULL;
  tmp->n = 0;
  tmp->size = 0;
  tmp->nfds = -1;
  tmp->q_head = NULL;
  tmp->q_tail = NULL;
  FD_ZERO(&(tmp->set));
  return tmp;
}

void delqueue(msg_queue_t *queue)
{
  msg_queue_t *ptr = queue;
  while(ptr != NULL)
  {
    ptr = queue->next;
    free(queue);
    queue = ptr;
  }
}

void delnett(net_t **net)
{
  for(int i=(*net)->n-1; i>=0; i--)
  {
    if((*net)->addr[i] != NULL)
    {
      closesfd(*net, i);
    }
  }
  delqueue((*net)->q_head);
  free((*net)->fd);
  for(int i=(*net)->n-1; i>=0; i--)
  {
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
  if(fd_i < net->n)
  {
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
  if(ip == NULL)
  {
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    RTEST(bind(sd, (struct sockaddr*)(addr),
          sizeof(struct sockaddr_in)), -1);
    RTEST(listen(sd, 10), -1);
  }
  else
  {
    addr->sin_addr.s_addr = inet_addr(ip);
    RTEST(connect(sd, (struct sockaddr*)(addr),
          sizeof(struct sockaddr_in)), -1);
  }

  return addfd(net, sd, addr);
}

int addfd(net_t *net, int sd, struct sockaddr_in *addr)
{
  if(net->n == net->size)
  {
    RTEST(resize(net, 1), -1);
  }
  net->fd[net->n] = sd;
  net->addr[net->n] = addr;
  net->n++;
  if(sd > net->nfds)
  {
    net->nfds = sd;
  }
  return net->n - 1;
}

int sendfmsg(int sfd, char *msg)
{
  char size = sizeof(msg);
  char *fmsg;
  RPTEST(fmsg = malloc((size+1)*sizeof(char)), -1);
  fmsg[0] = size;
  memcpy(fmsg+1, msg, size);
  return send(sfd, fmsg, size+1, 0);
}

int recmsg(int sfd, char **msg)
{
  char chr;
  recv(sfd, &chr, 1, 0);
  RPTEST((*msg) = malloc(chr*sizeof(char)), -1);
  return recv(sfd, (*msg), chr, 0);
}

int selectfd(net_t *net, int usec)
{
  int s;
  for(int i=net->n-1; i>=0; i--)
  {
    FD_SET(net->fd[i], &(net->set));
  }
  if(usec > 0)
  {
    static struct timeval timeout;
    timeout.tv_usec = usec;
    RTEST(s = select(net->nfds+1, &net->set, NULL, NULL, &timeout), -1);
    if(s == 0)
    {
      return 0;
    }
  }
  else
  {
    RTEST(s = select(net->nfds+1, &net->set, NULL, NULL, NULL), -1);
  }
  if(FD_ISSET(net->fd[0], &(net->set)))
  {
    acceptsfd(net, 0);
  }
  for(int i=net->n-1; i>0; i--)
  {
    if(FD_ISSET(net->fd[i], &(net->set)))
    {
      msg_queue_t *msg;
      RPTEST(msg = malloc(sizeof(msg_queue_t)), -1);
      recmsg(net->fd[i], &(msg->msg));
//    if(recmsg(net->fd[i], &(msg->msg)) == 0)
//    {
//      closesfd(net, i);
//      free(msg);
//      continue;
//    }
      if(msg->msg == NULL)
      {
        free(msg);
        continue;
      }
      msg->fd_i = i;
      msg->next = NULL;
  
      if(net->q_head == NULL)
      {
        net->q_head = msg;
        net->q_tail = msg;
      }
      else
      {
        net->q_tail->next = msg;
        net->q_tail = msg;
      }
      printf("r%d: %s\n", i, net->q_tail->msg);
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
  for(; i<net->size; i++)
  {
    tmp_fd[i] = net->fd[i];
    tmp_addr[i] = net->addr[i];
  }
  net->size += expand;
  for(; i<net->size; i++)
  {
    tmp_fd[i] = 0;
    tmp_addr[i] = NULL;
  }
  free(net->fd);
  free(net->addr);
  net->fd = tmp_fd;
  net->addr = tmp_addr;
  return 0;
}
