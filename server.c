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
  int server_i;
  net_t *net;
  map_t *map;

  wprintf(L"a\n");
  ERPTEST(map = mapget(MAP_FILL, 4, 4, L'#'), -1);
  grid(map);
  mapprint(map);
  wprintf(L"b\n");
  ERPTEST(net = getnett(), 0);
  wprintf(L"c\n");
  
  ERTEST(server_i = creatsfd(net, NULL), -1);
  ERTEST(acceptsfd(net, server_i), -1);
  wprintf(L"accept\n");
  sleep(1);
  ERTEST(selectfd(net, 0), -1);
  wprintf(L"select\n");
//wprintf(L"m[%d]: %s\n", sizeof(net->q_tail->msg)/sizeof(net->q_tail->msg[0]), net->q_tail->msg);
  for(int i=0; i<net->q_tail->msg->size-1; i++) {
    wprintf(L"%c", net->q_tail->msg->str[i]);
  }
  wprintf(L"\n");
  map_t *t = mapdeserialize(net->q_tail->msg);
  mapprint(t);

  ERTEST(closesfd(net, 1), -1);
  wprintf(L"end\n");
  delnett(&net);
  return 0;
} 
