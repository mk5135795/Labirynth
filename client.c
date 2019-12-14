//define _DEFAUL_SOURCE
#define _XOPEN_SOURCE_EXTENDED 

#define PORT 55555
#define SAFE(v) \
  if((v) < 0) \
{ \
  wprintf(L"error - line %d in  %s\n", __LINE__, __FILE__); \
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <wchar.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "map.h"
#include "maze.h"
#include "graphic.h"

int sd;
int h = 9;
int w = 9;

void sigclose(int signum)
{
  shutdown(sd, 2);
  close(sd);
//windel(area);
  exit(0);
}

int wr_msg_ack(wchar_t **msg, wchar_t *wstr)
{
  if(wstr != NULL)
    swprintf(*msg, 8, L"%ls", wstr);
  SAFE(write(sd, *msg, 8*sizeof(wchar_t)));
  read(sd, *msg, 8*sizeof(wchar_t));
//wprintf(L"%ls\n", *msg);
  if(wcsstr(*msg, L"ack") == NULL)
    return -1;
  return 0;
}

void wr_msg(wchar_t **msg, wchar_t *wstr)
{
  if(wstr != NULL)
    swprintf(*msg, 8, L"%ls", wstr);
  SAFE(write(sd, *msg, 8*sizeof(wchar_t)));
}

int reqdata(wchar_t **msg, wchar_t **data, char dir)
{
    swprintf(*msg, 8, L"mov%lc", dir);
    wr_msg(msg, NULL);
    return read(sd, *data, h*(w+1)*sizeof(wchar_t));
}

int mov(wchar_t **msg, wchar_t **data, area_t *area, char dir)
{
  reqdata(msg, data, dir);
//wprintf(L"%ls\n", *data);
  if(wcscmp(*data, L"") == 0)
    return -1;
  mapfree(&area->map);
  area->map = mapdeserialize(*data, h, w);
  winupdate(area);
  return 0;
}

int init(struct sockaddr_in *addr, char *ip)
{
  setlocale(LC_ALL, "");
  RTEST(signal(SIGINT, sigclose), -1);

  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(ip);
  addr->sin_port = htons(PORT);

  RTEST(sd = socket(AF_INET, SOCK_STREAM, 0), -1);
  if(connect(sd, (struct sockaddr*) addr, sizeof(*addr)) < 0)
  {
    wprintf(L"no server\n");
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) 
{
  struct sockaddr_in addr;
  char* ip = "0.0.0.0";

  if(argc > 1)
  {
    ip = argv[1];
  }

  if(init(&addr, ip) < 0)
  {
    return -1;
  }
  wininit();
  area_t *area; 

  char dir=' ';
  int size = 0;
  wchar_t *ws_msg = NULL;
  wchar_t *wstr = NULL;
  fd_set set_rd;
  int sel = 0;
  struct timeval timeout;

  //read connection ack
  ws_msg = malloc(8*sizeof(wchar_t));
  if((size = read(sd, ws_msg, 8*sizeof(wchar_t))) < 1)
  {
    return -1;
  }
  h = ws_msg[4];
  w = ws_msg[5];
  area = winget(h, w);
  wstr = malloc(h*(w+1)*sizeof(wchar_t));
  
//read(sd, ws_msg, 8*sizeof(wchar_t));
  if(mov(&ws_msg, &wstr, area, ' ') < 0)
  {
    return -1;
  }
  wr_msg(&ws_msg, L"ready");
  while(dir != 'q')
  {
    FD_SET(0, &set_rd);
    FD_SET(sd, &set_rd);
    timeout.tv_usec = 100000;
    if((sel = select(sd+1, &set_rd, NULL, NULL, &timeout)) < 0)
    {
      return -1;
    }

    if(sel == 0)
    {
      winupdate(area);
      wr_msg(&ws_msg, L"get");
      do{
        read(sd, ws_msg, 8*sizeof(wchar_t));
        if(wcsstr(ws_msg, L"pl") != NULL)
        {
          winplayer(area, ws_msg[2], ws_msg[3]);
        }
      }while(wcsstr(ws_msg, L"eof") == NULL);
    }
    else if(FD_ISSET(0, &set_rd))
    {
      dir = wgetch(area->win);
      if( dir == 'w' || dir == 'a' || dir == 's' || dir == 'd' || dir == ' ')
      {
        mov(&ws_msg, &wstr, area, dir);
      }
      else if (dir == 'Q')
      {
          wr_msg(&ws_msg, L"end");
          dir = 'q';
      }
      switch(dir)
      {
        case 'c': wr_msg(&ws_msg, L"chk");   break;
        case 'g': wr_msg(&ws_msg, L"grid");  break;
        case 'r': wr_msg(&ws_msg, L"rnd");   break;
        case 'p': wr_msg(&ws_msg, L"print"); break;
        case 'm': wr_msg(&ws_msg, L"maze");  break;
      }
      if(dir == 'q')
      {
        break;
      }
    }
    else if(FD_ISSET(sd, &set_rd))
    {
    }
  }
  if(size == 0)
  {
    SAFE(shutdown(sd, 2));
    SAFE(close(sd));
  }
  windel(area);
  return 0;
} 
