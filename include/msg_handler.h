/**
 *  /file msg_handler.h
 *  /file msg_handler.c
 */
#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "network.h"


MsgQueue *msg_queue_send_to_all(Net *net, MsgQueue **qmsg);
MsgQueue *msg_queue_handler(Net *net, MsgQueue **request_queue, MsgQueue *await,
    int (*req_msg_handler)(MsgQueue*));
int msg_queue_response(Net *net, MsgQueue **response_queue, MsgQueue **await);

#endif /*MSG_HANDLER_H*/
