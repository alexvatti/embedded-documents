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



#ifndef ARC_MOTION_ESTIMATION_MODEL_H
#define ARC_MOTION_ESTIMATION_MODEL_H

#include <stdio.h>
#include "typdef.h"
#include "EnumTypes.h"
#include "ArcMPC.h"
#include "defines.h"

#ifdef USE_CALLBACK
#include "CallBack.h"
#endif

#ifdef ARC_ISS_ME_MODEL
#include "simext.h"
#endif


// ME specific defines for Creo
#define SKIP_VEC_8		// try the skip vector for each 8x8 partition
#define TRY_INT_MVP		// try the prediction vector at integer level
#define TEST_NEIGHBOURS // test neighbouring vectors A, B and C in the integer part of the ME
#define TRY_MVP_NEIGHALL // try 3x3 int values around MVP
#define LIMITS			// check MV not over 16 pels away

#define MAX_FRAMES 2


#ifdef HARDWARE_ME
#define ME_AUX_REG_START_ADDR 0x0000103c
#else
#define ME_AUX_REG_START_ADDR 0x00000000
#endif


enum auxRegName
{
   ME_VCONFIG,
   ME_NAMESTR,
   ME_BUILD,
   ME_REFERENCE_START,
   ME_REFERENCE_STRIDE,
   ME_CURRENT_START,
   ME_CURRENT_STRIDE,
   ME_MB_DIMENSION,
   ME_MB_INDEX,
   ME_MB_SDM_ADDR,
   ME_REFERENCE_SDM_START,
   ME_THRESHOLD_SKIP,
   ME_THRESHOLD_16x16,
   ME_THRESHOLD_BLOCK_PATTERN,
   ME_THRESHOLD_SKIP_MAX,
   ME_CONTROL0,
   ME_CONTROL1,
   ME_SETUP,
   ME_LAMBDA,
   ME_COLLECTION_ID,
   ME_RESULT,
   ME_MB_P0,
   ME_MB_P1,
   ME_MB_P2,
   ME_MB_P3,
   ME_SKIP_BR_ADDR,
   ME_16x16_BR_ADDR,
   ME_8x8_BR_ADDR,
   ME_SDM_BR_ADDR,
   ME_BASE_BR_ADDR,
   ME_D_POINTER,
   ME_A_POINTER,
   ME_CURRENT_POINTER,
   ME_16x8_BR_ADDR,
   ME_8x16_BR_ADDR,
   ME_DEBUG,
   NUM_ME_AUX_REGS
};


// number of channel registers
#define NUM_ME_CHAN_REGS 17

// number of response requests needed before a response is actually returned
#define CHAN_MB_DELAY 0
#define CHAN_FINAL_DELAY 0


const u_int32 MeChannelToAuxRegMap[NUM_ME_CHAN_REGS]=
   {
      ME_REFERENCE_START,
      ME_CURRENT_START,
      ME_MB_SDM_ADDR,
      ME_REFERENCE_SDM_START,
      ME_THRESHOLD_SKIP,
      ME_THRESHOLD_16x16,
      ME_THRESHOLD_BLOCK_PATTERN,
      ME_THRESHOLD_SKIP_MAX,
      ME_CONTROL0,
      ME_CONTROL1,
      ME_LAMBDA,
      ME_COLLECTION_ID,
      ME_SETUP,
      ME_D_POINTER,
      ME_A_POINTER,
      ME_CURRENT_POINTER,
      ME_DEBUG
   };



//******************************************************************************
// Name:          ArcMotionEstimationModel
// Purpose:       Models the Motion Estimation hardware in Creo
//******************************************************************************
class ArcMotionEstimationModel
{
   public:

	   // constructor and deconstructor
	   ArcMotionEstimationModel();
	   ~ArcMotionEstimationModel();


      // public functions

      // auxiliary register functions
      void SetAuxReg (u_int32 addr, u_int32 data);
      u_int32 GetAuxReg (u_int32 addr);


      // Channel functions
#ifdef USE_CALLBACK
      void set_callback (call_back_to_worker *mew);
#endif

      void SendChannelCommand (unsigned int ch_data, unsigned int id_cmd);

      struct Response *GetChannelResponse (void);
      struct Response *SelectChannelResponse (int index);

	  void Software_ME_control(cMPCcb *);
	  void Load_Current_MB_Luma(cMPCcb *me_curr_pointer, int intra);

      // ISS channel responses
      void CreateChannelCommandMB(void);
#ifdef ARC_ISS_ME_MODEL
      void SetSimulatorAccess(ARC_simext_simulator_access *acc);
#endif

#ifdef ME_CYCLES_ESTIMATE
		int MB_cycles_estimate;
#endif // ME_CYCLES_ESTIMATE

   private:

      // private variables

      unsigned char *refYWthBrdrPels; // essentially the pixel cache
      int firstTimeEver; // flag used to allocate memory for internal picture buffer with border pels (once per instance)
      int firstTimePicture; // flag used to copy data into  internal picture buffer with border pels (once per picture)

      // copied from CBTPicture h264_encoder_control_tag
      int mv_for_hor[NUMBER_OF_VECTORS_PER_MACROBLOCK];		// forward horizontal motion vectors
      int mv_for_ver[NUMBER_OF_VECTORS_PER_MACROBLOCK];		// forward vertical motion vectors
      unsigned char best_inter_mtype;                       // best mtype for inter coding assuming forward prediction - set by motion estimation
      unsigned int best_inter_cost;

      // from enumtypes.h
      // The order of the following enum must be the order in the P picture code table
      // to allow the direct mapping from UVLC codeword to partition mode
      typedef enum
      {
	      H264_8X8_PARTITION,
	      H264_8X4_PARTITION,
	      H264_4X8_PARTITION,
	      H264_4X4_PARTITION,
	      H264_8X8_DIRECT_PARTITION,
	      H264_UNKNOWN_PARTITION_MODE
      }	h264_8x8_partition_mode_type_t;

      #define NUMBER_OF_LUMA_EIGHT_BY_EIGHT_BLOCKS	  4
      h264_8x8_partition_mode_type_t partition_mode[NUMBER_OF_LUMA_EIGHT_BY_EIGHT_BLOCKS]; // Partition modes for 8 x 8 sub partitions in P and B frames

      // copied from CH264Encoder thread_data_array_t
      unsigned int mvLambdaTimesCodeLength[16];
      int PSliceSkippedHor;
      int PSliceSkippedVer;
      unsigned int bestCost[4][4];
      unsigned int bestBitCost[4][4];
      int bestHor[4][4];
      int bestVer[4][4];

      int y_prediction_data[NUMBER_OF_Y_PELS_PER_BLOCK];	// Y Prediction data

      // copied from CH264Encoder
      #define LUMA_WORKING_ARRAY_SIZE 21
      struct arc_working_array_t
      {
	      int data[LUMA_WORKING_ARRAY_SIZE][LUMA_WORKING_ARRAY_SIZE];
      };

      struct arc_working_array_t ForwardWorkingArray;
      int frame_y_4x4_block_offset[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];  // Offset of top left of 4x4 block relative to top left of first 4x4 block


      int predP16x16VecX;				//	Predicted motion vectors X
      int predP16x16VecY;				//	Predicted motion vectors Y
	  int predP8x16VecX[2];				//  Predicted motion vectors X
	  int predP8x16VecY[2];				//  Predicted motion vectors Y
	  int predP16x8VecX[2];				//  Predicted motion vectors X
	  int predP16x8VecY[2];				//  Predicted motion vectors Y
      int predP8x8VecX[4];				//	Predicted motion vectors X
      int predP8x8VecY[4];				//	Predicted motion vectors Y

	unsigned int SkippedSAD_4[16];

      unsigned int skippedSAD_8[4]; // one for each 8x8 block
      unsigned int skippedModeSSD;	// One for the whole 16x16 macroblock

      // 8x8 block pattern, used to indicate that the residual data will probably
      // transform and quantise to zero
      unsigned char blockPattern[4];
      u_int32 arcRetAddr;


      // local copies of the mpc data
      struct NeighbourFlagsExpanded_t
      {
         u_int8 quantValue;
         u_int8 code;
         u_int8 intra;
         u_int8 validID;
         u_int8 valid;
         u_int8 nonZeroCoeffW; // 15 or 15
         u_int8 nonZeroCoeffX; // 13 or 14
         u_int8 nonZeroCoeffY; //  7 or 11
         u_int8 nonZeroCoeffZ; //  5 or 10
         MV_t mv[4]; // from ArcMPC.h
      };

      NeighbourFlagsExpanded_t currMB;
      NeighbourFlagsExpanded_t leftMB;
      NeighbourFlagsExpanded_t aboveMB;
      NeighbourFlagsExpanded_t aboveRightMB;
      NeighbourFlagsExpanded_t aboveLeftMB;


      //******************************************************************************
      // Auxiliary Registers
      struct
      {
         // ME_VCONFIG	Read Only
         u_int8 ME_VCONFIG__VERSION;
         #define ME_VCONFIG__VERSION_SHIFT   0
         #define ME_VCONFIG__VERSION_MASK    0xff

         u_int8 ME_VCONFIG__N_TSTR;
         #define ME_VCONFIG__N_TSTR_SHIFT    8
         #define ME_VCONFIG__N_TSTR_MASK     0x1f

         u_int8 ME_VCONFIG__N_ISTR;
         #define ME_VCONFIG__N_ISTR_SHIFT    13
         #define ME_VCONFIG__N_ISTR_MASK     0x7

         u_int8 ME_VCONFIG__MOD_ID;
         #define ME_VCONFIG__MOD_ID_SHIFT    16
         #define ME_VCONFIG__MOD_ID_MASK     0xff

         u_int8 ME_VCONFIG__N_AUX;
         #define ME_VCONFIG__N_AUX_SHIFT     24
         #define ME_VCONFIG__N_AUX_MASK      0xff

         // ME_NAMESTR	Read Only
         u_int32 ME_NAMESTR;
         #define ME_NAMESTR_SHIFT   0
         #define ME_NAMESTR_MASK    0xffffffff

         // ME_BUILD	Read Only
         u_int8 ME_BUILD__ME_VER;
         #define ME_BUILD__ME_VER_SHIFT      0
         #define ME_BUILD__ME_VER_MASK       0xf

         u_int8 ME_BUILD__PIXEL_CACHE;
         #define ME_BUILD__PIXEL_CACHE_SHIFT 8
         #define ME_BUILD__PIXEL_CACHE_MASK  0xf

         // ME_REFERENCE_START	R/W
         u_int32 ME_REFERENCE_START;
//#ifdef HARDWARE_ME
//         #define ME_REFERENCE_START_SHIFT 2
//         #define ME_REFERENCE_START_MASK  0x3fffffff
//#else
         #define ME_REFERENCE_START_SHIFT 0
         #define ME_REFERENCE_START_MASK  0xffffffff
//#endif

         // ME_REFERENCE_STRIDE	R/W
         u_int16 ME_REFERENCE_STRIDE;
         #define ME_REFERENCE_STRIDE_SHIFT   2
         #define ME_REFERENCE_STRIDE_MASK    0x3fff

         // ME_CURRENT_START	R/W
         u_int32 ME_CURRENT_START;
//#ifdef HARDWARE_ME
//         #define ME_CURRENT_START_SHIFT   2
//         #define ME_CURRENT_START_MASK    0x3fffffff
//#else
         #define ME_CURRENT_START_SHIFT   0
         #define ME_CURRENT_START_MASK    0xffffffff
//#endif

         // ME_CURRENT_STRIDE	R/W
         u_int16 ME_CURRENT_STRIDE;
         #define ME_CURRENT_STRIDE_SHIFT  2
         #define ME_CURRENT_STRIDE_MASK   0x3fff

         // ME_MB_DIMENSION	R/W
         u_int8 ME_MB_DIMENSION__MB_X_DIMENSION;
         #define ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT   0
         #define ME_MB_DIMENSION__MB_X_DIMENSION_MASK    0x7f

         u_int8 ME_MB_DIMENSION__MB_Y_DIMENSION;
         #define ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT   16
         #define ME_MB_DIMENSION__MB_Y_DIMENSION_MASK    0x7f

         // ME_MB_INDEX	R/W
         u_int8 ME_MB_INDEX__MB_X_INDEX;
         #define ME_MB_INDEX__MB_X_INDEX_SHIFT  0
         #define ME_MB_INDEX__MB_X_INDEX_MASK   0x7f

         u_int8 ME_MB_INDEX__MB_Y_INDEX;
         #define ME_MB_INDEX__MB_Y_INDEX_SHIFT  16
         #define ME_MB_INDEX__MB_Y_INDEX_MASK   0x7f

         // ME_MB_SDM_ADDR	R/W
         u_int32 ME_MB_SDM_ADDR;
//#ifdef HARDWARE_SIMD && HARDWARE_EE
 //        #define ME_MB_SDM_ADDR_SHIFT  4
 //        #define ME_MB_SDM_ADDR_MASK   0x0fff
//#else
         #define ME_MB_SDM_ADDR_SHIFT  0
         #define ME_MB_SDM_ADDR_MASK   0xffffffff
//#endif

         // ME_REFERENCE_SDM_START	R/W
         u_int32 ME_REFERENCE_SDM_START;
//#ifdef HARDWARE_SIMD
//         #define ME_REFERENCE_SDM_START_SHIFT   4
//         #define ME_REFERENCE_SDM_START_MASK    0x0fff
//#else
         #define ME_REFERENCE_SDM_START_SHIFT   0
         #define ME_REFERENCE_SDM_START_MASK    0xffffffff
//#endif

         // ME_THRESHOLD_SKIP	R/W
         u_int16 ME_THRESHOLD_SKIP;
         #define ME_THRESHOLD_SKIP_SHIFT  0
         #define ME_THRESHOLD_SKIP_MASK   0xffff

         // ME_THRESHOLD_16X16	R/W
         u_int16 ME_THRESHOLD_16x16;
         #define ME_THRESHOLD_16x16_SHIFT 0
         #define ME_THRESHOLD_16x16_MASK  0xffff

         // ME_THRESHOLD_BLOCK_PATTERN	R/W
         u_int16 ME_THRESHOLD_BLOCK_PATTERN;
         #define ME_THRESHOLD_BLOCK_PATTERN_SHIFT  0
         #define ME_THRESHOLD_BLOCK_PATTERN_MASK   0xffff

         // ME_THRESHOLD_SKIP_MAX	R/W
         u_int16 ME_THRESHOLD_SKIP_MAX;
         #define ME_THRESHOLD_SKIP_MAX_SHIFT 0
         #define ME_THRESHOLD_SKIP_MAX_MASK  0xffff

         // ME_CONTROL0	R/W
         u_int8 ME_CONTROL0__START_ITER;
         #define ME_CONTROL0__START_ITER_SHIFT        0
         #define ME_CONTROL0__START_ITER_MASK         0x1

         u_int8 ME_CONTROL0__TERM_ITER;
         #define ME_CONTROL0__TERM_ITER_SHIFT         1
         #define ME_CONTROL0__TERM_ITER_MASK          0x1

         u_int8 ME_CONTROL0__NO_ME;
         #define ME_CONTROL0__NO_ME_SHIFT             2
         #define ME_CONTROL0__NO_ME_MASK              0x1

         u_int8 ME_CONTROL0__REF_PIX_CACHE_EN;
         #define ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT  3
         #define ME_CONTROL0__REF_PIX_CACHE_EN_MASK   0x1

         u_int8 ME_CONTROL0__PAR_ID;
         #define ME_CONTROL0__PAR_ID_SHIFT            4
         #define ME_CONTROL0__PAR_ID_MASK             0x3

         s_int8 ME_CONTROL0__MV_X;
         #define ME_CONTROL0__MV_X_SHIFT              8
         #define ME_CONTROL0__MV_X_MASK               0xff

         s_int8 ME_CONTROL0__MV_Y;
         #define ME_CONTROL0__MV_Y_SHIFT              20
         #define ME_CONTROL0__MV_Y_MASK               0xff

         // ME_CONTROL1	R/W
         u_int8 ME_CONTROL1__SUB_PEL_CTRL;
         #define ME_CONTROL1__SUB_PEL_CTRL_SHIFT   0
         #define ME_CONTROL1__SUB_PEL_CTRL_MASK    0x3

         u_int8 ME_CONTROL1__STEP_ONE_CTRL;
         #define ME_CONTROL1__STEP_ONE_CTRL_SHIFT  2
         #define ME_CONTROL1__STEP_ONE_CTRL_MASK   0x3

         u_int8 ME_CONTROL1__ROUND_CTRL;
         #define ME_CONTROL1__ROUND_CTRL_SHIFT     4
         #define ME_CONTROL1__ROUND_CTRL_MASK      0x1

         // ME_SETUP	R/W
         u_int8 ME_SETUP__ITER_TYPE;
         #define ME_SETUP__ITER_TYPE_SHIFT            0
         #define ME_SETUP__ITER_TYPE_MASK             0x3

         u_int8 ME_SETUP__BLK_PAT_EN;
         #define ME_SETUP__BLK_PAT_EN_SHIFT           2
         #define ME_SETUP__BLK_PAT_EN_MASK            0x1

         u_int8 ME_SETUP__CONSTRAIN_INTRA_PRED;
         #define ME_SETUP__CONSTRAIN_INTRA_PRED_SHIFT 3
         #define ME_SETUP__CONSTRAIN_INTRA_PRED_MASK  0x1

         u_int8 ME_SETUP__PAD_CTRL;
         #define ME_SETUP__PAD_CTRL_SHIFT             4
         #define ME_SETUP__PAD_CTRL_MASK              0x3

         u_int8 ME_SETUP__SYS_DMA_EN;
         #define ME_SETUP__SYS_DMA_EN_SHIFT           7
         #define ME_SETUP__SYS_DMA_EN_MASK            0x1

         u_int8 ME_SETUP__CODEC;
         #define ME_SETUP__CODEC_SHIFT                8
         #define ME_SETUP__CODEC_MASK                 0x3

         u_int8 ME_SETUP__SKIP_ADJUST;
         #define ME_SETUP__SKIP_ADJUST_SHIFT          11
         #define ME_SETUP__SKIP_ADJUST_MASK           0x7

         u_int8 ME_SETUP__PMV;
         #define ME_SETUP__PMV_SHIFT          14
         #define ME_SETUP__PMV_MASK           0x1

         u_int8 ME_SETUP__EMV;
         #define ME_SETUP__EMV_SHIFT          16
         #define ME_SETUP__EMV_MASK           0x1

         // ME_LAMBDA	R/W
         u_int8 ME_LAMBDA;
         #define ME_LAMBDA_SHIFT 0
         #define ME_LAMBDA_MASK  0xff

         // ME_COLLECTION_ID	R/W
         u_int8 ME_COLLECTION_ID;
         #define ME_COLLECTION_ID_SHIFT 0
         #define ME_COLLECTION_ID_MASK  0xff

         // ME_RESULT	Read Only
         u_int16 ME_RESULT__COST_SAD;
         #define ME_RESULT__COST_SAD_SHIFT         0
         #define ME_RESULT__COST_SAD_MASK          0xffff

         u_int8 ME_RESULT__CANDIDATE_TYPE;
         #define ME_RESULT__CANDIDATE_TYPE_SHIFT   16
         #define ME_RESULT__CANDIDATE_TYPE_MASK    0x7

         u_int8 ME_RESULT__SKIP;
         #define ME_RESULT__SKIP_SHIFT             19
         #define ME_RESULT__SKIP_MASK              0x1

         u_int8 ME_RESULT__16x16_SEARCH;
         #define ME_RESULT__16x16_SEARCH_SHIFT     20
         #define ME_RESULT__16x16_SEARCH_MASK      0x1

         u_int8 ME_RESULT__8x8_SEARCH;
         #define ME_RESULT__8x8_SEARCH_SHIFT       21
         #define ME_RESULT__8x8_SEARCH_MASK        0x1

         u_int8 ME_RESULT__BLK_PAT;
         #define ME_RESULT__BLK_PAT_SHIFT          23
         #define ME_RESULT__BLK_PAT_MASK           0xf

         u_int8 ME_RESULT__CONFIG_ERR;
         #define ME_RESULT__CONFIG_ERR_SHIFT       29
         #define ME_RESULT__CONFIG_ERR_MASK        0x1

         u_int8 ME_RESULT__BUS_ERR;
         #define ME_RESULT__BUS_ERR_SHIFT          30
         #define ME_RESULT__BUS_ERR_MASK           0x1

         // ME_MB_P0	Read Only
         s_int8 ME_MB_P0__MV_P0_X;
         #define ME_MB_P0__MV_P0_X_SHIFT  0
         #define ME_MB_P0__MV_P0_X_MASK   0xff

         s_int8 ME_MB_P0__MV_P0_Y;
         #define ME_MB_P0__MV_P0_Y_SHIFT  8
         #define ME_MB_P0__MV_P0_Y_MASK   0xff

         s_int8 ME_MB_P0__MVP_P0_X;
         #define ME_MB_P0__MVP_P0_X_SHIFT 16
         #define ME_MB_P0__MVP_P0_X_MASK  0xff

         s_int8 ME_MB_P0__MVP_P0_Y;
         #define ME_MB_P0__MVP_P0_Y_SHIFT 24
         #define ME_MB_P0__MVP_P0_Y_MASK  0xff

         // ME_MB_P1	Read Only
         s_int8 ME_MB_P1__MV_P1_X;
         #define ME_MB_P1__MV_P1_X_SHIFT  0
         #define ME_MB_P1__MV_P1_X_MASK   0xff

         s_int8 ME_MB_P1__MV_P1_Y;
         #define ME_MB_P1__MV_P1_Y_SHIFT  8
         #define ME_MB_P1__MV_P1_Y_MASK   0xff

         s_int8 ME_MB_P1__MVP_P1_X;
         #define ME_MB_P1__MVP_P1_X_SHIFT 16
         #define ME_MB_P1__MVP_P1_X_MASK  0xff

         s_int8 ME_MB_P1__MVP_P1_Y;
         #define ME_MB_P1__MVP_P1_Y_SHIFT 24
         #define ME_MB_P1__MVP_P1_Y_MASK  0xff

         // ME_MB_P2	Read Only
         s_int8 ME_MB_P2__MV_P2_X;
         #define ME_MB_P2__MV_P2_X_SHIFT  0
         #define ME_MB_P2__MV_P2_X_MASK   0xff

         s_int8 ME_MB_P2__MV_P2_Y;
         #define ME_MB_P2__MV_P2_Y_SHIFT  8
         #define ME_MB_P2__MV_P2_Y_MASK   0xff

         s_int8 ME_MB_P2__MVP_P2_X;
         #define ME_MB_P2__MVP_P2_X_SHIFT 16
         #define ME_MB_P2__MVP_P2_X_MASK  0xff

         s_int8 ME_MB_P2__MVP_P2_Y;
         #define ME_MB_P2__MVP_P2_Y_SHIFT 24
         #define ME_MB_P2__MVP_P2_Y_MASK  0xff

         // ME_MB_P3	Read Only
         s_int8 ME_MB_P3__MV_P3_X;
         #define ME_MB_P3__MV_P3_X_SHIFT  0
         #define ME_MB_P3__MV_P3_X_MASK   0xff

         s_int8 ME_MB_P3__MV_P3_Y;
         #define ME_MB_P3__MV_P3_Y_SHIFT  8
         #define ME_MB_P3__MV_P3_Y_MASK   0xff

         s_int8 ME_MB_P3__MVP_P3_X;
         #define ME_MB_P3__MVP_P3_X_SHIFT 16
         #define ME_MB_P3__MVP_P3_X_MASK  0xff

         s_int8 ME_MB_P3__MVP_P3_Y;
         #define ME_MB_P3__MVP_P3_Y_SHIFT 24
         #define ME_MB_P3__MVP_P3_Y_MASK  0xff

         // ME_SKIP_BR_ADDR	R/W
         u_int32 ME_SKIP_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_SKIP_BR_ADDR_SHIFT 1
//         #define ME_SKIP_BR_ADDR_MASK  0xfffffffe
//#else
         #define ME_SKIP_BR_ADDR_SHIFT 0
         #define ME_SKIP_BR_ADDR_MASK  0xffffffff
//#endif

         // ME_16X16_BR_ADDR	R/W
         u_int32 ME_16x16_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_16x16_BR_ADDR_SHIFT 1
//         #define ME_16x16_BR_ADDR_MASK  0xfffffffe
//#else
         #define ME_16x16_BR_ADDR_SHIFT 0
         #define ME_16x16_BR_ADDR_MASK  0xffffffff
//#endif

         // ME_8X8_BR_ADDR	R/W
         u_int32 ME_8x8_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_8x8_BR_ADDR_SHIFT  1
//         #define ME_8x8_BR_ADDR_MASK   0xfffffffe
//#else
         #define ME_8x8_BR_ADDR_SHIFT  0
         #define ME_8x8_BR_ADDR_MASK   0xffffffff
//#endif

         // ME_SDM_BR_ADDR	R/W
         u_int32 ME_SDM_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_SDM_BR_ADDR_SHIFT  1
//         #define ME_SDM_BR_ADDR_MASK   0xfffffffe
//#else
         #define ME_SDM_BR_ADDR_SHIFT  0
         #define ME_SDM_BR_ADDR_MASK   0xffffffff
//#endif

         // MB_BASE_BR_ADDR	R/W
         u_int8 ME_BASE_BR_ADDR__CHAN_DIS;
         #define ME_BASE_BR_ADDR__CHAN_DIS_SHIFT   0
         #define ME_BASE_BR_ADDR__CHAN_DIS_MASK    0x1

         // ME_D_POINTER	R/W
         u_int32 ME_D_POINTER;
//#ifdef HARDWARE_ME
//         #define ME_D_POINTER_SHIFT 4
//         #define ME_D_POINTER_MASK  0x0fff
//#else
         #define ME_D_POINTER_SHIFT 0
         #define ME_D_POINTER_MASK  0xffffffff
//#endif

         // ME_A_POINTER	R/W
         u_int32 ME_A_POINTER;
//#ifdef HARDWARE_ME
//         #define ME_A_POINTER_SHIFT 4
 //        #define ME_A_POINTER_MASK  0x0fff
//#else
         #define ME_A_POINTER_SHIFT 0
         #define ME_A_POINTER_MASK  0xffffffff
//#endif

         // ME_CURRENT_POINTER	R/W
         u_int32 ME_CURRENT_POINTER;
//#ifdef HARDWARE_ME
//         #define ME_CURRENT_POINTER_SHIFT 4
//         #define ME_CURRENT_POINTER_MASK  0x0fff
//#else
         #define ME_CURRENT_POINTER_SHIFT 0
         #define ME_CURRENT_POINTER_MASK  0xffffffff
//#endif

         // ME_16X8_BR_ADDR	R/W
         u_int32 ME_16x8_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_16x8_BR_ADDR_SHIFT 1
//         #define ME_16x8_BR_ADDR_MASK  0xfffffffe
//#else
         #define ME_16x8_BR_ADDR_SHIFT 0
         #define ME_16x8_BR_ADDR_MASK  0xffffffff
//#endif

         // ME_8X16_BR_ADDR	R/W
         u_int32 ME_8x16_BR_ADDR;
//#ifdef HARDWARE_ME
//         #define ME_8x16_BR_ADDR_SHIFT 1
//         #define ME_8x16_BR_ADDR_MASK  0xfffffffe
//#else
         #define ME_8x16_BR_ADDR_SHIFT 0
         #define ME_8x16_BR_ADDR_MASK  0xffffffff
//#endif

         // ME_DEBUG	R/W
         u_int8 ME_DEBUG__START_ITER_CTRL;
         #define ME_DEBUG__START_ITER_CTRL_SHIFT   0
         #define ME_DEBUG__START_ITER_CTRL_MASK    0x1

         u_int8 ME_DEBUG__CHAN_MB_CTRL;
         #define ME_DEBUG__CHAN_MB_CTRL_SHIFT      1
         #define ME_DEBUG__CHAN_MB_CTRL_MASK       0x1

         u_int8 ME_DEBUG__CHAN_FINAL_CTRL;
         #define ME_DEBUG__CHAN_FINAL_CTRL_SHIFT   2
         #define ME_DEBUG__CHAN_FINAL_CTRL_MASK    0x1

         u_int8 ME_DEBUG__IDLE_CTRL;
         #define ME_DEBUG__IDLE_CTRL_SHIFT         3
         #define ME_DEBUG__IDLE_CTRL_MASK          0x1

         u_int8 ME_DEBUG__START_ITER_FLAG;
         #define ME_DEBUG__START_ITER_FLAG_SHIFT   16
         #define ME_DEBUG__START_ITER_FLAG_MASK    0x1

         u_int8 ME_DEBUG__CHAN_MB_FLAG;
         #define ME_DEBUG__CHAN_MB_FLAG_SHIFT      17
         #define ME_DEBUG__CHAN_MB_FLAG_MASK       0x1

         u_int8 ME_DEBUG__CHAN_FINAL_FLAG;
         #define ME_DEBUG__CHAN_FINAL_FLAG_SHIFT   18
         #define ME_DEBUG__CHAN_FINAL_FLAG_MASK    0x1

         u_int8 ME_DEBUG__IDLE_FLAG;
         #define ME_DEBUG__IDLE_FLAG_SHIFT         19
         #define ME_DEBUG__IDLE_FLAG_MASK          0x1

      } auxReg;


      struct AuxRegDef_s
      {
         auxRegName name;
         u_int32 resetVal;
      };

      AuxRegDef_s const * auxRegDef;

      // channel variable
      struct Response ChannelResponse[2];
      struct Response SendChannelResponse;

#ifdef ARC_ISS_ME_MODEL
      // iss channel variable
      ARC_simext_simulator_access * simAccess;
#endif

#ifdef USE_CALLBACK
      call_back_to_worker * MEcallback;
#endif

      // overall status
      bool MEHalted;


      // private functions


      // main ME loop
      void DoMotionEstimation(void);


      void IncrementMBIndices(void);

      // Border pel generation utility
      void GenerateLumaRefBorderPels(void);
      void GenerateBorderPels(unsigned char *srcPic,unsigned char *dstPic, int top_pels,int btm_pels,int lft_pels,int rgt_pels);

      // Fetch & calc cost for 16x16 at skip motion vector
      bool DoSkipTestLuma(void);

	//calculate SADs for skip test on a 4x4 luma block basis
	unsigned int CalculateSAD4(unsigned char* current, int* previous, unsigned int current_yinc, unsigned int previous_yinc);
	unsigned int CalculateSAD4(unsigned char* current, unsigned char* previous, unsigned int current_yinc, unsigned int previous_yinc);

      // Fetch & calc cost for 16x16 at skip motion vector
      // Inputs: predected motion vector x & Y
      // Outputs best motion vector cost & fetched pixels
      bool StepSearch16x16MB(void);

      // Fetch & calc cost for 16x8 at skip motion vector
      // Inputs: predected motion vector x & Y
      // Outputs best motion vector cost & fetched pixels
      void StepSearch16x8MB(void);

      // Fetch & calc cost for 8x16 at skip motion vector
      // Inputs: predected motion vector x & Y
      // Outputs best motion vector cost & fetched pixels
      void StepSearch8x16MB(void);

      // Fetch & calc cost for 8x8 at skip motion vector
      // Inputs: predected motion vector x & Y
      // Outputs best motion vector cost & fetched pixels
      void StepSearch8x8MB(void);

	  // These functions return the nimber of bits it takes to 
	  // code the MVDs
	  unsigned int h263_encoder_vector_code_length(int mvd);
	  unsigned int h264_encoder_vector_code_length(int mvd);
	  unsigned int mpeg4_encoder_vector_code_length(int mvd);
	  unsigned int encoder_vector_code_length(int mvd);

      unsigned int CalculateSSD8(unsigned char* current, int* previous,
                         unsigned int current_yinc, unsigned int previous_yinc, int height);
      unsigned int CalculateSSD8(unsigned char* current, unsigned char* previous,
                         unsigned int current_yinc, unsigned int previous_yinc, int height);

      // candidate selection
      void SetTotalModeCost(bool valid_16x16, bool valid_16x8, bool valid_8x16, bool valid_8x8);

      // Generates MVs
      void GenerateSkippedMotionVectors(void);
      void GeneratePredictedMotionVectors(int block_index,int vector_number,
			                                 h264_block_shape_type_t block_shape,
			                                 int& pred_hor, int& pred_ver);

      unsigned int CalculateSOAD(unsigned char* current, unsigned char* previous,
                                 unsigned int current_yinc, unsigned int previous_yinc,
                                 unsigned int stop_soad, int height);

      unsigned int CalculateSOADWidth8(unsigned char* current, unsigned char* previous,
                                       unsigned int current_yinc, unsigned int previous_yinc,
                                       unsigned int height, unsigned int stop_soad);


      // functions copied over from BT
      void ArcGetNeighbouringLocationNonMBAFF(int xN, int yN, int maxWH, NeighbourFlagsExpanded_t ** neighbouring_control, int& yW);

      bool ArcGetH264MotionVectorPrediction(int block_index, int vector_number, h264_block_shape_type_t block_shape,
                                          int& pred_hor, int& pred_ver);
      bool ArcGetMpeg4MotionVectorPrediction(int vector_number, h264_block_shape_type_t block_shape,
                                          int& pred_hor, int& pred_ver);
      bool ArcGetH263MotionVectorPrediction(int vector_number, h264_block_shape_type_t block_shape,
                                          int& pred_hor, int& pred_ver);

      void ArcCalculateH264SkippedMotionVectors(void);
      void ArcCalculateMpeg4SkippedMotionVectors(void);
      void ArcCalculateH263SkippedMotionVectors(void);

      bool ArcMakeH264InterPrediction(unsigned char mbType);
      bool ArcMakeH263InterPrediction(unsigned char mbType);

      void ArcGetIntegerPelReferenceBlockFromFrame(PIXEL * ref_picture_data,
													                                       arc_working_array_t * working_array,
													                                       int ref_hor_size,
													                                       int ref_ver_size,
													                                       int ref_hor_offset,
													                                       int ref_ver_offset,
													                                       int block_hor_size,
													                                       int block_ver_size);

      void ArcFilterH264LumaReferenceBlock(arc_working_array_t * working_array,
										                                    int fractional_hor_vector,
										                                    int fractional_ver_vector,
										                                    int block_hor_size,
										                                    int block_ver_size);
      void ArcFilterH263LumaReferenceBlock(arc_working_array_t * working_array,
										                                    int fractional_hor_vector,
										                                    int fractional_ver_vector,
										                                    int block_hor_size,
										                                    int block_ver_size);

      void ArcCopyToLumaPrediction(int block_index, int block_hor_size, int block_ver_size);


      bool ArcMakeH264InterPrediction8(int block, int block_shape);
      bool ArcMakeH263InterPrediction8(int block);

      unsigned int ArcCalculateSSD(unsigned char* current, unsigned char * previous,
                                   unsigned int current_yinc, unsigned int previous_yinc,int height);
      unsigned int ArcCalculateSSD(unsigned char* current, int * previous,
                                   unsigned int current_yinc, unsigned int previous_yinc,int height);

      // calculate the blockPattern value
      void CalculateBlockPattern (void);

      // read the neighbour information
      void ReadNeighbourDataFromMPC (void);

      // write neighbour data to mpc
      void WriteNeighbourDataToMPC (u_int8 mbCodingType);

      // update mpc with results
      void WriteResultsToMPC (void);

      // writes the current macroblock to the current mpc pixel data area in the sdm
      void WriteCurrentMBToSDM (void);

      // generates the reference macroblock and writes to the mpc pixel data area in the sdm
      void GenerateAndWriteReferenceMBToSDM (void);

      // generates a sad value for the 8x8 block specified in the aux regs
      void Manual8x8SAD (void);


      // ISS channel responses
     // void CreateChannelCommandMB(void);
      void CreateChannelCommandFinal(void);

      // memory access functions
      void MeMemoryWriteSDM (u_int8 * dst, u_int8 * src, int count);
      void MeMemoryRead (u_int8 * dst, u_int8 * src, int count);
      void MeMemoryReadSDM (u_int8 * dst, u_int8 * src, int count);
      void MeMemorySet (u_int8 * dst, u_int8 * src, int count);


#ifdef VERIFICATION_DUMP
        // intermediate results dumping for debugging and verifying the hardware model
        #define VDUMP_FN "VerificationDump.log"
        FILE * vdump_fp;
#endif

#ifdef ME_DUMP_CRR
void MeDumpCurrRefRes (const char * str, int maxX, int maxY, u_int8 * curr, u_int8 * ref, int currStride, int refStride);
void MeDumpCurrRefRes (const char * str, int maxX, int maxY, u_int8 * curr, int * ref, int currStride, int refStride);
#endif // ME_DUMP_CRR

#ifdef ME_DUMP_SUBPEL
        void MeDumpSubPel16x16 (int vecX, int vecY);
        void MeDumpSubPel8x8 (int vecX, int vecY, int block, int block_shape);
#endif // ME_DUMP_SUBPEL

};



#ifndef ARC_ISS_ME_MODEL
extern ArcMotionEstimationModel gArcMotionEstimationModel;
#endif



#endif // ARC_MOTION_ESTIMATION_MODEL_H
