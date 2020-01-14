#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"
#include "lab.h"

#include "map.h"

int main(int argc, char *argv[])
{
//map_t *map1 = NULL;
//map_t *map2 = NULL;
//ERPTEST(map1 = mapget(MAP_FILL, 20, 20, L'#'), -1);
//checkers(map1);
//ERPTEST(map2 = mapget(MAP_COPY|MAP_SCALE, map1, 2, 2), -1);

//wininit();
//area_t *are = NULL;
//ERPTEST(are = winget(40, 40), -1);
//mapfree(&are->map);
//are->map = map2;
//winupdate(are);
//sleep(5);

//mapfree(&map1);
//mapfree(&map2);
//windel(are);
//return 0;
  point_t client_size = {24, 80};
  point_t server_size = {1*24, 1*78};
  point_t pos = {0, 0};
  area_t *g_area;
  area_t *area;
  
  wininit();
  
  ERPTEST(g_area = winmaze(server_size.y, server_size.x), -1);
  ERPTEST(area = winget(client_size.y, client_size.x), -1);
  
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
    
    char dir;
    int style = 0;
    while((dir = wgetch(g_area->win)) != 'q')    
    {
      if(dir == 'z')
      {
        frame(g_area->map, --style);
      }
      else if(dir == 'x')
      {
        frame(g_area->map, ++style);
      }
      RTEST(winscroll(g_area, dir), -1);
    }
  }
  
  windel(g_area);
  windel(area);
  return 0;
}
