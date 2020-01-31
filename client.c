#include <stdio.h>//printf
#include <unistd.h>//_FILENO
#include "./include/network.h"
#include "./include/graphic.h"

#include "./include/map.h"
#include "./include/maze.h"

int main(int argc, char* argv[]) 
{
  int s = 0;
  int end = 0;
//Point client_size = {24, 80};
  Net *net;
  MsgQueue *qmsg;
//Area *area;
//
//win_init();

//ERPTEST(area = win_create(client_size.y, client_size.x), -1);

//char dir;
//Map *frag;
//while((dir = wgetch(area->win)) != 'q')    
//{
//  //if frag=recv
//  {
//    RTEST(win_move(area, dir, frag), -1);
//  }
//}

  ERPTEST(net = net_create(), -1);
  ERTEST(net_add_client(net, "0.0.0.0"), -1);
  ERTEST(net_add_fd(net, STDIN_FILENO, getchar, SRC_STDIN), -1);

//Map *map;
//ERPTEST(map = map_create(MAP_FILL, 4, 4, L'#'), -1);
//checkers(map);
//map_print(map);

  int msg_n = 2;
  Msg *msg;
  ERPTEST(msg = msg_create(100, MSG_TEXT, msg_n), -1);
  msg->str[1] = '\0';
  while(end == 0)
  {
    if((s = net_listen(net, 0)) <= 0)
    {
      return -1;
    }
    else
    {
      while(net->q_head != NULL)
      {
        switch(net->q_head->msg->type)
        {
          case MSG_ERROR:
            end = 1;
            wprintf(L"c recv %s\n", net->q_head->msg->str);
            break;
          case MSG_TEXT:
            msg->str[0] = net->q_head->msg->str[0];
            wprintf(L"c stdin %c\n", msg->str[0]);

            if(msg->str[0] == 'q') { msg->type = MSG_ERROR;
            } else {                 msg->type = MSG_TEXT ;}
            ERTEST(net_send_msg(net->fd[0], msg), -1);

            wprintf(L"c send %s\n", msg->str);
            break;
          case MSG_OTHER:
            wprintf(L"c recv %s\n", net->q_head->msg->str);
            break;
        }
        qmsg = net->q_head->next;
        msg_delete(&net->q_head->msg);
        free(net->q_head);
        net->q_head = qmsg;
      }
    }
  }

//msg = map_serialize(map);
  net_delete(&net);
  return 0;
} 
