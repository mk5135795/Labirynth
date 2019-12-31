#define _XOPEN_SOURCE //usleep

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"
#include "map.h"

int g_w = 60;
int g_h = 21;
int p_x = 0;
int p_y = 0;
area_t *g_area;

char rnddir()
{
  switch( rand()%4 )
  {
  case 0:
    return 'w';
  case 1:
    return 's';
  case 2:
    return 'a';
  case 3:
    return 'd';
  default:
    return ' ';
  }
}

map_t *getf(char dir, int h, int w)
{
  map_t *frag = NULL;
  switch(dir)
  {
  case 'w':
    if(p_y < 1)
    {
      break;
    }
    p_y--;
    frag = mapgetf(g_area->map, p_y, p_x, 1, w);
    if(frag == NULL)
    {
      p_y++;
    }
    break;
  case 's':
    if(p_y + h >= g_h)
    {
      break;
    }
    frag = mapgetf(g_area->map, p_y + h, p_x, 1, w);
    p_y++;
    if(frag == NULL)
    {
      p_y--;
    }
    break;
  case 'a':
    if(p_x < 1)
    {
      break;
    }
    p_x--;
    frag = mapgetf(g_area->map, p_y, p_x, h, 1);
    if(frag == NULL)
    {
      p_x++;
    }
    break;
  case 'd':
    if(p_x + w >= g_w)
    {
      break;
    }
    frag = mapgetf(g_area->map, p_y, p_x + w, h, 1);
    p_x++;
    if(frag == NULL)
    {
      p_x--;
    }
    break;
  case 'c':
    frag = mapgetf(g_area->map, p_y, p_x, h, w);
    break;
  }
  return frag;
}

int movlen(area_t *area, char dir)
{
  map_t *frag;
  for(int i=100; i>=0; i--)
  {
    usleep(25000*4);
    RPTEST(frag = getf(dir, g_h/2, g_w/2), -1);
    winmov(area, dir, frag);
//  wgetch(area->win);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  wininit();
  
  area_t *area;
  ERPTEST(g_area = winmaze(g_h, g_w), -1);
  ERPTEST(area = winget(g_h/2, g_w/2), -1);
  
  if(argc == 1)
  {
    winmov(area, 'c', getf('c', g_h/2, g_w/2));

    char dir;
    map_t *frag;
    while((dir = wgetch(area->win)) != 'q')    
    {
      if((frag = getf(dir, g_h/2, g_w/2)) != NULL)
      {
        RTEST(winmov(area, dir, frag), -1);
      }
    }

//  movlen(area, 's');
//  wgetch(area->win);
//  movlen(area, 'd');
//  wgetch(area->win);
//  movlen(area, 'w');
//  wgetch(area->win);
//  movlen(area, 'a');
//  wgetch(area->win);
  }
  else
  {
    winupdate(g_area);
    wgetch(g_area->win);
  }
  
  windel(g_area);
  windel(area);
  return 0;
}
