/**
 *  \file graphic.h
 *  \file graphic.c
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#ifndef _XOPEN_SOURCE_EXTENDED
  #define _XOPEN_SOURCE_EXTENDED
#endif

#include <wchar.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "macro.h"
#include "map.h"
#include "maze.h"

typedef struct 
{
  int y;
  int x;
} Point;

typedef struct
{
    WINDOW *win;
    Map    *map;
    Point   pos;
} Area;

/**
 *  \brief
 *    Setup ncurses.
 *  
 *  \details
 *    Must be called before winget.
 */
void win_init();

/**
 *  \brief
 *    Creates new Area structure.
 *
 *  \param h, w
 *    Size of window and Map.
 *
 *  \return
 *    NULL if error, pointer to created Area structure otherwise.
 */
Area *win_create(int h, int w);

/**
 *  \brief
 *    Draws Map structure on WINDOW.
 *
 *  \param area
 *    Structure storing area and window.
 */
void win_update(Area *area);

/**
 *  \brief
 *    Returns fragment needed to move subarea with given size and position
 *    in given direction, fragment describes newly uncovered area.
 *
 *  \param dir
 *    Move direction, acceptable directions: w, a, s, d, c
 *    + w, a, s, d returns line in right direction,
 *    + c returns whole subarea,
 *    other values will be ignored.
 *  \param  map
 *    Main area.
 *  \param  pos
 *    Position of subarea.
 *  \param  size
 *    Size of subarea.
 *
 *  \return
 *    Fragment needed for move operation.
 */
Map *win_get_area(char dir, Map *map, Point *pos, Point size);

/**
 *  \brief
 *    Shift area and applies given fragment.
 *
 *  \param area
 *    Area to modify.
 *  \param dir
 *    Movement direction.
 *  \param frag
 *    Area of newly uncover fragment.
 *
 *  \return
 *    -1 if error, 0 otherwise.
 */
int win_move(Area *area, char dir, Map *map);

/**
 *  \brief
 *    Shifting and displaying given area.
 *
 *  \param area
 *    Area to scroll.
 *  \param dir
 *    Movement direction .
 */
void win_scroll(Area *area, char dir);

/**
 *  \brief
 *    Deallocates Area structure and sets given pointer to NULL.
 *
 *  \param area
 *    Structure to deallocate.
 */
void win_delete(Area *area);

#endif /*GRAPHIC_H*/
