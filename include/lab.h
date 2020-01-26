/**
 *  \file lab.h
 *  \file lab.c
 */

#ifndef LAB_H
#define LAB_H

#include "macro.h"
#include "graphic.h"
#include "network.h"

Area *winmaze(int h, int w);
//int clientmsg();
//int servermsg();
int movpl(char dir, Point *pos, Map *map);
//add exit
//queue handler
//player

#endif /*LAB_H*/
