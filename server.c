#include <stdio.h>//printf
#include "./include/network.h"
#include "./include/map.h"
#include "./include/maze.h"

//msg_req + sem
//msg_resp + sem

int main(int argc, char* argv[]) 
{
  Net *net;
  Map *map;

  ERPTEST(map = map_create(MAP_FILL, 4, 4, L'#'), -1);
  grid(map);
  map_print(map);
  
  ERPTEST(net = net_create(), 0);
  ERTEST(net_add_server(net), -1);
  ERTEST(net_listen(net, 0), -1);
  ERTEST(net_listen(net, 0), -1);
  
  Map *t = map_deserialize(net->q_head->msg);
  map_print(t);

  ERTEST(net_close_sfd(net, 1), -1);
  net_delete(&net);
  return 0;
} 
