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

map_t *mapgetf(map_t *mp, int y, int x, int h, int w)
{
    map_t *frag = NULL;
    
    if((x < 0) || (y < 0) || (w <= 0) || (h <= 0)
            || (x+w > mp->w) || (y+h > mp->h))
    {
        return NULL;
    }

    RPTEST(frag = mapcreat(h, w), NULL);
    for(int j=h-1; j>=0; j--)
    {
        for(int i=w-1; i>=0; i--)
        {
            frag->data[j][i] = mp->data[y+j][x+i];
        }
    }
    return frag;
}

int mapsetf(map_t *mp, map_t *frag, int pos_y, int pos_x)
{
    if((frag->w <= 0) || (frag->h <= 0) 
            || (mp->w <= 0) || (mp->h <= 0) 
            || (pos_x + frag->w > mp->w) 
            || (pos_y + frag->h > mp->h))
    {
        mapfree(&frag);
        return -1;
    }

    for(int j=frag->h-1; j >=0; j--)
    {
        for(int i=frag->w-1; i>=0; i--)
        {
            mp->data[pos_y+j][pos_x+i] = frag->data[j][i];
        }
    }
    mapfree(&frag);
    return 0;
}

map_t *mapscale(map_t *mp, int scale_y, int scale_x)
{
    map_t *n_mp;
    RPTEST(n_mp = mapcreat(mp->h*scale_y, mp->w*scale_x), NULL);

    for(int y=n_mp->h-1; y>=0; y--)
    {
        for(int x=n_mp->w-1; x>=0; x--)
        {
            n_mp->data[y][x] = mp->data[y/scale_y][x/scale_x];
        }
    }

    return n_mp;
}

// DEL/REDO
void mapshift(map_t *map, int sh_y, int sh_x)
{
    wchar_t *wstr_x;
    wchar_t **wstr_y;
    PTEST(wstr_x = malloc(map->w*sizeof(wchar_t)));
    PTEST(wstr_y = malloc(map->h*sizeof(wchar_t*)));
    for(int y=map->h-1; y >=0; y--)
    {
        for(int x=map->w-1; x>=0; x--)
        {
            wstr_x[x] = map->data[y][x];
        }
        for(int x=map->w-1; x>=0; x--)
        {
            map->data[y][(x+sh_x)%map->w] = wstr_x[x];
        }
        wstr_y[y] = map->data[y];
    }
    for(int y=map->h-1; y >=0; y--)
    {
        map->data[(y-sh_y+map->h)%map->h] = wstr_y[y];
    }
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
        wprintf(L"%ls\n", map->data[j]);
    }
}

wchar_t *mapserialize(map_t *map)
{
    wchar_t *wstr = NULL;
    RPTEST(wstr = malloc(map->h*(map->w+1)*sizeof(wchar_t*)), NULL);
    memcpy(wstr, map->data[0], map->h*(map->w+1)*sizeof(wchar_t));
    for(int y=map->h-1; y>=0; y--)
    {
       wstr[y*(map->w+1) +map->w] = L'0';
    }
    return wstr;
}

map_t *mapdeserialize(wchar_t *d, int h, int w)
{
    map_t *map = malloc(sizeof(map_t));
    map->h = h;
    map->w = w;
    RPTEST(map->data = malloc(h*sizeof(wchar_t*)), NULL);
    RPTEST(map->data[0] = malloc(h*(w+1)*sizeof(wchar_t)), NULL);
    memcpy(map->data[0], d, h*(w+1)*sizeof(wchar_t));
    map->data[0][w] = L'\0';
    for(int y=h-1; y>0; y--)
    {
        map->data[y] = map->data[0] + (y*(w+1));
        map->data[y][w] = L'\0';
    }
    return map;
}
