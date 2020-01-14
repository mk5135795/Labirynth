#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>
#include "macro.h"

#define MAP_COPY  0x01
#define MAP_SCALE 0x02
#define MAP_FILL  0x04

typedef struct {
    int h;
    int w;
    wchar_t **data;
} map_t;

map_t *mapget(int flags, ...);
int mapctl(map_t **map, int flags, ...);
void mapfree(map_t **mp);

map_t *mapgetf(map_t *map, int y, int x, int h, int w); 
int mapsetf(map_t *map, map_t *frag, int pos_y, int pos_x);

void mapprint(map_t *map);

#endif /*MAP_H*/
