/**
 *  \file msg.h
 *  \file msg.c
 */

#ifndef MSG_H
#define MSG_H

#include <stdlib.h>
#include "macro.h"

#include <stdio.h>

enum response { MSG_IGNORE, MSG_REQUEST, MSG_RESPONSE };
enum type { MSG_ERROR , MSG_AREA, MSG_TEXT, MSG_COMM, MSG_OTHER };

typedef struct 
{
  char   response;
  char   id;
  char   type;
  size_t size;
  char  *str;
} Msg;

typedef struct MsgQueue 
{
  Msg             *msg;
  int              id;
  struct MsgQueue *next;
} MsgQueue;

Msg *msg_create(char id, char type, size_t size);
//set_text()
int msg_resize(Msg *msg, size_t size);
void msg_delete(Msg **msg);
int msg_queue_add(MsgQueue **queue, Msg *msg, int fd_i);
void msg_queue_move(MsgQueue **qmsg, MsgQueue **queue);
MsgQueue *msg_queue_find(MsgQueue *queue, int src_id, int msg_id);
void msg_queue_remove(MsgQueue **qmsg);
void msg_queue_delete(MsgQueue **queue);

#endif /*MSG_H*/
