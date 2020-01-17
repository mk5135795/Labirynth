#include <stdio.h>
#include "map.h"

Map *
map_create(int flags, 
           ...)
{
  va_list args;
  int sy = 1;
  int sx = 1;
  wchar_t bg = L' ';
  Map *old_map = NULL;
  Map *map = NULL;

  RPTEST(map = malloc(sizeof(Map)), NULL);

  //var args
  va_start(args, flags);
  if(flags & MAP_COPY) {
    old_map = va_arg(args, Map*);
    map->h = old_map->h;
    map->w = old_map->w;
    
    if(flags & MAP_SCALE) {
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      map->h *= sy;
      map->w *= sx;
    }/* scale */
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

int 
map_op(Map **map, 
       int   flags, 
       ...)
{
  va_list args;
  wchar_t bg;
  int sy;
  int sx;

  va_start(args, flags);
  if (flags & MAP_SCALE) {
    Map *nmap = NULL;
    
    if(flags & MAP_FILL) {
      bg = va_arg(args, wchar_t);
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      va_end(args);

      RPTEST(nmap = map_create(MAP_FILL|MAP_SCALE, bg, (*map)->h*sy, (*map)->w*sx), -1);
    } else {
      sy = va_arg(args, int);
      sx = va_arg(args, int);
      va_end(args);
      
      RPTEST(nmap = map_create(MAP_COPY|MAP_SCALE, *map, sy, sx), -1);
    }
    map_delete(map);
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

void 
map_delete(Map **mp)
{
  if(mp == NULL) {
    return;
  }

  free((*mp)->data[0]);
  free((*mp)->data);
  *mp = NULL;
}

Map *
map_get_area(Map *map, 
             int  y, 
             int  x, 
             int  h, 
             int  w)
{
  Map *frag = NULL;

  if((x < 0) || (w <= 0) || (x+w > map->w)
  || (y < 0) || (h <= 0) || (y+h > map->h))
  {
    return NULL;
  }

  RPTEST(frag = map_create(0, h, w), NULL);
  for(int j=h-1; j>=0; j--) {
  for(int i=w-1; i>=0; i--) {
    frag->data[j][i] = map->data[y+j][x+i];
  }}
  return frag;
}

int 
map_set_area(Map *map, 
             Map *frag, 
             int  pos_y, 
             int  pos_x)
{
  if((frag->w <= 0) || (map->w  <= 0) || (frag->w > map->w)
  || (frag->h <= 0) || (map->h  <= 0) || (frag->h > map->h))
  {
    map_delete(&frag);
    return -1;
  }
  
  for(int j=frag->h-1; j>=0; j--) {
  for(int i=frag->w-1; i>=0; i--) {
    map->data[(pos_y+j)%map->h][(pos_x+i)%map->w] = frag->data[j][i];
  }}
  map_delete(&frag);
  return 0;
}

void 
map_print(Map *map) 
{
  for(int j=0; j<map->h; j++) {
  for(int i=0; i<map->w; i++) {
    if(map->data[j][i] == L' ') { wprintf(L"-"); } 
    else                        { wprintf(L"#"); }
  } wprintf(L"\n");
  } wprintf(L"\n");
}

