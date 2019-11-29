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


#include <string.h>
#include "ArcMotionEstimationModel.h"
#include "ArcMPC.h"
#include "defines.h"
#include "EncoderControl.h"


#ifdef ME_TIME
unsigned int Timer0Total = 0;
unsigned int Timer0Count = 0;

#define RESET_AND_START_TIMER0 {_sr(0xffffffff, 0x23); _sr(0x3, 0x22); _sr(0x0, 0x21);}
#define REPORT_TIMER0 {int timeTaken = _lr(0x21); Timer0Total += timeTaken; Timer0Count++; double av = (double)Timer0Total/(double)Timer0Count; printf("Time taken = %12d %12.2lf %12d %12d\n", timeTaken, av, Timer0Total, Timer0Count);}
#endif // ME_TIME

#define CLIP_VEC(x) ( (x) < -15 ? -15 : (x) > 15 ? 15 : (x) )
//#define NOT_IN_RANGE_SUB(x, y, minx, miny, maxx, maxy) ((x<=minx) || (x>=maxx) || (y<=miny) ||(y>=maxy))
#define NOT_IN_RANGE_INT(x, y, minx, miny, maxx, maxy) ((x<minx) || (x>maxx) || (y<miny) ||(y>maxy))

#ifdef ME_CYCLES_ESTIMATE
// indexed by (is_16x16<<2 + is_16x8<<1 + is_8x8)
// based on Ryan Lim's powerpoint presentation 28th July 2006
const int me_cycles[8] = {928,2992,5056,7120,2944,5008,7072,9136};
#endif // ME_CYCLES_ESTIMATE


// global instance
#ifndef ARC_ISS_ME_MODEL
ArcMotionEstimationModel gArcMotionEstimationModel;
#endif
// subpel order is to mimic the (bizarre) way the hardware will do things:
// different for 1/2 pel and 1/4 pel    [2]
// 8 positions for each mode            [8]
// two displacements per position (down, across) [2]
int subpel_order[2][8][2] =
{
    { // half pel
        -1, -1,
         1, -1,
         0, -1,
        -1,  0,
         1,  0,
        -1,  1,
         1,  1,
         0,  1,
    },
    { // quarter pel
        -1, -1,
         0, -1,
         1, -1,
        -1,  0,
         1,  0,
        -1,  1,
         0,  1,
         1,  1
    }
};

ArcMotionEstimationModel::ArcMotionEstimationModel()
{
   // auxiliary registers
   // define storage and setup reset values
   static AuxRegDef_s auxRegInit[NUM_ME_AUX_REGS]=
      {
         // Name                      Reset Value
         {ME_VCONFIG,                 0x37030201},
         {ME_NAMESTR,                 0x30306977},
         {ME_BUILD,                   0x00000101},
         {ME_REFERENCE_START,         0x00000000},
         {ME_REFERENCE_STRIDE,        0x00000000},
         {ME_CURRENT_START,           0x00000000},
         {ME_CURRENT_STRIDE,          0x00000000},
         {ME_MB_DIMENSION,            0x00000000},
         {ME_MB_INDEX,                0x00000000},
         {ME_MB_SDM_ADDR,             0x00000000},
         {ME_REFERENCE_SDM_START,     0x00000000},
         {ME_THRESHOLD_SKIP,          0x00000000},
         {ME_THRESHOLD_16x16,         0x00000000},
         {ME_THRESHOLD_BLOCK_PATTERN, 0x00000000},
         {ME_THRESHOLD_SKIP_MAX,      0x00000000},
         {ME_CONTROL0,                0x00000004},
         {ME_CONTROL1,                0x00000000},
         {ME_SETUP,                   0x00000000},
         {ME_LAMBDA,                  0x00000000},
         {ME_COLLECTION_ID,           0x00000000},
         {ME_RESULT,                  0x00000000},
         {ME_MB_P0,                   0x00000000},
         {ME_MB_P1,                   0x00000000},
         {ME_MB_P2,                   0x00000000},
         {ME_MB_P3,                   0x00000000},
         {ME_SKIP_BR_ADDR,            0x00000000},
         {ME_16x16_BR_ADDR,           0x00000000},
         {ME_8x8_BR_ADDR,             0x00000000},
         {ME_SDM_BR_ADDR,             0x00000000},
         {ME_BASE_BR_ADDR,            0x00000000},
         {ME_D_POINTER,               0x00000000},
         {ME_A_POINTER,               0x00000000},
         {ME_CURRENT_POINTER,         0x00000000},
         {ME_16x8_BR_ADDR,            0x00000000},
         {ME_8x16_BR_ADDR,            0x00000000},
         {ME_DEBUG,                   0x00000000}
      };

   // check reg numbers are in order
   for (int loop=0; loop<NUM_ME_AUX_REGS; loop++)
   {
      if (auxRegInit[loop].name != loop)
      {
         // error because register enum names are not in the same order as register definitions
         assert(false);
      }
   }

   // assign initial values to aux regs
   auxReg.ME_VCONFIG__VERSION=(u_int8) (auxRegInit[ME_VCONFIG].resetVal >> ME_VCONFIG__VERSION_SHIFT) & ME_VCONFIG__VERSION_MASK;
   auxReg.ME_VCONFIG__N_TSTR=(u_int8) (auxRegInit[ME_VCONFIG].resetVal >> ME_VCONFIG__N_TSTR_SHIFT) & ME_VCONFIG__N_TSTR_MASK;
   auxReg.ME_VCONFIG__N_ISTR=(u_int8) (auxRegInit[ME_VCONFIG].resetVal >> ME_VCONFIG__N_ISTR_SHIFT) & ME_VCONFIG__N_ISTR_MASK;
   auxReg.ME_VCONFIG__MOD_ID=(u_int8) (auxRegInit[ME_VCONFIG].resetVal >> ME_VCONFIG__MOD_ID_SHIFT) & ME_VCONFIG__MOD_ID_MASK;
   auxReg.ME_VCONFIG__N_AUX=(u_int8) (auxRegInit[ME_VCONFIG].resetVal >> ME_VCONFIG__N_AUX_SHIFT) & ME_VCONFIG__N_AUX_MASK;
   auxReg.ME_NAMESTR=(auxRegInit[ME_NAMESTR].resetVal >> ME_NAMESTR_SHIFT) & ME_NAMESTR_MASK;
   auxReg.ME_BUILD__ME_VER=(u_int8) (auxRegInit[ME_BUILD].resetVal >> ME_BUILD__ME_VER_SHIFT) & ME_BUILD__ME_VER_MASK;
   auxReg.ME_BUILD__PIXEL_CACHE=(u_int8) (auxRegInit[ME_BUILD].resetVal >> ME_BUILD__PIXEL_CACHE_SHIFT) & ME_BUILD__PIXEL_CACHE_MASK;
   auxReg.ME_REFERENCE_START=(auxRegInit[ME_REFERENCE_START].resetVal >> ME_REFERENCE_START_SHIFT) & ME_REFERENCE_START_MASK;
   auxReg.ME_REFERENCE_STRIDE=(u_int16) (auxRegInit[ME_REFERENCE_STRIDE].resetVal >> ME_REFERENCE_STRIDE_SHIFT) & ME_REFERENCE_STRIDE_MASK;
   auxReg.ME_CURRENT_START=(auxRegInit[ME_CURRENT_START].resetVal >> ME_CURRENT_START_SHIFT) & ME_CURRENT_START_MASK;
   auxReg.ME_CURRENT_STRIDE=(u_int16) (auxRegInit[ME_CURRENT_STRIDE].resetVal >> ME_CURRENT_STRIDE_SHIFT) & ME_CURRENT_STRIDE_MASK;
   auxReg.ME_MB_DIMENSION__MB_X_DIMENSION=(u_int8) (auxRegInit[ME_MB_DIMENSION].resetVal >> ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) & ME_MB_DIMENSION__MB_X_DIMENSION_MASK;
   auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION=(u_int8) (auxRegInit[ME_MB_DIMENSION].resetVal >> ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT) & ME_MB_DIMENSION__MB_Y_DIMENSION_MASK;
   auxReg.ME_MB_INDEX__MB_X_INDEX=(u_int8) (auxRegInit[ME_MB_INDEX].resetVal >> ME_MB_INDEX__MB_X_INDEX_SHIFT) & ME_MB_INDEX__MB_X_INDEX_MASK;
   auxReg.ME_MB_INDEX__MB_Y_INDEX=(u_int8) (auxRegInit[ME_MB_INDEX].resetVal >> ME_MB_INDEX__MB_Y_INDEX_SHIFT) & ME_MB_INDEX__MB_Y_INDEX_MASK;
   auxReg.ME_MB_SDM_ADDR=(auxRegInit[ME_MB_SDM_ADDR].resetVal >> ME_MB_SDM_ADDR_SHIFT) & ME_MB_SDM_ADDR_MASK;
   auxReg.ME_REFERENCE_SDM_START=(auxRegInit[ME_REFERENCE_SDM_START].resetVal >> ME_REFERENCE_SDM_START_SHIFT) & ME_REFERENCE_SDM_START_MASK;
   auxReg.ME_THRESHOLD_SKIP=(u_int16) (auxRegInit[ME_THRESHOLD_SKIP].resetVal >> ME_THRESHOLD_SKIP_SHIFT) & ME_THRESHOLD_SKIP_MASK;
   auxReg.ME_THRESHOLD_16x16=(u_int16) (auxRegInit[ME_THRESHOLD_16x16].resetVal >> ME_THRESHOLD_16x16_SHIFT) & ME_THRESHOLD_16x16_MASK;
   auxReg.ME_THRESHOLD_BLOCK_PATTERN=(u_int16) (auxRegInit[ME_THRESHOLD_BLOCK_PATTERN].resetVal >> ME_THRESHOLD_BLOCK_PATTERN_SHIFT) & ME_THRESHOLD_BLOCK_PATTERN_MASK;
   auxReg.ME_THRESHOLD_SKIP_MAX=(u_int16) (auxRegInit[ME_THRESHOLD_SKIP_MAX].resetVal >> ME_THRESHOLD_SKIP_MAX_SHIFT) & ME_THRESHOLD_SKIP_MAX_MASK;
   auxReg.ME_CONTROL0__START_ITER=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__START_ITER_SHIFT) & ME_CONTROL0__START_ITER_MASK;
   auxReg.ME_CONTROL0__TERM_ITER=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__TERM_ITER_SHIFT) & ME_CONTROL0__TERM_ITER_MASK;
   auxReg.ME_CONTROL0__NO_ME=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__NO_ME_SHIFT) & ME_CONTROL0__NO_ME_MASK;
   auxReg.ME_CONTROL0__REF_PIX_CACHE_EN=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT) & ME_CONTROL0__REF_PIX_CACHE_EN_MASK;
   auxReg.ME_CONTROL0__PAR_ID=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__PAR_ID_SHIFT) & ME_CONTROL0__PAR_ID_MASK;
   auxReg.ME_CONTROL0__MV_X=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__MV_X_SHIFT) & ME_CONTROL0__MV_X_MASK;
   auxReg.ME_CONTROL0__MV_Y=(u_int8) (auxRegInit[ME_CONTROL0].resetVal >> ME_CONTROL0__MV_Y_SHIFT) & ME_CONTROL0__MV_Y_MASK;
   auxReg.ME_CONTROL1__SUB_PEL_CTRL=(u_int8) (auxRegInit[ME_CONTROL1].resetVal >> ME_CONTROL1__SUB_PEL_CTRL_SHIFT) & ME_CONTROL1__SUB_PEL_CTRL_MASK;
   auxReg.ME_CONTROL1__STEP_ONE_CTRL=(u_int8) (auxRegInit[ME_CONTROL1].resetVal >> ME_CONTROL1__STEP_ONE_CTRL_SHIFT) & ME_CONTROL1__STEP_ONE_CTRL_MASK;
   auxReg.ME_CONTROL1__ROUND_CTRL=(u_int8) (auxRegInit[ME_CONTROL1].resetVal >> ME_CONTROL1__ROUND_CTRL_SHIFT) & ME_CONTROL1__ROUND_CTRL_MASK;
   auxReg.ME_SETUP__ITER_TYPE=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__ITER_TYPE_SHIFT) & ME_SETUP__ITER_TYPE_MASK;
   auxReg.ME_SETUP__BLK_PAT_EN=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__BLK_PAT_EN_SHIFT) & ME_SETUP__BLK_PAT_EN_MASK;
   auxReg.ME_SETUP__CONSTRAIN_INTRA_PRED=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__CONSTRAIN_INTRA_PRED_SHIFT) & ME_SETUP__CONSTRAIN_INTRA_PRED_MASK;
   auxReg.ME_SETUP__PAD_CTRL=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__PAD_CTRL_SHIFT) & ME_SETUP__PAD_CTRL_MASK;
   auxReg.ME_SETUP__SYS_DMA_EN=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__SYS_DMA_EN_SHIFT) & ME_SETUP__SYS_DMA_EN_MASK;
   auxReg.ME_SETUP__CODEC=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__CODEC_SHIFT) & ME_SETUP__CODEC_MASK;
   auxReg.ME_SETUP__SKIP_ADJUST=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__SKIP_ADJUST_SHIFT) & ME_SETUP__SKIP_ADJUST_MASK;
   auxReg.ME_SETUP__PMV=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__PMV_SHIFT) & ME_SETUP__PMV_MASK;
   auxReg.ME_SETUP__EMV=(u_int8) (auxRegInit[ME_SETUP].resetVal >> ME_SETUP__EMV_SHIFT) & ME_SETUP__EMV_MASK;
   auxReg.ME_LAMBDA=(u_int8) (auxRegInit[ME_LAMBDA].resetVal >> ME_LAMBDA_SHIFT) & ME_LAMBDA_MASK;
   auxReg.ME_COLLECTION_ID=(u_int8) (auxRegInit[ME_COLLECTION_ID].resetVal >> ME_COLLECTION_ID_SHIFT) & ME_COLLECTION_ID_MASK;
   auxReg.ME_RESULT__COST_SAD=(u_int16) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__COST_SAD_SHIFT) & ME_RESULT__COST_SAD_MASK;
   auxReg.ME_RESULT__CANDIDATE_TYPE=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__CANDIDATE_TYPE_SHIFT) & ME_RESULT__CANDIDATE_TYPE_MASK;
   auxReg.ME_RESULT__SKIP=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__SKIP_SHIFT) & ME_RESULT__SKIP_MASK;
   auxReg.ME_RESULT__16x16_SEARCH=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__16x16_SEARCH_SHIFT) & ME_RESULT__16x16_SEARCH_MASK;
   auxReg.ME_RESULT__8x8_SEARCH=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__8x8_SEARCH_SHIFT) & ME_RESULT__8x8_SEARCH_MASK;
   auxReg.ME_RESULT__BLK_PAT=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__BLK_PAT_SHIFT) & ME_RESULT__BLK_PAT_MASK;
   auxReg.ME_RESULT__CONFIG_ERR=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__CONFIG_ERR_SHIFT) & ME_RESULT__CONFIG_ERR_MASK;
   auxReg.ME_RESULT__BUS_ERR=(u_int8) (auxRegInit[ME_RESULT].resetVal >> ME_RESULT__BUS_ERR_SHIFT) & ME_RESULT__BUS_ERR_MASK;
   auxReg.ME_MB_P0__MV_P0_X=(u_int8) (auxRegInit[ME_MB_P0].resetVal >> ME_MB_P0__MV_P0_X_SHIFT) & ME_MB_P0__MV_P0_X_MASK;
   auxReg.ME_MB_P0__MV_P0_Y=(u_int8) (auxRegInit[ME_MB_P0].resetVal >> ME_MB_P0__MV_P0_Y_SHIFT) & ME_MB_P0__MV_P0_Y_MASK;
   auxReg.ME_MB_P0__MVP_P0_X=(u_int8) (auxRegInit[ME_MB_P0].resetVal >> ME_MB_P0__MVP_P0_X_SHIFT) & ME_MB_P0__MVP_P0_X_MASK;
   auxReg.ME_MB_P0__MVP_P0_Y=(u_int8) (auxRegInit[ME_MB_P0].resetVal >> ME_MB_P0__MVP_P0_Y_SHIFT) & ME_MB_P0__MVP_P0_Y_MASK;
   auxReg.ME_MB_P1__MV_P1_X=(u_int8) (auxRegInit[ME_MB_P1].resetVal >> ME_MB_P1__MV_P1_X_SHIFT) & ME_MB_P1__MV_P1_X_MASK;
   auxReg.ME_MB_P1__MV_P1_Y=(u_int8) (auxRegInit[ME_MB_P1].resetVal >> ME_MB_P1__MV_P1_Y_SHIFT) & ME_MB_P1__MV_P1_Y_MASK;
   auxReg.ME_MB_P1__MVP_P1_X=(u_int8) (auxRegInit[ME_MB_P1].resetVal >> ME_MB_P1__MVP_P1_X_SHIFT) & ME_MB_P1__MVP_P1_X_MASK;
   auxReg.ME_MB_P1__MVP_P1_Y=(u_int8) (auxRegInit[ME_MB_P1].resetVal >> ME_MB_P1__MVP_P1_Y_SHIFT) & ME_MB_P1__MVP_P1_Y_MASK;
   auxReg.ME_MB_P2__MV_P2_X=(u_int8) (auxRegInit[ME_MB_P2].resetVal >> ME_MB_P2__MV_P2_X_SHIFT) & ME_MB_P2__MV_P2_X_MASK;
   auxReg.ME_MB_P2__MV_P2_Y=(u_int8) (auxRegInit[ME_MB_P2].resetVal >> ME_MB_P2__MV_P2_Y_SHIFT) & ME_MB_P2__MV_P2_Y_MASK;
   auxReg.ME_MB_P2__MVP_P2_X=(u_int8) (auxRegInit[ME_MB_P2].resetVal >> ME_MB_P2__MVP_P2_X_SHIFT) & ME_MB_P2__MVP_P2_X_MASK;
   auxReg.ME_MB_P2__MVP_P2_Y=(u_int8) (auxRegInit[ME_MB_P2].resetVal >> ME_MB_P2__MVP_P2_Y_SHIFT) & ME_MB_P2__MVP_P2_Y_MASK;
   auxReg.ME_MB_P3__MV_P3_X=(u_int8) (auxRegInit[ME_MB_P3].resetVal >> ME_MB_P3__MV_P3_X_SHIFT) & ME_MB_P3__MV_P3_X_MASK;
   auxReg.ME_MB_P3__MV_P3_Y=(u_int8) (auxRegInit[ME_MB_P3].resetVal >> ME_MB_P3__MV_P3_Y_SHIFT) & ME_MB_P3__MV_P3_Y_MASK;
   auxReg.ME_MB_P3__MVP_P3_X=(u_int8) (auxRegInit[ME_MB_P3].resetVal >> ME_MB_P3__MVP_P3_X_SHIFT) & ME_MB_P3__MVP_P3_X_MASK;
   auxReg.ME_MB_P3__MVP_P3_Y=(u_int8) (auxRegInit[ME_MB_P3].resetVal >> ME_MB_P3__MVP_P3_Y_SHIFT) & ME_MB_P3__MVP_P3_Y_MASK;
   auxReg.ME_SKIP_BR_ADDR=(auxRegInit[ME_SKIP_BR_ADDR].resetVal >> ME_SKIP_BR_ADDR_SHIFT) & ME_SKIP_BR_ADDR_MASK;
   auxReg.ME_16x16_BR_ADDR=(auxRegInit[ME_16x16_BR_ADDR].resetVal >> ME_16x16_BR_ADDR_SHIFT) & ME_16x16_BR_ADDR_MASK;
   auxReg.ME_8x8_BR_ADDR=(auxRegInit[ME_8x8_BR_ADDR].resetVal >> ME_8x8_BR_ADDR_SHIFT) & ME_8x8_BR_ADDR_MASK;
   auxReg.ME_SDM_BR_ADDR=(auxRegInit[ME_SDM_BR_ADDR].resetVal >> ME_SDM_BR_ADDR_SHIFT) & ME_SDM_BR_ADDR_MASK;
   auxReg.ME_BASE_BR_ADDR__CHAN_DIS=(u_int8) (auxRegInit[ME_BASE_BR_ADDR].resetVal >> ME_BASE_BR_ADDR__CHAN_DIS_SHIFT) & ME_BASE_BR_ADDR__CHAN_DIS_MASK;
   auxReg.ME_D_POINTER=(auxRegInit[ME_D_POINTER].resetVal >> ME_D_POINTER_SHIFT) & ME_D_POINTER_MASK;
   auxReg.ME_A_POINTER=(auxRegInit[ME_A_POINTER].resetVal >> ME_A_POINTER_SHIFT) & ME_A_POINTER_MASK;
   auxReg.ME_CURRENT_POINTER=(auxRegInit[ME_CURRENT_POINTER].resetVal >> ME_CURRENT_POINTER_SHIFT) & ME_CURRENT_POINTER_MASK;
   auxReg.ME_16x8_BR_ADDR=(auxRegInit[ME_16x8_BR_ADDR].resetVal >> ME_16x8_BR_ADDR_SHIFT) & ME_16x8_BR_ADDR_MASK;
   auxReg.ME_8x16_BR_ADDR=(auxRegInit[ME_8x16_BR_ADDR].resetVal >> ME_8x16_BR_ADDR_SHIFT) & ME_8x16_BR_ADDR_MASK;
   auxReg.ME_DEBUG__START_ITER_CTRL=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__START_ITER_CTRL_SHIFT) & ME_DEBUG__START_ITER_CTRL_MASK;
   auxReg.ME_DEBUG__CHAN_MB_CTRL=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__CHAN_MB_CTRL_SHIFT) & ME_DEBUG__CHAN_MB_CTRL_MASK;
   auxReg.ME_DEBUG__CHAN_FINAL_CTRL=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__CHAN_FINAL_CTRL_SHIFT) & ME_DEBUG__CHAN_FINAL_CTRL_MASK;
   auxReg.ME_DEBUG__IDLE_CTRL=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__IDLE_CTRL_SHIFT) & ME_DEBUG__IDLE_CTRL_MASK;
   auxReg.ME_DEBUG__START_ITER_FLAG=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__START_ITER_FLAG_SHIFT) & ME_DEBUG__START_ITER_FLAG_MASK;
   auxReg.ME_DEBUG__CHAN_MB_FLAG=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__CHAN_MB_FLAG_SHIFT) & ME_DEBUG__CHAN_MB_FLAG_MASK;
   auxReg.ME_DEBUG__CHAN_FINAL_FLAG=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__CHAN_FINAL_FLAG_SHIFT) & ME_DEBUG__CHAN_FINAL_FLAG_MASK;
   auxReg.ME_DEBUG__IDLE_FLAG=(u_int8) (auxRegInit[ME_DEBUG].resetVal >> ME_DEBUG__IDLE_FLAG_SHIFT) & ME_DEBUG__IDLE_FLAG_MASK;


   // make initial values visible to entire class
   auxRegDef=auxRegInit;


   // buffer for the reference picture with border padding
   refYWthBrdrPels=NULL;

   // flags used in handling internal picture buffer with border pels
   firstTimeEver=1;
   firstTimePicture=1;

#ifdef VERIFICATION_DUMP
    if ((vdump_fp=fopen(VDUMP_FN, "wb"))==NULL)
    {
        fprintf(stderr, "ArcMotionEstimationModel.cpp: Error - Unable to open %s for writing\n", VDUMP_FN);
        assert(false);
}
#endif // VERIFICATION_DUMP

}


ArcMotionEstimationModel::~ArcMotionEstimationModel()
{
   if (refYWthBrdrPels!=NULL) delete [] refYWthBrdrPels;

#ifdef VERIFICATION_DUMP
    if (vdump_fp!=NULL) fclose(vdump_fp);
#endif

}




// Generates the border pel for the current luma buffer
// TODO This will change as it copies the complete luma data to the buffer.
// Needs to be modifeid to use the same buffer with border pels
//
//      1-------------------------
//      --------------------------
//      ---2===================---
//      ---====================---
//
//
// 1. is address of memory area with border pel and
// 2. is the calculated start of y data.
// The picture width and height has to  be adjusted according to the border pels
void ArcMotionEstimationModel::GenerateLumaRefBorderPels(void)
{


  GenerateBorderPels((pel_t *) (auxReg.ME_REFERENCE_START << ME_REFERENCE_START_SHIFT), refYWthBrdrPels, NUMBER_OF_TOP_BORDER_PELS,
                       NUMBER_OF_BOTTOM_BORDER_PELS, NUMBER_OF_LEFT_BORDER_PELS,
                       NUMBER_OF_RIGHT_BORDER_PELS);

}

// Border clipping code
// Mpeg4 and h264 can have different border sizes.
// TODO ifthe sizes are the same for both h264/Mpeg4 ??
void ArcMotionEstimationModel::GenerateBorderPels(unsigned char *srcPic,unsigned char *dstPic,
                                                      int top_pels,int btm_pels,int lft_pels,int rgt_pels)
{
    int y;
    int out_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + lft_pels + rgt_pels;
    unsigned char* in_ptr;
    unsigned char* A_ptr;

    ///////////////////////////////////////////////////////////////////////////////////
    // Create the first NUMBER_OF_TOP_BORDER_PELS lines by copying the actual pels
    // and repeating the left and right edge pels
    ///////////////////////////////////////////////////////////////////////////////////

    in_ptr = srcPic;
    A_ptr = dstPic;

    for (y=0; y<top_pels; y++)
    {
        MeMemorySet(A_ptr, in_ptr, lft_pels);

        MeMemoryRead(A_ptr + lft_pels, in_ptr, (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16)); // like memcpy

        MeMemorySet(A_ptr + lft_pels + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16),
                    in_ptr + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) - 1,
                    rgt_pels);

        A_ptr += out_width;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // Create the middle decoded_picture->m_vsize lines by copying the actual pels
    // and repeating the left and right edge pels
    ///////////////////////////////////////////////////////////////////////////////////

    for (y=0; y<(auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION*16); y++)
    {
        MeMemorySet(A_ptr, in_ptr, lft_pels);

        MeMemoryRead(A_ptr + lft_pels, in_ptr, (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16)); // like memcpy

        MeMemorySet(A_ptr + lft_pels + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16),
                    in_ptr + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) - 1,
                    rgt_pels);

        in_ptr += (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT);
        A_ptr += out_width;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // Create the last NUMBER_OF_BOTTOM_BORDER_PELS lines by copying the actual pels
    // and repeating the left and right edge pels
    ///////////////////////////////////////////////////////////////////////////////////

    in_ptr -= (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT);

    for (y=0; y<btm_pels; y++)
    {
        MeMemorySet(A_ptr, in_ptr, lft_pels);

        MeMemoryRead(A_ptr + lft_pels, in_ptr, (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16)); // like memcpy

        MeMemorySet(A_ptr + lft_pels + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16),
                    in_ptr + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) - 1,
                    rgt_pels);

        A_ptr += out_width;
    }

#if ME_DUMP_CRR > 2
    fprintf(vdump_fp, "\nGenerate Border Pels\n");

    fprintf(vdump_fp, "Input Pels\n");
    in_ptr = srcPic;
    for (int y = 0; y < (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION*16); y++)
    {
        for (int x = 0; x < (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16); x++)
        {
            fprintf(vdump_fp, "%02X ", *in_ptr++);
        }
        in_ptr += (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT) - (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16);
        fprintf(vdump_fp, "\n");
    }

    fprintf(vdump_fp, "Output Pels\n");
    A_ptr = dstPic;
    for (int y = 0; y < (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION*16) + top_pels + btm_pels; y++)
    {
        for (int x = 0; x < (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + lft_pels + rgt_pels; x++)
        {
            fprintf(vdump_fp, "%02X ", *A_ptr++);
        }
        fprintf(vdump_fp, "\n");
    }

    fprintf(vdump_fp, "\n");
#endif // ME_DUMP_CRR

}

// ================================================================================================
// ==================================Motion vector generation code ================================
// ================================================================================================



// Do all motion estimation test & logic
// This will be called by the outer loop for all the macroblocks per frame
// Motion estimation will be done for each macro block
void ArcMotionEstimationModel::DoMotionEstimation(void)
{
    // TODO check how this will be passed down by the h/w
    bool is_16x16 = false;
    bool is_8x8   = false;
    bool is_8x16     = false;
    bool is_16x8  = false;

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "################################################################################\n");
    fprintf(vdump_fp, "MB: %d (%d, %d)\n", (auxReg.ME_MB_INDEX__MB_Y_INDEX * auxReg.ME_MB_DIMENSION__MB_X_DIMENSION) + auxReg.ME_MB_INDEX__MB_X_INDEX, auxReg.ME_MB_INDEX__MB_X_INDEX, auxReg.ME_MB_INDEX__MB_Y_INDEX);
    fprintf(vdump_fp, "   Lambda = 0x%X\n\n", auxReg.ME_LAMBDA);
#endif

    // reset these flags at the start of an iteration
    auxReg.ME_RESULT__16x16_SEARCH = 0;
    auxReg.ME_RESULT__8x8_SEARCH = 0;
    auxReg.ME_RESULT__SKIP = 0;

    // initialise lambda
    int mv_lambda = auxReg.ME_LAMBDA;

    mvLambdaTimesCodeLength[0] = 0;

    for (int code_length=1; code_length<16; code_length++)
    {
        mvLambdaTimesCodeLength[code_length]
        = mvLambdaTimesCodeLength[code_length-1] + mv_lambda;
    }

    // initialise block offsets
    unsigned int hsize = auxReg.ME_MB_DIMENSION__MB_X_DIMENSION;

    for (int block_number=0 ; block_number<16; block_number++)
    {
        frame_y_4x4_block_offset[block_number]  = ((block_number & 3) << 2) + ( hsize * (block_number & 12));
    }

    // if first ever iteration then initialise padding reference buffer
    // can't initialise in the constructor as we need the picture size info
    if (firstTimeEver==1)
    {
        if (refYWthBrdrPels==NULL)
        {
            int refWthBrdrSize=(NUMBER_OF_LEFT_BORDER_PELS + (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_RIGHT_BORDER_PELS) *
                (NUMBER_OF_TOP_BORDER_PELS + (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION*16) + NUMBER_OF_BOTTOM_BORDER_PELS);

            refYWthBrdrPels=new unsigned char[refWthBrdrSize];

            if (refYWthBrdrPels==NULL)
            {
                fprintf(stderr, "ArcMotionEstimationModel: Error - Unable to allocate memory for refYWthBrdrPels\n");
                assert(false);
            }
        }
        firstTimeEver=0;
    }

#ifdef ARC_ISS_ME_MODEL
    // check to make sure that accesses to main memory and sdm don't go out of bounds
    if (auxReg.ME_SETUP__ITER_TYPE == 0x3) // manual sad mode
    {
        // calculate the maximum allowable address for the current, above which overflow would occur
        // don't need to worry about search area increasing the address size as the hardware provides address clipping at the edge of the screen
        u_int32 maxReferenceAddr = 0xffffffff - (auxReg.ME_CONTROL0__MV_Y * 16 * (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT)) - (15 * (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT)) - (auxReg.ME_CONTROL0__MV_X * 16) - 16;

        // calculate the maximum allowable address for the current, above which overflow would occur
        u_int32 maxCurrentAddr   = 0xffffffff - (auxReg.ME_CONTROL0__MV_Y * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) - (15 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) - (auxReg.ME_CONTROL0__MV_X * 16) - 16;

        if ((auxReg.ME_REFERENCE_START << ME_REFERENCE_START_SHIFT) > maxReferenceAddr || // read reference from main memory
            (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT) > maxCurrentAddr) // read current from main memory
        {
            auxReg.ME_RESULT__CONFIG_ERR = 1;
        }
    }
    else // not manual sad mode
    {
        // calculate the maximum allowable address for the current, above which overflow would occur
        // don't need to worry about search area increasing the address size as the hardware provides address clipping at the edge of the screen
        u_int32 maxReferenceAddr = 0xffffffff - (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 * (auxReg.ME_REFERENCE_STRIDE << ME_REFERENCE_STRIDE_SHIFT));

        // calculate the maximum allowable address for the current, above which overflow would occur
        u_int32 maxCurrentAddr   = 0xffffffff - (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT));

        if ((auxReg.ME_REFERENCE_START << ME_REFERENCE_START_SHIFT) > maxReferenceAddr || // read reference from main memory
            (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT) > maxCurrentAddr || // read current from main memory
            (auxReg.ME_MB_SDM_ADDR + 256) > (SDM_BASE_ADDRESS + 0xffff) || // write current to sdm
            (auxReg.ME_REFERENCE_SDM_START + 256) > (SDM_BASE_ADDRESS + 0xffff) || // write reference to sdm
            (auxReg.ME_A_POINTER + 16) > (SDM_BASE_ADDRESS + 0xffff) || // read left neighbour from sdm
            (auxReg.ME_D_POINTER + 16) > (SDM_BASE_ADDRESS + 0xffff) || // read above neighbour from sdm
            (auxReg.ME_CURRENT_POINTER + 32) > (SDM_BASE_ADDRESS + 0xffff)) // write current neighbour to sdm
        {
            auxReg.ME_RESULT__CONFIG_ERR = 1;
        }
    }
#endif // ARC_ISS_ME_MODEL


    // start of an iteration


    // debug point start_iteration_ctrl

    // check system dma is enabled
    if (auxReg.ME_SETUP__SYS_DMA_EN==1)
    {
        // if not doing manual sad
        if (auxReg.ME_SETUP__ITER_TYPE!=0x3)
        {
            // read neighbouring information
            ReadNeighbourDataFromMPC();

            // generate skipped mv
            // In case of P slice, calculate the skip mode motion vectors and store in mpc.MVX & Y etc.
            GenerateSkippedMotionVectors();

            // if no ME then write neighbour information and an unknown prediction mode to mpc
            if (auxReg.ME_CONTROL0__REF_PIX_CACHE_EN==0 || auxReg.ME_CONTROL0__NO_ME==1)
            {
                if (auxReg.ME_SETUP__CODEC == H264)
                    WriteNeighbourDataToMPC(MBCT_H264_INTRA16X16);
                else
                    WriteNeighbourDataToMPC(MBCT_MPEG4_INTRA);
            }

            // fetch and write current mb to sdm
            WriteCurrentMBToSDM();

            // debug point ch_out_mb_ctrl


            // send mb channel command
            if (auxReg.ME_BASE_BR_ADDR__CHAN_DIS==0) CreateChannelCommandMB();

        } // if (auxReg.ME_SETUP__ITER_TYPE!=0x3)


        // if performing ME then do so
        if (auxReg.ME_CONTROL0__REF_PIX_CACHE_EN==1 && auxReg.ME_CONTROL0__NO_ME==0)
        {

            // is reference pixel data in the pixel cache, this is essentially a background operation
            // if first iteration in a new picture then copy reference buffer to internal buffer and add picture border padding
            if (firstTimePicture==1)
            {
                GenerateLumaRefBorderPels();

                firstTimePicture=0;
            }

            // if manual sad mode
            if (auxReg.ME_SETUP__ITER_TYPE==0x3)
            {

                // perform manual 8x8 sad
                Manual8x8SAD();

            } // if (auxReg.ME_SETUP__ITER_TYPE==0x3)
            else // (auxReg.ME_SETUP__ITER_TYPE!=0x3)
            {

                // generate 16x16 motion vector predictor
                GeneratePredictedMotionVectors(0, 0, H264_16X16, predP16x16VecX, predP16x16VecY);

                // Calculate skipped cost
                // If true it meets the skip threshhold
                if(!DoSkipTestLuma())
                {
                    is_16x16 = true;
                    auxReg.ME_RESULT__16x16_SEARCH = 1;
                    // Perform Block matching
                    if(!StepSearch16x16MB() &&
                        (auxReg.ME_SETUP__ITER_TYPE==0x0 || auxReg.ME_SETUP__ITER_TYPE==0x2) )
                    {
                        if((auxReg.ME_SETUP__CODEC==H264) && (auxReg.ME_SETUP__ITER_TYPE==0x2))
                        {
                            StepSearch8x16MB();
                            is_8x16 = true;

                            StepSearch16x8MB();
                            is_16x8 = true;
                        }

                        // 16x16 match is not good enough, need to check 8x8
                        StepSearch8x8MB();
                        is_8x8 = true;
                        auxReg.ME_RESULT__8x8_SEARCH = 1;
                    }
                }

                // candidate selection
                SetTotalModeCost(is_16x16,is_16x8, is_8x16, is_8x8);

                // generate reference block and write back to sdm
                GenerateAndWriteReferenceMBToSDM();

                // write motion vector results and neighbour info back to sdm
                WriteResultsToMPC();

            } // else (auxReg.ME_SETUP__ITER_TYPE!=0x3)


            // calculate block pattern (needs to be done before index counters are incremented)
            if (auxReg.ME_SETUP__BLK_PAT_EN) CalculateBlockPattern();
            else auxReg.ME_RESULT__BLK_PAT = 0;

            // debug point ch_out_final_ctrl


            // send final channel command
            if (auxReg.ME_BASE_BR_ADDR__CHAN_DIS==0) CreateChannelCommandFinal();


        } // if (auxReg.ME_CONTROL0__REF_PIX_CACHE_EN==1 && auxReg.ME_CONTROL0__NO_ME==0)


        // if not manual sad mode
        if (auxReg.ME_SETUP__ITER_TYPE!=0x3)
        {
            IncrementMBIndices();
        } // if (auxReg.ME_SETUP__ITER_TYPE!=0x3)


    } // if (auxReg.ME_SETUP__SYS_DMA_EN==1)


    // end of an iteration

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n");
    fflush(vdump_fp);
#endif

}


//******************************************************************************
// Name:         IncrementMBIndices
// Purpose:      This function is called after each iteration. It handles the
//               incrementing of the MB index counters and wrapping at picture
//               boundaries
// Arguments:    void
// Return Value: void
//******************************************************************************
void ArcMotionEstimationModel::IncrementMBIndices(void)
{
    //increment index counters
    auxReg.ME_MB_INDEX__MB_X_INDEX++;

    if (auxReg.ME_MB_INDEX__MB_X_INDEX >= auxReg.ME_MB_DIMENSION__MB_X_DIMENSION)
    {
        auxReg.ME_MB_INDEX__MB_X_INDEX=0;
        auxReg.ME_MB_INDEX__MB_Y_INDEX++;
    }

    if (auxReg.ME_MB_INDEX__MB_Y_INDEX >= auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION)
    {
        auxReg.ME_MB_INDEX__MB_Y_INDEX=0;
    }
}


// Generates MVs/ MVp
// MVs will point to the required quater pel offset reference 16x16 macroblock
// which will be used for skip test.
// Assumption, before this function is called all the inputs struct will be set to the
// correct values
void ArcMotionEstimationModel::GenerateSkippedMotionVectors(void)
{
   // Skipped motion vectors are generated depending upon the codec type
   switch (auxReg.ME_SETUP__CODEC)
   {
      case H264:
         // H264 skipped motion vector generation
         ArcCalculateH264SkippedMotionVectors();
         break;

      case MPEG4:
         ArcCalculateMpeg4SkippedMotionVectors();
         break;

      case H263:
         ArcCalculateH263SkippedMotionVectors();
         break;

      default: // Error
         assert(false);
   }
}



void ArcMotionEstimationModel::GeneratePredictedMotionVectors(int block_index,
                                                      int vector_number,
                                                       h264_block_shape_type_t block_shape,
                                                       int& pred_hor, int& pred_ver)
{
   switch (auxReg.ME_SETUP__CODEC)
   {
      case H264:
         /* mv_array_position is not used */
         ArcGetH264MotionVectorPrediction(block_index, vector_number, block_shape, pred_hor, pred_ver);
         break;

      case MPEG4:
         ArcGetMpeg4MotionVectorPrediction(vector_number, block_shape, pred_hor, pred_ver);
         break;

      case H263:
         ArcGetH263MotionVectorPrediction(vector_number, block_shape, pred_hor, pred_ver);
         break;

      default: // Error
         assert(false);
   }
}


// ===============================================================================================
// ==================================End of Motion vector generation==============================
// ===============================================================================================



//  Calculates the skipped cost of a macroblock in a P slice
//      using the SDD.

// TODO change this to the Input/Output data structure

bool  ArcMotionEstimationModel::DoSkipTestLuma(void)
{
  unsigned char * current;
    int ref_picture_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;
    int h_inc;


    int modified_mb_row = auxReg.ME_MB_INDEX__MB_Y_INDEX;

    bool ret = false;

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "----------------------------------------\n");
    fprintf(vdump_fp, "Search: Skip\n\n");
#endif

#ifdef ME_DUMP_SUBPEL
    int vecRes = (auxReg.ME_SETUP__CODEC == H264) ? 4 : 2;
    int save_mv_for_hor = mv_for_hor[0];
    int save_mv_for_ver = mv_for_ver[0];
    MeDumpSubPel16x16((PSliceSkippedHor/vecRes)*vecRes, (PSliceSkippedVer/vecRes)*vecRes);
    mv_for_hor[0] = save_mv_for_hor;
    mv_for_ver[0] = save_mv_for_ver;
#endif

    //////////////////////////////////////////////////////////////////////////////////
    // Luminance
    //////////////////////////////////////////////////////////////////////////////////

   current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16);
   h_inc = (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);


    int previous_y_offset;

    previous_y_offset = (((16 * modified_mb_row) + NUMBER_OF_TOP_BORDER_PELS) * ref_picture_width)
                        + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;

    unsigned char * previous;

    mv_for_hor[0] = PSliceSkippedHor;
    mv_for_ver[0] = PSliceSkippedVer;

    if ((0 == mv_for_hor[0]) && (0 == mv_for_ver[0]))
    {
        // Simple case of zero vector
        previous = refYWthBrdrPels + previous_y_offset;

        SkippedSAD_4[0] = CalculateSAD4(current, previous, h_inc, ref_picture_width);
        SkippedSAD_4[1] = CalculateSAD4(current+4, previous + 4, h_inc, ref_picture_width);
        SkippedSAD_4[2] = CalculateSAD4(current+8, previous + 8, h_inc, ref_picture_width);
        SkippedSAD_4[3] = CalculateSAD4(current+12, previous + 12, h_inc, ref_picture_width);

        SkippedSAD_4[4] = CalculateSAD4(current + 4*h_inc    , previous + 4*ref_picture_width, h_inc, ref_picture_width);
        SkippedSAD_4[5] = CalculateSAD4(current + 4*h_inc + 4, previous + 4*ref_picture_width + 4, h_inc, ref_picture_width);
        SkippedSAD_4[6] = CalculateSAD4(current + 4*h_inc + 8, previous + 4*ref_picture_width + 8, h_inc, ref_picture_width);
        SkippedSAD_4[7] = CalculateSAD4(current + 4*h_inc + 12,previous + 4*ref_picture_width + 12, h_inc, ref_picture_width);

        SkippedSAD_4[8] =  CalculateSAD4(current + 8*h_inc     , previous + 8*ref_picture_width , h_inc, ref_picture_width);
        SkippedSAD_4[9] =  CalculateSAD4(current + 8*h_inc + 4 , previous + 8*ref_picture_width + 4, h_inc, ref_picture_width);
        SkippedSAD_4[10] = CalculateSAD4(current + 8*h_inc + 8 , previous + 8*ref_picture_width + 8, h_inc, ref_picture_width);
        SkippedSAD_4[11] = CalculateSAD4(current + 8*h_inc + 12, previous + 8*ref_picture_width + 12, h_inc, ref_picture_width);

        SkippedSAD_4[12] = CalculateSAD4(current + 12*h_inc     , previous + 12*ref_picture_width , h_inc, ref_picture_width);
        SkippedSAD_4[13] = CalculateSAD4(current + 12*h_inc + 4 , previous + 12*ref_picture_width + 4, h_inc, ref_picture_width);
        SkippedSAD_4[14] = CalculateSAD4(current + 12*h_inc + 8 , previous + 12*ref_picture_width + 8, h_inc, ref_picture_width);
        SkippedSAD_4[15] = CalculateSAD4(current + 12*h_inc + 12, previous + 12*ref_picture_width + 12, h_inc, ref_picture_width);

#ifdef ME_DUMP_CRR
        MeDumpCurrRefRes ("Skip (Zero)", 16, 16, current, previous, h_inc, ref_picture_width);
#endif // ME_DUMP_CRR

    }
    else
    {
      switch(auxReg.ME_SETUP__CODEC)
      {
         case H264:
            ArcMakeH264InterPrediction(MBCT_H264_SKIPPED);
            break;
         case MPEG4:
         case H263:
            ArcMakeH263InterPrediction(MBCT_MPEG4_SKIPPED);
            break;
         default:
            fprintf(stderr, "ArcMotionEstimationModel::DoSkipTestLuma - Error: Unsupported Codec\n");
            assert(false);
            break;
      }


        SkippedSAD_4[0]  = CalculateSAD4(current  , y_prediction_data, h_inc, 16);
        SkippedSAD_4[1]  = CalculateSAD4(current+4, y_prediction_data + 4, h_inc, 16);
        SkippedSAD_4[2]  = CalculateSAD4(current+8, y_prediction_data + 8, h_inc, 16);
        SkippedSAD_4[3]  = CalculateSAD4(current+12, y_prediction_data + 12, h_inc, 16);

        SkippedSAD_4[4]  = CalculateSAD4(current + 4*h_inc    , y_prediction_data + 64, h_inc, 16);
        SkippedSAD_4[5]  = CalculateSAD4(current + 4*h_inc + 4, y_prediction_data + 68, h_inc, 16);
        SkippedSAD_4[6]  = CalculateSAD4(current + 4*h_inc + 8, y_prediction_data + 72, h_inc, 16);
        SkippedSAD_4[7]  = CalculateSAD4(current + 4*h_inc + 12,y_prediction_data + 76, h_inc, 16);

        SkippedSAD_4[8] =  CalculateSAD4(current + 8*h_inc     , y_prediction_data + 128, h_inc, 16);
        SkippedSAD_4[9] =  CalculateSAD4(current + 8*h_inc + 4 , y_prediction_data + 132, h_inc, 16);
        SkippedSAD_4[10] = CalculateSAD4(current + 8*h_inc + 8 , y_prediction_data + 136, h_inc, 16);
        SkippedSAD_4[11] = CalculateSAD4(current + 8*h_inc + 12, y_prediction_data + 140, h_inc, 16);

        SkippedSAD_4[12] = CalculateSAD4(current + 12*h_inc     , y_prediction_data + 192, h_inc, 16);
        SkippedSAD_4[13] = CalculateSAD4(current + 12*h_inc + 4 , y_prediction_data + 196, h_inc, 16);
        SkippedSAD_4[14] = CalculateSAD4(current + 12*h_inc + 8 , y_prediction_data + 200, h_inc, 16);
        SkippedSAD_4[15] = CalculateSAD4(current + 12*h_inc + 12, y_prediction_data + 204, h_inc, 16);

#ifdef ME_DUMP_CRR
        MeDumpCurrRefRes ("Skip (MVP)", 16, 16, current, y_prediction_data, h_inc, 16);
#endif // ME_DUMP_CRR

    }

        int i;

        skippedSAD_8[0] = SkippedSAD_4[0]  + SkippedSAD_4[1]  + SkippedSAD_4[4]  + SkippedSAD_4[5];
        skippedSAD_8[1] = SkippedSAD_4[2]  + SkippedSAD_4[3]  + SkippedSAD_4[6]  + SkippedSAD_4[7];
        skippedSAD_8[2] = SkippedSAD_4[8]  + SkippedSAD_4[9]  + SkippedSAD_4[12] + SkippedSAD_4[13];
        skippedSAD_8[3] = SkippedSAD_4[10] + SkippedSAD_4[11] + SkippedSAD_4[14] + SkippedSAD_4[15];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "SkippedSAD_4x4[0] = 0x%X\n", SkippedSAD_4[0]);
#endif

        skippedModeSSD = SkippedSAD_4[0];

        for(i=1;  i < 16; i++)
        {
            skippedModeSSD += SkippedSAD_4[i];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "SkippedSAD_4x4[%d] = 0x%X\n", i, SkippedSAD_4[i]);
#endif
        }

  // Check threshold
    if( (SkippedSAD_4[0]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[1]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[2]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[3]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[4]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[5]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[6]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[7]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[8]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[9]  < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[10] < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[11] < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[12] < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[13] < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[14] < auxReg.ME_THRESHOLD_SKIP) &&
        (SkippedSAD_4[15] < auxReg.ME_THRESHOLD_SKIP)
        )
    {
        bestCost[0][0] =
        bestCost[1][0] =
        bestCost[1][1] =
        bestCost[2][0] =
        bestCost[2][1] =
        bestCost[3][0] =
        bestCost[3][1] =
        bestCost[3][2] =
        bestCost[3][3] = UINT_MAX;
        // set 16x16 vecs in case chroma diffs don't allow a skip later on
        bestHor[0][0] = PSliceSkippedHor;
        bestVer[0][0] = PSliceSkippedVer;
        ret = true;
    }

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n--------------------\n");
    fprintf(vdump_fp, "Skip Result:\n");
    fprintf(vdump_fp, "   MV = (%3d, %3d)\n", PSliceSkippedHor, PSliceSkippedVer);
    fprintf(vdump_fp, "   Cost (SAD) = 0x%X\n", skippedModeSSD);
    if (ret) fprintf(vdump_fp, "   Less Than Threshold\n");
    else fprintf(vdump_fp, "   Not Less Than Threshold\n");
#endif // VERIFICATION_DUMP

    return ret;
}





// Fetch & calc cost for 16x16 at skip motion vector
// Inputs: predected motion vector x & Y
// Outputs best motion vector cost & fetched pixels

bool ArcMotionEstimationModel::StepSearch16x16MB(void)
{
    int step_size;                          // current step size in iteration (8,4,2,1)
    int tempX, tempY;                       // temporary best offset in the iteration
    unsigned int best_match = UINT_MAX;     // lowest cost achieved so far
    int horiz_disp, vert_disp;              // indexes for the loops
    unsigned int match;                             // temporary cost
    int iter;                               // iteration counter for subpel search

   int predX = predP16x16VecX;
   int predY = predP16x16VecY;
    int vecX, vecY;
    int StepOneControl = auxReg.ME_CONTROL1__STEP_ONE_CTRL;
    int SubPelMode = auxReg.ME_CONTROL1__SUB_PEL_CTRL;
    int ref_picture_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;
    unsigned int temp;
   int maxvecX, maxvecY, minvecX, minvecY;
   int maxvecsubX, maxvecsubY, minvecsubX, minvecsubY;
   int vec_shift = 2; // vector precision in H264
    unsigned int step_match ;
    int stepX, stepY;
#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------------------------------------\n");
    fprintf(vdump_fp, "Search: 16x16\n");

    // if mpeg4 then want to multiply vectors by 2 to dump in 1/4 pel resolution
    int vecMul = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

    fprintf(vdump_fp, "\n   MVP.4 = (%3d, %3d)\n", predX*vecMul, predY*vecMul);
#endif


#ifdef TEST_NEIGHBOURS
    int pred[3][2];
#endif // TEST_NEIGHBOURS
    unsigned int stop_soad = UINT_MAX;
    unsigned char * current;
    unsigned char * previous;

    // set MB offset in previous frame
    unsigned int previous_y_offset = (((16 * auxReg.ME_MB_INDEX__MB_Y_INDEX) + NUMBER_OF_TOP_BORDER_PELS) * ref_picture_width)
                        + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;

   if (auxReg.ME_SETUP__CODEC != H264)
   {
       vec_shift = 1; // we are dealing with 1/2 pel units

      if(auxReg.ME_SETUP__PAD_CTRL > 0)
      {
         maxvecX = maxvecY= 15;
         minvecX = minvecY = -15;
         maxvecsubX = maxvecsubY = 31;
         minvecsubX = minvecsubY = -31;
      }
      else
      {
         minvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ? 0 : -15;
         minvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ? 0 : -15;
         maxvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 15;
         maxvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 15;
         minvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ? 0 : -31;
         minvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ? 0 : -31;
         maxvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 31;
         maxvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 31;

      }
      if(auxReg.ME_SETUP__EMV)
      { // restricted to be within (-16, 15.5) of predictor
        minvecX = MAX(minvecX, (-32 + predX)/2); // nb pred is in 1/2 pel units, but these values are ints
        minvecY = MAX(minvecY, (-32 + predY)/2);
        maxvecX = MIN(maxvecX, ( 31 + predX)/2);
        maxvecY = MIN(maxvecY, ( 31 + predY)/2);
        minvecsubX = MAX(minvecsubX, (-32 + predX));
        minvecsubY = MAX(minvecsubY, (-32 + predY));
        maxvecsubX = MIN(maxvecsubX, ( 31 + predX));
        maxvecsubY = MIN(maxvecsubY, ( 31 + predY));
      }
   }
   else { // 264
        minvecX = minvecY = -15;
        maxvecX = maxvecY = 15;
        maxvecsubX = maxvecsubY = 63;
        minvecsubX = minvecsubY = -63;
   }

    step_size = StepOneControl != 0 ? 4 : 8;
    // 0 = full step search
    // 1 = 3 points in stage 1 : (-8,0), (0,0) and (0,8)
    // 2 = skip first step


    vecX = vecY = 0;

    current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16);


#ifdef TRY_INT_MVP  // ECO 1/5/07 step 1)

    // try the MVP vector as an integer vector
#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------\n");
    fprintf(vdump_fp, "Points: MVP Int\n\n");
#endif

   // round to nearest int
    tempX = predX < 0 ? -((-predX+vec_shift)>>vec_shift): ((predX+vec_shift)>>vec_shift);
    tempY = predY < 0 ? -((-predY+vec_shift)>>vec_shift) : ((predY+vec_shift)>>vec_shift); // put into integer units
    tempX = CLIP_VEC(tempX);
    tempY = CLIP_VEC(tempY);

    if(!NOT_IN_RANGE_INT(tempX, tempY, minvecX, minvecY, maxvecX, maxvecY))
    {
        previous = refYWthBrdrPels
                            + previous_y_offset + (tempY*ref_picture_width) + (tempX);

        match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                mvLambdaTimesCodeLength[encoder_vector_code_length(((tempX)<<vec_shift)-predX)] +
                        mvLambdaTimesCodeLength[encoder_vector_code_length(((tempY)<<vec_shift)-predY)];

#ifdef ME_DUMP_CRR
        MeDumpCurrRefRes ("16x16 (MVP Int)", 16, 16, current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width);
#endif // ME_DUMP_CRR

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }
    }
#endif // TRY_INT_MVP // end of step 1)


#ifdef TEST_NEIGHBOURS  // ECO 1/5/07 step 2) 3) and 4)

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------\n");
    fprintf(vdump_fp, "Points: A, B and C Int\n\n");
#endif

    pred[0][0] = (leftMB.validID && !leftMB.intra) ?  leftMB.mv[1].mvx : 0; pred[0][1] = (leftMB.validID && !leftMB.intra) ? leftMB.mv[1].mvy: 0;
    pred[1][0] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvx : 0; pred[1][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvy: 0;
    if(aboveRightMB.validID) {
        pred[2][0] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvx : 0;
        pred[2][1] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvy : 0;
    }
    else if(aboveLeftMB.validID) {
        pred[2][0] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvx : 0;
        pred[2][1] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvy : 0;
    }
    else {
        pred[2][0] = pred[2][1] = 0;
    }

    for(int neigh = 0; neigh < 3; neigh++){

   // round to nearest int
        tempX = pred[neigh][0] < 0 ? -((-pred[neigh][0]+vec_shift)>>vec_shift) : ((pred[neigh][0]+vec_shift)>>vec_shift);
        tempY = pred[neigh][1] < 0 ? -((-pred[neigh][1]+vec_shift)>>vec_shift) : ((pred[neigh][1]+vec_shift)>>vec_shift); // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);

        if(NOT_IN_RANGE_INT(tempX, tempY, minvecX, minvecY, maxvecX, maxvecY))continue; //
        previous = refYWthBrdrPels
                            + previous_y_offset + (tempY*ref_picture_width) + (tempX);

        match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                mvLambdaTimesCodeLength[encoder_vector_code_length(((tempX)<<vec_shift)-predX)] +
                            mvLambdaTimesCodeLength[encoder_vector_code_length(((tempY)<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }
    }// for neigh

#endif // TEST_NEIGHBOURS - end of step 2) 3) and 4)




#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------\n");
    fprintf(vdump_fp, "Points: Integer\n\n");
#endif

// ECO 1/5/07 step 5)
    //  in this section we need to keep a separate set of best vectors
    // and then compare with previous best results at the end of the 4 step search
    stepX = stepY = 0;
    // try (0,0) vector
    previous = refYWthBrdrPels+ previous_y_offset ;

#ifdef ME_DUMP_CRR
    MeDumpCurrRefRes ("16x16 (Integer 0,0)", 16, 16, current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width);
#endif // ME_DUMP_CRR

    step_match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, UINT_MAX, 16)+
      mvLambdaTimesCodeLength[encoder_vector_code_length(-predX)] +
         mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];


// ECO end of step 5)
#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", 0, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, UINT_MAX, 16), (step_match + auxReg.ME_LAMBDA)<<2);
#endif
// ECO 1/5/07 step 6)
    if(StepOneControl == 1)
    { // only do 3 points

        if(!(NOT_IN_RANGE_INT(-8, 0, minvecX, minvecY, maxvecX, maxvecY)) )
        {
            // try (-8,0) vector
            previous = refYWthBrdrPels  + previous_y_offset + (0*ref_picture_width) - 8;
            match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-(8<<vec_shift)-predX)] +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
             fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", -32, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

            if(match < step_match) {
                stepX = -8;
                stepY = 0;
                step_match = match;
            }
        }


        if(!(NOT_IN_RANGE_INT(8, 0, minvecX, minvecY, maxvecX, maxvecY)) )
        {
        // try (8,0) vector
            previous = refYWthBrdrPels  + previous_y_offset + (0*ref_picture_width) + 8;

            match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                mvLambdaTimesCodeLength[encoder_vector_code_length((8<<vec_shift)-predX)] +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", 32, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

            if(match < step_match) {
                stepX = 8;
                stepY = 0;
                step_match = match;
            }
        }// 8 < maxvecX

    }//if(StepOneControl == 1)


    while (step_size > 0)
    {
        tempX = stepX; tempY = stepY; // in case we don't find a better one

        for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)// horizontal disp
        {
            for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size) // vertical disp
            {

                if(!(horiz_disp|vert_disp))continue; // have already done it

            //  if((auxReg.ME_SETUP__CODEC != H264) &&
            //     (  (horiz_disp + stepX < minvecX)
            //     || (horiz_disp + stepX > maxvecX)
            //     || (vert_disp + stepY < minvecY)
            //     || (vert_disp + stepY > maxvecY))) continue; // not allowed over the edge matches
                if(NOT_IN_RANGE_INT(horiz_disp + stepX, vert_disp + stepY, minvecX, minvecY, maxvecX, maxvecY))continue;
                //offset to previous frame
                previous = refYWthBrdrPels  + previous_y_offset + ((stepY+vert_disp)*ref_picture_width) + (stepX+horiz_disp);

#ifdef ME_DUMP_CRR
                MeDumpCurrRefRes ("16x16 (Integer Loop)", 16, 16, current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width);
#endif // ME_DUMP_CRR

                match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                  mvLambdaTimesCodeLength[encoder_vector_code_length(((stepX+horiz_disp)<<vec_shift)-predX)] +
                  mvLambdaTimesCodeLength[encoder_vector_code_length(((stepY+vert_disp)<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", (stepX + horiz_disp)<<2, (stepY + vert_disp)<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

                if(match < step_match){ // new minimum
                    tempX = stepX + horiz_disp;
                    tempY = stepY + vert_disp;
                    step_match = match;
                }

            }//for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
        }//for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
        stepX = tempX;
        stepY = tempY;


        step_size = step_size >>1;
    }//while (step_size > 0)

    if(step_match < best_match) {
        best_match = step_match;
        vecX = stepX;
        vecY = stepY;
    }

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (Integer)\n", vecX, vecY);
#endif // ME_DUMP_VEC

// ECO end of step 6)

#ifdef TRY_MVP_NEIGHALL  // ECO 1/5/07 step 7)


    if(auxReg.ME_SETUP__PMV == 1)
    {


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 3x3 MVP Int\n\n");
#endif

        int dist = 1;

       // round to nearest int
    tempX = predX < 0 ? -((-predX+vec_shift)>>vec_shift) : ((predX+vec_shift)>>vec_shift);
        tempY = predY < 0 ? -((-predY+vec_shift)>>vec_shift) : ((predY+vec_shift)>>vec_shift); // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
    for(horiz_disp = tempX-dist; horiz_disp <= tempX+dist; horiz_disp += dist)
        {
#ifdef LIMITS
//          if((horiz_disp > 15) || (horiz_disp < -15))continue;
#endif // LIMITS
        for(vert_disp = tempY-dist; vert_disp <= tempY+dist; vert_disp += dist)
            {
#ifdef LIMITS
//              if(vert_disp > 15 || vert_disp < -15)continue;
#endif // LIMITS

                if(NOT_IN_RANGE_INT(horiz_disp, vert_disp, minvecX, minvecY, maxvecX, maxvecY))continue;

                previous = refYWthBrdrPels
                                + previous_y_offset + (vert_disp*ref_picture_width) + (horiz_disp);

                match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16) +
                            mvLambdaTimesCodeLength[encoder_vector_code_length(((horiz_disp)<<vec_shift)-predX)] +
                                mvLambdaTimesCodeLength[encoder_vector_code_length(((vert_disp)<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
               fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", horiz_disp<<2, vert_disp<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, stop_soad, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

                if(match < best_match){ // new minimum
                    vecX = horiz_disp;
                    vecY = vert_disp;
                    best_match = match;
                }
            }
        }
    } // if auxreg PMV is set
#endif // TRY_MVP_NEIGHALL - end of step 7)

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (3x3 MVP Int)\n", vecX, vecY);
#endif // ME_DUMP_VEC



   if (auxReg.ME_SETUP__CODEC == H264)
   {
    vecX = vecX <<2; // put into quarter pel units
    vecY = vecY <<2;
      // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)
      step_size = SubPelMode > 0 ? 2 : 0; // 0, 2, 2
   }
   else
   {
      vecX = vecX <<1; // put into half pel units
      vecY = vecY <<1;
      // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)
      step_size = SubPelMode > 1 ? 2 : 1; // 1, 1, 2
   }

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (Upscale)\n", vecX, vecY);
#endif // ME_DUMP_VEC

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------\n");
    fprintf(vdump_fp, "Points: Sub-Pel\n\n");
#endif

#ifdef ME_DUMP_SUBPEL
    int save_mv_for_hor = mv_for_hor[0];
    int save_mv_for_ver = mv_for_ver[0];
    MeDumpSubPel16x16(vecX, vecY);
    mv_for_hor[0] = save_mv_for_hor;
    mv_for_ver[0] = save_mv_for_ver;
#endif


    for(iter = 0; iter < SubPelMode; iter++, step_size = step_size>>1)
    {
        tempX = vecX; tempY = vecY;
        for(int counter = 0; counter < 8; counter++)
        {
            vert_disp = subpel_order[iter][counter][0]*step_size;

            horiz_disp = subpel_order[iter][counter][1]*step_size;
    //  for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)// vertical disp
    //  {
    //      for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)// horiz disp
    //      {
    //      if(!(horiz_disp|vert_disp))continue; // have already done it

     //       if((auxReg.ME_SETUP__CODEC != H264) &&
     //            ((horiz_disp + vecX < (minvecX<<step_size)) // these limits are in integer units
     //          || (horiz_disp + vecX > (maxvecX<<step_size))
     //          || (vert_disp  + vecY < (minvecY<<step_size))
     //          || (vert_disp  + vecY > (maxvecY<<step_size)))) continue; // not allowed over the edge matches

            if(NOT_IN_RANGE_INT(horiz_disp+vecX, vert_disp+vecY, minvecsubX, minvecsubY, maxvecsubX, maxvecsubY))continue;
                //set mv array to do prediction
            mv_for_hor[0] = vecX+horiz_disp;
            mv_for_ver[0] = vecY+vert_disp;

                // predict 16x16 array
            switch(auxReg.ME_SETUP__CODEC)
            {
               case H264:
                  ArcMakeH264InterPrediction(MBCT_H264_INTER16X16); // Note this also does the chroma so there is duplication of effort at present
                  break;
               case MPEG4:
               case H263:
                  ArcMakeH263InterPrediction(MBCT_MPEG4_INTER16X16);
                  break;
               default:
                  fprintf(stderr, "ArcMotionEstimationModel::StepSearch16x16MB - Error: Unsupported Codec\n");
                  assert(false);
                  break;
            }

                match = ArcCalculateSSD(current, y_prediction_data, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 16) +
                     mvLambdaTimesCodeLength[encoder_vector_code_length((vecX+horiz_disp)-predX)] +
                     mvLambdaTimesCodeLength[encoder_vector_code_length((vecY+vert_disp)-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", (vecX + horiz_disp)*vecMul, (vecY + vert_disp)*vecMul, ArcCalculateSSD(current, y_prediction_data, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 16), (match + auxReg.ME_LAMBDA)<<2);
#endif

                if(match < best_match){ // new minimum
                    tempX = vecX + horiz_disp;
                    tempY = vecY + vert_disp;
                    best_match = match;
                }
            //}//   for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
        }//for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
        vecX = tempX;
        vecY = tempY;
    }//for(step_size = SubPelMode; step_size > 0; step_size = step_size >>1)

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (Sub-Pel)\n", vecX, vecY);
#endif // ME_DUMP_VEC

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------\n");
    fprintf(vdump_fp, "Points: 16x16 Skip MV\n\n");
#endif

    // test skipVec as an Inter16x16 MB
    temp = skippedSAD_8[0]+skippedSAD_8[1]+skippedSAD_8[2]+skippedSAD_8[3] +
               mvLambdaTimesCodeLength[encoder_vector_code_length((PSliceSkippedHor)-predX)] +
               mvLambdaTimesCodeLength[encoder_vector_code_length((PSliceSkippedVer)-predY)];

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*4) = 0x%05X\n", PSliceSkippedHor*vecMul, PSliceSkippedVer*vecMul, skippedSAD_8[0]+skippedSAD_8[1]+skippedSAD_8[2]+skippedSAD_8[3], (temp + auxReg.ME_LAMBDA)<<2);
#endif

    if(best_match >= temp) // highest priority
    {
        best_match = temp;
        vecX = PSliceSkippedHor;
        vecY = PSliceSkippedVer;
    }

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (16x16 Skip MV)\n", vecX, vecY);
#endif // ME_DUMP_VEC

    // save the results
    bestCost[0][0] = best_match;

        // check against skip cost as Inter16x16


    bestBitCost[0][0] =
               mvLambdaTimesCodeLength[encoder_vector_code_length((vecX)-predX)] +
               mvLambdaTimesCodeLength[encoder_vector_code_length((vecY)-predY)];


    bestHor[0][0] = vecX;
    bestVer[0][0] = vecY;

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (Best)\n", vecX, vecY);
#endif // ME_DUMP_VEC


#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n--------------------\n");
    fprintf(vdump_fp, "16x16 Result:\n");
    fprintf(vdump_fp, "   MV.25 = (%3d, %3d)\n", bestHor[0][0]*vecMul, bestVer[0][0]*vecMul);
    fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
            (bestCost[0][0] - bestBitCost[0][0]),
            bestBitCost[0][0],
            auxReg.ME_LAMBDA,
            bestCost[0][0] + auxReg.ME_LAMBDA);
    fprintf(vdump_fp, "   Cost (SAD*4 + mvRD*4 + modeRD*4) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
            (bestCost[0][0] - bestBitCost[0][0])*4,
            bestBitCost[0][0]*4,
            auxReg.ME_LAMBDA*4,
            bestCost[0][0]*4 + auxReg.ME_LAMBDA*4);
#endif // VERIFICATION_DUMP

#ifdef ME_DUMP_VEC
    fprintf(vdump_fp, "vecX = %3d, vecY = %3d (End)\n", vecX, vecY);
#endif // ME_DUMP_VEC

  if(best_match - bestBitCost[0][0] < auxReg.ME_THRESHOLD_16x16)
    return true;

  return false;
}


void ArcMotionEstimationModel::StepSearch8x16MB(void)
{


    int step_size;                          // current step size in iteration (8,4,2,1)
    int tempX, tempY;                       // temporary best offset in the iteration
    unsigned int best_match = UINT_MAX;     // lowest cost achieved so far
    int horiz_disp, vert_disp;              // indexes for the loops
    unsigned int match;                     // temporary cost
    int iter;                               // iteration counter
    //unsigned int temp;
    int predX, predY;
    int vecX, vecY;
    int StepOneControl = auxReg.ME_CONTROL1__STEP_ONE_CTRL;
    int SubPelMode = auxReg.ME_CONTROL1__SUB_PEL_CTRL;
    int ref_picture_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;


    unsigned char * current;
    unsigned char * previous;
    int current_offset, previous_offset, block_offset;

    int block_height, block_index;
    unsigned int step_match ;
    int stepX, stepY;

    // setup MB offset in previous frame
    unsigned int previous_y_offset = (((16 * auxReg.ME_MB_INDEX__MB_Y_INDEX) + NUMBER_OF_TOP_BORDER_PELS) * ref_picture_width)
                        + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;
#ifdef TEST_NEIGHBOURS
    int pred[3][2];
#endif // TEST_NEIGHBOURS

    //control->mtype = H264_INTER_8X16;
    block_height = 16;
        // 0 = full step search
        // 1 = 3 points in stage 1 : (-8,0), (0,0) and (0,8)
        // 2 = skip first step

    // for each 16x8 block
    for (int vector_count=0; vector_count<2; vector_count++)
    {
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------------------------------------\n");
        fprintf(vdump_fp, "Search: 8x16\n");
        fprintf(vdump_fp, "Partition: %d\n", vector_count);
#endif

        step_size = StepOneControl != 0 ? 4 : 8;
        // first set up the offsets into current frame, reference frame, 16x16 prediction data and array of motion vectors
        if (1 == vector_count)
        {
            current_offset = 8; // sub block offset into current frame
            previous_offset = 8;// subblock offset into previous frame
            block_offset = 8; // subblock offset into motion compensated 16x16 array
#ifdef TEST_NEIGHBOURS
            pred[0][0] = mv_for_hor[1];
            pred[0][1] = mv_for_ver[1];
            pred[1][0] = (aboveMB.validID && !aboveMB.intra)? aboveMB.mv[3].mvx : 0;
            pred[1][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[3].mvy: 0;
            if(aboveRightMB.validID)
            {
                pred[2][0] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvx : 0;
                pred[2][1] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvy : 0;
            }
            else if(aboveMB.validID)
            {
                pred[2][0] = (!aboveMB.intra) ? aboveMB.mv[2].mvx : 0;
                pred[2][1] = (!aboveMB.intra) ? aboveMB.mv[2].mvy : 0;
            }
            else
            {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }

        else  // 0
        {
            current_offset = 0;
            previous_offset = 0;
            block_offset = 0;
#ifdef TEST_NEIGHBOURS
            pred[0][0] = (leftMB.validID && !leftMB.intra) ?  leftMB.mv[1].mvx : 0;
            pred[0][1] = (leftMB.validID && !leftMB.intra) ?  leftMB.mv[1].mvy: 0;
            pred[1][0] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvx : 0;
            pred[1][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvy: 0;
            //pred[2][0] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[3].mvx : 0;
            //pred[2][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[3].mvy : 0;
            if(aboveMB.validID)
            {
                pred[2][0] = (!aboveMB.intra) ? aboveMB.mv[3].mvx : 0;
                pred[2][1] = (!aboveMB.intra) ? aboveMB.mv[3].mvy : 0;
            }
            else if(aboveLeftMB.validID)
            {
                pred[2][0] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvx : 0;
                pred[2][1] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvy : 0;
            }
            else
            {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }
        best_match = UINT_MAX;

        // get the index of the raster 4x4s for the block
        block_index = h264_8x8_block_index_to_raster_index_table[vector_count];
        // predict motion vector for current 8x8
        //MotionEstimationGetMotionVectorPrediction(control, block_index, vector_count, H264_8X16, array_position, predX,
        //                                    predY);

        GeneratePredictedMotionVectors(block_index, vector_count, H264_8X16, predX, predY);
        // save predicted MVs
        predP8x16VecX[vector_count]=predX;
        predP8x16VecY[vector_count]=predY;

#ifdef VERIFICATION_DUMP
        // if mpeg4 then want to multiply vectors by 2 to dump in 1/4 pel resolution
        int vecMul = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

        fprintf(vdump_fp, "\n   MVP.4 = (%3d, %3d)\n", predX*vecMul, predY*vecMul);
#endif // VERIFICATION_DUMP

        vecX = vecY = 0;

        current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16) + current_offset;

#ifdef TRY_INT_MVP
        // try the MVP vector as an integer vector
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: MVP Int\n\n");
#endif

   // round to nearest int
        tempX = predX < 0 ? (predX-2)/4 : (predX+2)/4;
        tempY = predY < 0 ? (predY-2)/4 : (predY+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
        previous = refYWthBrdrPels  + previous_y_offset + previous_offset + (tempY*ref_picture_width) + tempX;


        match = CalculateSOADWidth8(current, previous,
                    (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempX)*4-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempY)*4-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }

#endif // TRY_INT_MVP  // ECO 1/5/07 step 1)

#ifdef TEST_NEIGHBOURS // ECO 1/5/07 step 2) 3) and 4)

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: A, B and C Int\n\n");
#endif

    for(int neigh = 0; neigh < 3; neigh++){

   // round to nearest int
        tempX = pred[neigh][0] < 0 ? (pred[neigh][0]-2)/4 : (pred[neigh][0]+2)/4;
        tempY = pred[neigh][1] < 0 ? (pred[neigh][1]-2)/4 : (pred[neigh][1]+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);

        previous = refYWthBrdrPels  + previous_y_offset + previous_offset + (tempY*ref_picture_width) + tempX;


        match = CalculateSOADWidth8(current, previous,
                    (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempX)*4-predX)] +
            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempY)*4-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }
    }// for neigh
#endif // TEST_NEIGHBOURS


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Integer\n\n");
#endif

        // ECO 1/5/07 step 5)
        //  in this section we need to keep a separate set of best vectors
        // and then compare with previous best results at the end of the 4 step search
        stepX = stepY = 0;

        // first calculate (0,0) vector match and set best_match

        previous = refYWthBrdrPels + previous_y_offset + previous_offset;

        step_match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX);
        step_match +=
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", 0, 0, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), step_match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(StepOneControl == 1){ // only do 3 points

            // first try (-8,0)
            previous = refYWthBrdrPels
                        + previous_y_offset + previous_offset + (0*ref_picture_width) - 8 ;

            match = CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-32-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", -32, 0, CalculateSOADWidth8(current, previous,    (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

            if(match < step_match) {
                stepX = -8;
                stepY = 0;
                step_match = match;
            }
            // next try (8,0)
            previous = refYWthBrdrPels
                        + previous_y_offset + previous_offset + (0*ref_picture_width) + 8;

            match = CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(32-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

            if(match < step_match) {
                stepX = 8;
                stepY = 0;
                step_match = match;
            }

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", 32, 0, CalculateSOADWidth8(current, previous,     (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        } //    if(StepOneControl == 1)

        // now main loop for integer search
        while (step_size > 0)
        {
            tempX = stepX; tempY = stepY; // in case we don't find a better one

            for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size){ // horizontal displacement

                for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size){ // vertical displacement

                    if(!(horiz_disp|vert_disp))continue; // have already done it

                    previous = refYWthBrdrPels  + previous_y_offset + previous_offset + ((stepY+vert_disp)*ref_picture_width) + (stepX+horiz_disp);

                    match = CalculateSOADWidth8(current, previous,
                                                (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX);
                    match +=
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((stepX+horiz_disp)*4-predX)] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((stepY+vert_disp)*4-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", (stepX + horiz_disp)<<2, (stepY + vert_disp)<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                    if(match < step_match){ // new minimum
                        tempX = stepX + horiz_disp;
                        tempY = stepY + vert_disp;
                        step_match = match;
                    }

                } //for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            } //for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            stepX = tempX;
            stepY = tempY;


            step_size = step_size >>1; // halve the step size for next iteration
        }

        if(step_match < best_match) {
            best_match = step_match;
            vecX = stepX;
            vecY = stepY;
        }
// ECO end of step 6)


#ifdef TRY_MVP_NEIGHALL  // ECO 1/5/07 step 7)

    if(auxReg.ME_SETUP__PMV == 1)
    {
    // now try 3x3 area around predicted vector

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 3x3 MVP Int\n\n");
#endif

        int dist = 1;

       // round to nearest int
    tempX = predX < 0 ? (predX-2)/4 : (predX+2)/4;
        tempY = predY < 0 ? (predY-2)/4 : (predY+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
    for(horiz_disp = tempX-dist; horiz_disp <= tempX+dist; horiz_disp += dist)
        {
#ifdef LIMITS
            if((horiz_disp > 15) || (horiz_disp < -15))continue;
#endif // LIMITS
        for(vert_disp = tempY-dist; vert_disp <= tempY+dist; vert_disp += dist)
            {
#ifdef LIMITS
                if(vert_disp > 15 || vert_disp < -15)continue;
#endif // LIMITS

                previous = refYWthBrdrPels  + previous_y_offset + previous_offset + (vert_disp*ref_picture_width) + horiz_disp;


                match = CalculateSOADWidth8(current, previous,
                        (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((horiz_disp)*4-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vert_disp)*4-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", horiz_disp<<2, vert_disp<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                if(match < best_match){ // new minimum
                    vecX = horiz_disp;
                    vecY = vert_disp;
                    best_match = match;
                }
            }
        }
    } // id auxreg PMV == 1
#endif // TRY_MVP_NEIGHALL


        vecX = vecX <<2; // put into quarter pel units
        vecY = vecY <<2;



#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Sub-Pel\n\n");
#endif

#ifdef ME_DUMP_SUBPEL
        int save_mv_for_hor = mv_for_hor[0];
        int save_mv_for_ver = mv_for_ver[0];
        MeDumpSubPel8x8(vecX, vecY, vector_count, H264_8X16);
        mv_for_hor[0] = save_mv_for_hor;
        mv_for_ver[0] = save_mv_for_ver;
#endif // ME_DUMP_SUBPEL


        // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)

        step_size = SubPelMode > 0 ? 2 : 0; // 0, 2, 2
        for(iter = 0; iter < SubPelMode; iter++, step_size = step_size>>1)
        {

            tempX = vecX; tempY = vecY; // in case we don't find a new minimum
            for(int counter = 0; counter < 8; counter++)
            {
                vert_disp = subpel_order[iter][counter][0]*step_size;

                horiz_disp = subpel_order[iter][counter][1]*step_size;
        //  for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size) // for vertical displ
        //  {
        //      for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)// horiz disp
        //      {
                    if(!(horiz_disp|vert_disp))continue; // have already done it

                    //set the mv arrays (4x4) for the prediction function
                    mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] =
                    mv_for_hor[block_index+8] = mv_for_hor[block_index+9] = mv_for_hor[block_index+12] = mv_for_hor[block_index+13] = vecX + horiz_disp;
                    mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] =
                    mv_for_ver[block_index+8] = mv_for_ver[block_index+9] = mv_for_ver[block_index+12] = mv_for_ver[block_index+13] = vecY + vert_disp;


                    ArcMakeH264InterPrediction8(vector_count, H264_8X16); // goes into quadrant

                    //MakeInterPrediction8(control, false, vector_count); // goes into quadrant
                    // and it predicts the entire 16x16 MB - we only need an 8x8 quadrant
                    match = CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 16) +
                            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecX+horiz_disp)-predX)] +
                            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecY+vert_disp)-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", (vecX + horiz_disp)*vecMul, (vecY + vert_disp)*vecMul, CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 16), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                    if(match < best_match){ // new minimum
                        tempX = vecX + horiz_disp;
                        tempY = vecY + vert_disp;
                        best_match = match;
                    }

        //      }//for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            }//for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            vecX = tempX;
            vecY = tempY;
        }
#ifdef SKIP_VEC_8
        // now try the 16x16 skip vector - we have already calculated the SAD

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 16x16 Skip MV\n\n");
#endif

        match = skippedSAD_8[vector_count] + skippedSAD_8[vector_count+2] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((PSliceSkippedHor)-predX)] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((PSliceSkippedVer)-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", PSliceSkippedHor*vecMul, PSliceSkippedVer*vecMul, skippedSAD_8[vector_count] + skippedSAD_8[vector_count+2], match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(best_match >= match)  // highest priority
        {
            best_match = match;
            vecX = PSliceSkippedHor;
            vecY = PSliceSkippedVer;
        }
        // end of skip trial
#endif // SKIP_VEC_8


        bestCost[2][vector_count] = best_match;
        bestBitCost[2][vector_count] =
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecX)-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecY)-predY)];


        bestHor[2][vector_count] = vecX;
        bestVer[2][vector_count] = vecY;

        // set the MV arrays so that the next block's MV prediction will be correct
        mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] =
        mv_for_hor[block_index+8] = mv_for_hor[block_index+9] = mv_for_hor[block_index+12] = mv_for_hor[block_index+13] = vecX ;
        mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] =
        mv_for_ver[block_index+8] = mv_for_ver[block_index+9] = mv_for_ver[block_index+12] = mv_for_ver[block_index+13] = vecY ;

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n--------------------\n");
        fprintf(vdump_fp, "8x16 Result - Partition %d:\n", vector_count);
        fprintf(vdump_fp, "   MV.25 = (%3d, %3d)\n", bestHor[2][vector_count]*vecMul, bestVer[2][vector_count]*vecMul);
        fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD/2) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[2][vector_count] - bestBitCost[2][vector_count]),
                bestBitCost[2][vector_count],
                (3 * auxReg.ME_LAMBDA)/2,
                bestCost[2][vector_count] + (3 * auxReg.ME_LAMBDA)/2);
        fprintf(vdump_fp, "   Cost (SAD*4 + mvRD*4 + modeRD*2) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[2][vector_count] - bestBitCost[2][vector_count])*4,
                bestBitCost[2][vector_count]*4,
                (3 * auxReg.ME_LAMBDA)*2,
                bestCost[2][vector_count]*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif // VERIFICATION_DUMP

    }
}

void ArcMotionEstimationModel::StepSearch16x8MB(void)
{

    int step_size;                          // current step size in iteration (8,4,2,1)
    int tempX, tempY;                       // temporary best offset in the iteration
    unsigned int best_match = UINT_MAX;     // lowest cost achieved so far
    int horiz_disp, vert_disp;              // indexes for the loops
    unsigned int match;                     // temporary cost
    int iter;                               // iteration counter
//  unsigned int temp;
    int predX, predY;
    int vecX, vecY;
    int StepOneControl = auxReg.ME_CONTROL1__STEP_ONE_CTRL;
    int SubPelMode = auxReg.ME_CONTROL1__SUB_PEL_CTRL;
    int ref_picture_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;


    unsigned int stop_soad = UINT_MAX;
    unsigned char * current;
    unsigned char * previous;
    int current_offset, previous_offset, block_offset;

    int block_height, block_index;
    int block_number;
    unsigned int step_match ;
    int stepX, stepY;

    // setup MB offset in previous frame
    unsigned int previous_y_offset = (((16 * auxReg.ME_MB_INDEX__MB_Y_INDEX) + NUMBER_OF_TOP_BORDER_PELS) * ref_picture_width)
                        + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;

#ifdef TEST_NEIGHBOURS
    int pred[3][2];
#endif // TEST_NEIGHBOURS
    block_height = 8;
        // 0 = full step search
        // 1 = 3 points in stage 1 : (-8,0), (0,0) and (0,8)
        // 2 = skip first step

    // for each 8x8 block
    for (int vector_count=0; vector_count<2; vector_count++)
    {
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------------------------------------\n");
        fprintf(vdump_fp, "Search: 16x8\n");
        fprintf(vdump_fp, "Partition: %d\n", vector_count);
#endif

        step_size = StepOneControl != 0 ? 4 : 8;
        // first set up the offsets into current frame, reference frame, 16x16 prediction data and array of motion vectors
        if (1 == vector_count)
        {
            current_offset = 8* (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);
            previous_offset = 8*ref_picture_width;
            block_offset = 8*16; // subblock offset into motion compensated 16x16 array
            block_number = 2;
#ifdef TEST_NEIGHBOURS
            pred[0][0] = (leftMB.validID && !leftMB.intra)? leftMB.mv[3].mvx : 0; pred[0][1] = (leftMB.validID && !leftMB.intra) ? leftMB.mv[3].mvy: 0;

            pred[1][0] = mv_for_hor[4]; pred[1][1] = mv_for_ver[4];
            // aboveRight never valid in this case!
             if(leftMB.validID)
            {
                pred[2][0] = (!leftMB.intra) ? leftMB.mv[1].mvx : 0;
                pred[2][1] = (!leftMB.intra) ? leftMB.mv[1].mvy : 0;
            }
            else
            {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }

        else  // 0
        {
            current_offset = 0;
            previous_offset = 0;
            block_offset = 0;
            block_number = 0;
#ifdef TEST_NEIGHBOURS
            pred[0][0] = (leftMB.validID && !leftMB.intra)?  leftMB.mv[1].mvx : 0; pred[0][1] = (leftMB.validID && !leftMB.intra) ? leftMB.mv[1].mvy: 0;
            pred[1][0] = (aboveMB.validID && !aboveMB.intra)? aboveMB.mv[2].mvx : 0; pred[1][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvy: 0;
            if(aboveRightMB.validID)
            {
                pred[2][0] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvx : 0;
                pred[2][1] = (!aboveRightMB.intra) ? aboveRightMB.mv[2].mvy : 0;
            }
            else if(aboveLeftMB.validID)
            {
                pred[2][0] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvx : 0;
                pred[2][1] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvy : 0;
            }
            else
            {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }
        best_match = UINT_MAX;

        // get the index of the raster 4x4s for the block
        block_index = (vector_count == 0 ? 0 : 8);
        // predict motion vector for current 8x8
        //MotionEstimationGetMotionVectorPrediction(control, block_index, vector_count, H264_16X8, array_position, predX, predY);

        GeneratePredictedMotionVectors(block_index, vector_count, H264_16X8, predX, predY);
        // save predicted MVs
        predP16x8VecX[vector_count]=predX;
        predP16x8VecY[vector_count]=predY;

#ifdef VERIFICATION_DUMP
        // if mpeg4 then want to multiply vectors by 2 to dump in 1/4 pel resolution
        int vecMul = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

        fprintf(vdump_fp, "\n   MVP.4 = (%3d, %3d)\n", predX*vecMul, predY*vecMul);
#endif // VERIFICATION_DUMP


        vecX = vecY = 0;
        current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16) + current_offset;

#ifdef TRY_INT_MVP // ECO 1/5/07 step 1)
    // try the MVP vector as an integer vector
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: MVP Int\n\n");
#endif

   // round to nearest int
        tempX = predX < 0 ? (predX-2)/4 : (predX+2)/4;
        tempY = predY < 0 ? (predY-2)/4 : (predY+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
        previous = refYWthBrdrPels  + previous_y_offset  + previous_offset + (tempY*ref_picture_width) + (tempX);

        match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempX)*4-predX)] +
                            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempY)*4-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }

#endif // TRY_INT_MVP
#ifdef TEST_NEIGHBOURS // ECO 1/5/07 step 2) 3) and 4)

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: A, B and C Int\n\n");
#endif

    for(int neigh = 0; neigh < 3; neigh++){

   // round to nearest int
        tempX = pred[neigh][0] < 0 ? (pred[neigh][0]-2)/4 : (pred[neigh][0]+2)/4;
        tempY = pred[neigh][1] < 0 ? (pred[neigh][1]-2)/4 : (pred[neigh][1]+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);

        previous = refYWthBrdrPels  + previous_y_offset  + previous_offset + (tempY*ref_picture_width) + (tempX);

        match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempX)*4-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length((tempY)*4-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(match < best_match){ // new minimum
            vecX = tempX;
            vecY = tempY;
            best_match = match;
        }
    }// for neigh

#endif // TEST_NEIGHBOURS


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Integer\n\n");
#endif

        // ECO 1/5/07 step 5)
        //  in this section we need to keep a separate set of best vectors
        // and then compare with previous best results at the end of the 4 step search
        stepX = stepY = 0;
        // first calculate (0,0) vector match and set best_match
        previous = refYWthBrdrPels + previous_y_offset + previous_offset;

        step_match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8);

        step_match +=
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", 0, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), step_match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(StepOneControl == 1){ // only do 3 points

            // first try (-8,0)
            previous = refYWthBrdrPels
                        + previous_y_offset + previous_offset + (0*ref_picture_width) - 8 ;

            match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-32-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", -32, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

            if(match < step_match) {
                stepX = -8;
                stepY = 0;
                step_match = match;
            }
            // next try (8,0)
            previous = refYWthBrdrPels
                        + previous_y_offset + previous_offset + (0*ref_picture_width) + 8;

            match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8) +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(32-predX)] +
                mvLambdaTimesCodeLength[h264_encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", 32, 0, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

            if(match < step_match) {
                stepX = 8;
                stepY = 0;
                step_match = match;
            }

        } //    if(StepOneControl == 1)

        // now main loop for integer search

        while (step_size > 0)
        {
            tempX = stepX; tempY = stepY; // in case we don't find a better one

            for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size){ // horizontal displacement

                for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size){ // vertical displacement

                    if(!(horiz_disp|vert_disp))continue; // have already done it

                    previous = refYWthBrdrPels  + previous_y_offset + previous_offset + ((stepY+vert_disp)*ref_picture_width) + (stepX+horiz_disp);

                    match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8);
                    match +=
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((stepX+horiz_disp)*4-predX)] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((stepY+vert_disp)*4-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", (stepX + horiz_disp)<<2, (stepY + vert_disp)<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                    if(match < step_match){ // new minimum
                        tempX = stepX + horiz_disp;
                        tempY = stepY + vert_disp;
                        step_match = match;
                    }

                } //for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            } //for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            stepX = tempX;
            stepY = tempY;


            step_size = step_size >>1; // halve the step size for next iteration
        }// while step size

        if(step_match < best_match) {
            best_match = step_match;
            vecX = stepX;
            vecY = stepY;
        }
// ECO end of step 6)



#ifdef TRY_MVP_NEIGHALL  // ECO 1/5/07 step 7)
    if(auxReg.ME_SETUP__PMV == 1)
    {

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 3x3 MVP Int\n\n");
#endif

        int dist = 1;


       // round to nearest int
    tempX = predX < 0 ? (predX-2)/4 : (predX+2)/4;
        tempY = predY < 0 ? (predY-2)/4 : (predY+2)/4; // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
    for(horiz_disp = tempX-dist; horiz_disp <= tempX+dist; horiz_disp += dist)
        {
#ifdef LIMITS
            if((horiz_disp > 15) || (horiz_disp < -15))continue;
#endif // LIMITS
        for(vert_disp = tempY-dist; vert_disp <= tempY+dist; vert_disp += dist)
            {
#ifdef LIMITS
                if(vert_disp > 15 || vert_disp < -15)continue;
#endif // LIMITS

                previous = refYWthBrdrPels  + previous_y_offset  + previous_offset + (vert_disp*ref_picture_width) + (horiz_disp);

                match = CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8) +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((horiz_disp)*4-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vert_disp)*4-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", horiz_disp<<2, vert_disp<<2, CalculateSOAD(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width,  UINT_MAX, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                if(match < best_match){ // new minimum
                    vecX = horiz_disp;
                    vecY = vert_disp;
                    best_match = match;
                }
            }
        }
    }// if auxreg PMV == 1
#endif // TRY_MVP_NEIGHALL


        vecX = vecX <<2; // put into quarter pel units
        vecY = vecY <<2;



#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Sub-Pel\n\n");
#endif

#ifdef ME_DUMP_SUBPEL
        int save_mv_for_hor = mv_for_hor[0];
        int save_mv_for_ver = mv_for_ver[0];
        MeDumpSubPel8x8(vecX, vecY, vector_count*2, H264_16X8);
        mv_for_hor[0] = save_mv_for_hor;
        mv_for_ver[0] = save_mv_for_ver;
#endif // ME_DUMP_SUBPEL


        // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)

        step_size = SubPelMode > 0 ? 2 : 0; // 0, 2, 2
        for(iter = 0; iter < SubPelMode; iter++, step_size = step_size>>1)
        {

            tempX = vecX; tempY = vecY; // in case we don't find a new minimum
            for(int counter = 0; counter < 8; counter++)
            {
                vert_disp = subpel_order[iter][counter][0]*step_size;

                horiz_disp = subpel_order[iter][counter][1]*step_size;
        //  for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size) // for vertical displ
        //  {
        //      for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)// horiz disp
        //      {
                    if(!(horiz_disp|vert_disp))continue; // have already done it

                    //set the mv arrays (4x4) for the prediction function
                    mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] =
                    mv_for_hor[block_index+2] = mv_for_hor[block_index+3] = mv_for_hor[block_index+6] = mv_for_hor[block_index+7] = vecX + horiz_disp;
                    mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] =
                    mv_for_ver[block_index+2] = mv_for_ver[block_index+3] = mv_for_ver[block_index+6] = mv_for_ver[block_index+7] = vecY + vert_disp;

                    ArcMakeH264InterPrediction8(vector_count*2, H264_16X8); // goes into quadrant

                    match = ArcCalculateSSD(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8) +
                    //match = CalculateSSD16(current, &y_prediction_data [block_offset],(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8) +
                            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecX+horiz_disp)-predX)] +
                            mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecY+vert_disp)-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", (vecX + horiz_disp)*vecMul, (vecY + vert_disp)*vecMul, ArcCalculateSSD(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8), match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

                    if(match < best_match){ // new minimum
                        tempX = vecX + horiz_disp;
                        tempY = vecY + vert_disp;
                        best_match = match;
                    }

            //  }//for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            }//for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            vecX = tempX;
            vecY = tempY;
        }
#ifdef SKIP_VEC_8
        // now try the 16x16 skip vector - we have already calculated the SAD

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 16x16 Skip MV\n\n");
#endif

        match = skippedSAD_8[vector_count*2] + skippedSAD_8[vector_count*2+1] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((PSliceSkippedHor)-predX)] +
                        mvLambdaTimesCodeLength[h264_encoder_vector_code_length((PSliceSkippedVer)-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD*2) = 0x%05X\n", PSliceSkippedHor*vecMul, PSliceSkippedVer*vecMul, skippedSAD_8[vector_count*2] + skippedSAD_8[vector_count*2+1], match*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif

        if(best_match >= match) // highest priority
        {
            best_match = match;
            vecX = PSliceSkippedHor;
            vecY = PSliceSkippedVer;
        }
        // end of skip trial
#endif // SKIP_VEC_8

        bestCost[1][vector_count] = best_match;
        bestBitCost[1][vector_count] =
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecX)-predX)] +
                    mvLambdaTimesCodeLength[h264_encoder_vector_code_length((vecY)-predY)];


        bestHor[1][vector_count] = vecX;
        bestVer[1][vector_count] = vecY;

        // set the MV arrays so that the next block's MV prediction will be correct
        mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] =
        mv_for_hor[block_index+2] = mv_for_hor[block_index+3] = mv_for_hor[block_index+6] = mv_for_hor[block_index+7] = vecX ;
        mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] =
        mv_for_ver[block_index+2] = mv_for_ver[block_index+3] = mv_for_ver[block_index+6] = mv_for_ver[block_index+7] = vecY ;


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n--------------------\n");
        fprintf(vdump_fp, "16x8 Result - Partition %d:\n", vector_count);
        fprintf(vdump_fp, "   MV.25 = (%3d, %3d)\n", bestHor[1][vector_count]*vecMul, bestVer[1][vector_count]*vecMul);
        fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD/2) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[1][vector_count] - bestBitCost[1][vector_count]),
                bestBitCost[1][vector_count],
                (3 * auxReg.ME_LAMBDA)/2,
                bestCost[1][vector_count] + (3 * auxReg.ME_LAMBDA)/2);
        fprintf(vdump_fp, "   Cost (SAD*4 + mvRD*4 + modeRD*2) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[1][vector_count] - bestBitCost[1][vector_count])*4,
                bestBitCost[1][vector_count]*4,
                (3 * auxReg.ME_LAMBDA)*2,
                bestCost[1][vector_count]*4 + (3 * auxReg.ME_LAMBDA)*2);
#endif // VERIFICATION_DUMP

    }

}

// Fetch & calc cost for 8x8 at skip motion vector
// Inputs: predected motion vector x & Y
// Outputs best motion vector cost & fetched pixels
void ArcMotionEstimationModel::StepSearch8x8MB(void)
{


    int step_size;                          // current step size in iteration (8,4,2,1)
    int tempX, tempY;                       // temporary best offset in the iteration
    unsigned int best_match = UINT_MAX;     // lowest cost achieved so far
    int horiz_disp, vert_disp;              // indexes for the loops
    unsigned int match;                     // temporary cost
    int iter;                               // iteration counter

    int predX, predY;
    int vecX, vecY;
    int StepOneControl = auxReg.ME_CONTROL1__STEP_ONE_CTRL;
    int SubPelMode = auxReg.ME_CONTROL1__SUB_PEL_CTRL;
    int ref_picture_width = (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;

    int maxvecX, maxvecY, minvecX, minvecY;
    int minvecsubX, minvecsubY, maxvecsubX, maxvecsubY;
    unsigned char * current;
    unsigned char * previous;
    int current_offset, previous_offset, block_offset;

    int block_height, block_index;
    int vec_shift = 2; // units of H264
    unsigned int step_match ;
    int stepX, stepY;

    // setup MB offset in previous frame
    unsigned int previous_y_offset = (((16 * auxReg.ME_MB_INDEX__MB_Y_INDEX) + NUMBER_OF_TOP_BORDER_PELS) * ref_picture_width)
                        + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;
#ifdef TEST_NEIGHBOURS
    int pred[3][2];
#endif // TEST_NEIGHBOURS

    partition_mode[0] = partition_mode[1] = partition_mode[2] = partition_mode[3] = H264_8X8_PARTITION; // no subdivision of 8x8 blocks
    block_height = 8;
        // 0 = full step search
        // 1 = 3 points in stage 1 : (-8,0), (0,0) and (0,8)
        // 2 = skip first step
    if (auxReg.ME_SETUP__CODEC != H264)
    {
        vec_shift = 1;
    }

    // this for H264

    // for each 8x8 block
    for (int vector_count=0; vector_count<4; vector_count++)
    {

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------------------------------------\n");
        fprintf(vdump_fp, "Search: 8x8\n");
        fprintf(vdump_fp, "Partition: %d\n", vector_count);
#endif
        maxvecX = maxvecY = 15;
        minvecX = minvecY = -15;
        minvecsubX = minvecsubY = (auxReg.ME_SETUP__CODEC != H264) ? -31 : -63;
        maxvecsubX = maxvecsubY = (auxReg.ME_SETUP__CODEC != H264) ? 31 : 63;
        step_size = StepOneControl != 0 ? 4 : 8;
        // first set up the offsets into current frame, reference frame, 16x16 prediction data and array of motion vectors
        if (1 == vector_count)
        {
            current_offset = 8; // sub block offset into current frame
            previous_offset = 8;// subblock offset into previous frame
            block_offset = 8; // subblock offset into motion compensated 16x16 array
            if ((auxReg.ME_SETUP__CODEC != H264) && (auxReg.ME_SETUP__PAD_CTRL == 0)) // restrict the vectors
            {
                minvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ? -8 : -15;
                minvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  0 : -15;
                maxvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 15;
                maxvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 8 : 15;
                minvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ? -16 : -31;
                minvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  0 : -31;
                maxvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 31;
                maxvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 16 : 31;

            }
#ifdef TEST_NEIGHBOURS
            pred[0][0] = mv_for_hor[1]; pred[0][1] = mv_for_ver[1];
            if(aboveMB.validID && !aboveMB.intra)
            {
                pred[1][0] =  aboveMB.mv[3].mvx ;  pred[1][1] = aboveMB.mv[3].mvy;
            }
            else
            {
                pred[1][0] = pred[1][1] = 0;
            }
            if(aboveRightMB.validID)
            {
                pred[2][0] = !aboveRightMB.intra ? aboveRightMB.mv[2].mvx : 0;
                pred[2][1] = !aboveRightMB.intra ? aboveRightMB.mv[2].mvy : 0;
            }
            else if(aboveMB.validID && !aboveMB.intra)
            {
                pred[2][0] =  aboveMB.mv[2].mvx;
                pred[2][1] =  aboveMB.mv[2].mvy;
            }
            else {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }
        else if (2 == vector_count)
        {
            current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);
            previous_offset = 8*ref_picture_width;
            block_offset = 128;
            if ((auxReg.ME_SETUP__CODEC != H264) && (auxReg.ME_SETUP__PAD_CTRL == 0)) // restrict the vectors
            {
                minvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?   0 : -15;
                minvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  -8 : -15;
                maxvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 8 : 15;
                maxvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 15;
                minvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?   0 : -31;
                minvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  -16 : -31;
                maxvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 16 : 31;
                maxvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 31;

            }
#ifdef TEST_NEIGHBOURS
            if(leftMB.validID && !leftMB.intra)
            {
                pred[0][0] =  leftMB.mv[3].mvx ;  pred[0][1] = leftMB.mv[3].mvy;
            }
            else
            {
                pred[0][0] = pred[0][1] = 0;
            }
            pred[1][0] = mv_for_hor[4]; pred[1][1] = mv_for_ver[4];
            pred[2][0] = mv_for_hor[6]; pred[2][1] = mv_for_ver[6];
#endif // TEST_NEIGHBOURS
        }
        else if (3 == vector_count)
        {
            current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT) + 8;
            previous_offset = 8*ref_picture_width + 8;
            block_offset = 136;
            if ((auxReg.ME_SETUP__CODEC != H264) && (auxReg.ME_SETUP__PAD_CTRL == 0)) // restrict the vectors
            {
                minvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?  -8 : -15;
                minvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  -8 : -15;
                maxvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 15;
                maxvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 15;
                minvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?  -16 : -31;
                minvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  -16 : -31;
                maxvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 0 : 31;
                maxvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 0 : 31;

            }
#ifdef TEST_NEIGHBOURS
            pred[0][0] = mv_for_hor[9]; pred[0][1] = mv_for_ver[9];
            pred[1][0] = mv_for_hor[6]; pred[1][1] = mv_for_ver[6];
            pred[2][0] = mv_for_hor[5]; pred[2][1] = mv_for_ver[5];
#endif // TEST_NEIGHBOURS
        }
        else  // 0
        {
            current_offset = 0;
            previous_offset = 0;
            block_offset = 0;
            if ((auxReg.ME_SETUP__CODEC != H264) && (auxReg.ME_SETUP__PAD_CTRL == 0)) // restrict the vectors
            {
                minvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?  0 : -15;
                minvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  0 : -15;
                maxvecX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 8 : 15;
                maxvecY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 8 : 15;
                minvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == 0) ?  0 : -31;
                minvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == 0) ?  0 : -31;
                maxvecsubX = (auxReg.ME_MB_INDEX__MB_X_INDEX == (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION - 1)) ? 16 : 31;
                maxvecsubY = (auxReg.ME_MB_INDEX__MB_Y_INDEX == (auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION - 1)) ? 16 : 31;

            }
#ifdef TEST_NEIGHBOURS
            pred[0][0] = (leftMB.validID && !leftMB.intra)?  leftMB.mv[1].mvx : 0; pred[0][1] = (leftMB.validID && !leftMB.intra) ? leftMB.mv[1].mvy: 0;
            pred[1][0] = (aboveMB.validID && !aboveMB.intra)? aboveMB.mv[2].mvx : 0; pred[1][1] = (aboveMB.validID && !aboveMB.intra) ? aboveMB.mv[2].mvy: 0;
            if(aboveMB.validID)
            {
                pred[2][0] = (!aboveMB.intra) ? aboveMB.mv[3].mvx : 0;
                pred[2][1] = (!aboveMB.intra) ? aboveMB.mv[3].mvy : 0;
            }
            else if(aboveLeftMB.validID)
            {
                pred[2][0] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvx : 0;
                pred[2][1] = (!aboveLeftMB.intra) ? aboveLeftMB.mv[3].mvy : 0;
            }
            else
            {
                pred[2][0] =  pred[2][1] = 0;
            }
#endif // TEST_NEIGHBOURS
        }


        best_match = UINT_MAX;
        // get the index of the raster 4x4s for the block
        block_index = h264_8x8_block_index_to_raster_index_table[vector_count];
        // predict motion vector for current 8x8
        GeneratePredictedMotionVectors(block_index, vector_count, H264_8X8, predX, predY);

        // save predicted MVs
        predP8x8VecX[vector_count]=predX;
        predP8x8VecY[vector_count]=predY;

        if((auxReg.ME_SETUP__EMV) && (auxReg.ME_SETUP__CODEC != H264))
        { // restricted to be within (-16, 15.5) of predictor
              // nb this only relevant to MPEG4 as
              // H263 baseline does not do 8x8 search
              // H263+ does, but with AnnexD on, then everything changes and it won't be supported
            minvecX = MAX(minvecX, (-32 + predX)/2); // nb pred is in 1/2 pel units, but these values are ints
            minvecY = MAX(minvecY, (-32 + predY)/2);
            maxvecX = MIN(maxvecX, ( 31 + predX)/2);
            maxvecY = MIN(maxvecY, ( 31 + predY)/2);
            minvecsubX = MAX(minvecsubX, (-32 + predX));
            minvecsubY = MAX(minvecsubY, (-32 + predY));
            maxvecsubX = MIN(maxvecsubX, ( 31 + predX));
            maxvecsubY = MIN(maxvecsubY, ( 31 + predY));
        }
#ifdef VERIFICATION_DUMP
        // if mpeg4 then want to multiply vectors by 2 to dump in 1/4 pel resolution
        int vecMul = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

        fprintf(vdump_fp, "\n   MVP.4 = (%3d, %3d)\n", predX*vecMul, predY*vecMul);
#endif // VERIFICATION_DUMP

        // first calculate (0,0) vector match and set best_match
        vecX = vecY = 0;

        current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16) + current_offset;
        previous = refYWthBrdrPels + previous_y_offset + previous_offset;

#ifdef TRY_INT_MVP // ECO 1/5/07 step 1)

        // try the MVP vector as an integer vector
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: MVP Int\n\n");
#endif

      // round to nearest int
        tempX = predX < 0 ? -((-predX+vec_shift)>>vec_shift) : ((predX+vec_shift)>>vec_shift);
        tempY = predY < 0 ? -((-predY+vec_shift)>>vec_shift) : ((predY+vec_shift)>>vec_shift); // put into integer units
        tempX = CLIP_VEC(tempX);
        tempY = CLIP_VEC(tempY);
        if(!NOT_IN_RANGE_INT(tempX, tempY, minvecX, minvecY, maxvecX, maxvecY))
        {
            previous = refYWthBrdrPels
                                + previous_y_offset  + previous_offset + (tempY*ref_picture_width) + (tempX);

            match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                    mvLambdaTimesCodeLength[encoder_vector_code_length(((tempX)<<vec_shift)-predX)] +
                                mvLambdaTimesCodeLength[encoder_vector_code_length(((tempY)<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

            if(match < best_match){ // new minimum
                vecX = tempX;
                vecY = tempY;
                best_match = match;
            }
        }
#endif // TRY_INT_MVP

#ifdef TEST_NEIGHBOURS // ECO 1/5/07 step 2) 3) and 4)

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: A, B and C Int\n\n");
#endif

        for(int neigh = 0; neigh < 3; neigh++){

         // round towards nearest int
            tempX = pred[neigh][0] < 0 ? -((-pred[neigh][0]+vec_shift)>>vec_shift) : ((pred[neigh][0]+vec_shift)>>vec_shift);
            tempY = pred[neigh][1] < 0 ? -((-pred[neigh][1]+vec_shift)>>vec_shift) : ((pred[neigh][1]+vec_shift)>>vec_shift); // put into integer units
            tempX = CLIP_VEC(tempX);
            tempY = CLIP_VEC(tempY);
            if(NOT_IN_RANGE_INT(tempX, tempY, minvecX, minvecY, maxvecX, maxvecY))continue;

            previous = refYWthBrdrPels
                                + previous_y_offset  + previous_offset + (tempY*ref_picture_width) + (tempX);


            match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                    mvLambdaTimesCodeLength[encoder_vector_code_length((tempX<<vec_shift)-predX)] +
                                mvLambdaTimesCodeLength[encoder_vector_code_length((tempY<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", tempX<<2, tempY<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

            if(match < best_match){ // new minimum
                vecX = tempX;
                vecY = tempY;
                best_match = match;
            }
        }// for neigh

#endif // TEST_NEIGHBOURS


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Integer\n\n");
#endif

// ECO 1/5/07 step 5)
    //  in this section we need to keep a separate set of best vectors
    // and then compare with previous best results at the end of the 4 step search
        stepX = stepY = 0;
    // try (0,0) vector
        previous = refYWthBrdrPels
                                + previous_y_offset  + previous_offset;
        step_match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX);
        step_match +=
               mvLambdaTimesCodeLength[encoder_vector_code_length(-predX)] +
               mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];

        if(step_match < best_match) {
            best_match = step_match;
            vecX = stepX;
            vecY = stepY;
        }
// ECO end of step 5)
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", 0, 0, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), step_match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif
// ECO 1/5/07 step 6)
        if(StepOneControl == 1){ // only do 3 points

            // first try (-8,0)

            if(!(NOT_IN_RANGE_INT(-8, 0, minvecX, minvecY, maxvecX, maxvecY)))
            {
                previous = refYWthBrdrPels
                        + previous_y_offset + previous_offset + (0*ref_picture_width) - 8 ;

                match = CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-(8<<vec_shift)-predX)] +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", -32, 0, CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif
                if(match < step_match) {
                    stepX = -8;
                    stepY = 0;
                    step_match = match;
                }
            }
            // next try (8,0)
            if(!(NOT_IN_RANGE_INT(8, 0, minvecX, minvecY, maxvecX, maxvecY)))
            {
                previous = refYWthBrdrPels
                            + previous_y_offset + previous_offset + (0*ref_picture_width) + 8;

                match = CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                mvLambdaTimesCodeLength[encoder_vector_code_length((8<<vec_shift)-predX)] +
                mvLambdaTimesCodeLength[encoder_vector_code_length(-predY)];

#ifdef VERIFICATION_DUMP
             fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", 32, 0, CalculateSOADWidth8(current, previous,  (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

                if(match < step_match) {
                    stepX = 8;
                    stepY = 0;
                    step_match = match;
                }
            }

        } //    if(StepOneControl == 1)

        // now main loop for integer search
        while (step_size > 0){ // for each step

            tempX = stepX; tempY = stepY; // in case we don't find a better one
            for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            { // horizontal displacement
                for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
                { // vertical displacement

                    if(!(horiz_disp|vert_disp))continue; // have already done it

                    if(NOT_IN_RANGE_INT(stepX+horiz_disp, stepY+vert_disp, minvecX, minvecY, maxvecX, maxvecY))continue;

                    previous = refYWthBrdrPels  + previous_y_offset + previous_offset + ((stepY+vert_disp)*ref_picture_width) + (stepX+horiz_disp);

                    match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX);
                    match +=
                      mvLambdaTimesCodeLength[encoder_vector_code_length(((stepX+horiz_disp)<<vec_shift)-predX)] +
                      mvLambdaTimesCodeLength[encoder_vector_code_length(((stepY+vert_disp)<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", (stepX + horiz_disp)<<2, (stepY + vert_disp)<<2, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

                    if(match < step_match){ // new minimum
                        tempX = stepX + horiz_disp;
                        tempY = stepY + vert_disp;
                        step_match = match;
                    }

                } //for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            } //for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            stepX = tempX;
            stepY = tempY;
            step_size = step_size >>1; // halve the step size for next iteration
        }//while (step_size > 0)

        if(step_match < best_match) {
            best_match = step_match;
            vecX = stepX;
            vecY = stepY;
        }
     // ECO end of step 6)

#ifdef TRY_MVP_NEIGHALL  // ECO 1/5/07 step 7)
        if(auxReg.ME_SETUP__PMV == 1)
        {
#ifdef VERIFICATION_DUMP
            fprintf(vdump_fp, "\n----------\n");
            fprintf(vdump_fp, "Points: 3x3 MVP Int\n\n");
#endif
            int dist = 1;

          // round away from zero
        tempX = predX < 0 ? -((-predX+vec_shift)>>vec_shift) : ((predX+vec_shift)>>vec_shift);
            tempY = predY < 0 ? -((-predY+vec_shift)>>vec_shift) : ((predY+vec_shift)>>vec_shift); // put into integer units
            tempX = CLIP_VEC(tempX);
            tempY = CLIP_VEC(tempY);
        for(horiz_disp = tempX-dist; horiz_disp <= tempX+dist; horiz_disp += dist)
            {
#ifdef LIMITS
            //  if((horiz_disp > 15) || (horiz_disp < -15))continue;
#endif // LIMITS
            for(vert_disp = tempY-dist; vert_disp <= tempY+dist; vert_disp += dist)
                {
#ifdef LIMITS
            //      if(vert_disp > 15 || vert_disp < -15)continue;
#endif // LIMITS
                    if(NOT_IN_RANGE_INT(horiz_disp, vert_disp, minvecX, minvecY, maxvecX, maxvecY))continue;

                    previous = refYWthBrdrPels
                                        + previous_y_offset  + previous_offset + (vert_disp*ref_picture_width) + (horiz_disp);

                    match = CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX) +
                        mvLambdaTimesCodeLength[encoder_vector_code_length((horiz_disp<<vec_shift)-predX)] +
                                    mvLambdaTimesCodeLength[encoder_vector_code_length((vert_disp<<vec_shift)-predY)];

#ifdef VERIFICATION_DUMP
                fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", horiz_disp<<vec_shift, vert_disp<<vec_shift, CalculateSOADWidth8(current, previous, (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), ref_picture_width, block_height, UINT_MAX), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

                    if(match < best_match){ // new minimum
                        vecX = horiz_disp;
                        vecY = vert_disp;
                        best_match = match;
                    }
                }
            }
        }// if auxreg PMV == 1
#endif // TRY_MVP_NEIGHALL






      if (auxReg.ME_SETUP__CODEC == H264)
      {
        vecX = vecX <<2; // put into quarter pel units
        vecY = vecY <<2;
        // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)
         step_size = SubPelMode > 0 ? 2 : 0; // 0, 2, 2
      }
      else
      {
         vecX = vecX <<1; // put into half pel units
         vecY = vecY <<1;
         // now sub-pel stuff  SubPelMode = 0 (none) 1 (1/2 pel only) 2 (1/2 and 1/4)
         step_size = SubPelMode > 1 ? 2 : 1; // 1, 1, 2
      }

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: Sub-Pel\n\n");
#endif

#ifdef ME_DUMP_SUBPEL
        int save_mv_for_hor = mv_for_hor[0];
        int save_mv_for_ver = mv_for_ver[0];
        MeDumpSubPel8x8(vecX, vecY, vector_count, H264_8X8);
        mv_for_hor[0] = save_mv_for_hor;
        mv_for_ver[0] = save_mv_for_ver;
#endif // ME_DUMP_SUBPEL


        for(iter = 0; iter < SubPelMode; iter++, step_size = step_size>>1)
        {

            tempX = vecX; tempY = vecY; // in case we don't find a new minimum
            for(int counter = 0; counter < 8; counter++)
            {
                vert_disp = subpel_order[iter][counter][0]*step_size;

                horiz_disp = subpel_order[iter][counter][1]*step_size;
    //      for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size) // for vertical displ
    //      {
    //          for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)// horiz disp
    //          {
    //              if(!(horiz_disp|vert_disp))continue; // have already done it
                if(NOT_IN_RANGE_INT(horiz_disp+vecX,vert_disp+vecY,minvecsubX, minvecsubY, maxvecsubX, maxvecsubY))continue;

                    //set the mv arrays (4x4) for the prediction function
                mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] = vecX + horiz_disp;
                mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] = vecY + vert_disp;

               switch(auxReg.ME_SETUP__CODEC)
               {
                  case H264:
                     ArcMakeH264InterPrediction8(vector_count,H264_8X8); // goes into quadrant of
                     break;
                  case MPEG4:
                  case H263:
                     ArcMakeH263InterPrediction8(vector_count);
                     break;
                  default:
                     fprintf(stderr, "ArcMotionEstimationModel::StepSearch8x8MB - Error: Unsupported Codec\n");
                     assert(false);
                     break;
               }
                    // and it predicts the entire 16x16 MB - we only need an 8x8 quadrant
                    match = CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8) +
                     mvLambdaTimesCodeLength[encoder_vector_code_length((vecX+horiz_disp)-predX)] +
                     mvLambdaTimesCodeLength[encoder_vector_code_length((vecY+vert_disp)-predY)];

#ifdef VERIFICATION_DUMP
                    fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", (vecX + horiz_disp)*vecMul, (vecY + vert_disp)*vecMul, CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8), match*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

                    if(match < best_match){ // new minimum
                        tempX = vecX + horiz_disp;
                        tempY = vecY + vert_disp;
                        best_match = match;
                    }

            //  }//for(horiz_disp = -step_size; horiz_disp <= step_size; horiz_disp += step_size)
            }//for(vert_disp = -step_size; vert_disp <= step_size; vert_disp += step_size)
            vecX = tempX;
            vecY = tempY;
        }

#ifdef SKIP_VEC_8
        // now try the 16x16 skip vector - we have already calculated the SAD

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n----------\n");
        fprintf(vdump_fp, "Points: 16x16 Skip MV\n\n");
#endif

        unsigned int temp = skippedSAD_8[vector_count] +
                        mvLambdaTimesCodeLength[encoder_vector_code_length((PSliceSkippedHor)-predX)] +
                        mvLambdaTimesCodeLength[encoder_vector_code_length((PSliceSkippedVer)-predY)];


#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "MV = (%3d, %3d) : SAD = 0x%04X : Cost (SAD*4 + mvRD*4 + modeRD) = 0x%05X\n", PSliceSkippedHor*vecMul, PSliceSkippedVer*vecMul, skippedSAD_8[vector_count], temp*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif

        if(best_match >= temp) // highest priority
        {
            best_match = temp;
            vecX = PSliceSkippedHor;
            vecY = PSliceSkippedVer;
        }
        // end of skip trial
#endif // SKIP_VEC_8

        bestCost[3][vector_count] = best_match;
        bestBitCost[3][vector_count] =
               mvLambdaTimesCodeLength[encoder_vector_code_length((vecX)-predX)] +
               mvLambdaTimesCodeLength[encoder_vector_code_length((vecY)-predY)];


        bestHor[3][vector_count] = vecX;
        bestVer[3][vector_count] = vecY;
        // set the MV arrays so that the next block's MV prediction will be correct
        mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] = vecX ;
        mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] = vecY ;

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "\n--------------------\n");
        fprintf(vdump_fp, "8x8 Result - Partition %d:\n", vector_count);
        fprintf(vdump_fp, "   MV.25 = (%3d, %3d)\n", bestHor[3][vector_count]*vecMul, bestVer[3][vector_count]*vecMul);
        fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD/4) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[3][vector_count] - bestBitCost[3][vector_count]),
                bestBitCost[3][vector_count],
                (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA)/4,
                bestCost[3][vector_count] + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA)/4);
        fprintf(vdump_fp, "   Cost (SAD*4 + mvRD*4 + modeRD) = (0x%X, 0x%X, 0x%X) = 0x%X\n",
                (bestCost[3][vector_count] - bestBitCost[3][vector_count])*4,
                bestBitCost[3][vector_count]*4,
                (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA),
                bestCost[3][vector_count]*4 + (((auxReg.ME_SETUP__CODEC==H264) ? 9 : 3) * auxReg.ME_LAMBDA));
#endif // VERIFICATION_DUMP

    }// vector_count

}


// candidate selection
void ArcMotionEstimationModel::SetTotalModeCost(bool valid_16x16, bool valid_16x8, bool valid_8x16, bool valid_8x8)
{
    unsigned int sad_tot = skippedSAD_8[0] + skippedSAD_8[1] + skippedSAD_8[2] + skippedSAD_8[3];

    unsigned int temp = valid_8x8? bestCost[3][0] + bestCost[3][1] + bestCost[3][2] + bestCost[3][3] + 3 * auxReg.ME_LAMBDA : UINT_MAX;


    ////////////////////////////////////////////////////////////////////////////
    // candidate selection priority below is in revserse order (i.e. lowest first)

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------------------------------------\n");
    fprintf(vdump_fp, "Candidate Selection:\n\n");

    float vecDiv = (auxReg.ME_SETUP__CODEC == H264) ? 4.0f : 2.0f;
#endif

    ////////////////////////////////////////////////////////////////////////////
    // 8x8 candidate
    if(valid_8x8 && (auxReg.ME_SETUP__CODEC == H264))
    {
        temp += (auxReg.ME_LAMBDA * 6); // to make up to 9
    }
    {
      best_inter_mtype = (auxReg.ME_SETUP__CODEC == H264) ? MBCT_H264_INTER8X8 :
                                                                      MBCT_MPEG4_INTER8X8;
      best_inter_cost = temp;
      //arcRetAddr = auxReg.ME_BASE_BR_ADDR | auxReg.ME_8x8_BR_ADDR;
      arcRetAddr = auxReg.ME_8x8_BR_ADDR << ME_8x8_BR_ADDR_SHIFT;
      auxReg.ME_RESULT__CANDIDATE_TYPE = 2;
#ifdef ME_DUMP_CT
      fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
      auxReg.ME_RESULT__SKIP = 0;
    }

#ifdef VERIFICATION_DUMP
    if (valid_8x8)
    {
        fprintf(vdump_fp, "8x8:\n");
        fprintf(vdump_fp, "   MV0 = (%.2f, %.2f)\n", bestHor[3][0]/vecDiv, bestVer[3][0]/vecDiv);
        fprintf(vdump_fp, "   MV1 = (%.2f, %.2f)\n", bestHor[3][1]/vecDiv, bestVer[3][1]/vecDiv);
        fprintf(vdump_fp, "   MV2 = (%.2f, %.2f)\n", bestHor[3][2]/vecDiv, bestVer[3][2]/vecDiv);
        fprintf(vdump_fp, "   MV3 = (%.2f, %.2f)\n", bestHor[3][3]/vecDiv, bestVer[3][3]/vecDiv);
        fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD) = 0x%X\n\n", temp);
    }
#endif

    if (auxReg.ME_SETUP__CODEC==H264)
    {
        ////////////////////////////////////////////////////////////////////////////
        // 8x16 candidate
        if(valid_8x16 && (bestCost[2][0] + bestCost[2][1] + 3*auxReg.ME_LAMBDA <= best_inter_cost))
        {
            best_inter_mtype = MBCT_H264_INTER8X16;
            best_inter_cost = (bestCost[2][0] + bestCost[2][1] + 3*auxReg.ME_LAMBDA );
          //arcRetAddr = auxReg.ME_BASE_BR_ADDR | auxReg.ME_8x16_BR_ADDR;
          arcRetAddr = auxReg.ME_8x16_BR_ADDR << ME_8x16_BR_ADDR_SHIFT;

          auxReg.ME_RESULT__CANDIDATE_TYPE = 4;
#ifdef ME_DUMP_CT
          fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
          auxReg.ME_RESULT__SKIP = 0;
        }

#ifdef VERIFICATION_DUMP
        if (valid_8x16)
        {
            fprintf(vdump_fp, "8x16:\n");
            fprintf(vdump_fp, "   MV0 = (%.2f, %.2f)\n", bestHor[2][0]/vecDiv, bestVer[2][0]/vecDiv);
            fprintf(vdump_fp, "   MV1 = (%.2f, %.2f)\n", bestHor[2][1]/vecDiv, bestVer[2][1]/vecDiv);
            fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD) = 0x%X\n\n", (bestCost[2][0] + bestCost[2][1] + 3*auxReg.ME_LAMBDA));
        }
#endif

        ////////////////////////////////////////////////////////////////////////////
        // 16x8 candidate
        if(valid_16x8 && (bestCost[1][0] + bestCost[1][1] + 3*auxReg.ME_LAMBDA <= best_inter_cost))
        {
            best_inter_mtype = MBCT_H264_INTER16X8;
            best_inter_cost = (bestCost[1][0] + bestCost[1][1] + 3*auxReg.ME_LAMBDA );
          arcRetAddr = auxReg.ME_16x8_BR_ADDR << ME_16x8_BR_ADDR_SHIFT;
        //  arcRetAddr = auxReg.ME_BASE_BR_ADDR | auxReg.ME_16x8_BR_ADDR;
          auxReg.ME_RESULT__CANDIDATE_TYPE = 3;
#ifdef ME_DUMP_CT
          fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
          auxReg.ME_RESULT__SKIP = 0;
        }

#ifdef VERIFICATION_DUMP
        if (valid_16x8)
        {
            fprintf(vdump_fp, "16x8:\n");
            fprintf(vdump_fp, "   MV0 = (%.2f, %.2f)\n", bestHor[1][0]/vecDiv, bestVer[1][0]/vecDiv);
            fprintf(vdump_fp, "   MV1 = (%.2f, %.2f)\n", bestHor[1][1]/vecDiv, bestVer[1][1]/vecDiv);
            fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD) = 0x%X\n\n", (bestCost[1][0] + bestCost[1][1] + 3*auxReg.ME_LAMBDA));
        }
#endif

    }

    ////////////////////////////////////////////////////////////////////////////
    // 16x16 candidate
    if(valid_16x16 && ( bestCost[0][0] + auxReg.ME_LAMBDA <= best_inter_cost))
    {
      best_inter_mtype = (auxReg.ME_SETUP__CODEC == H264) ? MBCT_H264_INTER16X16 :
                                                            MBCT_MPEG4_INTER16X16;
        best_inter_cost = bestCost[0][0] + auxReg.ME_LAMBDA;
      arcRetAddr = auxReg.ME_16x16_BR_ADDR << ME_16x16_BR_ADDR_SHIFT;
      auxReg.ME_RESULT__CANDIDATE_TYPE = 1;
#ifdef ME_DUMP_CT
      fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
      auxReg.ME_RESULT__SKIP = 0;
    }

#ifdef VERIFICATION_DUMP
    if (valid_16x16)
    {
        fprintf(vdump_fp, "16x16:\n");
        fprintf(vdump_fp, "   MV = (%.2f, %.2f)\n", bestHor[0][0]/vecDiv, bestVer[0][0]/vecDiv);
        fprintf(vdump_fp, "   Cost (SAD + mvRD + modeRD) = 0x%X\n\n", bestCost[0][0] + auxReg.ME_LAMBDA);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////
    // Skip (No other valid modes)
    if(!valid_16x16 && !valid_16x8 && !valid_8x16 && !valid_8x8) // has to skip
    {
        best_inter_cost = sad_tot;
      best_inter_mtype = (auxReg.ME_SETUP__CODEC == H264) ? MBCT_H264_SKIPPED :
                                                            MBCT_MPEG4_SKIPPED;
      arcRetAddr = auxReg.ME_SKIP_BR_ADDR << ME_SKIP_BR_ADDR_SHIFT;
      auxReg.ME_RESULT__CANDIDATE_TYPE = 0;
#ifdef ME_DUMP_CT
      fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
      auxReg.ME_RESULT__SKIP = 1;

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "Skipped (No other valid modes):\n");
        fprintf(vdump_fp, "   MV = (%.2f, %.2f)\n", PSliceSkippedHor/vecDiv, PSliceSkippedVer/vecDiv);
        fprintf(vdump_fp, "   Cost (SAD) = 0x%X\n\n", sad_tot);
#endif
    }


    // this test not logical (in that one side includes chroma, but not the other), but it gets the best results
//  if(control->best_inter_cost > m_SkippedModeSSD/* m_SkippedSAD_8[0]+m_SkippedSAD_8[1]+m_SkippedSAD_8[2]+m_SkippedSAD_8[3]*/)

    unsigned int ThresholdSkip = auxReg.ME_THRESHOLD_SKIP_MAX;//(5*auxReg.ME_THRESHOLD_SKIP_MAX)>>2;

    if( (SkippedSAD_4[0] < ThresholdSkip) &&
        (SkippedSAD_4[1] < ThresholdSkip) &&
        (SkippedSAD_4[2] < ThresholdSkip) &&
        (SkippedSAD_4[3] < ThresholdSkip) &&
        (SkippedSAD_4[4] < ThresholdSkip) &&
        (SkippedSAD_4[5] < ThresholdSkip) &&
        (SkippedSAD_4[6] < ThresholdSkip) &&
        (SkippedSAD_4[7] < ThresholdSkip) &&
        (SkippedSAD_4[8] < ThresholdSkip) &&
        (SkippedSAD_4[9] < ThresholdSkip) &&
        (SkippedSAD_4[10] < ThresholdSkip) &&
        (SkippedSAD_4[11] < ThresholdSkip) &&
        (SkippedSAD_4[12] < ThresholdSkip) &&
        (SkippedSAD_4[13] < ThresholdSkip) &&
        (SkippedSAD_4[14] < ThresholdSkip) &&
        (SkippedSAD_4[15] < ThresholdSkip)
        )
   {
       // ME_SETUP__SKIP_ADJUST (bit 0=1/16th, bit 1=1/8th, bit 2=1/4er)
      unsigned int sad_tot_skip_adj = sad_tot +
                                      ((auxReg.ME_SETUP__SKIP_ADJUST & 1) ? sad_tot >> 4 : 0) +
                                      ((auxReg.ME_SETUP__SKIP_ADJUST & 2) ? sad_tot >> 3 : 0) +
                                      ((auxReg.ME_SETUP__SKIP_ADJUST & 4) ? sad_tot >> 2 : 0);

#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, "Skipped (All 4x4 SADs < ME_THRESHOLD_SKIP_MAX):\n");
        fprintf(vdump_fp, "   MV = (%.2f, %.2f)\n", PSliceSkippedHor/vecDiv, PSliceSkippedVer/vecDiv);
        fprintf(vdump_fp, "   Cost (Skip Adjusted SAD) = 0x%X\n", sad_tot_skip_adj);

        if ( ((auxReg.ME_SETUP__CODEC==H264 && best_inter_mtype == MBCT_H264_INTER16X16) ||
              (auxReg.ME_SETUP__CODEC!=H264 && best_inter_mtype == MBCT_MPEG4_INTER16X16)) &&
             bestHor[0][0]==PSliceSkippedHor && bestVer[0][0]==PSliceSkippedVer)
        {
            fprintf(vdump_fp, "   (Best Mode = 16x16) && (16x16 MV == Skip MV)\n");
        }

        fprintf(vdump_fp, "   Cost (Returned) = 0x%X\n\n", sad_tot);
#endif

        ////////////////////////////////////////////////////////////////////////////
        // Skip (SAD Adjusted or 16x16 MV == Skip MV)
      if( (best_inter_cost >= sad_tot_skip_adj) ||
          (

          ( ((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype == MBCT_H264_INTER16X16)) ||
            ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype == MBCT_MPEG4_INTER16X16)) ) &&
           (bestHor[0][0] == PSliceSkippedHor) && (bestVer[0][0] == PSliceSkippedVer)) )
      {
            best_inter_cost = sad_tot;
         best_inter_mtype = (auxReg.ME_SETUP__CODEC == H264) ? MBCT_H264_SKIPPED : MBCT_MPEG4_SKIPPED;
         arcRetAddr = auxReg.ME_SKIP_BR_ADDR << ME_SKIP_BR_ADDR_SHIFT;

            bestHor[0][0] = PSliceSkippedHor; // in case skip not allowed
            bestVer[0][0] = PSliceSkippedVer; // in case skip not allowed

         auxReg.ME_RESULT__CANDIDATE_TYPE = 0;
#ifdef ME_DUMP_CT
         fprintf(vdump_fp, "ME_RESULT__CANDIDATE_TYPE set to %d\n", auxReg.ME_RESULT__CANDIDATE_TYPE);
#endif // ME_DUMP_CT
         auxReg.ME_RESULT__SKIP = 1;
      }

    }

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\nWinning Result - ");
    if (auxReg.ME_SETUP__CODEC == H264)
    {
        switch (best_inter_mtype)
        {
            case MBCT_H264_INTER16X16:
                fprintf(vdump_fp, "H264_INTER16X16");
                break;
            case MBCT_H264_INTER16X8:
                fprintf(vdump_fp, "H264_INTER16X8");
                break;
            case MBCT_H264_INTER8X16:
                fprintf(vdump_fp, "H264_INTER8X16");
                break;
            case MBCT_H264_INTER8X8:
                fprintf(vdump_fp, "H264_INTER8X8");
                break;
            case MBCT_H264_SKIPPED:
                fprintf(vdump_fp, "H264_SKIPPED");
                break;
            default:
                fprintf(vdump_fp, "ERROR(H264)");
                break;
        }
    }
    else
    {
        switch (best_inter_mtype)
        {
            case MBCT_MPEG4_INTER16X16:
                fprintf(vdump_fp, "MPEG4_INTER16X16");
                break;
            case MBCT_MPEG4_INTER8X8:
                fprintf(vdump_fp, "MPEG4_INTER8X8");
                break;
            case MBCT_MPEG4_SKIPPED:
                fprintf(vdump_fp, "MPEG4_SKIPPED");
                break;
            default:
                fprintf(vdump_fp, "ERROR(MPEG4)");
                break;
        }
    }
#endif


    // saturate cost to 16 bits
    if (best_inter_cost > 0xffff)
    {
#ifdef VERIFICATION_DUMP
        fprintf(vdump_fp, " : Winning Cost = 0xffff (SATURATED - 0x%X)", best_inter_cost);
#endif
        best_inter_cost = 0xffff;
    }
#ifdef VERIFICATION_DUMP
    else
    {
        fprintf(vdump_fp, " : Winning Cost = 0x%X", best_inter_cost);
    }
#endif


   // update aux regs with results
   auxReg.ME_RESULT__COST_SAD=(u_int16) best_inter_cost;

}

// ======================SAD calculations ==============================================

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CalculateSSD                                                */
/* DESCRIPTION: Calculates the sum of square differences between the
                current and the previous.                                   */
/* ------------------------------------------------------------------------ */

unsigned int ArcMotionEstimationModel::CalculateSSD8(unsigned char* current, unsigned char* previous,
                                        unsigned int current_yinc, unsigned int previous_yinc, int height)
{
    unsigned char *cpel = current;
    unsigned char *ppel = previous;
    unsigned int local_ssd = 0;

    int y = 0;

   unsigned char cpel_array[8];
    while (y<height)
    {
      MeMemoryRead(cpel_array, cpel, 8);

        local_ssd += ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3))
        + ABS_TABLE(*(cpel_array +  4) - *(ppel +  4))
        + ABS_TABLE(*(cpel_array +  5) - *(ppel +  5))
        + ABS_TABLE(*(cpel_array +  6) - *(ppel +  6))
        + ABS_TABLE(*(cpel_array +  7) - *(ppel +  7));

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::CalculateSOADWidth8                           */
/* DESCRIPTION: calculates sum of absolute differences between two
                8x8 or 8x16 blocks.
                current and previous are pointers to the top left pel of
                the current and reference picture.
                current_yinc is the number of pels per line in the
                current picture.
                previous_yinc is the number of pels per line in the
                previous picture, which will be different if the previous
                picture has been expanded to have repeated border pels.
                height is the height of the block, and is 8 or 16.
                stop_soad is the soad to beat - the function
                gives up once the calculated soad becomes larger than
                this value. This is not used in the SSE calculation         */
/* ------------------------------------------------------------------------ */

unsigned int ArcMotionEstimationModel::CalculateSOADWidth8(unsigned char* current, unsigned char* previous,
                                  unsigned int current_yinc, unsigned int previous_yinc,
                                  unsigned int height, unsigned int stop_soad)

{
    unsigned char *cpel = current;
    unsigned char *ppel = previous;
    unsigned int local_soad = 0;

    unsigned int y = 0;

   unsigned char cpel_array[8];

    while (y<height)
    {
      MeMemoryRead(cpel_array, cpel, 8);

        local_soad +=
          ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3))
        + ABS_TABLE(*(cpel_array +  4) - *(ppel +  4))
        + ABS_TABLE(*(cpel_array +  5) - *(ppel +  5))
        + ABS_TABLE(*(cpel_array +  6) - *(ppel +  6))
        + ABS_TABLE(*(cpel_array +  7) - *(ppel +  7));

        if (local_soad > stop_soad) break;

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_soad);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CalculateSSD                                                */
/* DESCRIPTION: Calculates the sum of square differences between the
                current and the previous.                                   */
/* ------------------------------------------------------------------------ */

unsigned int ArcMotionEstimationModel::CalculateSSD8(unsigned char* current, int* previous,
                                        unsigned int current_yinc, unsigned int previous_yinc, int height)
{
    unsigned char *cpel = current;
    int *ppel = previous;
    unsigned int local_ssd = 0;

    int y = 0;

   unsigned char cpel_array[8];
    while (y<height)
    {
      MeMemoryRead(cpel_array, cpel, 8);

        local_ssd +=
          ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3))
        + ABS_TABLE(*(cpel_array +  4) - *(ppel +  4))
        + ABS_TABLE(*(cpel_array +  5) - *(ppel +  5))
        + ABS_TABLE(*(cpel_array +  6) - *(ppel +  6))
        + ABS_TABLE(*(cpel_array +  7) - *(ppel +  7));

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::CalculateSOAD                                 */
/* DESCRIPTION: calculates sum of absolute differences between two
                macroblocks.
                current and previous are pointers to the top left pel of
                the current and reference picture.
                current_yinc is the number of pels per line in the
                current picture.
                previous_yinc is the number of pels per line in the
                previous picture, which will be different if the previous
                picture has been expanded to have repeated border pels.
                stop_soad is the soad to beat - the function
                gives up once the calculated soad becomes larger than
                this value. This is not used in the SSE calculation         */
/* ------------------------------------------------------------------------ */
unsigned int ArcMotionEstimationModel::CalculateSOAD(unsigned char* current, unsigned char* previous,
                                  unsigned int current_yinc, unsigned int previous_yinc,
                                  unsigned int stop_soad, int height)

{
    unsigned char *cpel = current;
    unsigned char *ppel = previous;
    unsigned int local_soad = 0;

    int y = 0;

   unsigned char cpel_array[16];
    while (y<height)
    {
      // cpel comes from external memory so need to read it with MeMemoryRead
      MeMemoryRead(cpel_array, cpel, 16);

        local_soad +=
          ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3))
        + ABS_TABLE(*(cpel_array +  4) - *(ppel +  4))
        + ABS_TABLE(*(cpel_array +  5) - *(ppel +  5))
        + ABS_TABLE(*(cpel_array +  6) - *(ppel +  6))
        + ABS_TABLE(*(cpel_array +  7) - *(ppel +  7))
        + ABS_TABLE(*(cpel_array +  8) - *(ppel +  8))
        + ABS_TABLE(*(cpel_array +  9) - *(ppel +  9))
        + ABS_TABLE(*(cpel_array + 10) - *(ppel + 10))
        + ABS_TABLE(*(cpel_array + 11) - *(ppel + 11))
        + ABS_TABLE(*(cpel_array + 12) - *(ppel + 12))
        + ABS_TABLE(*(cpel_array + 13) - *(ppel + 13))
        + ABS_TABLE(*(cpel_array + 14) - *(ppel + 14))
        + ABS_TABLE(*(cpel_array + 15) - *(ppel + 15));

        if (local_soad > stop_soad) break;

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_soad);
}


// ======================END SAD calculations ==============================================



/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcGetNeighbouringLocationNonMBAFF */
/* DESCRIPTION: Copied from BT function CH264Encoder::GetNeighbouringLocationNonMBAFF
            This function performs the function in Table 6-3 of the
                standard, and uses the terminology of that table.
                xN and yN are an offset to a pel relative to the top left
                of the current macroblock, given by current_control. The
                function returns the macroblock containing the pel, given
                by neighbouring_control and the vertical offset, yW, of the
                pel relative to the top left of the macroblock it is within.
                maxWH is the width/height of the macroblock, i.e. 16 for
                luma and 8 for chroma.                                      */
/* RETURNS:     void.                                                       */
/* ------------------------------------------------------------------------ */

void ArcMotionEstimationModel::ArcGetNeighbouringLocationNonMBAFF(int xN, int yN, int maxWH, NeighbourFlagsExpanded_t ** neighbouring_control, int& yW)
{
    *neighbouring_control = 0;  // Unless set otherwise

    if ((xN < 0) && (yN < 0))
    {
        *neighbouring_control = &aboveLeftMB; // above left
    }
    else if ((xN < 0) && (yN < maxWH))
    {
        *neighbouring_control = &leftMB; // left
    }
    else if ((xN < maxWH) && (yN < 0))
    {
        *neighbouring_control = &aboveMB; // above
    }
    else if ((xN < maxWH) && (yN < maxWH))
    {
        *neighbouring_control = &currMB; // current
    }
    else if (yN < 0)
    {
        *neighbouring_control = &aboveRightMB; // above right
    }

    yW = (yN + maxWH) & (maxWH-1);
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcGetH264MotionVectorPrediction  */
/* DESCRIPTION: Copied from BT function CH264Encoder::GetMotionVectorPrediction
            This is called to get the motion vector prediction
                for a macroblock to allow rate distortion optimisation in
                the motion estimation and mode decision process.

                block_index indicates the 4x4 block index for the top left
                4x4 block for which the current vector applies

                vector_number is the number of the current vector in raster
                scan order, and so goes from 0 to 1 for 16x8 and 8x16 and
                0 to 3 for 8x8. For 8x8 partitions, it should have the value
                it would have if the whole macroblock had been coded with
                the same sub-block size. So in the bottom left 8x8 block,
                the first 8x4 sub-block should have vector_number 8.

                block_shape indicates the shape of the block that the
                current vector represents: 16x16, 16x8, ..., 8x4, 4x4

                Prediction for the motion vector can be from the above
                block B, the above right block C, the left block A, or the
                median of the three. When C is not available, the vector
                of the above left block D is used instead.
                A is the block containing the pel to the left of the top
                left pel of the current block
                B is the block containing the pel above the top left pel of
                the current block
                C is the block containing the pel above and to the right of
                the top right pel of the current block
                D is the block containing the pel above and to the left of
                the top left pel of the current block

                    D  |  B        |  C
                    --------------------
                    A  |  Current  |
                       |           |
                                                                            */
/* RETURNS:     void.                                                       */
/* ------------------------------------------------------------------------ */

bool ArcMotionEstimationModel::ArcGetH264MotionVectorPrediction(int block_index, int vector_number, h264_block_shape_type_t block_shape,
                                             int& pred_hor, int& pred_ver)
{

    // The default reference frame is set to REFERENCE_FRAME_NOT_AVAILABLE meaning not available,
    // that is, outside of the picture or the current slice
    // The value of DIFFERENT_REFERENCE_FRAME is used to indicate the predictor is available but is intra, direct etc,
    // hence has different reference picture to current but is inside the picture.
    int ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE;
    int ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE;
    int ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE;
    int pred_hor_A = 0;
    int pred_hor_B = 0;
    int pred_hor_C = 0;
    int pred_ver_A = 0;
    int pred_ver_B = 0;
    int pred_ver_C = 0;

    ////////////////////////////////////////////////////////////////////////
    // Set the neighbouring controls
    ////////////////////////////////////////////////////////////////////////

    int left_block_index;
    int above_block_index;
    int above_left_block_index;
    int above_right_block_index;

   NeighbourFlagsExpanded_t * above_left_control;
   NeighbourFlagsExpanded_t * above_right_control;

    {
        // Field or Non-MB-AFF Frame
        int yW;

        // Calculate the top left position (xC, yC) of the current block
        int xC = (block_index & 3) << 2;
        int yC = (block_index & 12);

        // Left MB
        yW = (yC + 16) & (16-1);
        left_block_index = (((xC + 15) & 15) >> 2) + (yW & 12);

        // Above MB
        yW = (yC-1 + 16) & (16-1);
        above_block_index = (xC >> 2) + (yW & 12);

        // Above left MB
        ArcGetNeighbouringLocationNonMBAFF(xC - 1, yC - 1, 16, &above_left_control, yW);
        above_left_block_index = (((xC + 15) & 15) >> 2) + (yW & 12);

        // Above Right MB
        ArcGetNeighbouringLocationNonMBAFF(xC + h264_block_width_table[block_shape], yC - 1,
                                        16, &above_right_control, yW);
        above_right_block_index = (((xC + h264_block_width_table[block_shape]) & 15) >> 2) + (yW & 12);
    }

    // Determine the reference frame indices for blocks A, B and C
    // and set to REFERENCE_FRAME_NOT_AVAILABLE if not available and DIFFERENT_REFERENCE_FRAME if intra
    // And also get the prediction vectors if available

    ////////////////////////////////////////////////////////////////////////
    // Left Block : A
    ////////////////////////////////////////////////////////////////////////

    if ((block_index & 3) == 0)
    {
        // Left edge of MB, look to left MB
        if (leftMB.validID)
        {
            if (!leftMB.intra)
            {
                {
                    ref_frame_A = 0;

                    pred_hor_A = leftMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[left_block_index]].mvx;
                    pred_ver_A = leftMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[left_block_index]].mvy;
                }
            }
            else
            {
                ref_frame_A = DIFFERENT_REFERENCE_FRAME;
            }
        }
        // else default: ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE; etc.
    }
    else
    {
        // Left Motion Vector is in this macroblock
        {
            ref_frame_A = 0;

            pred_hor_A = mv_for_hor[block_index-1];
            pred_ver_A = mv_for_ver[block_index-1];
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Above Block : B
    ////////////////////////////////////////////////////////////////////////

    if (block_index < 4)
    {
        // Top edge of MB, look to above MB
        if (aboveMB.validID)
        {
            if (!aboveMB.intra)
            {
                {
                    ref_frame_B = 0;


                    pred_hor_B = aboveMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[above_block_index]].mvx;
                    pred_ver_B = aboveMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[above_block_index]].mvy;

                }

            }
            else
            {
                ref_frame_B = DIFFERENT_REFERENCE_FRAME;
            }
        }
        // else default: ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE; etc.
    }
    else
    {
        // Above Motion Vector is in this macroblock
        {
            ref_frame_B = 0;

            pred_hor_B = mv_for_hor[block_index-4];
            pred_ver_B = mv_for_ver[block_index-4];
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Above Right Block : C or
    // Above  Left Block : D
    ////////////////////////////////////////////////////////////////////////

    NeighbourFlagsExpanded_t * control_C = 0;
    int block_index_C;

    // The following case applies when the above right is in the current MB
    // but is in a subsequent 8x8 partition and so is not available
    // 8x4 or 4x4 bottom right of a left hand 8x8 partition - above right not available
    bool above_right_not_available =
        ((H264_8X4 == block_shape) || (H264_4X4 == block_shape)) &&
        (((block_index + (h264_block_width_table[block_shape] >> 2)) & 7) == 6);

    above_right_not_available |= (0 == above_right_control) || ((above_right_control->validID)?false:true);

    if (!above_right_not_available)
    {
        if (!above_right_control->intra)
        {
            control_C = above_right_control;
            block_index_C = above_right_block_index;
        }
        else
        {
            ref_frame_C = DIFFERENT_REFERENCE_FRAME;
        }
    }
    else
    {
        // Position C is not available so look at position D
        if ((0 != above_left_control) && above_left_control->validID)
        {
            if (!above_left_control->intra)
            {
                control_C = above_left_control;
                block_index_C = above_left_block_index;
            }
            else
            {
                ref_frame_C = DIFFERENT_REFERENCE_FRAME;
            }
        }
        // else default: ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE; etc.
    }
    // else default: ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE; etc.

    if (0 != control_C)
    {
        {
            ref_frame_C = 0;

            if (DIFFERENT_REFERENCE_FRAME != ref_frame_C)
            {

            if ((block_index & 0xc) == 0)
            {
               // 4x4 sub-blocks 0, 1, 2 and 3 (raster numbering)
               // above right and above left will both be in a neighbouring
               // macroblock
                   pred_hor_C = control_C->mv[reverse_h264_8x8_block_index_to_raster_index_table[block_index_C]].mvx;
                   pred_ver_C = control_C->mv[reverse_h264_8x8_block_index_to_raster_index_table[block_index_C]].mvy;
                }
            else
            {
                   // 4x4 sub-blocks 4, 8 and 12 (raster numbering)
                   // above right will always be in the current macroblock and
                   // therefore above left will never be used

                   // 4x4 sub-blocks 7, 11 and 15 (raster numbering)
                   // above right will always be in the neighbouring future
                   // macroblock and therefore we will always use above left (which
                   // will always be in the current macroblock)

               // 4x4 sub-blocks 5 and 13 (raster numbering)
               // above right will always be in a future 8x8 block of the
               // current macroblock. therefore we will always use above left
               // (which will always be in the current macroblock)

               // 4x4 sub-blocks 6, 9, 10 and 14 (raster numbering)
               // above right and above left will both be in the current
               // macroblock

               // all the above 4 cases can be grouped into one else by using
               // the block_index_C instead of block_index - N (where N would
               // have to change for each case)
                   pred_hor_C = mv_for_hor[block_index_C];
               pred_ver_C = mv_for_ver[block_index_C];
            }

            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////
    // If only one predictor matches the current reference frame, then use it
    // Otherwise, for 16x8, 8x16, if reference frame matches, use single predictor
    // Else use median prediction
    ///////////////////////////////////////////////////////////////////////////
    pred_hor = 0;
    pred_ver = 0;

    if      ((ref_frame_A == 0) &&
             (ref_frame_B != 0) &&
             (ref_frame_C != 0))
    {
        pred_hor = pred_hor_A;
        pred_ver = pred_ver_A;
    }
    else if ((ref_frame_A != 0) &&
             (ref_frame_B == 0) &&
             (ref_frame_C != 0))
    {
        pred_hor = pred_hor_B;
        pred_ver = pred_ver_B;
    }
    else if ((ref_frame_A != 0) &&
             (ref_frame_B != 0) &&
             (ref_frame_C == 0))
    {
        pred_hor = pred_hor_C;
        pred_ver = pred_ver_C;
    }
    else if ((H264_16X8 == block_shape) &&
             (0 == vector_number) &&
             (ref_frame_B == 0))
    {
        // Top 16x8 block, use B if ref frame matches
        pred_hor = pred_hor_B;
        pred_ver = pred_ver_B;
    }
    else if ((H264_16X8 == block_shape) &&
             (1 == vector_number) &&
             (ref_frame_A == 0))
    {
        // Bottom 16x8 block, use A if ref frame matches
        pred_hor = pred_hor_A;
        pred_ver = pred_ver_A;
    }
    else if ((H264_8X16 == block_shape) &&
             (0 == vector_number) &&
             (ref_frame_A == 0))
    {
        // Left 8x16 block, use A if ref frame matches
        pred_hor = pred_hor_A;
        pred_ver = pred_ver_A;
    }
    else if ((H264_8X16 == block_shape) &&
             (1 == vector_number) &&
             (ref_frame_C == 0))
    {
        // Right 8x16 block, use C if ref frame matches
        pred_hor = pred_hor_C;
        pred_ver = pred_ver_C;
    }
    else
    {
        // If D, B and C all are outside of the picture, use A
        // else use Median
        if ((REFERENCE_FRAME_NOT_AVAILABLE == ref_frame_B) && (REFERENCE_FRAME_NOT_AVAILABLE == ref_frame_C))
        {
            pred_hor = pred_hor_A;
            pred_ver = pred_ver_A;
        }
        else
        {
            // Use Median
            pred_hor = MEDIAN(pred_hor_A, pred_hor_B, pred_hor_C);
            pred_ver = MEDIAN(pred_ver_A, pred_ver_B, pred_ver_C);
        }
    }
   return true;
}


/*
   Function for calculating MV predictors for H263
*/
bool ArcMotionEstimationModel::ArcGetH263MotionVectorPrediction(
                                  int vector_number, h264_block_shape_type_t block_shape,
                                  int& pred_hor, int& pred_ver)
{
   // The default reference frame is set to REFERENCE_FRAME_NOT_AVAILABLE meaning not available,
   // that is, outside of the picture or the current slice
   // The value of DIFFERENT_REFERENCE_FRAME is used to indicate the predictor is available but is intra, direct etc,
   // hence has different reference picture to current but is inside the picture.

   int pred_hor_A = 0;
   int pred_hor_B = 0;
   int pred_hor_C = 0;
   int pred_ver_A = 0;
   int pred_ver_B = 0;
   int pred_ver_C = 0;

   // In H.263/MPEG4 the block index is that of the 8x8 block.
   // It does not refer to the 4x4 block index as in H.264.
   int block_index = vector_number;

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "#### H.263 MVP Calculation ####\n");
    fprintf(vdump_fp, "##   Block Index = %d\n", block_index);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Set the neighbouring controls
   ////////////////////////////////////////////////////////////////////////

   NeighbourFlagsExpanded_t* left_control;
   int left_block_index;

   NeighbourFlagsExpanded_t* above_control;
   int above_block_index;

   NeighbourFlagsExpanded_t* above_right_control;
   int above_right_block_index;

   {
      int yW;
      int c_shift = 16;

      if(block_index > 0) c_shift = 8; // i.e. 8x8 mode and not 0'th

      // Calculate the top left position (xC, yC) of the current block
      //int xC = (block_index & 3) << 2;
      //int yC = (block_index & 12);
      int xC = (block_index & 1)<<3;
      int yC = (block_index & 2)<<2;
      // Left MB
      ArcGetNeighbouringLocationNonMBAFF(xC - 1, yC, 16, &left_control, yW);
      left_block_index = (((xC + 15) & 15) >> 3) + ((yW & 8) >> 2);

      // Above MB
      ArcGetNeighbouringLocationNonMBAFF(xC, yC - 1, 16, &above_control, yW);
      above_block_index = (xC >> 3) + ((yW & 8) >> 2);

      // Above Right MB
      ArcGetNeighbouringLocationNonMBAFF(xC + c_shift, yC - 1,
                              16, &above_right_control, yW);
      above_right_block_index = (((xC + c_shift) & 15) >> 3) + ((yW & 8) >> 2);
   }

   // Determine the reference frame indices for blocks A, B and C
   // and set to REFERENCE_FRAME_NOT_AVAILABLE if not available and DIFFERENT_REFERENCE_FRAME if intra
   // And also get the prediction vectors if available

   ////////////////////////////////////////////////////////////////////////
   // Left Block : A
   ////////////////////////////////////////////////////////////////////////

   if ((block_index & 1) == 0)
   {
      // Left edge of MB, look to left MB
      if (leftMB.validID)
      {
         if (leftMB.code && !leftMB.intra)
         {
            pred_hor_A = leftMB.mv[left_block_index].mvx;
            pred_ver_A = leftMB.mv[left_block_index].mvy;
         }
         else
         {
            pred_hor_A = pred_ver_A = 0;
         }
      }
      else
      {
         pred_hor_A = pred_ver_A = 0;
      }
      // else default: ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE; etc.
   }
   else
   {
      // Left Motion Vector is in this macroblock
      pred_hor_A = mv_for_hor[h264_8x8_block_index_to_raster_index_table[left_block_index]];
      pred_ver_A = mv_for_ver[h264_8x8_block_index_to_raster_index_table[left_block_index]];
   }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   LEFT BLOCK A (MV1):              ValidID = %d - Code = %d - Intra = %d - MV1x = %d - MV1y = %d\n", leftMB.validID, leftMB.code, leftMB.intra, pred_hor_A, pred_ver_A);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Above Block : B
   ////////////////////////////////////////////////////////////////////////

   if (block_index < 2)
   {
      // Top edge of MB, look to above MB
      if (aboveMB.validID)
      {
         if (aboveMB.code && !aboveMB.intra)
         {
            pred_hor_B = aboveMB.mv[above_block_index].mvx;
            pred_ver_B = aboveMB.mv[above_block_index].mvy;
         }
         else
         {
            pred_hor_B = pred_ver_B = 0;
         }
      }
      else
      {
         pred_hor_B = pred_hor_A;
         pred_ver_B = pred_ver_A;
      }
      // else default: ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE; etc.
   }
   else
   {
      // Above Motion Vector is in this macroblock
      pred_hor_B = mv_for_hor[0];
      pred_ver_B = mv_for_ver[0];
   }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   ABOVE BLOCK B (MV2):             ValidID = %d - Code = %d - Intra = %d - MV2x = %d - MV2y = %d\n", aboveMB.validID, aboveMB.code, aboveMB.intra, pred_hor_B, pred_ver_B);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Above Right Block : C or
   // Above  Left Block : D
   ////////////////////////////////////////////////////////////////////////

/*
   // The following case applies when the above right is in the current MB
   // but is in a subsequent 8x8 partition and so is not available
   // 8x4 or 4x4 bottom right of a left hand 8x8 partition - above right not available
   bool above_right_not_available =
      ((H264_8X4 == block_shape) || (H264_4X4 == block_shape)) &&
      (((block_index + (h264_block_width_table[block_shape] >> 2)) & 7) == 6);
   bool above_right_not_available |= (0 == above_right_control) || (control->CollectionID != above_right_control->CollectionID);
*/

   if(block_index > 1)// all in this Macroblock
   {
      pred_hor_C = mv_for_hor[2];
      pred_ver_C = mv_for_ver[2];
   }
   else if(!aboveRightMB.valid) // rule 4
   {
      pred_hor_C = pred_ver_C = 0;
   }
   else
   {
      if (aboveRightMB.validID)
      {
         if (aboveRightMB.code && !aboveRightMB.intra)
         {
               pred_hor_C = aboveRightMB.mv[above_right_block_index].mvx;
               pred_ver_C = aboveRightMB.mv[above_right_block_index].mvy;
         }
         else
         {
            pred_hor_C = pred_ver_C = 0;
         }
      }
      else
      {
         pred_hor_C = pred_hor_A;
         pred_ver_C = pred_ver_A;
      }
   }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   ABOVE R/L C/D (MV3): Valid = %d - ValidID = %d - Code = %d - Intra = %d - MV3x = %d - MV3y = %d\n", aboveRightMB.valid, aboveRightMB.validID, aboveRightMB.code, aboveRightMB.intra, pred_hor_C, pred_ver_C);
#endif

   pred_hor = MEDIAN(pred_hor_A, pred_hor_B, pred_hor_C);
   pred_ver = MEDIAN(pred_ver_A, pred_ver_B, pred_ver_C);

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   MVPx = %d - MVPy = %d\n", pred_hor, pred_ver);
    fprintf(vdump_fp, "##############################\n\n");
#endif

    return true;
}

/*
   Function for calculating MV predictors for MPEG4
*/
bool ArcMotionEstimationModel::ArcGetMpeg4MotionVectorPrediction(
                                  int vector_number, h264_block_shape_type_t block_shape,
                                  int& pred_hor, int& pred_ver)
{
   // The default reference frame is set to REFERENCE_FRAME_NOT_AVAILABLE meaning not available,
   // that is, outside of the picture or the current slice
   // The value of DIFFERENT_REFERENCE_FRAME is used to indicate the predictor is available but is intra, direct etc,
   // hence has different reference picture to current but is inside the picture.

   int pred_hor_A = 0;
   int pred_hor_B = 0;
   int pred_hor_C = 0;
   int pred_ver_A = 0;
   int pred_ver_B = 0;
   int pred_ver_C = 0;
   int A_not_valid, B_not_valid, C_not_valid;

   // In H.263/MPEG4 the block index is that of the 8x8 block.
   // It does not refer to the 4x4 block index as in H.264.
   int block_index = vector_number;

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "#### MPEG4 MVP Calculation ####\n");
    fprintf(vdump_fp, "##   Block Index = %d\n", block_index);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Set the neighbouring controls
   ////////////////////////////////////////////////////////////////////////

   NeighbourFlagsExpanded_t* left_control;
   int left_block_index;

   NeighbourFlagsExpanded_t* above_control;
   int above_block_index;

   NeighbourFlagsExpanded_t* above_right_control;
   int above_right_block_index;

// mpeg4 standard 7.6.5

   {
      int yW;
      int c_shift = 16;

      if(block_index > 0) c_shift = 8; // i.e. 8x8 mode and not 0'th

      // Calculate the top left position (xC, yC) of the current block
      //int xC = (block_index & 3) << 2;
      //int yC = (block_index & 12);
      int xC = (block_index & 1)<<3;
      int yC = (block_index & 2)<<2;
      // Left MB
      ArcGetNeighbouringLocationNonMBAFF(xC - 1, yC, 16, &left_control, yW);
      left_block_index = (((xC + 15) & 15) >> 3) + ((yW & 8) >> 2);

      // Above MB
      ArcGetNeighbouringLocationNonMBAFF(xC, yC - 1, 16, &above_control, yW);
      above_block_index = (xC >> 3) + ((yW & 8) >> 2);

      // Above Right MB
      ArcGetNeighbouringLocationNonMBAFF(xC + c_shift, yC - 1,
                              16, &above_right_control, yW);
      above_right_block_index = (((xC + c_shift) & 15) >> 3) + ((yW & 8) >> 2);
   }

   // Determine the reference frame indices for blocks A, B and C
   // and set to REFERENCE_FRAME_NOT_AVAILABLE if not available and DIFFERENT_REFERENCE_FRAME if intra
   // And also get the prediction vectors if available

   ////////////////////////////////////////////////////////////////////////
   // Left Block : A
   ////////////////////////////////////////////////////////////////////////

   if ((block_index & 1) == 0)
   {
      // Left edge of MB, look to left MB
      if (leftMB.validID)
      {
         if (leftMB.code && !leftMB.intra)
         {
            pred_hor_A = leftMB.mv[left_block_index].mvx;
            pred_ver_A = leftMB.mv[left_block_index].mvy;
         }
         else
         {
            pred_hor_A = pred_ver_A = 0;
         }
         A_not_valid = 0;
      }
      else
      {
         pred_hor_A = pred_ver_A = 0;
         A_not_valid = 1;
      }
      // else default: ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE; etc.
   }
   else
   {
      // Left Motion Vector is in this macroblock
      pred_hor_A = mv_for_hor[h264_8x8_block_index_to_raster_index_table[left_block_index]];
      pred_ver_A = mv_for_ver[h264_8x8_block_index_to_raster_index_table[left_block_index]];

      A_not_valid = 0;
   }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   LEFT BLOCK A (MV1):  ValidID = %d - Code = %d - Intra = %d - MV1x = %d - MV1y = %d\n", leftMB.validID, leftMB.code, leftMB.intra, pred_hor_A, pred_ver_A);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Above Block : B
   ////////////////////////////////////////////////////////////////////////

   if (block_index < 2)
   {
      // Top edge of MB, look to above MB
      if (aboveMB.validID)
      {
         if (aboveMB.code && !aboveMB.intra)
         {
            pred_hor_B = aboveMB.mv[above_block_index].mvx;
            pred_ver_B = aboveMB.mv[above_block_index].mvy;
         }
         else
         {
            pred_hor_B = pred_ver_B = 0;
         }
         B_not_valid = 0;
      }
      else
      {
         pred_hor_B = 0;
         pred_ver_B = 0;
         B_not_valid = 1;
      }
      // else default: ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE; etc.
   }
   else
   {
      // Above Motion Vector is in this macroblock
      pred_hor_B = mv_for_hor[0];
      pred_ver_B = mv_for_ver[0];
      B_not_valid = 0;
   }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   ABOVE BLOCK B (MV2): ValidID = %d - Code = %d - Intra = %d - MV2x = %d - MV2y = %d\n", aboveMB.validID, aboveMB.code, aboveMB.intra, pred_hor_B, pred_ver_B);
#endif

   ////////////////////////////////////////////////////////////////////////
   // Above Right Block : C or
   // Above  Left Block : D
   ////////////////////////////////////////////////////////////////////////

   // The following case applies when the above right is in the current MB
   // but is in a subsequent 8x8 partition and so is not available
   // 8x4 or 4x4 bottom right of a left hand 8x8 partition - above right not available
   bool above_right_not_available ;

   above_right_not_available = (0 == above_right_control) || ((above_right_control->validID)?false:true);

   if(block_index > 1)// all in this Macroblock
   {
      pred_hor_C = mv_for_hor[2];
      pred_ver_C = mv_for_ver[2];
      C_not_valid = 0;
   }
   else
   {
      if (!above_right_not_available)
      {
         if (above_right_control->code && !above_right_control->intra )
         {
            pred_hor_C = above_right_control->mv[above_right_block_index].mvx;
            pred_ver_C = above_right_control->mv[above_right_block_index].mvy;
         }
         else
         {
            pred_hor_C = pred_ver_C = 0;
         }
         C_not_valid = 0;
      }
      else
      {
         pred_hor_C = 0;
         pred_ver_C = 0;
         C_not_valid = 1;
      }
   }

#ifdef ME_MVP_DUMP
    if (above_right_control != 0) {
      fprintf(vdump_fp, "##   ABOVE R/L C/D (MV3): ValidID = %d - Code = %d - Intra = %d - Not Availab = %d - MV3x = %d - MV3y = %d\n", above_right_control->validID, above_right_control->code, above_right_control->intra, above_right_not_available, pred_hor_C, pred_ver_C);
    } else {
      fprintf(vdump_fp, "##   ABOVE R/L C/D (MV3): Not Availab = %d - MV3x = %d - MV3y = %d\n", above_right_not_available, pred_hor_C, pred_ver_C);
    }
#endif

// none not valid  - go ahead and use the values
// one not-valid = set it to zero (already done)
// two not valid  - set to the valid one - see below
// three not valid - set to zero - already done

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   A_not_valid = %d - B_not_valid = %d - C_not_valid = %d\n", A_not_valid, B_not_valid, C_not_valid);
#endif

   if(A_not_valid + B_not_valid + C_not_valid == 2)
   {
      if(!A_not_valid)
      {
         pred_hor = pred_hor_A;
         pred_ver = pred_ver_A;
      }
      else if(!B_not_valid)
      {
         pred_hor = pred_hor_B;
         pred_ver = pred_ver_B;
      }
      else
      {
         pred_hor = pred_hor_C;
         pred_ver = pred_ver_C;
      }

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   MVPx = %d - MVPy = %d\n", pred_hor, pred_ver);
    fprintf(vdump_fp, "##############################\n\n");
#endif

      return true;
   }
   else
   {
        pred_hor = MEDIAN(pred_hor_A, pred_hor_B, pred_hor_C);
        pred_ver = MEDIAN(pred_ver_A, pred_ver_B, pred_ver_C);

#ifdef ME_MVP_DUMP
    fprintf(vdump_fp, "##   MVPx = %d - MVPy = %d\n", pred_hor, pred_ver);
    fprintf(vdump_fp, "##############################\n\n");
#endif

        return true;
   }
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcCalculateH264SkippedMotionVectors  */
/* DESCRIPTION: Copied from BT function CH264Encoder::CalculateSkippedMotionVectors

            This is called to calculate the motion vectors for skipped
                macroblocks in P and SP slices putting the result into
                m_PSliceSkippedHor and
                m_PSliceSkippedVer.
.
                Skipped macroblocks in inter pictures are coded as either
                   - having zero motion vector and simply being a copy of
                     the reference picture with index 0, and so can be
                     decoded as inter 16x16 with zero vector and zero
                     remapped reference picture index.
                   - having motion vector equal to the motion vector
                     predictor that the block would have had, if it had
                     been 16x16 coded

                The choice depends on the vectors of the above and left
                blocks, A and B, as follows

                if ((Vector A not available) || (Vector B not available) ||
                    (Vector A == 0 and Vector A has remapped index 0) ||
                    (Vector B == 0 and Vector B has remapped index 0))
                {
                    Zero vector is used: no action needed as vector
                    will already be zero
                }
                else
                {
                    Motion vector prediction is used
                }

                In both cases the picture index must be set to the array
                position of the picture with remapped index of zero.

                So inverting the logic to test only for the motion vector
                prediction case

                if ((Vector A available) && (Vector B available) &&
                    (Vector A != 0 or Vector A does not have reference index 0) &&
                    (Vector B != 0 or Vector B does not have reference index 0))
                                                                            */
/* RETURNS:     void.                                                       */
/* ------------------------------------------------------------------------ */

void ArcMotionEstimationModel::ArcCalculateH264SkippedMotionVectors(void)
{
    ////////////////////////////////////////////////////////////////////////
    // Set the neighbouring controls
    ////////////////////////////////////////////////////////////////////////

    int left_block_index;
    int above_block_index;

    int left_hor = -1, left_ver = -1, above_hor = -1, above_ver = -1, leftIntra = true, aboveIntra =  true;

    {
        // Field or Non-MB-AFF Frame
        int yW;

        // Left MB
      yW = (0 + 16) & (16-1);
        left_block_index = 3 + (yW & 12);

        // Above MB
      yW = (-1 + 16) & (16-1);
        above_block_index = (yW & 12);

        if (leftMB.valid)
        {
            left_hor = leftMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[left_block_index]].mvx;
            left_ver = leftMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[left_block_index]].mvy;
            leftIntra = leftMB.intra;
        }

        if (aboveMB.valid)
        {
            above_hor = aboveMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[above_block_index]].mvx;
            above_ver = aboveMB.mv[reverse_h264_8x8_block_index_to_raster_index_table[above_block_index]].mvy;
            aboveIntra = aboveMB.intra;
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Determine the skipped mode vector type: zero or 16x16 predictor
    ////////////////////////////////////////////////////////////////////////

    if ( leftMB.validID &&
          aboveMB.validID &&
          ( (0 != left_hor) || (0 != left_ver) || leftIntra) &&
          ( (0 != above_hor) || (0 != above_ver) || aboveIntra) )
    {
        // Skipped vector is the 16x16 vector prediction
      ArcGetH264MotionVectorPrediction(0, 0, H264_16X16, PSliceSkippedHor, PSliceSkippedVer);
    }
    else
    {
        // Skipped vector is zero
        PSliceSkippedHor = 0;
        PSliceSkippedVer = 0;
    }
}

/*
   Function for calculating skipped MV's for MPEG4
*/
void ArcMotionEstimationModel::ArcCalculateMpeg4SkippedMotionVectors(void)
{
   // Skipped vector is zero
   PSliceSkippedHor = 0;
   PSliceSkippedVer = 0;
}

/*
   Function for calculating skipped MV's for H263
*/
void ArcMotionEstimationModel::ArcCalculateH263SkippedMotionVectors(void)
{
   // Skipped vector is zero
   PSliceSkippedHor = 0;
   PSliceSkippedVer = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcMakeH264InterPrediction */
/* DESCRIPTION: Copied from BT function CH264Encoder::MakeInterPrediction
            Makes Inter prediction for the current macroblock, putting
                the result into y_prediction_data if make_skipped_prediction
                is false, and into m_skip_mb_y_prediction_data otherwise.
                The term block is used to refer to a 16x16, 16x8, 8x16 or
                8x8 area, and the term sub-block for the sub-division of a
                block.
                It was originally based on CH264Decoder::InterRecon8x8 with
                the following differences (and many more!):

                -   The output is the prediction in y_prediction_data rather
                    than the clipped sum of residual and prediction in y_data.
*/
/* RETURNS:     True if successful else false.                              */
/* ------------------------------------------------------------------------ */

bool ArcMotionEstimationModel::ArcMakeH264InterPrediction(unsigned char mbType)
{
    h264_block_shape_type_t block_shape;
    int number_of_blocks;

    switch (mbType&127)
    {
        case MBCT_H264_SKIPPED :
        case MBCT_H264_INTER16X16 :
            block_shape = H264_16X16;
            number_of_blocks = 1;
            break;

        case MBCT_H264_INTER16X8 :
            block_shape = H264_16X8;
            number_of_blocks = 2;
            break;

        case MBCT_H264_INTER8X16 :
            block_shape = H264_8X16;
            number_of_blocks = 2;
            break;

        case MBCT_H264_INTER8X8 :
            block_shape = H264_8X8;
            number_of_blocks = 4;
            break;

        default :
            assert(false);
            return false;  // Error.
    }

    // Loop for each block that needs to be motion compensated
    for (int block_number=0; block_number < number_of_blocks; block_number++)
    {
        // Map the partition mode into a block shape and number of sub-blocks.
        h264_block_shape_type_t sub_block_shape;
        int number_of_sub_blocks;
        int first_block_index_4x4; // Index of the 4x4 block at top left of block, 0, 2, 8, 10
        int block_index_8x8;  // Used to index the partition info etc, 0..3
        int sub_block_hor_size;
        int sub_block_ver_size;

        if (H264_8X8 != block_shape)
        {
            sub_block_shape = H264_8X8;
            number_of_sub_blocks = 1;
            first_block_index_4x4 = h264_block_offset_table[block_shape][block_number];

            block_index_8x8 = block_number;

            if (H264_16X8 == block_shape)
            {
                block_index_8x8 = 2*block_number;
            }

            sub_block_hor_size = h264_block_width_table[block_shape];
            sub_block_ver_size = h264_block_height_table[block_shape];
        }
        else
        {
            sub_block_shape = H264_8X8;
            number_of_sub_blocks = 1;

            first_block_index_4x4 = ((block_number & 1) << 1) + ((block_number & 2) << 2);
            block_index_8x8 = block_number;

            sub_block_hor_size = 8;
            sub_block_ver_size = 8;
        }

        pel_t * forward_reference_picture = 0;

        {
         forward_reference_picture = (pel_t *) refYWthBrdrPels;

            if (0 == forward_reference_picture)
            {
                // Really should never occur. Give up as it seems to be a lost cause!
                assert(false);
                return false;
            }
        }

        // Loop for each sub-block that needs to be motion compensated
        for (int sub_block_number=0; sub_block_number < number_of_sub_blocks; sub_block_number++)
        {
            // block_index indicates the index of the top left 4x4 block of the motion compensation block
            int block_index = first_block_index_4x4 + h264_block_offset_table[sub_block_shape][sub_block_number];

            // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
            // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
            int integer_for_hor_vector;
            int integer_for_ver_vector;
            int fractional_for_hor_vector;
            int fractional_for_ver_vector;


            // To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed
            int extra_for_left_pels;
            int extra_for_right_pels;
            int extra_for_top_pels;
            int extra_for_bottom_pels;

            {
                //////////////////////////////////////////////////////////////////////////////////////////////
                // LUMINANCE MOTION COMPENSATION
                //////////////////////////////////////////////////////////////////////////////////////////////

                // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
                integer_for_hor_vector = mv_for_hor[block_index] >> 2;
                integer_for_ver_vector = mv_for_ver[block_index] >> 2;

                fractional_for_hor_vector = mv_for_hor[block_index] - 4*integer_for_hor_vector;
                fractional_for_ver_vector = mv_for_ver[block_index] - 4*integer_for_ver_vector;

                {
                    // Calculate the extra pels needed
                    if ((0 == fractional_for_hor_vector) && (0 == fractional_for_ver_vector))
                    {
                        // Integer vector
                        extra_for_left_pels = 0;
                        extra_for_right_pels = 0;
                        extra_for_top_pels = 0;
                        extra_for_bottom_pels = 0;
                    }
                    else
                    {
                        if (0 == fractional_for_hor_vector)
                        {
                            extra_for_left_pels = 0;
                            extra_for_right_pels = 0;
                        }
                        else
                        {
                            extra_for_left_pels = 2;
                            extra_for_right_pels = 3;
                        }

                        if (0 == fractional_for_ver_vector)
                        {
                            extra_for_top_pels = 0;
                            extra_for_bottom_pels = 0;
                        }
                        else
                        {
                            extra_for_top_pels = 2;
                            extra_for_bottom_pels = 3;
                        }
                    }

                    // Get the integer pel values from the reference picture
                    // taking care with motion vectors that go off the edge of the picture

                        {
                            ArcGetIntegerPelReferenceBlockFromFrame(forward_reference_picture,
                                &ForwardWorkingArray,
                                (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS,
                                auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 + NUMBER_OF_TOP_BORDER_PELS + NUMBER_OF_BOTTOM_BORDER_PELS,
                                16*auxReg.ME_MB_INDEX__MB_X_INDEX + 4 * (block_index & 3) + integer_for_hor_vector - extra_for_left_pels + NUMBER_OF_LEFT_BORDER_PELS,
                                16*auxReg.ME_MB_INDEX__MB_Y_INDEX + 4 * (block_index / 4) + integer_for_ver_vector - extra_for_top_pels + NUMBER_OF_TOP_BORDER_PELS,
                                sub_block_hor_size + extra_for_left_pels + extra_for_right_pels,
                                sub_block_ver_size + extra_for_top_pels + extra_for_bottom_pels);
                        }

               ArcFilterH264LumaReferenceBlock(&ForwardWorkingArray, fractional_for_hor_vector, fractional_for_ver_vector,
                                             sub_block_hor_size, sub_block_ver_size);
                }
            }

            {
                // !make_skipped_prediction
                ArcCopyToLumaPrediction(
                                     block_index,
                                     sub_block_hor_size, sub_block_ver_size
                                     );
            }

        }
    }

    return true;
}

bool ArcMotionEstimationModel::ArcMakeH263InterPrediction(unsigned char mbType)
{
   int integer_for_hor_vector;
   int integer_for_ver_vector;
   int fractional_for_hor_vector;
   int fractional_for_ver_vector;

   // To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed
   int extra_for_left_pels;
   int extra_for_right_pels;
   int extra_for_top_pels;
   int extra_for_bottom_pels;

   pel_t * forward_reference_picture = 0;
   int block_index;

   h264_block_shape_type_t block_shape;
   int number_of_blocks;

   switch (mbType)
   {
      case MBCT_MPEG4_SKIPPED :
      case MBCT_MPEG4_INTER16X16 :
         block_shape = H264_16X16;
         number_of_blocks = 1;
         break;


      case MBCT_MPEG4_INTER8X8 :
         block_shape = H264_8X8;
         number_of_blocks = 4;
         break;

      default :
         assert(false);
         return false;  // Error.
   }

   // Loop for each block that needs to be motion compensated
   for (int block_number=0; block_number < number_of_blocks; block_number++)
   {
      // Map the partition mode into a block shape and number of sub-blocks.
      h264_block_shape_type_t sub_block_shape;
      int number_of_sub_blocks;
//      int first_block_index_4x4; // Index of the 4x4 block at top left of block, 0, 2, 8, 10
//      int block_index_8x8;  // Used to index the partition info etc, 0..3
      int sub_block_hor_size;
      int sub_block_ver_size;

      if (H264_8X8 != block_shape)
      {
         sub_block_shape = H264_8X8;
         number_of_sub_blocks = 1;
//          first_block_index_4x4 = h264_block_offset_table[block_shape][block_number];

//          block_index_8x8 = block_number;

         sub_block_hor_size = 16;
         sub_block_ver_size = 16;
      }
      else
      {
         sub_block_shape = H264_8X8;
         number_of_sub_blocks = 1;

//          first_block_index_4x4 = ((block_number & 1) << 1) + ((block_number & 2) << 2);
//          block_index_8x8 = block_number;

         sub_block_hor_size = 8;
         sub_block_ver_size = 8;
      }


      forward_reference_picture = (pel_t *) refYWthBrdrPels;


      // block_index indicates the index of the top left 4x4 block of the motion compensation block
//      block_index = first_block_index_4x4 + h264_block_offset_table[sub_block_shape][0];
      block_index = block_number;

      // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
      // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.


      //////////////////////////////////////////////////////////////////////////////////////////////
      // LUMINANCE MOTION COMPENSATION
      //////////////////////////////////////////////////////////////////////////////////////////////

      // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
      integer_for_hor_vector = mv_for_hor[h264_8x8_block_index_to_raster_index_table[block_index]] >> 1;
      integer_for_ver_vector = mv_for_ver[h264_8x8_block_index_to_raster_index_table[block_index]] >> 1;

      fractional_for_hor_vector = mv_for_hor[h264_8x8_block_index_to_raster_index_table[block_index]] - 2*integer_for_hor_vector;
      fractional_for_ver_vector = mv_for_ver[h264_8x8_block_index_to_raster_index_table[block_index]] - 2*integer_for_ver_vector;

      // Calculate the extra pels needed
      if ((0 == fractional_for_hor_vector) && (0 == fractional_for_ver_vector))
      {
         // Integer vector
         extra_for_left_pels = 0;
         extra_for_right_pels = 0;
         extra_for_top_pels = 0;
         extra_for_bottom_pels = 0;
      }
      else
      {
         if (0 == fractional_for_hor_vector)
         {
            extra_for_left_pels = 0;
            extra_for_right_pels = 0;
         }
         else
         {
            extra_for_left_pels = 0;
            extra_for_right_pels = 1;
         }

         if (0 == fractional_for_ver_vector)
         {
            extra_for_top_pels = 0;
            extra_for_bottom_pels = 0;
         }
         else
         {
            extra_for_top_pels = 0;
            extra_for_bottom_pels = 1;
         }
      }

         // Get the integer pel values from the reference picture
         // taking care with motion vectors that go off the edge of the picture


      ArcGetIntegerPelReferenceBlockFromFrame(forward_reference_picture,
         &ForwardWorkingArray,
        (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS,
        auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 + NUMBER_OF_TOP_BORDER_PELS + NUMBER_OF_BOTTOM_BORDER_PELS,
         16*auxReg.ME_MB_INDEX__MB_X_INDEX + 8 * (block_index & 1) + integer_for_hor_vector - extra_for_left_pels + NUMBER_OF_LEFT_BORDER_PELS,
         16*auxReg.ME_MB_INDEX__MB_Y_INDEX + 8 * (block_index / 2) + integer_for_ver_vector - extra_for_top_pels + NUMBER_OF_TOP_BORDER_PELS,
         sub_block_hor_size + extra_for_left_pels + extra_for_right_pels,
         sub_block_ver_size + extra_for_top_pels + extra_for_bottom_pels);


      ArcFilterH263LumaReferenceBlock(&ForwardWorkingArray, fractional_for_hor_vector, fractional_for_ver_vector,
                            sub_block_hor_size, sub_block_ver_size);




      ArcCopyToLumaPrediction(
                         h264_8x8_block_index_to_raster_index_table[block_index],
                         sub_block_hor_size, sub_block_ver_size
                         );
   }// for each block

   return true;

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcGetIntegerPelReferenceBlockFromFrame   */
/* DESCRIPTION: Copied from BT function CH264Encoder::GetIntegerPelReferenceBlockFromFrame
            This reads the given number of pels from the given
                reference picture into the given working array
                m_ForwardWorkingArray/m_BackwardWorkingArray, repeating
                pels as necessary when the vector points outside
                the picture area.
                The function firstly determines whether the target pixels
                are completely within the reference picture or whether some
                pixels are off the edge of the picture. It then runs
                optimal code for the actual case.

                The parameters are as follows:
                ptr to top left of picture data (y, u or v)
                ptr to working array
                picture size (x,y) of the component of the reference picture
                offset to top left of required block (x,y)
                size (x,y) of the required block
*/
/* RETURNS:     Void.                                                       */
/* ------------------------------------------------------------------------ */

void ArcMotionEstimationModel::ArcGetIntegerPelReferenceBlockFromFrame(PIXEL * ref_picture_data,
                                                        arc_working_array_t * working_array,
                                                        int ref_hor_size,
                                                        int ref_ver_size,
                                                        int ref_hor_offset,
                                                        int ref_ver_offset,
                                                        int block_hor_size,
                                                        int block_ver_size)
{
    int x_pos, y_pos;  // Gives the position of the pel to copy, and is within the picture area
    int x, y;
    PIXEL * source_ptr;
    int * working_array_ptr;

    bool vertical_in_picture = ((ref_ver_offset >= 0) && (ref_ver_offset + block_ver_size <= ref_ver_size));
    bool horizontal_in_picture = ((ref_hor_offset >= 0) && (ref_hor_offset + block_hor_size <= ref_hor_size));

    if (vertical_in_picture && horizontal_in_picture)
    {
        source_ptr = ref_picture_data + ref_hor_offset + ref_hor_size * ref_ver_offset;

        for (y=0; y<block_ver_size; y++)
        {
            working_array_ptr = &working_array->data[y][0];

            while (true)
            {
                working_array_ptr = &working_array->data[y][0];

                *(working_array_ptr +  0) = *(source_ptr +  0);
                *(working_array_ptr +  1) = *(source_ptr +  1);
                *(working_array_ptr +  2) = *(source_ptr +  2);
                *(working_array_ptr +  3) = *(source_ptr +  3);

                *(working_array_ptr +  4) = *(source_ptr +  4); if (block_hor_size <=  5) break;
                *(working_array_ptr +  5) = *(source_ptr +  5);
                *(working_array_ptr +  6) = *(source_ptr +  6);
                *(working_array_ptr +  7) = *(source_ptr +  7);

                *(working_array_ptr +  8) = *(source_ptr +  8); if (block_hor_size <=  9) break;
                *(working_array_ptr +  9) = *(source_ptr +  9);
                *(working_array_ptr + 10) = *(source_ptr + 10);
                *(working_array_ptr + 11) = *(source_ptr + 11);

                *(working_array_ptr + 12) = *(source_ptr + 12); if (block_hor_size <= 13) break;
                *(working_array_ptr + 13) = *(source_ptr + 13);
                *(working_array_ptr + 14) = *(source_ptr + 14);
                *(working_array_ptr + 15) = *(source_ptr + 15);

                *(working_array_ptr + 16) = *(source_ptr + 16); if (block_hor_size <= 17) break;
                *(working_array_ptr + 17) = *(source_ptr + 17);
                *(working_array_ptr + 18) = *(source_ptr + 18);
                *(working_array_ptr + 19) = *(source_ptr + 19);

                *(working_array_ptr + 20) = *(source_ptr + 20);

                break;
            }
            source_ptr += ref_hor_size;
        }
    }
    else if (vertical_in_picture)
    {
        for (x=0; x<block_hor_size; x++)
        {
            x_pos = CLIP(0, ref_hor_size - 1, ref_hor_offset + x);

            for (y=0; y<block_ver_size; y++)
            {
                y_pos = ref_hor_size * (ref_ver_offset + y);

                working_array->data[y][x] = *(ref_picture_data + x_pos + y_pos);
            }
        }
    }
    else if (horizontal_in_picture)
    {
        for (y=0; y<block_ver_size; y++)
        {
            y_pos = ref_hor_size * CLIP(0, ref_ver_size - 1, ref_ver_offset + y);

            source_ptr = ref_picture_data + ref_hor_offset + y_pos;

            working_array_ptr = &working_array->data[y][0];

            for (x=0; x<block_hor_size; x++)
            {
                *working_array_ptr++ = *source_ptr++;
            }
        }
    }
    else
    {
        for (y=0; y<block_ver_size; y++)
        {
            y_pos = ref_hor_size * CLIP(0, ref_ver_size - 1, ref_ver_offset + y);

            for (x=0; x<block_hor_size; x++)
            {
                x_pos = CLIP(0, ref_hor_size - 1, ref_hor_offset + x);

                working_array->data[y][x] = *(ref_picture_data + x_pos + y_pos);
            }
        }
    }
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcFilterLumaReferenceBlock   */
/* DESCRIPTION: Copied from BT function CH264Encoder::FilterLumaReferenceBlock
            This filters the array m_ForwardWorkingArray or
                m_BackwardWorkingArray, given by working_array, according
                to the given fractional pixel position.
                Assumes luminance and quarter pel precision on motion
                vectors.
                It then rounds and clips the result, and puts in it in the
                control structure (y_prediction_data).
                The intermediate values are retained in
                m_ForwardWorkingArray or m_BackwardWorkingArray, for a
                conceptually simple design, but one that could probably be speeded up.
                For a full description of the algorithm - read VCEG-N31.
                A summary is as follows. The diagram shows motion compensation positions

                A   d1  b1  d2   A
                e1  h1  f1  h2
                b2  g1   c  g2  b3
                e2  h3  f2  h4
                A       b4       A

                A is the integer position
                b1 and b4 are made by applying the 6 tap interpolator horizontally, then rounding and clipping
                b2 and b3 are made by applying the 6 tap interpolator vertically, then rounding and clipping
                c is made from either b1 and b4 or b2 and b3 by applying the six tap interpolator to the
                    full precision b values, then rounding and clipping.
                d1 and d2 are made by averaging the nearer A with the rounded and clipped b1
                e1 and e2 are made by averaging the nearer A with the rounded and clipped b2
                f1 and f2 are made by averaging the nearer the rounded and clipped b with the rounded and clipped c
                g1 and g2 are made by averaging the nearer the rounded and clipped b with the rounded and clipped c
                h1 is the average of the rounded and clipped b1 and b2
                h2 is the average of the rounded and clipped b1 and b3
                h3 is the average of the rounded and clipped b4 and b2
                h4 is the average of the rounded and clipped b4 and b3

                The parameters are as follows:
                fractional vector (x,y) in range 0..3
                size (x,y) of the motion compensation block (e.g. (16,8))
                ptr to data in the control structure into which the
                result is placed. The pointer points to the top left pel of
                the motion compensation block of y_prediction_data.

                This is based on CH264Decoder::FilterLumaReferenceBlock with
                the following changes.

                -   Adding to residual and subsequent clipping is replaced by
                    writing the prediction to y_prediction_data.
*/
/* RETURNS:     Void.                                                       */
/* ------------------------------------------------------------------------ */

const int quarter_pel_filter[6] = {   1,   -5,   20,   20,   -5,   1};

void ArcMotionEstimationModel::ArcFilterH264LumaReferenceBlock(arc_working_array_t * working_array,
                                            int fractional_hor_vector,
                                            int fractional_ver_vector,
                                            int block_hor_size,
                                            int block_ver_size)
{
    int x, y; // Used as block indices for resulting pels
    int *data_ptr;  // Used to point to the array m_ForwardWorkingArray or m_BackwardWorkingArray
    int input_ver_size;
    int input_hor_size;
    int half_pel_full_precision[LUMA_WORKING_ARRAY_SIZE];

    // macro_input are temporary variables used as the argument to macros
    // to avoid the need to evaluate variable inputs multiple times on expansion of the macro.
    int macro_input1, macro_input2;

    if (0 != fractional_hor_vector)
    {
        input_hor_size = block_hor_size + 5;
    }
    else
    {
        input_hor_size = block_hor_size;
    }

    if (0 != fractional_ver_vector)
    {
        input_ver_size = block_ver_size + 5;
    }
    else
    {
        input_ver_size = block_ver_size;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Integer Position
    /////////////////////////////////////////////////////////////////////////////

    if ((0 == fractional_hor_vector) && (0 == fractional_ver_vector))
    {
        // Nothing to do
    }

    /////////////////////////////////////////////////////////////////////////////
    // Pure Horizontal Filtering
    /////////////////////////////////////////////////////////////////////////////

    else if (0 == fractional_ver_vector)
    {
        if (2 == fractional_hor_vector)
        {
            // Ordinary six tap filtering
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = ( (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4)) +
                                      (*(data_ptr+5)) + 16) / 32;

                    *data_ptr = RECON_CLIP(macro_input1);
                    data_ptr++;
                }
            }
        }
        else if (1 == fractional_hor_vector)
        {
            // Make half pel as above and average with left integer pel
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = (  (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4)) +
                                      (*(data_ptr+5)) + 16) / 32;

                    *data_ptr = ( (*(data_ptr+2)) + RECON_CLIP(macro_input1) + 1) / 2;
                    data_ptr++;
                }
            }
        }
        else // (3 == fractional_hor_vector)
        {
            // Make half pel as above and average with right integer pel
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = (  (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4)) +
                                      (*(data_ptr+5)) + 16) / 32;

                    *data_ptr = ( (*(data_ptr+3)) + RECON_CLIP(macro_input1) + 1) / 2;
                    data_ptr++;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Pure Vertical Filtering
    /////////////////////////////////////////////////////////////////////////////

    else if (0 == fractional_hor_vector)
    {
        if (2 == fractional_ver_vector)
        {
            // Ordinary six tap filtering
            for (x=0; x<block_hor_size; x++)
            {
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = RECON_CLIP(macro_input1);
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
        }
        else if (1 == fractional_ver_vector)
        {
            // Make half pel as above and average with top integer pel
            for (x=0; x<block_hor_size; x++)
            {
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) + RECON_CLIP(macro_input1) + 1) / 2;
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
        }
        else // (3 == fractional_ver_vector)
        {
            // Make half pel as above and average with bottom integer pel
            for (x=0; x<block_hor_size; x++)
            {
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr)) +
                                     quarter_pel_filter[1] * (*(data_ptr+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) + RECON_CLIP(macro_input1) + 1) / 2;
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Half Pel Horizontal Filtering
    /////////////////////////////////////////////////////////////////////////////

    else if (2 == fractional_hor_vector)
    {
        // Firstly, ordinary six tap horizontal filtering, results into temporary array
        // working one column at a time
        for (x=0; x<block_hor_size; x++)
        {
            data_ptr = &(working_array->data[0][x]);

            for (y=0; y<input_ver_size; y++)
            {
                half_pel_full_precision[y] = quarter_pel_filter[0] * (*(data_ptr)) +
                                             quarter_pel_filter[1] * (*(data_ptr+1)) +
                                             quarter_pel_filter[2] * (*(data_ptr+2)) +
                                             quarter_pel_filter[3] * (*(data_ptr+3)) +
                                             quarter_pel_filter[4] * (*(data_ptr+4)) +
                                             quarter_pel_filter[5] * (*(data_ptr+5));
                data_ptr += LUMA_WORKING_ARRAY_SIZE;
            }

            // Now run the filter over the intermediate array to make the half half pel,
            // and average with the ordinary precision clipped intermediate value for the
            // quarter pel positions

            if (2 == fractional_ver_vector)
            {
                // The half half position
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * half_pel_full_precision[y] +
                                     quarter_pel_filter[1] * half_pel_full_precision[y+1] +
                                     quarter_pel_filter[2] * half_pel_full_precision[y+2] +
                                     quarter_pel_filter[3] * half_pel_full_precision[y+3] +
                                     quarter_pel_filter[4] * half_pel_full_precision[y+4] +
                                     quarter_pel_filter[5] * half_pel_full_precision[y+5] + 512) / 1024;

                    *data_ptr = RECON_CLIP(macro_input1);
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
            else if (1 == fractional_ver_vector)
            {
                // As above, but average with the above intermediate value, rounded and clipped
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = (half_pel_full_precision[y+2] + 16) / 32;
                    macro_input2 = ( quarter_pel_filter[0] * half_pel_full_precision[y] +
                                     quarter_pel_filter[1] * half_pel_full_precision[y+1] +
                                     quarter_pel_filter[2] * half_pel_full_precision[y+2] +
                                     quarter_pel_filter[3] * half_pel_full_precision[y+3] +
                                     quarter_pel_filter[4] * half_pel_full_precision[y+4] +
                                     quarter_pel_filter[5] * half_pel_full_precision[y+5] + 512) / 1024;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
            else // (3 == fractional_ver_vector)
            {
                // As above, but average with the below intermediate value, rounded and clipped
                data_ptr = &(working_array->data[0][x]);

                for (y=0; y<block_ver_size; y++)
                {
                    macro_input1 = (half_pel_full_precision[y+3] + 16) / 32;
                    macro_input2 = ( quarter_pel_filter[0] * half_pel_full_precision[y] +
                                     quarter_pel_filter[1] * half_pel_full_precision[y+1] +
                                     quarter_pel_filter[2] * half_pel_full_precision[y+2] +
                                     quarter_pel_filter[3] * half_pel_full_precision[y+3] +
                                     quarter_pel_filter[4] * half_pel_full_precision[y+4] +
                                     quarter_pel_filter[5] * half_pel_full_precision[y+5] + 512) / 1024;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;
                    data_ptr += LUMA_WORKING_ARRAY_SIZE;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Half Pel Vertical Filtering
    /////////////////////////////////////////////////////////////////////////////

    else if (2 == fractional_ver_vector)
    {
        // Firstly, ordinary six tap vertical filtering, results into temporary array
        // working one row at a time
        for (y=0; y<block_ver_size; y++)
        {
            data_ptr = &(working_array->data[y][0]);

            for (x=0; x<input_hor_size; x++)
            {
                half_pel_full_precision[x] = quarter_pel_filter[0] * (*(data_ptr)) +
                                             quarter_pel_filter[1] * (*(data_ptr+1*LUMA_WORKING_ARRAY_SIZE)) +
                                             quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                             quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                             quarter_pel_filter[4] * (*(data_ptr+4*LUMA_WORKING_ARRAY_SIZE)) +
                                             quarter_pel_filter[5] * (*(data_ptr+5*LUMA_WORKING_ARRAY_SIZE));
                data_ptr++;
            }

            // Now run the filter over the intermediate array to make the half half pel,
            // and average with the ordinary precision clipped intermediate value for the
            // quarter pel positions. Note no need for actual half half position as that has
            // been made in an earlier case

            if (1 == fractional_hor_vector)
            {
                // As above, but average with the above intermediate value, rounded and clipped
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = (half_pel_full_precision[x+2] + 16) / 32;
                    macro_input2 = ( quarter_pel_filter[0] * half_pel_full_precision[x] +
                                     quarter_pel_filter[1] * half_pel_full_precision[x+1] +
                                     quarter_pel_filter[2] * half_pel_full_precision[x+2] +
                                     quarter_pel_filter[3] * half_pel_full_precision[x+3] +
                                     quarter_pel_filter[4] * half_pel_full_precision[x+4] +
                                     quarter_pel_filter[5] * half_pel_full_precision[x+5] + 512) / 1024;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;
                    data_ptr++;
                }
            }
            else // (3 == fractional_hor_vector)
            {
                // As above, but average with the below intermediate value, rounded and clipped
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = (half_pel_full_precision[x+3] + 16) / 32;
                    macro_input2 = ( quarter_pel_filter[0] * half_pel_full_precision[x] +
                                     quarter_pel_filter[1] * half_pel_full_precision[x+1] +
                                     quarter_pel_filter[2] * half_pel_full_precision[x+2] +
                                     quarter_pel_filter[3] * half_pel_full_precision[x+3] +
                                     quarter_pel_filter[4] * half_pel_full_precision[x+4] +
                                     quarter_pel_filter[5] * half_pel_full_precision[x+5] + 512) / 1024;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;
                    data_ptr++;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Pure Quarter Pel Filtering
    /////////////////////////////////////////////////////////////////////////////

    else
    {
        // These positions are calculated as the average of two half zero positions
        // which have been scaled and clipped.

        if ((1 == fractional_hor_vector) && (1 == fractional_ver_vector))
        {
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[1] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+4)) +
                                     quarter_pel_filter[5] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+5)) + 16) / 32;

                    macro_input2 = ( quarter_pel_filter[0] * (*(data_ptr+2)) +
                                     quarter_pel_filter[1] * (*(data_ptr+2+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+2+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+2+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+2+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;

                    data_ptr++;
                }
            }
        }
        else if ((3 == fractional_hor_vector) && (1 == fractional_ver_vector))
        {
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[1] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+4)) +
                                     quarter_pel_filter[5] * (*(data_ptr+2*LUMA_WORKING_ARRAY_SIZE+5)) + 16) / 32;

                    macro_input2 = ( quarter_pel_filter[0] * (*(data_ptr+3)) +
                                     quarter_pel_filter[1] * (*(data_ptr+3+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+3+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+3+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+3+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;

                    data_ptr++;
                }
            }
        }
        else if ((1 == fractional_hor_vector) && (3 == fractional_ver_vector))
        {
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[1] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+4)) +
                                     quarter_pel_filter[5] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+5)) + 16) / 32;

                    macro_input2 = ( quarter_pel_filter[0] * (*(data_ptr+2)) +
                                     quarter_pel_filter[1] * (*(data_ptr+2+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+2+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+2+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+2+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+2+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;

                    data_ptr++;
                }
            }
        }
        else // ((3 == fractional_hor_vector) && (3 == fractional_ver_vector))
        {
            for (y=0; y<block_ver_size; y++)
            {
                data_ptr = &(working_array->data[y][0]);

                for (x=0; x<block_hor_size; x++)
                {
                    macro_input1 = ( quarter_pel_filter[0] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[1] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+1)) +
                                     quarter_pel_filter[2] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+2)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+3)) +
                                     quarter_pel_filter[4] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+4)) +
                                     quarter_pel_filter[5] * (*(data_ptr+3*LUMA_WORKING_ARRAY_SIZE+5)) + 16) / 32;

                    macro_input2 = ( quarter_pel_filter[0] * (*(data_ptr+3)) +
                                     quarter_pel_filter[1] * (*(data_ptr+3+LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[2] * (*(data_ptr+3+2*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[3] * (*(data_ptr+3+3*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[4] * (*(data_ptr+3+4*LUMA_WORKING_ARRAY_SIZE)) +
                                     quarter_pel_filter[5] * (*(data_ptr+3+5*LUMA_WORKING_ARRAY_SIZE)) + 16) / 32;

                    *data_ptr = ( RECON_CLIP(macro_input1) + RECON_CLIP(macro_input2) + 1) / 2;

                    data_ptr++;
                }
            }
        }
    }
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcFilterLumaReferenceBlock       */
/* DESCRIPTION: This is a simple bilinear filter (the big circles are
            the original samples and the small ones are the interpolated
            samples)

            A Oa   ob   O B

              oc   od

            C O         O D

            a = A;
            b = (A + B + 1 - rounding) >> 1;
            c = (A + C + 1 - rounding) >> 1;
            d = (A + B + C + D + 2 - rounding) >> 2;

            where "rounding" is a parameter defined in the ME_CONTROL1
            aux register.

   RETURNS:     Void                                                        */
/* ------------------------------------------------------------------------ */

void ArcMotionEstimationModel::ArcFilterH263LumaReferenceBlock(arc_working_array_t * working_array,
                                 int fractional_hor_vector,
                                 int fractional_ver_vector,
                                 int block_hor_size,
                                 int block_ver_size)
{
   int x, y; // Used as block indices for resulting pels
   int *data_ptr;  // Used to point to the array m_ForwardWorkingArray or m_BackwardWorkingArray
//  int input_ver_size;
//  int input_hor_size;
   //int half_pel_full_precision[LUMA_WORKING_ARRAY_SIZE];

   // macro_input are temporary variables used as the argument to macros
   // to avoid the need to evaluate variable inputs multiple times on expansion of the macro.
   int macro_input1;

   int roundCtrl = auxReg.ME_CONTROL1__ROUND_CTRL;

   /*
   if (0 != fractional_hor_vector)
   {
      input_hor_size = block_hor_size + 1;
   }
   else
   {
      input_hor_size = block_hor_size;
   }

   if (0 != fractional_ver_vector)
   {
      input_ver_size = block_ver_size + 1;
   }
   else
   {
      input_ver_size = block_ver_size;
   }
   */

   /////////////////////////////////////////////////////////////////////////////
   // Integer Position
   /////////////////////////////////////////////////////////////////////////////

   if ((0 == fractional_hor_vector) && (0 == fractional_ver_vector))
   {
      // Nothing to do
   }

   /////////////////////////////////////////////////////////////////////////////
   // Pure Horizontal Filtering
   /////////////////////////////////////////////////////////////////////////////

   else if (0 == fractional_ver_vector)
   {
      if (1 == fractional_hor_vector)
      {
         // Ordinary 2 tap filtering
         for (y=0; y<block_ver_size; y++)
         {
            data_ptr = &(working_array->data[y][0]);

            for (x=0; x<block_hor_size; x++)
            {
               macro_input1 = ( (*(data_ptr)) + (*(data_ptr+1)) + 1 - roundCtrl) / 2;

               *data_ptr = RECON_CLIP(macro_input1);
               data_ptr++;
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // Pure Vertical Filtering
   /////////////////////////////////////////////////////////////////////////////

   else if (0 == fractional_hor_vector)
   {
      if (1 == fractional_ver_vector)
      {
         // Ordinary two tap filtering
         for (x=0; x<block_hor_size; x++)
         {
            data_ptr = &(working_array->data[0][x]);

            for (y=0; y<block_ver_size; y++)
            {
               macro_input1 = ((*(data_ptr)) + (*(data_ptr+LUMA_WORKING_ARRAY_SIZE)) + 1 - roundCtrl) / 2;

               *data_ptr = RECON_CLIP(macro_input1);
               data_ptr += LUMA_WORKING_ARRAY_SIZE;
            }
         }
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   // Pure Half Pel Filtering
   /////////////////////////////////////////////////////////////////////////////

   else
   {
      // These positions are calculated as the average of two half zero positions
      // which have been scaled and clipped.

      for (y=0; y<block_ver_size; y++)
      {
         data_ptr = &(working_array->data[y][0]);

         for (x=0; x<block_hor_size; x++)
         {
            macro_input1 = ( (*(data_ptr)) +
                         (*(data_ptr + 1)) +
                         (*(data_ptr + LUMA_WORKING_ARRAY_SIZE)) +
                         (*(data_ptr + LUMA_WORKING_ARRAY_SIZE + 1)) +
                         2 - roundCtrl ) / 4;


            *data_ptr = RECON_CLIP(macro_input1);

            data_ptr++;
         }
      }

   }
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcCopyToLumaPrediction */
/* DESCRIPTION: Copied from BT function CH264Encoder::CopyToLumaPrediction
            This copies the results of FilterLuma to the
                y prediction data in the control structure.

                The parameters are as follows:
                size (x,y) of the motion compensation block (e.g. (16,8))
                ptr to the prediction data in the control structure to which the
                prediction is added. The pointer points to the prediction
                data for the top left pel of the motion compensation block.
                The fields component remapped_list0_index,
                remapped_list1_index and control_mb_aff are used in the
                case of weighted prediction.                                */
/* RETURNS:     Void.                                                       */
/* ------------------------------------------------------------------------ */

void ArcMotionEstimationModel::ArcCopyToLumaPrediction(
                                        int block_index, int block_hor_size, int block_ver_size)
{
    int * forward_data_ptr;


    int * top_left_of_control_block
        = (int *)(y_prediction_data + top_left_of_luma_4x4_block_raster_order_table[block_index]);


    /////////////////////////////////////////////////////////////////////////////
    // Add to prediction
    /////////////////////////////////////////////////////////////////////////////

    {
        {
            // No weighting
            for (int y=0; y<block_ver_size; y++)
            {
                forward_data_ptr = &ForwardWorkingArray.data[y][0];

                for (int x=0; x < block_hor_size; x++)
                {
                    *top_left_of_control_block = *forward_data_ptr;

                    top_left_of_control_block++;
                    forward_data_ptr++;
                }

                // Luminance block has width 16
                top_left_of_control_block += 16 - block_hor_size;
            }
        }
    }

}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcMakeInterPrediction8 */
/* DESCRIPTION: Copied from BT function CH264Encoder::MakeInterPrediction
            Makes Inter prediction for the current macroblock, putting
                the result into y_prediction_data if make_skipped_prediction
                is false, and into m_skip_mb_y_prediction_data otherwise.
                The term block is used to refer to a 16x16, 16x8, 8x16 or
                8x8 area, and the term sub-block for the sub-division of a
                block.
                It was originally based on CH264Decoder::InterRecon8x8 with
                the following differences (and many more!):

                -   The output is the prediction in y_prediction_data rather
                    than the clipped sum of residual and prediction in y_data.
*/
/* RETURNS:     True if successful else false.                              */
/* ------------------------------------------------------------------------ */

bool ArcMotionEstimationModel::ArcMakeH264InterPrediction8(int block, int block_shape)
{
    //h264_block_shape_type_t block_shape;
//  int number_of_blocks;


    //block_shape = H264_8X8;
    //number_of_blocks = 4;
    int block_number = block;

    // Loop for each block that needs to be motion compensated
    //for (int block_number=0; block_number < number_of_blocks; block_number++)
    //{
        // Map the partition mode into a block shape and number of sub-blocks.
        h264_block_shape_type_t sub_block_shape;
        int number_of_sub_blocks;
        int first_block_index_4x4; // Index of the 4x4 block at top left of block, 0, 2, 8, 10
        int block_index_8x8;  // Used to index the partition info etc, 0..3
        int sub_block_hor_size;
        int sub_block_ver_size;




        sub_block_shape = (h264_block_shape_type_t)block_shape;
        number_of_sub_blocks = 1;

        first_block_index_4x4 = ((block_number & 1) << 1) + ((block_number & 2) << 2);
        block_index_8x8 = block_number;

        sub_block_hor_size = h264_block_width_table[sub_block_shape];
        sub_block_ver_size = h264_block_height_table[sub_block_shape];


        pel_t * forward_reference_picture = 0;

        {
         forward_reference_picture = (pel_t *) refYWthBrdrPels;

            if (0 == forward_reference_picture)
            {
                // Really should never occur. Give up as it seems to be a lost cause!
                assert(false);
                return false;
            }
        }

        // Loop for each sub-block that needs to be motion compensated
        int sub_block_number=0;
            // block_index indicates the index of the top left 4x4 block of the motion compensation block
            int block_index = first_block_index_4x4 ;//+ h264_block_offset_table[sub_block_shape][sub_block_number];

            // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
            // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
            int integer_for_hor_vector;
            int integer_for_ver_vector;
            int fractional_for_hor_vector;
            int fractional_for_ver_vector;


            // To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed
            int extra_for_left_pels;
            int extra_for_right_pels;
            int extra_for_top_pels;
            int extra_for_bottom_pels;


                //////////////////////////////////////////////////////////////////////////////////////////////
                // LUMINANCE MOTION COMPENSATION
                //////////////////////////////////////////////////////////////////////////////////////////////

                // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
            //  integer_for_hor_vector = mv_for_hor[block_index] >> 2;
            //  integer_for_ver_vector = mv_for_ver[block_index] >> 2;

            //  fractional_for_hor_vector = mv_for_hor[block_index] - 4*integer_for_hor_vector;
            //  fractional_for_ver_vector = mv_for_ver[block_index] - 4*integer_for_ver_vector;

                integer_for_hor_vector = mv_for_hor[first_block_index_4x4] >> 2;
                integer_for_ver_vector = mv_for_ver[first_block_index_4x4] >> 2;

                fractional_for_hor_vector = mv_for_hor[first_block_index_4x4] - 4*integer_for_hor_vector;
                fractional_for_ver_vector = mv_for_ver[first_block_index_4x4] - 4*integer_for_ver_vector;


                    // Calculate the extra pels needed
                    if ((0 == fractional_for_hor_vector) && (0 == fractional_for_ver_vector))
                    {
                        // Integer vector
                        extra_for_left_pels = 0;
                        extra_for_right_pels = 0;
                        extra_for_top_pels = 0;
                        extra_for_bottom_pels = 0;
                    }
                    else
                    {
                        if (0 == fractional_for_hor_vector)
                        {
                            extra_for_left_pels = 0;
                            extra_for_right_pels = 0;
                        }
                        else
                        {
                            extra_for_left_pels = 2;
                            extra_for_right_pels = 3;
                        }

                        if (0 == fractional_for_ver_vector)
                        {
                            extra_for_top_pels = 0;
                            extra_for_bottom_pels = 0;
                        }
                        else
                        {
                            extra_for_top_pels = 2;
                            extra_for_bottom_pels = 3;
                        }
                    }

                    // Get the integer pel values from the reference picture
                    // taking care with motion vectors that go off the edge of the picture

                        {
                            ArcGetIntegerPelReferenceBlockFromFrame(forward_reference_picture,
                                &ForwardWorkingArray,
                                (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS,
                                auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 + NUMBER_OF_TOP_BORDER_PELS + NUMBER_OF_BOTTOM_BORDER_PELS,
                                16*auxReg.ME_MB_INDEX__MB_X_INDEX + 4 * (block_index & 3) + integer_for_hor_vector - extra_for_left_pels + NUMBER_OF_LEFT_BORDER_PELS,
                                16*auxReg.ME_MB_INDEX__MB_Y_INDEX + 4 * (block_index / 4) + integer_for_ver_vector - extra_for_top_pels + NUMBER_OF_TOP_BORDER_PELS,
                                sub_block_hor_size + extra_for_left_pels + extra_for_right_pels,
                                sub_block_ver_size + extra_for_top_pels + extra_for_bottom_pels);
                        }

               ArcFilterH264LumaReferenceBlock(&ForwardWorkingArray, fractional_for_hor_vector, fractional_for_ver_vector,
                                             sub_block_hor_size, sub_block_ver_size);





                ArcCopyToLumaPrediction(
                                         block_index,
                                         sub_block_hor_size, sub_block_ver_size
                                         );








    return true;
}

bool ArcMotionEstimationModel::ArcMakeH263InterPrediction8(int block)
{
   h264_block_shape_type_t block_shape;
   int number_of_blocks;


   block_shape = H264_8X8;
   number_of_blocks = 4;
   int block_number = block;

   // Loop for each block that needs to be motion compensated
   //for (int block_number=0; block_number < number_of_blocks; block_number++)
   //{
      // Map the partition mode into a block shape and number of sub-blocks.
      h264_block_shape_type_t sub_block_shape;
      int number_of_sub_blocks;
//      int first_block_index_4x4; // Index of the 4x4 block at top left of block, 0, 2, 8, 10
//      int block_index_8x8;  // Used to index the partition info etc, 0..3
      int sub_block_hor_size;
      int sub_block_ver_size;




      sub_block_shape = H264_8X8;
      number_of_sub_blocks = 1;

//      first_block_index_4x4 = ((block_number & 1) << 1) + ((block_number & 2) << 2);
//      block_index_8x8 = block_number;

      sub_block_hor_size = 8;
      sub_block_ver_size = 8;


      pel_t * forward_reference_picture = 0;

      {
         forward_reference_picture = (pel_t *) refYWthBrdrPels;

         if (0 == forward_reference_picture)
         {
            // Really should never occur. Give up as it seems to be a lost cause!
            assert(false);
            return false;
         }
      }

      // Loop for each sub-block that needs to be motion compensated
      int sub_block_number=0;
         // block_index indicates the index of the top left 4x4 block of the motion compensation block
//          int block_index = first_block_index_4x4 + h264_block_offset_table[sub_block_shape][sub_block_number];
         int block_index = block_number;

         // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
         // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
         int integer_for_hor_vector;
         int integer_for_ver_vector;
         int fractional_for_hor_vector;
         int fractional_for_ver_vector;


         // To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed
         int extra_for_left_pels;
         int extra_for_right_pels;
         int extra_for_top_pels;
         int extra_for_bottom_pels;


            //////////////////////////////////////////////////////////////////////////////////////////////
            // LUMINANCE MOTION COMPENSATION
            //////////////////////////////////////////////////////////////////////////////////////////////

               // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
               integer_for_hor_vector = mv_for_hor[h264_8x8_block_index_to_raster_index_table[block_index]] >> 1;
               integer_for_ver_vector = mv_for_ver[h264_8x8_block_index_to_raster_index_table[block_index]] >> 1;

               fractional_for_hor_vector = mv_for_hor[h264_8x8_block_index_to_raster_index_table[block_index]] - 2*integer_for_hor_vector;
               fractional_for_ver_vector = mv_for_ver[h264_8x8_block_index_to_raster_index_table[block_index]] - 2*integer_for_ver_vector;


               // Calculate the extra pels needed
               if ((0 == fractional_for_hor_vector) && (0 == fractional_for_ver_vector))
               {
                  // Integer vector
                  extra_for_left_pels = 0;
                  extra_for_right_pels = 0;
                  extra_for_top_pels = 0;
                  extra_for_bottom_pels = 0;
               }
               else
               {
                  if (0 == fractional_for_hor_vector)
                  {
                     extra_for_left_pels = 0;
                     extra_for_right_pels = 0;
                  }
                  else
                  {
                     extra_for_left_pels = 0;
                     extra_for_right_pels = 1;
                  }

                  if (0 == fractional_for_ver_vector)
                  {
                     extra_for_top_pels = 0;
                     extra_for_bottom_pels = 0;
                  }
                  else
                  {
                     extra_for_top_pels = 0;
                     extra_for_bottom_pels = 1;
                  }
               }

               // Get the integer pel values from the reference picture
               // taking care with motion vectors that go off the edge of the picture

            ArcGetIntegerPelReferenceBlockFromFrame(forward_reference_picture,
               &ForwardWorkingArray,
               (auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS,
               auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION * 16 + NUMBER_OF_TOP_BORDER_PELS + NUMBER_OF_BOTTOM_BORDER_PELS,
               16*auxReg.ME_MB_INDEX__MB_X_INDEX + 8 * (block_index & 1) + integer_for_hor_vector - extra_for_left_pels + NUMBER_OF_LEFT_BORDER_PELS,
               16*auxReg.ME_MB_INDEX__MB_Y_INDEX + 8 * (block_index / 2) + integer_for_ver_vector - extra_for_top_pels + NUMBER_OF_TOP_BORDER_PELS,
               sub_block_hor_size + extra_for_left_pels + extra_for_right_pels,
               sub_block_ver_size + extra_for_top_pels + extra_for_bottom_pels);


            ArcFilterH263LumaReferenceBlock(&ForwardWorkingArray, fractional_for_hor_vector, fractional_for_ver_vector,
                            sub_block_hor_size, sub_block_ver_size);

            ArcCopyToLumaPrediction(
                               h264_8x8_block_index_to_raster_index_table[block_index],
                               sub_block_hor_size, sub_block_ver_size
                               );

   return true;
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    ArcMotionEstimationModel::ArcCalculateSSD   */
/* DESCRIPTION: Copied from BT function CH264Encoder::CalculateSSD
            Calculates the sum of square differences between the
                current and the previous.                                   */
/* ------------------------------------------------------------------------ */

unsigned int ArcMotionEstimationModel::ArcCalculateSSD(unsigned char* current, unsigned char * previous,
                                        unsigned int current_yinc, unsigned int previous_yinc, int height)
{
    unsigned char *cpel = current;
    unsigned char *ppel = previous;
    unsigned int local_ssd = 0;

    int y = 0;

   unsigned char cpel_array[16];
    while (y<height)
    {
      MeMemoryRead(cpel_array, cpel, 16);

        local_ssd += ABS_TABLE(*(cpel_array     ) - *(ppel     ));
        local_ssd += ABS_TABLE(*(cpel_array +  1) - *(ppel +  1));
        local_ssd += ABS_TABLE(*(cpel_array +  2) - *(ppel +  2));
        local_ssd += ABS_TABLE(*(cpel_array +  3) - *(ppel +  3));
        local_ssd += ABS_TABLE(*(cpel_array +  4) - *(ppel +  4));
        local_ssd += ABS_TABLE(*(cpel_array +  5) - *(ppel +  5));
        local_ssd += ABS_TABLE(*(cpel_array +  6) - *(ppel +  6));
        local_ssd += ABS_TABLE(*(cpel_array +  7) - *(ppel +  7));
        local_ssd += ABS_TABLE(*(cpel_array +  8) - *(ppel +  8));
        local_ssd += ABS_TABLE(*(cpel_array +  9) - *(ppel +  9));
        local_ssd += ABS_TABLE(*(cpel_array + 10) - *(ppel + 10));
        local_ssd += ABS_TABLE(*(cpel_array + 11) - *(ppel + 11));
        local_ssd += ABS_TABLE(*(cpel_array + 12) - *(ppel + 12));
        local_ssd += ABS_TABLE(*(cpel_array + 13) - *(ppel + 13));
        local_ssd += ABS_TABLE(*(cpel_array + 14) - *(ppel + 14));
        local_ssd += ABS_TABLE(*(cpel_array + 15) - *(ppel + 15));

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}
unsigned int ArcMotionEstimationModel::ArcCalculateSSD(unsigned char* current, int * previous,
                                        unsigned int current_yinc, unsigned int previous_yinc, int height)
{
    unsigned char *cpel = current;
    int *ppel = previous;
    unsigned int local_ssd = 0;

    int y = 0;

   unsigned char cpel_array[16];
    while (y<height)
    {
      MeMemoryRead(cpel_array, cpel, 16);

        local_ssd += ABS_TABLE(*(cpel_array     ) - *(ppel     ));
        local_ssd += ABS_TABLE(*(cpel_array +  1) - *(ppel +  1));
        local_ssd += ABS_TABLE(*(cpel_array +  2) - *(ppel +  2));
        local_ssd += ABS_TABLE(*(cpel_array +  3) - *(ppel +  3));
        local_ssd += ABS_TABLE(*(cpel_array +  4) - *(ppel +  4));
        local_ssd += ABS_TABLE(*(cpel_array +  5) - *(ppel +  5));
        local_ssd += ABS_TABLE(*(cpel_array +  6) - *(ppel +  6));
        local_ssd += ABS_TABLE(*(cpel_array +  7) - *(ppel +  7));
        local_ssd += ABS_TABLE(*(cpel_array +  8) - *(ppel +  8));
        local_ssd += ABS_TABLE(*(cpel_array +  9) - *(ppel +  9));
        local_ssd += ABS_TABLE(*(cpel_array + 10) - *(ppel + 10));
        local_ssd += ABS_TABLE(*(cpel_array + 11) - *(ppel + 11));
        local_ssd += ABS_TABLE(*(cpel_array + 12) - *(ppel + 12));
        local_ssd += ABS_TABLE(*(cpel_array + 13) - *(ppel + 13));
        local_ssd += ABS_TABLE(*(cpel_array + 14) - *(ppel + 14));
        local_ssd += ABS_TABLE(*(cpel_array + 15) - *(ppel + 15));

        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}

//******************************************************************************
// Name:          ArcMotionEstimationModel::CalculateBlockPattern
// Purpose:       Calculates the 8x8 SADs for the given MVs and sets the
//                blockPattern appropriately
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::CalculateBlockPattern (void)
{
    unsigned char *current;
    int current_offset, block_offset;
    int block_index;

    int mvx[4];
    int mvy[4];

    // if manual sad mode then just compare the sad against the threshold
    if (auxReg.ME_SETUP__ITER_TYPE==0x3) // manual sad mode
    {
        // zero all block pattern flags
        blockPattern[0]=0;
        blockPattern[1]=0;
        blockPattern[2]=0;
        blockPattern[3]=0;

        // compare against threshold (keep as < threshold to be consistent with all other threshold comparisons)
        if (auxReg.ME_RESULT__COST_SAD < auxReg.ME_THRESHOLD_BLOCK_PATTERN) blockPattern[auxReg.ME_CONTROL0__PAR_ID]=0;
        else blockPattern[auxReg.ME_CONTROL0__PAR_ID]=1;
    }
    else
    {

        // setup MVs for chosen mode
        if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_SKIPPED || best_inter_mtype==MBCT_H264_INTER16X16)) ||
            ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_SKIPPED || best_inter_mtype==MBCT_MPEG4_INTER16X16)))
        {
            mvx[0]=mvx[1]=mvx[2]=mvx[3]=bestHor[0][0];
            mvy[0]=mvy[1]=mvy[2]=mvy[3]=bestVer[0][0];
        }
        else if((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER16X8))
        {
            mvx[0]=mvx[1]=bestHor[1][0];
            mvy[0]=mvy[1]=bestVer[1][0];
            mvx[2]=mvx[3]=bestHor[1][1];
            mvy[2]=mvy[3]=bestVer[1][1];
        }
        else if((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER8X16))
        {
            mvx[0]=mvx[2]=bestHor[2][0];
            mvy[0]=mvy[2]=bestVer[2][0];
            mvx[1]=mvx[3]=bestHor[2][1];
            mvy[1]=mvy[3]=bestVer[2][1];
        }
        else if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER8X8)) ||
            ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_INTER8X8)))
        {
            for (int i=0; i<4; i++)
            {
                mvx[i]=bestHor[3][i];
                mvy[i]=bestVer[3][i];
            }
        }
        else
        {
            assert(false);
        }

        // for each 8x8 block
        for (int vector_count=0; vector_count<4; vector_count++)
        {
            // first set up the offsets into current frame, reference frame, 16x16 prediction data and array of motion vectors
            if (1 == vector_count)
            {
                current_offset = 8; // sub block offset into current frame
                block_offset = 8; // subblock offset into motion compensated 16x16 array
            }
            else if (2 == vector_count)
            {
                current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);
                block_offset = 128;
            }
            else if (3 == vector_count)
            {
                current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT) + 8;
                block_offset = 136;
            }
            else  // 0
            {
                current_offset = 0;
                block_offset = 0;
            }

            current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16) + current_offset;

            // get the index of the raster 4x4s for the block
            block_index = h264_8x8_block_index_to_raster_index_table[vector_count];

            // set the MV arrays so that the correct reference is generatd
            mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] = mvx[vector_count];
            mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] = mvy[vector_count];

            // generate reference data for given MV
            switch(auxReg.ME_SETUP__CODEC)
            {
            case H264:
                ArcMakeH264InterPrediction8(vector_count,H264_8X8); // goes into quadrant
                break;
            case MPEG4:
            case H263:
                ArcMakeH263InterPrediction8(vector_count);
                break;
            default:
                fprintf(stderr, "ArcMotionEstimationModel::StepSearch8x8MB - Error: Unsupported Codec\n");
                assert(false);
                break;
            }

            // calculate 8x8 sad
            unsigned int sad8x8=CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8);

            // compare against threshold
            if (sad8x8<auxReg.ME_THRESHOLD_BLOCK_PATTERN) blockPattern[vector_count]=0;
            else blockPattern[vector_count]=1;

        } // for (int vector_count=0; vector_count<4; vector_count++)
    }

    // set aux reg based on block pattern
    auxReg.ME_RESULT__BLK_PAT=(blockPattern[3] << 3) |
                              (blockPattern[2] << 2) |
                              (blockPattern[1] << 1) |
                               blockPattern[0];

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::ReadNeighbourDataFromMPC
// Purpose:       Reads various neighbour information from the mpc
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::ReadNeighbourDataFromMPC (void)
{
   cMPCcb mpcCbBuffer;
   cMPCfb mpcFbBuffer;
   u_int8 * mpcCbBytePtr=(u_int8 *) &mpcCbBuffer;
   u_int8 * mpcFbBytePtr=(u_int8 *) &mpcFbBuffer;

   // motion vectors need truncating when they are read in. the amount is
   // dependant on the codec type (h.264 uses 7 bits and mpeg4/h.263 uses 6 bits)
   int mvTrunc = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

   // used as a place holder so that the ArcGetMotionVectorPrediction function
   // can call a modified ArcGetNeighbouringLocationNonMBAFF, for calculating the
   // above right and left macroblocks
   currMB.intra=0;
   currMB.validID=1;
   currMB.valid=1;

   // left neighbour info = A
   if (auxReg.ME_MB_INDEX__MB_X_INDEX==0)
   {
      leftMB.valid=0;
      leftMB.validID=0;
   }
   else
   {
      // read MB Information and Neighbour Flags Data Table (0x00-0F) and Motion
      // Vectors Data Table (0x10-1F) from the sdm
      MeMemoryReadSDM(mpcCbBytePtr, (u_int8 *) (auxReg.ME_A_POINTER << ME_A_POINTER_SHIFT), 32);

      cMPCcb * me_a_pointer=&mpcCbBuffer;

      // populate internal flags structure based on data from mpc, remember not
      // to access fields of mpc beyond 32 bytes
      leftMB.quantValue=me_a_pointer->QuantValue;
      leftMB.validID=(me_a_pointer->CollectionID==auxReg.ME_COLLECTION_ID) ? 1 : 0;
      leftMB.valid=1;

      if (auxReg.ME_SETUP__CODEC == H264)
      {
         leftMB.code=(me_a_pointer->MBCodingType==MBCT_H264_I_PCM) ? 1 : 0;
         leftMB.intra=(me_a_pointer->MBCodingType & MBCT_H264_INTRA) ? 1 : 0;

         // DC and AC coeffs can both just come from the neighbours current
         // nonzerocoeff flags, because they will have been set according to
         // whether mb coding type is intra16x16
         leftMB.nonZeroCoeffW=(me_a_pointer->NonZeroCoeffLuma & NZC_H264_SB15) ? 1 : 0; // 15
         leftMB.nonZeroCoeffX=(me_a_pointer->NonZeroCoeffLuma & NZC_H264_SB13) ? 1 : 0; // 13
         leftMB.nonZeroCoeffY=(me_a_pointer->NonZeroCoeffLuma & NZC_H264_SB7) ? 1 : 0;  //  7
         leftMB.nonZeroCoeffZ=(me_a_pointer->NonZeroCoeffLuma & NZC_H264_SB5) ? 1 : 0;  //  5
      }
      else // MPEG4, H263
      {
         leftMB.code=(me_a_pointer->MBCodingType != MBCT_MPEG4_SKIPPED) ? 1 : 0;
         leftMB.intra=(me_a_pointer->MBCodingType == MBCT_MPEG4_INTRA) ? 1 : 0;
      }

      for (int i=0; i<4; i++)
      {
         leftMB.mv[i].mvx=((s_int8) (me_a_pointer->mv[i].mvx << mvTrunc)) >> mvTrunc;
         leftMB.mv[i].mvy=((s_int8) (me_a_pointer->mv[i].mvy << mvTrunc)) >> mvTrunc;
      }

   }

   // above neighbour info = B = D + MPC_FB_SIZE
   if (auxReg.ME_MB_INDEX__MB_Y_INDEX==0)
   {
      aboveMB.valid=0;
      aboveMB.validID=0;
   }
   else
   {
      // read MB Information and Neighbour Flags Data Table (0x00-0F) and Motion
      // Vectors Data Table (0x10-1F) from the sdm
      MeMemoryReadSDM(mpcFbBytePtr, (u_int8 *) (auxReg.ME_D_POINTER << ME_D_POINTER_SHIFT) + sizeof(cMPCfb), 32);

      cMPCfb * me_b_pointer=&mpcFbBuffer;

      // populate internal flags structure based on data from mpc, remember not
      // to access fields of mpc beyond 32 bytes
      aboveMB.quantValue=me_b_pointer->QuantValue;
      aboveMB.validID=(me_b_pointer->CollectionID==auxReg.ME_COLLECTION_ID) ? 1 : 0;
      aboveMB.valid=1;

      if (auxReg.ME_SETUP__CODEC == H264)
      {
         aboveMB.code=(me_b_pointer->MBCodingType==MBCT_H264_I_PCM) ? 1 : 0;
         aboveMB.intra=(me_b_pointer->MBCodingType & MBCT_H264_INTRA) ? 1 : 0;

         // DC and AC coeffs can both just come from the neighbours current
         // nonzerocoeff flags, because they will have been set according to
         // whether mb coding type is intra16x16
         aboveMB.nonZeroCoeffW=(me_b_pointer->NonZeroCoeffLuma & NZC_H264_SB15) ? 1 : 0; // 15
         aboveMB.nonZeroCoeffX=(me_b_pointer->NonZeroCoeffLuma & NZC_H264_SB14) ? 1 : 0; // 14
         aboveMB.nonZeroCoeffY=(me_b_pointer->NonZeroCoeffLuma & NZC_H264_SB11) ? 1 : 0; // 11
         aboveMB.nonZeroCoeffZ=(me_b_pointer->NonZeroCoeffLuma & NZC_H264_SB10) ? 1 : 0; // 10
      }
      else // MPEG4, H263
      {
         aboveMB.code=(me_b_pointer->MBCodingType != MBCT_MPEG4_SKIPPED) ? 1 : 0;
         aboveMB.intra=(me_b_pointer->MBCodingType == MBCT_MPEG4_INTRA) ? 1 : 0;
      }

      for (int i=0; i<4; i++)
      {
         aboveMB.mv[i].mvx=((s_int8) (me_b_pointer->mv[i].mvx << mvTrunc)) >> mvTrunc;
         aboveMB.mv[i].mvy=((s_int8) (me_b_pointer->mv[i].mvy << mvTrunc)) >> mvTrunc;
      }

   }

   // above right neighbour info = C = D + (2 * MPC_FB_SIZE)
   if (auxReg.ME_MB_INDEX__MB_Y_INDEX==0 || auxReg.ME_MB_INDEX__MB_X_INDEX==auxReg.ME_MB_DIMENSION__MB_X_DIMENSION-1)
   {
      aboveRightMB.valid=0;
      aboveRightMB.validID=0;
   }
   else
   {
      // read MB Information and Neighbour Flags Data Table (0x00-0F) and Motion
      // Vectors Data Table (0x10-1F) from the sdm
      MeMemoryReadSDM(mpcFbBytePtr, (u_int8 *) (auxReg.ME_D_POINTER << ME_D_POINTER_SHIFT) + (sizeof(cMPCfb) * 2), 32);

      cMPCfb * me_c_pointer=&mpcFbBuffer;

      // populate internal flags structure based on data from mpc, remember not
      // to access fields of mpc beyond 32 bytes
      aboveRightMB.quantValue=me_c_pointer->QuantValue;
      aboveRightMB.validID=(me_c_pointer->CollectionID==auxReg.ME_COLLECTION_ID) ? 1 : 0;
      aboveRightMB.valid=1;

      if (auxReg.ME_SETUP__CODEC == H264)
      {
         aboveRightMB.code=(me_c_pointer->MBCodingType==MBCT_H264_I_PCM) ? 1 : 0;
         aboveRightMB.intra=(me_c_pointer->MBCodingType & MBCT_H264_INTRA) ? 1 : 0;

         // DC and AC coeffs can both just come from the neighbours current
         // nonzerocoeff flags, because they will have been set according to
         // whether mb coding type is intra16x16
         aboveRightMB.nonZeroCoeffW=(me_c_pointer->NonZeroCoeffLuma & NZC_H264_SB15) ? 1 : 0; // 15
         aboveRightMB.nonZeroCoeffX=(me_c_pointer->NonZeroCoeffLuma & NZC_H264_SB14) ? 1 : 0; // 14
         aboveRightMB.nonZeroCoeffY=(me_c_pointer->NonZeroCoeffLuma & NZC_H264_SB11) ? 1 : 0; // 11
         aboveRightMB.nonZeroCoeffZ=(me_c_pointer->NonZeroCoeffLuma & NZC_H264_SB10) ? 1 : 0; // 10
      }
      else
      {
         aboveRightMB.code=(me_c_pointer->MBCodingType != MBCT_MPEG4_SKIPPED) ? 1 : 0;
         aboveRightMB.intra=(me_c_pointer->MBCodingType == MBCT_MPEG4_INTRA) ? 1 : 0;
      }

      for (int i=0; i<4; i++)
      {
         aboveRightMB.mv[i].mvx=((s_int8) (me_c_pointer->mv[i].mvx << mvTrunc)) >> mvTrunc;
         aboveRightMB.mv[i].mvy=((s_int8) (me_c_pointer->mv[i].mvy << mvTrunc)) >> mvTrunc;
      }

   }

   // above left neighbour info = D
   if (auxReg.ME_MB_INDEX__MB_Y_INDEX==0 || auxReg.ME_MB_INDEX__MB_X_INDEX==0)
   {
      aboveLeftMB.valid=0;
      aboveLeftMB.validID=0;
   }
   else
   {
      // read MB Information and Neighbour Flags Data Table (0x00-0F) and Motion
      // Vectors Data Table (0x10-1F) from the sdm
      MeMemoryReadSDM(mpcFbBytePtr, (u_int8 *) (auxReg.ME_D_POINTER << ME_D_POINTER_SHIFT), 32);

      cMPCfb * me_d_pointer=&mpcFbBuffer;

      // populate internal flags structure based on data from mpc, remember not
      // to access fields of mpc beyond 32 bytes
      aboveLeftMB.quantValue=me_d_pointer->QuantValue;
      aboveLeftMB.validID=(me_d_pointer->CollectionID==auxReg.ME_COLLECTION_ID) ? 1 : 0;
      aboveLeftMB.valid=1;

      if (auxReg.ME_SETUP__CODEC == H264)
      {
         aboveLeftMB.code=(me_d_pointer->MBCodingType==MBCT_H264_I_PCM) ? 1 : 0;
         aboveLeftMB.intra=(me_d_pointer->MBCodingType & MBCT_H264_INTRA) ? 1 : 0;

         // DC and AC coeffs can both just come from the neighbours current
         // nonzerocoeff flags, because they will have been set according to
         // whether mb coding type is intra16x16
         aboveLeftMB.nonZeroCoeffW=(me_d_pointer->NonZeroCoeffLuma & NZC_H264_SB15) ? 1 : 0; // 15
         aboveLeftMB.nonZeroCoeffX=(me_d_pointer->NonZeroCoeffLuma & NZC_H264_SB14) ? 1 : 0; // 14
         aboveLeftMB.nonZeroCoeffY=(me_d_pointer->NonZeroCoeffLuma & NZC_H264_SB11) ? 1 : 0; // 11
         aboveLeftMB.nonZeroCoeffZ=(me_d_pointer->NonZeroCoeffLuma & NZC_H264_SB10) ? 1 : 0; // 10
      }
      else
      {
         aboveLeftMB.code=(me_d_pointer->MBCodingType != MBCT_MPEG4_SKIPPED) ? 1 : 0;
         aboveLeftMB.intra=(me_d_pointer->MBCodingType == MBCT_MPEG4_INTRA) ? 1 : 0;
      }

      for (int i=0; i<4; i++)
      {
         aboveLeftMB.mv[i].mvx=((s_int8) (me_d_pointer->mv[i].mvx << mvTrunc)) >> mvTrunc;
         aboveLeftMB.mv[i].mvy=((s_int8) (me_d_pointer->mv[i].mvy << mvTrunc)) >> mvTrunc;
      }

   }

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::WriteNeighbourDataToMPC
// Purpose:       Wites the various pieces of neighbour data and flags to the
//                mpc data structure
// Arguments:     mbCodingType = type of macroblock prediction choosen
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::WriteNeighbourDataToMPC (u_int8 mbCodingType)
{
   cMPCcb mpcBuffer;
   u_int8 * mpcBytePtr=(u_int8 *) &mpcBuffer;
   cMPCcb * me_curr_pointer=&mpcBuffer;

   // read current values of MB Information and Neighbour Flags Data Table (0x00-0F) from sdm
   MeMemoryReadSDM(mpcBytePtr, (u_int8 *) (auxReg.ME_CURRENT_POINTER << ME_CURRENT_POINTER_SHIFT), 16);

   // write current macroblocks info
   me_curr_pointer->CollectionID=auxReg.ME_COLLECTION_ID;
   me_curr_pointer->MBCodingType=mbCodingType;

   // write left neighbour info
   me_curr_pointer->LeftMB.QuantValue=leftMB.quantValue;

   if (auxReg.ME_SETUP__CODEC == H264)
   me_curr_pointer->LeftMB.Flags=((leftMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                 ((leftMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                 ((leftMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                 ((leftMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT) |
                                 ((leftMB.nonZeroCoeffW & MBNF_NONZERO_COEFF15_MASK) << MBNF_NONZERO_COEFF15_SHIFT) |
                                 ((leftMB.nonZeroCoeffX & MBNF_NONZERO_COEFF13_MASK) << MBNF_NONZERO_COEFF13_SHIFT) |
                                 ((leftMB.nonZeroCoeffY & MBNF_NONZERO_COEFF7_MASK) << MBNF_NONZERO_COEFF7_SHIFT) |
                                 ((leftMB.nonZeroCoeffZ & MBNF_NONZERO_COEFF5_MASK) << MBNF_NONZERO_COEFF5_SHIFT);
   else
      me_curr_pointer->LeftMB.Flags=((leftMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                    ((leftMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                    ((leftMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                    ((leftMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT);

   // write above neighbour info
   me_curr_pointer->AboveMB.QuantValue=aboveMB.quantValue;

   if (auxReg.ME_SETUP__CODEC == H264)
   me_curr_pointer->AboveMB.Flags=((aboveMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                  ((aboveMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                  ((aboveMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                  ((aboveMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT) |
                                  ((aboveMB.nonZeroCoeffW & MBNF_NONZERO_COEFF15_MASK) << MBNF_NONZERO_COEFF15_SHIFT) |
                                  ((aboveMB.nonZeroCoeffX & MBNF_NONZERO_COEFF14_MASK) << MBNF_NONZERO_COEFF14_SHIFT) |
                                  ((aboveMB.nonZeroCoeffY & MBNF_NONZERO_COEFF11_MASK) << MBNF_NONZERO_COEFF11_SHIFT) |
                                  ((aboveMB.nonZeroCoeffZ & MBNF_NONZERO_COEFF10_MASK) << MBNF_NONZERO_COEFF10_SHIFT);
   else
      me_curr_pointer->AboveMB.Flags=((aboveMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                     ((aboveMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                     ((aboveMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                     ((aboveMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT);

   // write above right neighbour info

   me_curr_pointer->AboveRightMB.QuantValue=aboveRightMB.quantValue;
   if (auxReg.ME_SETUP__CODEC == H264)
   me_curr_pointer->AboveRightMB.Flags=((aboveRightMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                       ((aboveRightMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                       ((aboveRightMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                       ((aboveRightMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT) |
                                       ((aboveRightMB.nonZeroCoeffW & MBNF_NONZERO_COEFF15_MASK) << MBNF_NONZERO_COEFF15_SHIFT) |
                                       ((aboveRightMB.nonZeroCoeffX & MBNF_NONZERO_COEFF14_MASK) << MBNF_NONZERO_COEFF14_SHIFT) |
                                       ((aboveRightMB.nonZeroCoeffY & MBNF_NONZERO_COEFF11_MASK) << MBNF_NONZERO_COEFF11_SHIFT) |
                                       ((aboveRightMB.nonZeroCoeffZ & MBNF_NONZERO_COEFF10_MASK) << MBNF_NONZERO_COEFF10_SHIFT);
   else
      me_curr_pointer->AboveRightMB.Flags=((aboveRightMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                          ((aboveRightMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                          ((aboveRightMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                          ((aboveRightMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT);

   // write above left neighbour info

   me_curr_pointer->AboveLeftMB.QuantValue=aboveLeftMB.quantValue;
   if (auxReg.ME_SETUP__CODEC == H264)
   me_curr_pointer->AboveLeftMB.Flags=((aboveLeftMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                      ((aboveLeftMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                      ((aboveLeftMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                      ((aboveLeftMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT) |
                                      ((aboveLeftMB.nonZeroCoeffW & MBNF_NONZERO_COEFF15_MASK) << MBNF_NONZERO_COEFF15_SHIFT) |
                                      ((aboveLeftMB.nonZeroCoeffX & MBNF_NONZERO_COEFF14_MASK) << MBNF_NONZERO_COEFF14_SHIFT) |
                                      ((aboveLeftMB.nonZeroCoeffY & MBNF_NONZERO_COEFF11_MASK) << MBNF_NONZERO_COEFF11_SHIFT) |
                                      ((aboveLeftMB.nonZeroCoeffZ & MBNF_NONZERO_COEFF10_MASK) << MBNF_NONZERO_COEFF10_SHIFT);
   else
      me_curr_pointer->AboveLeftMB.Flags=((aboveLeftMB.code & MBNF_CODE_MASK) << MBNF_CODE_SHIFT) |
                                         ((aboveLeftMB.intra & MBNF_INTRA_MASK) << MBNF_INTRA_SHIFT) |
                                         ((aboveLeftMB.validID & MBNF_VALID_ID_MASK) << MBNF_VALID_ID_SHIFT) |
                                         ((aboveLeftMB.valid & MBNF_VALID_MASK) << MBNF_VALID_SHIFT);


   // write MB Information and Neighbour Flags Data Table (0x00-0F) to sdm
   MeMemoryWriteSDM((u_int8 *) (auxReg.ME_CURRENT_POINTER << ME_CURRENT_POINTER_SHIFT), mpcBytePtr, 16);

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::WriteResultsToMPC
// Purpose:       Update the various data tables in the mpc data structure
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::WriteResultsToMPC (void)
{
   // update collection id, macroblock coding type and neighbour info
   WriteNeighbourDataToMPC(best_inter_mtype);


   // write motion vectors and predictors
   cMPCcb mpcBuffer;
   u_int8 * mpcBytePtr=(u_int8 *) &mpcBuffer;
   cMPCcb * me_curr_pointer=&mpcBuffer;

   if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_SKIPPED)) ||
       ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_SKIPPED)))
   {
      // update motion vector data table
      me_curr_pointer->mv[0].mvx=(s_int8) bestHor[0][0]; // = PSliceSkippedHor
      me_curr_pointer->mv[0].mvy=(s_int8) bestVer[0][0]; // = PSliceSkippedVer
      me_curr_pointer->mv[1].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[1].mvy=(s_int8) bestVer[0][0];
      me_curr_pointer->mv[2].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[2].mvy=(s_int8) bestVer[0][0];
      me_curr_pointer->mv[3].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[3].mvy=(s_int8) bestVer[0][0];

      // skipped mode doesn't have a predictor but if chroma veto causes a
      // switch to 16x16 then we will need the appropriate predictor
      me_curr_pointer->mvp[0].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[0].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[1].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[1].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[2].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[2].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[3].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[3].mvy=(s_int8) predP16x16VecY;

      // update aux regs
      auxReg.ME_MB_P0__MV_P0_X=(s_int8) bestHor[0][0]; // = PSliceSkippedHor
      auxReg.ME_MB_P0__MV_P0_Y=(s_int8) bestVer[0][0]; // = PSliceSkippedVer
      auxReg.ME_MB_P0__MVP_P0_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P0__MVP_P0_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P1__MV_P1_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P1__MV_P1_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P1__MVP_P1_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P1__MVP_P1_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P2__MV_P2_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P2__MV_P2_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P2__MVP_P2_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P2__MVP_P2_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P3__MV_P3_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P3__MV_P3_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P3__MVP_P3_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P3__MVP_P3_Y=(s_int8) predP16x16VecY;
   }
   else if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER16X16)) ||
            ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_INTER16X16)))
   {
      // update motion vector data table
      me_curr_pointer->mv[0].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[0].mvy=(s_int8) bestVer[0][0];
      me_curr_pointer->mv[1].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[1].mvy=(s_int8) bestVer[0][0];
      me_curr_pointer->mv[2].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[2].mvy=(s_int8) bestVer[0][0];
      me_curr_pointer->mv[3].mvx=(s_int8) bestHor[0][0];
      me_curr_pointer->mv[3].mvy=(s_int8) bestVer[0][0];

      me_curr_pointer->mvp[0].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[0].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[1].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[1].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[2].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[2].mvy=(s_int8) predP16x16VecY;
      me_curr_pointer->mvp[3].mvx=(s_int8) predP16x16VecX;
      me_curr_pointer->mvp[3].mvy=(s_int8) predP16x16VecY;

      // update aux regs
      auxReg.ME_MB_P0__MV_P0_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P0__MV_P0_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P0__MVP_P0_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P0__MVP_P0_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P1__MV_P1_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P1__MV_P1_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P1__MVP_P1_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P1__MVP_P1_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P2__MV_P2_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P2__MV_P2_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P2__MVP_P2_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P2__MVP_P2_Y=(s_int8) predP16x16VecY;

      auxReg.ME_MB_P3__MV_P3_X=(s_int8) bestHor[0][0];
      auxReg.ME_MB_P3__MV_P3_Y=(s_int8) bestVer[0][0];
      auxReg.ME_MB_P3__MVP_P3_X=(s_int8) predP16x16VecX;
      auxReg.ME_MB_P3__MVP_P3_Y=(s_int8) predP16x16VecY;
   }
   else if ((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER16X8))
   {
      // update motion vector data table
      me_curr_pointer->mv[0].mvx=(s_int8) bestHor[1][0];
      me_curr_pointer->mv[0].mvy=(s_int8) bestVer[1][0];
      me_curr_pointer->mv[1].mvx=(s_int8) bestHor[1][0];
      me_curr_pointer->mv[1].mvy=(s_int8) bestVer[1][0];
      me_curr_pointer->mv[2].mvx=(s_int8) bestHor[1][1];
      me_curr_pointer->mv[2].mvy=(s_int8) bestVer[1][1];
      me_curr_pointer->mv[3].mvx=(s_int8) bestHor[1][1];
      me_curr_pointer->mv[3].mvy=(s_int8) bestVer[1][1];

      me_curr_pointer->mvp[0].mvx=(s_int8) predP16x8VecX[0];
      me_curr_pointer->mvp[0].mvy=(s_int8) predP16x8VecY[0];
      me_curr_pointer->mvp[1].mvx=(s_int8) predP16x8VecX[0];
      me_curr_pointer->mvp[1].mvy=(s_int8) predP16x8VecY[0];
      me_curr_pointer->mvp[2].mvx=(s_int8) predP16x8VecX[1];
      me_curr_pointer->mvp[2].mvy=(s_int8) predP16x8VecY[1];
      me_curr_pointer->mvp[3].mvx=(s_int8) predP16x8VecX[1];
      me_curr_pointer->mvp[3].mvy=(s_int8) predP16x8VecY[1];

      // update aux regs
      auxReg.ME_MB_P0__MV_P0_X=(s_int8) bestHor[1][0];
      auxReg.ME_MB_P0__MV_P0_Y=(s_int8) bestVer[1][0];
      auxReg.ME_MB_P0__MVP_P0_X=(s_int8) predP16x8VecX[0];
      auxReg.ME_MB_P0__MVP_P0_Y=(s_int8) predP16x8VecY[0];

      auxReg.ME_MB_P1__MV_P1_X=(s_int8) bestHor[1][0];
      auxReg.ME_MB_P1__MV_P1_Y=(s_int8) bestVer[1][0];
      auxReg.ME_MB_P1__MVP_P1_X=(s_int8) predP16x8VecX[0];
      auxReg.ME_MB_P1__MVP_P1_Y=(s_int8) predP16x8VecY[0];

      auxReg.ME_MB_P2__MV_P2_X=(s_int8) bestHor[1][1];
      auxReg.ME_MB_P2__MV_P2_Y=(s_int8) bestVer[1][1];
      auxReg.ME_MB_P2__MVP_P2_X=(s_int8) predP16x8VecX[1];
      auxReg.ME_MB_P2__MVP_P2_Y=(s_int8) predP16x8VecY[1];

      auxReg.ME_MB_P3__MV_P3_X=(s_int8) bestHor[1][1];
      auxReg.ME_MB_P3__MV_P3_Y=(s_int8) bestVer[1][1];
      auxReg.ME_MB_P3__MVP_P3_X=(s_int8) predP16x8VecX[1];
      auxReg.ME_MB_P3__MVP_P3_Y=(s_int8) predP16x8VecY[1];
   }
   else if ((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER8X16))
   {
      // update motion vector data table
      me_curr_pointer->mv[0].mvx=(s_int8) bestHor[2][0];
      me_curr_pointer->mv[0].mvy=(s_int8) bestVer[2][0];
      me_curr_pointer->mv[1].mvx=(s_int8) bestHor[2][1];
      me_curr_pointer->mv[1].mvy=(s_int8) bestVer[2][1];
      me_curr_pointer->mv[2].mvx=(s_int8) bestHor[2][0];
      me_curr_pointer->mv[2].mvy=(s_int8) bestVer[2][0];
      me_curr_pointer->mv[3].mvx=(s_int8) bestHor[2][1];
      me_curr_pointer->mv[3].mvy=(s_int8) bestVer[2][1];

      me_curr_pointer->mvp[0].mvx=(s_int8) predP8x16VecX[0];
      me_curr_pointer->mvp[0].mvy=(s_int8) predP8x16VecY[0];
      me_curr_pointer->mvp[1].mvx=(s_int8) predP8x16VecX[1];
      me_curr_pointer->mvp[1].mvy=(s_int8) predP8x16VecY[1];
      me_curr_pointer->mvp[2].mvx=(s_int8) predP8x16VecX[0];
      me_curr_pointer->mvp[2].mvy=(s_int8) predP8x16VecY[0];
      me_curr_pointer->mvp[3].mvx=(s_int8) predP8x16VecX[1];
      me_curr_pointer->mvp[3].mvy=(s_int8) predP8x16VecY[1];

      // update aux regs
      auxReg.ME_MB_P0__MV_P0_X=(s_int8) bestHor[2][0];
      auxReg.ME_MB_P0__MV_P0_Y=(s_int8) bestVer[2][0];
      auxReg.ME_MB_P0__MVP_P0_X=(s_int8) predP8x16VecX[0];
      auxReg.ME_MB_P0__MVP_P0_Y=(s_int8) predP8x16VecY[0];

      auxReg.ME_MB_P1__MV_P1_X=(s_int8) bestHor[2][1];
      auxReg.ME_MB_P1__MV_P1_Y=(s_int8) bestVer[2][1];
      auxReg.ME_MB_P1__MVP_P1_X=(s_int8) predP8x16VecX[1];
      auxReg.ME_MB_P1__MVP_P1_Y=(s_int8) predP8x16VecY[1];

      auxReg.ME_MB_P2__MV_P2_X=(s_int8) bestHor[2][0];
      auxReg.ME_MB_P2__MV_P2_Y=(s_int8) bestVer[2][0];
      auxReg.ME_MB_P2__MVP_P2_X=(s_int8) predP8x16VecX[0];
      auxReg.ME_MB_P2__MVP_P2_Y=(s_int8) predP8x16VecY[0];

      auxReg.ME_MB_P3__MV_P3_X=(s_int8) bestHor[2][1];
      auxReg.ME_MB_P3__MV_P3_Y=(s_int8) bestVer[2][1];
      auxReg.ME_MB_P3__MVP_P3_X=(s_int8) predP8x16VecX[1];
      auxReg.ME_MB_P3__MVP_P3_Y=(s_int8) predP8x16VecY[1];
   }
   else if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER8X8)) ||
            ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_INTER8X8)))
   {
      // update motion vector data table
      me_curr_pointer->mv[0].mvx=(s_int8) bestHor[3][0];
      me_curr_pointer->mv[0].mvy=(s_int8) bestVer[3][0];
      me_curr_pointer->mv[1].mvx=(s_int8) bestHor[3][1];
      me_curr_pointer->mv[1].mvy=(s_int8) bestVer[3][1];
      me_curr_pointer->mv[2].mvx=(s_int8) bestHor[3][2];
      me_curr_pointer->mv[2].mvy=(s_int8) bestVer[3][2];
      me_curr_pointer->mv[3].mvx=(s_int8) bestHor[3][3];
      me_curr_pointer->mv[3].mvy=(s_int8) bestVer[3][3];

      me_curr_pointer->mvp[0].mvx=(s_int8) predP8x8VecX[0];
      me_curr_pointer->mvp[0].mvy=(s_int8) predP8x8VecY[0];
      me_curr_pointer->mvp[1].mvx=(s_int8) predP8x8VecX[1];
      me_curr_pointer->mvp[1].mvy=(s_int8) predP8x8VecY[1];
      me_curr_pointer->mvp[2].mvx=(s_int8) predP8x8VecX[2];
      me_curr_pointer->mvp[2].mvy=(s_int8) predP8x8VecY[2];
      me_curr_pointer->mvp[3].mvx=(s_int8) predP8x8VecX[3];
      me_curr_pointer->mvp[3].mvy=(s_int8) predP8x8VecY[3];

      // update aux regs
      auxReg.ME_MB_P0__MV_P0_X=(s_int8) bestHor[3][0];
      auxReg.ME_MB_P0__MV_P0_Y=(s_int8) bestVer[3][0];
      auxReg.ME_MB_P0__MVP_P0_X=(s_int8) predP8x8VecX[0];
      auxReg.ME_MB_P0__MVP_P0_Y=(s_int8) predP8x8VecY[0];

      auxReg.ME_MB_P1__MV_P1_X=(s_int8) bestHor[3][1];
      auxReg.ME_MB_P1__MV_P1_Y=(s_int8) bestVer[3][1];
      auxReg.ME_MB_P1__MVP_P1_X=(s_int8) predP8x8VecX[1];
      auxReg.ME_MB_P1__MVP_P1_Y=(s_int8) predP8x8VecY[1];

      auxReg.ME_MB_P2__MV_P2_X=(s_int8) bestHor[3][2];
      auxReg.ME_MB_P2__MV_P2_Y=(s_int8) bestVer[3][2];
      auxReg.ME_MB_P2__MVP_P2_X=(s_int8) predP8x8VecX[2];
      auxReg.ME_MB_P2__MVP_P2_Y=(s_int8) predP8x8VecY[2];

      auxReg.ME_MB_P3__MV_P3_X=(s_int8) bestHor[3][3];
      auxReg.ME_MB_P3__MV_P3_Y=(s_int8) bestVer[3][3];
      auxReg.ME_MB_P3__MVP_P3_X=(s_int8) predP8x8VecX[3];
      auxReg.ME_MB_P3__MVP_P3_Y=(s_int8) predP8x8VecY[3];
   }
   else
   {
      assert(false);
   }

   // write Motion Vectors Data Table (0x10-1F) to sdm
   MeMemoryWriteSDM(((u_int8 *) (auxReg.ME_CURRENT_POINTER << ME_CURRENT_POINTER_SHIFT)) + 16, mpcBytePtr + 16, 16);

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::SetAuxReg
// Purpose:       Set the contents of an auxiliary register
// Arguments:     reg = register to write
//                data = value to write to register
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::SetAuxReg (u_int32 addr, u_int32 data)
{
#ifdef USE_CALLBACK
   // by default none of the aux regs cause a response
   MEcallback->done_with_command(false);
#endif

   switch (addr)
   {
      case ME_VCONFIG:
         // Read Only
         break;

      case ME_NAMESTR:
         // Read Only
         break;

      case ME_BUILD:
         // Read Only
         break;

      case ME_REFERENCE_START:
         auxReg.ME_REFERENCE_START=(data >> ME_REFERENCE_START_SHIFT) & ME_REFERENCE_START_MASK;

         // need to reload internal picture buffer ever time the reference picture changes
         firstTimePicture=1;

         break;

      case ME_REFERENCE_STRIDE:
         auxReg.ME_REFERENCE_STRIDE=(u_int16) (data >> ME_REFERENCE_STRIDE_SHIFT) & ME_REFERENCE_STRIDE_MASK;
         break;

      case ME_CURRENT_START:
         auxReg.ME_CURRENT_START=(data >> ME_CURRENT_START_SHIFT) & ME_CURRENT_START_MASK;
         break;

      case ME_CURRENT_STRIDE:
         auxReg.ME_CURRENT_STRIDE=(u_int16) (data >> ME_CURRENT_STRIDE_SHIFT) & ME_CURRENT_STRIDE_MASK;
         break;

      case ME_MB_DIMENSION:
         auxReg.ME_MB_DIMENSION__MB_X_DIMENSION=(u_int8) (data >> ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) & ME_MB_DIMENSION__MB_X_DIMENSION_MASK;
         auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION=(u_int8) (data >> ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT) & ME_MB_DIMENSION__MB_Y_DIMENSION_MASK;
         break;

      case ME_MB_INDEX:
         auxReg.ME_MB_INDEX__MB_X_INDEX=(u_int8) (data >> ME_MB_INDEX__MB_X_INDEX_SHIFT) & ME_MB_INDEX__MB_X_INDEX_MASK;
         auxReg.ME_MB_INDEX__MB_Y_INDEX=(u_int8) (data >> ME_MB_INDEX__MB_Y_INDEX_SHIFT) & ME_MB_INDEX__MB_Y_INDEX_MASK;
         break;

      case ME_MB_SDM_ADDR:
         auxReg.ME_MB_SDM_ADDR=(data >> ME_MB_SDM_ADDR_SHIFT) & ME_MB_SDM_ADDR_MASK;
         break;

      case ME_REFERENCE_SDM_START:
         auxReg.ME_REFERENCE_SDM_START=(data >> ME_REFERENCE_SDM_START_SHIFT) & ME_REFERENCE_SDM_START_MASK;
         break;

      case ME_THRESHOLD_SKIP:
         auxReg.ME_THRESHOLD_SKIP=(u_int16) (data >> ME_THRESHOLD_SKIP_SHIFT) & ME_THRESHOLD_SKIP_MASK;
         break;

      case ME_THRESHOLD_16x16:
         auxReg.ME_THRESHOLD_16x16=(u_int16) (data >> ME_THRESHOLD_16x16_SHIFT) & ME_THRESHOLD_16x16_MASK;
         break;

      case ME_THRESHOLD_BLOCK_PATTERN:
         auxReg.ME_THRESHOLD_BLOCK_PATTERN=(u_int16) (data >> ME_THRESHOLD_BLOCK_PATTERN_SHIFT) & ME_THRESHOLD_BLOCK_PATTERN_MASK;
         break;

      case ME_THRESHOLD_SKIP_MAX:
         auxReg.ME_THRESHOLD_SKIP_MAX=(u_int16) (data >> ME_THRESHOLD_SKIP_MAX_SHIFT) & ME_THRESHOLD_SKIP_MAX_MASK;
         break;

      case ME_CONTROL0:
         auxReg.ME_CONTROL0__START_ITER=(u_int8) (data >> ME_CONTROL0__START_ITER_SHIFT) & ME_CONTROL0__START_ITER_MASK;
         auxReg.ME_CONTROL0__TERM_ITER=(u_int8) (data >> ME_CONTROL0__TERM_ITER_SHIFT) & ME_CONTROL0__TERM_ITER_MASK;
         auxReg.ME_CONTROL0__NO_ME=(u_int8) (data >> ME_CONTROL0__NO_ME_SHIFT) & ME_CONTROL0__NO_ME_MASK;
         auxReg.ME_CONTROL0__REF_PIX_CACHE_EN=(u_int8) (data >> ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT) & ME_CONTROL0__REF_PIX_CACHE_EN_MASK;
         auxReg.ME_CONTROL0__PAR_ID=(u_int8) (data >> ME_CONTROL0__PAR_ID_SHIFT) & ME_CONTROL0__PAR_ID_MASK;
         auxReg.ME_CONTROL0__MV_X=(s_int8) (data >> ME_CONTROL0__MV_X_SHIFT) & ME_CONTROL0__MV_X_MASK;
         auxReg.ME_CONTROL0__MV_Y=(s_int8) (data >> ME_CONTROL0__MV_Y_SHIFT) & ME_CONTROL0__MV_Y_MASK;

         // start ME
         if (auxReg.ME_CONTROL0__START_ITER==1 && auxReg.ME_CONTROL0__TERM_ITER==0)
         {
            DoMotionEstimation();

//#ifdef USE_CALLBACK
//            // this aux regs has a response
//            MEcallback->done_with_command(true);
//#endif
         }

         break;

      case ME_CONTROL1:
         auxReg.ME_CONTROL1__SUB_PEL_CTRL=(u_int8) (data >> ME_CONTROL1__SUB_PEL_CTRL_SHIFT) & ME_CONTROL1__SUB_PEL_CTRL_MASK;
         auxReg.ME_CONTROL1__STEP_ONE_CTRL=(u_int8) (data >> ME_CONTROL1__STEP_ONE_CTRL_SHIFT) & ME_CONTROL1__STEP_ONE_CTRL_MASK;
         auxReg.ME_CONTROL1__ROUND_CTRL=(u_int8) (data >> ME_CONTROL1__ROUND_CTRL_SHIFT) & ME_CONTROL1__ROUND_CTRL_MASK;
         break;

      case ME_SETUP:
         auxReg.ME_SETUP__ITER_TYPE=(u_int8) (data >> ME_SETUP__ITER_TYPE_SHIFT) & ME_SETUP__ITER_TYPE_MASK;
         auxReg.ME_SETUP__BLK_PAT_EN=(u_int8) (data >> ME_SETUP__BLK_PAT_EN_SHIFT) & ME_SETUP__BLK_PAT_EN_MASK;
         auxReg.ME_SETUP__CONSTRAIN_INTRA_PRED=(u_int8) (data >> ME_SETUP__CONSTRAIN_INTRA_PRED_SHIFT) & ME_SETUP__CONSTRAIN_INTRA_PRED_MASK;
         auxReg.ME_SETUP__PAD_CTRL=(u_int8) (data >> ME_SETUP__PAD_CTRL_SHIFT) & ME_SETUP__PAD_CTRL_MASK;
         auxReg.ME_SETUP__SYS_DMA_EN=(u_int8) (data >> ME_SETUP__SYS_DMA_EN_SHIFT) & ME_SETUP__SYS_DMA_EN_MASK;
         auxReg.ME_SETUP__CODEC=(u_int8) (data >> ME_SETUP__CODEC_SHIFT) & ME_SETUP__CODEC_MASK;
         auxReg.ME_SETUP__SKIP_ADJUST=(u_int8) (data >> ME_SETUP__SKIP_ADJUST_SHIFT) & ME_SETUP__SKIP_ADJUST_MASK;
         auxReg.ME_SETUP__PMV=(u_int8) (data >> ME_SETUP__PMV_SHIFT) & ME_SETUP__PMV_MASK;
         auxReg.ME_SETUP__EMV=(u_int8) (data >> ME_SETUP__EMV_SHIFT) & ME_SETUP__EMV_MASK;
         break;

      case ME_LAMBDA:
         auxReg.ME_LAMBDA=(u_int8) (data >> ME_LAMBDA_SHIFT) & ME_LAMBDA_MASK;
         break;

      case ME_COLLECTION_ID:
         auxReg.ME_COLLECTION_ID=(u_int8) (data >> ME_COLLECTION_ID_SHIFT) & ME_COLLECTION_ID_MASK;
         break;

      case ME_RESULT:
         // Read Only
         break;

      case ME_MB_P0:
         // Read Only
         break;

      case ME_MB_P1:
         // Read Only
         break;

      case ME_MB_P2:
         // Read Only
         break;

      case ME_MB_P3:
         // Read Only
         break;

      case ME_SKIP_BR_ADDR:
         auxReg.ME_SKIP_BR_ADDR=(data >> ME_SKIP_BR_ADDR_SHIFT) & ME_SKIP_BR_ADDR_MASK;
         break;

      case ME_16x16_BR_ADDR:
         auxReg.ME_16x16_BR_ADDR=(data >> ME_16x16_BR_ADDR_SHIFT) & ME_16x16_BR_ADDR_MASK;
         break;

      case ME_8x8_BR_ADDR:
         auxReg.ME_8x8_BR_ADDR=(data >> ME_8x8_BR_ADDR_SHIFT) & ME_8x8_BR_ADDR_MASK;
         break;

      case ME_SDM_BR_ADDR:
         auxReg.ME_SDM_BR_ADDR=(data >> ME_SDM_BR_ADDR_SHIFT) & ME_SDM_BR_ADDR_MASK;
         break;

      case ME_BASE_BR_ADDR:
         auxReg.ME_BASE_BR_ADDR__CHAN_DIS=(u_int8) (data >> ME_BASE_BR_ADDR__CHAN_DIS_SHIFT) & ME_BASE_BR_ADDR__CHAN_DIS_MASK;
         break;

      case ME_D_POINTER:
         auxReg.ME_D_POINTER=(data >> ME_D_POINTER_SHIFT) & ME_D_POINTER_MASK;
         break;

      case ME_A_POINTER:
         auxReg.ME_A_POINTER=(data >> ME_A_POINTER_SHIFT) & ME_A_POINTER_MASK;
         break;

      case ME_CURRENT_POINTER:
         auxReg.ME_CURRENT_POINTER=(data >> ME_CURRENT_POINTER_SHIFT) & ME_CURRENT_POINTER_MASK;
         break;

      case ME_16x8_BR_ADDR:
         auxReg.ME_16x8_BR_ADDR=(data >> ME_16x8_BR_ADDR_SHIFT) & ME_16x8_BR_ADDR_MASK;
         break;

      case ME_8x16_BR_ADDR:
         auxReg.ME_8x16_BR_ADDR=(data >> ME_8x16_BR_ADDR_SHIFT) & ME_8x16_BR_ADDR_MASK;
         break;

      case ME_DEBUG:
         auxReg.ME_DEBUG__START_ITER_CTRL=(u_int8) (data >> ME_DEBUG__START_ITER_CTRL_SHIFT) & ME_DEBUG__START_ITER_CTRL_MASK;
         auxReg.ME_DEBUG__CHAN_MB_CTRL=(u_int8) (data >> ME_DEBUG__CHAN_MB_CTRL_SHIFT) & ME_DEBUG__CHAN_MB_CTRL_MASK;
         auxReg.ME_DEBUG__CHAN_FINAL_CTRL=(u_int8) (data >> ME_DEBUG__CHAN_FINAL_CTRL_SHIFT) & ME_DEBUG__CHAN_FINAL_CTRL_MASK;
         auxReg.ME_DEBUG__IDLE_CTRL=(u_int8) (data >> ME_DEBUG__IDLE_CTRL_SHIFT) & ME_DEBUG__IDLE_CTRL_MASK;
         auxReg.ME_DEBUG__START_ITER_FLAG=(u_int8) (data >> ME_DEBUG__START_ITER_FLAG_SHIFT) & ME_DEBUG__START_ITER_FLAG_MASK;
         auxReg.ME_DEBUG__CHAN_MB_FLAG=(u_int8) (data >> ME_DEBUG__CHAN_MB_FLAG_SHIFT) & ME_DEBUG__CHAN_MB_FLAG_MASK;
         auxReg.ME_DEBUG__CHAN_FINAL_FLAG=(u_int8) (data >> ME_DEBUG__CHAN_FINAL_FLAG_SHIFT) & ME_DEBUG__CHAN_FINAL_FLAG_MASK;
         auxReg.ME_DEBUG__IDLE_FLAG=(u_int8) (data >> ME_DEBUG__IDLE_FLAG_SHIFT) & ME_DEBUG__IDLE_FLAG_MASK;
         break;

      default:
         // Invalid register
         assert(false);
         break;
   }
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::GetAuxReg
// Purpose:       Get the contents of an auxiliary register
// Arguments:     reg = register to read
// Return Value:  value read from register
//******************************************************************************
u_int32 ArcMotionEstimationModel::GetAuxReg (u_int32 addr)
{
   u_int32 readData;

   switch (addr)
   {
      case ME_VCONFIG:
         readData=((auxReg.ME_VCONFIG__VERSION & ME_VCONFIG__VERSION_MASK) << ME_VCONFIG__VERSION_SHIFT) |
                  ((auxReg.ME_VCONFIG__N_TSTR & ME_VCONFIG__N_TSTR_MASK) << ME_VCONFIG__N_TSTR_SHIFT) |
                  ((auxReg.ME_VCONFIG__N_ISTR & ME_VCONFIG__N_ISTR_MASK) << ME_VCONFIG__N_ISTR_SHIFT) |
                  ((auxReg.ME_VCONFIG__MOD_ID & ME_VCONFIG__MOD_ID_MASK) << ME_VCONFIG__MOD_ID_SHIFT) |
                  ((auxReg.ME_VCONFIG__N_AUX & ME_VCONFIG__N_AUX_MASK) << ME_VCONFIG__N_AUX_SHIFT);
         break;

      case ME_NAMESTR:
         readData=(auxReg.ME_NAMESTR & ME_NAMESTR_MASK) << ME_NAMESTR_SHIFT;
         break;

      case ME_BUILD:
         readData=((auxReg.ME_BUILD__ME_VER & ME_BUILD__ME_VER_MASK) << ME_BUILD__ME_VER_SHIFT) |
                  ((auxReg.ME_BUILD__PIXEL_CACHE & ME_BUILD__PIXEL_CACHE_MASK) << ME_BUILD__PIXEL_CACHE_SHIFT);
         break;

      case ME_REFERENCE_START:
         readData=(auxReg.ME_REFERENCE_START & ME_REFERENCE_START_MASK) << ME_REFERENCE_START_SHIFT;
         break;

      case ME_REFERENCE_STRIDE:
         readData=(auxReg.ME_REFERENCE_STRIDE & ME_REFERENCE_STRIDE_MASK) << ME_REFERENCE_STRIDE_SHIFT;
         break;

      case ME_CURRENT_START:
         readData=(auxReg.ME_CURRENT_START & ME_CURRENT_START_MASK) << ME_CURRENT_START_SHIFT;
         break;

      case ME_CURRENT_STRIDE:
         readData=(auxReg.ME_CURRENT_STRIDE & ME_CURRENT_STRIDE_MASK) << ME_CURRENT_STRIDE_SHIFT;
         break;

      case ME_MB_DIMENSION:
         readData=((auxReg.ME_MB_DIMENSION__MB_X_DIMENSION & ME_MB_DIMENSION__MB_X_DIMENSION_MASK) << ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) |
                  ((auxReg.ME_MB_DIMENSION__MB_Y_DIMENSION & ME_MB_DIMENSION__MB_Y_DIMENSION_MASK) << ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT);
         break;

      case ME_MB_INDEX:
         readData=((auxReg.ME_MB_INDEX__MB_X_INDEX & ME_MB_INDEX__MB_X_INDEX_MASK) << ME_MB_INDEX__MB_X_INDEX_SHIFT) |
                  ((auxReg.ME_MB_INDEX__MB_Y_INDEX & ME_MB_INDEX__MB_Y_INDEX_MASK) << ME_MB_INDEX__MB_Y_INDEX_SHIFT);
         break;

      case ME_MB_SDM_ADDR:
         readData=(auxReg.ME_MB_SDM_ADDR & ME_MB_SDM_ADDR_MASK) << ME_MB_SDM_ADDR_SHIFT;
         break;

      case ME_REFERENCE_SDM_START:
         readData=(auxReg.ME_REFERENCE_SDM_START & ME_REFERENCE_SDM_START_MASK) << ME_REFERENCE_SDM_START_SHIFT;
         break;

      case ME_THRESHOLD_SKIP:
         readData=(auxReg.ME_THRESHOLD_SKIP & ME_THRESHOLD_SKIP_MASK) << ME_THRESHOLD_SKIP_SHIFT;
         break;

      case ME_THRESHOLD_16x16:
         readData=(auxReg.ME_THRESHOLD_16x16 & ME_THRESHOLD_16x16_MASK) << ME_THRESHOLD_16x16_SHIFT;
         break;

      case ME_THRESHOLD_BLOCK_PATTERN:
         readData=(auxReg.ME_THRESHOLD_BLOCK_PATTERN & ME_THRESHOLD_BLOCK_PATTERN_MASK) << ME_THRESHOLD_BLOCK_PATTERN_SHIFT;
         break;

      case ME_THRESHOLD_SKIP_MAX:
         readData=(auxReg.ME_THRESHOLD_SKIP_MAX & ME_THRESHOLD_SKIP_MAX_MASK) << ME_THRESHOLD_SKIP_MAX_SHIFT;
         break;

      case ME_CONTROL0:
         readData=((auxReg.ME_CONTROL0__START_ITER & ME_CONTROL0__START_ITER_MASK) << ME_CONTROL0__START_ITER_SHIFT) |
                  ((auxReg.ME_CONTROL0__TERM_ITER & ME_CONTROL0__TERM_ITER_MASK) << ME_CONTROL0__TERM_ITER_SHIFT) |
                  ((auxReg.ME_CONTROL0__NO_ME & ME_CONTROL0__NO_ME_MASK) << ME_CONTROL0__NO_ME_SHIFT) |
                  ((auxReg.ME_CONTROL0__REF_PIX_CACHE_EN & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT) |
                  ((auxReg.ME_CONTROL0__PAR_ID & ME_CONTROL0__PAR_ID_MASK) << ME_CONTROL0__PAR_ID_SHIFT) |
                  ((auxReg.ME_CONTROL0__MV_X & ME_CONTROL0__MV_X_MASK) << ME_CONTROL0__MV_X_SHIFT) |
                  ((auxReg.ME_CONTROL0__MV_Y & ME_CONTROL0__MV_Y_MASK) << ME_CONTROL0__MV_Y_SHIFT);
         break;

      case ME_CONTROL1:
         readData=((auxReg.ME_CONTROL1__SUB_PEL_CTRL & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT) |
                  ((auxReg.ME_CONTROL1__STEP_ONE_CTRL & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT) |
                  ((auxReg.ME_CONTROL1__ROUND_CTRL & ME_CONTROL1__ROUND_CTRL_MASK) << ME_CONTROL1__ROUND_CTRL_SHIFT);
         break;

      case ME_SETUP:
         readData=((auxReg.ME_SETUP__ITER_TYPE & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
                  ((auxReg.ME_SETUP__BLK_PAT_EN & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                  ((auxReg.ME_SETUP__CONSTRAIN_INTRA_PRED & ME_SETUP__CONSTRAIN_INTRA_PRED_MASK) << ME_SETUP__CONSTRAIN_INTRA_PRED_SHIFT) |
                  ((auxReg.ME_SETUP__PAD_CTRL & ME_SETUP__PAD_CTRL_MASK) << ME_SETUP__PAD_CTRL_SHIFT) |
                  ((auxReg.ME_SETUP__SYS_DMA_EN & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
                  ((auxReg.ME_SETUP__CODEC & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
                  ((auxReg.ME_SETUP__SKIP_ADJUST & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
                  ((auxReg.ME_SETUP__PMV & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
                  ((auxReg.ME_SETUP__EMV & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT);
         break;

      case ME_LAMBDA:
         readData=(auxReg.ME_LAMBDA & ME_LAMBDA_MASK) << ME_LAMBDA_SHIFT;
         break;

      case ME_COLLECTION_ID:
         readData=(auxReg.ME_COLLECTION_ID & ME_COLLECTION_ID_MASK) << ME_COLLECTION_ID_SHIFT;
         break;

      case ME_RESULT:
         readData=((auxReg.ME_RESULT__COST_SAD & ME_RESULT__COST_SAD_MASK) << ME_RESULT__COST_SAD_SHIFT) |
                  ((auxReg.ME_RESULT__CANDIDATE_TYPE & ME_RESULT__CANDIDATE_TYPE_MASK) << ME_RESULT__CANDIDATE_TYPE_SHIFT) |
                  ((auxReg.ME_RESULT__SKIP & ME_RESULT__SKIP_MASK) << ME_RESULT__SKIP_SHIFT) |
                  ((auxReg.ME_RESULT__16x16_SEARCH & ME_RESULT__16x16_SEARCH_MASK) << ME_RESULT__16x16_SEARCH_SHIFT) |
                  ((auxReg.ME_RESULT__8x8_SEARCH & ME_RESULT__8x8_SEARCH_MASK) << ME_RESULT__8x8_SEARCH_SHIFT) |
                  ((auxReg.ME_RESULT__BLK_PAT & ME_RESULT__BLK_PAT_MASK) << ME_RESULT__BLK_PAT_SHIFT) |
                  ((auxReg.ME_RESULT__CONFIG_ERR & ME_RESULT__CONFIG_ERR_MASK) << ME_RESULT__CONFIG_ERR_SHIFT) |
                  ((auxReg.ME_RESULT__BUS_ERR & ME_RESULT__BUS_ERR_MASK) << ME_RESULT__BUS_ERR_SHIFT);

         // reading the result register causes the ME_RESULT__CONFIG_ERR bit to be reset
         auxReg.ME_RESULT__CONFIG_ERR = 0;

         break;

      case ME_MB_P0:
         readData=((auxReg.ME_MB_P0__MV_P0_X & ME_MB_P0__MV_P0_X_MASK) << ME_MB_P0__MV_P0_X_SHIFT) |
                  ((auxReg.ME_MB_P0__MV_P0_Y & ME_MB_P0__MV_P0_Y_MASK) << ME_MB_P0__MV_P0_Y_SHIFT) |
                  ((auxReg.ME_MB_P0__MVP_P0_X & ME_MB_P0__MVP_P0_X_MASK) << ME_MB_P0__MVP_P0_X_SHIFT) |
                  ((auxReg.ME_MB_P0__MVP_P0_Y & ME_MB_P0__MVP_P0_Y_MASK) << ME_MB_P0__MVP_P0_Y_SHIFT);
         break;

      case ME_MB_P1:
         readData=((auxReg.ME_MB_P1__MV_P1_X & ME_MB_P1__MV_P1_X_MASK) << ME_MB_P1__MV_P1_X_SHIFT) |
                  ((auxReg.ME_MB_P1__MV_P1_Y & ME_MB_P1__MV_P1_Y_MASK) << ME_MB_P1__MV_P1_Y_SHIFT) |
                  ((auxReg.ME_MB_P1__MVP_P1_X & ME_MB_P1__MVP_P1_X_MASK) << ME_MB_P1__MVP_P1_X_SHIFT) |
                  ((auxReg.ME_MB_P1__MVP_P1_Y & ME_MB_P1__MVP_P1_Y_MASK) << ME_MB_P1__MVP_P1_Y_SHIFT);
         break;

      case ME_MB_P2:
         readData=((auxReg.ME_MB_P2__MV_P2_X & ME_MB_P2__MV_P2_X_MASK) << ME_MB_P2__MV_P2_X_SHIFT) |
                  ((auxReg.ME_MB_P2__MV_P2_Y & ME_MB_P2__MV_P2_Y_MASK) << ME_MB_P2__MV_P2_Y_SHIFT) |
                  ((auxReg.ME_MB_P2__MVP_P2_X & ME_MB_P2__MVP_P2_X_MASK) << ME_MB_P2__MVP_P2_X_SHIFT) |
                  ((auxReg.ME_MB_P2__MVP_P2_Y & ME_MB_P2__MVP_P2_Y_MASK) << ME_MB_P2__MVP_P2_Y_SHIFT);
         break;

      case ME_MB_P3:
         readData=((auxReg.ME_MB_P3__MV_P3_X & ME_MB_P3__MV_P3_X_MASK) << ME_MB_P3__MV_P3_X_SHIFT) |
                  ((auxReg.ME_MB_P3__MV_P3_Y & ME_MB_P3__MV_P3_Y_MASK) << ME_MB_P3__MV_P3_Y_SHIFT) |
                  ((auxReg.ME_MB_P3__MVP_P3_X & ME_MB_P3__MVP_P3_X_MASK) << ME_MB_P3__MVP_P3_X_SHIFT) |
                  ((auxReg.ME_MB_P3__MVP_P3_Y & ME_MB_P3__MVP_P3_Y_MASK) << ME_MB_P3__MVP_P3_Y_SHIFT);
         break;

      case ME_SKIP_BR_ADDR:
         readData=(auxReg.ME_SKIP_BR_ADDR & ME_SKIP_BR_ADDR_MASK) << ME_SKIP_BR_ADDR_SHIFT;
         break;

      case ME_16x16_BR_ADDR:
         readData=(auxReg.ME_16x16_BR_ADDR & ME_16x16_BR_ADDR_MASK) << ME_16x16_BR_ADDR_SHIFT;
         break;

      case ME_8x8_BR_ADDR:
         readData=(auxReg.ME_8x8_BR_ADDR & ME_8x8_BR_ADDR_MASK) << ME_8x8_BR_ADDR_SHIFT;
         break;

      case ME_SDM_BR_ADDR:
         readData=(auxReg.ME_SDM_BR_ADDR & ME_SDM_BR_ADDR_MASK) << ME_SDM_BR_ADDR_SHIFT;
         break;

      case ME_BASE_BR_ADDR:
         readData=((auxReg.ME_BASE_BR_ADDR__CHAN_DIS & ME_BASE_BR_ADDR__CHAN_DIS_MASK) << ME_BASE_BR_ADDR__CHAN_DIS_SHIFT);
         break;

      case ME_D_POINTER:
         readData=(auxReg.ME_D_POINTER & ME_D_POINTER_MASK) << ME_D_POINTER_SHIFT;
         break;

      case ME_A_POINTER:
         readData=(auxReg.ME_A_POINTER & ME_A_POINTER_MASK) << ME_A_POINTER_SHIFT;
         break;

      case ME_CURRENT_POINTER:
         readData=(auxReg.ME_CURRENT_POINTER & ME_CURRENT_POINTER_MASK) << ME_CURRENT_POINTER_SHIFT;
         break;

      case ME_16x8_BR_ADDR:
         readData=(auxReg.ME_16x8_BR_ADDR & ME_16x8_BR_ADDR_MASK) << ME_16x8_BR_ADDR_SHIFT;
         break;

      case ME_8x16_BR_ADDR:
         readData=(auxReg.ME_8x16_BR_ADDR & ME_8x16_BR_ADDR_MASK) << ME_8x16_BR_ADDR_SHIFT;
         break;

      case ME_DEBUG:
         readData=((auxReg.ME_DEBUG__START_ITER_CTRL & ME_DEBUG__START_ITER_CTRL_MASK) << ME_DEBUG__START_ITER_CTRL_SHIFT) |
                  ((auxReg.ME_DEBUG__CHAN_MB_CTRL & ME_DEBUG__CHAN_MB_CTRL_MASK) << ME_DEBUG__CHAN_MB_CTRL_SHIFT) |
                  ((auxReg.ME_DEBUG__CHAN_FINAL_CTRL & ME_DEBUG__CHAN_FINAL_CTRL_MASK) << ME_DEBUG__CHAN_FINAL_CTRL_SHIFT) |
                  ((auxReg.ME_DEBUG__IDLE_CTRL & ME_DEBUG__IDLE_CTRL_MASK) << ME_DEBUG__IDLE_CTRL_SHIFT) |
                  ((auxReg.ME_DEBUG__START_ITER_FLAG & ME_DEBUG__START_ITER_FLAG_MASK) << ME_DEBUG__START_ITER_FLAG_SHIFT) |
                  ((auxReg.ME_DEBUG__CHAN_MB_FLAG & ME_DEBUG__CHAN_MB_FLAG_MASK) << ME_DEBUG__CHAN_MB_FLAG_SHIFT) |
                  ((auxReg.ME_DEBUG__CHAN_FINAL_FLAG & ME_DEBUG__CHAN_FINAL_FLAG_MASK) << ME_DEBUG__CHAN_FINAL_FLAG_SHIFT) |
                  ((auxReg.ME_DEBUG__IDLE_FLAG & ME_DEBUG__IDLE_FLAG_MASK) << ME_DEBUG__IDLE_FLAG_SHIFT);
         break;

      default:
         // Invalid register
         assert(false);
         break;
   }

   return(readData);
}

unsigned int ArcMotionEstimationModel::h264_encoder_vector_code_length(int mvd)
{
   return h264_encoder_signed_code_length_table[(mvd+128) & 255]   |
         h264_encoder_signed_code_msb_table[((mvd+128) >> 8) & SIZE_OF_MSB_TABLE_MINUS_ONE];
}

unsigned int ArcMotionEstimationModel::mpeg4_encoder_vector_code_length(int mvd)
{
   // MPEG4 and H.263 can share the same look-up table as long as
   // the vop_fcode value in MPEG4 is equal to 1, which is always
   // true in our encoder implementation
   return h263_encoder_signed_code_length_table[(mvd+128) & 255] ;
}

unsigned int ArcMotionEstimationModel::h263_encoder_vector_code_length(int mvd)
{
   return h263_encoder_signed_code_length_table[(mvd+128) & 255] ;
}

unsigned int ArcMotionEstimationModel::encoder_vector_code_length(int mvd)
{
   switch(auxReg.ME_SETUP__CODEC)
   {
      case H264:
         // H264 skipped motion vector generation
         return h264_encoder_vector_code_length(mvd);

      case MPEG4:
         return mpeg4_encoder_vector_code_length(mvd);

      case H263:
         return h263_encoder_vector_code_length(mvd);

      default: // Error
         assert(false);
       return 0;
   }
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::WriteCurrentMBToSDM
// Purpose:       Writes the current macroblock to the current MPC pixel data
//                area in the SDM
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::WriteCurrentMBToSDM (void)
{
   unsigned char * current=((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16);

   u_int8 * pixCeoffBuff=(u_int8 *) ((auxReg.ME_MB_SDM_ADDR << ME_MB_SDM_ADDR_SHIFT) + PCB_CURRENT_Y);

   unsigned char current_array[16];

   for (int y=0; y<16; y++)
   {
      MeMemoryRead(current_array, current, 16);
      MeMemoryWriteSDM(pixCeoffBuff, current_array, 16);

      pixCeoffBuff+=PCB_CURRENT_STRIDE;
      current+=(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);
   }

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::GenerateAndWriteReferenceMBToSDM
// Purpose:       Generates the reference macroblock using the candidate motion
//                vectors store in the auxiliary registers. It then writes this
//                macroblock to the mpc pixel data area in the sdm
//                area in the SDM
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::GenerateAndWriteReferenceMBToSDM (void)
{
   unsigned char * reference;
   int refStride;
   int block8x8;
   int refCoSited=0;

#ifdef VERIFICATION_DUMP
   fprintf(vdump_fp, "\n\n----------------------------------------\n");
   fprintf(vdump_fp, "Reference:\n\n");
#endif

   u_int8 * pixCeoffBuff=(u_int8 *) (auxReg.ME_REFERENCE_SDM_START << ME_REFERENCE_SDM_START_SHIFT); // = (auxReg.ME_MB_SDM_ADDR << ME_MB_SDM_ADDR_SHIFT) + PCB_REFERENCE_Y


   // generate reference
   if (((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_SKIPPED || best_inter_mtype==MBCT_H264_INTER16X16)) ||
       ((auxReg.ME_SETUP__CODEC!=H264) && (best_inter_mtype==MBCT_MPEG4_SKIPPED || best_inter_mtype==MBCT_MPEG4_INTER16X16)))
   {
      //set the mv arrays (4x4) for the prediction function
       mv_for_hor[0] = bestHor[0][0];
       mv_for_ver[0] = bestVer[0][0];

      // if reference is co-sited with current macroblock then just copy
      if (bestHor[0][0]==0 && bestVer[0][0]==0)
       {
         refStride=(auxReg.ME_MB_DIMENSION__MB_X_DIMENSION*16) + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS;

         reference=refYWthBrdrPels + (((16 * auxReg.ME_MB_INDEX__MB_Y_INDEX) + NUMBER_OF_TOP_BORDER_PELS) * refStride)
                                         + (16 * auxReg.ME_MB_INDEX__MB_X_INDEX) + NUMBER_OF_LEFT_BORDER_PELS;
         refCoSited=1;
       }
       else
       {
         if (auxReg.ME_SETUP__CODEC==H264)
            ArcMakeH264InterPrediction(best_inter_mtype); // goes into y_prediction_data
         else
            ArcMakeH263InterPrediction(best_inter_mtype); // goes into y_prediction_data
      }
   }
   else if((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER16X8))
   {
      for (block8x8=0; block8x8<4; block8x8+=2)
      {
         //set the mv arrays (4x4) for the prediction function
         for(int block4x4 = 0; block4x4 < 8; block4x4++) {
            mv_for_hor[(block8x8<<2)+ block4x4] = bestHor[1][block8x8/2];
            mv_for_ver[(block8x8<<2)+ block4x4] = bestVer[1][block8x8/2];
         }

         ArcMakeH264InterPrediction8(block8x8, H264_16X8); // goes into quadrant of y_prediction_data
      }
   }
   else if((auxReg.ME_SETUP__CODEC==H264) && (best_inter_mtype==MBCT_H264_INTER8X16))
   {
      for (block8x8=0; block8x8<2; block8x8++)
      {
         //set the mv arrays (4x4) for the prediction function
          // has this already been done ?

            mv_for_hor[block8x8] = mv_for_hor[block8x8+1] = mv_for_hor[block8x8+4] = mv_for_hor[block8x8+5] =
            mv_for_hor[block8x8+8] = mv_for_hor[block8x8+9] = mv_for_hor[block8x8+12] = mv_for_hor[block8x8+13] = bestHor[2][block8x8];
            mv_for_ver[block8x8] = mv_for_ver[block8x8+1] = mv_for_ver[block8x8+4] = mv_for_ver[block8x8+5] =
            mv_for_ver[block8x8+8] = mv_for_ver[block8x8+9] = mv_for_ver[block8x8+12] = mv_for_ver[block8x8+13] = bestVer[2][block8x8];


            ArcMakeH264InterPrediction8(block8x8, H264_8X16); // goes into quadrant of y_prediction_data
      }
   }
   else // (best_inter_mtype==MBCT_H264_INTER8X8))
   {
      for (block8x8=0; block8x8<4; block8x8++)
      {
         //set the mv arrays (4x4) for the prediction function
         int block_index = h264_8x8_block_index_to_raster_index_table[block8x8];

         mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] = bestHor[3][block8x8];
         mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] = bestVer[3][block8x8];

         if (auxReg.ME_SETUP__CODEC==H264)
            ArcMakeH264InterPrediction8(block8x8,H264_8X8); // goes into quadrant of y_prediction_data
         else
            ArcMakeH263InterPrediction8(block8x8); // goes into quadrant of y_prediction_data
      }
   }


   // write reference to sdm
   if (refCoSited==1)
   {
      for (int y=0; y<16; y++)
      {
         MeMemoryWriteSDM(pixCeoffBuff, reference, 16);

#ifdef VERIFICATION_DUMP
         fprintf(vdump_fp, "%02x", *reference);
         for (int loop=1; loop<16; loop++)
         {
            fprintf(vdump_fp, " %02x", *(reference + loop));
         }
         fprintf(vdump_fp, "\n");
#endif

         pixCeoffBuff+=PCB_REFERENCE_STRIDE;
         reference+=refStride;
      }
   }
   else
   {
      int yPredOff=0;

      for (int y=0; y<16; y++)
      {
         MeMemoryWriteSDM(pixCeoffBuff     , reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff     ), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  1, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  1), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  2, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  2), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  3, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  3), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  4, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  4), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  5, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  5), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  6, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  6), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  7, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  7), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  8, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  8), 1);
         MeMemoryWriteSDM(pixCeoffBuff +  9, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff +  9), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 10, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 10), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 11, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 11), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 12, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 12), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 13, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 13), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 14, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 14), 1);
         MeMemoryWriteSDM(pixCeoffBuff + 15, reinterpret_cast<u_int8 *> (y_prediction_data + yPredOff + 15), 1);

#ifdef VERIFICATION_DUMP
         fprintf(vdump_fp, "%02x", *(y_prediction_data + yPredOff));
         for (int loop=1; loop<16; loop++)
         {
            fprintf(vdump_fp, " %02x", *(y_prediction_data + yPredOff + loop));
         }
         fprintf(vdump_fp, "\n");
#endif

         pixCeoffBuff+=PCB_REFERENCE_STRIDE;
         yPredOff+=16; // stride of y_prediction_data
      }
   }

#ifdef VERIFICATION_DUMP
   fprintf(vdump_fp, "\n");
#endif

}


//******************************************************************************
// Name:          ArcMotionEstimationModel::Manual8x8SAD
// Purpose:       Generates a SAD value for the 8x8 block specified by the
//                appropriate auxiliary registers
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::Manual8x8SAD (void)
{
   //set the mv arrays (4x4) for the prediction function
   int block_index = h264_8x8_block_index_to_raster_index_table[auxReg.ME_CONTROL0__PAR_ID];

   // motion vectors need truncating. the amount is dependant on the codec type
   // (h.264 uses 7 bits and mpeg4/h.263 uses 6 bits)
   int mvTrunc = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

   mv_for_hor[block_index] = mv_for_hor[block_index+1] = mv_for_hor[block_index+4] = mv_for_hor[block_index+5] = ((s_int8) (auxReg.ME_CONTROL0__MV_X << mvTrunc)) >> mvTrunc;
   mv_for_ver[block_index] = mv_for_ver[block_index+1] = mv_for_ver[block_index+4] = mv_for_ver[block_index+5] = ((s_int8) (auxReg.ME_CONTROL0__MV_Y << mvTrunc)) >> mvTrunc;

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n----------------------------------------\n");
    fprintf(vdump_fp, "ManualSAD: 8x8\n");

    // if mpeg4 then want to multiply vectors by 2 to dump in 1/4 pel resolution
    int vecMul = (auxReg.ME_SETUP__CODEC == H264) ? 1 : 2;

    fprintf(vdump_fp, "\n   MVP.4 = (%3d, %3d)\n", ((auxReg.ME_CONTROL0__MV_X << mvTrunc) >> mvTrunc)*vecMul, ((auxReg.ME_CONTROL0__MV_Y << mvTrunc) >> mvTrunc)*vecMul);
#endif

   // generate the reference
   if (auxReg.ME_SETUP__CODEC==H264)
      ArcMakeH264InterPrediction8(auxReg.ME_CONTROL0__PAR_ID,H264_8X8); // goes into quadrant of y_prediction_data
   else
      ArcMakeH263InterPrediction8(auxReg.ME_CONTROL0__PAR_ID); // goes into quadrant of y_prediction_data

   int current_offset;
   int block_offset;

   if (auxReg.ME_CONTROL0__PAR_ID == 0)
   {
      current_offset = 0;
      block_offset = 0;
   }
   else if (auxReg.ME_CONTROL0__PAR_ID == 1)
   {
      current_offset = 8; // sub block offset into current frame
      block_offset = 8; // subblock offset into motion compensated 16x16 array
   }
   else if (auxReg.ME_CONTROL0__PAR_ID == 2)
   {
      current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT);
      block_offset = 128;
   }
   else // (auxReg.ME_CONTROL0__PAR_ID == 3)
   {
      current_offset = 8*(auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT) + 8;
      block_offset = 136;
   }

   unsigned char * current = ((unsigned char *) (auxReg.ME_CURRENT_START << ME_CURRENT_START_SHIFT)) + (auxReg.ME_MB_INDEX__MB_Y_INDEX * 16 * (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT)) + (auxReg.ME_MB_INDEX__MB_X_INDEX * 16) + current_offset;

   // calculate the sad value and store in aux reg
   auxReg.ME_RESULT__COST_SAD = (u_int16) CalculateSSD8(current, &y_prediction_data [block_offset], (auxReg.ME_CURRENT_STRIDE << ME_CURRENT_STRIDE_SHIFT), 16, 8);

   // update some of the other stuff in the result reg according to the spec
   auxReg.ME_RESULT__SKIP = 0;
   auxReg.ME_RESULT__16x16_SEARCH = 0;
   auxReg.ME_RESULT__8x8_SEARCH = 0;

   // return address comes from the skip branch address register
   arcRetAddr = auxReg.ME_SKIP_BR_ADDR << ME_SKIP_BR_ADDR_SHIFT;

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n--------------------\n");
    fprintf(vdump_fp, "ManualSAD 8x8 Result:\n");
    fprintf(vdump_fp, "   SAD = 0x%X\n", auxReg.ME_RESULT__COST_SAD);
#endif // VERIFICATION_DUMP

}


unsigned int ArcMotionEstimationModel::CalculateSAD4(unsigned char* current, int* previous,
                                        unsigned int current_yinc, unsigned int previous_yinc)
{
    unsigned char *cpel = current;
    int *ppel = previous;
    unsigned int local_ssd = 0;

    int y = 0;

    unsigned char cpel_array[4];

    while (y<4)
    {
        MeMemoryRead(cpel_array, cpel, 4);

        local_ssd +=
          ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3));


        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CalculateSSD                                                */
/* DESCRIPTION: Calculates the sum of square differences between the
                current and the previous.                                   */
/* ------------------------------------------------------------------------ */
unsigned int ArcMotionEstimationModel::CalculateSAD4(unsigned char* current, unsigned char* previous,
                                        unsigned int current_yinc, unsigned int previous_yinc)
{
    unsigned char *cpel = current;
    unsigned char *ppel = previous;
    unsigned int local_ssd = 0;

    unsigned char cpel_array[4];
    int y = 0;

    while (y<4)
    {
        MeMemoryRead(cpel_array, cpel, 4);
        local_ssd +=
          ABS_TABLE(*(cpel_array     ) - *(ppel     ))
        + ABS_TABLE(*(cpel_array +  1) - *(ppel +  1))
        + ABS_TABLE(*(cpel_array +  2) - *(ppel +  2))
        + ABS_TABLE(*(cpel_array +  3) - *(ppel +  3));


        cpel += current_yinc;
        ppel += previous_yinc;
        y++;
    }

    return(local_ssd);
}


// ISS functions

#ifdef USE_CALLBACK
//******************************************************************************
// Name:          ArcMotionEstimationModel::set_callback
// Purpose:       Set the worker callback for the ISS
// Arguments:     mew = pointer to the me model's worker callback
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::set_callback (call_back_to_worker *mew)
{
   MEcallback = mew;
}
#endif


//******************************************************************************
// Name:          ArcMotionEstimationModel::SendChannelCommand
// Purpose:       This function will be called by ARC processor for setting up all
//                the inward channel registers
// Arguments:     ch_data = data to write to register
//                id_cmd = channel register to write to
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::SendChannelCommand (unsigned int ch_data, unsigned int id_cmd)
{
    if (auxReg.ME_BASE_BR_ADDR__CHAN_DIS==0)
    {
        // Ignore the channel ID for ISS
        int chanReg = id_cmd & CHANNEL_CMD_MASK;
        u_int32 chanData;

        // if writing to aux reg me_control0 via the channel then there is an implied
        // kick to start an iteration
        if (MeChannelToAuxRegMap[chanReg]==ME_CONTROL0)
        {
            chanData=((u_int32) ch_data & ~0x2) | 0x1;
        }
        else
        {
            chanData=(u_int32) ch_data;
        }

        SetAuxReg(MeChannelToAuxRegMap[chanReg], chanData);
    }
}



//******************************************************************************
// Name:          ArcMotionEstimationModel::GetChannelResponse
// Purpose:       Chooses one of the channel responses to send back. Priority
//                order is response 0 then response 1, i.e. the order in which
//                the ME would send them
// Arguments:     void
// Return Value:  pointer to the returned channel command
//******************************************************************************
struct Response * ArcMotionEstimationModel::GetChannelResponse (void)
{
    struct Response * selectedResponse=NULL;

    // try to get the first channel response (channel out mb)
    selectedResponse=SelectChannelResponse(0);

    if (selectedResponse==NULL)
    {
        // try to get the second channel response (channel out final)
        selectedResponse=SelectChannelResponse(1);
    }

    return(selectedResponse);
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::SelectChannelResponse
// Purpose:       Implements an output channel back to the arc
// Arguments:     index = selects either the channel out mb response or the
//                        channel out final response
// Return Value:  pointer to the returned channel command
//******************************************************************************
struct Response * ArcMotionEstimationModel::SelectChannelResponse (int index)
{
    // check for valid responses
    bool val = false;
    for(int i=0; i<MAX_NUM_REGS_TO_RETURN; i++)
    {
        if(ChannelResponse[index].RegisterData[i].valid)
        {
            val = true;
            break;
        }
    }

    // are we still dealying
    if (val && ChannelResponse[index].DelayCycles > 0) ChannelResponse[index].DelayCycles--;

    // if no valid response or halted at debug point or still delaying then don't
    // return a response
    if (!val ||
        auxReg.ME_BASE_BR_ADDR__CHAN_DIS ||
        auxReg.ME_DEBUG__START_ITER_FLAG ||
        auxReg.ME_DEBUG__CHAN_MB_FLAG ||
        auxReg.ME_DEBUG__CHAN_FINAL_FLAG ||
        auxReg.ME_DEBUG__IDLE_FLAG ||
        ChannelResponse[index].DelayCycles)
        return (NULL);

    // otherwise clear the response structure and senda copy back down the channel
    SendChannelResponse = ChannelResponse[index];

    for(int i=0; i<MAX_NUM_REGS_TO_RETURN; i++)
    {
        ChannelResponse[index].RegisterData[i].valid = false;
    }

    return (&SendChannelResponse);
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::CreateChannelCommandMB
// Purpose:       Populate the MB channel response
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::CreateChannelCommandMB(void)
{
    // model a processing delay for the model i.e. a channel response will not
    // occur until after a certain number of calls to the response
    ChannelResponse[0].DelayCycles=CHAN_MB_DELAY;

    int pos=0;

    // R0 = 2 bit intra valid pattern
    ChannelResponse[0].RegisterData[pos].valid=true;
    ChannelResponse[0].RegisterData[pos].RegID=0;
    // if not in manual sad mode and we are using constrained intra prediction
    // then we need to look at the mb coding type of the neighbour as well
    if (auxReg.ME_SETUP__ITER_TYPE!=0x3 && auxReg.ME_SETUP__CONSTRAIN_INTRA_PRED)
    {
        ChannelResponse[0].RegisterData[pos].RegValue=((aboveMB.validID & aboveMB.intra) << 1) | (leftMB.validID & leftMB.intra);
    }
    else
    {
        ChannelResponse[0].RegisterData[pos].RegValue=(aboveMB.validID << 1) | leftMB.validID;
    }
    pos++;

    // R1 = skipped MV
    ChannelResponse[0].RegisterData[pos].valid=true;
    ChannelResponse[0].RegisterData[pos].RegID=1;
    ChannelResponse[0].RegisterData[pos].RegValue=(((s_int8) PSliceSkippedVer) << 8) | ((s_int8) PSliceSkippedHor & 0xff);
    pos++;

    // R63 = branch address
    ChannelResponse[0].RegisterData[pos].valid=true;
    ChannelResponse[0].RegisterData[pos].RegID=63;
    ChannelResponse[0].RegisterData[pos].RegValue=auxReg.ME_SDM_BR_ADDR << ME_SDM_BR_ADDR_SHIFT;
    pos++;

    // clear the remaining positions in response
    for ( ; pos<MAX_NUM_REGS_TO_RETURN; pos++)
    {
        ChannelResponse[0].RegisterData[pos].valid=false;
    }
#ifdef USE_CALLBACK
    // this aux regs has a response
    MEcallback->done_with_command(true);
#endif

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n--------------------\n");
    fprintf(vdump_fp, "CreateChannelCommandMB:\n");
    fprintf(vdump_fp, "    r0 = 0x%X\n", ChannelResponse[0].RegisterData[0].RegValue);
    fprintf(vdump_fp, "    r1 = 0x%X\n", ChannelResponse[0].RegisterData[1].RegValue);
    fprintf(vdump_fp, "   r63 = 0x%X\n", ChannelResponse[0].RegisterData[2].RegValue);
#endif // VERIFICATION_DUMP
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::CreateChannelCommandFinal
// Purpose:       Populate the MB channel response
// Arguments:     void
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::CreateChannelCommandFinal(void)
{
    // model a processing delay for the model i.e. a channel response will not
    // occur until after a certain number of calls to the response
    ChannelResponse[1].DelayCycles=CHAN_FINAL_DELAY;

    int pos=0;

    // R0 = ME cost
    ChannelResponse[1].RegisterData[pos].valid=true;
    ChannelResponse[1].RegisterData[pos].RegID=0;
    ChannelResponse[1].RegisterData[pos].RegValue=auxReg.ME_RESULT__COST_SAD;
    pos++;

    // only return block pattern if enabled
    if (auxReg.ME_SETUP__BLK_PAT_EN)
    {
        // R1 = block pattern
        ChannelResponse[1].RegisterData[pos].valid=true;
        ChannelResponse[1].RegisterData[pos].RegID=1;
        ChannelResponse[1].RegisterData[pos].RegValue=auxReg.ME_RESULT__BLK_PAT;
        pos++;
    }

    // R63 = branch address for choosen prediction mode or skip branch address when in manual sad mode
    ChannelResponse[1].RegisterData[pos].valid=true;
    ChannelResponse[1].RegisterData[pos].RegID=63;
    ChannelResponse[1].RegisterData[pos].RegValue=arcRetAddr;
    pos++;

    // clear the remaining positions in response
    for ( ; pos<MAX_NUM_REGS_TO_RETURN; pos++)
    {
        ChannelResponse[1].RegisterData[pos].valid=false;
    }

#ifdef USE_CALLBACK
    // this aux regs has a response
    MEcallback->done_with_command(true);
#endif

#ifdef VERIFICATION_DUMP
    fprintf(vdump_fp, "\n--------------------\n");
    fprintf(vdump_fp, "CreateChannelCommandFinal:\n");
    fprintf(vdump_fp, "    r0 = 0x%X\n", ChannelResponse[1].RegisterData[0].RegValue);
    if (auxReg.ME_SETUP__BLK_PAT_EN)
    {
        fprintf(vdump_fp, "    r1 = 0x%X\n", ChannelResponse[1].RegisterData[1].RegValue);
        fprintf(vdump_fp, "   r63 = 0x%X\n", ChannelResponse[1].RegisterData[2].RegValue);
    }
    else
    {
        fprintf(vdump_fp, "   r63 = 0x%X\n", ChannelResponse[1].RegisterData[1].RegValue);
    }
#endif // VERIFICATION_DUMP
}


#ifdef ARC_ISS_ME_MODEL
//******************************************************************************
// Name:          ArcMotionEstimationModel::SetSimulatorAccess
// Purpose:       Provides an access point to calling back to the simulator for
//                memory read and write operations
// Arguments:     acc = pointer to simulator access
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::SetSimulatorAccess(ARC_simext_simulator_access *acc)
{
   simAccess = acc;
}
#endif // ARC_ISS_ME_MODEL


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeMemoryWriteSDM
// Purpose:       Provides a mechanism for the ME to write a number of bytes to
//                the SDM
// Arguments:     dst = location to write to in the SDM
//                src = a buffer inside the ME model to read from
//                count = number of pieces of data to write
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeMemoryWriteSDM (u_int8 * dst, u_int8 * src, int count)
{
#ifdef ARC_ISS_ME_MODEL

#ifdef HARDWARE_ME
   simAccess->write_memory((unsigned long) dst + SDM_BASE_ADDRESS, src, sizeof(u_int8) * count, from_execution);
#else // HARDWARE_ME
   simAccess->write_memory((unsigned long) dst, src, sizeof(u_int8) * count, from_execution);
#endif // HARDWARE_ME

#else // ARC_ISS_ME_MODEL
   for (int i=0; i<count; i++)
   {
      *dst++=*src++;
   }
#endif // ARC_ISS_ME_MODEL
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeMemoryRead
// Purpose:       Provides a mechanism for the ME to read a number of bytes from
//                external memory
// Arguments:     dst = a buffer inside the ME model to write to
//                src = location to read from in external memory
//                count = number of pieces of data to read
// Return Value:  u_int8 = data read
//******************************************************************************
void ArcMotionEstimationModel::MeMemoryRead (u_int8 * dst, u_int8 * src, int count)
{
#ifdef ARC_ISS_ME_MODEL
   simAccess->read_memory((unsigned long) src, dst, sizeof(u_int8) * count, from_execution);
#else
   for (int i=0; i<count; i++)
   {
      *dst++=*src++;
   }
#endif
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeMemoryReadSDM
// Purpose:       Provides a mechanism for the ME to read a number of bytes from
//                the SDM
// Arguments:     dst = a buffer inside the ME model to write to
//                src = location to read from in the SDM
//                count = number of pieces of data to read
// Return Value:  u_int8 = data read
//******************************************************************************
void ArcMotionEstimationModel::MeMemoryReadSDM (u_int8 * dst, u_int8 * src, int count)
{
#ifdef ARC_ISS_ME_MODEL

#ifdef HARDWARE_ME
   simAccess->read_memory((unsigned long) src + SDM_BASE_ADDRESS, dst, sizeof(u_int8) * count, from_execution);
#else // HARDWARE_ME
   simAccess->read_memory((unsigned long) src, dst, sizeof(u_int8) * count, from_execution);
#endif // HARDWARE_ME

#else // ARC_ISS_ME_MODEL
   for (int i=0; i<count; i++)
   {
      *dst++=*src++;
   }
#endif // ARC_ISS_ME_MODEL
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeMemorySet
// Purpose:       Provides a mechanism for the ME to write a single byte from
//                external memory to multiple locations in an internal buffer
// Arguments:     dst = a buffer inside the ME model to write to
//                src = location of the value to read from in external memory
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeMemorySet (u_int8 * dst, u_int8 * src, int count)
{
   u_int8 value;

#ifdef ARC_ISS_ME_MODEL
   simAccess->read_memory((unsigned long) src, &value, 1, from_execution);
#else
   value=*src;
#endif

   memset(dst, value, count);
}


#ifdef ME_DUMP_CRR
//******************************************************************************
// Name:          ArcMotionEstimationModel::MeDumpCurrRefRes
// Purpose:       Dumps the current, reference and residual blocks to the me
//                verification dump file
// Arguments:     str = string to print out identifing the type of dump
//                maxX = width of block
//                maxY = height of block
//                curr = pointer to the current block
//                ref = pointer to the reference block
//                currStride = stride of the current block
//                refStride = stride of the reference block
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeDumpCurrRefRes (const char * str, int maxX, int maxY, u_int8 * curr, u_int8 * ref, int currStride, int refStride)
{
    fprintf(vdump_fp, "%s Current:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%02X ", *(curr+(y*currStride)+x));
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");

    fprintf(vdump_fp, "%s Reference:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%02X ", *(ref+(y*refStride)+x));
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");

    fprintf(vdump_fp, "%s Residual Signed:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%04X ", (*(curr+(y*currStride)+x) - *(ref+(y*refStride)+x)) & 0xffff);
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");
    fprintf(vdump_fp, "%s Residual Absolute:\n", str);
    int val;
    int sad=0;
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            val=ABS_TABLE(*(curr+(y*currStride)+x) - *(ref+(y*refStride)+x));
            sad+=val;
            fprintf(vdump_fp, "%04X ", val);
        }
        fprintf(vdump_fp, "   (%04X)\n", sad);
    }
    fprintf(vdump_fp, "\n");
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeDumpCurrRefRes
// Purpose:       Dumps the current, reference and residual blocks to the me
//                verification dump file
// Arguments:     str = string to print out identifing the type of dump
//                maxX = width of block
//                maxY = height of block
//                curr = pointer to the current block
//                ref = pointer to the reference block
//                currStride = stride of the current block
//                refStride = stride of the reference block
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeDumpCurrRefRes (const char * str, int maxX, int maxY, u_int8 * curr, int * ref, int currStride, int refStride)
{
    fprintf(vdump_fp, "%s Current:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%02X ", *(curr+(y*currStride)+x));
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");

    fprintf(vdump_fp, "%s Reference:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%02X ", *(ref+(y*refStride)+x));
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");

    fprintf(vdump_fp, "%s Residual Signed:\n", str);
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            fprintf(vdump_fp, "%04X ", (*(curr+(y*currStride)+x) - *(ref+(y*refStride)+x)) & 0xffff);
        }
        fprintf(vdump_fp, "\n");
    }
    fprintf(vdump_fp, "\n");
    fprintf(vdump_fp, "%s Residual Absolute:\n", str);
    int val;
    int sad=0;
    for (int y=0; y<maxY; y++)
    {
        for (int x=0; x<maxX; x++)
        {
            val=ABS_TABLE(*(curr+(y*currStride)+x) - *(ref+(y*refStride)+x));
            sad+=val;
            fprintf(vdump_fp, "%04X ", val);
        }
        fprintf(vdump_fp, "   (%04X)\n", sad);
    }
    fprintf(vdump_fp, "\n");
}
#endif // ME_DUMP_CRR


#ifdef ME_DUMP_SUBPEL
//******************************************************************************
// Name:          ArcMotionEstimationModel::MeDumpSubPel16x16
// Purpose:       Dumps all the sub-pel samples for a 16x16 block
// Arguments:     vecX = horizontal integer vector at 1/4-pel resolution
//                vecX = vertical integer vector at 1/4-pel resolution
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeDumpSubPel16x16 (int vecX, int vecY)
{
    int spRef[16*4][16*4];


    fprintf(vdump_fp, "Sub-pel MV = %d, %d\n\n", vecX, vecY);

    // generate all 16 sets of 1/4 pel values from the integer position
    for (int spLoopVert=0; spLoopVert < 4; spLoopVert++)
    {
        for (int spLoopHoriz=0; spLoopHoriz < 4; spLoopHoriz++)
        {
            // start at the integer position to the above left, so that we get the (-0.75, -0.75) offset
            mv_for_hor[0] = vecX-4+spLoopHoriz;
            mv_for_ver[0] = vecY-4+spLoopVert;

            // predict 16x16 array
            switch(auxReg.ME_SETUP__CODEC)
            {
                case H264:
                    ArcMakeH264InterPrediction(MBCT_H264_INTER16X16); // Note this also does the chroma so there is duplication of effort at present
                    break;
                case MPEG4:
                case H263:
                    ArcMakeH263InterPrediction(MBCT_MPEG4_INTER16X16);
                    break;
                default:
                    fprintf(stderr, "ArcMotionEstimationModel::MeDumpSubPel16x16 - Error: Unsupported Codec\n");
                    assert(false);
                    break;
            }

            // store results to overall sub-pel array
            for (int spLoopY=0; spLoopY < 16; spLoopY++)
            {
                for (int spLoopX=0; spLoopX < 16; spLoopX++)
                {
                    spRef[spLoopVert+(spLoopY*4)][spLoopHoriz+(spLoopX*4)]=y_prediction_data[(spLoopY*16)+spLoopX];
                }
            }

        }
    }


    // label horizontal axis
    fprintf(vdump_fp, "      ");
    for (int spLoopX=1; spLoopX < 1*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 3
    }
    for (int spLoopX=1*4; spLoopX < (10+1)*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 3
    }
    for (int spLoopX=(10+1)*4; spLoopX < 16*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "|%.1f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "|%.1f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "|%.1f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 3
    }
    fprintf(vdump_fp, "\n");
    fprintf(vdump_fp, "------");
    for (int spLoopX=1; spLoopX < 16*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 3
    }
    fprintf(vdump_fp, "\n");

    // dump out overall sub-pel array
    for (int spLoopY=1; spLoopY < 16*4; spLoopY++)
    {

#if ME_DUMP_SUBPEL > 1
        if ((spLoopY % 4) == 0)
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else if (spLoopY < (10+1)*4) fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopY % 4) == 2)
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else if (spLoopY < (10+1)*4) fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else if (spLoopY < (10+1)*4) fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 3

        for (int spLoopX=1; spLoopX < 16*4; spLoopX++)
        {
#if ME_DUMP_SUBPEL > 1
            if ((spLoopY % 4) == 0 && (spLoopX % 4) == 0)
            {
                fprintf(vdump_fp, "[%02X] ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
            else if (((spLoopY % 4) == 0 && (spLoopX % 4) == 2) ||
                     ((spLoopY % 4) == 2 && (spLoopX % 4) == 0) ||
                     ((spLoopY % 4) == 2 && (spLoopX % 4) == 2))
            {
                fprintf(vdump_fp, "(%02X) ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
            else
            {
                fprintf(vdump_fp, " %02X  ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 3
        }

#if ME_DUMP_SUBPEL > 1
            if ((spLoopY % 4) == 0) fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
            else if ((spLoopY % 4) == 2) fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
            else fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 3

    }

    fprintf(vdump_fp, "\n");
}


//******************************************************************************
// Name:          ArcMotionEstimationModel::MeDumpSubPel8x8
// Purpose:       Dumps all the sub-pel samples for a 8x8 block
// Arguments:     vecX = horizontal integer vector at 1/4-pel resolution
//                vecX = vertical integer vector at 1/4-pel resolution
//                block = 8x8 block number in a 16x16 macroblock
//                block_shape = block shape in a 16x16 macroblock
// Return Value:  void
//******************************************************************************
void ArcMotionEstimationModel::MeDumpSubPel8x8 (int vecX, int vecY, int block, int block_shape)
{
    int spRef[8*4][8*4];


    H264_16X16,
    H264_16X8,
    H264_8X16,
    H264_8X8,

    fprintf(vdump_fp, "Sub-pel MV = %d, %d : Block = %d : Block Shape = ", vecX, vecY, block);

    switch (block_shape)
    {
        case H264_16X8:
            fprintf(vdump_fp, "H264_16X8\n\n");
            break;
        case H264_8X16:
            fprintf(vdump_fp, "H264_8X16\n\n");
            break;
        case H264_8X8:
            fprintf(vdump_fp, "H264_8X8\n\n");
            break;
        default:
            fprintf(vdump_fp, "ERROR(Unsupported block shape)");
            break;
    }

    // generate all 16 sets of 1/4 pel values from the integer position
    for (int spLoopVert=0; spLoopVert < 4; spLoopVert++)
    {
        for (int spLoopHoriz=0; spLoopHoriz < 4; spLoopHoriz++)
        {
            // start at the integer position to the above left, so that we get the (-0.75, -0.75) offset
            mv_for_hor[0] = vecX-4+spLoopHoriz;
            mv_for_ver[0] = vecY-4+spLoopVert;

            // predict 8x8 array
            switch(auxReg.ME_SETUP__CODEC)
            {
                case H264:
                    ArcMakeH264InterPrediction8(block, block_shape); // goes into quadrant of
                    break;
                case MPEG4:
                case H263:
                    ArcMakeH263InterPrediction8(block);
                    break;
                default:
                    fprintf(stderr, "ArcMotionEstimationModel::MeDumpSubPel8x8 - Error: Unsupported Codec\n");
                    assert(false);
                    break;
            }

            // store results to overall sub-pel array
            for (int spLoopY=0; spLoopY < 8; spLoopY++)
            {
                for (int spLoopX=0; spLoopX < 8; spLoopX++)
                {
                    spRef[spLoopVert+(spLoopY*4)][spLoopHoriz+(spLoopX*4)]=y_prediction_data[(spLoopY*8)+spLoopX];
                }
            }

        }
    }


    // label horizontal axis
    fprintf(vdump_fp, "      ");
    for (int spLoopX=1; spLoopX < 1*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 3
    }
    for (int spLoopX=1*4; spLoopX < 8*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "|%.2f", (spLoopX/4.0f)-1.0f);
#endif // ME_DUMP_SUBPEL > 3
    }
    fprintf(vdump_fp, "\n");
    fprintf(vdump_fp, "------");
    for (int spLoopX=1; spLoopX < 8*4; spLoopX++)
    {
#if ME_DUMP_SUBPEL > 1
        if ((spLoopX % 4) == 0) fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopX % 4) == 2) fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else fprintf(vdump_fp, "-----");
#endif // ME_DUMP_SUBPEL > 3
    }
    fprintf(vdump_fp, "\n");

    // dump out overall sub-pel array
    for (int spLoopY=1; spLoopY < 8*4; spLoopY++)
    {

#if ME_DUMP_SUBPEL > 1
        if ((spLoopY % 4) == 0)
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
        else if ((spLoopY % 4) == 2)
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
        else
        {
            if (spLoopY < 1*4) fprintf(vdump_fp, "%.2f |", (spLoopY/4.0f)-1.0f);
            else fprintf(vdump_fp, " %.2f |", (spLoopY/4.0f)-1.0f);
        }
#endif // ME_DUMP_SUBPEL > 3

        for (int spLoopX=1; spLoopX < 8*4; spLoopX++)
        {
#if ME_DUMP_SUBPEL > 1
            if ((spLoopY % 4) == 0 && (spLoopX % 4) == 0)
            {
                fprintf(vdump_fp, "[%02X] ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
            else if (((spLoopY % 4) == 0 && (spLoopX % 4) == 2) ||
                     ((spLoopY % 4) == 2 && (spLoopX % 4) == 0) ||
                     ((spLoopY % 4) == 2 && (spLoopX % 4) == 2))
            {
                fprintf(vdump_fp, "(%02X) ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
            else
            {
                fprintf(vdump_fp, " %02X  ", spRef[spLoopY][spLoopX]);
            }
#endif // ME_DUMP_SUBPEL > 3
        }

#if ME_DUMP_SUBPEL > 1
            if ((spLoopY % 4) == 0) fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 1
#if ME_DUMP_SUBPEL > 2
            else if ((spLoopY % 4) == 2) fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 2
#if ME_DUMP_SUBPEL > 3
            else fprintf(vdump_fp, "\n");
#endif // ME_DUMP_SUBPEL > 3

    }

    fprintf(vdump_fp, "\n");
}
#endif // ME_DUMP_SUBPEL
