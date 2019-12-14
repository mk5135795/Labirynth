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
} area_t;

area_t *winget(int h, int w);
void wininit();
int winupdate(area_t *area);
void winplayer(area_t *area, int y, int x);
void windel(area_t *area);

#endif /*GRAPHIC_H*/
