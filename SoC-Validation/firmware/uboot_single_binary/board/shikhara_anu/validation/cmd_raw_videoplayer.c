#include <common.h>
#include <lcd.h>
#include <bmp_layout.h>
#include <command.h>
#include <asm/byteorder.h>
#include<asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_hdmi_bridge.h>

#define FRAME_ADDRESS  0x6400000
#define START_VIDEO_RAW  0x1500000 
#define START_VIDEO_FB (CONFIG_SYS_SDRAM_BASE + START_VIDEO_RAW)
#define FRAME_DDR_ADDRESS (CONFIG_SYS_SDRAM_BASE + FRAME_ADDRESS)

/*
 * Subroutine:  do_rawvideoplayer
 *
 * Description: Play rgb (raw) video, The video in real time is nothing but number 
 *              of frames. And playing video is showing some frames per second. So
 *              this code is like bmpslideshow.
 *
 * Inputs:      None
 *
 * Return:      None
 *
 * About RGB video : To create a rgb video from yuv video use yuvtobgr converter
 *                   placed at : soc/firmware/Shikhara_ANUSOC/tools/Video_Converter
 *
 */

static int do_rawvideoplayer(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int i=0,k=0,l=0,j=0,frame,m=0,n=0;
        int flen=0,fhight=0;
        int vlen=0,vhight=0;
        unsigned char *addr = FRAME_DDR_ADDRESS; //frame addr
        unsigned char *video = START_VIDEO_FB;  //video addr
        unsigned char *videoaddr;
        unsigned  bpix;
        unsigned long pwidth,phight,length;
	int rgb=1;

        if(argc != 5)
        {
                printf("error in command\n");
                printf("use : rawvideoplayer <width> <hight> <no of frames> <type-yuv420/rgb>\n");
                return 1;
        }

	if(!strcmp(argv[4],"rgb"))
	{
		rgb=1;
	}
	else if(!strcmp(argv[4],"yuv420"))
	{
		rgb=0;
	}
	else
	{
		printf("Invalid Option %s\n",argv[4]);
		return 0;
	}

#ifdef CONFIG_DSI
        pwidth = panel_info2->vl_col;
        phight = panel_info2->vl_row;
        bpix = panel_info2->vl_bpix;
#endif
#ifdef CONFIG_HDMI
        pwidth = panel_info3->vl_col;
        bpix = panel_info3->vl_bpix;
        phight = panel_info3->vl_row;
#endif

        if ( (bpix != 24) && rgb) {
               printf ("Error: %d bit/pixel mode, but we support 24  bit/pixel mode.\n",bpix);
               return 1;
        }

        vhight = simple_strtoul(argv[2],NULL,10); //video hight in lines
        frame = simple_strtoul(argv[3],NULL,10); // number of frames in video file
        fhight = phight; //frame hight in bytes

	if(rgb)
	{
        	vlen = 3*simple_strtoul(argv[1],NULL,10); //video length in bytes
        	flen = pwidth*3; //frame length in bytes

		//calculating the starting address from where the video frame will start
        	videoaddr = addr + flen * (fhight - vhight)/2 + (flen - vlen)/2;
	}
	else
	{
                vlen = simple_strtoul(argv[1],NULL,10); //video length in bytes
                flen = pwidth; //frame length in bytes

                //calculating the starting address from where the video frame will start
                videoaddr = addr;
	}
//	printf("vhight %d, frame %d, fhight %d, vlen %d, flen %d, videoaddr %d\n",vhight,frame,fhight,vlen,flen,videoaddr);
        //fill two frames with white colour.
	memset(addr, 0x7f, flen*fhight*2);

	if(rgb)
	{
		flip_to_new_fb(addr);
	}
	else
	{
#ifdef CONFIG_HDMI
	        shikhara_hdmi_bridge_reset();
        	shikhara_hdmi_reinit(FORMAT_YUV_420,addr);
	        shikhara_hdmi_bridge_reinit(FORMAT_YUV_420,addr);
	        printf("displaying YUV420 Image in HDMI\n");
	        enable_hdmi_bridge();
		printf("HDMI reinit for YUV420\n");
#endif
	}

	n=vlen*vhight;
        while(frame--)
        {
            l=0;
            //copy the frame of video in frame.
	    if(rgb)
	    {
		for(i=0;i<vhight;i++)
            	{
         		memcpy(&videoaddr[l], &video[k], vlen);
     	        	k += vlen;
                	l += flen;
            	}
	    }
	    else
	    {
                for(i=0,j=flen*fhight,m=flen*fhight+flen*fhight/4;i<vhight;i++)
                {
                        memcpy(&videoaddr[l], &video[k], vlen);
			if(i<vhight/2)
			{
				memcpy(&videoaddr[j], &video[n], vlen/2);
				memcpy(&videoaddr[m], &video[n+vlen*vhight/4], vlen/2);
			}
                        k += vlen;
                        l += flen;
			if(i<vhight/2)
			{
				j += flen/2;
				m += flen/2;
				n += vlen/2;
			}
                }
	        k += vlen*vhight/2;
	        n += vlen*vhight+vlen*vhight/4;
		//udelay(1000*10);
	    }
        }

	if(rgb==0)
	{
#ifdef CONFIG_HDMI
        	disable_hdmi_bridge();
        	shikhara_hdmi_uninit();
        	enable_hdmi_bridge();
#endif
	}
}

U_BOOT_CMD(
        rawvideoplayer,   5,      1,      do_rawvideoplayer,
        "rawvideoplayer - play rgb video",
        "For help see rawvideoplayer_help"
);


static int do_rawvideoplayer_help(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        printf("First use MMC commands to copy RGB video files in 0x%x addresses. Then run rawvideoplayer command\n",START_VIDEO_FB);
        printf("use : rawvideoplayer <width> <hight> <no of frames> <type-yuv420/rgb>\n");

}

U_BOOT_CMD(
        rawvideoplayer_help,      1,      1,      do_rawvideoplayer_help,
        "rawvideoplayer_help - help for rawvideoplayer",
        "play rgb video"
);

