/*
 * Copyright (C) 2012 Samsung Electronics
 * R. Chandrasekar <rcsekar@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <malloc.h>
#include <common.h>
#include <asm/io.h>
#include <asm/arch/shikhara_sound.h>
#include <asm/arch/shikhara_i2s.h>
#include <i2c.h>
#include <sound.h>
#include <asm/arch/shikhara_sound.h>
#include <asm/arch/shikhara_max98089.h>

/* defines */
#define SOUND_400_HZ 400
#define SOUND_BITS_IN_BYTE 8
#define LEFT            1
#define RIGHT           2
#define LEFT_RIGHT      3
#define DATA_OFFSET	8
/* pcm values for 400hz 48k sampling rate from 1 cycle */
unsigned short sine_table_400[120] = {
	0, 1714, 3425, 5125, 6812, 8480, 10125, 11742, 13327, 14875, 16383,
	17846, 19259, 20620, 21925, 23169, 24350, 25464, 26509, 27480, 28377,
	29195, 29934, 30590, 31163, 31650, 32050, 32363, 32587, 32722, 32767,
	32722, 32587, 32363, 32050, 31650, 31163, 30590, 29934, 29195, 28377,
	27480, 26509, 25464, 24350, 23169, 21925, 20620, 19259, 17846, 16383,
	14875, 13327, 11742, 10125, 8480, 6812, 5125, 3425, 1714, 0, -1714,
	-3425, -5125, -6812, -8480, -10125, -11742, -13327, -14875, -16383,
	-17846, -19259, -20620, -21925, -23169, -24350, -25464, -26509,
	-27480, -28377, -29195, -29934, -30590, -31163, -31650, -32050,
	-32363, -32587, -32722, -32767, -32722, -32587, -32363, -32050,
	-31650, -31163, -30590, -29934, -29195, -28377, -27480, -26509,
	-25464, -24350, -23169, -21925, -20620, -19259, -17846, -16383,
	-14875, -13327, -11742, -10125, -8480, -6812, -5125, -3425, -1714
};

/* Globals */


int sound_init(void)
{
	int ret;

#ifdef CONFIG_MAXIM_CODEC
	ret = max98088_init();
	if(ret) {
		printf("ERROR: Maxim Codec Init failed\n");
		return ret;
	}
#elif CONFIG_SGTL5000_CODEC
	ret = sgtl5000_init();
	if(ret) {
		printf("ERROR: SGTL5000 Codec Init failed\n");
		return ret;
	}

#endif

#ifdef CONFIG_I2S
	ret = shikhara_i2s_tx_init();
	if (ret) {
		printf("ERROR: Failed to init I2S");
		return ret;
	}
#endif
	return ret;

}

/*
 * Generates 400hz sine wave data for 1sec
 *
 * @param samplingrate	samplinng rate of the sinewave need to be generated
 * @param data		data buffer pointer
 */
static void sound_prepare_sinewave_400hz_buffer(unsigned short *data)
{
	int freq = SOUND_400_HZ;
	int i;

	while (freq--) {
		i = ARRAY_SIZE(sine_table_400);

		for (i = 0; i < ARRAY_SIZE(sine_table_400); i++) {
			*data++ = sine_table_400[i];
			*data++ = sine_table_400[i];
		}
	}
}

int sound_sine(void)
{
	unsigned int *data;
	unsigned long data_size;
	unsigned int ret;

	/* Sine wave Buffer length computation */
	data_size = sound_params.freq * sound_params.channels; /*sampling rate * no. of channels*/
	data_size *= (sound_params.bps/8) ; //(g_i2stx_pri.bitspersample / SOUND_BITS_IN_BYTE);
	data = malloc(data_size);

	if (data == NULL) {
		debug("%s: malloc failed\n", __func__);
		return -1;
	}

	sound_prepare_sinewave_400hz_buffer((unsigned short *)data);

#ifdef CONFIG_HDMI_AUDIO
/*	ret=shikhara_hdmi_transfer_tx_data((unsigned int) data,(data_size / sizeof(int)),STEREO); 
	free(data);
	return ret;*/
#endif      
#ifdef CONFIG_I2S 
	ret = shikhara_i2s_transfer_tx_data( data,
			(data_size / sizeof(int)),MONO);
	free(data);
#endif
	return ret;
}

/*
 * Generates square wave sound data for 1 second
 *
 * @param data          data buffer pointer
 * @param size          size of the buffer
 * @param freq          frequency of the wave
 */
static void sound_prepare_buffer(unsigned short *data, int size, uint32_t freq)
{
	const int sample = 48000;
	const unsigned short amplitude = 32767; /* between 1 and 32767 */
	const int period = freq ? sample / freq : 0;
	const int half = period / 2;

	assert(freq);

	/* Make sure we don't overflow our buffer */
	if (size % 2)
		size--;

	while (size) {
		int i;
		for (i = 0; size && i < half; i++) {
			size -= 2;
			*data++ = amplitude;
			*data++ = amplitude;
		}
		for (i = 0; size && i < period - half; i++) {
			size -= 2;
			*data++ = -amplitude;
			*data++ = -amplitude;
		}
	}
}


int sound_square(uint32_t msec)
{
	unsigned int *data;
	unsigned long data_size;
	unsigned int ret = 0;

	/*Buffer length computation */
	data_size = sound_params.freq * sound_params.channels;
	data_size *= (sound_params.bps / SOUND_BITS_IN_BYTE);
	data = malloc(data_size);

	if (data == NULL) {
		debug("%s: malloc failed\n", __func__);
		return -1;
	}

	sound_prepare_buffer((unsigned short *)data,
			data_size / sizeof(unsigned short), sound_params.freq);


#ifdef CONFIG_HDMI_AUDIO
/*	while (msec >= 1000) {
		ret = shikhara_hdmi_transfer_tx_data((unsigned int)data, (data_size / sizeof(int)),STEREO);
		msec -= 1000;
	}
	if (msec) {
		unsigned long size =
			(data_size * msec) / (sizeof(int) * 1000);

		ret = shikhara_hdmi_transfer_tx_data((unsigned int)data, size,STEREO);
	}
*/
#endif
#ifdef CONFIG_I2S
	while (msec >= 1000) {
		ret = shikhara_i2s_transfer_tx_data(data, (data_size / sizeof(int)),MONO);
		msec -= 1000;
	}
	if (msec) {
		unsigned long size =
			(data_size * msec) / (sizeof(int) * 1000);

		ret = shikhara_i2s_transfer_tx_data(data, size,MONO);
	}
	free(data);
#endif

	return ret;

}


int sound_wave(unsigned long addr)
{
	int ret=0;
	unsigned int size;
	unsigned int *wave_data;
	struct wavefile *wave = (struct wavefile *)addr;
	unsigned char *wave_file = (unsigned char *)addr;
	int16_t AudioFormat,NumChannels,BlockAlign,BitsPerSample;
	int32_t SampleRate,ByteRate;
	if(!(strncmp(wave->id,"RIFF",4)) == 0 && (strncmp(wave->wavefmt,"WAVE",4) == 0)) {

		printf("ERROR: Not a wav file \n");
		return 1;

	}
	AudioFormat = wave->pcm;
	if(!(AudioFormat == PCM)){
		printf("Not a PCM Uncompressed wave file\n");
		return 1;
	}
	NumChannels = wave->channels;
	BlockAlign = wave->bytes_by_capture;
	BitsPerSample = wave->bits_per_sample;
	SampleRate = wave->frequency;
	ByteRate = wave->bytes_per_second;
	size = wave->bytes_in_data;

	printf("\t\t AudioFormat = %d\n",AudioFormat);
	printf("\t\t NumChannels = %d\n",NumChannels);
	printf("\t\t SampleRate = %d\n",SampleRate);
	printf("\t\t ByteRate = %d\n",ByteRate);
	printf("\t\t BlockAlign = %d\n",BlockAlign);
	printf("\t\t BitsPerSample = %d\n",BitsPerSample);
	printf("\t\t Size of wav file is %u \n",size);

	while(strncmp(wave_file,"data",4)){
		wave_file++;
	}
	//printf("Address = 0x%x\n",wave_file);
	wave_data= (unsigned int *)(wave_file + DATA_OFFSET);                /* Data offset starts from here */

#ifdef CONFIG_I2S
	ret = shikhara_i2s_transfer_tx_data((unsigned int *)wave_data,size,BlockAlign);
#endif
	return ret;

}

#ifdef CONFIG_HDMI
int hdmi_wave(unsigned long addr)
{
        int ret=0;
        unsigned int size;
        unsigned int *wave_data;
        struct wavefile *wave = (struct wavefile *)addr;
        unsigned char *wave_file = (unsigned char *)addr;
        int16_t AudioFormat,NumChannels,BlockAlign,BitsPerSample;
        int32_t SampleRate,ByteRate;
        if(!(strncmp(wave->id,"RIFF",4)) == 0 && (strncmp(wave->wavefmt,"WAVE",4) == 0)) {

                printf("ERROR: Not a wav file \n");
                return 1;
                        
        }               
        AudioFormat = wave->pcm;
        if(!(AudioFormat == PCM)){
                printf("Not a PCM Uncompressed wave file\n");
                return 1;
        }               
        NumChannels = wave->channels;
        BlockAlign = wave->bytes_by_capture;
        BitsPerSample = wave->bits_per_sample;
        SampleRate = wave->frequency;
        ByteRate = wave->bytes_per_second;
        size = wave->bytes_in_data;
                        
        printf("\t\t AudioFormat = %d\n",AudioFormat);                                  
        printf("\t\t NumChannels = %d\n",NumChannels);
        printf("\t\t SampleRate = %d\n",SampleRate);
        printf("\t\t ByteRate = %d\n",ByteRate);
        printf("\t\t BlockAlign = %d\n",BlockAlign);
        printf("\t\t BitsPerSample = %d\n",BitsPerSample);
        printf("\t\t Size of wav file is %u \n",size);

        while(strncmp(wave_file,"data",4)){
                wave_file++;
        }
        //printf("Address = 0x%x\n",wave_file);
        wave_data= (unsigned int *)(wave_file + DATA_OFFSET);                /* Data offset starts from here */

        ret = shikhara_hdmi_transfer_tx_data((unsigned int *)wave_data,size,BlockAlign);
        return ret;
}
#endif


static int print_wav(unsigned long addr)
{
        struct wavefile *wave = (struct wavefile *)addr;
        char *wav_file = (char *)addr;
        unsigned int size;
        int16_t AudioFormat,NumChannels,BlockAlign,BitsPerSample;
        int32_t SampleRate,ByteRate;
        if(!(strncmp(wave->id,"RIFF",4)) == 0 && (strncmp(wave->wavefmt,"WAVE",4) == 0)) {

                printf("ERROR: Not a wav file \n");
                return 1;

        }
        AudioFormat = wave->pcm;
        if(!(AudioFormat == PCM)){
                printf("Not a PCM Uncompressed wave file\n");
                return 1;
        }
        NumChannels = wave->channels;
        BlockAlign = wave->bytes_by_capture;
        BitsPerSample = wave->bits_per_sample;
        SampleRate = wave->frequency;
        ByteRate = wave->bytes_per_second;
        size = wave->bytes_in_data;
	printf("\t-------Recorded WAVE file--------\n");
        printf("\t\t AudioFormat = %d\n",AudioFormat);
        printf("\t\t NumChannels = %d\n",NumChannels);
        printf("\t\t SampleRate = %d\n",SampleRate);
        printf("\t\t ByteRate = %d\n",ByteRate);
        printf("\t\t BlockAlign = %d\n",BlockAlign);
        printf("\t\t BitsPerSample = %d\n",BitsPerSample);
        printf("\t\t Size of wav file is %u \n",size);


        while(strncmp(wav_file,"data",4)){
                wav_file++;
        }
        //printf("Address = 0x%x\n",wav_file);
           return 0;
}


int sound_record(unsigned long addr)
{
	unsigned int sample_rate;		/* Sampling frequency 8000, 48000, etc */
	unsigned int num_channels;		/* Number of channels Mono=1,Stereo=2 */
	unsigned int bits_per_sample;		/* 8 bits = 8, 16 bits = 16, etc */
	unsigned int byte_rate;			/* SampleRate * NumChannels * BytesPerSample */
	unsigned int chunk_size;		/* Size of the entire file in bytes minus 8 bytes */
	unsigned int sub_chunk1_size;		/* 16 for PCM */
	unsigned int sub_chunk2_size;		/* Number of bytes in the data */
	unsigned int audio_format;		/* PCM = 1 */
	unsigned int block_align; 		/* Number of bytes for one sample including all channels */
	unsigned int num_samples;		/* Number of samples */
	unsigned int bytes_per_sample;
	int duration = 4;			/* duration of recording (sec) */
	char *wave_file;
	unsigned short *data;
	int ret;
	wave_file = (char *)addr;		/* Typecast addr to char pointer */
	
	sample_rate = sound_params.freq;
	num_channels = sound_params.channels;
	bits_per_sample = sound_params.bps;	
	bytes_per_sample = bits_per_sample/8;	/* Bits per sample(16)/8 */
	num_samples = (sample_rate *2) * duration;	/* 2xSampling frequency * duration(sec) */
	audio_format = 1;			/* PCM */
	block_align = bytes_per_sample * num_channels;
	byte_rate = bytes_per_sample * sample_rate * num_channels;
	chunk_size = 36 +(bytes_per_sample * num_samples *num_channels);
	sub_chunk1_size = 16;
	sub_chunk2_size = num_samples * num_channels * bytes_per_sample;

	/* RIFF Header */
	strncpy(wave_file, "RIFF", 4);
	wave_file += 4;
	writel(chunk_size, wave_file);
	wave_file += 4;
	strncpy(wave_file, "WAVE", 4);
	wave_file += 4;
	
	/* fmt Subchunk */
	strncpy(wave_file, "fmt ", 4);
	wave_file +=4;
	writel(sub_chunk1_size,wave_file);
	wave_file +=4;
	writew(audio_format,wave_file);
	wave_file +=2;
	writew(num_channels,wave_file);
	wave_file +=2;
	writel(sample_rate,wave_file);
	wave_file +=4;
	writel(byte_rate,wave_file);
	wave_file +=4;
	writew(block_align,wave_file);
	wave_file +=2;
	writew(bits_per_sample,wave_file);
	wave_file +=2;
	
	/* data Subchunk */
	strncpy(wave_file, "data", 4);
	wave_file += 4;
	writel(sub_chunk2_size,wave_file);
        wave_file +=4;

	
#ifdef CONFIG_I2S
	data = (unsigned short *)(addr + 44);
	ret = shikhara_i2s_record_rx_data((unsigned short *)data, sub_chunk2_size);
#endif

	ret = print_wav(addr);
	return ret;
}
int sound_beep(unsigned long duration, unsigned int freq,unsigned int channel)
{	
#ifdef CONFIG_I2S
	return shikhara_i2s_beep(duration,freq,channel) ;
#endif
}

int sound_sawtooth(unsigned long duration, unsigned int freq,unsigned int channel)
{
#ifdef CONFIG_I2S
	return shikhara_i2s_sawtooth(duration,freq,channel);
#endif
} 
