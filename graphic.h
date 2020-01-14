#ifndef GRAPHIC_H
#define GRAPHIC_H

#define _XOPEN_SOURCE_EXTENDED

#include <wchar.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "macro.h"
#include "map.h"
#include "maze.h"

typedef struct {
  int y;
  int x;
} point_t;

typedef struct {
    WINDOW *win;
    map_t *map;
    point_t pos;
} area_t;

void wininit();
area_t *winget(int h, int w);
void winupdate(area_t *area);
map_t *wingetfrag(char dir, map_t *map, point_t *pos, point_t size);
int winmov(area_t *area, char dir, map_t *map);
void winscroll(area_t *area, char dir);
void windel(area_t *area);

#endif /*GRAPHIC_H*/
