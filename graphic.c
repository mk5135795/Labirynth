#include "graphic.h"

#define DATA area->map->data
#define MAP area->map

area_t *winget(int h, int w)
{
    h = (h/3)*3;
    w = (w/3)*3;
    area_t *area = NULL;
    area = malloc(sizeof(area_t));
    area->map = mapcfill(h, w, L'█', 0);//█
//  genmaze(area->map);
//  mapprint(area->map);
//  map_t *tmp = mapscale(area->map, 2, 2);
//  mapfree(&(area->map));
//  area->map = tmp;
    area->win = newwin(h, w, 0, 0);
    return area;
}

void wininit()
{
    initscr();
    cbreak();
    refresh();
    clear();
    noecho();
    curs_set(0);
}

int winupdate(area_t *area)
{
    for(int y=area->map->h-1; y>=0; y--)
    {
        mvwaddwstr(area->win, y, 0, DATA[y]);
    }
    wrefresh(area->win);
    return 0;
}

void winplayer(area_t *area, int y, int x)
{
  //DATA[y][x] = L'@';
  //winupdate(area);
  mvwaddwstr(area->win, y, x, L"@");
  wrefresh(area->win);
}

void windel(area_t *area)
{
    mapfree(&(area->map));
    delwin(area->win);
    endwin();
}
