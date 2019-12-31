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
    WINDOW *win;
    map_t *map;
    int pos_x;
    int pos_y;
} area_t;

void wininit();
area_t *winget(int h, int w);
area_t *winmaze(int h, int w);
void winupdate(area_t *area);
map_t *wingetfrag(char dir, map_t *map, int pos_y, int pos_x, int h, int w);
int  winmov(area_t *area, char dir, map_t *map);
void winplayer(area_t *area, int y, int x);
void windel(area_t *area);

#endif /*GRAPHIC_H*/
