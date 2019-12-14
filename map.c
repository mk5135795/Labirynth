#include <stdlib.h>
#include "map.h"

#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>

map_t *mapcreat(int h, int w)
{
    map_t *mp = NULL;
    
    PRTEST(mp = malloc(sizeof(map_t)), NULL);
//    PRTEST(mp->color = malloc(h*sizeof(int*)), NULL);
    PRTEST(mp->data = malloc(h*sizeof(wchar_t*)), NULL);
    mp->h = h;
    mp->w = w;
    
//    PRTEST(mp->color[0] = malloc(h*w*sizeof(int)), NULL);
    PRTEST(mp->data[0] = malloc(h*(w+1)*sizeof(wchar_t)), NULL);
    mp->data[0][w] = L'\0';

    for(int y=h-1; y>0; y--)
    {
//        mp->color[y] = mp->color[0] + (y*w);
        mp->data[y] = mp->data[0] + (y*(w+1));
        mp->data[y][w] = L'\0';
    }
    return mp;
}

map_t *mapcpy(map_t *map)
{
    map_t *n_map = NULL;
    
    PRTEST(n_map = mapcreat(map->h, map->w), NULL);
    for(int y=map->h-1; y>=0; y--)
    {
        for(int x=map->w-1; x>=0; x--)
        {
            n_map->data[y][x] = map->data[y][x];
//            n_map->color[y][x] = map->color[y][x];
        }
    }

    return n_map;
}

map_t *mapcfill(int h, int w, wchar_t bg, int bg_color)
{
    map_t *n_map = NULL;
    
    PRTEST(n_map = mapcreat(h, w), NULL);
    
    for(int y=h-1; y>=0; y--)
    {
        for(int x=w-1; x>=0; x--)
        {
            n_map->data[y][x] = bg;
//            n_map->color[y][x] = bg_color;
        }
    }
    return n_map;
}

void mapfill(map_t *mp, wchar_t bg, int bg_color)
{
    for(int y=mp->h-1; y>=0; y--)
    {
        for(int x=mp->w-1; x>=0; x--)
        {
            mp->data[y][x] = bg;
//            mp->color[y][x] = bg_color;
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

    PRTEST(frag = mapcreat(h, w), NULL);
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
            //mp->data[pos_y+j][pos_x+i] = chr;
        }
    }
    mapfree(&frag);
    return 0;
}

map_t *mapscale(map_t *mp, int scale_y, int scale_x)
{
    map_t *n_mp;
    PRTEST(n_mp = mapcreat(mp->h*scale_y, mp->w*scale_x), NULL);

    for(int y=n_mp->h-1; y>=0; y--)
    {
        for(int x=n_mp->w-1; x>=0; x--)
        {
            n_mp->data[y][x] = mp->data[y/scale_y][x/scale_x];
//            n_mp->color[y][x] = mp->color[y/scale_y][x/scale_x];
        }
    }

    return n_mp;
}

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

//    free((*mp)->color[0]);
    free((*mp)->data[0]);
//    free((*mp)->color);
    free((*mp)->data);
    *mp = NULL;
}

wchar_t *mapserialize(map_t *map)
{
    wchar_t *wstr = NULL;
    PRTEST(wstr = malloc(map->h*(map->w+1)*sizeof(wchar_t*)), NULL);
    memcpy(wstr, map->data[0], map->h*(map->w+1)*sizeof(wchar_t));
    for(int y=map->h-1; y>=0; y--)
    {
       wstr[y*(map->w+1) +map->w] = L'0';
    }
    return wstr;
}

void mapprint(map_t *map)
{
    for(int j=0; j<map->h; j++)
    {
        wprintf(L"%ls\n", map->data[j]);
    }
}

map_t *mapdeserialize(wchar_t *d, int h, int w)
{
    map_t *map = malloc(sizeof(map_t));
    map->h = h;
    map->w = w;
    PRTEST(map->data = malloc(h*sizeof(wchar_t*)), NULL);
//    PRTEST(map->color = malloc(h*sizeof(wchar_t*)), NULL);
    PRTEST(map->data[0] = malloc(h*(w+1)*sizeof(wchar_t)), NULL);
    memcpy(map->data[0], d, h*(w+1)*sizeof(wchar_t));
//  wprintf(L"ma:  %ls\n", map->data[0]);
    map->data[0][w] = L'\0';
    for(int y=h-1; y>0; y--)
    {
        map->data[y] = map->data[0] + (y*(w+1));
        map->data[y][w] = L'\0';
//      wprintf(L"m:%ls\n", map->data[0]+(y*(w+1)));
    }
//  wprintf(L"m:%ls\n", map->data[0]);
    return map;
}
