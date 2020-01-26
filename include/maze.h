/**
 *  \file maze.h
 *  \file maze.c
 */

#ifndef MAZE_H
#define MAZE_H

#include "macro.h"
#include "map.h"

#define FRAME_STYLES_NUM 9

//void _connect_paths(Map *map, struct MazeTile **tiles, int y, int x, 
//  int sy, int sx, char loop);
//void _connect_tiles(Map *map, struct MazeTile **tiles, int y, int x, 
//  int h, int w, char dir, char loops);
int generate_maze(Map *map);
void add_frame(Map *map, int style);
//int _pattern_rand(int y, int x);
//int _pattern_checkers(int y, int x);
//int _pattern_grid(int y, int x);
void insert_pattern(Map *map, wchar_t wchr, char pattern);

#endif /*MAZE_H*/
