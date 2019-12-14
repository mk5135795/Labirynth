#define _DEFAUL_SOURCE
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
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include <wchar.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "map.h"
#include "maze.h"
#include "graphic.h"

const int c_h = 9;
const int c_w = 9;
int h = 40;
int w = 40;
int *sd;
int *rdy;
int (*a_pos)[2];
int (*p_pos)[2];
int n;
int n_max = 2;
struct sockaddr_in *addr;
area_t *area;
wchar_t *ws_msg = NULL;
wchar_t *ws_data = NULL;

void sigclose(int signum)
{
  for(int i=0; i <n; i++)
  {
    shutdown(sd[i], 2);
    close(sd[i]);
  }
  free(sd);
  free(rdy);
  free(a_pos);
  free(p_pos);
  free(addr);
  exit(0);
}

void sendarea(int i)
{
//mapprint(mapgetf(area->map, a_pos[i][0], a_pos[i][1], c_h, c_w));
  swprintf(ws_data, c_h*(c_w+1), L"%ls",
      mapserialize(mapgetf(area->map,
          a_pos[i][0], a_pos[i][1], c_h, c_w)));
//wprintf(L"send: %ls\n", ws_data);
  SAFE(write(sd[i], ws_data, c_h*(c_w+1)*sizeof(wchar_t)));
}

int collision(int i, wchar_t dir)
{
  switch(dir)
  {
    case L'w':
      if(area->map->data[a_pos[i][0]+p_pos[i][0]-1][a_pos[i][1]+p_pos[i][1]] == L' ')
      {
        return 1;
      }
      break;
    case L's':
      if(area->map->data[a_pos[i][0]+p_pos[i][0]+1][a_pos[i][1]+p_pos[i][1]] == L' ')
      {
        return 1;
      }
      break;
    case L'a':
      if(area->map->data[a_pos[i][0]+p_pos[i][0]][a_pos[i][1]+p_pos[i][1]-1] == L' ')
      {
        return 1;
      }
      break;
    case L'd':
      if(area->map->data[a_pos[i][0]+p_pos[i][0]][a_pos[i][1]+p_pos[i][1]+1] == L' ')
      {
        return 1;
      }
      break;
  }
  return 0;
}

int moveplayer(int i, wchar_t dir)
{
  switch(dir)
  {
    case L'w':
      if(p_pos[i][0] > 0 && collision(i, dir))
      {
        p_pos[i][0]--;
        return 0;
      }
      break;
    case L's':
      if(p_pos[i][0] < c_h-1 && collision(i, dir))
      {
        p_pos[i][0]++;
        return 0;
      }
      break;
    case L'd':
      if(p_pos[i][1] < c_w-1 && collision(i, dir))
      {
        p_pos[i][1]++;
        return 0;
      }
      break;
    case L'a':
      if(p_pos[i][1] > 0 && collision(i, dir))
      {
        p_pos[i][1]--;
        return 0;
      }
      break;
  }
  return -1;
}

int movearea(int i, wchar_t dir)
{
  switch(dir)
  {
    case L'w':
      if(a_pos[i][0] > 0 && collision(i, dir))
      {
        a_pos[i][0]--;
        sendarea(i);
        return 0;
      }
      break;
    case L's':
      wprintf(L"%d %d %d\n", a_pos[i][0], c_h, h-1);
      if(a_pos[i][0]+c_h < h-1 && collision(i, dir))
      {
        a_pos[i][0]++;
        sendarea(i);
        return 0;
      }
      break;
    case L'd':
      if(a_pos[i][1]+c_w < w-1 && collision(i, dir))
      {
        a_pos[i][1]++;
        sendarea(i);
        return 0;
      }
      break;
    case L'a':
      if(a_pos[i][1] > 0 && collision(i, dir))
      {
        a_pos[i][1]--;
        sendarea(i);
        return 0;
      }
      break;
    default:
      return -1;
  }
  //return -1;
  return moveplayer(i, dir)-1;
}

int mov(int i, wchar_t dir)
{
  if(dir == L' ')
  {
    sendarea(i);
    return 0;
  }
  if(p_pos[i][0] == c_h/2
      && p_pos[i][1] == c_w/2)
  {
    return movearea(i, dir);
  }
  return moveplayer(i, dir)-1;
}

void resize(int size)
{
  int *tmp_sd;
  int *tmp_rdy;
  int *tmp_a_pos;
  int *tmp_p_pos;
  struct sockaddr_in *tmp_addr;
  if(size < n_max)
  {
    size = n_max;
  }
  size += 2;
  SAFE(tmp_sd = malloc(size*sizeof(int)));
  SAFE(tmp_rdy = malloc(size*sizeof(int)));
  SAFE(tmp_a_pos = malloc(size*sizeof(int[2])));
  SAFE(tmp_p_pos = malloc(size*sizeof(int[2])));
  SAFE(tmp_addr = malloc(size*sizeof(struct sockaddr_in)));
  for(int i=0; i<n; i++)
  {
    tmp_sd[i] = sd[i];
    tmp_rdy[i] = rdy[i];
    tmp_a_pos[i] = a_pos[i];
    tmp_p_pos[i] = p_pos[i];
    tmp_addr[i] = addr[i];
  }
  free(sd);
  free(rdy);
  free(a_pos);
  free(p_pos);
  free(addr);
  n_max = size;
  sd = tmp_sd;
  rdy = tmp_rdy;
  a_pos = tmp_a_pos;
  p_pos = tmp_p_pos;
  addr = tmp_addr;
}

int init(struct sockaddr_in *addres)
{
  setlocale(LC_ALL, "");
  RTEST(signal(SIGINT, sigclose), -1);

  n = 1;
  sd = malloc(n_max*sizeof(int));
  rdy = malloc(n_max*sizeof(int));
  a_pos = malloc(n_max*sizeof(int[2]));
  p_pos = malloc(n_max*sizeof(int[2]));
  addr = malloc(n_max*sizeof(struct sockaddr_in));

  RTEST(sd[0] = socket(AF_INET, SOCK_STREAM, 0), -1);
  memset(addres, 0, sizeof(*addres));
  addres->sin_family = AF_INET;
  addres->sin_addr.s_addr = INADDR_ANY;
  addres->sin_port = htons(PORT);

  RTEST(bind(sd[0], (struct sockaddr*) addres, sizeof(*addres)), -1);
  RTEST(listen(sd[0], 100), -1);
  return 0;
}

int main(int argc, char* argv[]) 
{
  struct sockaddr_in addr_serv;

  if(init(&addr_serv) < 0)
  {
    wprintf(L"error\n");
    return 0;
  }
  area = winget(h, w);
  genmaze(area->map);
  mapprint(area->map);

  int nfds = sd[0];
  fd_set set;
  ws_msg = malloc(8*sizeof(wchar_t));
  ws_data = malloc(c_h*(c_w+1)*sizeof(wchar_t));
  wprintf(L"server(max clients %d)\n", n_max-1);
  while(1)
  {
    FD_ZERO(&set);
    for(int i=0; i<n; i++)
    {
      FD_SET(sd[i], &set);
    }
    if(select(nfds+1, &set, NULL, NULL, NULL) > 0)
    {
      for(int i=1; i<n; i++)
      {
        if(FD_ISSET(sd[i], &set))
        {
          //read req
          if((read(sd[i], ws_msg, 8*sizeof(wchar_t))) < 1)
          {
            swprintf(ws_msg, 8, L"end");
          }
//        wprintf(L"#%d >: %ls\n", i, ws_msg);

          //if req data
          if(wcsstr(ws_msg, L"mov") != NULL)
          {
//          wprintf(L"\"%lc\"\n", ws_msg[3]);
            if(mov(i, ws_msg[3]) < 0)
            {
              wprintf("<0\n");
              SAFE(write(sd[i], L"", sizeof(L"")));
            }
              wprintf("<0\n");
          }
          else if(wcscmp(ws_msg, L"maze") == 0){ mapfill(area->map, L'█', 0); genmaze(area->map);}
          else if(wcscmp(ws_msg, L"rnd") == 0){ mapfill(area->map, L'█', 0); randmap(area->map);}
          else if(wcscmp(ws_msg, L"chk") == 0){ mapfill(area->map, L'█', 0); checkers(area->map);}
          else if(wcscmp(ws_msg, L"grid") == 0){ mapfill(area->map, L'█', 0); grid(area->map);}
          else if(wcscmp(ws_msg, L"ready") == 0)
          {
            rdy[i] = 1;
          }
          //if serv print
          else if(wcscmp(ws_msg, L"print") == 0)
          {
            mapprint(area->map);
          }
          else if(wcscmp(ws_msg, L"get") == 0)
          {
            for(int j=1; j<n; j++)
            {
              int py = a_pos[j][0]+p_pos[j][0]-a_pos[i][0];
              int px = a_pos[j][1]+p_pos[j][1]-a_pos[i][1];
              if(py >= 0 && py < c_h && px >= 0 && px < c_w)
              {
//              wprintf(L"%d %d\n", py, px);
//              wprintf(L"%d %d %d | %d %d %d\n", a_pos[j][0],p_pos[j][0],a_pos[i][0],
//                  a_pos[j][1],p_pos[j][1],a_pos[i][1]);
                swprintf(ws_msg, 8, L"pl%lc%lc", py, px);
                SAFE(write(sd[i], ws_msg, 8*sizeof(wchar_t)));
              }
            }
            SAFE(write(sd[i], L"eof", sizeof(L"eof")));
          }
          //if end
          if(wcscmp(ws_msg, L"end") == 0)
          {
            n--;
            //close desc
            shutdown(sd[i], 2);
            close(sd[i]);
            wprintf(L"#%d disconnected\n", i);
            //keep cont list
            if(i < n)
            {
              addr[i] = addr[n];
              sd[i] = sd[n];
              rdy[i] = rdy[n];
              a_pos[i][0] = a_pos[n][0];
              a_pos[i][1] = a_pos[n][1];
              p_pos[i][0] = p_pos[n][0];
              p_pos[i][1] = p_pos[n][1];
              wprintf(L"#%d changed to #%d\n", n, i);
            }
          }
        }
      }
      if(FD_ISSET(sd[0], &set))
      {
        /*
         * if const max_slots
         * if n = max_slots
         * send 'nack'
         */
        //add connection slots
        if(n == n_max)
        {
          resize(0);
          wprintf(L"resized to %d max clients\n", n_max);
        }
        //accept connection
        int len = sizeof(addr[n]);
        SAFE(sd[n] = accept(sd[0], (struct sockaddr*)&addr[n], (socklen_t*)&len));
        if(nfds < sd[n])
        {
          nfds = sd[n];
        }
        //send connection ack
        swprintf(ws_msg, 8, L"ack %lc%lc", c_h, c_w);
        SAFE(write(sd[n], ws_msg, 8*sizeof(wchar_t)));
        rdy[n] = 0;
        a_pos[n][0] = a_pos[n][1] = 0;
        p_pos[n][0] = c_h/2;
        p_pos[n][1] = c_w/2;
        wprintf(L"#%d connected\n", n);
        n++;
      }
    }
    else
    {
      wprintf(L"error\n");
    }
//  if(pl_i >0)
//  {
//    for(; pl_i>0; pl_i--)
//    {
//      swprintf(ws_msg, 8, L"p%lc%lc", pl_pos[pl_i+1][0], pl_pos[pl_i][1]);
//      for(int i=1; i<n; i++)
//      {
//        wprintf(L"#%d < pl pos\n", i);
//        SAFE(write(sd[i], ws_msg, 8*sizeof(wchar_t)));
//      }
//    }
//  }
  }
  free(sd);
  free(addr);
} 
