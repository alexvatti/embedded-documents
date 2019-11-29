#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_hdmi.h>
#include <asm/io.h>
#include <common.h>
#include <lcd.h>
#include <sound.h>
#include <malloc.h>
#ifdef PCM
#undef PCM
#endif
#include "../lib/snps-hdmi/core/audio.h"
#include "../lib/snps-hdmi/core/halAudioDma.h"
#include "../lib/snps-hdmi/core/control.h"
#include "../lib/snps-hdmi/api/api.h"
#ifndef DEBUG_HDMI_AUDIO
//#define DEBUG_HDMI_AUDIO
#endif
#ifdef DEBUG_HDMI_AUDIO
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
#define AHB_AUDIO_DMA_INT_MASK		0xF7
#define AHB_AUDIO_DMA_INT_UNMASK        0x66	
#undef HDMI_DMA_PING_PONG_BUFF 

#define CONFIG_DDR_BASE 0x00000000


#define DDR_AUDIO_RAW  0x6400000
#define DDR_AUDIO_IEC_DATA (CONFIG_DDR_BASE + DDR_AUDIO_RAW)
void hdmi_clear_overflow(void);

static const u8 AHB_DMA_STRADDR1_0  = 0x20;
static const u8 AHB_DMA_STRADDR1_1  = 0x21;
static const u8 AHB_DMA_STRADDR1_2  = 0x22;
static const u8 AHB_DMA_STRADDR1_3  = 0x23;
static const u8 AHB_DMA_STPADDR1_0  = 0x24;
static const u8 AHB_DMA_STPADDR1_1  = 0x25;
static const u8 AHB_DMA_STPADDR1_2  = 0x26;
static const u8 AHB_DMA_STPADDR1_3  = 0x27;
static const u16 AUD_BASE_ADDR = 0x3100;
static const u16 AUDIO_DMA   = 0x0500;
static const u16 FC_BASE_ADDR = 0x1000;
static u16 hdmi_base_addr = 0x8000;
int acp_length = 16, auto_send = 0;

void hdmi_mc_audiodma_reset(void)
{
	hdmi_writeb(0, HDMI_MC_SWRSTREQ2);
}

void hdmi_insert_pcuv(void)
{
	hdmi_mask_writeb(1, HDMI_AHB_DMA_CONF0,
                        HDMI_AHB_DMA_PCUV_OFFSET,
                        HDMI_AHB_DMA_PCUV_MASK);
}

static unsigned char g_packet_head_table[48*8];
hdmi_audio_header_t iec_header;

static int hdmi_dma_update_iec_header(void)
{
		iec_header.B.source = 2;

        switch (sound_params.freq) {
        case 32000:
                iec_header.B.sample_freq = 0x03;
                iec_header.B.org_sample_freq = 0x0C;
                break;
        case 44100:
                iec_header.B.sample_freq = 0x00;
                iec_header.B.org_sample_freq = 0x0F;
                break;
        case 48000:
                iec_header.B.sample_freq = 0x02;
                iec_header.B.org_sample_freq = 0x0D;
                break;
        case 88200:
                iec_header.B.sample_freq = 0x08;
                iec_header.B.org_sample_freq = 0x07;
                break;
        case 96000:
                iec_header.B.sample_freq = 0x0A;
                iec_header.B.org_sample_freq = 0x05;
		break;
        case 176400:
                iec_header.B.sample_freq = 0x0C;
                iec_header.B.org_sample_freq = 0x03;
                break;
        case 192000:
                iec_header.B.sample_freq = 0x0E;
                iec_header.B.org_sample_freq = 0x01;
                break;
        default:
                printf("HDMI Audio sample rate error");
                return -1;
        }

        switch (sound_params.bps) {
        case 16:
                iec_header.B.word_length = 0x02;
                break;
        case 24:
                iec_header.B.word_length = 0x0b;
                break;
        default:
                return -1;
        }

        return 0;
}



static void init_table(int channels)
{
        int i;
        int ch = 0;
        unsigned char *p = g_packet_head_table;

        for (i = 0; i < 48; i++) {
                int b = 0;
                if (i == 0)
                        b = 1;

                for (ch = 0; ch < channels; ch++) {
                        int c = 0;
                        if (i < 42) {
                                iec_header.B.channel = ch+1;
                                c = (iec_header.U >> i) & 0x1;
                        }
                        /* preset bit p as c */
                        *p++ = (b << 4) | (c << 2) | (c << 3);
                }
        }
}


static void hdmi_dma_copy_16_c_lut(unsigned short *src, unsigned int *dst, int samples, unsigned char *lookup_table)
{
        int i;
        unsigned int sample;
        unsigned int p;
        unsigned int head;

        for (i = 0; i < samples; i++) {
                /* get source sample */
                sample = *src++;
#ifdef INSERT_AUTO_PCUV
                *dst++ = sample << 8;
#else
                /* xor every bit */
                p = sample ^ (sample >> 8);
                p ^= (p >> 4);
                p ^= (p >> 2);
                p ^= (p >> 1);
                p &= 1; /* only want last bit */
                p <<= 3; /* bit p */

                /* get packet header */
                head = *lookup_table++;

                /* fix head */
                head ^= p;

                /* store */
                *dst++ = (head << 24) | (sample << 8);
#endif
        }
}


static void hdmi_dma_copy_16_c_fast(unsigned short *src, unsigned int *dst, int samples)
{
        int i;
        unsigned int sample;
        unsigned int p;

        for (i = 0; i < samples; i++) {
                /* get source sample */
                sample = *src++;
#ifdef INSERT_AUTO_PCUV
		*dst++ = sample << 8;
#else

                /* xor every bit */
                p = sample ^ (sample >> 8);
                p ^= (p >> 4);
                p ^= (p >> 2);
                p ^= (p >> 1);
                p &= 1; /* only want last bit */
                p <<= 3; /* bit p */

                /* store */
                *dst++ = (p << 24) | (sample << 8);
#endif
        }
}

static void hdmi_dma_copy_16(unsigned short *src, unsigned int *dest, int framecount, int channelcount)
{
        /* split input frames into 192-frame each */
        int count_in_192 = (framecount + 191) / 192;
        int i;

        for (i = 0; i < count_in_192; i++) {
                int count;
                int samples;

                /* handles frame index [0, 48) */
                count = (framecount < 48) ? framecount : 48;
                samples = count * channelcount;
                hdmi_dma_copy_16_c_lut(src, dest, samples, g_packet_head_table);
                framecount -= count;
                if (framecount == 0)
                        break;

                src  += samples;
                dest += samples;

                /* handles frame index [48, 192) */
                count = (framecount < 192 - 48) ? framecount : 192 - 48;
                samples = count * channelcount;
                hdmi_dma_copy_16_c_fast(src, dest, samples);
                framecount -= count;
                src  += samples;
                dest += samples;

        }
}


static void hdmi_dma_mmap_copy(unsigned int data, unsigned int *iec_data, int count)
{
        u32 framecount;
        u32 *dest;
        unsigned short sample_align=2;
        u16 *src16;

        framecount =  count/(sample_align * sound_params.channels);

        dest = (u32 *)(iec_data);

        switch (sound_params.bps) {

        case 16:
                /* dma_buffer is the mmapped buffer we are copying pcm from. */
                src16 = (u16 *)(data);
                hdmi_dma_copy_16(src16, dest, framecount, sound_params.channels);
                break;

        default:
                printf("HDMI Audio invalid sample format\n");
                return;
        }
}


static void hdmi_dma_init_iec_header(void)
{
        iec_header.U = 0;
        iec_header.B.consumer = 0;              /* Consumer use */
        iec_header.B.linear_pcm = 0;            /* linear pcm audio */
        iec_header.B.copyright = 1;             /* no copyright */
        iec_header.B.pre_emphasis = 0;          /* 2 channels without pre-emphasis */
        iec_header.B.mode = 0;                  /* Mode 0 */

        iec_header.B.category_code = 0;

        iec_header.B.source = 2;                /* stereo */
        iec_header.B.channel = 0;

        iec_header.B.sample_freq = 0x02;        /* 48 KHz */
        iec_header.B.clock_acc = 0;             /* Level II */

        iec_header.B.word_length = 0x02;        /* 16 bits */
        iec_header.B.org_sample_freq = 0x0D;    /* 48 KHz */

        iec_header.B.cgms_a = 0;        /* Copying is permitted without restriction */
}

void hdmi_dma_irq_mute(int mute)
{
        if (mute)
                hdmi_writeb((1<<2), HDMI_IH_MUTE_AHBDMAAUD_STAT0);
        else
                hdmi_writeb(0x00, HDMI_IH_MUTE_AHBDMAAUD_STAT0);
}

static void hdmi_stop(void)
{
        audio_DmaStopRead(hdmi_base_addr);
        audio_DmaInterruptEnable(hdmi_base_addr, AHB_AUDIO_DMA_INT_MASK);
}

#ifdef HDMI_DMA_PING_PONG_BUFF
void hdmi_enable_dma_loop_mode()
{
	u8 reg;
	reg = hdmi_readb(HDMI_AHB_DMA_CONF2);
	reg |= 0x1;
	hdmi_writeb(reg, HDMI_AHB_DMA_CONF2);
}	

void hdmi_disable_dma_loop_mode()
{
	u8 reg;
	reg = hdmi_readb(HDMI_AHB_DMA_CONF2);
	reg &= ~(0x1);
	hdmi_writeb(reg, HDMI_AHB_DMA_CONF2);

}

void halAudioDma_StartAddress1(u16 baseAddress, u32 startAddress) 
{                       
        access_CoreWriteByte((u8)(startAddress), baseAddress + AHB_DMA_STRADDR1_0);
        access_CoreWriteByte((u8)(startAddress >> 8), baseAddress + AHB_DMA_STRADDR1_1);
        access_CoreWriteByte((u8)(startAddress >> 16), baseAddress + AHB_DMA_STRADDR1_2);
        access_CoreWriteByte((u8)(startAddress >> 24), baseAddress + AHB_DMA_STRADDR1_3);
}

void halAudioDma_StopAddress1(u16 baseAddress, u32 stopAddress)
{       
        access_CoreWriteByte((u8)(stopAddress), baseAddress + AHB_DMA_STPADDR1_0);
        access_CoreWriteByte((u8)(stopAddress >> 8), baseAddress + AHB_DMA_STPADDR1_1);
        access_CoreWriteByte((u8)(stopAddress >> 16), baseAddress + AHB_DMA_STPADDR1_2);
        access_CoreWriteByte((u8)(stopAddress >> 24), baseAddress + AHB_DMA_STPADDR1_3);
}

void audio_DmaRequestAddress1(u16 baseAddr, u32 startAddress, u32 stopAddress)
{
	halAudioDma_StartAddress1(baseAddr + AUD_BASE_ADDR + AUDIO_DMA, startAddress);
	halAudioDma_StopAddress1(baseAddr + AUD_BASE_ADDR + AUDIO_DMA, stopAddress);
}
#endif


void hdmi_mask_interrupts(void)
{
	u8 ih_mute;
	ih_mute = hdmi_readb(HDMI_IH_MUTE) |
                  HDMI_IH_MUTE_MUTE_WAKEUP_INTERRUPT |
                  HDMI_IH_MUTE_MUTE_ALL_INTERRUPT;
	hdmi_writeb(ih_mute, HDMI_IH_MUTE);
	hdmi_writeb(0xff, HDMI_VP_MASK);	
	hdmi_writeb(0xff, HDMI_FC_MASK0);
        hdmi_writeb(0xff, HDMI_FC_MASK1);
        hdmi_writeb(0xff, HDMI_FC_MASK2);			/* Unmasking overflow and underflow interrupts */
	hdmi_writeb(0xff, HDMI_PHY_MASK0);
        hdmi_writeb(0xff, HDMI_PHY_I2CM_INT_ADDR);
        hdmi_writeb(0xff, HDMI_PHY_I2CM_CTLINT_ADDR);
        hdmi_writeb(0xff, HDMI_AUD_INT);
        hdmi_writeb(0xff, HDMI_AUD_SPDIFINT);
        hdmi_writeb(0xff, HDMI_AUD_HBR_MASK);
        hdmi_writeb(0xff, HDMI_GP_MASK);
        hdmi_writeb(0xff, HDMI_A_APIINTMSK);
        hdmi_writeb(0xff, HDMI_CEC_MASK);
        hdmi_writeb(0xff, HDMI_I2CM_INT);
        hdmi_writeb(0xff, HDMI_I2CM_CTLINT);

	hdmi_writeb(0xff, HDMI_IH_MUTE_FC_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_FC_STAT1);
        hdmi_writeb(0xff, HDMI_IH_MUTE_FC_STAT2);
        hdmi_writeb(0xff, HDMI_IH_MUTE_AS_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_PHY_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_I2CM_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_CEC_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_VP_STAT0);
        hdmi_writeb(0xff, HDMI_IH_MUTE_I2CMPHY_STAT0);
	ih_mute &= ~(HDMI_IH_MUTE_MUTE_WAKEUP_INTERRUPT | HDMI_IH_MUTE_MUTE_ALL_INTERRUPT);
        hdmi_writeb(ih_mute, HDMI_IH_MUTE);
}



int shikhara_hdmiaudio_init(void)
{
	hdmi_dma_init_iec_header();
	hdmi_dma_update_iec_header();
	init_table(sound_params.channels);
	
	hdmi_mask_interrupts();			
	hdmi_writeb(0, HDMI_IH_MUTE_FC_STAT2);		/* Unmuting overflow and underflow interrupts */
	audio_DmaInterruptEnable(hdmi_base_addr, AHB_AUDIO_DMA_INT_UNMASK); /* Unmasking of DMA DOne Interrupt*/    	
#ifdef CONFIG_SHIKHARA_ASIC
	halAudioDma_HlockEnable(hdmi_base_addr + AUD_BASE_ADDR + AUDIO_DMA, TRUE);
#endif
	halAudioDma_ResetFifo(AUD_BASE_ADDR + AUDIO_DMA);
#ifdef INSERT_AUTO_PCUV
	hdmi_mc_audiodma_reset();
	hdmi_insert_pcuv();
#endif
#ifdef HDMI_DMA_PING_PONG_BUFF
	hdmi_enable_dma_loop_mode();
#endif
	if (hdmi_readb(HDMI_IH_FC_STAT2) & HDMI_IH_FC_STAT2_OVERFLOW_MASK)
	{
        	hdmi_clear_overflow();
		printf("Overflow interrupt received\n");
	        /* clear irq status */
	        hdmi_writeb(HDMI_IH_FC_STAT2_OVERFLOW_MASK, HDMI_IH_FC_STAT2);
	}
	printf("HDMI Audio driver configured successfully\n");
	return 0;
}

/*
 * Conditions for DMA to work:
 * ((final_addr - initial_addr)>>2)+1) < 2k.  So max period is 8k.
 * (inital_addr & 0x3) == 0
 * (final_addr  & 0x3) == 0x3
 *
 * The DMA Period should be an integer multiple of the IEC 60958 audio
 * frame size, which is 768 bytes (192 * 4).
 */

int shikhara_hdmi_transfer_tx_data(unsigned int data ,unsigned long data_size,int16_t Align)
{

	if(Align == MONO)
		{
	                printf("ERROR: Cannot support Mono format\n");
	                return 1;
	        }
	if(data_size & 0x3)
	{
		data_size &= (unsigned long)~(0x3);
		printf("Got miss aligned data_size and corrected\n");
	}
	if(data & 0x03)
	{
		data &=	(unsigned int) ~(0x3);  
		printf("Got miss aligned data and corrected\n");
	}

        unsigned int status, loop_count;
	unsigned int *iec_data;
	unsigned int hw_buffer;
	loop_count = 0;
	
	iec_data =  (unsigned int *)DDR_AUDIO_IEC_DATA;
	if(!iec_data)
		{
			printf("malloc for data_size is failed\n");
			return 1;
		}
	hdmi_dma_mmap_copy(data, iec_data, data_size);
	hw_buffer = (u32)iec_data;
	printf("hw_buffer is %x\n", hw_buffer);
	data_size = 2* data_size;
	halAudioDma_ResetFifo(AUD_BASE_ADDR + AUDIO_DMA);
        udelay(10);
	status = control_InterruptAudioDmaState(hdmi_base_addr);
        control_InterruptAudioDmaClear(hdmi_base_addr, status);             /* Clearing DMA interrupt*/
	audio_DmaInterruptEnable(hdmi_base_addr, AHB_AUDIO_DMA_INT_UNMASK); /* Unmasking of DMA DOne Interrupt*/    	
	hdmi_dma_irq_mute(1);
	{
		if((data_size -loop_count) >= DMA_PERIOD_BYTES)
		{
			audio_DmaRequestAddress(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (loop_count + DMA_PERIOD_BYTES))-1);
#ifdef HDMI_DMA_PING_PONG_BUFF
			audio_DmaRequestAddress1(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (loop_count + DMA_PERIOD_BYTES))-1);
#endif
			loop_count=loop_count+DMA_PERIOD_BYTES;
		}
		else
		{
			audio_DmaRequestAddress(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (data_size - loop_count))-1);
#ifdef HDMI_DMA_PING_PONG_BUFF
			audio_DmaRequestAddress1(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (data_size - loop_count))-1);
#endif
			loop_count=data_size;
		}
            	audio_DmaStartRead(hdmi_base_addr);
	}
	hdmi_dma_irq_mute(0);
	dbg("DMA started \n");
	while(1)
	       { 
		status = hdmi_readb(HDMI_IH_FC_STAT2);
		if(status)
			{
			 	dbg("HDMI_IH_FC_STAT2 status is %x\n", status);
				hdmi_writeb(status, HDMI_IH_FC_STAT2); 
			}
		status = hdmi_readb(HDMI_IH_AS_STAT0);
		if(status)
			{
				dbg("HDMI_IH_AS_STAT0 status is %x\n", status);
				hdmi_writeb(status, HDMI_IH_AS_STAT0);
			}

		status = control_InterruptAudioDmaState(hdmi_base_addr);
	        if((loop_count==data_size) && (status&HDMI_IH_AHBDMAAUD_STAT0_DONE))
                	break;
		if(status&HDMI_IH_AHBDMAAUD_STAT0_DONE)
		{
           		hdmi_dma_irq_mute(1);
			control_InterruptAudioDmaClear(hdmi_base_addr, status);             /*clearing DMA interrupt*/
	
			if((data_size -loop_count) >= DMA_PERIOD_BYTES)
			{
				audio_DmaRequestAddress(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (loop_count + DMA_PERIOD_BYTES))-1);
#ifdef HDMI_DMA_PING_PONG_BUFF
				audio_DmaRequestAddress1(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + (loop_count + DMA_PERIOD_BYTES))-1);
#endif				
				loop_count=loop_count+DMA_PERIOD_BYTES;
			}
			else
			{
				audio_DmaRequestAddress(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + data_size)-1);
#ifdef HDMI_DMA_PING_PONG_BUFF
				audio_DmaRequestAddress1(hdmi_base_addr, hw_buffer + loop_count, (hw_buffer + data_size)-1);
#endif			
				loop_count=data_size;
			}
			audio_DmaStartRead(hdmi_base_addr);	
			hdmi_dma_irq_mute(0);			
                }  /*end of while(status&HDMI_IH_AHBDMAAUD_STAT0_DONE)*/
		else
		  {
		   if (status)  
			 	{
					dbg("Got some other DMA interrupts as well %x\n", status);
		     			control_InterruptAudioDmaClear(hdmi_base_addr, status);             /*clearing DMA interrupt*/				     
				}
		  }
		if (ctrlc ())
			{
                        	printf("Playback Interrupted\n");
                        	return 0;
                        }


				
		}// end of while(1)

	hdmi_stop();
	printf("HDMI audio playback completed\n");
	return 0;

}






