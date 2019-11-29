#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <asm/arch/shikhara_hdmi_bridge.h>

/*
 * Subroutine:  do_display
 *
 * Description: display rgb, yuv444, yuv422 and yuv420 image
 *
 * Inputs:      None
 *
 * Return:      None
 */

static int do_display(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char *addr;
        if(argc != 3)
        {
                printf("error in command\n");
                printf("use : display <image_address> <rgb/yuv_444/yuv_422/yuv_420>\n");
                return 1;
        }

	addr = simple_strtoul(argv[1],NULL,16);

#ifdef CONFIG_HDMI
	shikhara_hdmi_bridge_reset();
#endif
	if(!strcmp(argv[2],"rgb"))
	{
#ifdef CONFIG_HDMI
		shikhara_hdmi_reinit(FORMAT_RGB,addr);
		shikhara_hdmi_bridge_reinit(FORMAT_RGB,addr);
		printf("displaying RGB Image in HDMI\n");
#endif
#ifdef CONFIG_DSI
		memcpy(lcd_base2, addr, panel_info2->vl_col*panel_info2->vl_row*3);
                printf("displaying RGB Image in DSI\n");
#endif
#ifdef CONFIG_PL111
		memcpy(lcd_base, addr, panel_info1->vl_col*panel_info1->vl_row*3);
		printf("displaying RGB Image in LCD (make sure lcd is working in 24bpp mode)\n");
#endif
	}
	else if(!strcmp(argv[2], "yuv_444"))
	{
#ifdef CONFIG_HDMI
		shikhara_hdmi_reinit(FORMAT_YUV_444,addr);
		shikhara_hdmi_bridge_reinit(FORMAT_YUV_444,addr);
		printf("displaying YUV444 Image in HDMI\n");
#endif
#ifdef CONFIG_DSI
                printf("displaying YUV444 Image in DSI is not supported.\n");
                printf("And we don't support for converting yuv444 to bgr/rgb.\n");
#endif
#ifdef CONFIG_PL111
		printf("displaying YUV444 Image in LCD is not supported.\n");
                printf("And we don't support for converting yuv444 to bgr/rgb.\n");
#endif
	}
	else if(!strcmp(argv[2],"yuv_422"))
	{
#ifdef CONFIG_HDMI
		shikhara_hdmi_reinit(FORMAT_YUV_422,addr);
		shikhara_hdmi_bridge_reinit(FORMAT_YUV_422,addr);
		printf("displaying YUV422 Image in HDMI\n");
#endif
#ifdef CONFIG_DSI
                printf("displaying YUV422 Image in DSI is not supported.\n");
                printf("Converting it into bgr and displaying.\n");
		convert_yuvtorbg(addr, lcd_base2, panel_info2->vl_col, panel_info2->vl_row, 24, 2);
#endif
#ifdef CONFIG_PL111
                printf("displaying YUV422 Image in LCD is not supported.\n");
                printf("Converting it into bgr and displaying (make sure lcd is working in 24bpp mode).\n");
		convert_yuvtorbg(addr, lcd_base, panel_info1->vl_col, panel_info1->vl_row, 24, 2);
#endif
	}
	else if(!strcmp(argv[2],"yuv_420"))
	{
#ifdef CONFIG_HDMI
		shikhara_hdmi_reinit(FORMAT_YUV_420,addr);
		shikhara_hdmi_bridge_reinit(FORMAT_YUV_420,addr);
		printf("displaying YUV420 Image in HDMI\n");
#endif
#ifdef CONFIG_DSI
                printf("displaying YUV420 Image in DSI is not supported.\n");
                printf("Converting it into bgr and displaying.\n");
		convert_yuvtorbg(addr, lcd_base2, panel_info2->vl_col, panel_info2->vl_row, 24, 1);
#endif
#ifdef CONFIG_PL111
                printf("displaying YUV420 Image in LCD is not supported.\n");
                printf("Converting it into bgr and displaying (make sure lcd is working in 24bpp mode).\n");
		convert_yuvtorbg(addr, lcd_base, panel_info1->vl_col, panel_info1->vl_row, 24, 1);
#endif
	}
	else
	{
		printf("Invalid option : %s\n",argv[1]);
	}

#ifdef CONFIG_HDMI
	enable_hdmi_bridge();
#endif

	printf("press CTRL+c to exit and return to RGB mode\n");
	while(1)
	{
		if (ctrlc ())
                        {
                                printf("Display Interrupted\n");
                                break;
                        }
	}

#ifdef CONFIG_HDMI
	disable_hdmi_bridge();
	shikhara_hdmi_uninit();
	enable_hdmi_bridge();
#endif
}


U_BOOT_CMD(
        display,   3,      1,      do_display,
        "display - display rgb, yuv444, yuv422 and yuv420 image",
        "Make sure the image resolution, It should match with the current resolution."
);

