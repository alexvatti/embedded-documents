/* 
 * Moschip Semiconductor Technology Limited
 *  
 */
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code("codesec5")
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "serial_pl01x.h"
#include "arc_codecs.h"
#include "uncached_buf.h"

#if defined __ARC_LINUX__
#include <sys/param.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#else
#define MIN(x, y) _min(x, y)
#endif

#if defined __ARC_SHARED_BUFFER__ && defined __ARC_LINUX__
#include "arc_pgu.h"
#endif

#define METAWARE_DEBUGGER
#undef METAWARE_DEBUGGER
#undef CONFIG_SHIKHARA_FPGA 

static void vbuf_free_last(void);

#define MAX_NUM_BUFFERS 18
int n_yuv_buffers = MAX_NUM_BUFFERS;

#define BUFSZ		0x100000 //1MB

#if defined __ARC_LINUX__
#define GUARD_SIZE 0
#define SDM_PHYS_BASE	0xC0F00000
#else
#define GUARD_SIZE 32
#define SDM_PHYS_BASE	_lr(0xA6)
#endif

#ifdef   CONFIG_SHIKHARA_FPGA 
#define  INPUT_FILE_PTR   0x41B00000 
#define  OUTPUT_FILE_PTR  0x41500000
#else
#define  INPUT_FILE_PTR   0x01B00000 
#define  OUTPUT_FILE_PTR  0x01500000
#endif

/******************************************************************************
  Access to the external function interface structure
 ******************************************************************************/
extern arc_codec_functions h264_functions;

#ifdef METAWARE_DEBUGGER
static FILE *inputFile = NULL;
#endif

static int bufferOffset = 0;

static unsigned char *fileBuffer = NULL;
#ifdef METAWARE_DEBUGGER
static unsigned char *input_Buffer = NULL;
#endif

#define INPUT_FILE_SIZE 77824
#define OUTPUT_FILE_SIZE 5532*1024 

unsigned char *outputbuffer;
unsigned char *actual_outputbuffer;

static int eof = 0;
static int readOffset = 0;
static int startOffset = 0;
static int startNAL = 1;

static int quitDecode = 0;
static int initialised = 0;
int imageHeight = 0;
int imageWidth = 0;
static int numBuffers = 0;
static int frameCounter = 0;
static void *SDMBase = NULL;

#if defined __ARC_SHARED_BUFFER__
#if defined __ARC_LINUX__
static struct fb_fix_screeninfo fb_finfo;
#endif
#else
/* if no shared buffer is used, then a temporary buffer is required to copy
   the decoded frames into */
static unsigned char frameDataBuffer[0x100000];
#endif
static unsigned long vAddresses[64];
static unsigned long pAddresses[64];




/******************************************************************************
  Utility functions
 ******************************************************************************/

static int SetSizes (unsigned long, unsigned long, int);
static int GetNextNAL (unsigned char **, int *);

#ifdef METAWARE_DEBUGGER
void USAGE_MSG(char *prog)
{
	fprintf(stderr, "Usage: %s %s[-stop <frame>] <inputfile> [<outputfile>]\n", 
			prog,
#ifdef AVATAR
			"[-avatar <host>:<port>] [-crc] [-yuv] "
#else
			""
#endif
	       );
	exit(1);
}
#endif

static void flush_dcache(void)
{
	/* Flush bytestream data into memory so that VLC DMA Assist
	   can get it. If we are running under Linux we do not do this 
	   because fileBuffer will be in uncached memory
	 */
#ifndef __ARC_LINUX__
	_sr(1, 0x4b);
	_ASM("sync");
#endif
}

static void fatal(const char *msg)
{
#ifdef METAWARE_DEBUGGER
#if !defined __BENCHMARK__
	if (errno)
		perror(msg);
	else
		fprintf(stderr, "\n%s\n", msg);
#endif
#else
	print(msg);
#endif

	exit(1);

}

#ifdef METAWARE_DEBUGGER

static char *suffix(char *fn)
{
	char *dot = strrchr(fn, '.');

	return dot ? dot + 1 : "";
}

void write_buf(int fd, const unsigned char *data, int sz)
{
	while (sz > 0)
	{
		int n = write(fd, data , MIN(sz, 2048));

		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			fatal("writing yuv data");
		}
		sz -= n;
		data += n;
	}
}

#endif


/******************************************************************************
  main function
 ******************************************************************************/
int main (int argc, char *argv[])
{
	int i,isz,result;
	int numframes = 0, sz;
	unsigned char *inputData = NULL;
	arc_frame_data frameData;
	int stop_after_frame = -1;
	int total_size = 0,yuvsize;
#ifdef METAWARE_DEBUGGER
	char *filename = NULL;
	char *progname = argv[0];
	char *outfilename = NULL;
	int outputFile = -1;
#endif


#ifndef METAWARE_DEBUGGER
	serial_init();
#endif


	//initialise the structure for the returned frame data
	memset(&frameData,0,sizeof(arc_frame_data));
	h264_functions.set_sizes = (fp_set_sizes)SetSizes;

#ifdef METAWARE_DEBUGGER
	if (argc < 2)
		USAGE_MSG(progname);

	argv++;
	argc--;

	/* Parse command line arguments */
	for (; argc; argv++, argc--)
	{
		if (0)
			;
		else if (!strcmp(*argv, "-stop") && argc > 1)
		{
			argc--;
			argv++;
			stop_after_frame = atoi(*argv);
			printf("Stop after frame %d\n", stop_after_frame);
		}
		else if (**argv == '-')
		{
			fprintf(stderr, "Unknown option %s\n", *argv);
			USAGE_MSG(progname);
		}
		else if (!filename)
			filename = *argv;
		else if (!outfilename)
			outfilename = *argv;
		else
			USAGE_MSG(progname); /* too many arguments */
	}

	/* open input stream */
	inputFile = fopen(filename,"rb");
	if (inputFile == NULL)
	{
		perror(filename); 
		return (2);
	} /* if */

#ifndef CRC_OUTPUT
	/* open output stream */
	if (outfilename)
	{
		{
			outputFile = open(outfilename, O_CREAT|O_WRONLY|O_TRUNC, 0666);
			if (outputFile < 0)
				fatal(outfilename);
		}
	}
#endif

#endif

	//SDM control
#if defined __ARC_LINUX__
	//if running under Linux, we need to map the physical location of the SDM memory to a virtual address
	//do this using the memory device and mmap
	int fd = open("/dev/mem",O_RDWR); //try to open /dev/mem
	if (fd < 0)
		fatal("Failed to open /dev/mem");
	//try to map memory to it
	SDMBase = (void *)mmap((void *)0,32768,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SDM_PHYS_BASE);
	if (SDMBase == (void *)-1 || !SDMBase)
		fatal("mmap");
#else
	SDMBase = (void *)SDM_PHYS_BASE; //physical start address of the SDM
#endif

	outputbuffer = (unsigned char *)malloc(OUTPUT_FILE_SIZE);
	actual_outputbuffer = outputbuffer;
	fileBuffer = uncached_bitstream_alloc();


	/***************************
	  Main codec processing
	 ***************************/
#ifndef METAWARE_DEBUGGER
	print("Started H264 Decoding\n");
#endif
	//initialise the codec
	if (!h264_functions.set_buffer_addresses((unsigned long)SDMBase,NULL,NULL) ||
			!h264_functions.init (ARC_H264,NULL,0,0,imageWidth,imageHeight))
		fatal("Failed to construct the decoder");

#ifdef METAWARE_DEBUGGER
	printf("\n Started Decoding : \n");
#else
	print("Started Decoding\n");
#endif
	while (1)
	{
		if (!GetNextNAL(&inputData,&sz))
		{
#ifdef METAWARE_DEBUGGER
			printf("getnextnal failed\n");
#else
			print("\nEnd Of Data\n");
#endif
			//insert a dummy NAL
			inputData = fileBuffer;
			fileBuffer[0] = 0x0B;
			sz = 1;
			quitDecode = 1;
		}
		/*print("Calling Decode Function\n");*/
		result = h264_functions.decode(inputData,sz,0,&numframes);
		if (result < 0)
		{
			fatal("Decode failed");
		}

#ifdef METAWARE_DEBUGGER
		fprintf(stderr,".");
#else
		print(".");
#endif


		for (i=0; i<numframes; i++)
		{
			int isz = (imageHeight * imageWidth);
#if !defined __ARC_SHARED_BUFFER__

			frameData.planes[0] = &frameDataBuffer[0];
			frameData.planes[1] = &frameDataBuffer[isz];
			frameData.planes[2] = &frameDataBuffer[(isz*5)/4];
#endif
			h264_functions.get_frame_data(&frameData);

#ifdef METAWARE_DEBUGGER
			if(outputFile >= 0)
			{

#endif
				int uvSize;
				uvSize = isz/4;
/*#ifdef METAWARE_DEBUGGER
				printf(" Copying Frame\n");
#else
				print(" Copying Frame\n");
#endif*/
				memcpy((void *)outputbuffer,(void *)frameData.planes[0],isz);
				outputbuffer=outputbuffer+isz;
				memcpy((void *)outputbuffer,(void *)frameData.planes[1],uvSize);
				outputbuffer=outputbuffer+isz/4;
				memcpy((void *)outputbuffer,(void *)frameData.planes[2],uvSize);
				outputbuffer=outputbuffer+isz/4;
				yuvsize = isz + (2*uvSize);
				total_size = total_size + yuvsize;
#ifdef METAWARE_DEBUGGER
			}
#endif


			// May need to write the decoded file to memory
#ifdef __ARC_SHARED_BUFFER__
			vbuf_free_last();
#endif

		}


		frameCounter += numframes;
		if (quitDecode)
		{
#ifdef METAWARE_DEBUGGER
			printf("Finished Bye\n");
#else
			print("Finished Bye\n");
#endif
			break;
		}
		if ((stop_after_frame > 0) && (frameCounter >= stop_after_frame))
			break;
	} /* while */
#ifdef METAWARE_DEBUGGER
	write_buf(outputFile,actual_outputbuffer, total_size);
#else
	memcpy((void *)OUTPUT_FILE_PTR,(void *)actual_outputbuffer,total_size);
#endif


	/***************************
	  End of decoding :
	 ***************************/
	h264_functions.uninit();
	uncached_bitstream_free(fileBuffer);

#ifdef METAWARE_DEBUGGER
	if (outputFile >= 0)
		close(outputFile); 
	fclose(inputFile); 
#endif

	return (0);
} /* main */


/******************************************************************************
  GetNextWholeNAL
 ******************************************************************************/

static unsigned char *find_start_code(unsigned char *p)
{
	unsigned state = 0xff00;

	while (state != 0x00000100)
		state = (state | *p++) << 8;
	return p;
}

static int preceding_zero_count(char *p)
{
	int n = 0;

	while (!*p--)
		n++;
	return n;
}


static void fill_buf(void)
{
	int rc;
	if (startOffset < bufferOffset)
		memmove(fileBuffer, &fileBuffer[startOffset], bufferOffset-startOffset);
	bufferOffset = bufferOffset - startOffset;
	readOffset -= startOffset;
	startOffset = 0;

#ifdef METAWARE_DEBUGGER
	input_Buffer = malloc(77824);
	printf("rc =%d\n",rc);
	rc = read(fileno(inputFile),input_Buffer,BUFSZ - bufferOffset - 3);
	if (rc < 0)
		perror("bitstrean read");

	printf("doing memcpy\n");
	memcpy(&fileBuffer[bufferOffset],input_Buffer,INPUT_FILE_SIZE);
	printf("h264 data is %x\n",&fileBuffer[bufferOffset]);
#else
	memcpy(&fileBuffer[bufferOffset],(char *)INPUT_FILE_PTR,INPUT_FILE_SIZE); //newly added below
	rc = INPUT_FILE_SIZE;
#endif

	bufferOffset += rc;
	/* add dummy start code as sentinel (for find_start_code) */
	fileBuffer[bufferOffset] = 0;
	fileBuffer[bufferOffset + 1] = 0;
	fileBuffer[bufferOffset + 2] = 1;
	flush_dcache();


}




static int GetNextNAL (unsigned char **data, int *nalsz)
{
	static int firstTime = 1;
	int zeros = 0;
	unsigned char *nal;
	if (firstTime)
	{
		//the first delimiter in the file should be used to set the startOffset;
		fill_buf();
		nal = find_start_code(fileBuffer);
		readOffset = nal - fileBuffer;
		firstTime = 0;
	}

	//move the start offset to the read offset (i.e. to the end of the previous NAL)
	startOffset = readOffset;
	nal = find_start_code(&fileBuffer[startOffset]);
	if (nal >= &fileBuffer[BUFSZ])
	{
		/* We have exhausted fileBuffer, refill it and try again */
		fill_buf();
		nal = find_start_code(&fileBuffer[startOffset]);
	}
	if (nal >= &fileBuffer[bufferOffset])
	{
		/* we have found our sintenel, i.e. we have reached the end
		   of the input file.  */
		if (startOffset == bufferOffset)
			return 0; //end of file, so need to insert a dummy
		readOffset = bufferOffset;
		*nalsz = readOffset - startOffset;
		*data = &fileBuffer[startOffset];
		return 1;
	}
	readOffset = nal - fileBuffer;
	/* nal points to the byte after 001 */
	zeros = preceding_zero_count(nal - 2);

	*nalsz = readOffset - startOffset - zeros - 1;
	*data = &fileBuffer[startOffset];
	return 1;
}



//temporary callback function until demux features are added to this front end
int SetSizes (unsigned long w, unsigned long h, int num_buffers)
{
	int i;
	//check for initialisation
	if (initialised == 0)
	{
		int isz = w * h * 3 / 2;
		imageWidth = w;
		imageHeight = h;
		numBuffers = num_buffers;

#ifdef METAWARE_DEBUGGER
		fprintf(stderr,"\nInitialising display: %d:%d with %u buffers\n",w,h,num_buffers);
#endif


#if defined __ARC_SHARED_BUFFER__ && defined __ARC_LINUX__
		//set the addresses of all buffers
		int i;
		short s;
		struct yuv_info yuv;
		unsigned long physaddr;
		//open the PGU device
		int fdFrameDevice = open("/dev/fb0",O_RDWR);
		if (fdFrameDevice == -1)
			fatal("arcpgufb: can't open /dev/fb0");
		//get the basic info
		if(ioctl(fdFrameDevice, FBIOGET_FSCREENINFO, &fb_finfo))
			fatal("H264: FBIOGET_FSCREENINFO ioctl failed");
		//mem-map the data area
		unsigned long memBase0 = (unsigned long)mmap(0, fb_finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fdFrameDevice, 0);
		if (ioctl(fdFrameDevice, ARCPGUFB_GETYUV, &yuv) == -1)
			fatal("H264: ARCPGUFB_GETYUV ioctl failed");
		//and the related physical address information
		physaddr = yuv.phys_base;
		//check the number of buffers is correct
		if (yuv.num_buffers < num_buffers)
			fatal("H264: Invalid Number of driver buffers");
		//calculate the start addresses of all buffers
		for (i=0; i < yuv.num_buffers; i++)
		{
			int offset = ioctl(fdFrameDevice, ARCPGUFB_GET_YUV_BASE, i);

			vAddresses[i] = (unsigned long)memBase0 + offset;
			pAddresses[i] = physaddr + offset;
		}
		//set the tail data (used for range checking)
		vAddresses[yuv.num_buffers] = vAddresses[yuv.num_buffers-1] + yuv.yuv_size;
		pAddresses[yuv.num_buffers] = pAddresses[yuv.num_buffers-1] + yuv.yuv_size;
		//initialise the codec memory controller
		close(fdFrameDevice);
		h264_functions.set_buffer_addresses((unsigned long)SDMBase,vAddresses,pAddresses);
#elif defined __ARC_LINUX__
		h264_functions.set_buffer_addresses((unsigned long)SDMBase,NULL,NULL);
#else
		unsigned long memBase = (unsigned long)malloc((isz + (2 * GUARD_SIZE)) * num_buffers);
		if (memBase == 0)
			fatal("H264: Failed malloc for buffers");
		for (i=0; i < MAX_NUM_BUFFERS+1; i++)
		{
			vAddresses[i] = (unsigned long)memBase + GUARD_SIZE;
			pAddresses[i] = vAddresses[i];
			memBase += isz + (2 * GUARD_SIZE);
		}
		h264_functions.set_buffer_addresses((unsigned long)SDMBase,vAddresses,pAddresses);
#endif
		initialised = 1;
	}
	return (1);
}


static int buf_ref_cnt[MAX_NUM_BUFFERS];
static int last_frame;

int vbuf_alloc(void)
{
	int i;

	for (i=0; buf_ref_cnt[i] && i< MAX_NUM_BUFFERS; i++);
	if (i == MAX_NUM_BUFFERS)
		fatal("no more output buffers");
	buf_ref_cnt[i]++;
	return i;
}

void vbuf_free(int b)
{
	buf_ref_cnt[b]--;
}

void vbuf_send(int b)
{
	last_frame = b;
	buf_ref_cnt[b]++;
}

static void vbuf_free_last(void)
{
	vbuf_free(last_frame);
}


#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code()
#endif
