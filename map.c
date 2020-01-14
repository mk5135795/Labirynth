#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include "map.h"

/*
 *  Function:  mapget
 *  ----------------
 *  creates new map structure
 *  variable arguments(varg): 
 *    (MAP_COPY, MAP_SCALE):  [map_t *old_map, int scale_y, int scale_x]
 *    (MAP_FILL):             [int h, int w, wchar_t fill_character]
 *
 *  flags:    defines behavior and required variable arguments, 
 *            available flags:
 *    MAP_COPY  - create copy of existing map_t
 *                if set, the first varg must be pointer to existing map_t,
 *                otherwise the first two vargs have to be height and width 
 *                of new map_t
 *    MAP_SCALE - scales created copy
 *                ignored if MAP_COPY is not set,
 *                otherwise the second and third vargs describes scaling
 *    MAP_FILL  - fills created map_t with given symbol
 *                ignored if MAP_COPY is set,
 *                otherwise the third varg is symbol used to fill
 *
 *  returns:  NULL on error, pointer to new structure otherwise
 */
map_t *mapget(int flags, ...)
{
  va_list args;
  int sy = 1;
  int sx = 1;
  wchar_t bg = L' ';
  map_t *old_map = NULL;
  map_t *map = NULL;

  RPTEST(map = malloc(sizeof(map_t)), NULL);

  //var args
  va_start(args, flags);
  if(flags & MAP_COPY) {
    old_map = va_arg(args, map_t*);
    map->h = old_map->h;
    map->w = old_map->w;
    
    if(flags & MAP_SCALE) {
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      map->h *= sy;
      map->w *= sx;
    }
  } else {
    map->h = va_arg(args, int);
    map->w = va_arg(args, int);
    
    if(flags & MAP_FILL) {
      bg = va_arg(args, wchar_t); 
    }
  }
  va_end(args);
  
  //init
  RPTEST(map->data = malloc(map->h*sizeof(wchar_t*)), NULL);
  RPTEST(map->data[0] = malloc(map->h*(map->w+1)*sizeof(wchar_t)), NULL);
  
  //fill
  for(int y=0; y<map->h; y += sy) {
    map->data[y] = map->data[0] + (y*(map->w+1));
    map->data[y][map->w] = L'\0';
    if(old_map != NULL) {
      for(int x=map->w-1; x>=0; x--) {
        map->data[y][x] = old_map->data[y/sy][x/sx];
      }
    } else {
      for(int x=map->w-1; x>=0; x--) {
        map->data[y][x] = bg;
      }
    }
    for(int my=sy-1; my>0; my--) {
      map->data[y+my] = map->data[0] + ((y+my)*(map->w+1));
//    strcpy(map->data[y+my], map->data[y]);
      for(int t=map->w; t>=0; t--) {
        map->data[y+my][t] = map->data[y][t];
      }
    }
  }
  return map;
}

/*
 *  Function:  mapctl
 *  -----------------
 *  modyfies given map_t structure, variable args format:
 *    [scale_y, scale_x, fill_symbol]
 *
 *  map:    structure to modify
 *  flags:  defines operation, avilable flags:
 *    MAP_SCALE - resizes map acording to first two vargs
 *    MAP_FILL  - fills map with symbol passed as
 *                third varg if MAP_SCALE is set,
 *                first varg otherwise
 *
 *  returns:  -1 on error, 0 otherwise
 */
int mapctl(map_t **map, int flags, ...)
{
  va_list args;
  wchar_t bg;
  int sy;
  int sx;

  va_start(args, flags);
  if (flags & MAP_SCALE) {
    map_t *nmap = NULL;
    
    if(flags & MAP_FILL) {
      bg = va_arg(args, wchar_t);
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      va_end(args);

      RPTEST(nmap = mapget(MAP_FILL|MAP_SCALE, bg, (*map)->h*sy, (*map)->w*sx), -1);
    } else {
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      va_end(args);
      
      RPTEST(nmap = mapget(MAP_COPY|MAP_SCALE, *map, sy, sx), -1);
    }
    mapfree(map);
    *map = nmap;
  } 
  else if(flags & MAP_FILL) {
    bg = va_arg(args, wchar_t);
    va_end(args);
    
    for(int y=(*map)->h-1; y>=0; y--) {
    for(int x=(*map)->w-1; x>=0; x--) {
      (*map)->data[y][x] = bg;
    }}
  }
  return 0;
}

/*
 *  Function:  mapfree
 *  ------------------
 *  frees memory associated with structure 
 *  and setting given pointer to NULL
 *
 *  map: structure to deallocate
 */
void mapfree(map_t **mp)
{
  if(mp == NULL) {
    return;
  }

  free((*mp)->data[0]);
  free((*mp)->data);
  *mp = NULL;
}

/*
 *  Function: mapgetf
 *  -----------------
 *  creates copy of choosen area from given map_t structure
 *
 *  map:  structure from which area will be chosen
 *  y, x: area's starting point
 *  h, w: area's dimension
 *
 *  returns:  NULL on error, pointer to new structure otherwise
 */
map_t *mapgetf(map_t *map, int y, int x, int h, int w)
{
  map_t *frag = NULL;

  if((x < 0) || (w <= 0) || (x+w > map->w)
  || (y < 0) || (h <= 0) || (y+h > map->h))
  {
    return NULL;
  }

  RPTEST(frag = mapget(0, h, w), NULL);
  for(int j=h-1; j>=0; j--) {
  for(int i=w-1; i>=0; i--) {
    frag->data[j][i] = map->data[y+j][x+i];
  }}
  return frag;
}

/*
 *  Function: mapsetf
 *  -----------------
 *  replaces area in structure with given fragment, 
 *  fragment is deleted afterwards
 *  if fragment is invalid (dim < 0 or frag.dim > map.dim)
 *  fragment won't be applied but will still be deleted
 *
 *  map:          structure to modify
 *  frag:         area that will be inserted
 *  pos_y, pos_x: point of insertion
 *
 *  returns:  -1 on error, 0 otherwise
 */
int mapsetf(map_t *map, map_t *frag, int pos_y, int pos_x)
{
  if((frag->w <= 0) || (map->w  <= 0) || (frag->w > map->w)
  || (frag->h <= 0) || (map->h  <= 0) || (frag->h > map->h))
  {
    mapfree(&frag);
    return -1;
  }
  
  for(int j=frag->h-1; j>=0; j--) {
  for(int i=frag->w-1; i>=0; i--) {
    map->data[(pos_y+j)%map->h][(pos_x+i)%map->w] = frag->data[j][i];
  }}
  mapfree(&frag);
  return 0;
}

/*
 *  Function: mapprint
 *  ------------------
 *  prints map to stdout, 
 *  replacing spaces with '-' and other symbols with '+'
 *
 *  map: structure to print
 */
void mapprint(map_t *map) {
  for(int j=0; j<map->h; j++) {
  for(int i=0; i<map->w; i++) {
    if(map->data[j][i] == L' ') 
         { wprintf(L"-"); } 
    else { wprintf(L"#"); }
  } wprintf(L"\n");
  } wprintf(L"\n");
}

