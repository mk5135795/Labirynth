#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"
#include "map.h"

int g_w = 60;
int g_h = 21;
int p_x = 0;
int p_y = 0;
area_t *g_area;

int main(int argc, char *argv[])
{
  wininit();
  
  area_t *area;
  ERPTEST(g_area = winmaze(g_h, g_w), -1);
  ERPTEST(area = winget(g_h/2, g_w/2), -1);
  
  if(argc == 1)
  {
    ERTEST(winmov(area, 'c', getf('c', g_h/2, g_w/2)), -1);

    char dir;
    map_t *frag;
    while((dir = wgetch(area->win)) != 'q')    
    {
      if((frag = getf(dir, g_h/2, g_w/2)) != NULL)
      {
        RTEST(winmov(area, dir, frag), -1);
      }
    }
  }
  else
  {
    winupdate(g_area);
    wgetch(g_area->win);
  }
  
  windel(g_area);
  windel(area);
  return 0;
}
