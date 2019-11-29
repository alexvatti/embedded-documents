/*
 * Copyright (C) 2012 Samsung Electronics
 * Rajeshwari Shinde <rajeshwari.s@samsung.com>
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

#include <common.h>
#include <command.h>
#include <sound.h>

#define DEFAULT_FREQ	48000
#define DEFAULT_DURATION	5000
#define LEFT		1
#define RIGHT		2
#define LEFT_RIGHT	3

/* globaldata */
DECLARE_GLOBAL_DATA_PTR;

/* Initilaise sound subsystem */
static int do_init(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;

	ret = sound_init();	/* Initialise the Sound Subsystem */
	if (ret) {
		printf("Initialise Audio driver failed\n");
		return CMD_RET_FAILURE;
	}

	return 0;
}
#ifdef CONFIG_HDMI
static int do_hdmi_init(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{               
        int ret;
	 ret = shikhara_hdmiaudio_init();     /* Initialising HDMI Audio  */
        if (ret) {
                printf("Initialise HDMI Audio driver failed\n");
                return CMD_RET_FAILURE;
        }       
        
        return 0;
}


/* play sound from buffer for HDMI */
static int do_hdmi_play(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        int ret = 0;
        unsigned long addr;
        uint32_t msec;
        switch (argc) {
        case 2:
		if (strncmp(argv[0],"wav",3) == 0) {
        		printf("Play Wavefile\n");
                        addr = simple_strtoul(argv[1], NULL, 16);
                        ret = hdmi_wave(addr);                                         /* Play a wavefile kept at 'addr' */
                        break;
		}

	default:
        	return cmd_usage(cmdtp);

        	}
        if (ret) {
                printf("play failed");
                return CMD_RET_FAILURE;
        }


        return 0;
	
}
#endif
/* play sound from buffer */
static int do_play(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = 0;
	unsigned long addr;
	uint32_t msec;
	switch (argc) {
        case 0:
		return cmd_usage(cmdtp);
        case 1:
		if (strncmp(argv[0],"saw",3) == 0) {
                        printf ("Play sawtooth\n");
                        ret = sound_sawtooth(DEFAULT_DURATION, DEFAULT_FREQ,LEFT_RIGHT);           /* Play a sawtooth resembling tone */
                        break;
                } else if (strncmp(argv[0],"sin",3) == 0) {
                        printf ("Play sinewave\n");
                        ret = sound_sine();                                             /* Play a sinewave resembling tone */
                        break;
                }

                return cmd_usage(cmdtp);
        case 2:
		if (strncmp(argv[0],"wav",3) == 0) {
                        printf("Play Wavefile\n");
                        addr = simple_strtoul(argv[1], NULL, 16);
                        ret = sound_wave(addr);                                         /* Play a wavefile kept at 'addr' */
                        break;
                } else if (strncmp(argv[0],"squ",3) == 0) {
                        printf("Playing Squarewave\n");
                        msec = simple_strtoul(argv[1], NULL, 10);
                        ret = sound_square(msec);
                        break;
                } else if (strncmp(argv[0],"rec",3) == 0) {
			printf("Recording START\n");
			addr = simple_strtoul(argv[1], NULL ,16);
			ret = sound_record(addr);
			break;
		}

		return cmd_usage(cmdtp);   
	
	default:	
		return cmd_usage(cmdtp);					
			
	}
	if (ret) {
		printf("play failed");
		return CMD_RET_FAILURE;
	}


	return 0;
}

static cmd_tbl_t cmd_sound_sub[] = {
	U_BOOT_CMD_MKENT(init, 0, 1, do_init, "", ""),
	U_BOOT_CMD_MKENT(sine, 2, 1, do_play, "", ""),
	U_BOOT_CMD_MKENT(saw, 2, 1, do_play, "", ""),
	U_BOOT_CMD_MKENT(square, 2, 1, do_play, "", ""),
	U_BOOT_CMD_MKENT(wave, 2, 1, do_play, "", ""),
	U_BOOT_CMD_MKENT(record, 2, 1, do_play, "", ""),
	};
#ifdef CONFIG_HDMI
static cmd_tbl_t cmd_hdmi_sub[] = {
        U_BOOT_CMD_MKENT(init, 0, 1, do_hdmi_init, "", ""),
        U_BOOT_CMD_MKENT(wave, 2, 1, do_hdmi_play, "", ""),
        };
#endif
/* process sound command */
static int do_sound(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *c;

	if (argc < 1)
		return CMD_RET_USAGE;

	/* Strip off leading 'sound' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_sound_sub[0], ARRAY_SIZE(cmd_sound_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_HDMI
/* process sound command */
static int do_hdmi(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        cmd_tbl_t *c;

        if (argc < 1)
                return CMD_RET_USAGE;

        /* Strip off leading 'sound' command argument */
        argc--;
        argv++;

        c = find_cmd_tbl(argv[0], &cmd_hdmi_sub[0], ARRAY_SIZE(cmd_hdmi_sub));

        if (c)
                return c->cmd(cmdtp, flag, argc, argv);
        else
                return CMD_RET_USAGE;
}

#endif

static int cmd_beep(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{       
        unsigned int channel;
        int ret=0;                
        switch (argc) { 
        case 0:         
        case 1: 
                channel = LEFT_RIGHT;
                break;
        case 2:
                if (strncmp(argv[1],"l",1) == 0)
                        channel = LEFT;
                else if (strncmp(argv[1],"r",1) == 0)
                        channel = RIGHT;
                else
                        channel = LEFT_RIGHT;
                break;
        default:
                return cmd_usage(cmdtp);
        }
         printf("Beep on ");
               ret = sound_beep(DEFAULT_DURATION, DEFAULT_FREQ, channel);
	       return ret;
}


U_BOOT_CMD(
	sound, 3, 1, do_sound,
	"sound sub-system",
	"init - initialise the sound driver\n"
	"sound square [msec] - play sound as square wave for 'msec' duration\n"
	"sound sine - play sound as sine\n"
	"sound saw  - play sound as sawtooth\n"
	"sound wave [addr]  - play wave file at address 'addr'\n"
	"sound record [addr] - record a wave file at address 'addr'\n"
);

U_BOOT_CMD(
        beep ,    2,    1,     cmd_beep,
        "play short beep",
        "[channel]\n"
        "    - play short beep on \"l\"eft or \"r\"ight channel"
);

#ifdef CONFIG_HDMI
U_BOOT_CMD(
        hdmi, 3, 1, do_hdmi,
        "hdmi audio sub-system",
        "init - initialise the sound driver\n"
        "hdmi wave [addr]  - play wave file at address 'addr'\n"
);

#endif
