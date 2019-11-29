
/* Moschip Semiconductor Technology Limited
 *
 *                                                                 
 * h264_encoder_app.c                                              
 *                                                                 
 * ARC H264 Encoder API implementation.                            
 *                                                                 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "serial_pl01x.h"

#include <arc_enc_utils.h>
#include <arc_encoder.h>
#include <arc_encoder_malloc.h>
#include <h264_interface.h>
#ifdef __ARC_LINUX__
#include <linux_support.h>
#endif
#include <standalone_support.h>
extern void print(const char *s);

#undef METAWARE_DEBUGGER
#undef CONFIG_SHIKHARA_FPGA 

#ifdef CONFIG_SHIKHARA_FPGA
#define INPUT_FILEPTR  0x41500000
#define OUTPUT_FILEPTR 0x41b00000
#else
#define INPUT_FILEPTR    0x01500000
#define OUTPUT_FILEPTR   0x01b00000
#endif

#define INPUT_FILE_SIZE 5532*1024
#define OUTPUT_FILE_SIZE 77824

unsigned char *inputbuffer=(unsigned char *)INPUT_FILEPTR;
unsigned char *outputbuffer=(unsigned char *)OUTPUT_FILEPTR;
unsigned char *actual_outputbuffer;
unsigned int final_size=0;
unsigned int total_size=0;
RawFrame * inframe;

#ifdef __ARC_LINUX__
#define STRCASECMP strcasecmp
#else
#define STRCASECMP _strcmpi
#endif

int verbose = 0;
int line_count = 0;
int total_written = 0;

H264SetupParams setupParams;
#define MAX_DECODED_FRAMES 5

#ifdef METAWARE_DEBUGGER
#define FILENAMELEN 300
char source_filename[FILENAMELEN];
char stats_filename[FILENAMELEN];
char bitstream_filename[FILENAMELEN];
#ifdef OUTPUT_DEBUG_INFO
char recon_filename[FILENAMELEN];
#endif // OUTPUT_DEBUG_INFO
#ifdef LOGGING
char log_filename[FILENAMELEN];
#endif

#ifndef OUTPUT_DEBUG_INFO
#ifdef LOGGING
char *filenames[4]={source_filename,
	stats_filename,
	bitstream_filename,
	log_filename};
#define NUMFILES 4
#else
char *filenames[3]={source_filename,
	stats_filename,
	bitstream_filename};
#define NUMFILES 3
#endif
#else // OUTPUT_DEBUG_INFO
#ifdef LOGGING
char *filenames[5]={source_filename,
	stats_filename,
	bitstream_filename,
	recon_filename,
	log_filename};
#define NUMFILES 5
#else
char *filenames[4]={source_filename,
	stats_filename,
	bitstream_filename,
	recon_filename};

#define NUMFILES 4
#endif
#endif // OUTPUT_DEBUG_INFO


#define INFILE 0
#define STATSFILE 1
#define OUTFILE 2
#define RECONFILE 3
#define LOGFILE 4
#endif

#define STATLINES_BETWEEN_HDRS 80

#ifdef LOGGING
#define ERROR(args...) fprintf(stderr,args); fprintf(logfd,args);
#else
#define ERROR(msg,args...) API_LOG_STDERR(msg,args); 
#endif

// determine if we have a hardware ME
extern int HwMe; // definition taken from VRaptorInfo.h (can't include because it's a c++ header)
#define HW_ME_AVAILABLE HwMe




int save_encoded_data(int fd, EncPacket* packet)
{
	total_written += packet->packet_len;
	if (verbose) API_LOG_STDERR("Writing %d bytes of encoded data at 0x%08x (%02x %02x %02x %02x %02x %02x %02x %02x) total %d\n", 
			packet->packet_len, packet->data, packet->data[0], packet->data[1], packet->data[2], packet->data[3], 
			packet->data[4], packet->data[5], packet->data[6], packet->data[7],total_written);
#ifdef METAWARE_DEBUGGER
	write(fd,(const char*)packet->data,packet->packet_len);
	printf("packet length is %x\n",packet->packet_len);
#else
	memcpy(outputbuffer,(char *)packet->data,packet->packet_len);
	outputbuffer=outputbuffer + packet->packet_len;
	final_size=final_size + packet->packet_len;
#endif
	// Release output packet
	packet->unused = true;
	return packet->packet_len;
}

/*----------------------------------------------------------------------------------*/

int save_debug_output(FILE* file, H264EncodingParams* params, StreamDescriptor* strDesc)
{
	int i;
	int size = 0;

	int HwME = HW_ME_AVAILABLE;
	unsigned char * writePtr;
	int lumaStride = strDesc->width;
	if (HwME == 1 && (strDesc->width & 0x10)) lumaStride += 16;

	// Write to file
	for (i = 0 ; i < params->results->number_of_output_pictures; i++)
	{
		writePtr = params->decoded_frames[i].data.Y_base;
		for (int j = 0; j < strDesc->height; j++)
		{
			fwrite(writePtr, strDesc->width, 1, file);
			writePtr += lumaStride;
		}

		fwrite(params->decoded_frames[i].data.U_base, (strDesc->height * strDesc->width)/4, 1, file);
		fwrite(params->decoded_frames[i].data.V_base, (strDesc->height * strDesc->width)/4, 1, file);
	}
	return size;
}




int main(int argc, char** argv)
{
	int i;
	int outfd = 0;
#ifdef METAWARE_DEBUGGER
	int infd = 0;
#endif

#ifndef METAWARE_DEBUGGER
	serial_init();
#endif

	int more_to_encode;
	StreamDescriptor strDesc;
	H264EncodingParams encParams;
	RawFrame inputFrame;
	RawFrame* inputArray[2];
	EncPacket* outputArray[MAX_OUTPUT_PACKETS];
	ArgFundamental fund;
	int frames = 0;
	RawFrame decoded[MAX_DECODED_FRAMES];

#ifndef METAWARE_DEBUGGER
	print("h.264 encoding\n");
#else
	printf("h.264 encoding\n");
#endif

	init_arc_encoder_api();
#ifdef METAWARE_DEBUGGER
	for (i = 0; i < NUMFILES; i++)
	{
		memset(filenames[i],0,FILENAMELEN);
	}
#endif  

	actual_outputbuffer=outputbuffer;
	// Initialise params and get setup
	memset(&encParams,0,sizeof(H264EncodingParams));
	memset(&setupParams,0,sizeof(H264SetupParams));
	SETUP_ENCARGUMENT(encParams, ENC_ARGTYPE_H264ENCPARAMS, (sizeof(H264EncodingParams)))
		SETUP_ENCARGUMENT(setupParams, ENC_ARGTYPE_H264SETUP, (sizeof(H264SetupParams)))
#ifdef NO_VUI
		setupParams.vui_parameters = 0;
#else
	setupParams.vui_parameters = malloc(sizeof(h264_vui_parameters));
	memset(setupParams.vui_parameters,0,sizeof(h264_vui_parameters));
#endif

	encParams.cumulative_stats = 0;

#ifdef METAWARE_DEBUGGER
	printf("setupparams\n");
#else
	print("setupparmas\n");
#endif
	/* first setup some params to unknown so we can check later that they've been set */
	setupParams.fixed_quantiser_value=-1;
	setupParams.target_bitrate=-1;
	setupParams.quality_level=-1;
	setupParams.frame_width  = 176;
	setupParams.frame_height = 144;
	setupParams.fixed_quantiser = 0 ;  // 1==fixed quantiser, 0==rate distortion
	setupParams.fixed_quantiser_value = 12;
	setupParams.target_bitrate = 128000;
	setupParams.disable_deblocking_filter_idc = 0; 
	setupParams.intra_picture_frequency = 0;
	setupParams.code_all_pictures = false;
	setupParams.target_picture_period = 3000;   // (90000/frame rate)
	setupParams.input_picture_period = 3000;   // (90000/frame rate)
	setupParams.intra_refresh_rate = 0;
	setupParams.real_time_rate_control = false;
	setupParams.chroma_qp_index_offset = 0; 
	setupParams.reaction_multiplier = 30;
	setupParams.maximum_packet_size = 40000;
	setupParams.intra_refresh_rate = 0;
	/* perform final syntax checks */
	if (setupParams.fixed_quantiser)
	{
		setupParams.target_bitrate = 0;

		if (setupParams.fixed_quantiser_value==-1)
		{
#ifdef METAWARE_DEBUGGER
			printf("Error - Fixed Quantiser Value not setup in config file\n");
#else
			print("Error - Fixed Quantiser Value not setup in config file\n");
#endif
			exit(1);
		}
	}
	else
	{
		setupParams.fixed_quantiser_value = 0;

		if (setupParams.target_bitrate==-1)
		{
#ifdef METAWARE_DEBUGGER
			printf("Error - Target Bitrate not setup in config file\n");
#else
			print("Error - Target Bitrate not setup in config file\n");
#endif
			exit(1);
		}
	}  

	// Set up stream descriptor
	strDesc.width = setupParams.frame_width;
	strDesc.height = setupParams.frame_height;
	strDesc.frames = 0;
	strDesc.current_time = 0;
	strDesc.current_frame = 0;
	strDesc.bitrate = 0;
	strDesc.interlaced = 0;

#ifdef METAWARE_DEBUGGER

	if ((infd = open("akiyo_qcif.yuv", O_RDONLY)) == -1)
	{
		ERROR("Error opening input file %s due to %s, giving up\n",filenames[INFILE],strerror(errno));
		return -1;
	}
#endif

#ifdef METAWARE_DEBUGGER
	printf("Set up input frame memory\n");
#else
	print("Set up input frame memory\n");
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
#ifdef METAWARE_DEBUGGER
		printf("%s: Error allocating input data structure, giving up\n",argv[0]);
#else
		print("Error allocating input data structure, giving up\n");
		return -1;
#endif
	}


	inputFrame.info.strDesc = &strDesc;
#ifdef METAWARE_DEBUGGER
	if ((outfd = open("akiyo_qcif.yuv.h264",(O_RDWR|O_CREAT|O_TRUNC),(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))) == -1)
	{
		ERROR("Error opening output file %s due to %s, giving up\n",filenames[OUTFILE],strerror(errno));
		return -1;
	}
#endif

#ifdef METAWARE_DEBUGGER
	printf("h264_enc_init\n");
#else
	print("h264_enc_init\n");
#endif

	if ((i = h264_enc_init(1,                                                           /* handle */
					ENC_TYPE_H264,                                      /* coding_type  */
					&strDesc,                                           /* stream info */
					&(ENC_ARG(setupParams)))) != ENC_ERR_NONE)          /* Encoder-specific extra data -                                                                                                setup params */
	{
#ifdef METAWARE_DEBUGGER
		printf("Error initialising encoder, going no further\n");
#else
		print("Error initialising encoder, going no further %d\n");
#endif
		return -1;
	}
	encParams.decoded_frames = decoded;
	encParams.max_decoded_frames = MAX_DECODED_FRAMES;

#ifdef METAWARE_DEBUGGER
	printf("started encoding\n");
#else
	print("started encoding\n");
#endif

	more_to_encode = 1;
	while (more_to_encode) 
	{
		u32 result;
		u32 status = 0;
		u32 numin;
		u32 numout;
		int i;
		int size = (setupParams.frame_width * setupParams.frame_height * 3)/2;

#ifndef METAWARE_DEBUGGER
		inframe = &inputFrame;
		memcpy((void *)(inframe->data.Y_base),(void *)inputbuffer,size);
		inputbuffer = inputbuffer + size;
		total_size = total_size + size;
#else


		if ((numin = get_frame_from_file(infd, &inputFrame,size)) == 0xffffffff) //FIXME
		{
			ERROR("Fatal error getting input frame %d from file\n",frames);
			return -1;
		}

		if (numin == 0)
		{
			if (verbose) API_LOG_STDERR("End of file\n");
			break;
		}
#endif


		inputFrame.info.frame_num = frames++;

		inputArray[0] = &inputFrame;
		inputArray[1] = 0;
		numin = 1;

		do
		{
			numout =  MAX_OUTPUT_PACKETS;
/*#ifdef METAWARE_DEBUGGER
			printf("h264_enc_encode\n");
#else
			print("h264_enc_encode\n");
#endif*/
			if ((result = h264_enc_encode (1,                    /* handle */
							inputArray,           /* input data */
							&numin,               /* No of input frames available (in) / consumed (out) */
							outputArray,          /* output destination */
							&numout,              /* no of output packets available (in) / produced (out) */
							&status,              /* flags/status */
							&(ENC_ARG(encParams))))          /* Encoder-specific extra data */
					!=  ENC_ERR_NONE)
			{	
				if (result != ENC_ERR_OUTOFOPS)
				{
					if (result == ENC_ERR_EOF)
						more_to_encode = 0;	
					else
					{
#ifdef METAWARE_DEBUGGER
						printf("error during encoding\n");
#else
						print("error during encoding\n");
#endif
						return -1;
					}
				}
			}
/*#ifdef METAWARE_DEBUGGER
			printf("Encoding\n");
#else
			print("Encoding\n");
#endif*/


			if (numout > 0)
			{
				for (i = 0; i < numout; i++)
				{		
					if (save_encoded_data(outfd, outputArray[i]) < 0)
					{
#ifdef METAWARE_DEBUGGER
						print("%s - Fatal error saving encoded output\n");
#else
						print("%s - Fatal error saving encoded output\n");
#endif
						return -1;
					}
				}
			}
		}
		while (result == ENC_ERR_OUTOFOPS);
		if(total_size>=INPUT_FILE_SIZE)
			break;
	} 
#ifdef METAWARE_DEBUGGER
	printf("Completed encoding\n");
#else
	print("Completed encoding\n");
#endif

	h264_enc_uninit   (1);                    /* handle */

	arc_encoder_api_free(inputFrame.data.Y_base,ENC_MEM_INPUTYUV|ENC_MEM_FB_YUV|ENC_MEM_ALIGN_8,0);



	uninit_arc_encoder_api();
#ifdef METAWARE_DEBUGGER
	printf("Finished ... bye!\n");
#else
	print("Finished ... bye!\n");
#endif
	return 1;
}

/*----------------------------------------------------------------------------------*/


