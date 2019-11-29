/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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

/* ------------------------------------------------------------------------ */
/* FILE:			AuroraH264Dec.h											*/
/* DATE:															        */
/* DESCRIPTION:		Interface for the AuroraH264Dec class.					*/
/* ------------------------------------------------------------------------ */
#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#include "H264EnumTypes.h"
#include "H264OffsetTables.h"
#include "H264Tables.h"
#if defined __ARC_MPLAYER__
extern "C" {
#include "memctl.h"
};
#endif









//#else


void AuroraH264Dec::SetDMAFrameTableEntry( unsigned int tableIndex,
                                                    unsigned char * picture_data_y,
                                                    unsigned char * picture_data_u,
                                                    unsigned char * picture_data_v,
                                                    int hsize,
                                                    int vsize )
{
    SDM_Offset sdm_frame_table_add_y;
    SDM_Offset sdm_frame_table_add_u;
    SDM_Offset sdm_frame_table_add_v;

    unsigned int pic_phy_add_y;
    unsigned int pic_phy_add_u;
    unsigned int pic_phy_add_v;

    cur_dma_fr_tbl_idx = tableIndex;    //set the current frame index
        
	sdm_frame_table_add_y = m_dma_y_frames;
	sdm_frame_table_add_u = m_dma_u_frames;
	sdm_frame_table_add_v = m_dma_v_frames;

    // get the address of the correct index
    sdm_frame_table_add_y += (tableIndex*DMA_FR_TBL_ENTRY_SZ);
    sdm_frame_table_add_u += (tableIndex*DMA_FR_TBL_ENTRY_SZ);
    sdm_frame_table_add_v += (tableIndex*DMA_FR_TBL_ENTRY_SZ);

#if defined(__ARC_MPLAYER__)
    pic_phy_add_y = h264_get_physical_adrs( (unsigned int) picture_data_y);
    pic_phy_add_u = h264_get_physical_adrs( (unsigned int) picture_data_u);
    pic_phy_add_v = h264_get_physical_adrs( (unsigned int) picture_data_v);

#else
    pic_phy_add_y = (unsigned int) picture_data_y;
    pic_phy_add_u = (unsigned int) picture_data_u;
    pic_phy_add_v = (unsigned int) picture_data_v;
#endif

	SetDMAFrame(sdm_frame_table_add_y /* frame table offset from sdm-base*/,
					pic_phy_add_y /* picture add in ARC memory */,
					hsize	/* Stride */,
					hsize	/* xDim */,
					vsize	/* yDim */);

	SetDMAFrame(sdm_frame_table_add_u /* frame table offset from sdm-base*/,
					pic_phy_add_u /* picture add in ARC memory */,
					hsize>>1	/* Stride */,
					hsize>>1	/* xDim */,
					vsize>>1	/* yDim */);

	SetDMAFrame(sdm_frame_table_add_v /* frame table offset from sdm-base*/,
					pic_phy_add_v /* picture add in ARC memory */,
					hsize>>1	/* Stride */,
					hsize>>1	/* xDim */,
					vsize>>1	/* yDim */);


}

