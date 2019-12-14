#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <wchar.h>
#include "macro.h"

typedef struct {
    int h;
    int w;
    wchar_t **data;
    int **color;
} map_t;

map_t *mapcreat(int h, int w);                              //4
map_t *mapcpy(map_t *mp);                                   //23
map_t *mapcfill(int h, int w, wchar_t bg, int bg_col);      //47
void mapfill(map_t *mp, wchar_t bg, int bg_color);          //71
map_t *mapgetf(map_t *mp, int y, int x, int h, int w);      //83
int mapsetf(map_t *mp, map_t *frag, int pos_y, int pos_x);  //104
map_t *mapscale(map_t *mp, int scale_y, int scale_x);       //126
void mapshift(map_t *map, int sh_y, int sh_x);              //141
void mapfree(map_t **mp);                                   //165
wchar_t *mapserialize(map_t *map);
void mapprint(map_t *map);
map_t *mapdeserialize(wchar_t *d, int h, int w);

#endif /*MAP_H*/
