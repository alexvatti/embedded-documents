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
#include "ArcFrameBuffers.h"
#include <assert.h>

#ifdef ARC_ENCODER_API
#include <arc_encoder.h>
#include <arc_encoder_malloc.h>
#endif

#include "defines.h"
#include "VRaptorInfo.h"

#ifdef ENABLE_PERFORMANCE
	buff YUVBuffer;
	#include <stdio.h>
#endif
// global instance
ArcFrameBuffers gArcFrameBuffers;


//******************************************************************************
// Name:          ArcFrameBuffers::SetupDMAFrameTable
// Purpose:       Writes a frame table entry into the sdm
// Arguments:     frmTblAddr = address in the sdm to store the frame table
//                baseAddr = frame base address
//                stride = frame stride
//                xDim = frame x dimension
//                yDim = frame y dimension
// Return Value:  None
//******************************************************************************
_Asm ASM_SetupDMAFrameTable(u_int16 frmTblAddr, u_int32 baseAddr, u_int16 stride, u_int16 xDim, u_int16 yDim)
{
    %reg frmTblAddr, baseAddr, stride, xDim, yDim

    // base_addr [32:2] 30 bits
    vmovw       %vr00, baseAddr, 0x1
    lsr         baseAddr, baseAddr, 16
    vmovw       %vr00, baseAddr, 0x2

    // stride [47:34] 14 bits
    vmovw       %vr00, stride, 0x4

    // x_dim [75:64] 12 bits
    vmovw       %vr00, xDim, 0x10

    // y_dim [107:96] 12 bits
    vmovw       %vr00, yDim, 0x40

    vst128r     %vr00, [frmTblAddr]
}


//******************************************************************************
// Name:          ArcFrameBuffers::ArcFrameBuffers
// Purpose:       Constructor
// Arguments:     void
// Return Value:  None
//******************************************************************************
ArcFrameBuffers::ArcFrameBuffers ()
{
   InputY     = NULL; // Input luma from file
   InputU     = NULL;
   InputV     = NULL;

   OutputRefY = NULL;	    	
   OutputRefU = NULL;	    	
   OutputRefV = NULL;	    	
   OutputY    = NULL;			
   OutputU    = NULL;			
   OutputV    = NULL;
   OutputRefBufferAllocated = NULL;
   OutputBufferAllocated = NULL;
//#ifndef ENABLE_PERFORMANCE
   InputBufferAllocated = NULL;
//#endif

   PhysIPY    = NULL;
   PhysIPU    = NULL;
   PhysIPV    = NULL;
   PhysOPRefY = NULL;
   PhysOPRefU = NULL;
   PhysOPRefV = NULL;
   PhysOPY    = NULL;
   PhysOPU    = NULL;
   PhysOPV    = NULL;

   YWidth     = 0;
   YHeight    = 0;
   UVWidth    = 0;
   UVHeight   = 0;
   FrameSize  = 0;

#ifdef ARC_ENCODER_API
   OPTag = 0;
   OPRefTag = 0;
#else
   YUVFile    = NULL;
#endif

   YUVPos     = 1;
   #ifdef ENABLE_PERFORMANCE
	YUVBuffer.TimerTicks = (unsigned int*)TIMER_TICK_MEM;
	YUVBuffer.OutputPtr = 0;
   #ifdef ENABLE_BVCI_COUNTERS
	YUVBuffer.BVCICounts = (bvci_data *)BVCI_COUNT_MEM;
   #endif
   #endif
}


//******************************************************************************
// Name:          ArcFrameBuffers::~ArcFrameBuffers
// Purpose:       Destructor
// Arguments:     None
// Return Value:  None
//******************************************************************************
ArcFrameBuffers::~ArcFrameBuffers ()
{
   DeleteInternalPictureBuffs();
}


#ifndef ARC_ENCODER_API
//================================ FILE HANDLING =============================

// Open the source YUV file for encoding
bool ArcFrameBuffers::OpenSourceFile(char* basename)
{
    if (0 != YUVFile)
    {
	    return false;
    }
	
	char   filename[512];

	/* construct filename */
	sprintf(filename,"%s",basename);

	/* try to open file */
	YUVFile = fopen(filename,"rb");

	if (YUVFile == NULL)
	{
		return(false);
	}
	setvbuf(YUVFile,0,_IOFBF,32768);
	
	#ifdef ENABLE_PERFORMANCE
	    memset(YUVBuffer.TimerTicks,0,500*sizeof(unsigned int));
	#endif
		//fseek(YUVFile,0,SEEK_END);
		//YUVBuffer.FileSz = ftell(YUVFile);
		//if(YUVBuffer.FileSz > YUV_BUFF_SIZE)
		//{
			//printf("\nFile Size Exceeds the YUV Buffer size\n");
			//return false;
		//}
		//unsigned char *addr;
		/*switch ((reinterpret_cast<unsigned int> (YUVBuffer.InputBuffer)) & 0x7)  
	    {
	        case 0:
	        	addr = reinterpret_cast<unsigned char *> (YUVBuffer.InputBuffer);
	            break;
	        case 4:
	        	addr = reinterpret_cast<unsigned char *> (YUVBuffer.InputBuffer + 1);
	            break;
	        default:
	            fprintf(stderr, "ArcFrameBuffers: Error - InputBuffer memory alignment is not supported\n");
	            assert(false);
	            return(false);
	    }
		
		fseek(YUVFile,0,SEEK_SET);
		if(fread(addr,1,YUVBuffer.FileSz,YUVFile) != YUVBuffer.FileSz) 
			return false;
		fclose(YUVFile);
		printf("end of read file\n");
		fflush(stdout);
	#endif*/

	return true;
}


// Read input YUV from file into internal buffers
int ArcFrameBuffers::ReadInputPictureFromFile(void)
{
 //#ifndef ENABLE_PERFORMANCE
    if(!YUVFile) return 0;

    // Read one YUV frame of data
    if(fread(InputY,1,FrameSize,YUVFile) != FrameSize) return 0;
	flush_dcache();
	
//#else
    // Adjust the buffer pointers to start of next frame.
   // InputY = InputY + FrameSize;

   // InputU = InputY + (YWidth * YHeight);
   // InputV = InputU + (UVWidth * UVHeight);
	   
	// ASM_SetupDMAFrameTable(FRAME_TABLE_U_CURR_ADDR, reinterpret_cast<u_int32> (InputU), UVWidth, UVWidth, UVHeight);
	// ASM_SetupDMAFrameTable(FRAME_TABLE_V_CURR_ADDR, reinterpret_cast<u_int32> (InputV), UVWidth, UVWidth, UVHeight);

	//if((unsigned int)InputY > ((unsigned int)YUVBuffer.InputBuffer + (YUVBuffer.FileSz)))
    //	return 0;


//#endif

    return 1;
}

void ArcFrameBuffers::CloseFile(void)
{
	if(YUVFile) fclose(YUVFile);
}

//===============================End File handling==============================
#endif

void ArcFrameBuffers::GetOutputPic(unsigned char **y, unsigned char **u, unsigned char **v)
{
    if(YUVPos == 0)
    {
        *y = OutputY;
        *u = OutputU;
        *v = OutputV;
        YUVPos++;
    }
    else
    {
        *y = OutputRefY;
        *u = OutputRefU;
        *v = OutputRefV;
        YUVPos = 0;
    }
    
}


// Use to init internal buffers or change size if already created
// Size in pixels of input images
bool ArcFrameBuffers::SetSize(int xSize, int ySize)
{
    // If there are are already allocated buffers delete them and allocate with new sizes
    if(YWidth != xSize || YHeight != ySize)
    {
        DeleteInternalPictureBuffs();
    }

    YWidth  = xSize;				// Width of input buffer
    YHeight = ySize;			    // Height ..

	// MeBusConv:
	// if width is an odd number of macroblocks then make it even
	int refYWidth  = xSize;				// Width of output ref buffers
	if (HwMe == 1 && (xSize & 0x10)) refYWidth  += 16;	// Width of output ref buffers

    UVWidth  = YWidth/2;               // U/V width
    UVHeight = YHeight/2;

    FrameSize = (YWidth * YHeight *3)/2;
    int refFrameSize = (refYWidth * YHeight *3)/2;

#ifndef ARC_ENCODER_API
    //#ifndef ENABLE_PERFORMANCE
        // Allocate memory for the internal buffers
    	InputBufferAllocated = new unsigned int[(FrameSize/4)+15 + 128];
        InputBuffer = InputBufferAllocated + 64;
        if(!InputBufferAllocated) return false;
    //#else
    //unsigned int * InputBuffer = YUVBuffer.InputBuffer;
    //#endif

    OutputRefBufferAllocated = new unsigned int[(refFrameSize/4)+15 +128];
    OutputRefBuffer = OutputRefBufferAllocated + 64;
    if(!OutputRefBufferAllocated) return false;

    OutputBufferAllocated = new unsigned int[(refFrameSize/4)+15 +128];
    OutputBuffer = OutputBufferAllocated + 64;
    if(!OutputBufferAllocated) return false;

	// MeBusConv:
    // check to see if the buffers are 512 bit (64 bytes) aligned. if it's not
    // then we add a the appropriate offset
	unsigned int alignment512Bit;
	alignment512Bit = reinterpret_cast<unsigned int> (InputBuffer) & 0x3f;
	if (alignment512Bit == 0)
	{
		InputY = reinterpret_cast<unsigned char *> (InputBuffer);
	}
	else
	{
		InputY = reinterpret_cast<unsigned char *> (InputBuffer + 16 - (alignment512Bit >> 2));
	}

	alignment512Bit = reinterpret_cast<unsigned int> (OutputRefBuffer) & 0x3f;
	if (alignment512Bit == 0)
	{
		OutputRefY = reinterpret_cast<unsigned char *> (OutputRefBuffer);
	}
	else
	{
		OutputRefY = reinterpret_cast<unsigned char *> (OutputRefBuffer + 16 - (alignment512Bit >> 2));
	}

	alignment512Bit = reinterpret_cast<unsigned int> (OutputBuffer) & 0x3f;
	if (alignment512Bit == 0)
	{
		OutputY = reinterpret_cast<unsigned char *> (OutputBuffer);
	}
	else
	{
		OutputY = reinterpret_cast<unsigned char *> (OutputBuffer + 16 - (alignment512Bit >> 2));
	}

    PhysIPY = InputY;
    PhysOPY = OutputY;
    PhysOPRefY = OutputRefY;
#else
    // Input Y is set by the API on a frame-by-frame basis
    OutputRefY = arc_encoder_api_malloc(refFrameSize,ENC_MEM_RECONYUV|ENC_MEM_ALIGN_64|ENC_MEM_DC_PAD|ENC_MEM_DMA_TGT,&PhysOPRefY,&OPRefTag);
    if(!OutputRefY) return false;

    OutputY = arc_encoder_api_malloc(refFrameSize,ENC_MEM_RECONYUV|ENC_MEM_ALIGN_64|ENC_MEM_DC_PAD|ENC_MEM_DMA_TGT,&PhysOPY,&OPTag);
    if(!OutputY) return false;
#endif	


    InputU = InputY + (YWidth * YHeight);
    InputV = InputU + (UVWidth * UVHeight);

    // This will be without the borders
    // At any time one of these buffres will be used for decoding picture
    // and the other one will be for reference buffer

    OutputRefU = OutputRefY + (refYWidth * YHeight);
    OutputRefV = OutputRefU + (UVWidth * UVHeight);

    OutputU = OutputY + (refYWidth * YHeight);
    OutputV = OutputU + (UVWidth * UVHeight);

    PhysIPU = PhysIPY + (YWidth * YHeight);
    PhysIPV = PhysIPU + (UVWidth * UVHeight);

    PhysOPRefU = PhysOPRefY + (refYWidth * YHeight);
    PhysOPRefV = PhysOPRefU + (UVWidth * UVHeight);

    PhysOPU = PhysOPY + (refYWidth * YHeight);
    PhysOPV = PhysOPU + (UVWidth * UVHeight);

    // setup dma frame tables for current, ref1 and ref2, for luma and chroma

    u_int16 xDimLuma = YWidth - 1;
    u_int16 yDimLuma = YHeight - 1;
    u_int16 refXDimLuma = refYWidth - 1;

    u_int16 xDimChroma = UVWidth - 1;
    u_int16 yDimChroma = UVHeight - 1;

    ASM_SetupDMAFrameTable(FRAME_TABLE_Y_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPY), YWidth, xDimLuma, yDimLuma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_Y_REF1_ADDR, reinterpret_cast<u_int32> (PhysOPRefY), refYWidth, refXDimLuma, yDimLuma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_Y_REF2_ADDR, reinterpret_cast<u_int32> (PhysOPY), refYWidth, refXDimLuma, yDimLuma);

    ASM_SetupDMAFrameTable(FRAME_TABLE_U_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPU), UVWidth, xDimChroma, yDimChroma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_U_REF1_ADDR, reinterpret_cast<u_int32> (PhysOPRefU), UVWidth, xDimChroma, yDimChroma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_U_REF2_ADDR, reinterpret_cast<u_int32> (PhysOPU), UVWidth, xDimChroma, yDimChroma);

    ASM_SetupDMAFrameTable(FRAME_TABLE_V_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPV), UVWidth, xDimChroma, yDimChroma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_V_REF1_ADDR, reinterpret_cast<u_int32> (PhysOPRefV), UVWidth, xDimChroma, yDimChroma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_V_REF2_ADDR, reinterpret_cast<u_int32> (PhysOPV), UVWidth, xDimChroma, yDimChroma);

    return true;
}

// Use externally managed frame buffers. 
bool ArcFrameBuffers::SetInputYUV(unsigned char* Y_base,unsigned char* U_base,unsigned char* V_base, 
				  unsigned char* Y_phys,unsigned char* U_phys,unsigned char* V_phys)
{
  if ((!Y_base)||(!U_base)||(!V_base))
    return false;

  if (! Y_phys) Y_phys = Y_base;
  if (! U_phys) U_phys = U_base;
  if (! V_phys) V_phys = V_base;

  InputY = Y_base;
  InputU = U_base;
  InputV = V_base;

    u_int16 xDimLuma = YWidth - 1;
    u_int16 yDimLuma = YHeight - 1;

    u_int16 xDimChroma = UVWidth - 1;
    u_int16 yDimChroma = UVHeight - 1;

    PhysIPY = Y_phys;
    PhysIPU = U_phys;
    PhysIPV = V_phys;

    ASM_SetupDMAFrameTable(FRAME_TABLE_Y_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPY), YWidth, xDimLuma, yDimLuma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_U_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPU), UVWidth, xDimChroma, yDimChroma);
    ASM_SetupDMAFrameTable(FRAME_TABLE_V_CURR_ADDR, reinterpret_cast<u_int32> (PhysIPV), UVWidth, xDimChroma, yDimChroma);

  return true;
}

// Delete all the picture buffers
void ArcFrameBuffers::DeleteInternalPictureBuffs(void)
{
#ifdef ARC_ENCODER_API
    if (OutputRefY) arc_encoder_api_free(OutputRefY,ENC_MEM_RECONYUV,&OPRefTag);
    if (OutputY) arc_encoder_api_free(OutputY,ENC_MEM_RECONYUV,&OPTag);
    OutputRefY = NULL;
    OutputY = NULL;
#else
//#ifndef ENABLE_PERFORMANCE
  if(InputBufferAllocated) { delete [] InputBufferAllocated; InputBufferAllocated = NULL; }
//#endif
  if(OutputRefBufferAllocated) { delete [] OutputRefBufferAllocated; OutputRefBufferAllocated = NULL; }
  if(OutputBufferAllocated) { delete [] OutputBufferAllocated;OutputBufferAllocated = NULL; }
#endif
}
