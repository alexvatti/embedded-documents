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



#ifndef ARC_FRAME_BUFFERS_H
#define ARC_FRAME_BUFFERS_H

#include <stdio.h>

#include "typdef.h"
//NB these two definition are also in ArcSDMTables.h and ArcMP4SDMTables.h
#define 											SDMOF_FrameTabDMA									0x0000

#define CHROMA_WORKING_ARRAY_STRIDE 16
#define ME_REFERENCE_ARRAY_STRIDE	32


// setup dma frame tables for current, ref1 and ref2, for luma and chroma
// i.e. 9 frame tables in all, each frame table occupies 16 bytes
#define FRAME_TABLE_CURR_INDEX 0
#define FRAME_TABLE_REF1_INDEX 1
#define FRAME_TABLE_REF2_INDEX 2

#define FRAME_TABLE_Y_INDEX 0
#define FRAME_TABLE_U_INDEX 3
#define FRAME_TABLE_V_INDEX 6

#define FRAME_TABLE_Y_CURR_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_Y_INDEX + FRAME_TABLE_CURR_INDEX))) // 0 (0)
#define FRAME_TABLE_Y_REF1_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX))) // 2 (16)
#define FRAME_TABLE_Y_REF2_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX))) // 3 (32)

#define FRAME_TABLE_U_CURR_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_U_INDEX + FRAME_TABLE_CURR_INDEX))) // 4 (48)
#define FRAME_TABLE_U_REF1_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX))) // 5 (64)
#define FRAME_TABLE_U_REF2_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX))) // 6 (80)

#define FRAME_TABLE_V_CURR_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_V_INDEX + FRAME_TABLE_CURR_INDEX))) // 7 (96)
#define FRAME_TABLE_V_REF1_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX))) // 8 (102)
#define FRAME_TABLE_V_REF2_ADDR (SDMOF_FrameTabDMA + (16 * (FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX))) // 9 (128)




class ArcFrameBuffers
{
    public:

        // constructor and deconstructor
        ArcFrameBuffers();
        ~ArcFrameBuffers();


        // public functions

        // buffer management functions
        bool SetSize(int xSize, int ySize);
	bool SetInputYUV(unsigned char* Y_base, unsigned char* U_base, unsigned char* V_base,
			 unsigned char* Y_phys=0, unsigned char* U_phys=0,unsigned char* V_phys=0);

        // Delete all the allocated buffers
        void DeleteInternalPictureBuffs(void);

#ifndef ARC_ENCODER_API
	bool OpenSourceFile(char* basename);
        // Read input YUV from file into internal buffers
        int ReadInputPictureFromFile(void);
        void CloseFile(void);
#endif

        void GetOutputPic( unsigned char **y, unsigned char **u, unsigned char **v);

        // Fetch next input macroblock luma ( for extern processing of INTRA etc)
        // Also updates InputLumaMB
        void GetInputLumaBlock(unsigned char * inputBuffer, int x, int y, int w, int h);


        // public variables
        unsigned char * InputY;     // Input luma from file
        unsigned char * InputU;		// Input luma from file
        unsigned char * InputV;		// Input luma from file

        int YUVPos;                 // Indicates which buffer is being used

        unsigned char * OutputRefY;	//	Reference frame under construction luma
        unsigned char * OutputRefU;	//	Reference frame under construction chroma U
        unsigned char * OutputRefV;	//	Reference frame under construction chroma V

        unsigned char * OutputY;	//	Reference frame under construction luma
        unsigned char * OutputU;	//	Reference frame under construction chroma U
        unsigned char * OutputV;	//	Reference frame under construction chroma V

//#ifndef ENABLE_PERFORMANCE
        unsigned int * InputBuffer;
        unsigned int * InputBufferAllocated;
//#endif
        unsigned int * OutputRefBuffer;
        unsigned int * OutputBuffer;
        unsigned int * OutputRefBufferAllocated;
        unsigned int * OutputBufferAllocated;

	// Physical addresses of all buffers
        unsigned char * PhysIPY;
        unsigned char * PhysIPU;
        unsigned char * PhysIPV;

        unsigned char * PhysOPRefY;
        unsigned char * PhysOPRefU;
        unsigned char * PhysOPRefV;

        unsigned char * PhysOPY;
        unsigned char * PhysOPU;
        unsigned char * PhysOPV;

    private:

        // private functions


        // private variables

        // Internal data set at start of sequence or frame ------------------------------

        int YWidth;				// Width of inpout & output ref buffers
        int YHeight;			// Height ..

        int UVWidth;               // U/V width
        int UVHeight;               // U/V height

        unsigned int FrameSize;


#ifdef ARC_ENCODER_API
	unsigned long  OPTag;
	unsigned long  OPRefTag;
#else
        FILE *YUVFile;
#endif
};


extern ArcFrameBuffers gArcFrameBuffers;



#endif // ARC_FRAME_BUFFERS_H
