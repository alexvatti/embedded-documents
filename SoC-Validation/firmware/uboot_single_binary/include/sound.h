/*
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

#ifndef __SOUND_H__
#define __SOUND_H__

#ifdef CONFIG_SHIKHARA 
struct wavefile {
    char    id[4];          // should always contain "RIFF"
    int32_t totallength;    // total file length minus 8
    char    wavefmt[8];     // should be "WAVEfmt "
    int32_t format;         // 16 for PCM format
    int16_t pcm;            // 1 for PCM format
    int16_t channels;       // channels
    int32_t frequency;      // sampling frequency
    int32_t bytes_per_second;
    int16_t bytes_by_capture;
    int16_t bits_per_sample;
    char    data[4];        // should always contain "data"
    int32_t bytes_in_data;
} __attribute__((__packed__));

#define PCM     0x1
#define MONO    0x2
#define STEREO  0x4
typedef struct sound_param
{
unsigned int freq;
unsigned int bps;
unsigned int rfs;
unsigned short channels;
}sound_info;

extern sound_info sound_params;
#endif
/* sound codec enum */
enum en_sound_codec {
	CODEC_WM_8994,
	CODEC_WM_8995,
};

/* sound codec enum */
enum sound_compat {
	AUDIO_COMPAT_SPI,
	AUDIO_COMPAT_I2C,
};

/* Codec information structure to store the info from device tree */
struct sound_codec_info {
	int i2c_bus;
	int i2c_dev_addr;
	enum en_sound_codec codec_type;
};

/*
 * Initialises audio sub system
 *
 * @return	int value 0 for success, -1 for error
 */
int sound_init(void);
/*
   Generates and plays the square wave sound
*/
int sound_square(unsigned int freq);

/* Genetates and plays sawtooth sound
*/
int sound_sawtooth(unsigned long duration, unsigned int freq,unsigned int channel);

/* Play a wave file*/
int sound_wave(unsigned long addr);

/* Play a sine wave*/
int sound_sine(void);

/* Play a beep sound */
int sound_beep(unsigned long duration, unsigned int freq,unsigned int channel);

/* Record a wave file */
int sound_record(unsigned long addr);
#endif  /* __SOUND__H__ */
