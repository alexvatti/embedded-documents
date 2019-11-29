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
/* DESCRIPTION:		Interface for the AuroraH264Dec class.					*/
/* DATE:															        */
/* ------------------------------------------------------------------------ */


#if !defined(_AURORA_H264_DEC_H_)
#define _AURORA_H264_DEC_H_

#include "AuroraRecMgr.h"

#define		LUMA_IN_OUT_BUF_X_SIZE		24
#define		CHROMA_IN_OUT_BUF_X_SIZE	16
#define		BT_WORKING_ARRAY_SIZE		21

#define 	DMA_FR_TBL_ENTRY_SZ  	16     /* Size of each DMA frame table entry in bytes */

typedef enum
{
	SIMD_H264_UNKNOWN_REGION,
	SIMD_H264_FORWARD_REGION,
	SIMD_H264_BACKWARD_REGION,
	SIMD_H264_BIDIRECTIONAL_REGION,
	SIMD_H264_INTRA_REGION,
	SIMD_H264_DIRECT_REGION		// Has to be translated to H264_BIDIRECTIONAL before use
}	SIMD_H264_PARTITION_DIR_T;


_Asm void WaitForDMAIn_ASM(  )
{
    vdiwr           %dr0,	0            ;//    SDM destination addr

}

_Asm void WaitForDMAOut_ASM(  )
{
    vdowr           %dr0,	0            ;//    SDM destination addr

}


_Asm void AsmDMAIn(unsigned int sdm_input_array_add, unsigned int dma_blk_reg, unsigned int sdm_frame_table_add_sd8, int x ,int y, int sdm_stride)
{
    %reg sdm_input_array_add, dma_blk_reg, sdm_frame_table_add_sd8, x, y, sdm_stride
    vdiwr           %dr0,sdm_input_array_add            ;//    SDM destination addr
    vdiwr           %dr1,sdm_stride                     ;//    SDM Stride always 24
    vdiwr           %dr2,dma_blk_reg                    ;//    idx 0, blk_ver_size<<8 || block_hor_size
    vdiwr           %dr6,2                              ;//    Clip mode - NON INTERLACE
    vdiwr           %dr7, sdm_frame_table_add_sd8       ;//    SDM frame table address

        // set the values for LOC_X and LOC_Y
    vdirun          x,y                                 ;//    Do DMA
    
}

_Asm void ASM_DMA_Out(unsigned int macroblk_add_sdm, unsigned int table_adds8 , int locX, int locY, unsigned int dma_blk_reg, unsigned int sdm_stride)
{
    %reg macroblk_add_sdm, table_adds8, locX, locY, dma_blk_reg, sdm_stride
      vdowr   %dr0, macroblk_add_sdm        ;//    SDM addr
      vdowr   %dr1, sdm_stride                          ;//    SDM Stride always 16
      vdowr   %dr2, dma_blk_reg                      ;//    idx 0, (16<<8) | 16
      vdowr   %dr6, 0                           ;//    mode - NON INTERLACE
      vdowr   %dr7, table_adds8                 ;//    SDM frame table address

      vdorun  locX,locY               ;//    Do DMA

}


_Asm void ReconLuma_HBlk(unsigned int coef_data_sdm_add,
								   unsigned int fil_pel_sdm_add,
								   unsigned int rec_out_sdm_add)
{
	%reg coef_data_sdm_add, fil_pel_sdm_add, rec_out_sdm_add
.off	percent_reg

	// load inp and output pointers
	vmovw	vr00, coef_data_sdm_add, 2		;	// load i1 with r1 - coef vals
	vmovw	vr00, fil_pel_sdm_add, 4		;	// load i2 with r2 - filtered data
	vmovw	vr00, rec_out_sdm_add, 8		;	// load i3 with r3

.on	percent_reg
}

_Asm void LumaIntra16x8VerPred( unsigned int sdm_top_line_add,
										unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg sdm_top_line_add, coef_buf_add, out_mcblk_add

.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add

.on	percent_reg
}

_Asm void LumaIntra16x4HorPred( unsigned int sdm_left_line_add,
										unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg sdm_left_line_add, coef_buf_add, out_mcblk_add

.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, sdm_left_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add

.on	percent_reg
}

_Asm void Luma16DCConstMask( )
{

.off	percent_reg
	// clear vr01
	vsubw	vr01, vr01, vr01	;
	vbaddw	vr01, vr01, 128		; 
.on	percent_reg
}

_Asm void Luma16DCVerMask( unsigned int sdm_top_line_add )
{
	% reg sdm_top_line_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
.on	percent_reg
}


_Asm void Luma16DCHorMask( unsigned int  sdm_left_line_add )
{
	% reg sdm_left_line_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_left_line_add, 2	;	// load i1 with sdm_left_line_add
.on	percent_reg
}


_Asm void Luma16DCVerAndHorMask( unsigned int sdm_top_line_add,
							unsigned int sdm_left_line_add )
{
	% reg sdm_top_line_add, sdm_left_line_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, sdm_left_line_add, 4	;	// load i2 with sdm_top_line_add

.on	percent_reg
}


_Asm void LumaIntra16x8DCPred(	unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg coef_buf_add, out_mcblk_add

.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add

.on	percent_reg
}



	// calculate a/b/c in vr01/vr02/vr03
_Asm void Luma16x16CalcPlaneModeParams(  unsigned int sdm_top_line_add,
							unsigned int sdm_left_line_add ,
							unsigned int sdm_intra_plprd_coeffs)
{
	% reg sdm_top_line_add, sdm_left_line_add, sdm_intra_plprd_coeffs

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, sdm_left_line_add, 4	;	// load i2 with sdm_top_line_add
	vmovw	vr00, sdm_intra_plprd_coeffs, 8 ; 

.on	percent_reg
}


_Asm void Luma16x4IntraPlanarPred(unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg coef_buf_add, out_mcblk_add

.off	percent_reg

    // load pointers
    vmovw	vr00, coef_buf_add, 2	;	// load i1 with sdm_top_line_add
	vmovw	vr00, out_mcblk_add, 4	; 
	vmovzw	vr23, 64,2				; Ptr increments
	vmovw	vr23, 32,4	 

.on	percent_reg
}



class AuroraH264Dec : public ARCMedia
{
private:

//	unsigned int	m_sdm_BaseAddr;
	
	unsigned int	cur_dma_fr_tbl_idx;

	int m_block_ver_size;
	int m_block_hor_size;
	
//	bool AllocateDMAFrame();

public:

    // Static memory allocation for SDM
    // Address relative to start of SDM
    enum sdm_offsets
    {
        m_dma_frames            = 0x0000,    // offset to DMA frames
        m_dma_y_frames          = 0x0200,
        m_dma_u_frames          = 0x0400,
        m_dma_v_frames          = 0x0600,
        
        m_y_work_buf            = 0x0800,       // (24x24)BYTES
        m_u_work_buf            = 0x0a40,       // (9x16)BYTES
        m_v_work_buf            = 0x0ae0,       // (9x16)BYTES
 
        m_ver_half_pel_unrnd_buf= 0x0B80,
        m_ver_half_pel_rnd_buf  = 0x0E80,
        m_hor_half_pel_buf      = 0x1000,
        m_centre_pel_buf        = 0x1180,
        m_y_topline             = 0x1300,
        m_u_topline             = 0x1320,
        m_v_topline             = 0x1330,
        m_intra_plmode_coeffs   = 0x1340,
        
        m_dma_y_frm_offst_ls9   = m_dma_y_frames >> 9,
        m_dma_u_frm_offst_ls9   = m_dma_u_frames >> 9,
        m_dma_v_frm_offst_ls9   = m_dma_v_frames >> 9,
//#ifdef INLOOP_DBLOCK 
        m_top_intra_Y = 0x1370,
        m_top_intra_U = 0x1640,
        m_top_intra_V = 0x17A8,
        m_top_left_intra = 0x1910,
        m_top_dblock_Y = 0x1920,
        m_top_dblock_U =0x4620,
        m_top_dblock_V =0x5160,
//#endif        
        
    };

	// Construction
    AuroraH264Dec();
	virtual ~AuroraH264Dec();
	bool InitSIMDMemory();
	unsigned char * GetRefBlkInpBuffer();

	static void WaitForSIMDFree();

	inline SDM_Offset GetLumaWorkBuffer() const
	{
		return m_y_work_buf;
	}

	inline SDM_Offset GetChrUWorkBuffer() const
	{
		return m_u_work_buf;
	}

	inline SDM_Offset GetChrVWorkBuffer() const
	{
		return m_v_work_buf;
	}

	inline SDM_Offset GetLumaTopLineBuf() const
	{
		return m_y_topline;
	}

	inline SDM_Offset GetChromaUTopLineBuf() const
	{
		return m_u_topline;
	}

	inline SDM_Offset GetChromaVTopLineBuf() const
	{
		return m_v_topline;
	}


	/* SUB-PEL DMA FUNCTIONS*/
	void GetRefBlockInSDM(unsigned char * ref_picture_data,
									    unsigned char *sdm_work_buf,
										int ref_hor_size,
										int ref_ver_size,
										int ref_hor_offset,
										int ref_ver_offset,
										int block_hor_size,
										int block_ver_size,
										int sdm_stride);

	void SetDMAFrameTableEntry( unsigned int tableIndex,
	                                unsigned char * m_y,
	                                unsigned char * m_u,
	                                unsigned char * m_v,
	                                int hsize,
	                                int vsize );
        


// move data from SDM to Main memory
inline void MoveLumaMacroBlock2Pic( SDM_Offset macroblk_add_sdm,
									 int    mcblk_hor_offset,
									 int    mcblk_ver_offset)
{
/*#ifdef INLOOP_DBLOCK
	//if(m_InloopDeblock)
	//{
		// copy reconstructed Macroblock data to Deblocking buffer
		int db_buf = AuroraH264Dec::m_y_work_buf + (24 * 4)+ 8;
		int recon_buf = macroblk_add_sdm;
		_vr00 = _vmovw(recon_buf, 0x1);       // %i0
	    _vr00 = _vmovw(db_buf, 0x2);       // %i1
	    //simdu->RecMgr.Run(AM_COPY16X16_OFF_16_24_DBINPUT);
	    RecMgr.Run(AM_COPY16X16_OFF_16_24_DBINPUT);
	//}
	//else
	//{
#else*/
		unsigned int dma_blk_reg=(0x1010)|(cur_dma_fr_tbl_idx<<16);
	
		ASM_DMA_Out(macroblk_add_sdm, m_dma_y_frm_offst_ls9, 
	            mcblk_hor_offset, mcblk_ver_offset, dma_blk_reg, 16);
	//}
//#endif    
}

inline void MoveLumaMacroBlock2DBBuffer( SDM_Offset macroblk_add_sdm)
{
	// copy reconstructed Macroblock data to Deblocking buffer
	int db_buf = AuroraH264Dec::m_y_work_buf + (24 * 4)+ 8;
	int recon_buf = macroblk_add_sdm;
	_vr00 = _vmovw(recon_buf, 0x1);       // %i0
	_vr00 = _vmovw(db_buf, 0x2);       // %i1
	RecMgr.Run(AM_COPY16X16_OFF_16_24_DBINPUT);
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:    AuroraH264Dec::MoveChromaMacroBlock2Pic                     */
/* DESCRIPTION:                                                             */
/* RETURNS:     move data from sdm_ptr to mem_ptr                           */
/* ------------------------------------------------------------------------ */
inline void MoveChromaUMacroBlock2Pic( SDM_Offset macroblk_add_sdm,
                                             int    mcblk_hor_offset,
                                             int    mcblk_ver_offset)
{
/*#ifdef INLOOP_DBLOCK
	//copy reconstructed Macroblock data to Deblocking buffer
	int db_buf = AuroraH264Dec::m_u_work_buf + (16 * 2)+ 8;
	int recon_buf = macroblk_add_sdm;
	_vr00 = _vmovw(recon_buf, 0x1);       // %i0
    _vr00 = _vmovw(db_buf, 0x2);       // %i1
    RecMgr.Run(AM_COPY8X8_OFF_8_16_DBINPUT);

#else*/    	
      _vdowr(_dr0, macroblk_add_sdm    )        ;//    SDM addr
      _vdowr(_dr1, 8                   )        ;//    SDM Stride always 16
      _vdowr(_dr2, (0x808)|(cur_dma_fr_tbl_idx<<16)   )             ;//    idx 0, (16<<8) | 16
      _vdowr(_dr6, 0                   )        ;//    mode - NON INTERLACE
      _vdowr(_dr7, m_dma_u_frm_offst_ls9 )      ;//    SDM frame table address

      _vdorun(mcblk_hor_offset,mcblk_ver_offset);//    Do DMA
//#endif          
}

inline void MoveChromaUMacroBlock2DBBuffer( SDM_Offset macroblk_add_sdm)
{
	//copy reconstructed Macroblock data to Deblocking buffer
	int db_buf = AuroraH264Dec::m_u_work_buf + (16 * 2)+ 8;
	int recon_buf = macroblk_add_sdm;
	_vr00 = _vmovw(recon_buf, 0x1);       // %i0
	_vr00 = _vmovw(db_buf, 0x2);       // %i1
	RecMgr.Run(AM_COPY8X8_OFF_8_16_DBINPUT);
}
// Assumes above called first
inline void MoveChromaVMacroBlock2Pic( SDM_Offset macroblk_add_sdm,
                                             int    mcblk_hor_offset,
                                             int    mcblk_ver_offset)
{
/*#ifdef INLOOP_DBLOCK
	//copy reconstructed Macroblock data to Deblocking buffer
	int db_buf = AuroraH264Dec::m_v_work_buf + (16 * 2)+ 8;
	int recon_buf = macroblk_add_sdm;
	_vr00 = _vmovw(recon_buf, 0x1);       // %i0
	_vr00 = _vmovw(db_buf, 0x2);       // %i1
	RecMgr.Run(AM_COPY8X8_OFF_8_16_DBINPUT);
#else*/    	
      _vdowr(_dr0, macroblk_add_sdm    )        ;//    SDM addr
//          _vdowr(_dr1, 8                   )        ;//    SDM Stride always 16
//          _vdowr(_dr2, (0x808)|(cur_dma_fr_tbl_idx<<16)   )             ;//    idx 0, (16<<8) | 16
//          _vdowr(_dr6, 0                   )        ;//    mode - NON INTERLACE
      _vdowr(_dr7, m_dma_v_frm_offst_ls9 )      ;//    SDM frame table address

      _vdorun(mcblk_hor_offset,mcblk_ver_offset);//    Do DMA
//#endif         
}

inline void MoveChromaVMacroBlock2DBBuffer( SDM_Offset macroblk_add_sdm)
{
	//copy reconstructed Macroblock data to Deblocking buffer
	int db_buf = AuroraH264Dec::m_v_work_buf + (16 * 2)+ 8;
	int recon_buf = macroblk_add_sdm;
	_vr00 = _vmovw(recon_buf, 0x1);       // %i0
	_vr00 = _vmovw(db_buf, 0x2);       // %i1
	RecMgr.Run(AM_COPY8X8_OFF_8_16_DBINPUT);
}


//#if defined(AURORA_DMA)
	inline void GetLumaRefBlockInSDM(int frame_tbl_idx,
    									    SDM_Offset sdm_work_buf,
    										int ref_hor_offset,
    										int ref_ver_offset,
    										int block_hor_size,
    										int block_ver_size,
    										int sdm_stride )
	{	
	    AsmDMAIn(sdm_work_buf, (frame_tbl_idx<<16) | (block_ver_size<<8) | (block_hor_size), m_dma_y_frm_offst_ls9, 
	                                ref_hor_offset , ref_ver_offset,sdm_stride);
	    WaitForDMAIn();
	}

	inline void GetChromaURefBlockInSDM(int frame_tbl_idx,
    									    SDM_Offset sdm_work_buf,
    										int ref_hor_offset,
    										int ref_ver_offset,
    										int block_hor_size,
    										int block_ver_size,
    										int sdm_stride )
	{
	    AsmDMAIn(sdm_work_buf, (frame_tbl_idx<<16) | (block_ver_size<<8) | (block_hor_size), m_dma_u_frm_offst_ls9, 
	                                ref_hor_offset , ref_ver_offset,sdm_stride);
	
	    WaitForDMAIn();
	}

	inline void GetChromaVRefBlockInSDM(int frame_tbl_idx,
    									    SDM_Offset sdm_work_buf,
    										int ref_hor_offset,
    										int ref_ver_offset,
    										int block_hor_size,
    										int block_ver_size,
    										int sdm_stride )
	{	
	    AsmDMAIn(sdm_work_buf, (frame_tbl_idx<<16) | (block_ver_size<<8) | (block_hor_size), m_dma_v_frm_offst_ls9, 
	                                ref_hor_offset , ref_ver_offset,sdm_stride);
	
	    WaitForDMAIn();
	}

    void GetChromaRefBlockInSDM(int frame_tbl_idx,
                                                int ref_hor_offset,
                                                int ref_ver_offset,
                                                int block_hor_size,
                                                int block_ver_size,
                                                int sdm_stride )
    {
        unsigned int dma_blk_reg;
    
    
        dma_blk_reg = (frame_tbl_idx<<16) | (block_ver_size<<8) | (block_hor_size);
    
        _vdiwr(_dr0,m_u_work_buf )           ;//    SDM destination addr
        _vdiwr(_dr1,sdm_stride   )                  ;//    SDM Stride always 24
        _vdiwr(_dr2,dma_blk_reg  )                  ;//    idx 0, blk_ver_size<<8 || block_hor_size
        _vdiwr(_dr6,2            )                  ;//    Clip mode - NON INTERLACE
        _vdiwr(_dr7, m_dma_u_frm_offst_ls9  )     ;//    SDM frame table address
    
        _vdirun(ref_hor_offset,ref_ver_offset )  ;//    Do DMA
        
        _vdiwr(_dr0,m_v_work_buf )           ;//    SDM destination addr
        _vdiwr(_dr7, m_dma_v_frm_offst_ls9  )     ;//    SDM frame table address
    
        _vdirun(ref_hor_offset,ref_ver_offset )  ;//    Do DMA
        WaitForDMAIn();
    }
                                            
	inline void WaitForDMAIn()
	{
		// write a dummy value in any of the DMA-IN registers
		WaitForDMAIn_ASM();
	}

	inline void WaitForDMAOut()
	{
		// write a dummy value in any of the DMA-IN registers
		WaitForDMAOut_ASM();
	}


	/* Inverse transform*/
	void Inverse2DLumaTransform( SDM_Offset data );
	void Inverse2DChromaTransform( SDM_Offset data);

	/* SubPel Filtering*/
	void SIMDFilterLumaASM( SDM_Offset sdm_work_buf,
								    int fractional_hor_vector,
								    int fractional_ver_vector,
								    int block_hor_size,
								    int block_ver_size);
	
	void SIMDFilterChromaASM(          int fractional_hor_vector,
									    int fractional_ver_vector,
									    int block_hor_size,
									    int block_ver_size);

	void PerformInpHorFiltering( SDM_Offset inp_ptr, 
							  SDM_Offset out_ptr,
							  int output_line_length);
	
	void ShiftBy2AndQPelFiltering( SDM_Offset inp1_ptr,
						SDM_Offset inp2_ptr,
						SDM_Offset out_ptr); // a, e, i, p

	void ShiftBy3AndQPelFiltering( SDM_Offset inp1_ptr,
						SDM_Offset inp2_ptr,
						SDM_Offset out_ptr); // c, g, k, r

	void QPelFiltering( SDM_Offset inp1_ptr,
						SDM_Offset inp2_ptr,
						SDM_Offset out_ptr); // d,f,n,q

	void VerticalFilteringAndRounding(SDM_Offset inp_ptr, 
							  SDM_Offset out_ptr,
							  int horSize,
							  int horOffset);

	void VerticalFilteringWORounding(SDM_Offset inp_ptr, 
							  SDM_Offset out_ptr,
							  int horSize,
							  int horOffset );


	void CentrePelFilterWOInpRounding( SDM_Offset inp_ptr,
							SDM_Offset out_ptr );
	
	void CentrePelFilAndInpRounding( SDM_Offset inp_ptr,
							SDM_Offset out_ptr,
							SDM_Offset rounded_input_ptr );

	/* ------------------------------------------------------------------------ */
	/* FUNCTION:	AuroraH264Dec::SIMDReconLuma        	                    */
	/* DESCRIPTION:							                                    */
	/* RETURNS:		Pointer to a  buffer                                        */
	/* ------------------------------------------------------------------------ */
	inline void SIMDReconLuma( SDM_Offset coef_data_sdm_add, // m_y_data 16x32
	                        				   SDM_Offset  rec_out_sdm_add, // m_y_out_data 16x16
	                        				   SDM_Offset  fil_pel_sdm_add,
	                        				   int block_hor_size,
	                        				   int block_ver_size )
	{
		int rowIndex;
		ReconLuma_HBlk(coef_data_sdm_add, fil_pel_sdm_add, rec_out_sdm_add);
	
		switch(block_hor_size)
		{
	
		case 4:
			// No weighting
			// process 4 rows at a time
			for(rowIndex=0; rowIndex<block_ver_size; rowIndex += 4)
			{
	            RecMgr.Run(AM_Inter_ReconLuma_HBlkSz_4);
			}
			break;
	
		case 8:
	
			// process 4 rows at a time
			for(rowIndex=0; rowIndex<block_ver_size; rowIndex += 4)
			{
	            RecMgr.Run(AM_Inter_ReconLuma_HBlkSz_8);
			}
			break;
	
	
		case 16:
	
			// process 8 rows at a time
			for(rowIndex=0; rowIndex<block_ver_size; rowIndex += 8)
			{
	            RecMgr.Run(AM_Inter_ReconLuma_HBlkSz_16);
			}
	
			break;
		}
	
	
	}

private:
	
public:
	inline void SIMDReconChroma( SDM_Offset sdm_coef_buf_add, // m_u_data 16x32
	                    				   SDM_Offset sdm_mcblk_out_buf_add, // m_y_out_data 16x16
	                    				   SDM_Offset sdm_fwd_work_buf_add,
	                    				   const int block_hor_size,
	                    				   const int block_ver_size )
	{
		
		const short	macro = RecMgr.ChromaBlockMacros[(block_hor_size & 0xc)|(block_ver_size>> 2)];

		_vr00 = _vmovw(sdm_coef_buf_add, 2)		;	// load i1 with inp1 - coef vals
		_vr00 = _vmovw(sdm_fwd_work_buf_add, 4)	;	// load i2 with inp2 - filtered data
		_vr00 = _vmovw(sdm_mcblk_out_buf_add, 8)	;	// load i3 with out
	
		_vrun(macro);
		_vrun(macro);

	    
	}

	void SDMMemClear( SDM_Offset memOfs );


	inline void Luma16x16IntraPred( SDM_Offset sdm_top_line_add,
								SDM_Offset  sdm_left_line_add,
								SDM_Offset coef_buf_add,
								SDM_Offset  out_mcblk_add,
								unsigned int pred_mode,
								unsigned int sub_mode)
	{
		switch(pred_mode)
		{
		case 0:
			// Vertical prediction
			LumaIntra16x8VerPred( sdm_top_line_add+4, coef_buf_add, out_mcblk_add );
	
	        RecMgr.Run(AM_Intra_Luma16x8VerPred);    
	        RecMgr.Run(AM_Intra_Luma16x8VerPred);    
	
			break;
	
		case 1:
		    // Horizontal prediction
	    	LumaIntra16x4HorPred( sdm_left_line_add, coef_buf_add, out_mcblk_add);
            RecMgr.Run(AM_Intra_Luma16x4HorPred);    
            RecMgr.Run(AM_Intra_Luma16x4HorPred);    
            RecMgr.Run(AM_Intra_Luma16x4HorPred);    
            RecMgr.Run(AM_Intra_Luma16x4HorPred);    
			break;
		
		case 2:
				// DC prediction
	
			// construct the mask in vr01
			switch(sub_mode)
			{
			case 0:
				// constant;
				Luma16DCConstMask( );
				break;
	
			case 1:
				//vertical only
				Luma16DCVerMask( sdm_top_line_add+4 );
	
	            RecMgr.Run(AM_Intra_Luma16DCVerMask);    
				break;
			
			case 2:
				//horizontal only
				Luma16DCHorMask( sdm_left_line_add );
	
	            RecMgr.Run(AM_Intra_Luma16DCHorMask);    
				break;
	
			case 3:
				// both horizontal and vertical
				Luma16DCVerAndHorMask( sdm_top_line_add+4,
											sdm_left_line_add );
	
	            RecMgr.Run(AM_Intra_Luma16DCVerAndHorMask);    
				break;
			}
	
			// perform reconstruction
			LumaIntra16x8DCPred( coef_buf_add, out_mcblk_add );
	        RecMgr.Run(AM_Intra_Luma16x8DCPred);    
	        RecMgr.Run(AM_Intra_Luma16x8DCPred);    
			break;
	
		case 3:
			// Plane prediction
			{
				Luma16x16CalcPlaneModeParams( sdm_top_line_add,
									sdm_left_line_add ,
									m_intra_plmode_coeffs);
		
		        RecMgr.Run(AM_Intra_Luma16x16CalcPlaneModeParams);    
		
				int macro = RecMgr.GetMacroAddr(AM_Intra_Luma16x4PlanarPred);
				Luma16x4IntraPlanarPred(coef_buf_add, out_mcblk_add);
		        _vrun(macro);    
		        _vrun(macro);    
		        _vrun(macro);    
		        _vrun(macro);
		         
		        _vrun(macro);    
		        _vrun(macro);    
		        _vrun(macro);    
		        _vrun(macro);    
			
			}
			break;
		}
	        
	}

	void ChromaIntraPred(  SDM_Offset sdm_top_line_ptr,
							SDM_Offset sdm_left_line_ptr,
							SDM_Offset coef_buf_ptr,
							SDM_Offset sdm_mcblk_out_buf_ptr,
							unsigned int pred_mode,
							unsigned int sub_mode);

	void IntraReconPCMLuma( SDM_Offset coef_buf_ptr,
							SDM_Offset sdm_mcblk_out_buf_ptr );

	void IntraReconPCMChroma( SDM_Offset coef_buf_ptr,
							SDM_Offset sdm_mcblk_out_buf_ptr );

// DEBLOCK functions
#ifdef AURORA_DEBLOCK
	inline void LoopFilterEdgeLumaHorzontal(int blockNum,
					const unsigned char * strength,
				//	bool strong_filtering,
					int index_a, 
					int beta);

//	inline void LoopFilterEdgeLumaVertical(int blockNum,
//					const unsigned char * strength,
//				//	bool strong_filtering,
//					int index_a, 
//					int beta);

	void SetupDeblockDMA(unsigned char * pic_y, unsigned char * pic_u, unsigned char * pic_v, int picWidth, int picHeight);
	inline void DMA_inDeblock(int x, int y);
	inline void copy_top_left_Deblock_data(int x, int y);
	
	inline void DMA_outDeblock(int x, int y);
//#ifdef INLOOP_DBLOCK	
	inline void CopyAndDMA_outDeblock(int x, int y, bool dma_macroblock);
	inline void DMA_out_Last_MB_Row(int mbs_in_row, int y_pos);
//#endif
        inline void LoopFilterEdgeChromaHorizontal(int data, int data1,  const unsigned char * strength, int index_a, int beta);
//       inline void LoopFilterEdgeChromaHorizontalStrong(int data, int data1,   int index_a, int beta);
        inline void LoopFilterEdgeChromaVertical(int data, int data1,   const unsigned char * strength, int index_a, int beta);
//        inline void LoopFilterEdgeChromaVerticalStrong(int data, int data1,   int index_a, int beta);
protected:


#endif

public:
        AuroraRecMgr    RecMgr;

};




#endif
