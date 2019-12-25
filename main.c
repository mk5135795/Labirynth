#define _XOPEN_SOURCE //usleep

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "graphic.h"

void movlen(area_t *area, char dir)
{
  for(int i=rand()%50+20; i>=0; i--)
  {
    usleep(25000);
    winmov(area, dir);
  }
}

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

int main(int argc, char *argv[])
{
  int w = 60;
  int h = 21;
  wininit();
  
  area_t *area;
  ERPTEST(area = winmaze(h, w), -1);
  winupdate(area);

  for(int i=0; i<20; i++)
  {
    movlen(area, rnddir());
  }
  usleep(1000000);
  
  windel(area);
  return 0;
}
