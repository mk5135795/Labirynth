#ifndef MAZE_H
#define MAZE_H

#include "macro.h"
#include "map.h"

//connectpaths
//choosepath
void genmaze(Map *mp);
void frame(Map *map, int style);
void randmap(Map *map);
void checkers(Map *map);
void grid(Map *map);

#endif /*MAZE_H*/
