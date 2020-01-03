#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <wchar.h>
#include "macro.h"

typedef struct {
    int h;
    int w;
    wchar_t **data;
} map_t;

map_t *mapcreat(int h, int w);          
map_t *mapcpy(map_t *mpp);               
map_t *mapcfill(int h, int w, wchar_t bg);
void mapfill(map_t *map, wchar_t bg);
map_t *mapcscale(map_t *map, int scale_y, int scale_x);
int mapscale(map_t *map, int scale_y, int scale_x);
map_t *mapgetf(map_t *map, int y, int x, int h, int w); 
int mapsetf(map_t *map, map_t *frag, int pos_y, int pos_x);
void mapfree(map_t **mp);
void mapprint(map_t *map);

#endif /*MAP_H*/
