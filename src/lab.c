#include "../include/lab.h"

Area *winmaze(int h, int w)
{
  Area *area = NULL;
  h = (h/3)*3;
  w = (w/3)*3;
  RPTEST(area = win_create(h, w), NULL);

  generate_maze(area->map);
  RTEST(map_op(&area->map, MAP_SCALE, 2, 2), NULL);
  add_frame(area->map, 1);
  
  return area;
}

int movpl(char dir, Point *pos, Map *map)
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
