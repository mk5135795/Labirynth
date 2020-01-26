#include "../include/maze.h"

#define _connect_tiles_rnd(m, t, y, x, h, w, l) \
  _connect_tiles(m, t, y, x, h, w, rand()%4, l)

#define _connect_rand_tiles(m, t, h, w, l) \
  _connect_tiles(m, t, rand()%h, rand()%w, h, w, rand()%4, l)


struct MazeTile
{
  int id;
  struct MazeTile *prev;
  struct MazeTile *next;
};

void 
_connect_paths(Map              *map, 
               struct MazeTile **tiles, 
               int               y, 
               int               x, 
               int               sy, 
               int               sx,
               char              loop)
{
  char propagate = 1;
  
  //return if loops not allowed and already connected
  if(tiles[y][x].id == tiles[y+sy][x+sx].id) {
    if(loop == 1){
      propagate = 0;
    }
    else {
      return;
    }
  }

  struct MazeTile *ptr  = &tiles[y   ][x   ];
  struct MazeTile *ptr2 = &tiles[y+sy][x+sx];
  
  //ptr->id > ptr2->id
  if(tiles[y][x].id < tiles[y+sy][x+sx].id) {
    ptr  = &tiles[y+sy][x+sx];
    ptr2 = &tiles[y   ][x   ];
  }

  if(propagate == 1) {
    //propagate
    ptr->id = ptr2->id;
    ptr2 = ptr->next;
    while(ptr2 != ptr) {
      ptr2->id = ptr->id;
      ptr2 = ptr2->next;
    }

    //connect
    tiles[y+sy][x+sx].next->prev =  tiles[y   ][x   ].prev; 
    tiles[y   ][x   ].prev->next =  tiles[y+sy][x+sx].next;
    tiles[y+sy][x+sx].next       = &tiles[y   ][x   ];
    tiles[y   ][x   ].prev       = &tiles[y+sy][x+sx];
  }
  
  map->data[ y    *3+(sy+1)][ x    *3+(sx+1)] = L' ';
  map->data[(y+sy)*3-(sy-1)][(x+sx)*3-(sx-1)] = L' ';

//if(propagate == 0) {
//  map->data[ y    *3+(sy+1)][ x    *3+(sx+1)] = L'+';
//  map->data[(y+sy)*3-(sy-1)][(x+sx)*3-(sx-1)] = L'+';
//}
}

void 
_connect_tiles(Map              *map, 
               struct MazeTile **tiles,
               int               y, 
               int               x, 
               int               h, 
               int               w,
               char              dir,
               char              loops)
{
  switch(dir) 
  {
  case 0: 
    if(y > 0  ) { _connect_paths(map, tiles, y, x, -1,  0, loops); return; }
  case 1: 
    if(x < w-1) { _connect_paths(map, tiles, y, x,  0, +1, loops); return; }
  case 2: 
    if(x > 0  ) { _connect_paths(map, tiles, y, x,  0, -1, loops); return; }
  case 3: 
    if(y < h-1) { _connect_paths(map, tiles, y, x, +1,  0, loops); return; }
  }
  //ensure connection
  _connect_tiles(map, tiles, y, x, h, w, 0, loops);
}

int 
generate_maze(Map *mp)
{
  struct MazeTile **tiles = NULL;
  wchar_t empty = L' ';
  int h = mp->h/3;
  int w = mp->w/3;

  RPTEST(tiles = malloc((mp->h/3)*sizeof(struct MazeTile*)), -1);
  for(int y=h-1; y>=0; y--) {
    if((tiles[y] = malloc((mp->w/3)*sizeof(struct MazeTile))) == NULL) {
      while(++y < h) {
        free(tiles[y]);
      }
      return -1;
    }
    for(int x=w-1; x>=0; x--) {
      tiles[y][x].id = y*w + x + 1;
      tiles[y][x].prev = &tiles[y][x];
      tiles[y][x].next = &tiles[y][x];
      mp->data[y*3+1][x*3+1] = empty;
    }
  }
  tiles[h/2][w/2].id = 0;

  for(int y=h-1; y>=0; y--) {
  for(int x=w-1; x>=0; x--) {
    _connect_tiles_rnd(mp, tiles, y, x, h, w, 0);
  }}
  for(int i=h*w/2; i>=0; i--) {
    _connect_rand_tiles(mp, tiles, h, w, 0);
  }
  for(int i=h*w/4; i>=0; i--) {
    _connect_rand_tiles(mp, tiles, h, w, 1);
  }

//for(int y=h-1; y>=0; y--) {
//for(int x=w-1; x>=0; x--) {
//  if(tiles[y][x].id != 0) {
//    mp->data[y*3+1][x*3+1] = L'a' + tiles[y][x].id%26;
//  }
//}}

  for(int y=h-1; y>=0; y--) {
  for(int x=w-1; x>=0; x--) {
    if(tiles[y][x].id != 0) {
      _connect_tiles(mp, tiles, y, x, h, w, 0, 0);
    }
  }}

  for(int y=h-1; y>=0; y--) {
    free(tiles[y]);
  }
  free(tiles);

  return 0;
}

void 
add_frame(Map *map, 
          int  style)
{
  if(style < 0 || style >= FRAME_STYLES_NUM) {
    return;
  }
  wchar_t frame_el[6][FRAME_STYLES_NUM] = {
    // 0     1     2     3     4     5     6     7     8 
    {L'┌', L'┏', L'╔', L'╭', L'┼', L' ', L'░', L'▒', L'▓'}, //0 ul
    {L'┐', L'┓', L'╗', L'╮', L'┼', L' ', L'░', L'▒', L'▓'}, //1 ur
    {L'└', L'┗', L'╚', L'╰', L'┼', L' ', L'░', L'▒', L'▓'}, //2 bl
    {L'┘', L'┛', L'╝', L'╯', L'┼', L' ', L'░', L'▒', L'▓'}, //3 br
    {L'─', L'━', L'═', L'─', L'─', L' ', L'░', L'▒', L'▓'}, //4 -c
    {L'│', L'┃', L'║', L'│', L'│', L' ', L'░', L'▒', L'▓'}};//5 m-
    
  for(int y=map->h-2; y>0; y--) {
    map->data[y][       0] = frame_el[5][style];
    map->data[y][map->w-1] = frame_el[5][style];
  }
  for(int x=map->w-2; x>0; x--) {
    map->data[       0][x] = frame_el[4][style];
    map->data[map->h-1][x] = frame_el[4][style];
  }
  map->data[       0][       0] = frame_el[0][style];
  map->data[       0][map->w-1] = frame_el[1][style];
  map->data[map->h-1][       0] = frame_el[2][style];
  map->data[map->h-1][map->w-1] = frame_el[3][style];
}

int _pattern_rand(    int y, int x) { return rand()%2;                 }
int _pattern_checkers(int y, int x) { return (x+y)%2;                  }
int _pattern_grid(    int y, int x) { return (x%2 == 1) || (y%2 == 1); }

void 
insert_pattern(Map    *map, 
               wchar_t wchr, 
               char    pattern)
{
  int (*fun)(int, int);
  switch(pattern)
  {
  case 0: fun = _pattern_rand;     break;
  case 1: fun = _pattern_checkers; break;
  case 2: fun = _pattern_grid;     break;
  }

  for(int y=map->h-1; y>=0; y--) {
  for(int x=map->w-1; x>=0; x--) {
    if(fun(y, x) == 1) {
      map->data[y][x] = wchr;
    }
  }}
}
