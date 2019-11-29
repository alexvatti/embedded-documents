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
/* May contain code (c) British Telecommunications plc 2005        */


#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "EnumTypes.h"
#include "VRaptorInfo.h"
#include "SymbolByteBuffer.h"
#include "MP4EncoderControl.h"
#include "ArcChannelRoutines.h"
#include "MP4Tables.h"
#include "ArcEntropyEncoderModel.h"
#include "ArcMacroRecordSettings.h"
#include "ArcTimer.h"

#define AUXREG_EE_CODEC_CFG  (VRaptorDevs.EntropyEncoder.AuxBase + 3)

#ifndef ARC_ENCODER_API
extern FILE *bitstream_file;
#endif

#define m_TraceFile gCH264Encoder.m_TraceFile

static char start_code[5] = {0,0,0,1,0};


void Initialise_Entropy_Encoder(unsigned int packer_id)
{
  // Set the residual data offset MPC_Y_OFFSET
  WriteToAuxRegEE((VRaptorDevs.EntropyEncoder.AuxBase + 0x23),(MPO_PixelCoeffBuffer + PCB_RESIDUAL_Y));

  if (gCH264Encoder.m_coding_standard == 1)
	  WriteToAuxRegEE(AUXREG_EE_CODEC_CFG, 1);
  else
	  WriteToAuxRegEE(AUXREG_EE_CODEC_CFG, (u_int32)(((gCH264Encoder.m_h263_modified_quant!=0)<<5) |
                                            ((gCH264Encoder.m_h263_aic!=0)<<4) | 2) );
  // Select the packer or concat unit
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0),packer_id);

  // Reset the buffer
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6),0x1);
}

static void UpdateSymbolBuffStatus(unsigned int reg1, unsigned int reg2)
{
   gCH264Encoder.m_ThreadBufferStatus += reg1;
   gCH264Encoder.m_CurrentSymbolByteBuffer[0]->AddBitCount(reg1);
   gCH264Encoder.m_CurrentSymbolByteBuffer[0]->SetEmuByteCnt(reg2);
   
}

/* -----------------------------------------------------------------------------*/
/* FUNCTION:	void VLCEncodeFixedLengthCode(int value);			            */
/* DESCRIPTION:	This writes a fixed length code									*/

/* RETURNS:		void.														    */
/* -----------------------------------------------------------------------------*/
void VLCEncodeFixedLengthCode( unsigned int length,unsigned int value)
{
	if (length == 0)
	{
		return;
	}
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_BIT_OFFSET + length),value);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	SymbolEjectMP4ByteAlign							            */
/* DESCRIPTION:	This writes zero to seven stuffing bits to the symbol buffer
				to achieve byte alignment 01....1 This uses the count of the 
				number of bits in the current picture to determine current 
				bit position, and so relies on the picture starting byte 
				aligned.													*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */
static void SymbolEjectMP4ByteAlign()
{
    VLCEncodeFixedLengthCode( 1, 0); // in MPEG4, there is always stuffing even
                                     // the stream is byte aligned. And the stuffing
                                     // is '0' followed by up to seven '1's. So to keep
                                     // the byte align logic simple in the EE HW, we add
                                     // the first zero here, and let the HW add the ones

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R7), 1); // PACK_TO_BYTE
    // Flush the current buffer
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x2); // BUFFER_CTRL
}

static void ARCClearMbBitCount(void)
{
   // Reset the MB_BIT_CNT as for the last macroblock
   // it has already been added to m_BitCount
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R20),0); // MB_BIT_COUNT
}

static void write_quant_matrix( short *matrix){
    int i;

    if(matrix){
        VLCEncodeFixedLengthCode( 1, 1);
        for(i=0;i<64;i++) {
            VLCEncodeFixedLengthCode( 8, matrix[ ff_zigzag_direct_copy[i] ]);
        }
    }else
        VLCEncodeFixedLengthCode( 1, 0);
}

static void set_mpeg4_time()
{
    int time_div;
	int temp_ref;
	
	temp_ref = gCH264Encoder.m_time/3000; // m_time is in units of 90000 Hz
    time_div= temp_ref/gCH264Encoder.m_time_increment_resolution;
  
    gCH264Encoder.m_last_time_base= gCH264Encoder.m_time_base;
    gCH264Encoder.m_time_base= time_div;
    
}

static int h263_get_picture_format(int width, int height)
{
    int format;

    if (width == 128 && height == 96)		//SQCIF
        format = 1;
    else if (width == 176 && height == 144) // QCIF
        format = 2;
    else if (width == 352 && height == 288)	// CIF
        format = 3;
    else if (width == 704 && height == 576)	// 4CIF
        format = 4;
    else if (width == 1408 && height == 1152)// 16CIF
        format = 5;
    else
        format = 7;
    return format;
}

/*static*/ int CheckForSliceEnd(unsigned int unit_sel)
{
      // Last macroblock in the slice
    if ((MgrMPC.ActMPC[MgrMPC.IdxFromEE].Column == MgrMPC.Picture.Width-1) && 
        (MgrMPC.ActMPC[MgrMPC.IdxFromEE].Row == MgrMPC.Picture.Hgt-1))
	{
		MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit = 0;
        return END_OF_PICT;
	}
	// Check whether there is guaranteed space in the buffer for the contents of the symbol buffer 
	// and the maximum contents of this macroblock.
	// And if not, then consider the slice complete, and consider the current macroblock the first
	// of another slice. This requires any skipped macroblocks in the current slice to be 'flushed'.
    else if( MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit)
		///if (((dummy = (gCH264Encoder.m_CurrentSymbolByteBuffer[0]->GetBitCount() >> 3))
		//	+ (h263_maximum_mb_bits[gCH264Encoder.m_LastTransmittedQuant]>>3)) >= gCH264Encoder.m_MaximumNumberOfBytes

	{

        MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit = 0;

		MgrMPC.CurrentEEMb.SliceStarted = 1;
		MgrMPC.CurrentEEMb.WrSliceHdr = 1;
		// End of slice.  Get next SymbolByteBuffer, increment counter(s)
		if(gCH264Encoder.m_h263_slice_structured || gCH264Encoder.m_video_packets)
		{
            return END_OF_SLICE;
		}
        else
        {
            return IN_SLICE;
        }

    } // if packet size too big

    return IN_SLICE;
}

CHANNEL_SERVICE	Service_WriteMpeg4VolHeader(int vo_number, int vol_number)
{
    int vo_ver_id = 1;
	int vo_type = SIMPLE_VO_TYPE;
    unsigned int addr_val;

    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1);     // Reset EE    
#ifdef ARC_ENCODER_API
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer;
#else
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer;
#endif
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0), 0x4);  // UNIT_SEL: select concat unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R16),0x0);  // EMU_CTRL: turn off emulation prevention unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R14), addr_val); // CONCAT_CUR_ADDR
    ARCClearMbBitCount();
    addr_val = (unsigned int)Service_Mpeg4VolHeaderResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),addr_val);   // Set the SequenceHeader response function address


    VLCEncodeFixedLengthCode( 16, 0);
    VLCEncodeFixedLengthCode( 16, 0x100 + vo_number);        /* video obj */
    VLCEncodeFixedLengthCode( 16, 0);
    VLCEncodeFixedLengthCode( 16, 0x120 + vol_number);       /* video obj layer */

    VLCEncodeFixedLengthCode( 1, 0);		/* random access vol */
    VLCEncodeFixedLengthCode( 8, vo_type);	/* video obj type indication */
    VLCEncodeFixedLengthCode( 1, 1);		/* is obj layer id= yes */
    VLCEncodeFixedLengthCode( 4, vo_ver_id);	/* is obj layer ver id */
    VLCEncodeFixedLengthCode( 3, 1);		/* is obj layer priority */

   // aspect_to_info(s, s->avctx->sample_aspect_ratio);

    VLCEncodeFixedLengthCode( 4, 1 /*s->aspect_ratio_info*/);/* aspect ratio info */
 /*   if (s->aspect_ratio_info == FF_ASPECT_EXTENDED){
        VLCEncodeFixedLengthCode( 8, s->avctx->sample_aspect_ratio.num);
        VLCEncodeFixedLengthCode( 8, s->avctx->sample_aspect_ratio.den);
    }*/

    VLCEncodeFixedLengthCode( 1, 1);		/* vol control parameters= yes */
    VLCEncodeFixedLengthCode( 2, 1);		/* chroma format YUV 420/YV12 */
    VLCEncodeFixedLengthCode( 1, 1/*s->low_delay*/);
    VLCEncodeFixedLengthCode( 1, 0);		/* vbv parameters= no */

    VLCEncodeFixedLengthCode( 2, RECT_SHAPE);	/* vol shape= rectangle */
    VLCEncodeFixedLengthCode( 1, 1);		/* marker bit */

    VLCEncodeFixedLengthCode( 16, gCH264Encoder.m_time_increment_resolution);
    if (gCH264Encoder.m_time_increment_bits < 1)
        gCH264Encoder.m_time_increment_bits = 1;
    VLCEncodeFixedLengthCode( 1, 1);		/* marker bit */
    VLCEncodeFixedLengthCode( 1, 0);		/* fixed vop rate=no */
    VLCEncodeFixedLengthCode( 1, 1);		/* marker bit */
    VLCEncodeFixedLengthCode( 13, gCH264Encoder.m_frame_width_in_mbs<<4);	/* vol width */
    VLCEncodeFixedLengthCode( 1, 1);		/* marker bit */
    VLCEncodeFixedLengthCode( 13, gCH264Encoder.m_frame_height_in_mbs<<4);	/* vol height */
    VLCEncodeFixedLengthCode( 1, 1);		/* marker bit */
    VLCEncodeFixedLengthCode( 1, 0);
    VLCEncodeFixedLengthCode( 1, 1);		/* obmc disable */
    if (vo_ver_id == 1) {
        VLCEncodeFixedLengthCode( 1, 0);		/* sprite enable */
    }else{
        VLCEncodeFixedLengthCode( 2, 0);		/* sprite enable */
    }

    VLCEncodeFixedLengthCode( 1, 0);		/* not 8 bit == false */
    VLCEncodeFixedLengthCode( 1, gCH264Encoder.m_mpeg_quant);	/* quant type= (0=h263 style)*/

    if(gCH264Encoder.m_mpeg_quant){
    //    write_quant_matrix( (short *)ff_mpeg4_default_intra_matrix);
    //    write_quant_matrix( (short *)ff_mpeg4_default_non_intra_matrix);
  //   NB if we use default quant matrices, then we don't neeed to send them
        write_quant_matrix( (short *)NULL);
        write_quant_matrix( (short *)NULL);
    }

  //  if (vo_ver_id != 1)
  //      VLCEncodeFixedLengthCode( 1, s->quarter_sample);
    VLCEncodeFixedLengthCode( 1, 1);		/* complexity estimation disable */
  //  s->resync_marker= s->rtp_mode;
    VLCEncodeFixedLengthCode( 1,/* s->resync_marker ? 0 : 1*/1);/* resync marker disable */
    VLCEncodeFixedLengthCode( 1,/* s->data_partitioning ? 1 : */0);
  ///  if(s->data_partitioning){
   //     VLCEncodeFixedLengthCode( 1, 0);		/* no rvlc */
   // }

    if (vo_ver_id != 1){
        VLCEncodeFixedLengthCode( 1, 0);		/* newpred */
        VLCEncodeFixedLengthCode( 1, 0);		/* reduced res vop */
    }
    VLCEncodeFixedLengthCode( 1, 0);		/* scalability */

	SymbolEjectMP4ByteAlign();
}
CHANNEL_SERVICE	Service_Mpeg4VolHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, 0);
    // Reset the buffer
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1); // BUFFER_CTRL

	gCH264Encoder.m_OverallBufferStatus = gCH264Encoder.m_ThreadBufferStatus;
	gCH264Encoder.m_PictureBitCount += (reg2 << 3); // 8 for the NalRefIDC

	// Copy the encoded sequence parameter set into the byte buffer
	int amount = gCH264Encoder.m_CurrentSymbolByteBuffer[0]->GetLength();
#ifndef ARC_ENCODER_API
#ifndef ENABLE_PERFORMANCE
    fwrite(gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, sizeof(unsigned char), amount, bitstream_file);
    fflush(bitstream_file);
	flush_dcache();
#endif
#ifdef ENABLE_FILE_WRITE

		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, amount*sizeof(unsigned char));
		YUVBuffer.OutputPtr += amount;
		flush_dcache();
#endif 
#else
    gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_Amount+=amount;
// 	fprintf(stderr,"Writing (1) %d bytes to output buffer at 0x%08x, length now %d\n",
// 			amount,gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer,
// 			gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_Amount);
#endif
}

CHANNEL_SERVICE	Service_WriteMpeg4UserData()
{
	unsigned int addr_val;

    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1);     // Reset EE    
#ifdef ARC_ENCODER_API
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer;
#else
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer;
#endif
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0), 0x4);  // UNIT_SEL: select concat unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R16),0x0);  // EMU_CTRL: turn off emulation prevention unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R14), addr_val); // CONCAT_CUR_ADDR
    ARCClearMbBitCount();
    addr_val = (unsigned int)Service_Mpeg4UserDataResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),addr_val);   // Set the SequenceHeader response function address

	// now write version number as user data
    VLCEncodeFixedLengthCode( 24, 1);		/* start code */
	
    VLCEncodeFixedLengthCode( 8, 0xb2);		/* userdata*/


	// loop to insert the product name into the bitstream
	for (int loop=0; loop<6; loop++)
	{
        VLCEncodeFixedLengthCode( 8, ProductName[loop]);
	}

	VLCEncodeFixedLengthCode( 8, '-');
    VLCEncodeFixedLengthCode( 8, 'S');
    VLCEncodeFixedLengthCode( 8, 'v');
	char version[255];
	if(HwMe) 
	{
		sprintf(version,"%2d.%02d",SOFTWARE_MAJOR_VERSION, SOFTWARE_MINOR_VERSION);
	}
	else {
		sprintf(version,"%2d.%02d",SOFTWARE_MAJOR_VERSION, MESOFTWARE_MINOR_VERSION);
	}
	for(int i = 0 ; i < 5; i ++) {

		 VLCEncodeFixedLengthCode( 8, version[i]);
	}
    VLCEncodeFixedLengthCode( 8, '-');
    VLCEncodeFixedLengthCode( 8, 'H');
    VLCEncodeFixedLengthCode( 8, 'v');
	sprintf(version,"%2d.%02d",HARDWARE_MAJOR_VERSION, HARDWARE_MINOR_VERSION);
	for(int i = 0 ; i < 5; i ++) {

		 VLCEncodeFixedLengthCode( 8, version[i]);
	}
	SymbolEjectMP4ByteAlign();
}

CHANNEL_SERVICE	Service_Mpeg4UserDataResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, 0);
    // Reset the buffer
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1); // BUFFER_CTRL

	gCH264Encoder.m_OverallBufferStatus = gCH264Encoder.m_ThreadBufferStatus;
	gCH264Encoder.m_PictureBitCount += (reg2 << 3); // 8 for the NalRefIDC

	// Copy the encoded sequence parameter set into the byte buffer
	int amount = gCH264Encoder.m_CurrentSymbolByteBuffer[0]->GetLength();
#ifndef ARC_ENCODER_API
#ifndef ENABLE_PERFORMANCE
    fwrite(gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, sizeof(unsigned char), amount, bitstream_file);
    fflush(bitstream_file);
	flush_dcache();
#endif
#ifdef ENABLE_FILE_WRITE

		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, amount*sizeof(unsigned char));
		YUVBuffer.OutputPtr += amount;
		flush_dcache();
#endif 
#else
	gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_Amount+=amount;
#endif
}

/* write mpeg4 VOP header */
CHANNEL_SERVICE	Service_WriteMpeg4PictureHeader(unsigned int quant)
{
    int time_incr;
    int time_div, time_mod;

    // Set the picture header response function address
    unsigned int addr_val = (unsigned int)Service_Mpeg4PictureHeaderResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R23),addr_val);   // Set the Picture Header response function address

    // Set the MB end response function address
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R21),(unsigned int)Service_MacroBlockEndResponse);

    // Set the slice type
    unsigned int val = 0;
    if(H264_P_SLICE == gCH264Encoder.m_SliceType)
        val = 1;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R1), val); // SLICE_TYPE

    // Set up the concat unit
#ifdef ARC_ENCODER_API
    unsigned int tmp = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer;
#else
    unsigned int tmp = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer;
#endif
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R14), tmp); // CONCAT_CURR_ADDR
    ARCClearMbBitCount();
    
    set_mpeg4_time();
    
    VLCEncodeFixedLengthCode( 16, 0);	        /* vop header */
    VLCEncodeFixedLengthCode( 16, VOP_STARTCODE);	/* vop header */
    VLCEncodeFixedLengthCode( 2, gCH264Encoder.m_PictureType == H264_P_PICTURE);	/* pict type: I = 0 , P = 1 */
	

    time_div= (gCH264Encoder.m_time/3000)/gCH264Encoder.m_time_increment_resolution;
	time_mod= (gCH264Encoder.m_time/3000)%gCH264Encoder.m_time_increment_resolution;
    time_incr= time_div - gCH264Encoder.m_last_time_base;

    assert(time_incr >= 0);
    while(time_incr--)
        VLCEncodeFixedLengthCode( 1, 1);

    VLCEncodeFixedLengthCode( 1, 0);

    VLCEncodeFixedLengthCode( 1, 1);	/* marker */
    VLCEncodeFixedLengthCode( gCH264Encoder.m_time_increment_bits, time_mod);	/* time increment */
    VLCEncodeFixedLengthCode( 1, 1);	/* marker */
    VLCEncodeFixedLengthCode( 1, 1);	/* vop coded */
    if (    gCH264Encoder.m_PictureType == H264_P_PICTURE
      /*  || (s->pict_type == S_TYPE && s->vol_sprite_usage==GMC_SPRITE)*/) {
	VLCEncodeFixedLengthCode( 1, gCH264Encoder.m_rounding_control);	/* rounding type */
    }
    VLCEncodeFixedLengthCode( 3, 0);	/* intra dc VLC threshold */



	VLCEncodeFixedLengthCode( 5, quant);

    if (gCH264Encoder.m_PictureType != H264_I_PICTURE)
        VLCEncodeFixedLengthCode( 3, 1/*s->f_code*/);	/* fcode_for */

    // Issue a CH_Call command to get the bits encoded
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R25), 1);

}

CHANNEL_SERVICE	Service_Mpeg4PictureHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, reg2);
   if(gCH264Encoder.m_DynamicControl == 1)
   {
		Service_EnableTimer();
   }
}


CHANNEL_SERVICE	Service_WriteMpeg4PacketHeader(int mb_number, unsigned char quant_value)
{
    int i;
	unsigned int addr_val;
    // Set the picture header response function address
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1);     // Reset EE  
#ifdef ARC_ENCODER_API
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer;
#else
    addr_val = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer;
#endif
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0), 0x4);  // UNIT_SEL: select concat unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R16),0x0);  // EMU_CTRL: turn off emulation prevention unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R14), addr_val); // CONCAT_CUR_ADDR
    ARCClearMbBitCount();

    addr_val = (unsigned int)Service_Mpeg4PacketHeaderResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R23),addr_val);   // Set the Picture Header response function address

    VLCEncodeFixedLengthCode( 16+1, 1); // the one is the fcode = 1 in our case

    // encode the current macroblock address
    for(i=1; i<15; i++){
		if(gCH264Encoder.m_NumberOfMacroblocksInPicture <= (1<<(i))) break;
    }
	VLCEncodeFixedLengthCode( i, mb_number);

    // encode quantiser value
	VLCEncodeFixedLengthCode( 5, quant_value);
	VLCEncodeFixedLengthCode( 1, 0 /*m_use_hec*/);
	if(gCH264Encoder.m_use_hec) 
	{ // need time data

	}

    // Issue a CH_Call command to get the bits encoded
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R25), 1);

}

CHANNEL_SERVICE	Service_Mpeg4PacketHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, reg2);
   if(gCH264Encoder.m_DynamicControl == 1)
   {
		Service_EnableTimer();
   }
}

CHANNEL_SERVICE Service_WriteH263PictureHeader(unsigned int quant)
{
    int format, coded_frame_rate, coded_frame_rate_base, i, temp_ref;
    int best_clock_code=1;
    int best_divisor=60;
    int best_error= INT_MAX;

    // Set the picture header response function address
    unsigned int addr_val = (unsigned int)Service_H263PictureHeaderResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R23),addr_val);   // Set the Picture Header response function address

    // Set the MB end response function address
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R21),(unsigned int)Service_MacroBlockEndResponse);

    // Set the slice type
    unsigned int val = 0;
    if(H264_P_SLICE == gCH264Encoder.m_SliceType)
        val = 1;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R1), val); // SLICE_TYPE

    // Set up the concat unit
#ifdef ARC_ENCODER_API
    unsigned int tmp = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_PhysBufferPointer;
#else
    unsigned int tmp = (unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer;
#endif
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R14), tmp); // CONCAT_CURR_ADDR
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0), 0x4);  // UNIT_SEL: select concat unit
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R16),0x0);  // EMU_CTRL: turn off emulation prevention unit
    ARCClearMbBitCount();

    if(gCH264Encoder.m_coding_standard == H263_PLUS){
        for(i=0; i<2; i++){
            int div, error;
            div= (gCH264Encoder.m_frame_rate_base*1800000L + 500L*gCH264Encoder.m_frame_rate) / ((1000L+i)*gCH264Encoder.m_frame_rate);
			div= div < 1 ? 1 : div > 127 ? 127 : div;//clip(1, div, 127);
            error= ABS(gCH264Encoder.m_frame_rate_base*1800000L - (1000L+i)*gCH264Encoder.m_frame_rate*div);
            if(error < best_error){
                best_error= error;
                best_divisor= div;
                best_clock_code= i;
            }
        }
    }

    coded_frame_rate= 1800000;
    coded_frame_rate_base= (1000+best_clock_code)*best_divisor;


    /* Update the pointer to last GOB */

    VLCEncodeFixedLengthCode( 22, 0x20); /* PSC */
	temp_ref = gCH264Encoder.m_time/3000;// m_time is in units of 90000 Hz

   VLCEncodeFixedLengthCode( 8, temp_ref & 0xff); /* TemporalReference */

    VLCEncodeFixedLengthCode( 1, 1);	/* marker */
    VLCEncodeFixedLengthCode( 1, 0);	/* h263 id */
    VLCEncodeFixedLengthCode( 1, 0);	/* split screen off */
    VLCEncodeFixedLengthCode( 1, 0);	/* camera  off */
    VLCEncodeFixedLengthCode( 1, 0);	/* freeze picture release off */

    format = h263_get_picture_format(gCH264Encoder.m_SourcePicture->m_hsize, gCH264Encoder.m_SourcePicture->m_vsize);
    if (gCH264Encoder.m_coding_standard != H263_PLUS) {
        /* H.263v1 */
        VLCEncodeFixedLengthCode( 3, format);
		VLCEncodeFixedLengthCode( 1, (gCH264Encoder.m_PictureType == H264_P_PICTURE)? 1 : 0);
        /* By now UMV IS DISABLED ON H.263v1, since the restrictions
        of H.263v1 UMV implies to check the predicted MV after
        calculation of the current MB to see if we're on the limits */
        VLCEncodeFixedLengthCode( 1, gCH264Encoder.m_umvplus);	/* unrestricted motion vector */
        VLCEncodeFixedLengthCode( 1, 0);	/* SAC: off */
        VLCEncodeFixedLengthCode( 1, 0);	/* advanced prediction mode */
        VLCEncodeFixedLengthCode( 1, 0);	/* not PB frame */
        VLCEncodeFixedLengthCode( 5, quant);
        VLCEncodeFixedLengthCode( 1, 0);	/* Continuous Presence Multipoint mode: off */
    } else {
        int ufep=1;
        /* H.263v2 */
        /* H.263 Plus PTYPE */

        VLCEncodeFixedLengthCode( 3, 7);
        VLCEncodeFixedLengthCode(3,ufep); /* Update Full Extended PTYPE */
        if (format == 7)
            VLCEncodeFixedLengthCode(3,6); /* Custom Source Format */
        else
            VLCEncodeFixedLengthCode( 3, format);

        VLCEncodeFixedLengthCode(1, 0);
        VLCEncodeFixedLengthCode(1, 0); /* Unrestricted Motion Vector */
        VLCEncodeFixedLengthCode(1,0); /* SAC: off */
        VLCEncodeFixedLengthCode(1,0); /* Advanced Prediction Mode */
        VLCEncodeFixedLengthCode(1,gCH264Encoder.m_h263_aic); /* Advanced Intra Coding */
        VLCEncodeFixedLengthCode(1,gCH264Encoder.m_h263_loop_filter); /* Deblocking Filter */
        VLCEncodeFixedLengthCode(1,gCH264Encoder.m_h263_slice_structured); /* Slice Structured */
        VLCEncodeFixedLengthCode(1,0); /* Reference Picture Selection: off */
        VLCEncodeFixedLengthCode(1,0); /* Independent Segment Decoding: off */
        VLCEncodeFixedLengthCode(1,0); /* Alternative Inter VLC */
        VLCEncodeFixedLengthCode(1,gCH264Encoder.m_h263_modified_quant); /* Modified Quantization: */
        VLCEncodeFixedLengthCode(1,1); /* "1" to prevent start code emulation */
        VLCEncodeFixedLengthCode(3,0); /* Reserved */

        VLCEncodeFixedLengthCode( 3, gCH264Encoder.m_PictureType == H264_P_PICTURE);

        VLCEncodeFixedLengthCode(1,0); /* Reference Picture Resampling: off */
        VLCEncodeFixedLengthCode(1,0); /* Reduced-Resolution Update: off */
        VLCEncodeFixedLengthCode(1,gCH264Encoder.m_rounding_control); /* Rounding Type */
        VLCEncodeFixedLengthCode(2,0); /* Reserved */
        VLCEncodeFixedLengthCode(1,1); /* "1" to prevent start code emulation */

        /* This should be here if PLUSPTYPE */
        VLCEncodeFixedLengthCode( 1, 0);	/* Continuous Presence Multipoint mode: off */

		if (format == 7) {
            /* Custom Picture Format (CPFMT) */

            VLCEncodeFixedLengthCode(4,1/*s->aspect_ratio_info*/);// square
            VLCEncodeFixedLengthCode(9,(gCH264Encoder.m_SourcePicture->m_vsize >> 2) - 1);
            VLCEncodeFixedLengthCode(1,1); /* "1" to prevent start code emulation */
            VLCEncodeFixedLengthCode(9,(gCH264Encoder.m_SourcePicture->m_hsize >> 2));
        }

        /* Unlimited Unrestricted Motion Vectors Indicator (UUI) */
        if (gCH264Encoder.m_umvplus)
//            VLCEncodeFixedLengthCode(1,1); /* Limited according tables of Annex D */

            VLCEncodeFixedLengthCode(2,1); /* unlimited */
        if(gCH264Encoder.m_h263_slice_structured)
            VLCEncodeFixedLengthCode(2,0); /* no weird submodes */

        VLCEncodeFixedLengthCode( 5, quant);
    }

    VLCEncodeFixedLengthCode( 1, 0);	/* no PEI */

    if(gCH264Encoder.m_h263_slice_structured){
        VLCEncodeFixedLengthCode( 1, 1);
        assert(MgrMPC.CurrentEEMb.MbNum == 0);
        VLCEncodeFixedLengthCode( ff_mba_length[format-1], MgrMPC.CurrentEEMb.MbNum);
        VLCEncodeFixedLengthCode( 1, 1);
    }

    // Issue a CH_Call command to get the bits encoded
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R25), 1);

}

CHANNEL_SERVICE	Service_H263PictureHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, reg2);
   if(gCH264Encoder.m_DynamicControl == 1)
   {
		Service_EnableTimer();
   }
}

CHANNEL_SERVICE	Service_WriteH263GobHeader(unsigned int quant_value)
{
    // Set the picture header response function address
    unsigned int addr_val = (unsigned int)Service_H263GobHeaderResponse;
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R23),addr_val);   // Set the Picture Header response function address

	if (MgrMPC.CurrentEEMb.MbNum)
        VLCEncodeFixedLengthCode( 17, 1); /* GBSC */

    if(gCH264Encoder.m_h263_slice_structured){
        VLCEncodeFixedLengthCode( 1, 1); // SEPB1

        int format = h263_get_picture_format(gCH264Encoder.m_SourcePicture->m_hsize, gCH264Encoder.m_SourcePicture->m_vsize);
    	VLCEncodeFixedLengthCode( ff_mba_length[format-1], MgrMPC.CurrentEEMb.MbNum);

        if(MgrMPC.CurrentEEMb.MbNum > 1583)
            VLCEncodeFixedLengthCode( 1, 1); // SEPB2

	    VLCEncodeFixedLengthCode( 5, quant_value); /* SQUANT */
		VLCEncodeFixedLengthCode( 1, 1); // SEPB3
		VLCEncodeFixedLengthCode( 2, gCH264Encoder.m_PictureType == H264_I_PICTURE); /* GFID */
		
    }else{
		if(gCH264Encoder.m_h263_rows_per_gob == 2) 
	        VLCEncodeFixedLengthCode( 5, (MgrMPC.CurrentEEMb.Row>>1)); /* GN */
		else 
	        VLCEncodeFixedLengthCode( 5, MgrMPC.CurrentEEMb.Row); /* GN */

        VLCEncodeFixedLengthCode( 2, gCH264Encoder.m_PictureType == H264_I_PICTURE); /* GFID */
        VLCEncodeFixedLengthCode( 5, quant_value); /* GQUANT */
    }

    // Issue a CH_Call command to get the bits encoded
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R25), 1);
}

CHANNEL_SERVICE	Service_H263GobHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    UpdateSymbolBuffStatus(reg1, reg2);
   if(gCH264Encoder.m_DynamicControl == 1)
   {
		Service_EnableTimer();
   }
}

CHANNEL_SERVICE	Service_WriteMacroblockMP4(int cIdx, int dquant)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
    int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
    cMPCcb &lcb = WorkAreaSDM->CircularBuffs[leftIdx];
    cMPCfb &fb = WorkAreaSDM->FixedBuffs[MgrMPC.ActMPC[cIdx].Column];
    int last_mb = 0;
    if(MgrMPC.CurrentEEMb.MbNum == MgrMPC.CurrentEEMb.LstMb)
        last_mb = 0x08;

   // Set if the last macroblock
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), last_mb); // BUFFER_CTRL

   // Set the current,top and left MPC structures
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R17), (unsigned int)&cb); // CURRENT_MPC_PTR

   unsigned int val = 0;
   if(cb.AboveMB.Flags & MBNF_VALID)
      val = (unsigned int)&fb;
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R18), val); // TOP_MPC_PTR

   val = 0;
   if(cb.LeftMB.Flags & MBNF_VALID)
      val = (unsigned int)&lcb;
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R19), val); // LEFT_MPC_PTR

   // DQuant is in the range -26 to 25
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R15), dquant); // DELTA_QUANT

   // Kick off macroblock encoding
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x04); // BUFFER_CTRL

    // Increment MB number
    if (++MgrMPC.CurrentEEMb.Col == MgrMPC.Picture.Width)
    {
        MgrMPC.CurrentEEMb.Col = 0;
        MgrMPC.CurrentEEMb.Row++;
    }
    MgrMPC.CurrentEEMb.MbNum++;
 
}

CHANNEL_SERVICE	Service_MacroBlockEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
	MgrMPC.IdxFromEE = MgrMPC.EEQueue.Pull();
	
#ifdef _DEBUG_VRAPTORISATION
	printf("Service_MacroBlockEndResponse: MPC Index = %d\n", MgrMPC.IdxFromEE);
#endif
	
	// If next MB's DCT has completed and the EE for that MB has not been 
	// triggered yet then do it from here
	short nextIdx = MgrMPC.IdxFromEE + 1;
	if (nextIdx == MAX_CIRC_BUFFS) // better than modulo
		nextIdx = 0;
    if (MgrMPC.ActMPC[nextIdx].IsUsed && MgrMPC.ActMPC[nextIdx].EENotStarted && MgrMPC.ActMPC[nextIdx].FwdTransformsOk)
    {
    	MgrMPC.EEQueue.Push(nextIdx);
    	Service_WriteMacroblockMP4(nextIdx, MgrMPC.ActMPC[nextIdx].Dquant);
    }
	
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxFromEE];
    int tmp = gCH264Encoder.m_ThreadBufferStatus;
    UpdateSymbolBuffStatus(reg1, reg2);
    if (MBCT_MPEG4_SKIPPED == cb.MBCodingType)
        gCH264Encoder.m_ThreadBufferStatus = tmp;
	gCH264Encoder.RateControlMacroblockEnd();

    int mbEnd = CheckForSliceEnd(0);
    switch (mbEnd)
    {
    case END_OF_PICT:
        // Set the CH_FL_ADDR response function to be the end of picture response fct
        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),(unsigned int)Service_PictureEndResponse);
        // Clear the MB_BIT_COUNT
        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R20),0);
        if (gCH264Encoder.m_coding_standard == MPEG4)
        {
            VLCEncodeFixedLengthCode( 1, 0); // in MPEG4, there is always stuffing even
                                             // the stream is byte aligned. And the stuffing
                                             // is '0' followed by up to seven '1's. So to keep
                                             // the byte align logic simple in the EE HW, we add
                                             // the first zero here, and let the HW add the ones
	        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R7), 1); // PACK_TO_BYTE
        }
        else
        {
	        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R7), 0); // PACK_TO_BYTE
        }
        // Flush the current buffer
		if(gCH264Encoder.m_DynamicControl)Service_StopTimer();
		if(gCH264Encoder.m_DynamicControl == 2)
		{
			Service_InitTimer(0x7FFFFFFF); // set to large number
			Service_EnableTimer(); // in order to start it and record cycles between frames		
		}

        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x2); // BUFFER_CTRL
			

        break;

    case END_OF_SLICE:

        // Set the CH_FL_ADDR response function to be the end of slice response fct
        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),(unsigned int)Service_SliceEndResponse);
        if (gCH264Encoder.m_coding_standard == MPEG4)
        {
            VLCEncodeFixedLengthCode( 1, 0); // in MPEG4, there is always stuffing even
                                             // the stream is byte aligned. And the stuffing
                                             // is '0' followed by up to seven '1's. So to keep
                                             // the byte align logic simple in the EE HW, we add
                                             // the first zero here, and let the HW add the ones
	        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R7), 1); // PACK_TO_BYTE
        }
        else
        {
	        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R7), 0); // PACK_TO_BYTE
        }
        // Flush the current buffer
        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x2); // BUFFER_CTRL


	

        break;

    case IN_SLICE:
        MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk = true;
        break;
    default:
        assert(false);
    }
}

CHANNEL_SERVICE	Service_PictureEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
	if(gCH264Encoder.m_DynamicControl == 1) 
	{
		Service_DisableTimer();
	}
    MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk = true;
	// Eject the trailing bits into the last symbol buffer
    UpdateSymbolBuffStatus(reg1, reg2);
    // Reset the buffer
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1); // BUFFER_CTRL
	// Copy the encoded sequence parameter set into the byte buffer
    gCH264Encoder.m_PictureBitCount += (reg2 << 3);
#ifndef ARC_ENCODER_API
#ifndef ENABLE_PERFORMANCE
    fwrite(gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, sizeof(unsigned char), reg2, bitstream_file);
    fflush(bitstream_file);
	flush_dcache();
#endif
#ifdef ENABLE_FILE_WRITE

		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, reg2*sizeof(unsigned char));
		YUVBuffer.OutputPtr += reg2;
		flush_dcache();
#endif 
#else
	gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_Amount+=(reg2);
#endif

    gCH264Encoder.m_CurrentSymbolByteBuffer[0]->ResetBuffer();
}

CHANNEL_SERVICE	Service_SliceEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
	if(gCH264Encoder.m_DynamicControl == 1) 
	{
		Service_DisableTimer();
	}
    MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk = true;
	// Eject the trailing bits into the last symbol buffer
    UpdateSymbolBuffStatus(reg1, reg2);
    // Reset the buffer
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6), 0x1); // BUFFER_CTRL
    // Set a flag to indicate the start of a new packet


	// Copy the encoded sequence parameter set into the byte buffer
    
    gCH264Encoder.m_PictureBitCount += (reg2 << 3);
#ifndef ARC_ENCODER_API
#ifndef ENABLE_PERFORMANCE
    fwrite(gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, sizeof(unsigned char), reg2, bitstream_file);
    fflush(bitstream_file);
	flush_dcache();
#endif
#ifdef ENABLE_FILE_WRITE

		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_BufferPointer, reg2*sizeof(unsigned char));
		YUVBuffer.OutputPtr += reg2;
		flush_dcache();
#endif 
#else
	gCH264Encoder.m_CurrentSymbolByteBuffer[0]->m_Amount+=(reg2);
#endif
    
    gCH264Encoder.m_StartNewPacket = true;
    gCH264Encoder.m_CurrentSymbolByteBuffer[0]->ResetBuffer();
	MgrMPC.CurrentEEMb.SliceStarted = 0;

	if(MgrMPC.CurrentEEMb.WrSliceHdr) // put out slice header
	{
			int quantValue = gCH264Encoder.RateControlQuantSelect();
			if(gCH264Encoder.m_coding_standard == MPEG4)  
			{
				Service_WriteMpeg4PacketHeader(MgrMPC.CurrentEEMb.MbNum+1, quantValue);
				gCH264Encoder.m_LastTransmittedQuant = quantValue;
			}
			else // 263
			{
				if((MgrMPC.CurrentMeMB.Col == 0) && gCH264Encoder.m_h263_use_gob_headers && !(MgrMPC.CurrentMeMB.Row&(gCH264Encoder.m_h263_rows_per_gob-1)))
				{
					Service_WriteH263GobHeader(quantValue);
					gCH264Encoder.m_LastTransmittedQuant = quantValue;
					gCH264Encoder.m_SliceID[0] += MAXIMUM_NUMBER_OF_SLICE_GROUPS;
					Service_ME_SetCollectionID((u_int32) gCH264Encoder.m_SliceID[0]);
					//gCH264Encoder.m_FirstMacroblockInSlice[0] = MgrMPC.CurrentEEMb.MbNum;
				}
			}
			MgrMPC.CurrentEEMb.WrSliceHdr = 0;
	}


}

