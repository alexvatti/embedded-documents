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

/* May contain code (c) British Telecommunications plc 2005        */


#ifndef INLCUDED_ARC_ENTROPY_ENCODER_MODEL
#define INLCUDED_ARC_ENTROPY_ENCODER_MODEL
// Creo project Entropy Encoder unit internals

//

// (C) Arc internaltional Ltd. 2006


// This will include functionality level model of the H/W entropy encoder
// and the all the interfaces of the h/W block
// For running the code on the h/w the model will not be used.
// ISS will use this to display all the auxilary registers/ channel registers 
// SDM memory etc

#include <stdio.h>
#include "defines.h"
#include "typdef.h"
#include "ArcMPC.h"
#ifdef ARC_ISS_EE_MODEL
   #include "simext.h"
   #define CALL_BACK_METHOD 1
   #include "../ISS/CallBack.h"
#endif

#define ENDIAN_MASK 0x200

#define EE_I_SLICE 0
#define EE_P_SLICE 1
#define TOTAL_REGS ADD_BITS + 2

#define PCB_VLC_TRANSFORM_U 32*16 
#define PCB_VLC_TRANSFORM_V PCB_VLC_TRANSFORM_U + 16*8
#define PCB_VLC_TRANSFORM 768			//512+128+128
#define PCB_VLC_TRANSFORM_Y_STRIDE  32
#define PCB_VLC_TRANSFORM_UV_STRIDE 16







// Refer the MPC data structure for the description of these fields
typedef struct MpcDataStruct
{
   u_int8 CollectionID;
   NeighbourFlags_t LeftMB;
   NeighbourFlags_t AboveMB;
   u_int8 NonZeroCoeffChroma;
   u_int8 UV_DC_NonZeroCoeff;
   u_int8 MBCodingType;
   u_int8 IntraMode;
   u_int8 QuantValue;
   u_int16 NonZeroCoeffLuma;

   u_int8 PixelCoeffBuffer[PCB_VLC_TRANSFORM];
   int16 Y_DC_CoeffStore0[8];
   int16 Y_DC_CoeffStore1[8];
   int16 U_DC_CoeffStore[4];
   int16 V_DC_CoeffStore[4];
   u_int32 CoeffCountSB[4];

   MV_t mv[4];
   MV_t mvp[4];

}VlcMpcData;

typedef struct TopMpcDataStruct
{
  u_int8 CollectionID;
  u_int32 Intra4x4ModeCoeffCountSB[2]; // Ignore the first 4 bytes as not needed by EE
}TopVlcMpcData;

// Inorder to compile this model with systemC the implementation of the 
// constructor/destructor is moved into the header file.
class ArcEntropyEncoderModel
{
public:
   ArcEntropyEncoderModel();
   ~ArcEntropyEncoderModel();

   void ResetEEModule(void);

   void WriteAuxRegister(unsigned int id, unsigned int val);
   unsigned int ReadAuxRegister(unsigned int id);

#ifdef ARC_ISS_EE_MODEL
   void SetSimulatorAccess(ARC_simext_simulator_access *acc)
   {
      access = acc;
   }

   bool CanEEAcceptChannelCommand(void);
#endif

#ifdef CALL_BACK_METHOD
   void set_callback(call_back_to_worker *eew)
   {
      EEcallback = eew;
   }
#endif

private:

   // AddX_bits
   void WriteFixedLengthCode(unsigned int vaue,unsigned int length);

   void WriteToBufferCtrlReg(unsigned int val);

   int WritePackToByteRegister(unsigned int val,bool update_status=true);

   void WriteToConcatSelRegister(unsigned int val);
   
   void WriteToAddUevRegister(unsigned int val);

   void WriteToAddSevRegister(unsigned int val);

public:   

   void SendChannelCommand(unsigned int ch_data,unsigned int id_cmd);

   struct Response *GetChannelResponse(void)
   {  

      // If there are no pending valid responses then return
      bool val = false;
      for(int i=0;i<MAX_NUM_REGS_TO_RETURN;i++)
      {
         if(ChannelResponse.RegisterData[i].valid)
         {
            val = true;
            break;
         }
      }

      if( !val || EE_AUX_REGS[EE_DEBUG_LOC] & 0x20000 || ChannelResponse.DelayCycles)
         return NULL;

      // If the debug COC bit is set
      // do not send the response
      if(!EEHalted && (EE_AUX_REGS[EE_DEBUG_LOC] & 0x2))
      {
         EE_AUX_REGS[EE_DEBUG_LOC] |= 0x20000;
         EEHalted = true;
         return NULL;
      }

      EEHalted = false;
      SendChannelResponse = ChannelResponse;
      for(int i=0;i<MAX_NUM_REGS_TO_RETURN;i++)
        ChannelResponse.RegisterData[i].valid = false;
      return &SendChannelResponse;
   }

   void SetMBNumber(int n)
   {
       MbNumber = n;
   }

private:
   void BitPacker(unsigned int value,unsigned int length);  // Models the Bit packer of the h/w unit
   void WritePackedBits(unsigned int value, int bytes_valid = 4);
   void ResetBuffer(void);
   void FlushBuffer(void);
   void DoEndianCorrection(unsigned int* ptr);
   void GenerateUnsignedExpGolombCode(int val);
   void EntropyEncodeMacroBlock(void);
   void H264EntropyEncodeMacroBlock(void);
   void GenerateCodedBlockPattern(void);
   void CalculateCodedBlockPatternH264(void);
   void EncodeCodeMBType(void);
   bool EncodeMVAndCBP(void);
   void EncodeDifferentialMotionVector(unsigned char block_index);

   void CalculateCodedBlockPatternH263(void);
   void ArcEncodeMotion(int val,int f_code );
   void ArcEncodeMotionUMV(int val);
   void Mpeg4EntropyEncodeMacroBlock(void);
   void H263EntropyEncodeMacroBlock(void);
   void ArcEncodeBlockMP4(short * block, int n, int intra_dc, unsigned char *scan_table);
   void ArcEncodeBlockH263(short * block, int n);
   void EncodeDcMP4( int level, int n);
   void ArcLoad8x8Block(short *blk, short *buf, int stride);
   void ArcPutEscapeH263(int last, int run, int level);

   inline void ClearLMandCMBBit(void)
   {
         EE_AUX_REGS[BUFFER_CTRL] &= 0x3; 
   }

   void ResetCoefficientVariables(void);
   void ProcessCoefficient(void);
   void DetermineVLCTableForLumaCoeffToken(unsigned int encode_order_block_number);
   void CodeCoefficientsToken(void);
   void EncodeH264Coefficients(void);
   void EncodeChromaACCoefficients(int PixelBuffOffset,const short * bit_pos,short offset);
#ifdef EE_DUMP
   void EncodeLevels(int initial_level, int trailing_ones, bool first_coeff, int block_num, int chroma_num, int index, int mode);
#else
   void EncodeLevels(int initial_level, int trailing_ones, bool first_coeff);
#endif
   void EncodeZeroes(bool dc_coeff_ignored);
   void CodeChromaDCCoefficientsToken(void);
   void CodeChromaDCZeroes(void);
   void DetermineVLCTableForUVCoeffToken(unsigned char encode_order_block_number,const short * bit_pos,short sh_off);
   void PopulateMPCDataStruct(int id);
   void PopulateTopMPCDataStruct(int id);
   void CopyVLCSpecificMPCData(class cMPCcb *src,VlcMpcData *dst);

private:

   // ISS interface
#ifdef ARC_ISS_EE_MODEL
   ARC_simext_simulator_access *access;
#endif

#ifdef CALL_BACK_METHOD
   call_back_to_worker *EEcallback;
#endif
   
   // Array which will be EE auxiliary register
   unsigned int EE_AUX_REGS[TOTAL_REGS];

   // This holds a MAP of the channel registers to the auxiliary
   // register. Each channel index will store the aux reg ID
   unsigned int ChToAuxRegMap[R31];

   enum
   {
      PACKER0,
      PACKER1,
      PACKER2,
      PACKER3,
      CONCAT_UNIT
   };

   struct Response ChannelResponse;
   struct Response SendChannelResponse;
   
   int cbp;

   // Coefficient encoding
   bool FoundFirstCoeff;
	int RunCount;
	int CurrentRun[16];
	int CoefficientArray[16];
	bool FoundFirstNonT1;
	int Coeff;
	unsigned char CoefficientCount;
	int TotalCoeffs;
   int CurrentVLCTableNumber;
   unsigned char NumberOfTrailingOnes;
   unsigned char CoeffTokenTable;
   unsigned char TrailingOnesSignTable[3];
	unsigned char TotalZeroes;

    int MbNumber;

   int IntraModeLuma;

   bool EEHalted;

   
public: 
#ifdef EE_DUMP
   //--------------------------------------------
   // VARIABLES REQUIRED FOR MPEG4/ H263
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_mpeg4_not_coded;
   int mpeg4_not_coded;
   
   bool sample_mpeg4_cbp;
   int mpeg4_cbp_value;
   bool sample_mpeg4_cbpc;

   int mpeg4_cbpc;
   int mpeg4_vop;
   int mpeg4_mb_type;
   int mpeg4_dquant;
   
   bool sample_mpeg4_cbpy;
   int mpeg4_cbpy;

   bool sample_mpeg4_intra_dc_level;
   int mpeg4_intra_dc_level_array[6];

   bool sample_mpeg4_ac;
   int mpeg4_ac_run_array[384];
   int mpeg4_ac_level_array[384];
   int mpeg4_ac_last_array[384];
   int mpeg4_ac_mb_type;
    
   bool sample_h263_not_coded;
   int h263_not_coded;
   
   bool sample_h263_cbp;
   int h263_cbp_value;
   int h263_vop;
   int h263_mb_type;
   int h263_dquant;

   bool sample_h263_cbpc;
   int h263_cbpc;

   bool sample_h263_cbpy;
   int h263_cbpy;

   bool sample_h263_intra_dc_level;
   int h263_intra_dc_level_array[6];

   bool sample_h263_ac;
   int h263_ac_run_array[384];
   int h263_ac_level_array[384];
   int h263_ac_last_array[384];
   
   bool sample_h264_nc;
   int  h264_nc;
   int  h264_chroma_nc;
   
   bool sample_h264_coeff_token;
   int h264_nc_val[25];
   int h264_total_coeffs[25];
   int h264_num_trailing_ones[25];
   int h264_coeff_token_valid_array[25];
   int luma_dc_offset;
   int luma_ac_offset;
   int chroma_ac_offset;
   
   bool sample_h264_trail_ones_sign;
   int  h264_trailing_ones_sign[25];

   bool sample_h264_total_zeros;
   int  h264_total_zeros[25];
   int  h264_total_coeffs_tz[25];
#endif


#ifdef EE_DUMP
   int wr_cnt;
   
   //--------------------------------------------
   // VARIABLES REQUIRED FOR CAVLC
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_delta_quant;
   int delta_quant_value;
   
   bool sample_cbp;
   int cbp_value;
   
   bool sample_mb_type;
   int mb_type_value;
   
   bool sample_run_before;
   int run_before;
   int run_zeros_left;
   
   //--------------------------------------------
   // VARIABLES REQUIRED FOR INTRA 16x16 DC
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_intra16x16_dc_coeff_token;
   int  intra16x16_dc_total_coeffs;
   int  intra16x16_dc_num_trailing_ones;
   
   bool sample_intra16x16_dc_trail_ones_sign;
   int  intra16x16_dc_trailing_ones_sign;

   bool sample_intra16x16_dc_coeff;
   int  intra_dc_lc_array[16];
   int  intra16x16_dc_sl_array[16];

   bool sample_intra16x16_dc_total_zeros;
   int  intra16x16_dc_total_zeros;
   //--------------------------------------------
   
   //--------------------------------------------
   // VARIABLES REQUIRED FOR LUMA AC
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_luma_ac_coeff_token;
   int  luma_ac_total_coeffs;
   int  luma_ac_num_trailing_ones;
   
   bool sample_luma_ac_trail_ones_sign;
   int  luma_ac_trailing_ones_sign;

   bool sample_luma_ac_coeff;
   int luma_ac_lc_array[256];
   int luma_ac_valid_array[256];
   int luma_ac_sl_array[256];

   bool sample_luma_ac_total_zeros;
   int  luma_ac_total_zeros;
   //--------------------------------------------
   
   //--------------------------------------------
   // VARIABLES REQUIRED FOR CHROMA DC
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_chroma_dc_coeff_token;
   int chroma_dc_nc;
   int chroma_dc_total_coeffs;
   int chroma_dc_num_trailing_ones;
   
   bool sample_chroma_dc_trail_ones_sign;
   int chroma_dc_trailing_ones_sign;
   
   bool sample_chroma_dc_coeff;
   int chroma_dc_lc_array[8];
   int chroma_dc_sl_array[8];

   bool sample_chroma_dc_total_zeros;
   int chroma_dc_total_zeros;
   //--------------------------------------------
   
   int coeff_sl_array[400];
   int coeff_lc_array[400];
   int coeff_valid_array[400];
   bool sample_coeff;
   
   //--------------------------------------------
   // VARIABLES REQUIRED FOR CHROMA DC
   // VERIFICATION COVERAGE COLLECTION
   //--------------------------------------------
   bool sample_chroma_ac_coeff_token;
   int chroma_ac_total_coeffs;
   int chroma_ac_num_trailing_ones;
   
   bool sample_chroma_ac_trail_ones_sign;
   int chroma_ac_trailing_ones_sign;
   
   bool sample_chroma_ac_coeff;
   int chroma_ac_lc_array[128];
   int chroma_ac_valid_array[128];
   int chroma_ac_sl_array[128];

   bool sample_chroma_ac_total_zeros;
   int chroma_ac_total_zeros;
   //--------------------------------------------
#endif
   
   VlcMpcData *MpcCurrentPtr;
   TopVlcMpcData *MpcTopPtr;
   VlcMpcData *MpcLeftPtr;
private:
   bool HasChannelResponse;

#ifdef EE_DUMP
public:   
   FILE * verif_log;
#endif

#ifdef MAKE_H264_SYMEJECT_TRACE_FILE
public:
   FILE * m_TraceFile;
   void SetTraceFile(FILE *ptr)
   {
      m_TraceFile = ptr;
   }
#endif
};

#ifndef ARC_ISS_EE_MODEL
   extern ArcEntropyEncoderModel gArcEntropyEncoderModel;
#endif


#endif	// INLCUDED_ARC_ENTROPY_ENCODER_MODEL
