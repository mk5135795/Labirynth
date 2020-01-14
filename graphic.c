#include "graphic.h"

#define POS area->pos
#define MAP area->map
#define DATA area->map->data
#define POS_INC(val, max) { if(++val >= max) { val -= max; }}
#define POS_DEC(val, max) { if(--val <    0) { val += max; }}

/*
 *  Function: wininit
 *  -----------------
 *  setup ncurses
 *  must be called before winget
 */
void wininit() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  refresh();
  clear();
  noecho();
  curs_set(0);
}

/*
 *  Function: winget
 *  ----------------
 *  creates new area_t structure
 *
 *  h, w: size of window and map_t
 *
 *  returns:  NULL if error, pointer to created area_t structure otherwise
 */
area_t *winget(int h, int w) {
  area_t *area = NULL;
  RPTEST(area = malloc(sizeof(area_t)), NULL);

  area->pos = (point_t){0, 0};
  RPTEST(area->map = mapget(MAP_FILL, h, w, L'█'), NULL);//█
  RPTEST(area->win = newwin(h, w, 0, 0), NULL);
  return area;
}

/*
 *  Function: winupdate
 *  -------------------
 *  draws map_t structure on WINDOW
 *
 *  area: structure storing area and window
 */
void winupdate(area_t *area) {
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

/*
 *  Function: wingetfrag
 *  --------------------
 *  returns fragment needed to move subarea with given size and position
 *  in given direction, fragment describes newly uncovered area
 *
 *  dir:  move direction, acceptable directions: w, a, s, d, c
 *        w, a, s, d returns line in right direction,
 *        c returns whole subarea,
 *        other values will be ignored
 *  map:  main area
 *  pos:  position of subarea
 *  size: size of subarea
 *
 *  returns:  fragment needed for move operation
 */
map_t *wingetfrag(char dir, map_t *map, point_t *pos, point_t size) {
  map_t *frag = NULL;
  if(((pos->y) < 0) || ((pos->y) + size.y > map->h) 
  || ((pos->x) < 0) || ((pos->x) + size.x > map->w))
  {
    return NULL;
  }

  switch(dir)
  {
  case 'w':
    if((frag = mapgetf(map, --pos->y, pos->x, 1, size.x))  == NULL) {
      pos->y++;
    }
    break;
  case 'a':
    if((frag = mapgetf(map, pos->y, --pos->x, size.y , 1))  == NULL) {
      pos->x++;
    }
    break;
  case 's':
    if((frag = mapgetf(map, size.y + pos->y, pos->x, 1, size.x)) != NULL) {
      pos->y++;
    }
    break;
  case 'd':
    if((frag = mapgetf(map, pos->y, size.x + pos->x, size.y , 1)) != NULL) {
      pos->x++;
    }
    break;
  case 'c':
    frag = mapgetf(map, pos->y, pos->x, size.y , size.x);
    break;
  }
  return frag;
}

/*
 *  Function: winmov
 *  ----------------
 *  shift area and applies given fragment
 *
 *  area: area to modify
 *  dir:  movement direction
 *  frag: area of newly uncover fragment
 *
 *  return: -1 if error, 0 otherwise
 */
int winmov(area_t *area, char dir, map_t *frag) {
  switch(dir)
  {
  case 'w':
    POS_DEC(POS.y, MAP->h)
    if(mapsetf(MAP, frag, POS.y, POS.x) < 0) {
      POS_INC(POS.y, MAP->h);
      return -1;
    }
    break;
  case 'a':
    POS_DEC(POS.x, MAP->w)
    if(mapsetf(MAP, frag, POS.y, POS.x) < 0) {
      POS_INC(POS.x, MAP->w);
      return -1;
    }
    break;
  case 's':
    RTEST(mapsetf(MAP, frag, POS.y, POS.x), -1);
    POS_INC(POS.y, MAP->h);
    break;
  case 'd':
    RTEST(mapsetf(MAP, frag, POS.y, POS.x), -1);
    POS_INC(POS.x, MAP->w);
    break;
  case 'c':
    RTEST(mapsetf(MAP, frag, POS.y, POS.x), -1);
    break;
  }
  winupdate(area);
  return 0;
}

/*
 *  Function: winscroll
 *  -------------------
 *  shifting and displaying given area
 *
 *  area: area to scroll
 *  dir:  movement direction 
 */
void winscroll(area_t *area, char dir) {
  switch(dir)
  {
  case 'w': POS_DEC(POS.y, MAP->h); break;
  case 'a': POS_DEC(POS.x, MAP->w); break;
  case 's': POS_INC(POS.y, MAP->h); break;
  case 'd': POS_INC(POS.x, MAP->w); break;
  }
  winupdate(area);
}

/*
 *  Function: windel
 *  ----------------
 *  deallocates area_t structure and sets given pointer to NULL
 *
 *  area: structure to free
 */
void windel(area_t *area)
{
  mapfree(&(MAP));
  delwin(area->win);
  endwin();
}
