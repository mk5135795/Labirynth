/**
 *  \file map.h
 */

#ifndef MAP_H
#define MAP_H

#include <locale.h>
#include <string.h>

#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>
#include "macro.h"

#define MAP_COPY  0x01
#define MAP_SCALE 0x02
#define MAP_FILL  0x04

typedef struct
{
    wchar_t **data;
    int       h;
    int       w;
} Map;

/**
 *  \brief
 *    Creates new map structure.
 *
 *  \details
 *    Variable arguments(varg) for set flags:\n 
 *    + (MAP_COPY, MAP_SCALE):  [Map *old_map, int scale_y, int scale_x],
 *    + (MAP_FILL):             [int h, int w, wchar_t fill_character].
 *
 *  \param flags
 *    Defines behavior and required variable arguments,\n
 *    available flags:
 *    + MAP_COPY  - create copy of existing Map\n
 *                  if set, the first varg must be pointer to existing Map,
 *                  otherwise the first two vargs have to be height and width
 *                  of new Map
 *    + MAP_SCALE - scales created copy\n
 *                  ignored if MAP_COPY is not set,
 *                  otherwise the second and third vargs describes scaling
 *    + MAP_FILL  - fills created Map with given symbol\n
 *                  ignored if MAP_COPY is set,
 *                  otherwise the third varg is symbol used to fill.
 *
 *  \return 
 *    NULL on error, pointer to new structure otherwise.
 */
Map *map_create(int flags, ...);

/**
 *  \brief
 *    Modyfies given Map structure.
 *
 *  \details
 *    Variable args order:\n
 *    [scale_y, scale_x, fill_symbol].
 *
 *  \param map
 *    Structure to modify.
 *  \param flags
 *    Defines operation, avilable flags:
 *    + MAP_SCALE - resizes map acording to first two vargs
 *    + MAP_FILL  - fills map with symbol passed as
 *                  third varg if MAP_SCALE is set,
 *                  first varg otherwise.
 *
 *  \return
 *    -1 on error, 0 otherwise.
 *
 *  \todo
 *    split to map_scale and map_fill
 */
int map_op(Map **map, int flags, ...);

/**
 *  \brief
 *    Frees memory associated with structure 
 *    and setting given pointer to NULL.
 *
 *  \param map
 *    Structure to deallocate.
 */
void map_delete(Map **mp);

/**
 *  \brief
 *    Creates copy of choosen area from given Map structure.
 *
 *  \param map
 *    Structure from which area will be chosen.
 *  \param y, x
 *    Area's starting point.
 *  \param h, w
 *    Area's dimension.
 *
 *  \return
 *    NULL on error, pointer to new structure otherwise.
 */
Map *map_get_area(Map *map, int y, int x, int h, int w); 

/**
 *  \brief
 *    Replaces area in structure with given fragment.
 *
 *  \details
 *    Fragment is deleted after operation,\n
 *    if fragment is invalid (dim < 0 or frag.dim > map.dim)
 *    it won't be applied but still will be deleted.
 *
 *  \param map
 *    Structure to modify.
 *  \param frag
 *    Area that will be inserted.
 *  \param pos_y, pos_x
 *    Point of insertion.
 *
 *  \return
 *    -1 on error, 0 otherwise.
 */
int map_set_area(Map *map, Map *frag, int pos_y, int pos_x);

/**
 *  \brief
 *    Prints map to stdout.
 *
 *  \details
 *    Replaces spaces with '-' and other symbols with '+'.
 *
 *  \param map
 *    Structure to print.
 *
 *  \todo
 *    mov to map_test, rm <stdio.h>
 */
void map_print(Map *map);

#endif /*MAP_H*/
