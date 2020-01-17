#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"
#include "lab.h"

#include "map.h"

int main(int argc, char *argv[])
{
//Map *map1 = NULL;
//Map *map2 = NULL;
//ERPTEST(map1 = mapget(MAP_FILL, 20, 20, L'#'), -1);
//checkers(map1);
//ERPTEST(map2 = mapget(MAP_COPY|MAP_SCALE, map1, 2, 2), -1);

//wininit();
//Area *are = NULL;
//ERPTEST(are = winget(40, 40), -1);
//mapfree(&are->map);
//are->map = map2;
//winupdate(are);
//sleep(5);

//mapfree(&map1);
//mapfree(&map2);
//windel(are);
//return 0;
  Point client_size = {24, 80};
  Point server_size = {1*24, 1*78};
  Point pos = {0, 0};
  Area *g_area;
  Area *area;
  
  win_init();
  
  ERPTEST(g_area = winmaze(server_size.y, server_size.x), -1);
  ERPTEST(area = win_create(client_size.y, client_size.x), -1);
  
  if(argc == 1)
  {
    ERTEST(win_move(area, 'c', win_get_area('c', g_area->map, &pos, client_size)), -1);

    char dir;
    Map *frag;
    while((dir = wgetch(area->win)) != 'q')    
    {
      if((frag = win_get_area(dir, g_area->map, &pos, client_size)) != NULL)
      {
        RTEST(win_move(area, dir, frag), -1);
      }
    }
  }
  else
  {
    win_update(g_area);
    
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
      win_scroll(g_area, dir);
    }
  }
  
  win_delete(g_area);
  win_delete(area);
  return 0;
}
