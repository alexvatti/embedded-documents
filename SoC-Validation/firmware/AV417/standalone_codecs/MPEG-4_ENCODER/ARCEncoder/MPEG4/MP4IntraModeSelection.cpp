/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */



#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "CMP4Encoder.h"
#include "MP4EncoderControl.h"
#include "MP4EncoderTables.h"
#include "ArcEEOffsetTables.h"
#include "MP4Tables.h"
#include "MP4Transform.h"
#include "ArcMP4SDMTables.h"
#include "mpeg4data.h"
#include "defines.h"

/* dc encoding for mpeg4 */
const unsigned char DCtab_lum2[13][2] =
{
    {3,3}, {3,2}, {2,2}, {2,3}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7},
    {1,8}, {1,9}, {1,10}, {1,11},
}; 

unsigned char uni_DCtab_lum_len2[512];
void CH264Encoder::init_uni_dc_tab2(void)
{
    int level, uni_len;

    for(level=-256; level<256; level++){
        int size, v;
        /* find number of bits */
        size = 0;
        v = abs(level);
        while (v) {
            v >>= 1;
	    size++;
        }

        /* luminance */
        uni_len = DCtab_lum2[size][1];

        if (size > 0) {
            uni_len+=size;
            if (size > 8){
                uni_len++;
            }
        }
        //uni_DCtab_lum_len2 [level+256]= uni_len;
		WorkAreaSDM->mpeg4_DC_bits_plus_256[level+256] = uni_len;
    }
	//flush_dcache();
}


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

