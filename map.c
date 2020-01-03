#include <stdlib.h>
#include "map.h"

#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>

map_t *mapcreat(int h, int w)
{
    map_t *map = NULL;
  
    RPTEST(map = malloc(sizeof(map_t)), NULL);
    RPTEST(map->data = malloc(h*sizeof(wchar_t*)), NULL);
    map->h = h;
    map->w = w;

    RPTEST(map->data[0] = malloc(h*(w+1)*sizeof(wchar_t)), NULL);  
    map->data[0][w] = L'\0';
    for(int y=h-1; y>0; y--)
    {
        map->data[y] = map->data[0] + (y*(w+1));
        map->data[y][w] = L'\0';
    }

    return map;
}

map_t *mapcpy(map_t *map)
{
    map_t *n_map = NULL;
    
    RPTEST(n_map = mapcreat(map->h, map->w), NULL);
    for(int y=map->h-1; y>=0; y--)
    {
        for(int x=map->w-1; x>=0; x--)
        {
            n_map->data[y][x] = map->data[y][x];
        }
    }

    return n_map;
}

map_t *mapcfill(int h, int w, wchar_t bg)
{
    map_t *map = NULL;
    
    RPTEST(map = mapcreat(h, w), NULL);
    
    for(int y=h-1; y>=0; y--)
    {
        for(int x=w-1; x>=0; x--)
        {
            map->data[y][x] = bg;
        }
    }
    return map;
}

void mapfill(map_t *map, wchar_t bg)
{
    for(int y=map->h-1; y>=0; y--)
    {
        for(int x=map->w-1; x>=0; x--)
        {
            map->data[y][x] = bg;
        }
    }
}

map_t *mapcscale(map_t *map, int scale_y, int scale_x)
{
    map_t *n_map;
    RPTEST(n_map = mapcreat(map->h*scale_y, map->w*scale_x), NULL);

    for(int y=n_map->h-1; y>=0; y--)
    {
        for(int x=n_map->w-1; x>=0; x--)
        {
            n_map->data[y][x] = map->data[y/scale_y][x/scale_x];
        }
    }

    return n_map;
}

int mapscale(map_t **map, int scale_y, int scale_x)
{
    map_t *n_map;
    RPTEST(n_map = mapcreat((*map)->h*scale_y, (*map)->w*scale_x), -1);

    for(int y=n_map->h-1; y>=0; y--)
    {
        for(int x=n_map->w-1; x>=0; x--)
        {
            n_map->data[y][x] = (*map)->data[y/scale_y][x/scale_x];
        }
    }

    mapfree(map);
    (*map) = n_map;
    return 0;
}

map_t *mapgetf(map_t *map, int y, int x, int h, int w)
{
    map_t *frag = NULL;
    
    if((x < 0) || (y < 0) || (w <= 0) || (h <= 0)
            || (x+w > map->w) || (y+h > map->h))
    {
        return NULL;
    }

    RPTEST(frag = mapcreat(h, w), NULL);
    for(int j=h-1; j>=0; j--)
    {
        for(int i=w-1; i>=0; i--)
        {
            frag->data[j][i] = map->data[y+j][x+i];
        }
    }
    return frag;
}

int mapsetf(map_t *map, map_t *frag, int pos_y, int pos_x)
{
    if((frag->w <= 0) || (frag->h <= 0) 
            || (map->w <= 0) || (map->h <= 0) 
            || (frag->w > map->w) 
            || (frag->h > map->h))
    {
        mapfree(&frag);
        return -1;
    }

//  for(int j=frag->h-1; j >=0; j--)
//  {
//      for(int i=frag->w-1; i>=0; i--)
//      {
//          mp->data[pos_y+j][pos_x+i] = frag->data[j][i];
//      }
//  }
    for(int j=frag->h-1; j>=0; j--)
    {
        for(int i=frag->w-1; i>=0; i--)
        {
            map->data[(pos_y+j)%map->h][(pos_x+i)%map->w] = frag->data[j][i];
        }
    }
    mapfree(&frag);
    return 0;
}

void mapfree(map_t **mp)
{
    if(mp == NULL)
        return;

    free((*mp)->data[0]);
    free((*mp)->data);
    *mp = NULL;
}

void mapprint(map_t *map)
{
    for(int j=0; j<map->h; j++)
    {
      for(int i=0; i<map->w; i++)
      {
        if(map->data[j][i] == L' ')
        {
          wprintf(L"-");
        }
        else
        {
          wprintf(L"#");
        }
        //wprintf(L"%lc", map->data[j][i]);
      }
      wprintf(L"\n");
    }
    wprintf(L"\n");
}

