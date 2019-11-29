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
 * Nigel Topham, April 2005.
 *
 */

#include "../dsputil.h"
#include "../mpegvideo.h"
#include "../avcodec.h"
#include "aurora_model.h"
/*
extern void idct_put_aurora(uint8_t*, int, int16_t*);
extern void idct_add_aurora(uint8_t*, int, int16_t*);

extern void put_pixels16_y2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels16_x2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels16_xy2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels8_xy2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels8_x2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels8_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void put_pixels16_aurora(uint8_t *block,uint8_t *pixels,int line_size,int h);
extern void put_pixels8_y2_aurora( uint8_t *block, uint8_t *pixels, int line_size, int h );

extern void avg_pixels16_aurora(uint8_t *block,uint8_t *pixels,int line_size,int h);
extern void avg_pixels8_aurora( uint8_t *block, uint8_t *pixels, int line_size, int h );
extern void avg_pixels16_y2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void avg_pixels16_x2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void avg_pixels16_xy2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void avg_pixels8_xy2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void avg_pixels8_x2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
extern void avg_pixels8_y2_aurora( uint8_t *block, const uint8_t *pixels, int line_size, int h);
*/
void MPV_common_init_arc (MpegEncContext *s)
{
#ifdef HAVE_AURORA
    //aurora_reset (); // performance counter reset
	 
    // Test to make sure that the dct required alignments are met.
    if ((((long)(s->q_intra_matrix) & 0x0f) != 0) ||
            (((long)(s->q_inter_matrix) & 0x0f) != 0))
    {
        av_log(s->avctx, AV_LOG_INFO, "Internal Error: q-matrix blocks must be 16-byte aligned "
                "to use Aurora DCT. Reverting to non-aurora version.\n");
        return;
    }
	
//	s->dsp.idct_permutation_type = FF_TRANSPOSE_IDCT_PERM;
// s->dct_quantize = dct_quantize_aurora;
// s->dct_unquantize_h263_intra = dct_unquantize_h263_aurora;
// s->dct_unquantize_h263_inter = dct_unquantize_h263_aurora;
#endif
}
