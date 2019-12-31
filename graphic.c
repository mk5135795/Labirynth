#include "graphic.h"

#define DATA area->map->data
#define MAP area->map

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
//wresize(area->win, map->h, map->w);
  return area;
}

void winupdate(area_t *area)
{
  int ys;
  wchar_t wchr;
  
  for(int y=0; y<MAP->h; y++)
  {
    ys = (y + area->pos_y + MAP->h)%MAP->h;
    {
      wchr = DATA[ys][area->pos_x];
      DATA[ys][area->pos_x] = L'\0';
      mvwaddwstr(area->win, y, MAP->w - area->pos_x, DATA[ys]);
      DATA[ys][sarea->pos_x] = wchr;
      mvwaddwstr(area->win, y, 0, DATA[ys] + area->pos_x);
    }
  }
  wrefresh(area->win);
}

int winmov(area_t *area, char dir, map_t *frag)
{
  switch(dir)
  {
  case 'w':
    area->pos_y--;
    if(area->pos_y < 0)
      area->pos_y += MAP->h;
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    break;
  case 's':
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    area->pos_y++;
    if(area->pos_y >= MAP->h)
      area->pos_y -= MAP->h;
    break;
  case 'a':
    area->pos_x--;
    if(area->pos_x < 0)
      area->pos_x += MAP->w;
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    break;
  case 'd':
    RTEST(mapsetf(area->map, frag, area->pos_y, area->pos_x), -1);
    area->pos_x++;
    if(area->pos_x >= MAP->w)
      area->pos_x -= MAP->w;
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
