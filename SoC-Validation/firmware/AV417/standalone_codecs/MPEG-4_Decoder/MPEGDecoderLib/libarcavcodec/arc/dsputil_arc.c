/*
 * Copyright (c) 2005 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * aurora_revdct.c
 *
 * Nigel Topham, April 2005.
 * Rishabh Uppal, June 2005.
 * Alastair Murray, June 2005.
 */


//#include <stdio.h>
#include "aurora_asm.h"
#include "../dsputil.h"
#include "aurora_defines.h"
#include "aurora_dct.h"
#include "aurora_dma.h"

static void idct_put_aurora (uint8_t* dest, int line_size, int16_t* block)
{
  idct_put_aurora_asm (dest,line_size,block);
}


static void idct_add_aurora (uint8_t* dest, int line_size, int16_t* block)
{
  idct_add_aurora_asm (dest,line_size,block);
}


static void idct_add64_aurora (uint8_t* dest, int line_size, int16_t* block)
{
  idct_add64_aurora_asm (dest,line_size,block);
}

static void idct_add32_aurora (uint8_t* dest, int line_size, int16_t* block)
{
  idct_add32_aurora_asm (dest,line_size,block);
}


/*--------------------------------------------------------------------------*/

void dsputil_init_aurora(DSPContext* c, AVCodecContext *avctx)
{

    /* Set some DMA registers which we never change the value of: */
    VDIWRi(DI6,0x2); /* We clip on reading input. */
    VDOWRi(DO6,0x0); /* No clipping on output, would make no sense. */
    VDIWRi(DI7,/*DMA_TABLE_POINTER*/0x0);
    VDOWRi(DO7,/*DMA_TABLE_POINTER*/0x0);
    c->idct_put = idct_put_aurora;
    c->idct_add = idct_add_aurora;
    c->idct_add32 = idct_add32_aurora;
    c->idct_add64 = idct_add64_aurora;

}
