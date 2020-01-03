#include "graphic.h"

#define DATA area->map->data
#define MAP area->map
#define POS_INC(val, max)\
  {\
    val++;\
    if(val >= max)\
    {\
      val -= max;\
    }\
  }
#define POS_DEC(val, max)\
  {\
    val--;\
    if(val < 0)\
    {\
      val += max;\
    }\
  }


void wininit()
{
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  refresh();
  clear();
  noecho();
  curs_set(0);
}

area_t *winget(int h, int w)
{
  area_t *area = NULL;
  area = malloc(sizeof(area_t));
  area->pos_x = 0;
  area->pos_y = 0;
  area->map = mapcfill(h, w, L'█');//█
  area->win = newwin(h, w, 0, 0);
  return area;
}

area_t *winmaze(int h, int w)
{
  h = (h/3)*3;
  w = (w/3)*3;
  area_t *area = NULL;
  area = malloc(sizeof(area_t));
  area->pos_x = 0;
  area->pos_y = 0;
  RPTEST(area->map = mapcfill(h, w, L'█'), NULL);
  area->win = newwin(h, w, 0, 0);
  genmaze(area->map);
  return area;
}

void winupdate(area_t *area)
{
  int ys;
  wchar_t wchr;
  
  for(int y=0; y<MAP->h; y++)
  {
    ys = (y + area->pos_y)%MAP->h;
    wchr = DATA[ys][area->pos_x];
    DATA[ys][area->pos_x] = L'\0';
    mvwaddwstr(area->win, y, MAP->w - area->pos_x, DATA[ys]);
    DATA[ys][area->pos_x] = wchr;
    mvwaddwstr(area->win, y, 0, DATA[ys] + area->pos_x);
  }
  wrefresh(area->win);
}

map_t *wingetfrag(char dir, map_t *map, int *pos_y, int *pos_x, int h, int w)
{
  map_t *frag = NULL;
  if(((*pos_y) < 0) || ((*pos_y) + h > map->h) 
  || ((*pos_x) < 0) || ((*pos_x) + w > map->w))
  {
//  wprintf(L"%d %d\t%d>=%d\t%d>=%d\n", *pos_y, *pos_x, (*pos_y)+h, map->h, (*pos_x)+w, map->w);
    return NULL;
  }

  switch(dir)
  {
  case 'w':
    if((frag = mapgetf(map, --(*pos_y), (*pos_x), 1, w)) == NULL)
    {
      (*pos_y)++;
    }
    break;
  case 's':
    if((frag = mapgetf(map, h + (*pos_y)++, (*pos_x), 1, w)) == NULL)
    {
      (*pos_y)--;
    }
    break;
  case 'a':
    if((frag = mapgetf(map, (*pos_y), --(*pos_x), h, 1)) == NULL)
    {
      (*pos_x)++;
    }
    break;
  case 'd':
    if((frag = mapgetf(map, (*pos_y), w + (*pos_x)++, h, 1)) == NULL)
    {
      (*pos_x)--;
    }
    break;
  case 'c':
    frag = mapgetf(map, (*pos_y), (*pos_x), h, w);
    break;
  }
  return frag;
}

int winmov(area_t *area, char dir, map_t *frag)
{
  switch(dir)
  {
  case 'w':
    POS_DEC(area->pos_y, MAP->h)
    if(mapsetf(area->map, frag, area->pos_y, area->pos_x) < 0)
    {
      POS_INC(area->pos_y, MAP->h);
      return -1;
    }
    break;
  case 's':
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    POS_INC(area->pos_y, MAP->h);
    break;
  case 'a':
    POS_DEC(area->pos_x, MAP->w)
    if(mapsetf(area->map, frag, area->pos_y, area->pos_x) < 0)
    {
      POS_INC(area->pos_x, MAP->w);
      return -1;
    }
    break;
  case 'd':
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    POS_INC(area->pos_x, MAP->w);
    break;
  case 'c':
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    break;
  }
  winupdate(area);
  return 0;
}

void winplayer(area_t *area, int y, int x)
{
  mvwaddwstr(area->win, y, x, L"@");
  wrefresh(area->win);
}

void windel(area_t *area)
{
  mapfree(&(area->map));
  delwin(area->win);
  endwin();
}
