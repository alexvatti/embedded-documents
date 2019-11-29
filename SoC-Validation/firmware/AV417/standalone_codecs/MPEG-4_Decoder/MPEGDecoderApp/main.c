/*
 * Moschip Semiconductor Technology Limited
 *
 * main.c
 *
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "serial_pl01x.h"
#include "arc_codecs.h"
#include "uncached_buf.h"

#undef CONFIG_SHIKHARA_FPGA 
#define  METAWARE_DEBUGGER 1
#undef   METAWARE_DEBUGGER
#define  MIN(x, y) _min(x, y)

#ifdef   CONFIG_SHIKHARA_FPGA 
#define  INPUT_FILE_PTR   0x41B00000 
#define  OUTPUT_FILE_PTR  0x41500000
#else
#define  INPUT_FILE_PTR   0x01B00000 
#define  OUTPUT_FILE_PTR  0x01500000
#endif

#define YUV_RATIO        (2)
#define NUM_BUFFERS       5
#define SDM_PHYS_BASE	 _lr(0xA6)
#define GUARD_SIZE       32
#define INPUT_FILE_SIZE  158720
#define OUTPUT_FILE_SIZE 5532*1024
extern void print(const char *s);

extern arc_codec_functions mpg_functions; 
extern unsigned char isYUV422;
static unsigned char *out_buffer=NULL;        
static unsigned char *actual_out_buffer=NULL; 
int vd_use_slices = 1;
static void *SDMBase;
#ifdef METAWARE_DEBUGGER 
static FILE *inputFile = NULL;	               /* The file pointer for the input stream */
#endif
static unsigned int hwptr = 0;
static unsigned char *fileBuffer = NULL;       /* The input buffer used during parsing */
static unsigned char *hw_ptr=NULL;
static int readOffset = 0;	               /* current read offset in the buffer */
static int imageHeight = 0;                    /* height of the image in pixels */
static int imageWidth = 0;                     /* width of the image in pixels */
static unsigned int nextNalType = 0;           /* Type of the next (not current) NAL */
static unsigned long vAddresses [NUM_BUFFERS]; /* virtual addresses of the buffers */
static int sliceDataSize = 0;
static unsigned char *inputData;
static int inputType = 0;
static void *output_buf_base = NULL;           /* base address for output buffers */

/******************************************************************************
  Function prototypes
 ******************************************************************************/
static int GetNextNAL(unsigned char **);
static int GetNextChar(unsigned char *);
static void ProcessMP4Headers(void);
static void SetSizes (int, int, int);
static void vbuf_free_last(void);

void write_buf(int fd, const unsigned char *data, int sz)
{
	while (sz > 0)
	{
		int n = write(fd, data , MIN(sz, 2048));

		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			print("writing yuv data");
		}
		sz -= n;
		data += n;
	}
}


void *alloc_output_buf(int width, int height, int nbufs)
{
	int isz = (width * height * YUV_RATIO);
	void *buf = malloc((isz * nbufs)  + (2 * GUARD_SIZE));
	if (!buf)
		print("Out of memory");
	return buf;
}



/******************************************************************************
  main function
 ******************************************************************************/

int main (int argc, char *argv[])
{
	serial_init();

	int i;
	unsigned int num_bytes;
	int nalType;
	int numFrames = 0,nal_size,image_size,total_size=0;
	char *filename = NULL;
	unsigned char *startSliceData;
	int frameCounter = 0;
	arc_frame_data frameData;

#ifdef METAWARE_DEBUGGER 
	int outputFile = -1;
#endif

	/* initialise the structure for the returned frame data */
	memset(&frameData,0,sizeof(arc_frame_data));
	mpg_functions.set_sizes = (fp_set_sizes)SetSizes;
	inputType = ARC_MPEG4;

#ifdef METAWARE_DEBUGGER 
	inputFile = fopen("input.m4v", "rb");    /* open the input stream */
	if (inputFile == NULL)
		print("No InputFile\n");
	outputFile = open("outputfile.yuv", O_CREAT|O_WRONLY|O_TRUNC, 0666); /* open output stream */
	if (outputFile < 0)
		print("No output file\n");
#endif    

	SDMBase = (void *)SDM_PHYS_BASE;         /*physical start address of the SDM*/
	fileBuffer = uncached_bitstream_alloc();
	hw_ptr= uncached_bitstream_alloc();

#ifdef METAWARE_DEBUGGER 
	num_bytes=fread(hw_ptr,1,INPUT_FILE_SIZE,inputFile);
#else
	memcpy(hw_ptr,(char *)INPUT_FILE_PTR,INPUT_FILE_SIZE);
#endif

	startSliceData = fileBuffer;

	/* MPEG4 will decode using different video object planes this function will check
	   different NAL packet types and retuns the slicedatasize of video object plane*/
	ProcessMP4Headers();

	/* The codec requires access to the SIMD unit<92>s SDM buffer and enough working YUV buffers to
	   complete a successful decode. This set_buffer_addressesfunction initializes the codec to set the correct addresses
	   according to the execution environment*/
	mpg_functions.set_buffer_addresses((unsigned long)SDMBase,NULL,NULL);
	mpg_functions.init (inputType,NULL,0,0,imageWidth,imageHeight);
	print("Decoding :\n");
	out_buffer=(unsigned char *)malloc(OUTPUT_FILE_SIZE);
	actual_out_buffer=out_buffer;
	print("Started decoding a frame\n");
	while (1)
	{
		while (1)
		{

			/* This function returns the size of the NAL packet */
			nal_size = GetNextNAL(&inputData);
			if (nal_size == 0)
			{
				nextNalType = 0;
				break;
			}
			nalType = nextNalType;
			sliceDataSize += nal_size;
			if (nalType == 0x1B6)
			{
				sliceDataSize -= 4;
				break;
			}
		}

		/* flush data cache */
		_sr(0x1,0x47);

		if (!mpg_functions.decode (startSliceData,sliceDataSize,0,&numFrames))
		{
			print("\nDecode failed\n");
			return (2);
		}
		/*print("completed decoding a frame\n");*/
		/* get pointers to the decoded data if there is any */
		if (numFrames == 1)
			mpg_functions.get_frame_data(&frameData);
		if (numFrames == 1)
		{
			int isz = (imageHeight * imageWidth);
			int uvSize;
			if (isYUV422)
				uvSize = isz/2;
			else
				uvSize = isz/4;
			memcpy(out_buffer,frameData.planes[0],isz);
			out_buffer=out_buffer+isz;
			memcpy(out_buffer,frameData.planes[1],uvSize);
			out_buffer=out_buffer+uvSize;
			memcpy(out_buffer,frameData.planes[2],uvSize);
			out_buffer=out_buffer+uvSize;
			image_size=isz+(2*uvSize);
			total_size=total_size+image_size;		
		}
		if (numFrames == 1) vbuf_free_last();
		/*Total number of frames decoded */
		frameCounter += numFrames;
		/* check for end of sequence */
		if (sliceDataSize == 0)
			break;

		/* If the next NAL type is 0, it must be end of file */
		if (nextNalType == 0)
		{
			readOffset = 0;
			sliceDataSize = 0;
		}
		else
		{
			/* otherwise, reinstate the header of the next NAL at the start
			   of the file input buffer */
			readOffset = 4;
			fileBuffer[0] = 0x00;
			fileBuffer[1] = 0x00;
			fileBuffer[2] = 0x01;
			fileBuffer[3] = (unsigned char)nextNalType;
		}

	} /* while */
	print("Completed decoding all frames\n");
#ifdef METAWARE_DEBUGGER 
	write_buf(outputFile,actual_out_buffer,total_size);
#else
	memcpy((char *)OUTPUT_FILE_PTR,actual_out_buffer,total_size);
#endif
	print("completed decoding\n");
	print("Finished ... bye!");
	/**************   End of decoding  *************/
	mpg_functions.uninit();
#ifdef METAWARE_DEBUGGER 
	close(outputFile); 
	fclose(inputFile);
#endif
	uncached_bitstream_free(baseVirtualAddress);
	return (0);
}


/* read in data from the current NAL until the start code of the next NAL is reached 
   and remember its type */
static int GetNextNAL (unsigned char **data)
{
	static int firstTime = 1;
	unsigned char c;
	int size = 0;
	unsigned long value = 0x000000FF;
	*data = &fileBuffer[readOffset];
	nextNalType = 0;
	while (GetNextChar(&c)) {
		fileBuffer[readOffset++] = c; 
		size++;
		value <<= 8;
		if (value == 0x00000100)
		{
			nextNalType = 0x100 | (unsigned int)c;
			break;
		}
		value |= (unsigned long)c;
	}

	return (size);
}

static int GetNextChar (unsigned char *c)
{
	*c=hw_ptr[hwptr];
	hwptr++;
	if(hwptr>=INPUT_FILE_SIZE)
	{
		sliceDataSize=0;
		nextNalType=0;
		return 0;
	}
	return (1);
}

static void ProcessMP4Headers (void)
{
	int i;
	unsigned char c;
	unsigned long value = 0;
	while(1)
	{
		int nalsize = GetNextNAL(&inputData);
		if (!nalsize)
			print("INTERNAL ERROR: ProcessMP4Headers failed!\n");
		sliceDataSize += nalsize;
		if ((nextNalType >= 0x0120) && (nextNalType <= 0x012F))
			break; 
		else
			print("Different NAL Type\n");
	}
	while(1)
	{
		sliceDataSize += GetNextNAL(&inputData);
		if (nextNalType == 0x01B6)
			break;
	}
}

void SetSizes (int w, int h, int numBuffers)
{

	int i;
	imageWidth = w;
	imageHeight = h;
	int isz = ((imageHeight * imageWidth) * YUV_RATIO);
	if (!output_buf_base)
	{
		output_buf_base = alloc_output_buf(imageHeight, imageWidth, NUM_BUFFERS);

	}
	for (i=0; i<NUM_BUFFERS; i++)
	{
		vAddresses[i] = (unsigned long)output_buf_base + (i*isz) + GUARD_SIZE;

	}
	if (!mpg_functions.set_buffer_addresses((unsigned long)SDMBase, vAddresses, vAddresses))
		print("Failed to construct the decoder");
}

static int buf_ref_cnt[NUM_BUFFERS];
static int last_frame;

int vbuf_alloc(void)
{
	int i;

	for (i=0; buf_ref_cnt[i] && i< NUM_BUFFERS; i++);
	if (i == NUM_BUFFERS)
		print("no more output buffers");
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

