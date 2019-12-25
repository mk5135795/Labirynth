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

int winupdate(area_t *area)
{
  int ys;
  int xs;
  wchar_t wchr;
  
  xs = (area->pos_x + MAP->w)%MAP->w;
  for(int y=0; y<MAP->h; y++)
  {
    ys = (y + area->pos_y + MAP->h)%MAP->h;
    wchr = DATA[ys][MAP->w - xs];
    DATA[ys][MAP->w - xs] = L'\0';
    mvwaddwstr(area->win, y, xs, DATA[ys]);
    DATA[ys][MAP->w - xs] = wchr;
    mvwaddwstr(area->win, y, 0, DATA[ys] + MAP->w - xs);
  }
  wrefresh(area->win);
  return 0;
}

void winmov(area_t *area, char dir)
{
  switch(dir)
  {
  case 'w':
    area->pos_y--;
    if(area->pos_y <= -1*MAP->h)
      area->pos_y = 0;
    break;
  case 's':
    area->pos_y++;
    if(area->pos_y >= MAP->h)
      area->pos_y = 0;
    break;
  case 'a':
    area->pos_x--;
    if(area->pos_x <= -1*MAP->w)
      area->pos_x = 0;
    break;
  case 'd':
    area->pos_x++;
    if(area->pos_x >= MAP->w)
      area->pos_x = 0;
    break;
  }
  winupdate(area);
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
