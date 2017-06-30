/*
 * Copyright (c) 2012 Peter Eckstrand
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the cocoOS operating system.
 * Author: Peter Eckstrand <info@cocoos.net>
 */
 


#ifndef OS_MSGQUEUE_H__
#define OS_MSGQUEUE_H__

/** @file os_msgqueue.h Message queue header file*/

#include "os_defines.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint8_t signal;
    uint8_t reserved;   /* Alignment byte */
    uint8_t pad0;
    uint8_t pad1;
    uint16_t delay;     /* Delay of posting in ticks */
    uint16_t reload;    /* Reload value for periodic messages */
} Msg_t;


typedef uint8_t MsgQ_t;


enum {
    MSG_QUEUE_UNDEF,
    MSG_QUEUE_DEF,
    MSG_QUEUE_EMPTY,
    MSG_QUEUE_FULL,
    MSG_QUEUE_RECEIVED,
    MSG_QUEUE_POSTED
};




#define OS_MSG_Q_POST(task, msg, delay, period, async )     do {\
                                                                static uint8_t os_posted;\
                                                                static MsgQ_t queue;\
                                                                static Evt_t event;\
                                                                queue = os_msgQ_find( task );\
                                                                event = os_msgQ_event_get( queue );\
                                                                do {\
                                                                    os_posted = os_msg_post( (Msg_t*)&msg, queue, delay, period );\
                                                                    if ( os_posted == MSG_QUEUE_FULL ){\
                                                                        if ( async == 0 ) {\
                                                                            event_wait(event);\
                                                                        }\
                                                                        else {\
                                                                            os_posted = MSG_QUEUE_UNDEF;\
                                                                        }\
                                                                    }\
                                                                } while ( os_posted == MSG_QUEUE_FULL );\
                                                                if ( MSG_QUEUE_POSTED == os_posted ) {\
                                                                	os_signal_event(event);\
                                                                	os_event_set_signaling_tid( event, running_tid );\
                                                                }\
                                                            } while(0)



#define OS_MSG_Q_RECEIVE(task, pMsg, async)     do {\
                                                    static MsgQ_t queue;\
                                                    static uint8_t os_received;\
                                                    static Evt_t event;\
                                                    queue = os_msgQ_find( task );\
                                                    event = os_msgQ_event_get( queue );\
                                                    do {\
                                                        os_received = os_msg_receive( (Msg_t*)pMsg, queue );\
                                                        if ( os_received == MSG_QUEUE_EMPTY ){\
                                                            if ( async == 0 ) {\
                                                                event_wait(event);\
                                                            }\
                                                            else {\
                                                                ((Msg_t*)pMsg)->signal = NO_MSG_ID;\
                                                                os_received = MSG_QUEUE_UNDEF;\
                                                            }\
                                                        }\
                                                    } while ( os_received == MSG_QUEUE_EMPTY );\
                                                    if ( MSG_QUEUE_RECEIVED == os_received) {\
                                                    	os_signal_event(event);\
                                                    	os_event_set_signaling_tid(event, running_tid );\
													}\
                                                } while(0)




void os_msgQ_init();
MsgQ_t os_msgQ_create( Msg_t *buffer, uint8_t nMessages, uint16_t msgSize, taskproctype taskproc );
MsgQ_t os_msgQ_find( taskproctype taskproc );
//Sem_t os_msgQ_sem_get( MsgQ_t queue );
Evt_t os_msgQ_event_get( MsgQ_t queue );
void os_msgQ_tick( MsgQ_t queue );

uint8_t os_msg_post( Msg_t *msg, MsgQ_t queue, uint16_t delay, uint16_t period );
uint8_t os_msg_receive( Msg_t *msg, MsgQ_t queue );


#ifdef __cplusplus
}
#endif

#endif
