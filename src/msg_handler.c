#include "../include/msg_handler.h"

#include <stdio.h>

MsgQueue *_msg_queue_send_to_all(Net       *net,
                                 MsgQueue **qmsg)
{
  MsgQueue *temp = *qmsg;
  MsgQueue *end = NULL;

  *qmsg = (*qmsg)->next;
  temp->next = NULL;
  for(int i=net->n-1; i>=0; i--) {
    if(net->type[i] == temp->id) {
      if(msg_queue_add(qmsg, NULL, net->id[i]) < 0) {
        msg_queue_delete(&temp);
        return NULL;
      }
      if(((*qmsg)->msg = msg_copy(temp->msg)) == NULL) {
        msg_queue_delete(&temp);
        //rm invalid msg
        temp = (*qmsg);
        *qmsg = (*qmsg)->next;
        free(temp);
        return NULL;
      }
      if(end == NULL) {
        end = (*qmsg);
      }
    }
  }
  msg_queue_delete(&temp);
  return end;
}

MsgQueue *_msg_queue_request_handler(Net       *net,
                                     MsgQueue **qmsg)
{}

MsgQueue *msg_queue_handler(Net       *net,
                            MsgQueue **request_queue, 
                            MsgQueue  *await, 
                            int      (*req_msg_handler)(MsgQueue*))
{
  MsgQueue *response_head = NULL;
  MsgQueue *response_tail = NULL;
  MsgQueue *tmp;

  while(*request_queue != NULL) {
    switch((*request_queue)->msg->response)
    {
    case MSG_REQUEST:
      //handle request
      switch(req_msg_handler(*request_queue))
      {
      case 0:
        msg_queue_remove(request_queue);
        break;

      case 1:
        //move to response queue
        if(response_head == NULL) {
          response_head = *request_queue;
          response_tail = *request_queue;
        }
        else {
          response_tail->next = *request_queue;
          response_tail = response_tail->next;
        }
        response_tail->next = NULL;
        *request_queue = (*request_queue)->next;
        break;

      case 2:
        tmp = _msg_queue_send_to_all(net, request_queue);
        if(tmp == NULL) {
          //error
        }
        //move to response queue
        if(response_head == NULL) {
          response_head = *request_queue;
          response_tail = tmp;
        }
        else {
          response_tail->next = *request_queue;
          response_tail = tmp;
        }
        response_tail->next = NULL;
        *request_queue = tmp->next;
        break;
      }
      break;

    case MSG_RESPONSE:
      tmp = msg_queue_find(await, (*request_queue)->id, (*request_queue)->msg->id);
      if(tmp != NULL) {
        //handle response
        //fun(resp, org_req);
        msg_queue_remove(&tmp);
        msg_queue_remove(request_queue);
        break;
      }

    default:
      //error
      msg_queue_remove(request_queue);
      break;
    }
  }
  return response_head;
}

int msg_queue_response(Net       *net, 
                       MsgQueue **response_queue, 
                       MsgQueue **await)
{
  int fd_i = -1;
  MsgQueue *qmsg;
  
  while(*response_queue != NULL)
  { 
    qmsg = *response_queue;
    *response_queue = (*response_queue)->next;
    
    //find dest
    ERTEST(fd_i = net_find(net, qmsg->id), -1);
    //send msg
    ERTEST(net_send_msg(net->fd[fd_i], qmsg->msg), -1);
    
    //mv requests to awaiting queue
    if(qmsg->msg->response == MSG_REQUEST) {
      msg_queue_move(&qmsg, await);
    }
    //rm responses
    else {
      msg_delete(&qmsg->msg);
      free(qmsg);
    }
  }
  return 0;
}

