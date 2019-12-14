#define _XOPEN_SOURCE_EXTENDED 

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "map.h"
#include "maze.h"
#include "graphic.h"

void print(map_t *mp)
{
    for(int j=0; j<mp->h; j++)
    {
        wprintf(L"%ls\n", mp->data[j]);
    }
    mapfree(&mp);
}

int main(int argc, char *argv[])
{
    int sc_y = 3;
    int sc_x = 3;
    int h = 43;
    int w = 132;
    int mode=2;
    setlocale(LC_ALL, "");
    if(mode == 2)
    {
        wininit();
        area_t *area = winget(h, w);
        winupdate(area);
        sleep(2);
        windel(area);
        return 0;
    }
    else if(mode ==1)
    {
//      printf("aaa\n");
//      print(mapgetf(area_map, 0, 0, h, w));
//      mapfree(&area_map);
    }
    //map_t *area_map = mapcfill(h*sc_y, w*sc_x, L'█', 0);
    map_t *area_map = mapcfill(h, w, L'█', 0);
    //rwalk3(area_map);
    genmaze(area_map);
    map_t *tmp = mapscale(area_map, sc_y, sc_x);
    mapfree(&area_map);
    area_map = tmp;
//  set();
    return 0;
}
