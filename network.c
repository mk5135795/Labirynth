#include "network.h"

net_t *getnett()
{
  net_t *tmp;
  RPTEST(tmp = malloc(sizeof(net_t)), NULL);
  tmp->fd = NULL;
  tmp->addr = NULL;
  tmp->n = 0;
  tmp->size = 0;
  return tmp;
}

void delnett(net_t **net)
{
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
    addr->sin_addr.s_addr = INADDR_ANY;
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
  return net->n - 1;
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
