#include <stdio.h>//printf
#include "./include/network.h"
#include "./include/map.h"
#include "./include/maze.h"

int main(int argc, char* argv[]) 
{
  Net *net;
  Map *map;
  Msg *msg;

  ERPTEST(map = map_create(MAP_FILL, 4, 4, L'#'), -1);
  checkers(map);
  map_print(map);
  ERPTEST(net = net_create(), -1);

  ERTEST(net_add_client(net, "0.0.0.0"), -1);
  msg = map_serialize(map);
  ERTEST(net_send_msg(net->fd[0], msg), -1);
  net_delete(&net);
  return 0;
} 
