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

#ifndef HARDWARE_EE // Only compile this when the model is required

#include "ArcMPC.h"
#include "ArcEntropyEncoderModel.h"
#include "stdlib.h"
#include "defines.h"
#include "EnumTypes.h"
#include "ArcEEtables.h"
#include "ArcEEOffsetTables.h"
#include <memory.h>

typedef class cMPCcb* MPCPtr;

const short CoffCntsBitPos[24] = {0, 5, 10,15,20,25,
                                  32,37,42,47,52,57,
                                  64,69,74,79,84,89,
                                  96,101,106,111,116,121};

#define U_BIT_POSTIONS &CoffCntsBitPos[16]
#define V_BIT_POSTIONS &CoffCntsBitPos[20]

// HACK Need to get this info from ISS
#ifdef HARDWARE_EE_
    #define SDM_BASE 0xf00000
#else
    #define SDM_BASE 0
#endif

// ==========================================================================
//  Global instance of ArcMotionEstimationInternals==========================
#ifndef ARC_ISS_EE_MODEL
   ArcEntropyEncoderModel gArcEntropyEncoderModel;
#endif


#ifndef ARC_ISS_EE_MODEL
   #define ReadMPCData(src,dst,amount) {memcpy(dst,src,amount);}
   #define WriteMPCData(index) {((class cMPCcb *)EE_AUX_REGS[CURR_MPC_PTR])->CoeffCountSB[index] = MpcCurrentPtr->CoeffCountSB[index];}
#else
   #define ReadMPCData(src,dst,amount) {unsigned long ret = \
   access->read_memory((unsigned long)src,dst,amount,from_execution);}
   
   #define WriteMPCData(index) { u_int32 val  = MpcCurrentPtr->CoeffCountSB[index]; \
                                 unsigned long addr = (unsigned long)&(((class cMPCcb *)(SDM_BASE+EE_AUX_REGS[CURR_MPC_PTR]))->CoeffCountSB[index]); \
                                 access->write_memory(addr,&val,sizeof(u_int32),from_execution); \
                               }

#endif
// ==========================================================================

const unsigned char top_left_of_luma_4x4_for_mpc[16] =
	{   0,   1,  4,  5, 
		2,   3,  6,  7,
	    8,   9, 12, 13,
	   10,  11, 14, 15};


// Constructor
ArcEntropyEncoderModel::ArcEntropyEncoderModel()
{
         
   EE_AUX_REGS[EE_VCONFIG]    = 0x58040201;
   EE_AUX_REGS[EE_NAMESTR0]   = 0x4545;
   EE_AUX_REGS[EE_BUILD]      = 0x1;
   ResetEEModule();
#ifdef EE_DUMP
   verif_log = fopen("EEModelData.txt","w");
#endif
}

// Destructor
ArcEntropyEncoderModel::~ArcEntropyEncoderModel()
{
   delete MpcCurrentPtr;
   delete MpcTopPtr;
   delete MpcLeftPtr ;
}


void ArcEntropyEncoderModel::ResetEEModule(void)
{
     for(int i =3; i<=EE_DEBUG_LOC; i++)
      EE_AUX_REGS[i] = 0;

   cbp               = 0x0;
   MbNumber          = 0x0;
   FoundFirstCoeff   = false;
	RunCount          = 0;
	FoundFirstNonT1   = false;
	Coeff             = 0;
   CoefficientCount  = 0;
	TotalCoeffs       = 0;


   NumberOfTrailingOnes       = 0;;
   CoeffTokenTable            = 0;
	TotalZeroes                = 0;

   for(int i=0; i<10;i++)
      ChToAuxRegMap[i] = i+4;
   
   for(int i=10;i<15;i++)
      ChToAuxRegMap[i] = i+10;

    for(int i=15;i<20;i++)
      ChToAuxRegMap[i] = i+15;

    ChToAuxRegMap[20] = 37;

    for(int i=21;i<27;i++)
      ChToAuxRegMap[i] = i+20;
    
    
    ChToAuxRegMap[27] = EE_DEBUG;

    ChToAuxRegMap[30] = ADD_BITS;
    ChannelResponse.DelayCycles = 0;
    ChannelResponse.RegisterData[0].RegID             = Reg_R0;
    ChannelResponse.RegisterData[1].RegID             = Reg_R1;
    ChannelResponse.RegisterData[2].RegID             = Reg_R2;
    ChannelResponse.RegisterData[3].RegID             = Reg_R63;

    for(int i=0;i<MAX_NUM_REGS_TO_RETURN;i++)
       ChannelResponse.RegisterData[i].valid = false;

    EEHalted = false;

   MpcCurrentPtr  = new VlcMpcData;
   MpcTopPtr      = new TopVlcMpcData;
   MpcLeftPtr     = new VlcMpcData;

   Mp4TableInit();
}



// Used to populate the local copies of the MPC data structure
// This is needed as in ISS model there will be access functions
// to do this
void ArcEntropyEncoderModel::PopulateMPCDataStruct(int id)
{
   class cMPCcb *src = (class cMPCcb *)(EE_AUX_REGS[id]+SDM_BASE);
   
   VlcMpcData *dst = MpcCurrentPtr;
   switch (id - CURR_MPC_PTR)
   {
      case 2:
      {
         dst = MpcLeftPtr;
         break;
      }
      default:
      {
        break;
      }
   }

   CopyVLCSpecificMPCData(src,dst);
}


void ArcEntropyEncoderModel::PopulateTopMPCDataStruct(int id)
{
  class cMPCfb *src = (class cMPCfb *)(EE_AUX_REGS[id] + SDM_BASE);
  ReadMPCData(&(src->Intra4x4ModeCoeffCountSB[2]),MpcTopPtr->Intra4x4ModeCoeffCountSB,2*sizeof(u_int32))
  ReadMPCData(&(src->CollectionID),&(MpcTopPtr->CollectionID),1*sizeof(u_int8))
}


void ArcEntropyEncoderModel::CopyVLCSpecificMPCData(class cMPCcb *src,VlcMpcData *dst)
{
   // Copy coefficients
   // Read the Y transformed coefficientscoefficient
   ReadMPCData((u_int8 *)(src->PixelCoeffBuffer+PCB_TRANSFORM_Y),(u_int8 *)(dst->PixelCoeffBuffer),512*sizeof(u_int8))
   
   // Read U and V transformed coefficients
   u_int8 * srcAddr   = (u_int8 *)(src->PixelCoeffBuffer+PCB_TRANSFORM_U);
   u_int8 * u_dstAddr = (u_int8 *)(dst->PixelCoeffBuffer + PCB_VLC_TRANSFORM_U);
   u_int8 * v_dstAddr = (u_int8 *)(dst->PixelCoeffBuffer + PCB_VLC_TRANSFORM_V);
   for(int i=0; i<8;i++)
   {
	  
	 // for(int k=0; k<16;k++)
	  {
	    ReadMPCData(srcAddr,u_dstAddr,16*sizeof(u_int8))
		ReadMPCData((u_int8 *)(srcAddr+16),v_dstAddr,16*sizeof(u_int8))
	  }
	  u_dstAddr += 16;
	  v_dstAddr += 16;
	  srcAddr   += 32;

   }

   ReadMPCData(src->Y_DC_CoeffStore0,dst->Y_DC_CoeffStore0,8*sizeof(int16))
   ReadMPCData(src->Y_DC_CoeffStore1,dst->Y_DC_CoeffStore1,8*sizeof(int16))

   ReadMPCData(src->U_DC_CoeffStore,dst->U_DC_CoeffStore,4*sizeof(int16))
   ReadMPCData(src->V_DC_CoeffStore,dst->V_DC_CoeffStore,4*sizeof(int16))
   
   ReadMPCData(&src->NonZeroCoeffLuma,&dst->NonZeroCoeffLuma,sizeof(u_int16))
   ReadMPCData(&src->NonZeroCoeffChroma,&dst->NonZeroCoeffChroma,sizeof(u_int8))
   ReadMPCData(&src->UV_DC_NonZeroCoeff,&dst->UV_DC_NonZeroCoeff,sizeof(u_int8))
   ReadMPCData(&src->QuantValue,&dst->QuantValue,sizeof(u_int8))
   ReadMPCData(&src->IntraMode,&dst->IntraMode,sizeof(u_int8))
   ReadMPCData(&src->MBCodingType,&dst->MBCodingType,sizeof(u_int8))
   
   ReadMPCData(&src->mv,&dst->mv,4*sizeof(MV_t))
   ReadMPCData(&src->mvp,&dst->mvp,4*sizeof(MV_t))
   
   ReadMPCData(&src->CollectionID,&dst->CollectionID,sizeof(u_int8))
   ReadMPCData(&src->LeftMB,&dst->LeftMB,sizeof(NeighbourFlags_t))
   ReadMPCData(&src->AboveMB,&dst->AboveMB,sizeof(NeighbourFlags_t))

   ReadMPCData(&src->CoeffCountSB,&dst->CoeffCountSB, 4*sizeof(u_int32))
}

// Writes to the auxiliary registers

void ArcEntropyEncoderModel::WriteAuxRegister(unsigned int id, unsigned int val)
{
   
   // Read only
   if(id < 3)
      return;

   if(id > EE_DEBUG)
   {
     // printf("FATAL Error RegId%d\n Not in EE module",id);
      return;
   }

   if(id >= ADD_BITS && id < EE_DEBUG)
   {
#ifdef EE_DUMP      
     //fprintf(verif_log, "\n--------------------------------------------------------\n");
     //fprintf(verif_log, "WRITING TO ADD_%d_BITS WITH DATA = 0x%x\n", (id - ADD_BIT_OFF), val);
     fflush(verif_log);
#endif
      // ADD bit functionality
      WriteFixedLengthCode(val,(id - ADD_BIT_OFF));
      return;
   }

   switch(id)
   {
      case P0_CUR_ADDR:
      case P1_CUR_ADDR:
      case P2_CUR_ADDR:
      case P3_CUR_ADDR:
      case CONCAT_CUR_ADDR:
      case CONCAT_DI_ADR:
      {
         EE_AUX_REGS[id] = val & 0xFFFFFFFC;
         break;
      }
      case MPC_Y_OFFSET:
      {
          val &= 0x01FF0;
          EE_AUX_REGS[id] = val;
          break;
      }
      case BUFFER_CTRL:
      {
         WriteToBufferCtrlReg(val);
         break;
      }
      case PACK_TO_BYTE:
      {
         WritePackToByteRegister(val);
         break;
      }
      case CONCAT_SEL:
      {
         WriteToConcatSelRegister(val);
         break;
      }
      case DELTA_QUANT:
      {
         if(val &0x40)
           val |= 0xFFFFFFC0;
         EE_AUX_REGS[id] = val;
         break;
      }
      case SLICE_TYPE:
      case EMU_CTRL:
      {
         EE_AUX_REGS[id] = val & 0x1;
         break;
      }
      case UNIT_SEL:
      {
         #ifdef EE_DUMP
           //fprintf(verif_log, "\n---------------------------\n");
           //fprintf(verif_log, "WRITING UNIT_SEL TO %d\n", val & 0x07);
           //fprintf(verif_log, "---------------------------\n");
           fflush(verif_log);
         #endif
         EE_AUX_REGS[id] = val & 0x07;
         break;
      }
      case ADD_UEV:
      {
         WriteToAddUevRegister(val);
         break;
      }
      case ADD_SEV:
      {
         WriteToAddSevRegister(val);
         break;
      }
      case ADD_MEV_INTRA:
      {
         WriteToAddUevRegister(h264_encoder_intra_cbp_table[val]);
		 WRITE_ENCODED_SYMBOL("Intra CBP", h264_encoder_intra_cbp_table[val])
         break;
      }
      case ADD_MEV_INTER:
      {
         WriteToAddUevRegister(h264_encoder_inter_cbp_table[val]);
		 WRITE_ENCODED_SYMBOL("Inter CBP", h264_encoder_inter_cbp_table[val])
         break;
      }
      case CH_FL_ADR:
      case CH_MBE_ADR:
      case CH_CALL_ADR:
      {
        #ifdef HARDWARE_EE_
            val &= 0xFFFFFFFE;
        #endif
        EE_AUX_REGS[id] = val;
        break;
      }
      case MB_BIT_CNT:
      {
         EE_AUX_REGS[id] = (val & 0xFFF);
         break;
      }
      case SKIP_CNT:
      {
         EE_AUX_REGS[id] = (val & 0xFFFF);
         break;
      }
      case CONCAT_BYTE_CNT:
      {
         EE_AUX_REGS[id] = (val & 0xFFFFF);
         break;
      }
      case EMU_BYTE_CNT:
      {
         EE_AUX_REGS[id] = val & 0xFF;
         break;
      }
      case ADD_SKIPRUN:
      {
         // Hardware needs change like this
          if((EE_AUX_REGS[SKIP_CNT] != 0) || ((val & 0x01) == 1))
            GenerateUnsignedExpGolombCode(EE_AUX_REGS[SKIP_CNT]);

         break;
      }
      case EE_DEBUG:
      {

         // check if the SIH bit is set then only call the macroblock to
         // be encoded
         if((EE_AUX_REGS[EE_DEBUG_LOC] & 0x10000) && !(val & 0x100000))
         {
            EntropyEncodeMacroBlock();
         }
      

         EE_AUX_REGS[EE_DEBUG_LOC] = val;
         break;

      }
      case RESTORE:
      {
         int unit_sel = EE_AUX_REGS[UNIT_SEL];
         EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET]   = EE_AUX_REGS[BUF_STORE];
         EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET]  = EE_AUX_REGS[CURR_ADDR_STORE]; 
         EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET] = EE_AUX_REGS[STATUS_STORE];
         EE_AUX_REGS[EMU_BUFF]                     = EE_AUX_REGS[EMU_BUFF_STORE];
         EE_AUX_REGS[MB_BIT_CNT]                   = EE_AUX_REGS[MB_BIT_CNT_STORE];
         EE_AUX_REGS[SKIP_CNT]                     = EE_AUX_REGS[SKIP_CNT_STORE];
         EE_AUX_REGS[CONCAT_BYTE_CNT]              = EE_AUX_REGS[CN_BYTE_CNT_STORE];
         EE_AUX_REGS[EMU_BYTE_CNT]                 = EE_AUX_REGS[EMU_BYTE_CNT_STORE];
         break;
      }
      case CH_CALL:
      {
         // If the encoding is finished then return the response
         if(!(EE_AUX_REGS[STATUS_CTRL]&0x100))
         {
            ChannelResponse.RegisterData[0].RegValue     = EE_AUX_REGS[UNIT_SEL];
            
            ChannelResponse.RegisterData[0].valid        = true;

            ChannelResponse.RegisterData[1].RegValue     = EE_AUX_REGS[MB_BIT_CNT];
            ChannelResponse.RegisterData[1].valid        = true;

            if(EE_AUX_REGS[UNIT_SEL] == CONCAT_UNIT)
            {
               ChannelResponse.RegisterData[2].RegValue  = EE_AUX_REGS[EMU_BYTE_CNT];
               ChannelResponse.RegisterData[2].valid     = true;
            }

            ChannelResponse.RegisterData[3].RegValue     = EE_AUX_REGS[CH_CALL_ADR];
            ChannelResponse.RegisterData[3].valid        = true;

            HasChannelResponse = true;
         }
         break;
      }
      case CURR_MPC_PTR:
      case TOP_MPC_PTR:
      case LEFT_MPC_PTR:
      {
         #ifdef HARDWARE_EE_
            val &= 0x1FFF0;
         #endif

         EE_AUX_REGS[id] = val;
         // If it is a valid pointer then update
         if(val)
         {
           if(id == TOP_MPC_PTR)
            PopulateTopMPCDataStruct(id);
           else
            PopulateMPCDataStruct(id);
         }
         break;
      }
      default:
      {
         if(id <= ADD_BIT_OFF)
            EE_AUX_REGS[id] = val;
         break;
      }

   }
}

unsigned int ArcEntropyEncoderModel::ReadAuxRegister(unsigned int id)
{
   if(id > EE_DEBUG)
   {
     // printf("FATAL Error RegId%d\n Not in EE module",id);
      return 0;
   }


   if((id >= ADD_BITS && id < EE_DEBUG) 
      || (id >= ADD_UEV && id <= ADD_MEV_INTER )
      || (id == PACK_TO_BYTE))
   {
      return 0;
   }

   if(id == DELTA_QUANT)
       return (EE_AUX_REGS[id] & 0x7F);
   
   if(id == EE_DEBUG)
      return  EE_AUX_REGS[EE_DEBUG_LOC];

   return EE_AUX_REGS[id];
}


// This function will be called by ARC processor for setting up all 
// the inward channel registers
void ArcEntropyEncoderModel::SendChannelCommand(unsigned int ch_data,unsigned int id_cmd)
{
   // Ignore the channel ID for ISS
   int tmp = id_cmd & CHANNEL_CMD_MASK;
   if((tmp -CH_ADD_BIT_OFFSET)>32)
   {
      //printf("FATAL error Adding bits more than 32 is not supported");
      return;
   }
   
   HasChannelResponse = false;
   // Reg ID above 50 is add bit functionality
   (tmp > CH_ADD_BIT_OFFSET)? WriteFixedLengthCode(ch_data,(tmp - CH_ADD_BIT_OFFSET)) : WriteAuxRegister(ChToAuxRegMap[tmp],ch_data);   
   #ifdef CALL_BACK_METHOD
      EEcallback->done_with_command(HasChannelResponse);
   #endif
}






// This functions the model of the Bit packer and bit packing
// functionality of the concatination unit.
// There are 4 Bit packer units and one concatination unit, each of which operates independantly
// Each unit works on it's own 32 bit output buffer and packed 32 bit
// values are written back to the main memory and pointed to by the respective
// CUR_ADDR. The current address is also updated after each write.
// Before the main memory write the 32 bit value is converted into little
// endian format
// Each unit has it's own status register which keeps track of the valid
// bits in the packed buffer
// Concatination unit will do the start code prevention functionality
// if it is enabled which is handeled when the writing the bits

// Unit select decide which packer or cocatination unit will be used
void ArcEntropyEncoderModel::BitPacker(unsigned int value,unsigned int length)
{
   // Check if the length to be packed and the already packed bits exceed 32 bits
   // On exceeding 32 bits, the newly 32 bits will be written to the main memory
   // and the remaining bits will be packed in
   int unit_sel = EE_AUX_REGS[UNIT_SEL];
   unsigned int bits_remaining;
   unsigned int remaining_val;
   unsigned int remaining_len;
   unsigned int prv_pkd_bits = EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET] & 0x1F;    // Last 5 bits is the packed bit count
   
   do
   {
    bits_remaining = 32 - prv_pkd_bits;
    remaining_val = 0;
    remaining_len = 0;

     EE_AUX_REGS[MB_BIT_CNT] += length; // Update the bit count
     if(EE_AUX_REGS[MB_BIT_CNT] > 4095)
        EE_AUX_REGS[MB_BIT_CNT] = 4095;
  
/*#ifdef EE_DUMP
  if (length <= 4 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%01X\n", value);
  }
  else if (length <= 8 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%02X\n", value);
  }
  else if (length <= 12 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%03X\n", value);
  }
  else if (length <= 16 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%04X\n", value);
  }
  else if (length <= 20 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%05X\n", value);
  }
  else if (length <= 24 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%06X\n", value);
  }
  else if (length <= 28 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%07X\n", value);
  }
  else if (length <= 32 ) {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%08X\n", value);
  }
  else {
    fprintf(verif_log, "VALUE ADDED TO BUFFER = 0x%08X\n", value);
  };
  fprintf(verif_log, "LENGTH OF VALUE ADDED TO BUFFER = %d BITS\n", length);
#endif*/

     if(length > bits_remaining)
     {
        remaining_len = length - bits_remaining;
        remaining_val = (value << (32 - remaining_len));
        length        = bits_remaining;
     }
     
     unsigned int tmp = 0;

     // Hack for shifting 32 bits as when the remaining_len is 32 or more
     // then the (value >> remaining_len) shift produces the same 
     // result as value rather than zero
     if(remaining_len < 32)
      tmp = ((value >> remaining_len) << (bits_remaining-length));

     EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET] |= tmp;
     prv_pkd_bits += length;

#ifdef EE_DUMP
  //fprintf(verif_log, "P0 BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[0 + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "P1 BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[1 + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "P2 BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[2 + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "P3 BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[3 + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "Cn BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[4 + BUFF_REG_OFFSET]);
       
  //fprintf(verif_log, "BUFFER CONTENTS (LEFT ALIGNED) = 0x%08X\n", EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "NUMBER OF VALID BITS IN BUFFER = %d\n", prv_pkd_bits);
  fflush(verif_log);
#endif

     // Set the CN_EP bits of concat_status register.
     // Used for each bit is set to high, it indicates 
     // that emulation prevention is to be performed using that byte
     if(EE_AUX_REGS[unit_sel] == CONCAT_UNIT &&
        EE_AUX_REGS[EMU_CTRL] &&!(prv_pkd_bits%8))
     {
        EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET] |= 1 << (CN_EP_START - prv_pkd_bits/8);
     }


     if(prv_pkd_bits == 32)
     {
#ifdef EE_DUMP
  wr_cnt += 1;     
  //fprintf(verif_log, "\n********************************************************\n");
  //fprintf(verif_log, "32 BITS IN BUFFER: WRITE TO SYS MEM NUMBER %d WITH DATA = 0x%08X\n", wr_cnt, EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET]);
  //fprintf(verif_log, "********************************************************\n");
  //fprintf(verif_log, "MB_BIT_CNT VALUE FOLLOWING WRITE TO MEMORY = %d\n", EE_AUX_REGS[MB_BIT_CNT]);
 // fprintf(verif_log, "********************************************************\n\n");
  fflush(verif_log);
#endif
        // write to the memory pointed by the packer current address
        WritePackedBits(EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET]);
        prv_pkd_bits = 0;
        EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET] = 0;
     }

     if(remaining_len)
     {
        // Rewrite to the buffer
        EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET] = remaining_val;
        prv_pkd_bits = remaining_len;
        
#ifdef EE_DUMP
        //fprintf(verif_log, "NUMBER OF BITS REMAINING IN BUFFER FOLLOWING WRITE = %d BITS\n", remaining_len);
        //fprintf(verif_log, "VALUE OF BUFFER FOLLOWING WRITE = 0x%08X\n", remaining_val);
        fflush(verif_log);
#endif
     }

     EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET] = (EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET] & 0x3E0)| prv_pkd_bits;
     length = 0;
   
   }while(prv_pkd_bits >= 32);
}







// Writes 32 bit data buffer data to the main memory.
// H/W will do a DMA out for this.
// Memory address is pointed to by the packer current address.
// The address will get incremented if there are 4 bytes written flag is set
// else the address will not be incremented. Address will not be incrementd
// on a flush command
void ArcEntropyEncoderModel::WritePackedBits(unsigned int value, int bytes_valid)
{

   int leading_zeros = ((EE_AUX_REGS[EMU_BUFF]&0x8)>>3)+((EE_AUX_REGS[EMU_BUFF]&0x10)>>4);

   int tot_buff_valid = (EE_AUX_REGS[EMU_BUFF]&0x1)+
                        ((EE_AUX_REGS[EMU_BUFF]&0x2)>> 1)+
                        ((EE_AUX_REGS[EMU_BUFF]&0x4)>> 2);

   // If it is test only mode, do not write anything
   if(EE_AUX_REGS[BUFFER_CTRL] & 0x20)
      return;

   bytes_valid +=tot_buff_valid;

   // No writes if there are no bytes valid
   if(!bytes_valid)
      return;

   unsigned char val;
   int byte_pos=3;

   unsigned char byte_buffer[3];
   int prv_leading_zeros = 0;

   for(int i=0;i<3;i++)
      byte_buffer[i] = EE_AUX_REGS[EMU_BUFF] >> (8*(i+1));
   
   int unit_sel = EE_AUX_REGS[UNIT_SEL];
   do
   {
      int bytes_written = 0;
      unsigned int final_val = 0;
      while(bytes_written<4)
      {
         bool inc = false;
         bool shiftCnBuff = false;
         val = (value >> 8*byte_pos);
         bool emu_valid = false;
         if(EE_AUX_REGS[EMU_CTRL] && EE_AUX_REGS[unit_sel] == CONCAT_UNIT 
            && (EE_AUX_REGS[unit_sel+STATUS_REG_OFFSET] >> ((6+byte_pos) & 0x1)))
         {
            emu_valid =true;
         }
  
         byte_pos--;
         // Start code prvention logic
         if(emu_valid || tot_buff_valid)
         {
            // Previously valid bytes from EMU unit
            if(tot_buff_valid)
            {
               tot_buff_valid--;
               val = byte_buffer[2];
               shiftCnBuff = true;
               inc = true;
            }
            if(leading_zeros == 2 )
            {
               // byte prevention
               if(!(val & 0xFC))
               {
                  // insert start code prevention byte
                  val = 0x3;
                  bytes_valid++;

                  if(inc)
                  {
                     tot_buff_valid++;
                     shiftCnBuff = false;
                  }

                  EE_AUX_REGS[EMU_BYTE_CNT]++;
/*#ifdef EE_DUMP
  fprintf(verif_log, "********************************************************\n");
  fprintf(verif_log, "EMU_BYTE_CNT INCREMENTED TO A VALUE OF %d\n", EE_AUX_REGS[EMU_BYTE_CNT]);
  fprintf(verif_log, "********************************************************\n\n");
#endif*/

                  //Saturation
                  if(EE_AUX_REGS[EMU_BYTE_CNT] > 255)
                     EE_AUX_REGS[EMU_BYTE_CNT] = 255;

                  inc = true;
               }
               leading_zeros = 0;
            }
            else
               (val == 0) ? leading_zeros++ : leading_zeros=0;

            if(inc)
            {
               byte_pos++;
            }

            // Adjust the EMUInt buffer
            if(tot_buff_valid && shiftCnBuff)
            {
               for(int i=2;i>0;i--)
                  byte_buffer[i] = byte_buffer[i-1];
            }
         }

        final_val |= val << ((3-bytes_written)*8);
        bytes_written++;
        if((--bytes_valid)==0)
          break;
      }  
      if(bytes_written == 4)
      {
         prv_leading_zeros = leading_zeros;
      }
      else if(!(EE_AUX_REGS[BUFFER_CTRL] &0x2))
      {
         bytes_valid    = bytes_written;
         byte_pos       = (bytes_written - 1);
         leading_zeros  = prv_leading_zeros;
         break;
      }

      // Do the endian correction
      DoEndianCorrection(&final_val);

      #ifdef ARC_ISS_EE_MODEL
         int ret = access->write_memory((unsigned long)(EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET]),&final_val,4,from_execution);
         if(ret != 4)  
          //  printf("FATAL ERROR! could not write to memory location %x\n",EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET]);
      #else
         *((unsigned int *)(EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET])) = final_val;
         //printf("FINAL VALUE =%x\n",final_val);
         //fflush(stdout);
      #endif
      if(unit_sel == CONCAT_UNIT)
         EE_AUX_REGS[CONCAT_BYTE_CNT] += bytes_written;

      // Saturate
      if(EE_AUX_REGS[CONCAT_BYTE_CNT] > 0xFFFFF)
         EE_AUX_REGS[CONCAT_BYTE_CNT] = 0xFFFFF;

      // No address increment if full 4 bytes are not written
      // else do not increment the address
      if(bytes_written == 4)
         EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET] += 4;

   }
   while(bytes_valid > 2);   // Do this repeteadly if more than one 32 bit data is 
   
   // 32 bits done check if all the bytes have been written if not then
   // store the last byte into the cancat internal buffer
   // This should be done only if the packer unit staus is not flushed
   // On being flushed all the bits have already been written into
   EE_AUX_REGS[EMU_BUFF] = 0x0;
   while(bytes_valid > 0 )
   {
      val = (value >> 8*byte_pos);
      //CnInternalBuffer[3-tot_buff_valid]  =   val;
      
      // 31	24	23	16	15	 8	7		 5	 4	 3	2	1	0
      // BYTE0	BYTE1	BYTE2	E0	E1	E2	ZB	ZA	V0	V1	V2

      EE_AUX_REGS[EMU_BUFF]               |=  val << ((3-tot_buff_valid)*8); // Byte0 ..
      EE_AUX_REGS[EMU_BUFF]               |=  1 << (2-tot_buff_valid);  // V0...
      EE_AUX_REGS[EMU_BUFF]               |=  1 << (7-tot_buff_valid);  // E0 .. Enable EMU
      tot_buff_valid++;
      bytes_valid--;
      byte_pos--;
   }

   // Store the leading zeros
   while(leading_zeros > 0)
   {
      leading_zeros--;
      EE_AUX_REGS[EMU_BUFF]   |= 1<<(4-leading_zeros);  // Za , Zb
   }
}




int ArcEntropyEncoderModel::WritePackToByteRegister(unsigned int val,bool update_status)
{
   EE_AUX_REGS[PACK_TO_BYTE] = val & 0x1;
   int unit_sel = EE_AUX_REGS[UNIT_SEL];
   unsigned int valid_bits = EE_AUX_REGS[STATUS_REG_OFFSET + unit_sel] & 0x1F;
   unsigned int bits_remaining = 32 - valid_bits;

   // check for byte aligned
   // find out how many bits in the byte has to be packed
   unsigned int bits_to_pack;
   bits_to_pack = bits_remaining % 8;

   // Already byte aligned
   if(!bits_to_pack || !update_status)
      return bits_to_pack;

   EE_AUX_REGS[MB_BIT_CNT] += bits_to_pack;

   if(EE_AUX_REGS[MB_BIT_CNT] > 4095)
      EE_AUX_REGS[MB_BIT_CNT] = 4095;

   // Zero all the remaining bits
   EE_AUX_REGS[BUFF_REG_OFFSET + unit_sel] >>= bits_remaining;
   EE_AUX_REGS[BUFF_REG_OFFSET + unit_sel] <<= bits_remaining;
   unsigned int pack_val = 0xFFFFFFFF;

   if(!EE_AUX_REGS[PACK_TO_BYTE])
   {
        pack_val = 0x0;
   }
   else
   {
        pack_val >>= (32 - bits_to_pack);
        pack_val <<= (32 - (valid_bits+bits_to_pack));
   }

   // Do not care about all 32 bits being written as while writing back to memory
   // we use the valid bits
   // A corner CASE. Packer status does not have a count for 32 valid bits
   // so if after packing to byte boundary if the total valid bits becomes 32
   // write back to main memory and do not increase the address as this will be the
   // last call before flush
   int tot = valid_bits + bits_to_pack;
   EE_AUX_REGS[BUFF_REG_OFFSET + unit_sel]  |=  pack_val;

   if(32 == tot)
   {
      WritePackedBits(EE_AUX_REGS[BUFF_REG_OFFSET + unit_sel]);
      tot = 0;
   }

   EE_AUX_REGS[STATUS_REG_OFFSET + unit_sel]    = (EE_AUX_REGS[STATUS_REG_OFFSET + unit_sel] & 0x20)|tot;

   return 0;
}

// This is the H/W implementation of ADD_X_Bits
// Since these registers are write only no need of updating the
// register valuse.
// lenght parameter will be the number of bits to be written
void ArcEntropyEncoderModel::WriteFixedLengthCode(unsigned int vaue,unsigned int length)
{
   if(length == 0)
      return;
   BitPacker(vaue,length);
}


// Does the endian correction

void ArcEntropyEncoderModel::DoEndianCorrection(unsigned int *ptr)
{
   unsigned int val = *ptr;
   unsigned int correct_val = 0;
   if(!(EE_AUX_REGS[STATUS_CTRL] & ENDIAN_MASK))
   {
      for(int i=3; i>=0;i--)
      {
         correct_val |= (((val >> 8*i) & 0xFF) << 8*(3-i));
      }

   }
   else 
   {
     correct_val = val;
   }

   *ptr = correct_val;
}

//====================================================================================================
//======================================BUFFER_CTRL REG OPERATIONS====================================
//====================================================================================================


// The BUFFER_CTRL  register is used to control the buffers in the
// selected packer or concatenation unit, and also to kick of a MB encoding process.
// The packer or concat unit is selected depending on the value of
// UNIT_SEL value.
// The Bit defination of the register is
//     6  5 4  3	 2  	1	0
// 	CS	TO	RO LM	CMB	FB	RB
// In one write to this register can only invoke one operation
void ArcEntropyEncoderModel::WriteToBufferCtrlReg(unsigned int val)
{
   // Update the register
   val = val & 0x3F;
   EE_AUX_REGS[BUFFER_CTRL] = val;
   
   val &= 0x7;
   switch(val)
   {
      case 0x1:
      {
         ResetBuffer();
         EE_AUX_REGS[BUFFER_CTRL]   = 0x0;
          EE_AUX_REGS[MB_BIT_CNT]    = 0;
         EE_AUX_REGS[SKIP_CNT]      = 0;
         break;
      }
      case 0x2:
      {
         FlushBuffer();
         EE_AUX_REGS[BUFFER_CTRL]            &= 0x7D;
         if(!(EE_AUX_REGS[STATUS_CTRL]&0x100))
         {

            ChannelResponse.RegisterData[0].RegValue     = EE_AUX_REGS[UNIT_SEL];
            ChannelResponse.RegisterData[0].valid        = true;

            ChannelResponse.RegisterData[1].RegValue  = EE_AUX_REGS[MB_BIT_CNT];
            ChannelResponse.RegisterData[1].valid     = true;

            if(EE_AUX_REGS[UNIT_SEL] == CONCAT_UNIT)
            {
               ChannelResponse.RegisterData[2].RegValue  = EE_AUX_REGS[CONCAT_BYTE_CNT];
               ChannelResponse.RegisterData[2].valid     = true;
            }
            
            ChannelResponse.RegisterData[3].RegValue     = EE_AUX_REGS[CH_FL_ADR];
            ChannelResponse.RegisterData[3].valid        = true;
            HasChannelResponse = true;
         }
         break;
      }
      case 0x4:
      {
/*#ifdef EE_DUMP
         fprintf(verif_log, "\n**********************************************\n");
         fprintf(verif_log, "**********************************************\n");
         fprintf(verif_log, "CODE_MB BIT WRITTEN IN BUFFER_CNTL REGISTER\n");
         fprintf(verif_log, "**********************************************\n");
         fprintf(verif_log, "**********************************************\n");
         fflush(verif_log);
#endif*/
         EE_AUX_REGS[MB_BIT_CNT] = 0;

         // Context save
         if((EE_AUX_REGS[BUFFER_CTRL] & 0x60)== 0x40)
         {
            int unit_sel                     = EE_AUX_REGS[UNIT_SEL];
            EE_AUX_REGS[BUF_STORE]           = EE_AUX_REGS[unit_sel + BUFF_REG_OFFSET];
            EE_AUX_REGS[CURR_ADDR_STORE]     = EE_AUX_REGS[unit_sel + CURR_ADDR_OFFSET]; 
            EE_AUX_REGS[STATUS_STORE]        = EE_AUX_REGS[unit_sel + STATUS_REG_OFFSET];
            EE_AUX_REGS[EMU_BUFF_STORE]      = EE_AUX_REGS[EMU_BUFF];
            EE_AUX_REGS[MB_BIT_CNT_STORE]    = EE_AUX_REGS[MB_BIT_CNT];
            EE_AUX_REGS[SKIP_CNT_STORE]      = EE_AUX_REGS[SKIP_CNT];
            EE_AUX_REGS[CN_BYTE_CNT_STORE]   = EE_AUX_REGS[CONCAT_BYTE_CNT];
            EE_AUX_REGS[EMU_BYTE_CNT_STORE]  = EE_AUX_REGS[EMU_BYTE_CNT];
         }

         // Set the REB Bit of status bit
         EE_AUX_REGS[STATUS_CTRL] |= 0x1;

         // If the SIC bit is set then return
         if(EE_AUX_REGS[EE_DEBUG_LOC] & 0x1)
         {
            EE_AUX_REGS[EE_DEBUG_LOC] |= 0x10000;
            return;
         }
         
         EntropyEncodeMacroBlock();

         break;
      }
      case 0x8:
      {
         break;
      }
   }
}

// Resets the selected current packer or concat unit.
// Which packer or concat is decided by the value of UNIT_SEL
// This will clearing all valid and flush status bits to zero.
// No attempt will be made to ensure that any valid data in the
// buffers are written to system memory
void ArcEntropyEncoderModel::ResetBuffer(void)
{
   int unit_sel = EE_AUX_REGS[UNIT_SEL];
   EE_AUX_REGS[STATUS_REG_OFFSET + unit_sel]       = 0x0;
   EE_AUX_REGS[BUFF_REG_OFFSET + unit_sel]         = 0x0;
   if(unit_sel == CONCAT_UNIT )
   {
      EE_AUX_REGS[CONCAT_BYTE_CNT]  = 0;
      EE_AUX_REGS[EMU_BYTE_CNT]     = 0;
      EE_AUX_REGS[EMU_BUFF]         = 0;
   }
}

// This function will cause the selected packer or the concatenation
// unit’s buffer contents to be flushed.
// Flushing means that the packers contents, including incomplete
// packing buffer contents are written to system memory.
// Flushing the buffer will not clear the buffer valid bits nor
// will it reset the packing buffer.
// When writing the incomplete packer buffer data to system memory,
// the current address register for the packer will also not increment

void ArcEntropyEncoderModel::FlushBuffer(void)
{
   // Write the packer buffer contents without incrementing the address
   int unit_sel = EE_AUX_REGS[UNIT_SEL];
   
   // Flush by packing to byte boundary
   int bits_remaining = WritePackToByteRegister(0,false);
   WritePackedBits(EE_AUX_REGS[unit_sel+BUFF_REG_OFFSET],((EE_AUX_REGS[unit_sel+STATUS_REG_OFFSET]&0x1f)+bits_remaining)/8);
   
   EE_AUX_REGS[unit_sel+STATUS_REG_OFFSET] |= 0x20; // Set the flush status
   // If there is anything still remaining in the EMU buffer write to memory
   if(EE_AUX_REGS[EMU_BUFF] & 0x7)
      WritePackedBits(EE_AUX_REGS[unit_sel+BUFF_REG_OFFSET],0);

}


//====================================================================================================
//======================================CONACT_SEL REG OPERATIONS====================================
//====================================================================================================
// This triggers the Concatenation unit to start loading and packing
// the bitstream data that are generated by the packer unit selected
// using the value written to this register.
// Once packing has started, all commands to the concatenation unit
// will be held off until packing is completed. The two bit value are defined as follows:
//    “00” – Packer 0
//    “01” – Packer 1
//    “10” – Packer 2
//    “11” – Packer 3

void ArcEntropyEncoderModel::WriteToConcatSelRegister(unsigned int val)
{
   // If the address of the packer and concat_DI regs are not the same then
   // read the data in

   unsigned int mem_val;
   EE_AUX_REGS[CONCAT_SEL] = val;
   EE_AUX_REGS[UNIT_SEL]   = val;

   EE_AUX_REGS[STATUS_CTRL] |= 0x2;

   while(EE_AUX_REGS[CONCAT_DI_ADR] != EE_AUX_REGS[CONCAT_SEL+CURR_ADDR_OFFSET])
   {
      mem_val = *((unsigned int *)EE_AUX_REGS[CONCAT_DI_ADR]);
      DoEndianCorrection(&mem_val);
      BitPacker(mem_val,32);
      EE_AUX_REGS[CONCAT_DI_ADR] += 4;
   }

   // Packer address and the concat_di address are the same
   // check if the packer has been flushed.
   // If not flushed flag programming error and return
   if(!(EE_AUX_REGS[CONCAT_SEL + STATUS_REG_OFFSET] & 0x20))
   {
    //  printf("FATAL PROGRAMMING ERROR PACKER %d not flushed\n",CONCAT_SEL);
      exit(1);
   }

   // last data has to be be read
   mem_val = *((unsigned int *)EE_AUX_REGS[CONCAT_DI_ADR]);
   BitPacker(mem_val,(EE_AUX_REGS[CONCAT_SEL + STATUS_REG_OFFSET] &0x1F));

   EE_AUX_REGS[STATUS_CTRL] &= 0x3FD;
}




//====================================================================================================
//======================================Exp-Golomb functions====================================
//====================================================================================================
// This performs Unsigned Exp-Golomb encoding as specified in the H.264 specification.
// The resulting code is sent to the selected packer unit for packing
//				Codes are formed by a series of flag bits, the last of
//				which is a "1" bit, followed by data bits, the number of
//				which is equal to the number of "0" flag bits. Hence the
//				code table looks as follows.
//								1
//							0	1	x
//						0	0	1	x	x
//					0	0	0	1	x	x	x

void ArcEntropyEncoderModel::GenerateUnsignedExpGolombCode(int val)
{
   if (val == 0)
	{
/*#ifdef EE_DUMP
       fprintf(verif_log, "----------------------------------------------\n");
       fprintf(verif_log, "UEV INPUT   = 0x%x\n", 0);
       fprintf(verif_log, "UEV OUTPUT  = 0x%x\n", 1);
       fprintf(verif_log, "UEV LENGTH  = 0x%x\n", 1);
       fflush(verif_log);
#endif*/
		BitPacker(1,1);
	}
   else
   {

	   // All other codes consist of z zeros (Z >= 1), a one bit, and z data bits
	   // The lowest codeword with z zeros is 2^z - 1, the highest is 2*(2^z - 1)
	   // The first 'highest' is 2, the next is always h(n+1) = 2*h(n) + 2
	   int highest = 2;
	   int zero_count = 1;

	   while (val > highest)
	   {
		   highest = (highest << 1) + 2;
		   zero_count++;
	   }

/*#ifdef EE_DUMP
       fprintf(verif_log, "----------------------------------------------\n");
       fprintf(verif_log, "UEV INPUT   = 0x%x\n", val);
       fprintf(verif_log, "UEV OUTPUT  = 0x%x\n", (1 << zero_count) + (val - (highest >> 1)));
       fprintf(verif_log, "UEV LENGTH  = 0x%x\n", ((zero_count << 1) + 1));
       fflush(verif_log);
#endif*/
	   // Note lowest = highest >> 1
	   BitPacker( (1 << zero_count) + (val - (highest >> 1)) , (zero_count << 1) + 1);
   }
}

void ArcEntropyEncoderModel::WriteToAddUevRegister(unsigned int val)
{
   GenerateUnsignedExpGolombCode(val);
}

// This writes a signed exp-Golomb code to the symbol buffer.
//				Values are mapped to code numbers as follows
//				    Code Number        Value
//						0				 0
//						1				 1
//						2				-1
//						3				 2
//						4				-2
//						...

void ArcEntropyEncoderModel::WriteToAddSevRegister(unsigned int value)
{
	int val = (int)value;
   if (0 == val)
	{
		GenerateUnsignedExpGolombCode(0);
	}
	else
	{
		if (val < 0)
		{
			GenerateUnsignedExpGolombCode(-2 * val);
		}
		else
		{
			GenerateUnsignedExpGolombCode((val << 1) - 1);
		}
	}

}




// Generates the coded block pattern
// lower 4 bits represent the 4 8x8 luma blocks and the 5th and 6th represent
// the chroma blocks
// NonZeroCoeff 0 to 15 represents the luma 4x4 block
void ArcEntropyEncoderModel::GenerateCodedBlockPattern(void)
{

  CalculateCodedBlockPatternH264();
#if 0
  //cbp = 0;

	//////////////////////////////////////////////////////////////////////////////
	// Luminance
	//////////////////////////////////////////////////////////////////////////////
  cbp = MpcCurrentPtr->NonZeroCoeffLuma & 0xF;

	//////////////////////////////////////////////////////////////////////////////
	// Chrominance
	//////////////////////////////////////////////////////////////////////////////

  if (MpcCurrentPtr->NonZeroCoeffChroma & 0x3)
	{
		//AC coefficients
      cbp |= 32;
	}
  else if (MpcCurrentPtr->UV_DC_NonZeroCoeff)
	{
		cbp |= 16;
	}
#endif

  IntraModeLuma =  (MpcCurrentPtr->IntraMode & INTRAMODE_LUMA_MASK);

	if (MBCT_H264_INTRA16X16 == MpcCurrentPtr->MBCodingType)
	{
		// Add the CBP information to IntraModeLuma

		// Add 12 if there are any luma coeffs
		if (cbp & 0xF)
		{
			cbp |= 0xF;
         IntraModeLuma +=  12;
		}

		// Add 4 or 8 if chroma part is 16 or 32 respectively
		IntraModeLuma += ((cbp & 0x30) >> 2);
	}
}


void ArcEntropyEncoderModel::CalculateCodedBlockPatternH264(void)
{
  cbp = 0;
  int block0,block1,block2,block3;
  for (int block_count=0; block_count<4; block_count++)
	{
		block0 = MpcCurrentPtr->NonZeroCoeffLuma >> top_left_of_luma_4x4_for_mpc[(block_count<<2)] & 0x1;
		block1 = MpcCurrentPtr->NonZeroCoeffLuma >> top_left_of_luma_4x4_for_mpc[(block_count<<2) + 1] & 0x1;
		block2 = MpcCurrentPtr->NonZeroCoeffLuma >> top_left_of_luma_4x4_for_mpc[(block_count<<2) + 2] & 0x1;
		block3 = MpcCurrentPtr->NonZeroCoeffLuma >> top_left_of_luma_4x4_for_mpc[(block_count<<2) + 3] & 0x1;
    if(block0 || block1 || block2 || block3)
      cbp |= 1<<block_count;

  }
  
  if (MpcCurrentPtr->NonZeroCoeffChroma & 0xF || ((MpcCurrentPtr->NonZeroCoeffChroma >> 4) &0xF))
    cbp |= 32;
  else if (MpcCurrentPtr->UV_DC_NonZeroCoeff)
	{
		cbp |= 16;
	}
}

void ArcEntropyEncoderModel::CalculateCodedBlockPatternH263(void)
{
  cbp = (MpcCurrentPtr->NonZeroCoeffLuma >> 6) & 0x3F;
#ifdef EE_DUMP 
  
  fflush(verif_log);
  if ((EE_AUX_REGS[CODEC_CFG]&0x3) == 1)
  {
    sample_mpeg4_cbp = true;
    mpeg4_cbp_value = cbp;
    fprintf(verif_log, "MPEG4_CBP_VALUE = 0x%x\n", mpeg4_cbp_value);
  }
  else if ((EE_AUX_REGS[CODEC_CFG]&0x3) == 2)
  {
    sample_h263_cbp = true;
    h263_cbp_value = cbp;
  }
  fflush(verif_log);
#endif
}



//====================================================================================================
//======================================Macroblock Entropy encoding===================================
//====================================================================================================

void ArcEntropyEncoderModel::EntropyEncodeMacroBlock(void)
{
   switch(EE_AUX_REGS[CODEC_CFG]&0x3)
   {
      case 0:
      {
         H264EntropyEncodeMacroBlock();
         break;
      }
      case 1:
	  {
         Mpeg4EntropyEncodeMacroBlock();
         break;
	  }
      case 2:
      {
         H263EntropyEncodeMacroBlock();
         break;
      }
      default:
         break;
   }

   EE_AUX_REGS[BUFFER_CTRL]            &= 0x7B;
   if(!(EE_AUX_REGS[STATUS_CTRL]&0x100))
   {
      ChannelResponse.RegisterData[0].RegValue     = EE_AUX_REGS[UNIT_SEL];
      ChannelResponse.RegisterData[0].valid        = true;

      ChannelResponse.RegisterData[1].RegValue     = EE_AUX_REGS[MB_BIT_CNT];
      ChannelResponse.RegisterData[1].valid        = true;

      if(EE_AUX_REGS[UNIT_SEL] == CONCAT_UNIT)
      {
         ChannelResponse.RegisterData[2].RegValue  = EE_AUX_REGS[EMU_BYTE_CNT];
         ChannelResponse.RegisterData[2].valid     = true;
      }

      ChannelResponse.RegisterData[3].RegValue     = EE_AUX_REGS[CH_MBE_ADR];
      ChannelResponse.RegisterData[3].valid        = true;
      HasChannelResponse                           = true;
   }

   EE_AUX_REGS[STATUS_CTRL] &= 0x306;

   EE_AUX_REGS[DELTA_QUANT] = 0;
}
void ArcEntropyEncoderModel::H264EntropyEncodeMacroBlock(void)
{
   GenerateCodedBlockPattern();

   // Check if it has to code only the residue
   if(!(EE_AUX_REGS[BUFFER_CTRL]& RESIDUE_ONLY_MASK))
   {
      
      // Skipped macroblock
      if (MBCT_H264_SKIPPED == MpcCurrentPtr->MBCodingType)
	   {
		   WRITE_ENCODED_SYMBOL("Macroblock is skipped",MbNumber)
#ifdef EE_DUMP        
               //fprintf(verif_log, "\n----------------------------------------------\n");
               //fprintf(verif_log, "MACROBLOCK IS SKIPPED:  = %d\n",MbNumber);
               fflush(verif_log);
#endif               

               EE_AUX_REGS[SKIP_CNT]++;
#ifdef EE_DUMP        
               //fprintf(verif_log,"CURRENT SKIP CNT = %d\n",EE_AUX_REGS[SKIP_CNT]);
               fflush(verif_log);
#endif               

		   // If this is the last macroblock of the picture,
         // then code the run of skipped macroblocks
		   if (EE_AUX_REGS[BUFFER_CTRL] & 0x08)
		   {
			   WriteToAddUevRegister(EE_AUX_REGS[SKIP_CNT]);
			   WRITE_ENCODED_SYMBOL("Skipped Macroblock Count", EE_AUX_REGS[SKIP_CNT])
#ifdef EE_DUMP        
               //fprintf(verif_log,"LAST MACROBLOCK SKIP CNT = %d\n",EE_AUX_REGS[SKIP_CNT]);
               fflush(verif_log);
#endif               
			   EE_AUX_REGS[SKIP_CNT] = 0;
		   }
         
#ifdef EE_DUMP        
         for(int x=0;x<4;x++) {
           //fprintf(verif_log, "CLEARING MODEL COEFF COUNTS\n");
          // printf("CLEARING MODEL COEFF COUNTS\n");
           MpcCurrentPtr->CoeffCountSB[x] = 0x00000000;
           //fprintf(verif_log, "MpcCurrentPtr->CoeffCountSB[%d] = 0x%08X\n", x, MpcCurrentPtr->CoeffCountSB[x]);
           //printf("MpcCurrentPtr->CoeffCountSB[%d] = \n", x, MpcCurrentPtr->CoeffCountSB[x]);
           WriteMPCData(x);
           fflush(verif_log);
         };
#endif               
           
         ClearLMandCMBBit();
         return;
#ifdef EE_DUMP        
             //  fprintf(verif_log, "----------------------------------------------\n");
               fflush(verif_log);
#endif               
	   }



      ////////////////////////////////////////////////////////////////
	   // Code Skipped Macroblock count for non intra and SI pictures
	   ////////////////////////////////////////////////////////////////

	   if (EE_I_SLICE != EE_AUX_REGS[SLICE_TYPE])
	   {
#ifdef EE_DUMP        
           /*fprintf(verif_log, "\n----------------------------------------------\n");
           fprintf(verif_log, "SLICE TYPE IS P_SLICE THERFORE WRITING\n");
           fprintf(verif_log, "SKIP_CNT WRITTEN TO ADD_UEV REG: VALUE WRITTEN = 0x%x\n", EE_AUX_REGS[SKIP_CNT]);
           fprintf(verif_log, "----------------------------------------------\n");*/
           fflush(verif_log);
           
           for(int x=0;x<4;x++) {
            // fprintf(verif_log, "CLEARING MODEL COEFF COUNTS\n");
            // printf("CLEARING MODEL COEFF COUNTS\n");
             MpcCurrentPtr->CoeffCountSB[x] = 0x00000000;
             //fprintf(verif_log, "MpcCurrentPtr->CoeffCountSB[%d] = 0x%08X\n", x, MpcCurrentPtr->CoeffCountSB[x]);
            // printf("MpcCurrentPtr->CoeffCountSB[%d] = \n", x, MpcCurrentPtr->CoeffCountSB[x]);
             WriteMPCData(x);
             fflush(verif_log);
           };
#endif               
		   // Run of skipped macroblocks
		   WriteToAddUevRegister(EE_AUX_REGS[SKIP_CNT]);
		   WRITE_ENCODED_SYMBOL("Skipped Macroblock Count",EE_AUX_REGS[SKIP_CNT])

		   // Reset run of skipped macroblocks
		   EE_AUX_REGS[SKIP_CNT] = 0;
	   }

      EncodeCodeMBType();

      if(EncodeMVAndCBP() && (0==cbp))
         return;

      int DQuant_codeword;
      int differential_quant = EE_AUX_REGS[DELTA_QUANT];

	   if (differential_quant > 0)
	   {
		   DQuant_codeword = 2*differential_quant - 1;
	   }
	   else
	   {
		   DQuant_codeword = (-2*differential_quant);
	   }

#ifdef EE_DUMP        
           delta_quant_value = DQuant_codeword;
           sample_delta_quant = true;
#endif               
	   WriteToAddUevRegister(DQuant_codeword);
	   WRITE_ENCODED_SYMBOL("DQuant", DQuant_codeword)
   }
   EncodeH264Coefficients();
}



// Encodes the MB_TYPE field
void ArcEntropyEncoderModel::EncodeCodeMBType(void)
{
   unsigned int mb_type=0;

   if (EE_AUX_REGS[SLICE_TYPE] == EE_P_SLICE)
   {
      switch (MpcCurrentPtr->MBCodingType)
      {
         case MBCT_H264_INTRA16X16: mb_type = 6; break;
         case MBCT_H264_INTER16X16: mb_type = 0; break;
         case MBCT_H264_INTER16X8:  mb_type = 1; break;
         case MBCT_H264_INTER8X16:  mb_type = 2; break;
         case MBCT_H264_INTER8X8:   mb_type = 4; break;
         default:
            // error - illegal MBCodingType
            assert(false);
            break;
      }
   }
   else if (EE_AUX_REGS[SLICE_TYPE] == EE_I_SLICE)
   {
      if (MpcCurrentPtr->MBCodingType == MBCT_H264_INTRA16X16)
      {
         mb_type = 1;
      }
      else
      {
         // error - illegal MBCodingType
         assert(false);
      }
   }

   if (MpcCurrentPtr->MBCodingType == MBCT_H264_INTRA16X16)
   {
      mb_type += IntraModeLuma;
   }

#ifdef EE_DUMP        
           mb_type_value = MpcCurrentPtr->MBCodingType; 
           sample_mb_type = true;
#endif               

	WriteToAddUevRegister(mb_type);
	WRITE_ENCODED_SYMBOL("MB Type", mb_type + 100*MbNumber)
}

/* ------------------------------------------------------------------------ */
/*          This is called to encode the differential motion vectors
				for a macroblock. It substracts the predicted motion vector
				from the absolute and the difference is encoded.			*/
/* ------------------------------------------------------------------------ */

void ArcEntropyEncoderModel::EncodeDifferentialMotionVector(unsigned char block_index)
{

	///////////////////////////////////////////////////////////////////////////
	// Subtract the predictors from the absolute vector to get the differential
	// vector. Encode this differential vector.
	///////////////////////////////////////////////////////////////////////////
    int differential_hor = MpcCurrentPtr->mv[block_index].mvx - MpcCurrentPtr->mvp[block_index].mvx;
	int differential_ver = MpcCurrentPtr->mv[block_index].mvy - MpcCurrentPtr->mvp[block_index].mvy;
    
	int codeword_hor, codeword_ver;
    
	if (differential_hor > 0)
	{
		codeword_hor = 2*differential_hor - 1;
	}
	else
	{
		codeword_hor = (-2*differential_hor);
	}

	if (differential_ver > 0)
	{
		codeword_ver = 2*differential_ver - 1;
	}
	else
	{
		codeword_ver = (-2*differential_ver);
	}

	WriteToAddUevRegister(codeword_hor);
	WRITE_ENCODED_PAIR("Horiz Vector (abs,diff)", MpcCurrentPtr->mv[block_index].mvx, differential_hor)

	WriteToAddUevRegister(codeword_ver);
	WRITE_ENCODED_PAIR("Vert Vector (abs,diff)", MpcCurrentPtr->mv[block_index].mvy, differential_ver)
}



// Encodes the Motion Vectors and coded block pattern
bool ArcEntropyEncoderModel::EncodeMVAndCBP(void)
{
               
   if (MBCT_H264_INTER16X16 == MpcCurrentPtr->MBCodingType)
	{
		// Code the motion vectors
		EncodeDifferentialMotionVector(0);

		// Code CBP
		WriteToAddUevRegister(h264_encoder_inter_cbp_table[cbp]);
		WRITE_ENCODED_SYMBOL("Inter CBP", h264_encoder_inter_cbp_table[cbp])
      return true;

	}
	else if (MBCT_H264_INTER16X8 == MpcCurrentPtr->MBCodingType)
	{
		// Code the motion vectors
		EncodeDifferentialMotionVector(0);
		EncodeDifferentialMotionVector(2);

		// Code CBP
		WriteToAddUevRegister(h264_encoder_inter_cbp_table[cbp]);
		WRITE_ENCODED_SYMBOL("Inter CBP", h264_encoder_inter_cbp_table[cbp])
      return true;
	}
	else if (MBCT_H264_INTER8X16 == MpcCurrentPtr->MBCodingType)
	{
		// Code the motion vectors
		EncodeDifferentialMotionVector(0);
		EncodeDifferentialMotionVector(1);

		// Code CBP
      WriteToAddUevRegister(h264_encoder_inter_cbp_table[cbp]);
		WRITE_ENCODED_SYMBOL("Inter CBP", h264_encoder_inter_cbp_table[cbp])
      return true;

	}
	else if ( ( MBCT_H264_INTER8X8 == MpcCurrentPtr->MBCodingType) )
	{
		// The syntax is summarised as below
		// for i=0 to 3 { region type; }
		// for i=0 to 3 { reference frame; }
		// for i=0 to 3 { vectors }

		unsigned char block_count_8x8;

		// Code the four region types
		for (block_count_8x8=0; block_count_8x8<4; block_count_8x8++)
		{
			WriteToAddUevRegister(0); // H264_8X8_PARTITION (=0)
			WRITE_ENCODED_SYMBOL("Region type", 0) // H264_8X8_PARTITION (=0)
		}


		// Code up to four sets of forward vectors
		for (block_count_8x8=0; block_count_8x8<4; block_count_8x8++)
		{
			{
				// Calculate block_index for top left of 8x8 partition. In case of block shape other
				// than 8x8, add offset to this value.
				// unsigned int first_block_number = ((block_count_8x8 & 1) << 1) + ((block_count_8x8 & 2) << 2);
                EncodeDifferentialMotionVector(block_count_8x8);
			}
		}

		// Code CBP
		WriteToAddUevRegister(h264_encoder_inter_cbp_table[cbp]);
		WRITE_ENCODED_SYMBOL("Inter CBP", h264_encoder_inter_cbp_table[cbp])
      return true;
	}
	else // one of the 16x16 modes
	{
      u_int8 IntraModeChroma = (MpcCurrentPtr->IntraMode >> INTRAMODE_CHROMA_SHIFT) & INTRAMODE_CHROMA_MASK;             
      WriteToAddUevRegister(IntraModeChroma);
		WRITE_ENCODED_SYMBOL("Intra chroma prediction mode", IntraModeChroma)
	}

   return false;

}

// Residual encoding
// CAVLC is used in H.264 for encoding macroblock residues.
// It specifies a multi-step scheme where the residues of each 4x4 block
// can be decoded from. These steps are:
//	   Coeff_token encoding
//	   Trailing one transform coefficient level encoding
//	   Other non-zero coeffcient level encoding
//	   Total Zero Encoding
//	   Run Zero Encoding.


void ArcEntropyEncoderModel::EncodeH264Coefficients(void)
{


   /////////////////////////////////////////////////////////
	// Coefficient Encoding
	/////////////////////////////////////////////////////////

	unsigned char * luma_scan_dc_table = (unsigned char *)frame_mb_zig_zag_luma_dc_table;;
	unsigned char * luma_scan_table    = (unsigned char *)frame_mb_zig_zag_luma_table;;
#ifdef EE_DUMP        
    //clear luma_ac arrays at start of iteration
    for (int i=0; i<=24; i++) {
      h264_nc_val[i] = 0;
      h264_total_coeffs[i] = 0;
      h264_num_trailing_ones[i] = 0;
      h264_coeff_token_valid_array[i] = 0;
    };
#endif

	/////////////////////////////////////////////////////////
	// Intra 16x16 Luma DC coeffs
	/////////////////////////////////////////////////////////


	if (MBCT_H264_INTRA16X16 == MpcCurrentPtr->MBCodingType)
	{

		// Code DC 4X4 block
		ResetCoefficientVariables();
        int index = 0;
		for (int j=15; j >= 0; j--)
		{
		    index = luma_scan_dc_table[j];
            if(index < 8)
                Coeff = MpcCurrentPtr->Y_DC_CoeffStore0[index];
            else
                Coeff = MpcCurrentPtr->Y_DC_CoeffStore1[index-8];


            ProcessCoefficient();
		}

		// CAVLC Coefficient Token

		TotalCoeffs = CoefficientCount + NumberOfTrailingOnes;
#ifdef EE_DUMP        
        intra16x16_dc_total_coeffs = TotalCoeffs;
        //printf("intra16x16_dc_total_coeffs = %d\n", intra16x16_dc_total_coeffs);
        intra16x16_dc_num_trailing_ones = NumberOfTrailingOnes;
        //printf("intra16x16_dc_num_trailing_ones = %d\n", intra16x16_dc_num_trailing_ones);

        sample_intra16x16_dc_coeff_token = true;
#endif

		// encode token
		// Which coding table do we use? The following function call sets CoeffTokenTable appropriately.
		// NB Intra DC is treated as if it were the top left AC 4x4 block.
		DetermineVLCTableForLumaCoeffToken(0);
#ifdef EE_DUMP
        luma_dc_offset = 0;
        h264_total_coeffs[luma_dc_offset] = TotalCoeffs;
        //printf("h264_total_coeffs[%d] = %d\n", luma_dc_offset, h264_total_coeffs[luma_dc_offset]);
        h264_num_trailing_ones[luma_dc_offset] = NumberOfTrailingOnes;
        //printf("h264_num_trailing_ones[%d] = %d\n", luma_dc_offset, h264_num_trailing_ones[luma_dc_offset]);
        h264_nc_val[luma_dc_offset] = h264_nc;
        //printf("h264_nc_val[%d] = %d\n", luma_dc_offset, h264_nc_val[luma_dc_offset]);
        h264_coeff_token_valid_array[luma_dc_offset] = 1;
        sample_h264_coeff_token = true;
#endif

        
		// Now code the symbol
		CodeCoefficientsToken();
		WRITE_ENCODED_SYMBOL("Total coeffs", TotalCoeffs)
		WRITE_ENCODED_SYMBOL("Trailing ones", NumberOfTrailingOnes)

		// Now the trailing ones signs
		for (int i = 0; i < NumberOfTrailingOnes; i++)
		{
			WriteFixedLengthCode(TrailingOnesSignTable[i], 1);
#ifdef EE_DUMP        
            intra16x16_dc_trailing_ones_sign = TrailingOnesSignTable[i];
            sample_intra16x16_dc_trail_ones_sign = true;
#endif
#ifdef EE_DUMP        
            h264_trailing_ones_sign[luma_dc_offset] = TrailingOnesSignTable[i];
            sample_h264_trail_ones_sign = true;
#endif
			WRITE_ENCODED_SYMBOL("Trailing ones sign", TrailingOnesSignTable[i])
		}


		if (0 != TotalCoeffs)
		{
			bool first_coeff = true;
			for (int i = 0; i < CoefficientCount; i++)
			{
#ifdef EE_DUMP				
                EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, 0, 0, i, 0);
#else                
				EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff);
#endif                
				first_coeff = false;
			}
#ifdef EE_DUMP        
            intra16x16_dc_total_zeros = TotalZeroes;
            sample_intra16x16_dc_total_zeros = true;
#endif
#ifdef EE_DUMP        
            //h264_total_zeros = TotalZeroes;
        
            h264_total_zeros[luma_dc_offset] = TotalZeroes;
            h264_total_coeffs_tz[luma_dc_offset] = TotalCoeffs;
            sample_h264_total_zeros = true;
#endif
			EncodeZeroes(false);
		}
	}

    for(int x=0;x<4;x++)
        MpcCurrentPtr->CoeffCountSB[x] = 0;

	/////////////////////////////////////////////////////////
	// Luma AC coeffs
	/////////////////////////////////////////////////////////
#ifdef EE_DUMP        
    //clear luma_ac arrays at start of iteration
    for (int i=0; i<=255; i++) {
      luma_ac_lc_array[i] = 0;
      luma_ac_sl_array[i] = 0;
      luma_ac_valid_array[i] = 0;
    };
#endif
	
    if (0 != (cbp & 15))		// ie if there *are* some LUMA AC coefficients
	{
        unsigned int block_number;
		for (block_number=0; block_number<NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS; block_number++)
		{
			
      
      // Check CBP to determine whether block is part of an 8x8 block that has coefficients
			if (0 != (cbp & (1 << (block_number >> 2))))
			{
				ResetCoefficientVariables();
            int loop_count;
            (MBCT_H264_INTRA16X16 == MpcCurrentPtr->MBCodingType)?loop_count=0:loop_count=-1;

                short *tmp = (short *)(MpcCurrentPtr->PixelCoeffBuffer);
#ifdef EE_DUMP        
                    fprintf(verif_log, "\n");
#endif

				for (int j=15; j > loop_count; j--)
				{
                    Coeff = tmp[top_left_of_luma_4x4_block_bitstream_order_table[block_number] + luma_scan_table[j]];

					ProcessCoefficient();
				}

                u_int8 tot = CoefficientCount + NumberOfTrailingOnes;
                short idx = CoffCntsBitPos[h264_4x4_transmission_to_raster_table[block_number]];
				MpcCurrentPtr->CoeffCountSB[idx >> 5] |= tot << (idx % 32) ;

				// CAVLC Coefficient Token

            TotalCoeffs = CoefficientCount + NumberOfTrailingOnes;
#ifdef EE_DUMP        
           

            luma_ac_total_coeffs = CoefficientCount;
            //printf("luma_ac_total_coeffs = %d\n", luma_ac_total_coeffs);
            luma_ac_num_trailing_ones = NumberOfTrailingOnes;
            //printf("luma_ac_num_trailing_ones = %d\n", luma_ac_num_trailing_ones);

            sample_luma_ac_coeff_token = true;
#endif
				// encode token
				// Which coding table do we use? The following function call sets m_SymbolEjectCoeffTokenTable appropriately
				DetermineVLCTableForLumaCoeffToken(block_number);
#ifdef EE_DUMP
        //fprintf(verif_log, "TotalCoeffs = %d\n", TotalCoeffs);
        //fprintf(verif_log, "NumberOfTrailingOnes = %d\n", NumberOfTrailingOnes);
        //fflush(verif_log);

        luma_ac_offset = 1;
        
        h264_total_coeffs[luma_ac_offset + block_number] = CoefficientCount;
        //printf("h264_total_coeffs[%d] = %d\n", luma_ac_offset + block_number, h264_total_coeffs[luma_ac_offset + block_number]);
        h264_num_trailing_ones[luma_ac_offset + block_number] = NumberOfTrailingOnes;
        //printf("h264_num_trailing_ones[%d] = %d\n", luma_ac_offset + block_number, h264_num_trailing_ones[luma_ac_offset + block_number]);
        
        h264_nc_val[luma_ac_offset + block_number] = h264_nc;
        //printf("h264_nc_val[%d] = %d\n", luma_ac_offset + block_number, h264_nc_val[luma_ac_offset + block_number]);
        
        h264_coeff_token_valid_array[luma_ac_offset + block_number] = 1;
        
        sample_h264_coeff_token = true;
#endif

				// Now code the symbol
				CodeCoefficientsToken();
				WRITE_ENCODED_SYMBOL("Total coeffs", TotalCoeffs)
				WRITE_ENCODED_SYMBOL("Trailing ones", NumberOfTrailingOnes)


				// Now the trailing ones signs
				for (int i = 0; i < NumberOfTrailingOnes; i++)
				{
					WriteFixedLengthCode(TrailingOnesSignTable[i], 1);
					WRITE_ENCODED_SYMBOL("Trailing ones sign", TrailingOnesSignTable[i])
#ifdef EE_DUMP        
           
            luma_ac_trailing_ones_sign = TrailingOnesSignTable[i];
            sample_luma_ac_trail_ones_sign = true;
#endif
#ifdef EE_DUMP        
            
            //h264_trailing_ones_sign = TrailingOnesSignTable[i];
            h264_trailing_ones_sign[luma_ac_offset + block_number] = TrailingOnesSignTable[i];
            sample_h264_trail_ones_sign = true;
#endif
				}

                
				if (0 != TotalCoeffs)
				{
					bool first_coeff = true;
					for (int i = 0; i < CoefficientCount; i++)
					{
                      #ifdef EE_DUMP				
                        EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, block_number, 0, i, 1);
                      #else                
						EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff);
                      #endif                        
				      first_coeff = false;
					}
#ifdef EE_DUMP        
            		luma_ac_total_zeros = TotalZeroes;
            		//printf("luma_ac_total_zeros  = %d\n", luma_ac_total_zeros);
            		//fflush(verif_log);
            		sample_luma_ac_total_zeros = true;
#endif
#ifdef EE_DUMP        
             		//h264_total_zeros = TotalZeroes;
                    h264_total_zeros[luma_ac_offset + block_number] = TotalZeroes;
                    h264_total_coeffs_tz[luma_ac_offset + block_number] = CoefficientCount;
             		//printf("h264_total_zeros  = %d\n", h264_total_zeros);
             		//fflush(verif_log);
             		sample_h264_total_zeros = true;
#endif
					EncodeZeroes((MBCT_H264_INTRA16X16 == MpcCurrentPtr->MBCodingType));
				}
			}
		}
	}


	/////////////////////////////////////////////////////////
	// Chroma DC coeffs
	/////////////////////////////////////////////////////////


	if (0 != (cbp >> 4))
	{
		int j;
		// CAVLC Coefficient Token

		ResetCoefficientVariables();
		for (j=3; j > -1; j--)
		{
			Coeff = MpcCurrentPtr->U_DC_CoeffStore[j];
			ProcessCoefficient();
		}

		// CAVLC Coefficient Token

		TotalCoeffs = CoefficientCount + NumberOfTrailingOnes;

		// encode token
		// Now code the symbol
		CodeChromaDCCoefficientsToken();
		WRITE_ENCODED_SYMBOL("Total U DC coeffs", TotalCoeffs)
		WRITE_ENCODED_SYMBOL("U DC Trailing ones", NumberOfTrailingOnes)
#ifdef EE_DUMP        
        chroma_dc_total_coeffs = TotalCoeffs;
       // printf("chroma_dc_total_coeffs = %d\n", chroma_dc_total_coeffs);
        chroma_dc_num_trailing_ones = NumberOfTrailingOnes;
        //printf("chroma_dc_num_trailing_ones = %d\n", chroma_dc_num_trailing_ones);
        sample_chroma_dc_coeff_token = true;
#endif


		// Now the trailing ones signs
		for (int i = 0; i < NumberOfTrailingOnes; i++)
		{
			WriteFixedLengthCode(TrailingOnesSignTable[i], 1);
			WRITE_ENCODED_SYMBOL("Trailing ones sign", TrailingOnesSignTable[i])
#ifdef EE_DUMP        
            chroma_dc_trailing_ones_sign = TrailingOnesSignTable[i];
            sample_chroma_dc_trail_ones_sign = true;
#endif
		}


		if (0 != TotalCoeffs)
		{
			bool first_coeff = true;
			for (int i = 0; i < CoefficientCount; i++)
			{
#ifdef EE_DUMP				
               // fprintf(verif_log, "CALLING EncodeLevels FOR THE 1st CHROMA BLOCK\n");
              EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, 0, 0, i, 2);
#else              
			  EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff);
#endif                
				first_coeff = false;
			}
#ifdef EE_DUMP        
            chroma_dc_total_zeros = TotalZeroes;
            //printf("chroma_dc_total_zeros  = %d\n", chroma_dc_total_zeros);
            //fflush(verif_log);
            sample_chroma_dc_total_zeros = true;
#endif
			CodeChromaDCZeroes();
		}

		// CAVLC Coefficient Token

		ResetCoefficientVariables();
		for (j=3; j > -1; j--)
		{
			Coeff = MpcCurrentPtr->V_DC_CoeffStore[j];
			ProcessCoefficient();
		}

		// CAVLC Coefficient Token

		TotalCoeffs = CoefficientCount + NumberOfTrailingOnes;

		// encode token
		//Which coding table do we use? The chroma DC one (there is only one).

		// Now code the symbol
		CodeChromaDCCoefficientsToken();
		WRITE_ENCODED_SYMBOL("Total U DC coeffs", TotalCoeffs)
		WRITE_ENCODED_SYMBOL("U DC Trailing ones", NumberOfTrailingOnes)
#ifdef EE_DUMP        
       
        
        chroma_dc_nc = -1;
        //printf("CHROMA DC CT: chroma_dc_nc = %d\n", chroma_dc_nc);
        
        chroma_dc_total_coeffs = TotalCoeffs;
        //printf("CHROMA DC CT: chroma_dc_total_coeffs = %d\n", chroma_dc_total_coeffs);
        chroma_dc_num_trailing_ones = NumberOfTrailingOnes;
        //printf("CHROMA DC CT: chroma_dc_num_trailing_ones = %d\n", chroma_dc_num_trailing_ones);
        sample_chroma_dc_coeff_token = true;
#endif


		// Now the trailing ones signs
		for (int i = 0; i < NumberOfTrailingOnes; i++)
		{
			WriteFixedLengthCode(TrailingOnesSignTable[i], 1);
			WRITE_ENCODED_SYMBOL("Trailing ones sign", TrailingOnesSignTable[i])
#ifdef EE_DUMP        
            
            
            chroma_dc_trailing_ones_sign = TrailingOnesSignTable[i];
            sample_chroma_dc_trail_ones_sign = true;
#endif
		}


		if (0 != TotalCoeffs)
		{
			bool first_coeff = true;
			for (int i = 0; i < CoefficientCount; i++)
			{
#ifdef EE_DUMP				
                //fprintf(verif_log, "CALLING EncodeLevels FOR THE 2nd CHROMA BLOCK\n");
                EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, 0, 1, i, 2);
#else              
				EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff);
#endif                
				first_coeff = false;
			}
#ifdef EE_DUMP        
            chroma_dc_total_zeros = TotalZeroes;
           // printf("chroma_dc_total_zeros  = %d\n", chroma_dc_total_zeros);
            //fflush(verif_log);
            sample_chroma_dc_total_zeros = true;
#endif
			CodeChromaDCZeroes();
		}
	}

	/////////////////////////////////////////////////////////
	// Chroma AC coeffs
	/////////////////////////////////////////////////////////


	if (2 == (cbp >> 4))
	{

        EncodeChromaACCoefficients(PCB_VLC_TRANSFORM_U,U_BIT_POSTIONS,0);
        EncodeChromaACCoefficients(PCB_VLC_TRANSFORM_V,V_BIT_POSTIONS,20);
	}

    for(int x=0;x<4;x++)
        WriteMPCData(x);

}


void ArcEntropyEncoderModel::EncodeChromaACCoefficients(int PixelBuffOffset,const short * bit_pos,short offset)
{
   unsigned char * chroma_scan_table = (unsigned char *)frame_mb_zig_zag_chroma_table;
   unsigned char block_number;
	for (block_number=0; block_number<NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS; block_number++)
	{
		ResetCoefficientVariables();
		int loop_count = 0;
        short *tmp = (short *)(MpcCurrentPtr->PixelCoeffBuffer + PixelBuffOffset);
		for (int j=15; j > loop_count; j--)
		{
			Coeff = tmp[top_left_of_chroma_4x4_block_table[block_number] + chroma_scan_table[j]];
			ProcessCoefficient();
		}

        int tot = CoefficientCount + NumberOfTrailingOnes;
        int idx = bit_pos[block_number];
        MpcCurrentPtr->CoeffCountSB[idx >> 5] |= tot << (idx % 32);


		// CAVLC Coefficient Token

		TotalCoeffs = CoefficientCount + NumberOfTrailingOnes;

		// encode token
		// Which coding table do we use? The following function call sets m_SymbolEjectCoeffTokenTable appropriately
		DetermineVLCTableForUVCoeffToken(block_number,bit_pos,offset);

		// Now code the symbol
		CodeCoefficientsToken();
		WRITE_ENCODED_SYMBOL("Total coeffs", TotalCoeffs)
		WRITE_ENCODED_SYMBOL("Trailing ones", NumberOfTrailingOnes)
#ifdef EE_DUMP        
       

        chroma_ac_total_coeffs = TotalCoeffs;
        //printf("chroma_ac_total_coeffs = %d\n", chroma_ac_total_coeffs);
        chroma_ac_num_trailing_ones = NumberOfTrailingOnes;
        //printf("chroma_ac_num_trailing_ones = %d\n", chroma_ac_num_trailing_ones);
        sample_chroma_ac_coeff_token = true;
#endif

#ifdef EE_DUMP
      
        
        chroma_ac_offset = 17;
        
        h264_total_coeffs[chroma_ac_offset + block_number] = TotalCoeffs;
       // printf("h264_total_coeffs[%d] = %d\n", chroma_ac_offset + block_number, h264_total_coeffs[chroma_ac_offset + block_number]);
        h264_num_trailing_ones[chroma_ac_offset + block_number] = NumberOfTrailingOnes;
       // printf("h264_num_trailing_ones[%d] = %d\n", chroma_ac_offset + block_number, h264_num_trailing_ones[chroma_ac_offset + block_number]);
        
        h264_nc_val[chroma_ac_offset + block_number] = h264_chroma_nc;
       // printf("h264_nc_val[%d] = %d\n", chroma_ac_offset + block_number, h264_nc_val[chroma_ac_offset + block_number]);
        
        h264_coeff_token_valid_array[chroma_ac_offset + block_number] = 1;
        
        sample_h264_coeff_token = true;
#endif


		// Now the trailing ones signs
		for (int i = 0; i < NumberOfTrailingOnes; i++)
		{
			WriteFixedLengthCode(TrailingOnesSignTable[i], 1);
			WRITE_ENCODED_SYMBOL("Trailing ones sign", TrailingOnesSignTable[i])
#ifdef EE_DUMP        
            
            
            chroma_ac_trailing_ones_sign = TrailingOnesSignTable[i];
            sample_chroma_ac_trail_ones_sign = true;
#endif
#ifdef EE_DUMP        
           
            h264_trailing_ones_sign[chroma_ac_offset + block_number] = TrailingOnesSignTable[i];
            sample_h264_trail_ones_sign = true;
#endif
		}


		if (0 != TotalCoeffs)
		{
			bool first_coeff = true;
			for (int i = 0; i < CoefficientCount; i++)
			{
#ifdef EE_DUMP
                if (offset == 0) { // 1st CHROMA
				  EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, block_number, 0, i, 3);
                }
                else if (offset == 20) { // 2nd CHROMA
                  EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff, block_number, 1, i, 3);
                };
#else
				EncodeLevels(CoefficientArray[i], NumberOfTrailingOnes, first_coeff);
#endif
				first_coeff = false;
			}
#ifdef EE_DUMP        
            chroma_ac_total_zeros = TotalZeroes;
           // printf("chroma_ac_total_zeros  = %d\n", chroma_ac_total_zeros);
            //fflush(verif_log);
            sample_chroma_ac_total_zeros = true;
#endif
#ifdef EE_DUMP        
            //h264_total_zeros = TotalZeroes;
        
            h264_total_zeros[chroma_ac_offset + block_number] = TotalZeroes;
            h264_total_coeffs_tz[chroma_ac_offset + block_number] = TotalCoeffs;
            //printf("h264_total_zeros  = %d\n", h264_total_zeros);
            //fflush(verif_log);
            sample_h264_total_zeros = true;
#endif
			EncodeZeroes(true);
		}
	}
}

inline void ArcEntropyEncoderModel::ResetCoefficientVariables(void)
{
	FoundFirstCoeff = false;
	TotalZeroes = 0;
	NumberOfTrailingOnes = 0;
	CoefficientCount = 0;
	RunCount = 0;
	memset(CurrentRun, 0, 16*sizeof(int));
	FoundFirstNonT1 = false;

}

/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::void ProcessCoefficient				 */
/* DESCRIPTION:	This function examines the coefficient passed to it
				and does prccesses it according to its value and the history
				of coefficients for the block.
				If coeff == 0 && non-zero coeff processed for the block
					then increment total zero counter and current run counter
				If coeff == 1 && T1 count < 3
					then increment T1 count and store sign; increment run index
					else treat as for coeff > 1
				If coeff > 1
					if first coeff > 1 && T1 count > 0
						code T1s
					code coeff
					increment run index

				See section 9.1.6 of H.26L spec

   PARAMETERS:	coeff - the value to be processed.

*/
/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::ProcessCoefficient(void)
{
    if (0 == Coeff)
	{
		if (!FoundFirstCoeff)
		{
			return;
		}
		TotalZeroes++;
		CurrentRun[RunCount-1]++;
	}
	else if (!FoundFirstNonT1 && (NumberOfTrailingOnes < 3) && (1 == Coeff))
	{
		RunCount++;
		FoundFirstCoeff = true;
		TrailingOnesSignTable[NumberOfTrailingOnes] = 0;
		NumberOfTrailingOnes++;
	}
	else if (!FoundFirstNonT1 && (NumberOfTrailingOnes < 3) && (-1 == Coeff))
	{
		RunCount++;
		FoundFirstCoeff = true;
		TrailingOnesSignTable[NumberOfTrailingOnes] = 1;
		NumberOfTrailingOnes++;
	}
	else
	{
		RunCount++;
		FoundFirstCoeff = true;
		if (!FoundFirstNonT1)
		{
			FoundFirstNonT1 = true;
		}
		CoefficientArray[CoefficientCount] = Coeff;
		CoefficientCount++;
	}
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	ArcEntropyEncoderModel::DetermineVLCTableForLumaCoeffToken	*/
/* DESCRIPTION:	This determines the VLC table that is to be used for
				decoding the next luma coeff token. The parameter is the
				block number in decoding order, and is 0 for intra DC.
				The VLC table depends on the number of non zero
				coefficients in the above and left 4x4 blocks if they are
				in the same slice as the current block.
				Intra DC is treated as if it were the top left AC
				4x4 block.
				The function sets the value of CoeffTokenTable.				*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */
inline void ArcEntropyEncoderModel::DetermineVLCTableForLumaCoeffToken(unsigned int encode_order_block_number)
{
	// Determine the number of coefficients in the above and left 4x4 blocks and use -1 to
	// indicate block is not available
    
	int nu = -1;
	int nl = -1;

	unsigned char raster_block_number = h264_4x4_transmission_to_raster_table[encode_order_block_number];
    
	if (raster_block_number < 4)
	{
		// Above block is in above macroblock
        if((MpcCurrentPtr->AboveMB.Flags & MBNF_VALID_ID)
         && (MpcCurrentPtr->CollectionID == MpcTopPtr->CollectionID))
		{
            nu = ((MpcTopPtr->Intra4x4ModeCoeffCountSB[0] >> raster_block_number*5 )) & COEFF_MASK;
            
		}
	}
	else
	{
      // Above block is in current macroblock
      short idx = CoffCntsBitPos[raster_block_number-4];
	  nu = ((MpcCurrentPtr->CoeffCountSB[idx >> 5])>> (idx % 32)) & COEFF_MASK;
	}

	if (0 == (raster_block_number & 3))
	{
		// Left block is in left macroblock
		unsigned char left_block_index = raster_block_number + 3;


        if ((MpcCurrentPtr->LeftMB.Flags & MBNF_VALID_ID)
         && (MpcCurrentPtr->CollectionID == MpcLeftPtr->CollectionID))
		{
			short idx = CoffCntsBitPos[left_block_index];
            nl = ((MpcLeftPtr->CoeffCountSB[(idx >> 5)]) >> (idx%32)) & COEFF_MASK;
		}
	}
	else
	{
		// Left block is in current macroblock
        short idx = CoffCntsBitPos[raster_block_number-1];
        nl = ((MpcCurrentPtr->CoeffCountSB[(idx >> 5)]) >> (idx%32))& COEFF_MASK;
	}

	int n;  // The value produced by combining nu and nl

	if ((nu < 0) && (nl < 0))
	{
		n = 0;
	}
	else if (nu < 0)
	{
		n = nl;
	}
	else if (nl < 0)
	{
		n = nu;
	}
	else
	{
		n = (nu + nl + 1) >> 1;
	}
#ifdef EE_DUMP
    sample_h264_nc = true;
    h264_nc = n;
#endif

	if (n < 2)
	{
		CoeffTokenTable = 0;
	}
	else if (n < 4)
	{
		CoeffTokenTable = 1;
	}
	else if (n < 8)
	{
		CoeffTokenTable = 2;
	}
	else
	{
		CoeffTokenTable = 3;
	}
}


/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::CodeCoefficientsToken				 */
/* DESCRIPTION:	This function encodes the total coefficients and trailing ones
				supplied as parameters into the coefficient token. It uses the
				table specified in CoeffTokenTable, which is set
				using the function DetermineVLCTableForLumaCoeffToken.
				NB this function does not code the token for chroma dc			*/

/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::CodeCoefficientsToken(void)
{
	int value;

    
	switch (CoeffTokenTable)
	{
	case 0:
		WriteFixedLengthCode(h264_total_coeff_trailing_ones_table0[TotalCoeffs][NumberOfTrailingOnes],
									  h264_total_coeff_trailing_ones_lengths_table0[TotalCoeffs][NumberOfTrailingOnes]);
		break;

	case 1:
		WriteFixedLengthCode(h264_total_coeff_trailing_ones_table1[TotalCoeffs][NumberOfTrailingOnes],
									  h264_total_coeff_trailing_ones_lengths_table1[TotalCoeffs][NumberOfTrailingOnes]);
		break;

	case 2:
		WriteFixedLengthCode(h264_total_coeff_trailing_ones_table2[TotalCoeffs][NumberOfTrailingOnes],
									  h264_total_coeff_trailing_ones_lengths_table2[TotalCoeffs][NumberOfTrailingOnes]);
		break;

	case 3:		// Have to calculate fixed length code
		if (0 == TotalCoeffs)
		{
			value = 3;
		}
		else
		{
			value = TotalCoeffs-1;
			value <<= 2;
			value += NumberOfTrailingOnes;
		}

		WriteFixedLengthCode(value, 6);
		break;

	default:
		// error - should never happen
		assert(false);
		break;
	}

}


/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel:: EncodeLevels							 */
/* DESCRIPTION:	This function encodes level information for all typesof block
				See section 9.1.6 of H.26L spec
   PARAMETERS:	initial_level - the level to be coded.  It is called initial_level
				because for the first coefficient to be transmitted for a bloc,
				if the number of trailing ones is less than three the absolute
				value of the level transmitted is reduced by one.
				the number of trailing ones
				first_coeff indicates whether the level is the first of a block	*/


/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

#ifdef EE_DUMP
inline void ArcEntropyEncoderModel::EncodeLevels(int initial_level, int trailing_ones, bool first_coeff, int block_num, int chroma_num, int index, int mode)
#else
inline void ArcEntropyEncoderModel::EncodeLevels(int initial_level, int trailing_ones, bool first_coeff)
#endif
{
#ifdef EE_DUMP
    int dc_offset;
    dc_offset = 16; // 16x1 litra dc luma coeffs
    int luma_ac_offset;
    luma_ac_offset = 256 + dc_offset; // 16x16 luma ac coeffs : jump over dc values and start placing ac values in element 16
    
    

    int level_code;

    if (initial_level >= 0) {
      level_code = (initial_level << 1) - 2;
      fprintf(verif_log, "LEVEL CODE = 0x%04X\n", (level_code&0xFFFF));
    }
    else {
      level_code = ((~initial_level) << 1) + 1;
      fprintf(verif_log, "LEVEL CODE = 0x%04X\n", (level_code&0xFFFF));
    };
    
    if (mode == 0) {// LUMA DC
	  intra_dc_lc_array[index] = level_code;
	  fprintf(verif_log, "intra_dc_lc_array[%d] = 0x%04X\n", index, (intra_dc_lc_array[index]&0xFFFF));

      coeff_lc_array[index] = level_code;
      coeff_valid_array[index] = 1;
	  fprintf(verif_log, "LUMA DC coeff_lc_array[%d] = 0x%04X\n", index, (coeff_lc_array[index]&0xFFFF));
    }
    else if (mode == 1) {// LUMA AC
	  luma_ac_lc_array[(block_num*16) + index] = level_code;
	  luma_ac_valid_array[(block_num*16) + index] = 1;
	  if (luma_ac_valid_array[(block_num*16) + index] == 1) {
        fprintf(verif_log, "luma_ac_lc_array[%d] = 0x%04X\n", (block_num*16) + index, (luma_ac_lc_array[(block_num*16) + index]&0xFFFF));
	    fprintf(verif_log, "luma_ac_valid_array[%d] = %d\n", (block_num*16) + index, luma_ac_valid_array[(block_num*16) + index]);
      };
      coeff_lc_array[(block_num*16) + index + dc_offset] = level_code;
      coeff_valid_array[(block_num*16) + index + dc_offset] = 1;
      fprintf(verif_log, "LUMA AC coeff_lc_array[%d] = 0x%04X\n", (block_num*16) + index + dc_offset, (coeff_lc_array[(block_num*16) + index + dc_offset]&0xFFFF));
    }            
    else if (mode == 2) {// CHROMA DC
	  chroma_dc_lc_array[(chroma_num*4) + index] = level_code;
	  fprintf(verif_log, "chroma_dc_lc_array[%d] = 0x%04X\n", (chroma_num*4) + index, (chroma_dc_lc_array[(chroma_num*4) + index]&0xFFFF));
    }
    else if (mode == 3) {// CHROMA AC
	  chroma_ac_lc_array[(chroma_num*64) + (block_num*16) + index] = level_code;
	  chroma_ac_valid_array[(chroma_num*64) + (block_num*16) + index] = 1;
      if (chroma_ac_valid_array[(chroma_num*64) + (block_num*16) + index] == 1) {
	    fprintf(verif_log, "chroma_ac_lc_array[%d] = 0x%04X\n", (chroma_num*64) + (block_num*16) + index, (chroma_ac_lc_array[(chroma_num*64) + (block_num*16) + index]&0xFFFF));
	    fprintf(verif_log, "chroma_ac_valid_array[%d] = %d\n", (chroma_num*64) + (block_num*16) + index, chroma_ac_valid_array[(chroma_num*64) + (block_num*16) + index]);
      };
      coeff_lc_array[(chroma_num*64) + (block_num*16) + index + luma_ac_offset] = level_code;
      coeff_valid_array[(chroma_num*64) + (block_num*16) + index + luma_ac_offset] = 1;
      fprintf(verif_log, "LUMA AC coeff_lc_array[%d] = 0x%04X\n", (block_num*16) + index + luma_ac_offset, (coeff_lc_array[(block_num*16) + index + luma_ac_offset]&0xFFFF));
    };
#endif

	int VlcIncTable[7] = {0, 3, 6, 12, 24, 48, 32768};
   int level = initial_level;
	if (first_coeff)
	{
		CurrentVLCTableNumber = ((TotalCoeffs > 10) && (NumberOfTrailingOnes < 3));

		if (trailing_ones < 3)
		{
			level -= (level > 0);		// If level positive, subtracts one
			level += (level < 0);		// If level negative, adds one
		}
	}

	int value=0; // default initialisation to remove compile time warning
	int length, sign;

	int abslevel = ABS(level);

#ifdef EE_DUMP
    fprintf(verif_log, "SUFFIX_LENGTH = %d\n", CurrentVLCTableNumber);
    if (mode == 0) { // LUMA DC
      intra16x16_dc_sl_array[index] = CurrentVLCTableNumber;
      fprintf(verif_log, "intra16x16_dc_sl_array[%d] = 0x%04X\n", index, (intra16x16_dc_sl_array[index]&0xFFFF));
      sample_intra16x16_dc_coeff = true;
      
      coeff_sl_array[index] = CurrentVLCTableNumber;
      fprintf(verif_log, "LUMA DC coeff_sl_array[%d] = 0x%04X\n", index, (coeff_sl_array[index]&0xFFFF));
      sample_coeff = true;
    }
    else if (mode == 1) {// LUMA AC
      luma_ac_sl_array[(block_num*16) + index] = CurrentVLCTableNumber;
	  if (luma_ac_valid_array[(block_num*16) + index] == 1) {
        fprintf(verif_log, "luma_ac_sl_array[%d] = 0x%04X\n", (block_num*16) + index, (luma_ac_sl_array[(block_num*16) + index]&0xFFFF));
      };
      sample_luma_ac_coeff = true;
      
      coeff_sl_array[(block_num*16) + index + dc_offset] = CurrentVLCTableNumber;
      fprintf(verif_log, "LUMA AC coeff_sl_array[%d] = 0x%04X\n", (block_num*16) + index + dc_offset, (coeff_sl_array[(block_num*16) + index + dc_offset]&0xFFFF));
      sample_coeff = true;
    }            
    else if (mode == 2) {// CHROMA DC
      chroma_dc_sl_array[(chroma_num*4) + index] = CurrentVLCTableNumber;
      fprintf(verif_log, "chroma_dc_sl_array[%d] = 0x%04X\n", (chroma_num*4) + index, (chroma_dc_sl_array[(chroma_num*4) + index]&0xFFFF));
      sample_chroma_dc_coeff = true;
    }
    else if (mode == 3) {// CHROMA AC
      chroma_ac_sl_array[(chroma_num*64) + (block_num*16) + index] = CurrentVLCTableNumber;
      if (chroma_ac_valid_array[(chroma_num*64) + (block_num*16) + index] == 1) {
        fprintf(verif_log, "chroma_ac_sl_array[%d] = 0x%04X\n", (chroma_num*64) + (block_num*16) + index, (chroma_ac_sl_array[(chroma_num*64) + (block_num*16) + index]&0xFFFF));
      };
      sample_chroma_ac_coeff = true;
      
      coeff_sl_array[(chroma_num*64) + (block_num*16) + index + luma_ac_offset] = CurrentVLCTableNumber;
      fprintf(verif_log, "CHROMA AC coeff_sl_array[%d] = 0x%04X\n", (chroma_num*64) + (block_num*16) + index + luma_ac_offset, (coeff_sl_array[(chroma_num*64) + (block_num*16) + index + luma_ac_offset]&0xFFFF));
      sample_coeff = true;
    };
#endif

	if (0 == CurrentVLCTableNumber)
	{
		if (15 >= abslevel)	// Use table to determine code
		{
#ifdef EE_DUMP
            if (h264_vlc0_table[level+15][1] <= 4 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%01X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 8 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%02X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 12 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%03X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 16 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%04X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 20 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%05X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 24 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%06X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 28 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%07X\n", value);
            }
            else if (h264_vlc0_table[level+15][1] <= 32 ) {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%08X\n", value);
            }
            else {
              fprintf(verif_log, "ENCODE LEVELS_1: VALUE ADDED TO BUFFER = 0x%08X\n", value);
            };
            fprintf(verif_log, "ENCODE LEVELS_1: LENGTH OF VALUE TO BE ENCODED = %d\n", h264_vlc0_table[level+15][1]);
            fflush(verif_log);
            
#endif
			WriteFixedLengthCode(h264_vlc0_table[level+15][0], h264_vlc0_table[level+15][1]);
		}
		else						// Calculate code
		{
			length = 28;
			sign = level<0;		// sign = 1 if negative, 0 if positive
			value = 4096;		// flag bit
			value += ((abslevel-16) << 1)+sign;
#ifdef EE_DUMP
            fprintf(verif_log, "ENCODE LEVELS_2: VALUE ADDED TO BUFFER = 0x%07X\n", value);
            fprintf(verif_log, "ENCODE LEVELS_2: LENGTH OF VALUE TO BE ENCODED = %d\n", length);
            fflush(verif_log);
#endif
			WriteFixedLengthCode(value, length);
		}

	}
	else		// Tables 1-6 - use formula
	{
		if ((abslevel-1)<(15<<(CurrentVLCTableNumber-1)))
		{
			length = ((abslevel - 1) >> (CurrentVLCTableNumber-1)) // number of zeroes
						+ CurrentVLCTableNumber + 1;				// number of x's  + flag bit(1) + sign bit

			sign = level<0;		// sign = 1 if negative, 0 if positive

			value = 1;												// Flag bit
			value <<= (CurrentVLCTableNumber-1);							// Shift it by number of x's
			int two_exp_vlc_table_number_minus_one = 2 << (CurrentVLCTableNumber-2);
			two_exp_vlc_table_number_minus_one += (two_exp_vlc_table_number_minus_one == 0);  // If zero, make it one
			value += (abslevel-1) % two_exp_vlc_table_number_minus_one * ((CurrentVLCTableNumber-1) != 0);	// Add value of x's
			value <<= 1;											// Shift it for sign bit
			value += sign;											// Add sign bit
#ifdef EE_DUMP
            if (length <= 4 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%01X\n", value);
            }
            else if (length <= 8 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%02X\n", value);
            }
            else if (length <= 12 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%03X\n", value);
            }
            else if (length <= 16 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%04X\n", value);
            }
            else if (length <= 20 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%05X\n", value);
            }
            else if (length <= 24 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%06X\n", value);
            }
            else if (length <= 28 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%07X\n", value);
            }
            else if (length <= 32 ) {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%08X\n", value);
            }
            else {
              fprintf(verif_log, "ENCODE LEVELS_3: VALUE ADDED TO BUFFER = 0x%08X\n", value);
            };
            fprintf(verif_log, "ENCODE LEVELS_3: LENGTH OF VALUE TO BE ENCODED = %d\n", length);
            fflush(verif_log);
#endif
			WriteFixedLengthCode(value, length);

		}
		else
		{
			length = 28;
			sign = level<0;
			value = 2048;		// Flag bit
			value += (abslevel-1) - (15 << (CurrentVLCTableNumber-1));
			value <<= 1;
			value += sign;
#ifdef EE_DUMP
            fprintf(verif_log, "ENCODE LEVELS_4: VALUE TO BE ADDED TO BUFFER = 0x%07X\n", value);
            fprintf(verif_log, "ENCODE LEVELS_4: LENGTH OF VALUE TO BE ENCODED = %d\n", length);
            fflush(verif_log);
#endif
			WriteFixedLengthCode(value, length);
		}
	}

	int absinitiallevel = ABS(initial_level);
	if (absinitiallevel > VlcIncTable[CurrentVLCTableNumber])
	{
		CurrentVLCTableNumber++;
	}
	if (first_coeff && (absinitiallevel > 3))
	{
		CurrentVLCTableNumber = 2;
	}

	first_coeff = false;
}


/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::void EncodeZeroes						 */
/* DESCRIPTION:	This function encodes the run information for all block types
				See section 9.1.6 of H.26L spec									*/


/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::EncodeZeroes(bool dc_coeff_ignored)
{

#ifdef EE_DUMP
    fprintf(verif_log, "\n----------------------------------------------\n");
    fprintf(verif_log, "ENCODE ZEROES & RUN WITH TotalCoeffs = %d AND TotalZeroes = %d\n", TotalCoeffs, TotalZeroes);
    fflush(verif_log);
#endif

	// Code total zeroes - but only if all coefficients are not non-zero
	if (TotalCoeffs != (16 - (true == dc_coeff_ignored)))
	{
		// choose total zeros table
		switch(TotalCoeffs)
		{
			case 1:
				WriteFixedLengthCode(h264_total_zeros_1_coeff[TotalZeroes][0], h264_total_zeros_1_coeff[TotalZeroes][1]);
				break;
			case 2:
				WriteFixedLengthCode(h264_total_zeros_2_coeff[TotalZeroes][0], h264_total_zeros_2_coeff[TotalZeroes][1]);
				break;
			case 3:
				WriteFixedLengthCode(h264_total_zeros_3_coeff[TotalZeroes][0], h264_total_zeros_3_coeff[TotalZeroes][1]);
				break;
			case 4:
				WriteFixedLengthCode(h264_total_zeros_4_coeff[TotalZeroes][0], h264_total_zeros_4_coeff[TotalZeroes][1]);
				break;
			case 5:
				WriteFixedLengthCode(h264_total_zeros_5_coeff[TotalZeroes][0], h264_total_zeros_5_coeff[TotalZeroes][1]);
				break;
			case 6:
				WriteFixedLengthCode(h264_total_zeros_6_coeff[TotalZeroes][0], h264_total_zeros_6_coeff[TotalZeroes][1]);
				break;
			case 7:
				WriteFixedLengthCode(h264_total_zeros_7_coeff[TotalZeroes][0], h264_total_zeros_7_coeff[TotalZeroes][1]);
				break;
			case 8:
				WriteFixedLengthCode(h264_total_zeros_8_coeff[TotalZeroes][0], h264_total_zeros_8_coeff[TotalZeroes][1]);
				break;
			case 9:
				WriteFixedLengthCode(h264_total_zeros_9_coeff[TotalZeroes][0], h264_total_zeros_9_coeff[TotalZeroes][1]);
				break;
			case 10:
				WriteFixedLengthCode(h264_total_zeros_10_coeff[TotalZeroes][0], h264_total_zeros_10_coeff[TotalZeroes][1]);
				break;
			case 11:
				WriteFixedLengthCode(h264_total_zeros_11_coeff[TotalZeroes][0], h264_total_zeros_11_coeff[TotalZeroes][1]);
				break;
			case 12:
				WriteFixedLengthCode(h264_total_zeros_12_coeff[TotalZeroes][0], h264_total_zeros_12_coeff[TotalZeroes][1]);
				break;
			case 13:
				WriteFixedLengthCode(h264_total_zeros_13_coeff[TotalZeroes][0], h264_total_zeros_13_coeff[TotalZeroes][1]);
				break;
			case 14:
				WriteFixedLengthCode(h264_total_zeros_14_coeff[TotalZeroes][0], h264_total_zeros_14_coeff[TotalZeroes][1]);
				break;
			case 15:
				WriteFixedLengthCode(h264_total_zeros_15_coeff[TotalZeroes][0], h264_total_zeros_15_coeff[TotalZeroes][1]);
				break;

			default:		// error
				assert(false);
				break;

		}
		// Now code runs
		int run;
		int zeroes_left = TotalZeroes;

		for (int i = 0; i < RunCount-1; i++)	// We don't code the last run
		{
			if (0 == zeroes_left)
			{
				break;
			}
			run = CurrentRun[i];
#ifdef EE_DUMP
            //zeros_left_sampled = zeroes_left;
            //run_sampled = run;
            fprintf(verif_log, "ENCODE RUN_BEFORE WITH zeroes_left = %d AND run_before = %d\n", zeroes_left, run);
            run_before = run;
            run_zeros_left  = zeroes_left;
            sample_run_before = true;
            fflush(verif_log);
#endif
			// Use the appropriate table
			switch(zeroes_left)
			{
				case 1:
					WriteFixedLengthCode(vld_run_before_1_zeros_left[run][0], vld_run_before_1_zeros_left[run][1]);
					break;
				case 2:
					WriteFixedLengthCode(vld_run_before_2_zeros_left[run][0], vld_run_before_2_zeros_left[run][1]);
					break;
				case 3:
					WriteFixedLengthCode(vld_run_before_3_zeros_left[run][0], vld_run_before_3_zeros_left[run][1]);
					break;
				case 4:
					WriteFixedLengthCode(vld_run_before_4_zeros_left[run][0], vld_run_before_4_zeros_left[run][1]);
					break;
				case 5:
					WriteFixedLengthCode(vld_run_before_5_zeros_left[run][0], vld_run_before_5_zeros_left[run][1]);
					break;
				case 6:
					WriteFixedLengthCode(vld_run_before_6_zeros_left[run][0], vld_run_before_6_zeros_left[run][1]);
					break;
				default:
					if ((6 < zeroes_left) && (15 > zeroes_left))
					{
						WriteFixedLengthCode(vld_run_before_7_zeros_left[run][0], vld_run_before_7_zeros_left[run][1]);
					}
					else	// error
					{
						assert(false);
					}

			}
			zeroes_left -= run;

		}
	}
}


/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::void CodeChromaDCCoefficientsToken		 */
/* DESCRIPTION:	This function encodes the total coefficients and trailing ones
				supplied as parameters into the coefficient token for DC Chroma.
				It uses the total coeff/trailing ones table specific to Chroma
				DC.																*/

/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::CodeChromaDCCoefficientsToken(void)
{
	WriteFixedLengthCode(h264_total_coeff_trailing_ones_table_DC_chroma[TotalCoeffs][NumberOfTrailingOnes],
									 h264_total_coeff_trailing_ones_length_table_DC_chroma[TotalCoeffs][NumberOfTrailingOnes]);
}

/* ----------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::void CodeChromaDCZeroes				 */
/* DESCRIPTION:	This function encodes the run information for all block types
				See section 9.1.6 of H.26L spec									*/


/* RETURNS:		void.														     */
/* ----------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::CodeChromaDCZeroes(void)
{

	// Code total zeroes - but only if all coefficients are not non-zero
	if (TotalCoeffs != 4)
	{
		// choose total zeros table
		switch(TotalCoeffs)
		{
			case 1:
				WriteFixedLengthCode(h264_total_zeros_dc_1_coeff[TotalZeroes][0], h264_total_zeros_dc_1_coeff[TotalZeroes][1]);
				break;
			case 2:
				WriteFixedLengthCode(h264_total_zeros_dc_2_coeff[TotalZeroes][0], h264_total_zeros_dc_2_coeff[TotalZeroes][1]);
				break;
			case 3:
				WriteFixedLengthCode(h264_total_zeros_dc_3_coeff[TotalZeroes][0], h264_total_zeros_dc_3_coeff[TotalZeroes][1]);
				break;


			default:		// error
				assert(false);
				break;

		}
		// Now code runs
		int run;
		int zeroes_left = TotalZeroes;

		for (int i = 0; i < RunCount-1; i++)	// We don't code the last run
		{
			if (0 == zeroes_left)
			{
				break;
			}
			run = CurrentRun[i];
			// Use the appropriate table
			switch(zeroes_left)
			{
				case 1:
					WriteFixedLengthCode(vld_run_before_1_zeros_left[run][0], vld_run_before_1_zeros_left[run][1]);
					break;
				case 2:
					WriteFixedLengthCode(vld_run_before_2_zeros_left[run][0], vld_run_before_2_zeros_left[run][1]);
					break;
				case 3:
					WriteFixedLengthCode(vld_run_before_3_zeros_left[run][0], vld_run_before_3_zeros_left[run][1]);
					break;
				case 4:
					WriteFixedLengthCode(vld_run_before_4_zeros_left[run][0], vld_run_before_4_zeros_left[run][1]);
					break;
				case 5:
					WriteFixedLengthCode(vld_run_before_5_zeros_left[run][0], vld_run_before_5_zeros_left[run][1]);
					break;
				case 6:
					WriteFixedLengthCode(vld_run_before_6_zeros_left[run][0], vld_run_before_6_zeros_left[run][1]);
					break;
				default:
					if ((6 < zeroes_left) && (15 > zeroes_left))
					{
						WriteFixedLengthCode(vld_run_before_7_zeros_left[run][0], vld_run_before_7_zeros_left[run][1]);
					}
					else	// error
					{
						assert(false);
					}

			}
			zeroes_left -= run;

		}
	}
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::DetermineVLCTableForUVCoeffToken		*/
/* DESCRIPTION:	This determines the VLC table that is to be used for
				decoding the next U coeff token. The parameter is the
				block number in decoding order.
				The VLC table depends on the number of non zero
				coefficients in the above and left 4x4 blocks if they are
				in the same slice as the current block.
				Intra Chroma DC has its own table so is not relevant to this
				function.
				The function sets the value of m_SymbolEjectCoeffTokenTable.	*/
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */

inline void ArcEntropyEncoderModel::DetermineVLCTableForUVCoeffToken(unsigned char encode_order_block_number,const short * bit_pos,short sh_off)
{
	// Determine the number of coefficients in the above and left 4x4 blocks and use -1 to
	// indicate block is not available
	// The following were defined as char but this caused problems on symbian where
	// the initialisation to -1 caused initialisation to 255.
	int nu = -1;
	int nl = -1;
	unsigned char raster_block_number = encode_order_block_number;

	if (raster_block_number < 2)
	{
		// Above block is in above macroblock
      if((MpcCurrentPtr->AboveMB.Flags & MBNF_VALID_ID)
         && (MpcCurrentPtr->CollectionID == MpcTopPtr->CollectionID))
		{
         nu = ((MpcTopPtr->Intra4x4ModeCoeffCountSB[1])>>(raster_block_number*5 +sh_off))& COEFF_MASK;
		}
	}
	else
	{
	  // Above block is in current macroblock
      short idx = bit_pos[(raster_block_number-2)];
	  nu = ((MpcCurrentPtr->CoeffCountSB[(idx>>5)]) >> (idx %32)) & COEFF_MASK;
	}

	if (0 == (raster_block_number & 1))
	{
		// Left block is in left macroblock
		unsigned char left_block_index = raster_block_number + 1;

      if((MpcCurrentPtr->LeftMB.Flags & MBNF_VALID_ID)
         && (MpcCurrentPtr->CollectionID == MpcLeftPtr->CollectionID))
	    {
            short idx = bit_pos[left_block_index];
            nl = ((MpcLeftPtr->CoeffCountSB[(idx>>5)]) >> (idx %32))& COEFF_MASK;
		}
	}
	else
	{
	    // Left block is in current macroblock
        short idx = bit_pos[raster_block_number-1];
		nl = ((MpcCurrentPtr->CoeffCountSB[(idx>>5)]) >> (idx %32))& COEFF_MASK;
	}

	int n;  // The value produced by combining nu and nl
	if ((nu < 0) && (nl < 0))
	{
		n = 0;
	}
	else if (nu < 0)
	{
		n = nl;
	}
	else if (nl < 0)
	{
		n = nu;
	}
	else
	{
		n = (nu + nl + 1) >> 1;
	}

	if (n < 2)
	{
		CoeffTokenTable = 0;
	}
	else if (n < 4)
	{
		CoeffTokenTable = 1;
	}
	else if (n < 8)
	{
		CoeffTokenTable = 2;
	}
	else
	{
		CoeffTokenTable = 3;
	}
#ifdef EE_DUMP
    h264_chroma_nc = n;
    fprintf(verif_log, "h264_chroma_nc = %d\n", h264_chroma_nc);
    fflush(verif_log);
#endif
}


#ifdef ARC_ISS_EE_MODEL

// return true if the EE module can accept new channel command
// It can accept new channel command only if the module is not 
// halted, and encoding/concatination is in operation or there is a valid
// response from the previous channel command
bool ArcEntropyEncoderModel::CanEEAcceptChannelCommand(void)
{
   int val = 0;
   bool ret = true;
   for(int i=0;i<4;i++)
   {
      if(ChannelResponse.RegisterData[i].valid)
      {
         val = 1;
         break;
      }
   }

   if((EE_AUX_REGS[EE_DEBUG_LOC] & 0x30000)
    |(EE_AUX_REGS[STATUS_CTRL] & 0x3)
    |(val))
   {
      ret = false;
   }

   return ret;
}
#endif

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::Mpeg4EntropyEncodeMacroBlock    		*/
/* DESCRIPTION:	VLC encodes a macroblock as per the MPEG4 syntax.				*/
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::Mpeg4EntropyEncodeMacroBlock(void)
{
    int cbpc, cbpy, pred_x, pred_y;
    const int interleaved_stats= /*(s->flags&CODEC_FLAG_PASS1) && !s->data_partitioning ? 1 : */0;
    const int dquant_code[5]= {1,0,9,2,3};
	int motion_x = MpcCurrentPtr->mv[0].mvx, motion_y = MpcCurrentPtr->mv[0].mvy;
	unsigned int dquant = EE_AUX_REGS[DELTA_QUANT];
    short block[6][64];
    int mpcBufOffset;

	// Set the cbp data member based on the NonZeroCoeff fields in the MPC data structs
	CalculateCodedBlockPatternH263();

	if (MpcCurrentPtr->MBCodingType != MBCT_MPEG4_INTRA) {
        int i;

        if (MpcCurrentPtr->MBCodingType == MBCT_MPEG4_SKIPPED)
            {
#ifdef EE_DUMP
                fprintf(verif_log, "CURRENT MB IS A SKIP\n");
                fflush(verif_log);
#endif
                /* skip macroblock */
                WriteFixedLengthCode( 1, 1);
                return;                
            }
#ifdef EE_DUMP
            sample_mpeg4_not_coded = true;
            mpeg4_not_coded = 0;
#endif
            WriteFixedLengthCode( 0, 1);	/* mb coded */
            cbpc = cbp & 3;
            cbpy = cbp >> 2;
            cbpy ^= 0xf;
#ifdef EE_DUMP
            fprintf(verif_log, "CBPC = %d\n", cbpc);
            fprintf(verif_log, "CBPY = %d\n", cbpy);
            fprintf(verif_log, "MpcCurrentPtr->MBCodingType = %d\n", MpcCurrentPtr->MBCodingType);
            fflush(verif_log);
            
            
            sample_mpeg4_cbpc = true;
            mpeg4_cbpc = cbpc;
            mpeg4_vop = EE_AUX_REGS[SLICE_TYPE]&0x1;
            mpeg4_mb_type = MpcCurrentPtr->MBCodingType;
            mpeg4_dquant =  EE_AUX_REGS[DELTA_QUANT];
            fprintf(verif_log, "MPEG4_VOP = 0x%x\n", mpeg4_vop);
            fprintf(verif_log, "MPEG4_MB_TYPE = 0x%x\n", mpeg4_mb_type);
            fprintf(verif_log, "MPEG4_DQUANT = 0x%x\n", mpeg4_dquant);
#endif

            if(MpcCurrentPtr->MBCodingType==MBCT_MPEG4_INTER16X16){
                if(dquant) cbpc+= 8;
                WriteFixedLengthCode(
                        inter_MCBPC_code[cbpc], 
						inter_MCBPC_bits[cbpc]
                        );
                
                #ifdef EE_DUMP
                  sample_mpeg4_cbpy = true;
                  mpeg4_cbpy = cbpy;
                #endif

                WriteFixedLengthCode(cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
                if(dquant)
                    WriteFixedLengthCode(dquant_code[dquant+2], 2);

                /* motion vectors: 16x16 mode */
                pred_x = MpcCurrentPtr->mvp[0].mvx;
                pred_y = MpcCurrentPtr->mvp[0].mvy;
                
                #ifdef EE_DUMP
                  fprintf(verif_log, "\n CALLING ArcEncodeMotion FUNTION FOR HORIZONTAL MVs\n");
                  fflush(verif_log);
                #endif
                ArcEncodeMotion(motion_x - pred_x, 1/*control->f_code*/);
                
                #ifdef EE_DUMP
                  fprintf(verif_log, "\n CALLING ArcEncodeMotion FUNTION FOR VERTICAL MVs\n");
                  fflush(verif_log);
                #endif
                ArcEncodeMotion(motion_y - pred_y, 1/*control->f_code*/);
            }else {
                WriteFixedLengthCode(
                        inter_MCBPC_code[cbpc+16], 
						inter_MCBPC_bits[cbpc+16]
                        );

                #ifdef EE_DUMP
                  sample_mpeg4_cbpy = true;
                  mpeg4_cbpy = cbpy;
                #endif
                WriteFixedLengthCode(cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);

                for(i=0; i<4; i++){
                    /* motion vectors: 8x8 mode*/
                    pred_x = MpcCurrentPtr->mvp[i].mvx;
                    pred_y = MpcCurrentPtr->mvp[i].mvy;
                
                    #ifdef EE_DUMP
                      fprintf(verif_log, "\n CALLING ArcEncodeMotion FUNTION FOR HORIZONTAL MVs\n");
                      fflush(verif_log);
                    #endif
                    ArcEncodeMotion(MpcCurrentPtr->mv[i].mvx - pred_x, 1/*s->f_code*/);
                    
                    #ifdef EE_DUMP
                      fprintf(verif_log, "\n CALLING ArcEncodeMotion FUNTION FOR VERTICAL MVs\n");
                      fflush(verif_log);
                    #endif
                    ArcEncodeMotion(MpcCurrentPtr->mv[i].mvy - pred_y, 1/*s->f_code*/);
                }
            }


            /* encode each block */
            for (i = 0; i < 4; i++) {
                mpcBufOffset = (i/2)*PCB_VLC_TRANSFORM_Y_STRIDE*8 + (i%2)*(PCB_VLC_TRANSFORM_Y_STRIDE/2);
                ArcLoad8x8Block(block[i], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[mpcBufOffset])), PCB_VLC_TRANSFORM_Y_STRIDE/2);
#ifdef EE_DUMP
                fprintf(verif_log, "CALLING ArcEncodeBlockMP4 WITH i = \n", i);
#endif
                ArcEncodeBlockMP4(block[i], i, 0, (unsigned char *)ff_zigzag_direct);
            }
            // U block
            ArcLoad8x8Block(block[4], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_U])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
            ArcEncodeBlockMP4(block[4], 4, 0, (unsigned char *)ff_zigzag_direct);
            // V block
            ArcLoad8x8Block(block[5], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_V])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
            ArcEncodeBlockMP4(block[5], 5, 0, (unsigned char *)ff_zigzag_direct);
        
    } else { // intra block
#ifdef EE_DUMP
        fprintf(verif_log, "\nCURRENT MACROBLOCK IS INTRA\n");
#endif
    
		unsigned char *scan_table[6];
        int i;

//        if(m_use_ac_pred /*s->flags & CODEC_FLAG_AC_PRED*/){
//        }else{
//			control->ac_pred = 0;
            for(i=0; i<6; i++)
                scan_table[i]= (unsigned char *)ff_zigzag_direct;
//        }

        cbpc = cbp & 3;
        #ifdef EE_DUMP
          fprintf(verif_log, "\nCBP = 0x%X\n", cbp);
          fprintf(verif_log, "\nCBPC = 0x%X\n", cbpc);
            
          sample_mpeg4_cbpc = true;
          mpeg4_cbpc = cbpc;
          mpeg4_vop = EE_AUX_REGS[SLICE_TYPE]&0x1;
          mpeg4_mb_type = MpcCurrentPtr->MBCodingType;
          mpeg4_dquant =  EE_AUX_REGS[DELTA_QUANT];
          fprintf(verif_log, "MPEG4_VOP = 0x%x\n", mpeg4_vop);
          fprintf(verif_log, "MPEG4_MB_TYPE = 0x%x\n", mpeg4_mb_type);
          fprintf(verif_log, "MPEG4_DQUANT = 0x%x\n", mpeg4_dquant);
        #endif
          
        if (EE_AUX_REGS[SLICE_TYPE] == EE_I_SLICE) {
            if(dquant) cbpc+=4;
        #ifdef EE_DUMP
          fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH MCBPC VALUE = 0x%X AND LENGTH = %d\n", intra_MCBPC_code[cbpc], intra_MCBPC_bits[cbpc]);
        #endif
          
            WriteFixedLengthCode(
                intra_MCBPC_code[cbpc], 
				intra_MCBPC_bits[cbpc]
                );
        } else {
            if(dquant) cbpc+=8;
            WriteFixedLengthCode( 0, 1);	/* mb coded */
            WriteFixedLengthCode(
				inter_MCBPC_code[cbpc + 4],
                inter_MCBPC_bits[cbpc + 4]
                );
        }
#ifdef EE_DUMP
        fprintf(verif_log, "\nWRITING AC_PRED_FLAG = 0 TO WriteFixedLengthCode\n");
#endif
        WriteFixedLengthCode(0, 1);
        cbpy = cbp >> 2;
#ifdef EE_DUMP
        //fprintf(verif_log, "INTRA MACROBLOCK\n", MpcCurrentPtr->MBCodingType);
        //fprintf(verif_log, "CBP  = 0x%X\n", cbp);
        //fprintf(verif_log, "CBPC = 0x%X\n", cbpc);
        //fprintf(verif_log, "CBPY = 0x%X\n", cbpy);
        fprintf(verif_log, "MpcCurrentPtr->MBCodingType = %d\n", MpcCurrentPtr->MBCodingType);
        fprintf(verif_log, "DQUANT = %d\n", dquant);
        sample_mpeg4_cbpy = true;
        mpeg4_cbpy = cbpy;
#endif

#ifdef EE_DUMP
        fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH CBPY VALUE = 0x%X AND LENGTH = %d\n", cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
#endif
        WriteFixedLengthCode( cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
        if(dquant) {
            #ifdef EE_DUMP
              fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", dquant_code[dquant+2], 2);
            #endif
            WriteFixedLengthCode( dquant_code[dquant+2], 2);
        }
        
        /* encode each block */
        // Y blocks
        for (i = 0; i < 4; i++) {
#ifdef EE_DUMP
            fprintf(verif_log, "\nENCODING 8x8 BLOCK NUMBER %d\n", i);
#endif
            mpcBufOffset = (i/2)*PCB_VLC_TRANSFORM_Y_STRIDE*8 + (i%2)*(PCB_VLC_TRANSFORM_Y_STRIDE/2);
            ArcLoad8x8Block(block[i], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[mpcBufOffset])), PCB_VLC_TRANSFORM_Y_STRIDE/2);
            ArcEncodeBlockMP4(block[i], i, MpcCurrentPtr->Y_DC_CoeffStore0[i], scan_table[i]);
        }
        // U block
#ifdef EE_DUMP
        fprintf(verif_log, "ENCODING 8x8 BLOCK NUMBER 4\n");
#endif
        ArcLoad8x8Block(block[4], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_U])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
        ArcEncodeBlockMP4(block[4], 4, MpcCurrentPtr->U_DC_CoeffStore[0], scan_table[4]);

#ifdef EE_DUMP
        fprintf(verif_log, "ENCODING 8x8 BLOCK NUMBER 5\n");
#endif
        // V block
        ArcLoad8x8Block(block[5], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_V])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
        ArcEncodeBlockMP4(block[5], 5, MpcCurrentPtr->U_DC_CoeffStore[1], scan_table[5]);

    }
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::H263EntropyEncodeMacroBlock     		*/
/* DESCRIPTION:	VLC encodes a macroblock as per the H.263 syntax.				*/
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::H263EntropyEncodeMacroBlock(void)
{
    int cbpc, cbpy, i, pred_x, pred_y;
    const int interleaved_stats= 0;
    const int dquant_code[5]= {1,0,9,2,3};
	int motion_x = MpcCurrentPtr->mv[0].mvx, motion_y = MpcCurrentPtr->mv[0].mvy;
	unsigned int dquant = EE_AUX_REGS[DELTA_QUANT];
    short block[6][64];
    int mpcBufOffset;

	// Set the cbp data member based on the NonZeroCoeff fields in the MPC data structs
	CalculateCodedBlockPatternH263();

	if (MpcCurrentPtr->MBCodingType != MBCT_MPEG4_INTRA/*!s->mb_intra*/) {
	    if (MpcCurrentPtr->MBCodingType == MBCT_MPEG4_SKIPPED) 
		{
            /* skip macroblock */
            #ifdef EE_DUMP
              fprintf(verif_log, "\nEE_I_SLICE: WRITING TO WriteFixedLengthCode FOR SKIPPED MB WITH VALUE = 0x%X AND LENGTH = %d\n", 1, 1);
            #endif
            WriteFixedLengthCode( 1, 1);
            return;
        }
							
        #ifdef EE_DUMP
          fprintf(verif_log, "\nEE_P_SLICE: WRITING TO WriteFixedLengthCode WITH NOT_CODED VALUE = 0x%X AND LENGTH = %d\n", 0, 1);
          sample_h263_not_coded = true;
          h263_not_coded = 0;
        #endif
        WriteFixedLengthCode( 0, 1);	/* mb coded */

        cbpc = cbp & 3;
        cbpy = cbp >> 2;
        cbpy ^= 0xF;
        #ifdef EE_DUMP
          fprintf(verif_log, "\nCBP = 0x%X\n", cbp);
          fprintf(verif_log, "\nCBPC = 0x%X\n", cbpc);
          sample_h263_cbpc = true;
          h263_cbpc = cbpc;
          h263_vop = EE_AUX_REGS[SLICE_TYPE]&0x1;
          h263_mb_type = MpcCurrentPtr->MBCodingType;
          h263_dquant =  EE_AUX_REGS[DELTA_QUANT];
          fprintf(verif_log, "H263_VOP = 0x%x\n", h263_vop);
          fprintf(verif_log, "H263_MB_TYPE = 0x%x\n", h263_mb_type);
          fprintf(verif_log, "H263_DQUANT = 0x%x\n", h263_dquant);
        #endif
        if(dquant) cbpc+= 8;
        if(MpcCurrentPtr->MBCodingType==MBCT_MPEG4_INTER16X16){
            #ifdef EE_DUMP
              fprintf(verif_log, "\nEE_P_SLICE: WRITING TO WriteFixedLengthCode WITH MCBPC VALUE = 0x%X AND LENGTH = %d\n", inter_MCBPC_code[cbpc], inter_MCBPC_bits[cbpc]);
            #endif
            WriteFixedLengthCode(
                    inter_MCBPC_code[cbpc], 
                    inter_MCBPC_bits[cbpc]);

            #ifdef EE_DUMP
              sample_h263_cbpy = true;
              h263_cbpy = cbpy;
              fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH CBPY VALUE = 0x%X AND LENGTH = %d\n", cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
            #endif
            WriteFixedLengthCode(cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
            if(dquant)
			{
				if(!((EE_AUX_REGS[CODEC_CFG]>>5) & 1)){ // not Annexe T (Modified quant)
                    #ifdef EE_DUMP
                      fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", dquant_code[dquant+2], 2);
                    #endif
	                WriteFixedLengthCode( dquant_code[dquant+2], 2 );
				}
				else // modified quant 
				{	// quant - dquant is previous quant
					if(modified_quant[0][MpcCurrentPtr->QuantValue - dquant] == dquant) {
                  	    #ifdef EE_DUMP
                          fprintf(verif_log, "\nif WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", 2, 2);
                    	#endif
						WriteFixedLengthCode(2, 2);
                    }    
					else if(modified_quant[1][MpcCurrentPtr->QuantValue - dquant] == dquant) {
                  	    #ifdef EE_DUMP
                          fprintf(verif_log, "\nif WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", 3, 2);
                    	#endif
						WriteFixedLengthCode(3, 2);
				    }
                    else {
                  	    #ifdef EE_DUMP
                          fprintf(verif_log, "\nif WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", MpcCurrentPtr->QuantValue, 6);
                        #endif
                        WriteFixedLengthCode(MpcCurrentPtr->QuantValue, 6);
                    }
				}
			}

            /* motion vectors: 16x16 mode */
            pred_x = MpcCurrentPtr->mvp[0].mvx;
            pred_y = MpcCurrentPtr->mvp[0].mvy;

            #ifdef EE_DUMP
              fprintf(verif_log, "\nCALLING ArcEncodeMotion FUNTION FOR HORIZONTAL MVs\n");
              fflush(verif_log);
            #endif
            ArcEncodeMotion(motion_x - pred_x, 1);
            
            #ifdef EE_DUMP
              fprintf(verif_log, "\nCALLING ArcEncodeMotion FUNTION FOR VERTICAL MVs\n");
              fflush(verif_log);
            #endif
            ArcEncodeMotion(motion_y - pred_y, 1);

        }else{ // 4MVs
            WriteFixedLengthCode(
                    inter_MCBPC_code[cbpc+16], 
                    inter_MCBPC_bits[cbpc+16]);
            WriteFixedLengthCode(cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
            if(dquant)
			{
				if(!((EE_AUX_REGS[CODEC_CFG]>>5) & 1)){ // not Annexe T (Modified quant)
	                WriteFixedLengthCode(dquant_code[dquant+2], 2);
				}
				else // modified quant 
				{	// quant - dquant is previous quant
					if(modified_quant[0][MpcCurrentPtr->QuantValue - dquant] == dquant)
						WriteFixedLengthCode( 2, 2);
					else if(modified_quant[1][MpcCurrentPtr->QuantValue - dquant] == dquant)
						WriteFixedLengthCode( 3, 2);
					else WriteFixedLengthCode(MpcCurrentPtr->QuantValue, 6);
				}
			}
            for(i=0; i<4; i++){
                /* motion vectors: 8x8 mode*/
                pred_x = MpcCurrentPtr->mvp[i].mvx;
                pred_y = MpcCurrentPtr->mvp[i].mvy;

                motion_x= MpcCurrentPtr->mv[i].mvx;//s->current_picture.motion_val[0][ s->block_index[i] ][0];
                motion_y= MpcCurrentPtr->mv[i].mvy;//s->current_picture.motion_val[0][ s->block_index[i] ][1];


                #ifdef EE_DUMP
                  fprintf(verif_log, "\nCALLING ArcEncodeMotion FUNTION FOR HORIZONTAL MVs\n");
                  fflush(verif_log);
                #endif
                ArcEncodeMotion(motion_x - pred_x, 1);
            
                #ifdef EE_DUMP
                  fprintf(verif_log, "\nCALLING ArcEncodeMotion FUNTION FOR VERTICAL MVs\n");
                  fflush(verif_log);
                #endif
                ArcEncodeMotion(motion_y - pred_y, 1);

            }
        }


    } else {// intra
       #ifdef EE_DUMP
         fprintf(verif_log, "\nH263 INTRA MB\n");
       #endif
      
      //  assert(s->mb_intra);
//		control->coded = 1;// used in deblock

        cbpc = cbp & 3;
        #ifdef EE_DUMP
          fprintf(verif_log, "\nCBP = 0x%X\n", cbp);
          fprintf(verif_log, "\nCBPC = 0x%X\n", cbpc);
          sample_h263_cbpc = true;
          h263_cbpc = cbpc;
          h263_vop = EE_AUX_REGS[SLICE_TYPE]&0x1;
          h263_mb_type = MpcCurrentPtr->MBCodingType;
          h263_dquant =  EE_AUX_REGS[DELTA_QUANT];
          fprintf(verif_log, "H263_VOP = 0x%x\n", h263_vop);
          fprintf(verif_log, "H263_MB_TYPE = 0x%x\n", h263_mb_type);
          fprintf(verif_log, "H263_DQUANT = 0x%x\n", h263_dquant);
        #endif
        if (EE_AUX_REGS[SLICE_TYPE] == EE_I_SLICE) {
            if(dquant) cbpc+=4;
            #ifdef EE_DUMP
              fprintf(verif_log, "\nEE_I_SLICE: WRITING TO WriteFixedLengthCode WITH MCBPC VALUE = 0x%X AND LENGTH = %d\n", intra_MCBPC_code[cbpc], intra_MCBPC_bits[cbpc]);
            #endif
            WriteFixedLengthCode(
                intra_MCBPC_code[cbpc],
                intra_MCBPC_bits[cbpc]);
        } else {
            if(dquant) cbpc+=8;
            #ifdef EE_DUMP
              fprintf(verif_log, "\nEE_P_SLICE: WRITING TO WriteFixedLengthCode WITH NOT_CODED VALUE = 0x%X AND LENGTH = %d\n", 0, 1);
              fprintf(verif_log, "\nEE_P_SLICE: WRITING TO WriteFixedLengthCode WITH MCBPC VALUE = 0x%X AND LENGTH = %d\n", intra_MCBPC_code[cbpc], intra_MCBPC_bits[cbpc]);
              sample_h263_not_coded = true;
              h263_not_coded = 0;
            #endif
            WriteFixedLengthCode( 0, 1);	/* mb coded */
            WriteFixedLengthCode(
                inter_MCBPC_code[cbpc + 4],
                inter_MCBPC_bits[cbpc + 4]);
        }
        if ((EE_AUX_REGS[CODEC_CFG]>>4) & 1) { // Annexe I (Advanced Intra Coding)
            /* XXX: currently, we do not try to use ac prediction */
            WriteFixedLengthCode( 0, 1);	/* no AC prediction */
        }
        cbpy = cbp >> 2;
        #ifdef EE_DUMP
          sample_h263_cbpy = true;
          h263_cbpy = cbpy;
          fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH CBPY VALUE = 0x%X AND LENGTH = %d\n", cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
        #endif
        WriteFixedLengthCode(cbpy_tab[cbpy][0], cbpy_tab[cbpy][1]);
        if(dquant)
		{
			if(!((EE_AUX_REGS[CODEC_CFG]>>5) & 1)){
                #ifdef EE_DUMP
                  fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", dquant_code[dquant+2], 2);
                #endif
                WriteFixedLengthCode(dquant_code[dquant+2], 2);
			}
			else // modified quant 
			{	// quant - dquant is previous quant
				if(modified_quant[0][MpcCurrentPtr->QuantValue - dquant] == dquant)
                {  
                   #ifdef EE_DUMP
                     fprintf(verif_log, "\nif WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", 2, 2);
                    #endif
					WriteFixedLengthCode( 2, 2);
                } else if(modified_quant[1][MpcCurrentPtr->QuantValue - dquant] == dquant)
                {  
                   #ifdef EE_DUMP
                     fprintf(verif_log, "\nelse if WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", 3, 2);
                    #endif
					WriteFixedLengthCode(3, 2);
                } else 
                {
                   #ifdef EE_DUMP
                     fprintf(verif_log, "\nelse WRITING TO WriteFixedLengthCode WITH DQUANT VALUE = 0x%X AND LENGTH = %d\n", MpcCurrentPtr->QuantValue, 6);
                    #endif
                    WriteFixedLengthCode(MpcCurrentPtr->QuantValue, 6);
                };
			}
		}


    }

    /* encode each block */
    // Y blocks
    for (i = 0; i < 4; i++) {
        mpcBufOffset = (i/2)*PCB_VLC_TRANSFORM_Y_STRIDE*8 + (i%2)*(PCB_VLC_TRANSFORM_Y_STRIDE/2);
        ArcLoad8x8Block(block[i], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[mpcBufOffset])), PCB_VLC_TRANSFORM_Y_STRIDE/2);
        #ifdef EE_DUMP
          fprintf(verif_log, "\nCALLING ArcEncodeBlockH263 WITH i = %d\n", i);
        #endif
        ArcEncodeBlockH263(block[i], i);
    }
    // U block
    ArcLoad8x8Block(block[4], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_U])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
    ArcEncodeBlockH263(block[4], 4);
    // V block
    ArcLoad8x8Block(block[5], reinterpret_cast<short *>(&(MpcCurrentPtr->PixelCoeffBuffer[PCB_VLC_TRANSFORM_V])), PCB_VLC_TRANSFORM_UV_STRIDE/2);
    ArcEncodeBlockH263(block[5], 5);

}

#define INT_BIT 32
/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::ArcEncodeMotion                         */
/* DESCRIPTION:	VLC encodes a motion vector component.                          */
/*              Used in both MPEG4 and H.263                                    */
/* PARAMETERS:  val: the X or Y component of the MV;                            */
/*              f_code: indicates range in which the components lie             */
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::ArcEncodeMotion(int val,int f_code )
{
    int range, l, bit_size, sign, code, bits;
    
    if (val == 0) {
        /* zero vector */
        code = 0;
        WriteFixedLengthCode(mvtab[code][0], mvtab[code][1]);
    } else {
        bit_size = f_code - 1;
        range = 1 << bit_size;
        /* modulo encoding */
        l= INT_BIT - 6 - bit_size;
        val = (val<<l)>>l;
        sign = val>>31;
        val= (val^sign)-sign;
        sign&=1;

        val--;
        code = (val >> bit_size) + 1;
        bits = val & (range - 1);
        #ifdef EE_DUMP
          fprintf(verif_log, "WRITING TO WriteFixedLengthCode WITH MOTION VECTOR VALUE = 0x%X AND LENGTH = %d\n",(mvtab[code][0] << 1) | sign, mvtab[code][1] + 1);
        #endif
        WriteFixedLengthCode((mvtab[code][0] << 1) | sign, mvtab[code][1] + 1);
        if (bit_size > 0) {
            #ifdef EE_DUMP
              fprintf(verif_log, "WRITING TO WriteFixedLengthCode WITH MOTION VECTOR VALUE = 0x%X AND LENGTH = %d\n", bits, bit_size);
            #endif
            WriteFixedLengthCode(bits, bit_size);
        }
    }

}

/** Encode MV differences on H.263+ with Unrestricted MV mode 
 *  This is currently not used
 */
 void ArcEntropyEncoderModel::ArcEncodeMotionUMV(int val)
{
    short sval = 0;
    short i = 0;
    short n_bits = 0;
    short temp_val;
    int code = 0;
    int tcode;

    if ( val == 0)
        WriteFixedLengthCode( 1, 1);
    else if (val == 1)
        WriteFixedLengthCode( 0, 3);
    else if (val == -1)
        WriteFixedLengthCode( 2, 3);
    else {

        sval = ((val < 0) ? (short)(-val):(short)val);
        temp_val = sval;

        while (temp_val != 0) {
            temp_val = temp_val >> 1;
            n_bits++;
        }

        i = n_bits - 1;
        while (i > 0) {
            tcode = (sval & (1 << (i-1))) >> (i-1);
            tcode = (tcode << 1) | 1;
            code = (code << 2) | tcode;
            i--;
        }
        code = ((code << 1) | (val < 0)) << 1;
        WriteFixedLengthCode( code, (2*n_bits)+1 );
    }
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::ArcLoad8x8Block                         */
/* DESCRIPTION:	loads a 8x8 block from MPC data structure to a local block      */
/* PARAMETERS:  blk: the 8x8 block; buf: the MPC buffer; stride: MPC buf stride */
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::ArcLoad8x8Block(short *blk, short *buf, int stride)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		blk[0] = buf[0];
		blk[1] = buf[1];
		blk[2] = buf[2];
		blk[3] = buf[3];
		blk[4] = buf[4];
		blk[5] = buf[5];
		blk[6] = buf[6];
		blk[7] = buf[7];
		blk += 8;
		buf += stride;
	}
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::ArcEncodeBlockMP4                       */
/* DESCRIPTION:	encodes a 8x8 block as per MPEG4                                */
/* PARAMETERS:  block: the 8x8 block; n: block index (0-3: luma, 4-5: chroma)   */
/*              intra_dc: DC coeff; scan_table: the zigzag scan table           */
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::ArcEncodeBlockMP4(short * block, int n, int intra_dc,
                               unsigned char *scan_table)
{
#ifdef EE_DUMP
    fprintf(verif_log, "C++ ENTERED ArcEncodeBlockMP4 FUNCTION WITH intra_dc = %d\n", intra_dc);
#endif
    int i, last_non_zero;
	int level, save_run, save_level, first;

    unsigned int *bits_tab;
    unsigned char *len_tab;

    if (MpcCurrentPtr->MBCodingType == MBCT_MPEG4_INTRA) 
	{ 
		/* mpeg4 based DC predictor */
		EncodeDcMP4(intra_dc, n);

		i = 1;
        bits_tab= uni_mpeg4_intra_rl_bits;
        len_tab = uni_mpeg4_intra_rl_len;
    } 
	else 
	{
		i = 0;
        bits_tab= uni_mpeg4_inter_rl_bits;
        len_tab = uni_mpeg4_inter_rl_len;
    }
#ifdef EE_DUMP
    if (!(cbp & (1 << (5-n)))) {
      fprintf(verif_log, "C++ !(cbp & (1 << (5-n))) IS TRUE THEREFORE RETURNING FROM ArcEncodeBlockMP4 FUNCTION\n");
    };
#endif
    
    if(!(cbp & (1 << (5-n))))return;
    /* AC coefs */
    last_non_zero = i - 1;
    
    #ifdef EE_DUMP
      fprintf(verif_log, "\nAC COEFFICIENT ENCODING STARTING FOR SUB BLOCK %d\n", n);
    #endif

	first = true;
    save_level = 0;
    for (; i < 64; i++) 
	{
		level = block[ scan_table[i] ];
        #ifdef EE_DUMP
          fprintf(verif_log, "AC COEFFICIENT %d HAS LEVEL = %d\n", i, level);
        #endif
        if (level) 
		{
			if(!first)
			{
				
				save_level+=64;
				if((save_level&(~127)) == 0){
					const int index= UNI_MPEG4_ENC_INDEX(0, save_run, save_level);
                    #ifdef EE_DUMP
                      sample_mpeg4_ac = true;
                      mpeg4_ac_run_array[(n*64)+(i-1)] = save_run;
                      mpeg4_ac_level_array[(n*64)+(i-1)] = (save_level-64);
                      mpeg4_ac_last_array[(n*64)+(i-1)] = 0;
                      mpeg4_ac_mb_type = MpcCurrentPtr->MBCodingType;
                      
                      fprintf(verif_log, "\nSAVE_LEVEL = %d\n", save_level);
                      fprintf(verif_log, "SAVE_RUN = %d\n", save_run);
                      fprintf(verif_log, "1: WRITING TO WriteFixedLengthCode WITH VALUE = 0x%X AND LENGTH = %d\n", bits_tab[index], len_tab[index]);
                    #endif
                    WriteFixedLengthCode(bits_tab[index], len_tab[index]);
				}
				else
				{ //ESC3
                    #ifdef EE_DUMP
                      sample_mpeg4_ac = true;
                      mpeg4_ac_run_array[(n*64)+(i-1)] = save_run;
                      mpeg4_ac_level_array[(n*64)+(i-1)] = (save_level-64);
                      mpeg4_ac_last_array[(n*64)+(i-1)] = 0;
                      mpeg4_ac_mb_type = MpcCurrentPtr->MBCodingType;
                      
                      fprintf(verif_log, "\nSAVE_LEVEL = %d\n", save_level);
                      fprintf(verif_log, "SAVE_RUN = %d\n", save_run);
                      fprintf(verif_log, "2: WRITING TO WriteFixedLengthCode WITH VALUE = 0x%X AND LENGTH = %d\n", (3<<23)+(3<<21)+(0<<20)+(save_run<<14)+(1<<13)+(((save_level-64)&0xfff)<<1)+1, 7+2+1+6+1+12+1);
                    #endif
					WriteFixedLengthCode( (3<<23)+(3<<21)+(0<<20)+(save_run<<14)+(1<<13)+(((save_level-64)&0xfff)<<1)+1, 7+2+1+6+1+12+1); // 30 bit code added
				}
			}// !first
			first = false;
			save_run = i - last_non_zero -1;
			save_level = level;
			last_non_zero = i;
		}
    }

    if (save_level) 
    {
      save_level+=64;
      if((save_level&(~127)) == 0)
  	  {
          const int index= UNI_MPEG4_ENC_INDEX(1, save_run, save_level);
          #ifdef EE_DUMP
            sample_mpeg4_ac = true;
            mpeg4_ac_run_array[(n*64)+(i-1)] = save_run;
            mpeg4_ac_level_array[(n*64)+(i-1)] = (save_level-64);
            mpeg4_ac_last_array[(n*64)+(i-1)] = 1;
            mpeg4_ac_mb_type = MpcCurrentPtr->MBCodingType;
            
            fprintf(verif_log, "\nSAVE_LEVEL = %d\n", save_level);
            fprintf(verif_log, "SAVE_RUN = %d\n", save_run);
            fprintf(verif_log, "3: WRITING TO WriteFixedLengthCode WITH VALUE = 0x%X AND LENGTH = %d\n", bits_tab[index], len_tab[index]);
          #endif
          WriteFixedLengthCode(bits_tab[index], len_tab[index]);
      }else
  	  { //ESC3
          #ifdef EE_DUMP
            sample_mpeg4_ac = true;
            mpeg4_ac_run_array[(n*64)+(i-1)] = save_run;
            mpeg4_ac_level_array[(n*64)+(i-1)] = (save_level-64);
            mpeg4_ac_last_array[(n*64)+(i-1)] = 1;
            mpeg4_ac_mb_type = MpcCurrentPtr->MBCodingType;
            
            fprintf(verif_log, "\nSAVE_LEVEL = %d\n", save_level);
            fprintf(verif_log, "SAVE_RUN = %d\n", save_run);
            fprintf(verif_log, "\n4: WRITING TO WriteFixedLengthCode WITH VALUE = 0x%X AND LENGTH = %d\n",  (3<<23)+(3<<21)+(1<<20)+(	save_run<<14)+(1<<13)+(((save_level-64)&0xfff)<<1)+1, 7+2+1+6+1+12+1 );
          #endif
          WriteFixedLengthCode( (3<<23)+(3<<21)+(1<<20)+(	save_run<<14)+(1<<13)+(((save_level-64)&0xfff)<<1)+1, 7+2+1+6+1+12+1 );
      }
    }
#ifdef EE_DUMP
    fprintf(verif_log, "C++ EXITING ArcEncodeBlockMP4 FUNCTION\n");
#endif
    
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::EncodeDcMP4                             */
/* DESCRIPTION:	encodes DC coeff for MPEG4                                      */
/* PARAMETERS:  level: the DC value; n: block index (0-3: luma, 4-5: chroma)    */
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::EncodeDcMP4( int level, int n)
{
#ifdef EE_DUMP
    fprintf(verif_log, "C++ ENTERED EncodeDcMP4 FUNCTION WITH n = %d AND level = %d\n", n, level);
    sample_mpeg4_intra_dc_level = true;
    mpeg4_intra_dc_level_array[n] = level;
#endif
    level+=256;
    if (n < 4) {
   	/* luminance */
#ifdef EE_DUMP
       fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH LUMA DC VALUE = 0x%X AND LENGTH = %d\n", uni_DCtab_lum_bits[level], uni_DCtab_lum_len[level]);
#endif
	   WriteFixedLengthCode( uni_DCtab_lum_bits[level], uni_DCtab_lum_len[level] );
    } else {
#ifdef EE_DUMP
       fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH CHROMA DC VALUE = 0x%X AND LENGTH = %d\n", uni_DCtab_chrom_bits[level], uni_DCtab_chrom_len[level]);
#endif
	   /* chrominance */
	   WriteFixedLengthCode( uni_DCtab_chrom_bits[level], uni_DCtab_chrom_len[level] );
    }
#ifdef EE_DUMP
    fprintf(verif_log, "C++ EXITING EncodeDcMP4 FUNCTION\n");
#endif
}

/* ---------------------------------------------------------------------------- */
/* FUNCTION:	ArcEntropyEncoderModel::ArcEncodeBlockH263                      */
/* DESCRIPTION:	encodes a 8x8 block as per H.263                                */
/* PARAMETERS:  block: the 8x8 block; n: block index (0-3: luma, 4-5: chroma)   */
/* RETURNS:		void.															*/
/* ---------------------------------------------------------------------------- */
void ArcEntropyEncoderModel::ArcEncodeBlockH263(short * block, int n)
{
#ifdef EE_DUMP
    fprintf(verif_log, "C++ ENTERED ArcEncodeBlockH263 FUNCTION FOR 8x8 BLOCK %d\n", n);
#endif
	int level,  i, j,  last_non_zero, sign, slevel, code;
	int save_run, save_level;
    RLTable *rl;
	bool first;

    rl = &rl_inter;
    if (MpcCurrentPtr->MBCodingType == MBCT_MPEG4_INTRA && 
        !((EE_AUX_REGS[CODEC_CFG]>>4) & 1)) {
        /* DC coef */
        level = block[0];
        #ifdef EE_DUMP
          fprintf(verif_log, "\nDC COEFFICIENT : LEVEL = 0X%02X\n", level);
          sample_h263_intra_dc_level = true;
	      h263_intra_dc_level_array[n] = level;
	      //fprintf(verif_log, "h263_intra_dc_level_array[%d] = 0x%04X\n", n, (h263_intra_dc_level_array[n]&0xFFFF));
        #endif
//        /* 255 cannot be represented, so we clamp */
//       if (level > 254) {
//           level = 254;
//           block[0] = 254;
//       }
//        /* 0 cannot be represented also */
//        else if (level < 1) {
//            level = 1;
//            block[0] = 1;
//        }
        if (level == 128) 
            WriteFixedLengthCode(0xff, 8);
        else
            WriteFixedLengthCode(level, 8);
        i = 1;
    } else { // Advanced Intra Coding mode on (Annexe I)
        i = 0;
        if ( ((EE_AUX_REGS[CODEC_CFG]>>4) & 1) && 
            MpcCurrentPtr->MBCodingType == MBCT_MPEG4_INTRA )
            rl = &rl_intra_aic;
    }

    /* AC coefs */
    #ifdef EE_DUMP
      fprintf(verif_log, "\nAC COEFFICIENTS\n");
    #endif
	if(cbp & (1 << (5-n))) // if we have coeffs
	{
		last_non_zero = i - 1;
		first = true;
        save_level = 0;
		for (; i < 64; i++) {
			j = ff_zigzag_direct[i];
			level = block[j];
			if (level) 
			{
				if(!first)
				{			
					sign = 0;
					slevel = save_level;
					if (save_level < 0) 
					{
						sign = 1;
						save_level = -save_level;
					}
					code = get_rl_index(rl, 0, save_run, save_level);
                    #ifdef EE_DUMP
                      if (i == 0) {
                        fprintf(verif_log, "\nDC COEFFICIENT : LEVEL = 0X%02X\n", level);
                      }
                      else {
                        fprintf(verif_log, "\nAC COEFFICIENT : LEVEL = 0X%02X\n", level);
                      } 
                      sample_h263_ac = true;
                      h263_ac_run_array[(n*64)+(i-1)] = save_run;
                      h263_ac_level_array[(n*64)+(i-1)] = slevel;
                      h263_ac_last_array[(n*64)+(i-1)] = 0;
                      //fprintf(verif_log, "h263_ac_run_array[%d] = 0x%X\n", (n*64)+(i-1), h263_ac_run_array[(n*64)+(i-1)]);
                      //fprintf(verif_log, "ZIGZAG [%d]: h263_ac_level_array[%d] = 0x%X\n", ff_zigzag_direct[i-1], (n*64)+(i-1), h263_ac_level_array[(n*64)+(i-1)]);
                      //fprintf(verif_log, "h263_ac_last_array[%d] = 0x%X\n", (n*64)+(i-1), h263_ac_last_array[(n*64)+(i-1)]);
                      if (rl->table_vlc[code][0] == 0x03) {
                        fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH ESCAPE CODE VALUE = 0x%X AND LENGTH = %d\n", rl->table_vlc[code][0], rl->table_vlc[code][1]);
                      }
                      else {
                        fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH VLC VALUE = 0x%X AND LENGTH = %d\n", rl->table_vlc[code][0], rl->table_vlc[code][1]);
                      }
					#endif
					WriteFixedLengthCode(rl->table_vlc[code][0], rl->table_vlc[code][1]);
					if (code == rl->n)
					{
						ArcPutEscapeH263(0,save_run,slevel);
					} else 
					{
#ifdef EE_DUMP
                        fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH SIGN VALUE = 0x%X AND LENGTH = %d\n", rl->table_vlc[code][0], rl->table_vlc[code][1]);
#endif
						WriteFixedLengthCode(sign, 1);
					}
				}
				first = false;
				save_run = i - last_non_zero - 1;
				save_level = level;

				last_non_zero = i;
			}// level not zero
		}// i to 64

      if (save_level) 
      {

	      sign = 0;
		  slevel = save_level;
		  if (save_level < 0) 
	      {
			sign = 1;
			save_level = -save_level;
		  }
		  code = get_rl_index(rl, 1, save_run, save_level);
          #ifdef EE_DUMP
            sample_h263_ac = true;
            h263_ac_run_array[(n*64)+63] = save_run;
            h263_ac_level_array[(n*64)+63] = slevel;
            h263_ac_last_array[(n*64)+63] = 1;
          
            fprintf(verif_log, "h263_ac_run_array[%d] = 0x%X\n", (n*64)+63, h263_ac_run_array[(n*64)+63]);
            fprintf(verif_log, "ZIGZAG [%d]: h263_ac_level_array[%d] = 0x%X\n", ff_zigzag_direct[i-1], (n*64)+63, h263_ac_level_array[(n*64)+63]);
            fprintf(verif_log, "h263_ac_last_array[%d] = 0x%X\n", (n*64)+63, h263_ac_last_array[(n*64)+63]);
            fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH CODE VALUE = 0x%X AND LENGTH = %d\n", rl->table_vlc[code][0], rl->table_vlc[code][1]);
		  #endif
		  WriteFixedLengthCode(rl->table_vlc[code][0], rl->table_vlc[code][1]);
		  if (code == rl->n)
		  {
			 ArcPutEscapeH263(1,save_run,slevel);
		  } else 
		  {
			 WriteFixedLengthCode(sign, 1);
		  }
      }
	}// if coeffs
#ifdef EE_DUMP
    fprintf(verif_log, "C++ EXITING ArcEncodeBlockH263 FUNCTION\n");
#endif
}

void ArcEntropyEncoderModel::ArcPutEscapeH263(int last, int run, int level)
{      
     #ifdef EE_DUMP
       fprintf(verif_log, "\nESCAPE CODE WRITTEN: FOLLOWING VALUES SHOULD NOW BE ENCODED: RUN = %d, LEVEL = 0x%02X, LAST = %d\n", run, level, last);
     #endif

	 int slevel = level;
	  
	 if (level < 0)
	 {
		  level = -level;
	 }


    #ifdef EE_DUMP
	  fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH LAST VALUE = %d AND LENGTH = %d\n", last, 1);
    #endif
	WriteFixedLengthCode(last, 1);
    #ifdef EE_DUMP
	  fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH RUN VALUE = %d AND LENGTH = %d\n", run, 1);
    #endif
	WriteFixedLengthCode(run, 6);

	assert(slevel != 0);

    #ifdef EE_DUMP
	  //fprintf(verif_log, "\nslevel = 0x%08X\n", slevel);
	  //fprintf(verif_log, "level = 0x%08X\n", level);
	  fprintf(verif_log, "\nWRITING TO WriteFixedLengthCode WITH LEVEL VALUE = 0x%02X AND LENGTH = %d\n", slevel, 8);
    #endif
	if(level < 128)
		WriteFixedLengthCode(slevel & 0xff, 8);
	else
	{ // extended escape for Annex T
		WriteFixedLengthCode(128, 8);
		WriteFixedLengthCode(slevel & 0x1f, 5);
		WriteFixedLengthCode((slevel>>5)&0x3f, 6);
	}

}


#endif // HARDWARE_EE
