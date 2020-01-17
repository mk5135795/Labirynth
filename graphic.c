/// \file graphic.c
#include "graphic.h"

#define POS area->pos
#define MAP area->map
#define DATA area->map->data
#define POS_INC(val, max) { if(++val >= max) { val -= max; } }
#define POS_DEC(val, max) { if(--val <    0) { val += max; } }

void 
win_init() 
{
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  refresh();
  clear();
  noecho();
  curs_set(0);
}

Area *
win_create(int h, 
           int w) 
{
  Area *area = NULL;
  RPTEST(area = malloc(sizeof(Area)), NULL);

  area->pos = (Point){0, 0};
  RPTEST(area->map = map_create(MAP_FILL, h, w, L'█'), NULL);//█
  RPTEST(area->win = newwin(h, w, 0, 0), NULL);
  return area;
}

void 
win_update(Area *area) 
{
  int ys;
  wchar_t wchr;
  
  for(int y=0; y<MAP->h; y++) {
    ys = (y + POS.y)%MAP->h;
    
    mvwaddwstr(area->win, y, 0, DATA[ys] + POS.x);
    
    wchr = DATA[ys][POS.x];
    DATA[ys][POS.x] = L'\0';
    mvwaddwstr(area->win, y, MAP->w - POS.x, DATA[ys]);
    DATA[ys][POS.x] = wchr;
  }
  wrefresh(area->win);
}

Map *
win_get_area(char   dir, 
             Map   *map, 
             Point *pos, 
             Point  size) 
{
  Map *frag = NULL;
  if(((pos->y) < 0) || ((pos->y) + size.y > map->h) 
  || ((pos->x) < 0) || ((pos->x) + size.x > map->w))
  {
    return NULL;
  }

  switch(dir)
  {
  case 'w':
    if((frag = map_get_area(map, --pos->y, pos->x, 1, size.x))  == NULL) {
      pos->y++;
    }
    break;
  case 'a':
    if((frag = map_get_area(map, pos->y, --pos->x, size.y , 1))  == NULL) {
      pos->x++;
    }
    break;
  case 's':
    if((frag = map_get_area(map, size.y + pos->y, pos->x, 1, size.x)) != NULL) {
      pos->y++;
    }
    break;
  case 'd':
    if((frag = map_get_area(map, pos->y, size.x + pos->x, size.y , 1)) != NULL) {
      pos->x++;
    }
    break;
  case 'c':
    frag = map_get_area(map, pos->y, pos->x, size.y , size.x);
    break;
  }
  return frag;
}

int 
win_move(Area *area, 
         char  dir, 
         Map  *frag) 
{
  switch(dir)
  {
  case 'w':
    POS_DEC(POS.y, MAP->h)
    if(map_set_area(MAP, frag, POS.y, POS.x) < 0) {
      POS_INC(POS.y, MAP->h);
      return -1;
    }
    break;
  case 'a':
    POS_DEC(POS.x, MAP->w)
    if(map_set_area(MAP, frag, POS.y, POS.x) < 0) {
      POS_INC(POS.x, MAP->w);
      return -1;
    }
    break;
  case 's':
    RTEST(map_set_area(MAP, frag, POS.y, POS.x), -1);
    POS_INC(POS.y, MAP->h);
    break;
  case 'd':
    RTEST(map_set_area(MAP, frag, POS.y, POS.x), -1);
    POS_INC(POS.x, MAP->w);
    break;
  case 'c':
    RTEST(map_set_area(MAP, frag, POS.y, POS.x), -1);
    break;
  }
  win_update(area);
  return 0;
}

void 
win_scroll(Area *area, 
           char  dir) 
{
  switch(dir)
  {
  case 'w': POS_DEC(POS.y, MAP->h); break;
  case 'a': POS_DEC(POS.x, MAP->w); break;
  case 's': POS_INC(POS.y, MAP->h); break;
  case 'd': POS_INC(POS.x, MAP->w); break;
  }
  win_update(area);
}

void 
win_delete(Area *area)
{
  map_delete(&(MAP));
  delwin(area->win);
  endwin();
}
