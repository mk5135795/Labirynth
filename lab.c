#include "lab.h"

area_t *winmaze(int h, int w)
{
  area_t *area = NULL;
  h = (h/3)*3;
  w = (w/3)*3;
  RPTEST(area = winget(h, w), NULL);

  genmaze(area->map);
  RTEST(mapctl(&area->map, MAP_SCALE, 2, 2), NULL);
  frame(area->map, 1);
  
  return area;
}

int movpl(char dir, point_t *pos, map_t *map)
{
  switch(dir)
  {
  case 'w':
    if(1)
    {
      //sendmsg(sd, 
    }
    break;
  }

  return 0;
}
