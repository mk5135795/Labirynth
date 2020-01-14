#define PORT 55555
#define SAFE(v) \
  if((v) < 0) \
{ \
  wprintf(L"error - line %d in  %s\n", __LINE__, __FILE__); \
}

#include <stdio.h>//printf
#include <unistd.h>//sleep
#include "network.h"
#include "map.h"
#include "maze.h"

int main(int argc, char* argv[]) 
{
  net_t *net;
  map_t *map;
  str_t *msg;

  ERPTEST(map = mapget(MAP_FILL, 4, 4, L'#'), -1);
  checkers(map);
  mapprint(map);
  ERPTEST(net = getnett(), -1);

  ERTEST(creatsfd(net, "0.0.0.0"), -1);
  wprintf(L"accept\n");
  sleep(1);
//ERTEST(sendfmsg(net->fd[0], "test"), -1);
  msg = mapserialize(map);
//wprintf(L"m[%d]: [%s]\n", msg->size, msg->str);
  ERTEST(sendfmsg(net->fd[0], msg), -1);
  wprintf(L"end\n");
  delnett(&net);
  return 0;
} 
