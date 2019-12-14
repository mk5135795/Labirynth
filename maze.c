#include "maze.h"

struct tile_t {
    int id;
    struct tile_t *prev;
    struct tile_t *next;
};

void connectpaths(map_t *mp, struct tile_t **tiles, 
        int y, int x, int sy, int sx)
{
    if(tiles[y][x].id == tiles[y+sy][x+sx].id)
        return;

    struct tile_t *ptr = &tiles[y][x];
    struct tile_t *ptr2 = &tiles[y+sy][x+sx];

    if(tiles[y][x].id < tiles[y+sy][x+sx].id)
    {
        ptr = &tiles[y+sy][x+sx];
        ptr2 = &tiles[y][x];
    }

    ptr->id = ptr2->id;
    ptr2 = ptr->next;
    while(ptr2 != ptr)
    {
        ptr2->id = ptr->id;
        ptr2 = ptr2->next;
    }

    tiles[y+sy][x+sx].next->prev = tiles[y   ][x   ].prev; 
    tiles[y   ][x   ].prev->next = tiles[y+sy][x+sx].next;
    tiles[y+sy][x+sx].next       = &tiles[y   ][x   ];
    tiles[y   ][x   ].prev       = &tiles[y+sy][x+sx];
    
    mp->data[ y    *3+(sy+1)][ x    *3+(sx+1)] = L' ';
    mp->data[(y+sy)*3-(sy-1)][(x+sx)*3-(sx-1)] = L' ';
}

void choosepath(map_t *mp, struct tile_t **tiles,
        int y, int x, int h, int w)
{
    switch(rand()%4) {
    case 0: if(y > 0  ) { connectpaths(mp, tiles, y, x, -1,  0); break; }
    case 1: if(x < w-1) { connectpaths(mp, tiles, y, x,  0, +1); break; }
    case 2: if(x > 0  ) { connectpaths(mp, tiles, y, x,  0, -1); break; }
    case 3: if(y < h-1) { connectpaths(mp, tiles, y, x, +1,  0); break; }
    default:if(y > 0  ) { connectpaths(mp, tiles, y, x, -1,  0); break; }
    }
}

void genmaze(map_t *mp)
{
    struct tile_t **tiles = NULL;
    wchar_t empty = L' ';
    int h = mp->h/3;
    int w = mp->w/3;

    PTEST(tiles = malloc((mp->h/3)*sizeof(struct tile_t*)));
    for(int y=h-1; y>=0; y--)
    {
        PTEST(tiles[y] = malloc((mp->w/3)*sizeof(struct tile_t)));
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
                    //mp->data[y*3+1][x*3+1] = L'#';
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

void rwalk3(map_t *mp)
{
    wchar_t wchr = L' ';
    int len = 0;
    
    int l = 100;
    int nl = 10;
    int n = 8;
    int (*p)[3];

    srand(rand());
    PTEST(p = malloc(n*sizeof(int[3])));
    p[0][0] = mp->h/2-3;
    p[0][1] = mp->w/2;
    p[0][2] = 0;
    p[1][0] = mp->h/2;
    p[1][1] = mp->w/2+3;
    p[1][2] = 0;
    p[2][0] = mp->h/2+3;
    p[2][1] = mp->w/2;
    p[2][2] = 0;
    p[3][0] = mp->h/2;
    p[3][1] = mp->w/2-3;
    p[3][2] = 0;
    while(l)
    {
        if(l%nl == 0)
        {
            p[4][0] = p[0][0];
            p[4][1] = p[0][1];
            p[4][2] = p[0][2];
            p[5][0] = p[1][0];
            p[5][1] = p[1][1];
            p[5][2] = p[1][2];
            p[6][0] = p[2][0];
            p[6][1] = p[2][1];
            p[6][2] = p[2][2];
            p[7][0] = p[3][0];
            p[7][1] = p[3][1];
            p[7][2] = p[3][2];
        }
        for(int k=7; k>=0; k--)
        {
            if(rand()%2 == 0){
                 p[k][2]= (p[k][2] +1) %4;
            } else {
                p[k][2] = (p[k][2] -1 +4) %4;
            }

            len = (rand() % 50)+5;
            switch(p[k][2])
            {
                case 0:
                    if(p[k][0]-len > 0)
                    {
                        for(int m=len; m>0; m--)
                        {
                            p[k][0]--;
                            mp->data[p[k][0]][p[k][1]]=wchr;
                        }
                        break;
                    }
                case 1:
                    if(p[k][1]+len < mp->w-1)
                    {
                        for(int m=len; m>0; m--)
                        {
                            p[k][1]++;
                            mp->data[p[k][0]][p[k][1]]=wchr;
                        }
                        break;
                    }
                case 2:
                    if(p[k][0]+len < mp->h-1)
                    {
                        for(int m=len; m>0; m--)
                        {
                            p[k][0]++;
                            mp->data[p[k][0]][p[k][1]]=wchr;
                        }
                        break;
                    }
                case 3:
                    if(p[k][1]-len > 0)
                    {
                        for(int m=len; m>0; m--)
                        {
                            p[k][1]--;
                            mp->data[p[k][0]][p[k][1]]=wchr;
                        }
                        break;
                    }
            }
        }
        l--;
    }
    for(int y=-3; y<4; y++)
    {
        for(int x=-3; x<4; x++)
        {
            mp->data[mp->h/2 +y][mp->w/2 +x]=L'#';//wchr;
        }
    }
    free(p);
}

void randmap(map_t *map)
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

void checkers(map_t *map)
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

void grid(map_t *map)
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
