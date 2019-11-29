#include "../headers/shikhara_map.h"
#include "../headers/shikhara_uart.h"

#define TIMER_CTRL 0x22
#define TIMER_CNT  0x21
#define TIMER_LMT  0x23
#define SDM_BG_BUF       0
#define SDM_FG_BUF       8192
#define SDM_BLENDED_BUF  16384
#define SDM_BUF_SIZE     8192


void InitialiseDMA();

_Asm void GetData(unsigned int sdm_dest, unsigned int sys_source) 
{
	% reg sdm_dest, sys_source
		vdiwr %dr0, sdm_dest   // DR0   SDM destination addr
		vdiwr %dr4, sys_source // DR4   System base addr
		vdiwr %dr2, 0x1f4080   // DR2   idx 31, blk_ver_size<<8 || block_hor_size // note number of lines can't be >                                                 128 or would be too big for SDM (32KB)
		vdirun %r0, %r0        //  Do DMA; location does not need to be specified for non-frame table mode
}

_Asm void WriteData(unsigned int sdm_source, unsigned int sys_dest)
{
	% reg sdm_source, sys_dest
		vdowr %dr0, sdm_source  // DR0   SDM destination addr
		vdowr %dr4, sys_dest    // DR4   System base addr
		vdowr %dr2, 0x1f4080    // DR2   idx 31, blk_ver_size<<8 || block_hor_size // note number of lines can't be >                                                 128 or would be too big for SDM (32KB)
		vdorun %r0, %r0         //  Do DMA; location does not need to be specified for non-frame table mode
}

_Asm void AlphaBlend(unsigned char alpha_value, unsigned int bg_sdm_addr, unsigned int fg_sdm_addr, unsigned int blended_sdm_addr)
{
	% reg alpha_value, bg_sdm_addr, fg_sdm_addr, blended_sdm_addr
		vmovw %vr00, fg_sdm_addr, 1                          // I0 = 8192
		vmovw %vr00, bg_sdm_addr, 2                          // I1 = 0
		vmovw %vr00, blended_sdm_addr, 4                     // I2 = 16384
		vld64w %vr01, [%i0, 0]                               /* v1 contains foreground pixels. I0 should start at 
                                                                        8192 for foreground pixels */
		vld64w %vr02, [%i1, 0]                               /* v2 contains background pixels. I1 should start at                                                                            zero for background pixels */
		/*Below does the following calculation:
		(alpha*(pixfg - pixbg))>>8 + pixbg*/
		vsubw %vr03, %vr01, %vr02                            // v3 contains result of subtraction
		vbmulw %vr04, %vr03, alpha_value                     // alpha val is broadcast across all eight 16-bit words
		vasrw %vr05, %vr04, 7                                // 8-bit right shift
		vaddw %vr06, %vr05, %vr02                            // final blended pixel in v6 (16 bits per pixel)
		vasrpwb %vr07, %vr06, 0                              // shift right zero places and pack words to bytes
		vst64 %vr07, [%i2, 0]                                // write lower 8 bytes to blended_sdm_addr
}

int main (int argc, char*argv[])
{   
	int i, j;
	unsigned char alpha_val;
	unsigned int timer_count, busy_flags;
	_sr(0xffffffff, TIMER_LMT);                                  /* set limit */
 	_sr(3, TIMER_CTRL);                                          /* set conrol register to generate interrupt on wrap and                                                                        only count when ARC 700 is in run mode */
	_sr(0, TIMER_CNT);                                           // reset timer count
	//Call initialise dma
	alpha_val = 60;
	InitialiseDMA();
	unsigned int bg_ext_addr=BG_EXT_BASE, fg_ext_addr=FG_EXT_BASE, blended_ext_addr=BLENDED_EXT_BASE; 
	for (j=0; j<126; j++)
	{
		GetData(SDM_BG_BUF, bg_ext_addr);                    // SDM base is zero, system address 
		GetData(SDM_FG_BUF, fg_ext_addr);                    // SDM base is 8192, system address 
		_ASM("vdiwr %dr6, 2");		                     // DR6 Clip mode - NON INTERLACE
		unsigned int bg_sdm=SDM_BG_BUF, fg_sdm=SDM_FG_BUF, blended_sdm=SDM_BLENDED_BUF;
		for (i=0; i<1024;i++)
		{
			AlphaBlend(alpha_val, bg_sdm, fg_sdm, blended_sdm); /* Do Alpha blend on data in SDM - alpha value;                                                                                 bg_sdm_address; fg_sdm_address; 
                                                                               blended_sdm_address */ 
			bg_sdm+=8;
			fg_sdm+=8;
			blended_sdm+=8;
		}
		do{
		busy_flags = _lr(0xa2) >> 6;
		busy_flags = busy_flags & 7;}
        	while (busy_flags != 0);
	
		WriteData(SDM_BLENDED_BUF, blended_ext_addr); 
		bg_ext_addr+=SDM_BUF_SIZE;
		fg_ext_addr+=SDM_BUF_SIZE;
		blended_ext_addr+=SDM_BUF_SIZE;
	}


	print("completed\n");
	timer_count = _lr(TIMER_CNT);
	return 0;
}


void InitialiseDMA()
{
	_ASM("vdiwr %dr1, 128");	// DR1 SDM Stride  - 128
	_ASM("vdiwr %dr5, 128");	// DR5 System Memory Stride  - 128
	_ASM("vdiwr %dr6, 2");		// DR6 Clip mode - NON INTERLACE
	_ASM("vdowr %dr1, 128");	// DR1 SDM Stride  - 128
	_ASM("vdowr %dr5, 128");	// DR5 System Memory Stride  - 128
	_ASM("vdowr %dr6, 0");		// DR6 single stride
}

void isr3_dummy(void)
{
        // dummy isr
}
void isr4_dummy(void)
{
        // dummy isr
}
void isr5_dummy(void)
{
        // dummy isr
}
void isr6_dummy(void)
{
        // dummy isr
}
void isr7_dummy(void)
{
        // dummy isr
}
void isr8_dummy(void)
{
        // dummy isr
}
void isr9_dummy(void)
{
        // dummy isr
}
void isr10_dummy(void)
{
        // dummy isr
}
void isr11_dummy(void)
{
        // dummy isr
}
void isr12_dummy(void)
{
        // dummy isr
}
void isr13_dummy(void)
{
        // dummy isr
}
void isr14_dummy(void)
{
        // dummy isr
}
void isr15_dummy(void)
{
        // dummy isr
}
