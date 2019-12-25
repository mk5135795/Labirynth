#define PORT 55555
#define SAFE(v) \
  if((v) < 0) \
{ \
  wprintf(L"error - line %d in  %s\n", __LINE__, __FILE__); \
}

#include <stdio.h>//printf
#include <unistd.h>//sleep
#include "network.h"

int main(int argc, char* argv[]) 
{
  int server_i;
  net_t *net;
  ERPTEST(net = getnett(), 0);
  ERTEST(server_i = creatsfd(net, NULL), -1);
  ERTEST(acceptsfd(net, server_i), -1);
  printf("accept\n");
  sleep(1);
  ERTEST(selectfd(net, 0), -1);
  printf("select\n");
  ERTEST(closesfd(net, 1), -1);
  printf("end\n");
  delnett(&net);
  return 0;
} 
