
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
 * packet_queue.c                                                  *
 *                                                                 *
 * ARC Video Encoder API output data queue                         *
 *                                                                 */


#include <packet_queue.h>

#include <stdio.h>

#define USED_FLAG_OFFSET 5

void reset_packet_queue(struct packet_queue_t* dest)
{
    memset(dest,0,sizeof(struct packet_queue_t));
}

int arc_encoder_enqueue_data(unsigned char* inpacket, 
			     unsigned int inlength, 
			     struct packet_queue_t* dest)
{
    if (dest && dest->num_entries < MAX_OUTPUT_PACKETS)
    {
	send_buffer(inpacket);
	dest->entries[dest->num_entries].packet = inpacket;
	dest->entries[dest->num_entries].length = inlength;
	dest->num_entries++;
	return 0;
    }
    return -1;
}

void send_buffer(unsigned char* buf)
{
    *(buf - USED_FLAG_OFFSET) = (unsigned char) 0xff;
}

void return_buffer(unsigned char* buf)
{
    *(buf - USED_FLAG_OFFSET) = (unsigned char) 0;
}

int is_buffer_free(unsigned char* buf)
{
    return *(buf - USED_FLAG_OFFSET) ? 0 : 1;
}
