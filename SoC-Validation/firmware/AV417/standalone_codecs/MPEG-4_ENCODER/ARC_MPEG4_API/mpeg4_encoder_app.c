
/*                                                         -*-C-*- *
 * Moschip Semiconductor Technology Limited                                                                *
 * mpeg4_encoder_app.c                                              *
 *                                                                 *
 * ARC MPEG4 Encoder API implementation.                            *
 *                                                                 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arc_enc_utils.h>
#include <arc_encoder.h>
#include <arc_encoder_malloc.h>
#include <mpeg4_interface.h>
#include <standalone_support.h>
#include "serial_pl01x.h"

MPEG4SetupParams setupParams;
extern int HwMe;
#define   MAX_DECODED_FRAMES 5 
#define   HW_ME_AVAILABLE HwMe
#define   METAWARE_DEBUGGER 1
#undef    METAWARE_DEBUGGER
#undef  CONFIG_SHIKHARA_FPGA
#ifdef    CONFIG_SHIKHARA_FPGA 
#define   INPUT_FILE_PTR    0x41500000
#define   OUTPUT_FILE_PTR   0x41B00000
#else
#define   INPUT_FILE_PTR    0x01500000
#define   OUTPUT_FILE_PTR   0x01B00000
#endif

#define   INPUT_FILE_SIZE   1024*5534
#define   OUTPUT_FILE_SIZE  1024*120

int main (int argc, char **argv)
{

	serial_init();

	int i;
#ifdef METAWARE_DEBUGGER
	int infd = 0;
	int outfd = 0;
#endif
	print("Entered Into Encoding\n");
	StreamDescriptor strDesc;
	MPEG4EncodingParams encParams;
	RawFrame inputFrame;
	char *InputData;
	char *outputData;
	char *pointer_outputData;
	RawFrame *inputArray[2];
	ArgFundamental fund;
	EncPacket* outputArray[MAX_OUTPUT_PACKETS];
	int frames = 0;
	int size,total_size=0;
	int output_size=0;
	RawFrame decoded[MAX_DECODED_FRAMES];

	print("calling init_arc_encoder_api\n");
	init_arc_encoder_api();

	/* Initialise params and get setup */
	memset (&encParams, 0, sizeof (MPEG4EncodingParams));
	memset (&setupParams, 0, sizeof (MPEG4SetupParams));
	SETUP_ENCARGUMENT (encParams, ENC_ARGTYPE_MPEG4ENCPARAMS,
			(sizeof (MPEG4EncodingParams)))
	SETUP_ENCARGUMENT (setupParams, ENC_ARGTYPE_MPEG4SETUP,
			(sizeof (MPEG4SetupParams)))

	/* first setup some params to unknown so we can check later that they've been set */
	setupParams.fixed_quantiser_value = -1;
	setupParams.target_bitrate = -1;
	setupParams.quality_level = -1;
	setupParams.vui_parameters = malloc(sizeof(h264_vui_parameters));
	memset(setupParams.vui_parameters,0,sizeof(h264_vui_parameters));
	encParams.cumulative_stats = 0;


	/* Setup Parameters */

	setupParams.coding_standard = 1 ;

	setupParams.frame_width= 176;             /* Number of pixels in the luminance in the input YUV file */

	setupParams.frame_height= 144;            /* Number of luminance lines present in the input YUV data */

	setupParams.fixed_quantiser=false;        /* If this bit is true, then the bitrate is variable otherwise the bitrate                                                      is constant */

	setupParams.target_bitrate= 128000;       /* If a fixed quantizer is not being used, then this is the constant 
                                                     bit-rate that will be attempted. If fixed_quantiser is 1, this value is                                                      disregarded */

	setupParams.intra_picture_frequency= 0;   /* Frequency at which intra frames are transmitted. A value of 0 means
						     that only the first frame is Intra */

	setupParams.code_all_pictures= true;      /* Binary parameter indicating that all input pictures should be coded */

	setupParams.target_picture_period= 4500;  /* This value is the number of units assigned to one target frame 
						     for 9000 the target framerate is 9000/90000sec = 10
						     for 3000 the target framerate is 3000/90000sec =30 */

	setupParams.input_picture_period= 3000;   /* This value is the number of units assigned to one input frame */



	/* perform final syntax checks */
	if (setupParams.fixed_quantiser)
	{
		setupParams.target_bitrate = 0;

		if (setupParams.fixed_quantiser_value == -1)
		{
			print("Error - Fixed Quantiser Value not setup in config file");
		}
	}
	else
	{
		setupParams.fixed_quantiser_value = 0;

		if (setupParams.target_bitrate == -1)
		{
			print("Error - Target Bitrate not setup in config file\n");

		}
	}
	print("Initializing Encoder strean desc Parameters\n");

	/* Set up stream descriptor */
	strDesc.width = setupParams.frame_width;
	strDesc.height = setupParams.frame_height;
	strDesc.frames = 0;
	strDesc.current_time = 0;
	strDesc.current_frame = 0;
	strDesc.bitrate = 0;
	strDesc.interlaced = 0;
#ifdef METAWARE_DEBUGGER
	if ((infd = open ("akiyo_qcif.yuv" , O_RDONLY)) == -1)
	{
		print("Error opening input file, giving up\n");
		return -1;
	}
#endif
	/* Set up input frame memory */
	inputFrame.data.Y_base = arc_encoder_api_malloc(((setupParams.frame_width * setupParams.frame_height * 3)/2),
			(ENC_MEM_INPUTYUV|ENC_MEM_FB_YUV|ENC_MEM_ALIGN_8|ENC_MEM_DC_PAD),
			&inputFrame.data.Y_phys, 0);

	inputFrame.data.U_base = inputFrame.data.Y_base + setupParams.frame_width * setupParams.frame_height;
	inputFrame.data.V_base = inputFrame.data.U_base + (setupParams.frame_width * setupParams.frame_height)/4;
	inputFrame.data.U_phys = inputFrame.data.Y_phys + setupParams.frame_width * setupParams.frame_height;
	inputFrame.data.V_phys = inputFrame.data.U_phys + (setupParams.frame_width * setupParams.frame_height)/4;

	if ((! inputFrame.data.Y_base) || (! inputFrame.data.U_base) || (! inputFrame.data.V_base)) 
	{
		print("Error allocating input data structure, giving up\n");
		return -1;
	}

	inputFrame.info.strDesc = &strDesc;

#ifdef METAWARE_DEBUGGER
	if ((outfd = open ("akiyo_qcif.yuv.m4v", (O_RDWR | O_CREAT | O_TRUNC),
					(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))) == -1)
	{
		print("error\n");
		return -1;
	}
#endif
	print("Initializing Encoder\n");
	/* initialise encoder */
	if (mpeg4_enc_init (1,		/* handle */
				ENC_TYPE_MPEG4,	/* coding_type */
				&strDesc,	        /* stream info */
				&(ENC_ARG (setupParams))) != ENC_ERR_NONE)	/* Encoder-specific extra data - setup params */
	{
		print("Error initialising encoder, going no further\n");
		return -1;
	}
	outputData= (char *)malloc(OUTPUT_FILE_SIZE);
	InputData =(char *)malloc(INPUT_FILE_SIZE);
#ifdef METAWARE_DEBUGGER
	read(infd,InputData,INPUT_FILE_SIZE);
#else
	print("Copying Input File\n");
	memcpy(InputData,(char *)INPUT_FILE_PTR,INPUT_FILE_SIZE);
#endif
	encParams.decoded_frames = decoded;
	encParams.max_decoded_frames = MAX_DECODED_FRAMES;
	size = (setupParams.frame_width * setupParams.frame_height * 3) / 2;
	pointer_outputData=outputData;
	print("Encoding Started\n");
	while (1)
	{
		u32 result;
		u32 status = 0;
		u32 numin;
		u32 numout = MAX_OUTPUT_PACKETS;
		int i;
		if(total_size>5534*1024)
			break;
		total_size=total_size+size; 
		memcpy((char *) (inputFrame.data.Y_base),InputData,size);
		InputData = InputData+size;
		inputFrame.info.frame_num = frames++;
		inputArray[0] = &inputFrame;
		inputArray[1] = 0;
		numin = 1;
		/*print("Encoding:\n");*/
		if ((result = mpeg4_enc_encode (1,	                    /* handle */
						inputArray,	            /* input data */
						&numin,	                    /* No of input frames available (in) / consumed                                                                                 (out) */
						outputArray,	            /* output destination */
						&numout,	            /* no of output packets available (in) / produced                                                                               (out) */
						&status,	            /* flags/status */
						&(ENC_ARG (encParams))))    /* Encoder-specific extra data */
				!= ENC_ERR_NONE)
		{
			if (result != ENC_ERR_OUTOFOPS)
			{
				print("Fatal error during encoding\n");
				break;
			}
		}

		if (numout > 0)
		{
			for (i = 0; i < numout; i++)
			{
				EncPacket * packet=outputArray[i];
				memcpy(outputData,(char *)packet->data,packet->packet_len);
				outputData=outputData+packet->packet_len;
				output_size=output_size+packet->packet_len;
				/* Release output packet */
				packet->unused = true;
			}
		}
	}


#ifdef METAWARE_DEBUGGER
	write (outfd, (const char *) pointer_outputData,output_size);
#else
	memcpy((char *)OUTPUT_FILE_PTR,(const char *) pointer_outputData,output_size);
#endif
	arc_encoder_api_free(inputFrame.data.Y_base,
			(ENC_MEM_INPUTYUV|ENC_MEM_FB_YUV|ENC_MEM_ALIGN_8|ENC_MEM_DC_PAD),
			0);
	print("completed Encoding\n");

	mpeg4_enc_uninit (1);		 /* handle */
#ifdef METAWARE_DEBUGGER
	close (infd);
	close (outfd);
#endif

	uninit_arc_encoder_api();

	print("Finished ... bye!");
	return 1;
}

/*----------------------------------------------------------------------------------*/
