#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"
#include "map.h"

int main(int argc, char *argv[])
{
  point_t client_size = {24, 80};
  point_t server_size = {2*24, 2*78};
  point_t pos = {0, 0};
  area_t *g_area;
  area_t *area;
  
  wininit();
  
  ERPTEST(g_area = winmaze(server_size.y, server_size.x), -1);
  ERPTEST(area = winget(client_size.y, client_size.x), -1);
  ERTEST(mapscale(&(g_area->map), 2, 2), -1);
  
  if(argc == 1)
  {
    ERTEST(winmov(area, 'c', wingetfrag('c', g_area->map, &pos, client_size)), -1);

    char dir;
    map_t *frag;
    while((dir = wgetch(area->win)) != 'q')    
    {
      if((frag = wingetfrag(dir, g_area->map, &pos, client_size)) != NULL)
      {
        RTEST(winmov(area, dir, frag), -1);
      }
    }
  }
  else
  {
    winupdate(g_area);
    wgetch(g_area->win);
//  char dir;
//  while((dir = wgetch(g_area->win)) != 'q')    
//  {
//    RTEST(winscroll(area, dir), -1);
//  }
  }
  
  windel(g_area);
  windel(area);
  return 0;
}
