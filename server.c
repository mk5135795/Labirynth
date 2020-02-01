#include <stdio.h>//printf
#include "./include/network.h"
#include "./include/graphic.h"
#include "./include/msg_handler.h"

#include "./include/map.h"
#include "./include/maze.h"

//q_recived -> q_to_send
//MsgQueue mq_process(MsgQueue *queue);
//send(q_to_send)
//void mq_send(MsgQueue *queue);

int msg_set_ack(Msg *msg)
{
  RTEST(msg_resize(msg, 4), -1);
  strcpy(msg->str, "ACK");
  msg->type = MSG_COMM;
  msg->response = MSG_RESPONSE;
  return 0;
}

int msg_handler(MsgQueue *qmsg)
{
    switch(qmsg->msg->type)
    {
    case MSG_ERROR:
      break;
    case MSG_AREA:
      break;
    case MSG_TEXT:
      wprintf(L"%d>: %s\n", qmsg->id, qmsg->msg->str);
      break;
    case MSG_COMM:
      wprintf(L"%d comm %s\n", qmsg->id, qmsg->msg->str);
      qmsg->id = SRC_CLIENT;
      RTEST(msg_resize(qmsg->msg, 5), -1);
      strcpy(qmsg->msg->str, "ECHO");
      qmsg->msg->type = MSG_TEXT;
      qmsg->msg->response = MSG_RESPONSE;
      return 2;
      break;
    case MSG_OTHER:
      break;
    default:
      return 0;
    }
    if(msg_set_ack(qmsg->msg) < 0) {
      msg_delete(&qmsg->msg);
    }
    return 1;
}

int main(int argc, char* argv[]) 
{
  Net *net;
//Point client_size = {24, 80};
//Point server_size = {1*24, 1*78};
//Point pos = {0, 0};

//Area *g_area;
//
//win_init();
//ERPTEST(g_area = winmaze(server_size.y, server_size.x), -1);
//
//char dir;
//Map *frag;
//while((dir = wgetch(area->win)) != 'q')    
//{
//  if((frag = win_get_area(dir, g_area->map, &pos, client_size)) != NULL)
//  {
//    //send
//  }
//}
//  
//Map *map;
//
//ERPTEST(map = map_create(MAP_FILL, 4, 4, L'#'), -1);
//grid(map);
//map_print(map);
  
  int end = 0;
  int s = 0;
  MsgQueue *qmsg;
  MsgQueue *await = NULL;

  ERPTEST(net = net_create(), 0);
  ERTEST(net_add_server(net), -1);
  
  int n = 4;
  int fd_i = -1;
  Msg *msg;
  ERPTEST(msg = msg_create(1, MSG_OTHER, n), -1);
  while(end == 0)
  {
    if((s = net_listen(net, 0)) <= 0)
    {
      return -1;
    }
    else
    {
      if(1) {
      MsgQueue *tmp = msg_queue_handler(net, &net->q_head, await, msg_handler);
      msg_queue_response(net, &tmp, &await);
      }
      else {
      while(net->q_head != NULL)
      {
        switch(net->q_head->msg->type)
        {
          case MSG_ERROR:
            end = 1;
            wprintf(L"s recv %s\n", net->q_head->msg->str);
            msg->type = MSG_ERROR;
            strcpy(msg->str, "END");
            ERTEST(fd_i = net_find(net, net->q_head->id), -1);
            ERTEST(net_send_msg(net->fd[fd_i], msg), -1);
            wprintf(L"s send %s\n", msg->str);
            break;
          case MSG_TEXT:
            wprintf(L"s recv %s\n", net->q_head->msg->str);
            msg->type = MSG_OTHER;
            strcpy(msg->str, "ACK");
            ERTEST(fd_i = net_find(net, net->q_head->id), -1);
            ERTEST(net_send_msg(net->fd[fd_i], msg), -1);
            wprintf(L"s send %s\n", msg->str);
            break;
        }
        qmsg = net->q_head->next;
        msg_delete(&net->q_head->msg);
        free(net->q_head);
        net->q_head = qmsg;
      }
      }
    }
  }

//Map *t = map_deserialize(net->q_head->msg);
//map_print(t);

  ERTEST(net_close_sfd(net, 1), -1);
  net_delete(&net);
  return 0;
} 
