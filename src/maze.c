#include "../include/maze.h"

struct MazeTile 
{
  int id;
  struct MazeTile *prev;
  struct MazeTile *next;
};

void 
connectpaths(Map              *mp, 
             struct MazeTile **tiles, 
             int               y, 
             int               x, 
             int               sy, 
             int               sx)
{
  //return if already connected (no loops)
  if(tiles[y][x].id == tiles[y+sy][x+sx].id)
  {
    return;
  }

  //ptr->id > ptr2->id
  struct MazeTile *ptr  = &tiles[y   ][x   ];
  struct MazeTile *ptr2 = &tiles[y+sy][x+sx];
  //
  if(tiles[y][x].id < tiles[y+sy][x+sx].id)
  {
    ptr  = &tiles[y+sy][x+sx];
    ptr2 = &tiles[y   ][x   ];
  }

  //propagate
  ptr->id = ptr2->id;
  ptr2 = ptr->next;
  while(ptr2 != ptr)
  {
    ptr2->id = ptr->id;
    ptr2 = ptr2->next;
  }

  //connect
  tiles[y+sy][x+sx].next->prev =  tiles[y   ][x   ].prev; 
  tiles[y   ][x   ].prev->next =  tiles[y+sy][x+sx].next;
  tiles[y+sy][x+sx].next       = &tiles[y   ][x   ];
  tiles[y   ][x   ].prev       = &tiles[y+sy][x+sx];
  //
  mp->data[ y    *3+(sy+1)][ x    *3+(sx+1)] = L' ';
  mp->data[(y+sy)*3-(sy-1)][(x+sx)*3-(sx-1)] = L' ';
}

void 
choosepath(Map              *mp, 
           struct MazeTile **tiles,
           int               y, 
           int               x, 
           int               h, 
           int               w)
{
  switch(rand()%4) {
    case 0: if(y > 0  ) { connectpaths(mp, tiles, y, x, -1,  0); break; }
    case 1: if(x < w-1) { connectpaths(mp, tiles, y, x,  0, +1); break; }
    case 2: if(x > 0  ) { connectpaths(mp, tiles, y, x,  0, -1); break; }
    case 3: if(y < h-1) { connectpaths(mp, tiles, y, x, +1,  0); break; }
    default:if(y > 0  ) { connectpaths(mp, tiles, y, x, -1,  0); break; }
  }
}

void 
genmaze(Map *mp)
{
  struct MazeTile **tiles = NULL;
  wchar_t empty = L' ';
  int h = mp->h/3;
  int w = mp->w/3;

  PTEST(tiles = malloc((mp->h/3)*sizeof(struct MazeTile*)));
  for(int y=h-1; y>=0; y--)
  {
    PTEST(tiles[y] = malloc((mp->w/3)*sizeof(struct MazeTile)));
    for(int x=w-1; x>=0; x--)
    {
      tiles[y][x].id = y*w + x + 1;
      tiles[y][x].prev = &tiles[y][x];
      tiles[y][x].next = &tiles[y][x];
      mp->data[y*3+1][x*3+1] = empty;
    }
  }
  tiles[h/2][w/2].id = 0;

  for(int y=h-1; y>=0; y--)
  {
    for(int x=w-1; x>=0; x--)
    {
      choosepath(mp, tiles, y, x, h, w);
    }
  }

  int connected = 0;
  while(connected < 1)
  {
    for(int i=0, x=0, y=0; i <100; i++)
    {
      y = rand()%h;
      x = rand()%w;
      choosepath(mp, tiles, y, x, h, w);
    }

    connected = 1;
    for(int y=h-1; y>=0; y--)
    {
      for(int x=w-1; x>=0; x--)
      {
        if(tiles[y][x].id != 0)
        {
          connected = 0;
          break;
        }
      }
      if(connected == 0)
      {
        break;
      }
    }
  }

  for(int y=h-1; y>=0; y--)
  {
    free(tiles[y]);
  }
  free(tiles);
}

void 
randmap(Map *map)
{
  int y, x;
  int n = 0.5*(map->h-1)*(map->w-1);
  for(int i=0; i<n; i++)
  {
    y = rand()%map->h;
    x = rand()%map->w;
    map->data[y][x] = L' ';
  }
}

void 
frame(Map *map, int style)
{
  if(style < 0 || style >= 9)
  {
    return;
  }
  wchar_t frame_el[6][9] = {
    // 0     1     2     3     4     5     6     7     8 
    {L'┌', L'┏', L'╔', L'╭', L'┼', L' ', L'░', L'▒', L'▓'}, //0 ul
    {L'┐', L'┓', L'╗', L'╮', L'┼', L' ', L'░', L'▒', L'▓'}, //1 ur
    {L'└', L'┗', L'╚', L'╰', L'┼', L' ', L'░', L'▒', L'▓'}, //2 bl
    {L'┘', L'┛', L'╝', L'╯', L'┼', L' ', L'░', L'▒', L'▓'}, //3 br
    {L'─', L'━', L'═', L'─', L'─', L' ', L'░', L'▒', L'▓'}, //4 -c
    {L'│', L'┃', L'║', L'│', L'│', L' ', L'░', L'▒', L'▓'}};//5 m-
    
  for(int y=map->h-2; y>0; y--)
  {
    map->data[y][       0] = frame_el[5][style];
    map->data[y][map->w-1] = frame_el[5][style];
  }
  for(int x=map->w-2; x>0; x--)
  {
    map->data[       0][x] = frame_el[4][style];
    map->data[map->h-1][x] = frame_el[4][style];
  }
  map->data[       0][       0] = frame_el[0][style];
  map->data[       0][map->w-1] = frame_el[1][style];
  map->data[map->h-1][       0] = frame_el[2][style];
  map->data[map->h-1][map->w-1] = frame_el[3][style];
}

void 
checkers(Map *map)
{
  for(int y=map->h-1; y>=0; y--)
  {
    for(int x=map->w-1; x>=0; x--)
    {
      if((y+x)%2)
      {
        map->data[y][x] = L' ';
      }
    }
  }
}

void 
grid(Map *map)
{
  for(int y=map->h-1; y>=0; y--)
  {
    for(int x=map->w-1; x>=0; x--)
    {
      if(y%2 + x%2 == 2)
      {
        map->data[y][x] = L' ';
      }
    }
  }
}
