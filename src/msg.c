#include "../include/msg.h"

Msg *
msg_create(char   id,
           char   type,
           size_t size)
{
  Msg *msg;

  RPTEST(msg = malloc(sizeof(Msg)), NULL);
  msg->response = MSG_REQUEST;
  msg->id = id;
  msg->type = type;
  msg->size = size;
  if((msg->str = malloc(size*sizeof(char))) == NULL)
  {
    free(msg);
    msg = NULL;
  }

  return msg;
}

int
msg_resize(Msg   *msg,
           size_t size)
{
  char *tmp;
  RPTEST(tmp = malloc(size*sizeof(char)), -1);
  
  free(msg->str);
  msg->str = tmp;

  return 0;
}

void 
msg_delete(Msg **msg) 
{
  free((*msg)->str);
  free((*msg));
  msg = NULL;
}

int
msg_queue_add(MsgQueue **queue,
              Msg       *msg,
              int        id)
{
  MsgQueue *qmsg;

  RPTEST(qmsg = malloc(sizeof(MsgQueue)), -1);
  qmsg->msg = msg;
  qmsg->id = id;
  qmsg->next = *queue;

  *queue = qmsg;
  return 0;
}

void 
msg_queue_remove(MsgQueue **queue)
{
  MsgQueue *tmp = (*queue)->next;
  msg_delete(&(*queue)->msg);
  free(*queue);
  *queue = tmp;
}

void 
msg_queue_delete(MsgQueue **queue)
{
  MsgQueue **ptr = queue;
  while(*ptr != NULL) {
    ptr = &(*queue)->next;
    msg_delete(&(*queue)->msg);
    free(*queue);
    *queue = *ptr;
  }
  *queue = NULL;
}

