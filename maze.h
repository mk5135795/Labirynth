#ifndef MAZE_H
#define MAZE_H

#include "macro.h"
#include "map.h"

//connectpaths
//choosepath
void genmaze(map_t *mp);
void frame(map_t *map, int style);
void randmap(map_t *map);
void checkers(map_t *map);
void grid(map_t *map);

#endif /*MAZE_H*/
