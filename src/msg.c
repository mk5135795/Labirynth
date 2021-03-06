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

Msg *
msg_copy(Msg *msg)
{
  Msg *nmsg;
  RPTEST(nmsg = malloc(sizeof(Msg)), NULL);
  memcpy(nmsg, msg, sizeof(Msg));
  if((nmsg->str = malloc(msg->size*sizeof(char))) == NULL)
  {
    free(nmsg);
    nmsg = NULL;
  }
  strcpy(nmsg->str, msg->str);

  return nmsg;
}

int
msg_resize(Msg   *msg,
           size_t size)
{
  char *tmp;
  RPTEST(tmp = malloc(size*sizeof(char)), -1);
  
  free(msg->str);
  msg->str = tmp;
  msg->size = size;

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
msg_queue_move(MsgQueue **qmsg,
               MsgQueue **queue)
{
  MsgQueue *tmp = (*qmsg);
  (*qmsg) = (*qmsg)->next;
  tmp->next = (*queue);
  (*queue) = tmp;
}

MsgQueue *
msg_queue_find(MsgQueue *queue,
               int       src_id,
               int       msg_id)
{
  while(queue != NULL) {
    if(queue->id == src_id
    && queue->msg->id == msg_id)
    {
      return queue;
    }
    queue = queue->next;
  }
  return NULL;
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

