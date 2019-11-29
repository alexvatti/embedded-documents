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
/* FILE NAME:       AuroraH264Dec.cpp                                       */
/* DATE:															        */
/* DESCRIPTION:		SIMD code Chroma inter prediction.                      */
/*																			*/
/* ------------------------------------------------------------------------ */
#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

#include <memory.h>

#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#if defined __ARC_MPLAYER__
#include "memctl.h"
#endif

AuroraH264Dec::AuroraH264Dec() : ARCMedia()
{
        _vr00 = _vmovzw(0,0);
        
        _vr01 = _vmvzw(_vr00,255);
        _vr02 = _vmvzw(_vr00,255);
        _vr03 = _vmvzw(_vr00,255);
        _vr04 = _vmvzw(_vr00,255);
        _vr05 = _vmvzw(_vr00,255);
        _vr06 = _vmvzw(_vr00,255);
        _vr07 = _vmvzw(_vr00,255);
        _vr08 = _vmvzw(_vr00,255);
        _vr09 = _vmvzw(_vr00,255);

        _vr10 = _vmvzw(_vr00,255);
        _vr11 = _vmvzw(_vr00,255);
        _vr12 = _vmvzw(_vr00,255);
        _vr13 = _vmvzw(_vr00,255);
        _vr14 = _vmvzw(_vr00,255);
        _vr15 = _vmvzw(_vr00,255);
        _vr16 = _vmvzw(_vr00,255);
        _vr17 = _vmvzw(_vr00,255);
        _vr18 = _vmvzw(_vr00,255);
        _vr19 = _vmvzw(_vr00,255);

        _vr20 = _vmvzw(_vr00,255);
        _vr21 = _vmvzw(_vr00,255);
        _vr22 = _vmvzw(_vr00,255);
        _vr23 = _vmvzw(_vr00,255);

    cur_dma_fr_tbl_idx = 0; //init the table index to zero, this prevents illegal mem access
//	cout << "Arc SIMD Test bench class (AuroraH264Dec) instance created" << endl;
}

//bool AuroraH264Dec::AllocateDMAFrame()
//{
////	unsigned int num_bytes_required;
////	SDM_Offset offset;
////    printf("AllocateDMAFrame\n");
////
////	offset =  GetFreePtrSDM() & 0x1ff;
////	if( offset != 0 )
////	{
////		num_bytes_required = 16*32 + (512-offset);
////	}
////	else
////	{
////		num_bytes_required = 16*32;
////	}
////
////	SDM_Offset nOffset  = MallocSDM(num_bytes_required);
////	if(nOffset == -1)
////		return false;
////
////    if(offset != 0)
////    {
////	    m_dma_frames	= nOffset + (512-offset);
////    }
////    else
////    {
////	    m_dma_frames	= nOffset;
////    }
////
////    //we assume the the next available memory location is 512 bytes aligned
////    m_dma_y_frames = MallocSDM(16*32);
////    m_dma_u_frames = MallocSDM(16*32);
////    m_dma_v_frames = MallocSDM(16*32);
////
////   	m_dma_y_frm_offst_ls9 = m_dma_y_frames >> 9;
////   	m_dma_u_frm_offst_ls9 = m_dma_u_frames >> 9;
////   	m_dma_v_frm_offst_ls9 = m_dma_v_frames >> 9;
//
//	return true;
//}

bool AuroraH264Dec::InitSIMDMemory()
{
	
//	SDM_Buffers.
//	m_sdm_BaseAddr = (unsigned int)BaseSDM;
//	//allocate memory for DMA frames
//	if(!AllocateDMAFrame())
//		return false;

//////////////////////////////////////////////
// Needs to be replaced with static allocation
//////////////////////////////////////////////
//    printf("InitSIMDMemory\n");
//    //assign memory for Inter-prediction calculations
//    m_y_work_buf		= MallocSDM(24*24);    // yDIM=xDIM=24
//	m_u_work_buf		= MallocSDM(10*16);    // yDIM=10 xDIM=16
//	m_v_work_buf		= MallocSDM(10*16);    // yDIM=10 xDIM-16
//
//        if(m_y_work_buf != sdm_y_work_buf)
//        {
//            printf("Error with buffer address constants\n");
//            printf("sdm_y_work_buf %08x\n",m_y_work_buf);
//            printf("sdm_u_work_buf %08x\n",m_u_work_buf);
//            printf("sdm_v_work_buf %08x\n",m_v_work_buf);
//            printf("TEMP during opt phase:: Please hack to fix\n");
//            return false;
//        }
//
//	m_ver_half_pel_unrnd_buf	= MallocSDM(16*24*2);      // yDIM=16 xDIM=24
//	m_ver_half_pel_rnd_buf		= MallocSDM(16*24);// yDIM=16 xDIM=24
//    m_hor_half_pel_buf          = MallocSDM(16*24);// yDIM=16 xDIM=24
//                                        //-- hor filtering is applied only on relevant rows
//                                        
//    m_centre_pel_buf			= MallocSDM(16*24);// yDIM=16 xDIM=24
//                                        //-- though max reqd xDIM is 16, but for compatibility 
//                                        // with other inp arrays to QPEL-filtering use xDIM=24
//    
//	m_y_topline					= MallocSDM(32*1);
//	m_u_topline					= MallocSDM(16*1);
//	m_v_topline					= MallocSDM(16*1);
//	m_intra_plmode_coeffs		= MallocSDM(2*8*3);

	// print the pointer values
//	printf("m_inter_pred_in_out_buf  = 0x%X\n", m_inter_pred_in_out_buf);
//	printf("m_ver_half_pel_unrnd_buf = 0x%X\n", m_ver_half_pel_unrnd_buf);
//	printf("m_ver_half_pel_rnd_buf   = 0x%X\n", m_ver_half_pel_rnd_buf);
//	printf("m_hor_half_pel_buf       = 0x%X\n", m_hor_half_pel_buf);
//	printf("m_centre_pel_buf         = 0x%X\n", m_centre_pel_buf);

//	// if any of the pinters are NULL return false
//	if( (m_y_work_buf==NULL) || (m_ver_half_pel_unrnd_buf==NULL) ||
//		(m_u_work_buf == NULL) ||	(m_ver_half_pel_rnd_buf==NULL) || 
//		(m_hor_half_pel_buf==NULL) || (m_centre_pel_buf==NULL) || 
//		(m_y_topline == NULL) || (m_u_topline==NULL) || 
//		(m_v_topline==NULL) || (m_intra_plmode_coeffs == NULL) )
//	{
//		printf("Error in allocating SDM fixed buffers\n");
//		return false;
//	}

	// initialize planar mode coeffs
	int x;
	for(x=0; x<8; x++)
	{
		// coeffs for 16x16 luma modes
		*((short *)(m_intra_plmode_coeffs+(int)SDM_Buffers.GetSDM_BaseAddress()) + x) = x+1;
		*((short *)(m_intra_plmode_coeffs+(int)SDM_Buffers.GetSDM_BaseAddress()) + (15-x) ) = (-1) * (x);

		//coeffs for 8x8 chroma modes
		*((short *)(m_intra_plmode_coeffs+(int)SDM_Buffers.GetSDM_BaseAddress()) + 16 + x) = (x - 3);
	}
	return true;
}


AuroraH264Dec::~AuroraH264Dec()
{

}




void AuroraH264Dec::WaitForSIMDFree()
{
	// wait till VEB is zero
    _nop();
    _nop();
    while(_lr(0xa2) & 0x1e0)
    {
//      printf("#");
    }
//  printf("\n");
	return;
}

_Asm void SDMMemClrAsm(unsigned int sdm_buf_add_lower_half,  
                                unsigned int sdm_buf_add_upper_half)
{
	% reg sdm_buf_add_lower_half, sdm_buf_add_upper_half
.off	percent_reg

	vmovw	vr00, sdm_buf_add_lower_half, 2 ;// load i1 with sdm_buf_add_lower_half
	vmovw	vr00, sdm_buf_add_upper_half, 4 ;// load i1 with sdm_buf_add_lower_half

.on	percent_reg
}

_Asm int Macro_SDMMemClrAsm(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vsubw	vr01, vr01, vr01	;	// vr01 = 00 00 00 00 00 00 00 00

	vst128	vr01, [i1, 0]	;
	vst128	vr01, [i1, 16]	;
	vst128	vr01, [i1, 32]	;
	vst128	vr01, [i1, 48]	;
	vst128	vr01, [i1, 64]	;
	vst128	vr01, [i1, 80]	;
	vst128	vr01, [i1, 96]	;
	vst128	vr01, [i1, 112]	;

	vst128	vr01, [i1, 128]	;
	vst128	vr01, [i1, 144]	;
	vst128	vr01, [i1, 160]	;
	vst128	vr01, [i1, 176]	;
	vst128	vr01, [i1, 192]	;
	vst128	vr01, [i1, 208]	;
	vst128	vr01, [i1, 224]	;
	vst128	vr01, [i1, 240]	;

	vst128	vr01, [i1, 256]	;
	vst128	vr01, [i1, 272]	;
	vst128	vr01, [i1, 288]	;
	vst128	vr01, [i1, 304]	;
	vst128	vr01, [i1, 320]	;
	vst128	vr01, [i1, 336]	;
	vst128	vr01, [i1, 352]	;
	vst128	vr01, [i1, 368]	;

	vst128	vr01, [i2, 0]	;
	vst128	vr01, [i2, 16]	;
	vst128	vr01, [i2, 32]	;
	vst128	vr01, [i2, 48]	;
	vst128	vr01, [i2, 64]	;
	vst128	vr01, [i2, 80]	;
	vst128	vr01, [i2, 96]	;
	vst128	vr01, [i2, 112]	;

	vst128	vr01, [i2, 128]	;
	vst128	vr01, [i2, 144]	;
	vst128	vr01, [i2, 160]	;
	vst128	vr01, [i2, 176]	;
	vst128	vr01, [i2, 192]	;
	vst128	vr01, [i2, 208]	;
	vst128	vr01, [i2, 224]	;
	vst128	vr01, [i2, 240]	;

	vst128	vr01, [i2, 256]	;
	vst128	vr01, [i2, 272]	;
	vst128	vr01, [i2, 288]	;
	vst128	vr01, [i2, 304]	;
	vst128	vr01, [i2, 320]	;
	vst128	vr01, [i2, 336]	;
	vst128	vr01, [i2, 352]	;
	vst128	vr01, [i2, 368]	;

//-------------------------------------
    vendrec r0              ;
.on     percent_reg
}

void AuroraH264Dec::SDMMemClear( SDM_Offset memOfs )
{
	// clear (256 + 2x64)words starting at the given memory location

    SDMMemClrAsm(memOfs, memOfs + 384 );

    RecMgr.Run(AM_SDM_Mem_Clear);    

}

void AuroraRecMgr::RecordSDMMemClearMacro()
{
    Record(AM_SDM_Mem_Clear, Macro_SDMMemClrAsm(NextAddr));
}
