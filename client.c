#define _XOPEN_SOURCE_EXTENDED 

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
  net_t *net;
  ERPTEST(net = getnett(), -1);
  ERTEST(creatsfd(net, "0.0.0.0"), -1);
  printf("accept\n");
  sleep(1);
  printf("end\n");
  delnett(&net);
  return 0;
} 
