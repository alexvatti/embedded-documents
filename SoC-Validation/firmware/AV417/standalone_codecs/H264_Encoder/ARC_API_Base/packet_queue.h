
/*                                                         -*-C-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2007 ARC International (Unpublished)                  *
 * All Rights Reserved.                                            *
 *                                                                 *
 * This document, material and/or software contains confidential   *
 * and proprietary information of ARC International and is         *
 * protected by copyright, trade secret and other state, federal,  *
 * and international laws, and may be embodied in patents issued   *
 * or pending.  Its receipt or possession does not convey any      *
 * rights to use, reproduce, disclose its contents, or to          *
 * manufacture, or sell anything it may describe.  Reverse         *
 * engineering is prohibited, and reproduction, disclosure or use  *
 * without specific written authorization of ARC International is  *
 * strictly forbidden.  ARC and the ARC logotype are trademarks of *
 * ARC International.                                              *
 *                                                                 *
 *                                                                 *
 * packet_queue.h                                                  *
 *                                                                 *
 * ARC Video Encoder API output data queue                         *
 *                                                                 */


#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include <constants.h>

#ifdef __cplusplus
extern "C" 
{
#endif 

struct packet_q_entry_t
{
    unsigned char* packet;
    unsigned int length;
};

struct packet_queue_t
{
  struct packet_q_entry_t entries[MAX_OUTPUT_PACKETS];
  unsigned int num_entries;
};

void reset_packet_queue(struct packet_queue_t*);

int arc_encoder_enqueue_data(unsigned char*, 
			     unsigned int, 
			     struct packet_queue_t*);

void send_buffer(unsigned char*);
void return_buffer(unsigned char*);
int is_buffer_free(unsigned char*);

#ifdef __cplusplus
}
#endif 

#endif // PACKET_QUEUE_H
