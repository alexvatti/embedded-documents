#include<common.h>
#include<config.h>
#include<command.h>
#include<amba_clcd.h>
#include <lcd.h>
#include <asm/arch/shikhara_kmi.h>
#include<asm/arch/shikhara_map.h>

#define DEBUG_CURSOR
#ifdef DEBUG_CURSOR
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#define CAMERA_ICON_USECASE_DESKTOP        
#define START_CAMERA_BMP	  0x6400000	
#define SIZEOF_ONE_BMP            0x4B040        //image is 640x480x8 bit
#define DESKTOP_IMAGE             (CONFIG_SYS_SDRAM_BASE + START_CAMERA_BMP + (0*SIZEOF_ONE_BMP))
#define CAMERA_CAPTURE_IMAGE      (CONFIG_SYS_SDRAM_BASE + START_CAMERA_BMP + (1*SIZEOF_ONE_BMP))

void show_lcd_cursor(short x, short y);
static void lcd_mousecursor();

void lcd_cursor_init();
int mouse_init();
int mouse_read(short *mousex, short *mousey, uchar *buttons, int repeat_flag);


static void do_lcd_cursor(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	
	short x,y;
	x = simple_strtoul(argv[1],NULL,10);
	y = simple_strtoul(argv[2],NULL,10);
	if((x > panel_info1->vl_col) || (y > panel_info1->vl_row))
	{
		dbg("Please enter x & y value below %d & %d \n",panel_info1->vl_col,panel_info1->vl_row);
		return;
	}

	lcd_cursor_init();

	show_lcd_cursor(x, y);	
}


static void do_lcd_mousecursor(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{	
	lcd_cursor_init();

	dbg("Please plug a Mouse into J16.\n");
        dbg("The test displays the mouse X,Y and button values.\n");
        dbg("Use ctrl c to exit from test.\n\n");
        if(mouse_init()<0)
           return;
        lcd_mousecursor();
}

static void lcd_mousecursor()
{
	int  clear_flag;
	short x,y;	
	short mousex; 
	short mousey;
	short iconx=50,icony=50;
	uchar buttons=0;
	char cmd[64];

        mousex = (panel_info1->vl_col)/2; 
	mousey = (panel_info1->vl_row)/2;

#ifdef CAMERA_ICON_USECASE_DESKTOP
        sprintf(cmd,"bmp display 0x%x 0 0", DESKTOP_IMAGE);
        dbg("cmd : %s\n",cmd);
        run_command(cmd,NULL);
#endif
        while(1)
        {
          mouse_read(&mousex , &mousey, &buttons, 0);
          dbg("MouseX:%d, MouseY:%d, Buttons:%lx\n", mousex, mousey,buttons);

         if (mousex > panel_info1->vl_col)
                mousex = panel_info1->vl_col;
         if (mousex < 0)
                mousex = 0;
         if (mousey > panel_info1->vl_row)
                mousey = panel_info1->vl_row;
         if (mousey < 0)
                mousey = 0;	 
        dbg("MouseX Adjusted :%d, MouseY Adjusted :%d, Buttons:%lx\n", mousex, mousey,buttons);
	
	x = mousex;
	y = mousey;
	
	show_lcd_cursor(x, y);

         
	if(mousex<=iconx && mousey<=icony && (buttons & MOUSE_LEFT))
          {
	   #ifdef CAMERA_ICON_USECASE_DESKTOP
         	sprintf(cmd,"bmp display 0x%x 0 0", CAMERA_CAPTURE_IMAGE);
                dbg("cmd : %s\n",cmd);
                run_command(cmd,NULL);
	   #else
		show_flyer();
	   #endif
		clear_flag = 1;
	  }

        if(mousex<=iconx && mousey<=icony && (buttons & MOUSE_RIGHT) && (clear_flag))
          {
           #ifdef CAMERA_ICON_USECASE_DESKTOP
                sprintf(cmd,"bmp display 0x%x 0 0", DESKTOP_IMAGE);
                dbg("cmd : %s\n",cmd);
                run_command(cmd,NULL);
           #else
                lcd_clear();
           #endif
           }

          if(ctrlc())
	    {
	     #ifdef CAMERA_ICON_USECASE_DESKTOP
                sprintf(cmd,"bmp display 0x%x 0 0", DESKTOP_IMAGE);
                dbg("cmd : %s\n",cmd);
                run_command(cmd,NULL);
             #else
                lcd_clear();
	     #endif
		dbg("Ctrl c is pressed to exit from test.\n");
	  	return;
	     }
	 } 
}


static void do_lcd_mousecursor_help(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
       dbg("First use MMC commands to copy 2 BMP files at below addresses. Then run showlcdcursor command.\n");

       dbg("DESKTOP_IMAGE at address : 0x%x\n", DESKTOP_IMAGE);
       dbg("CAMERA_CAPTURE_IMAGE at address : 0x%x\n",CAMERA_CAPTURE_IMAGE);
}

U_BOOT_CMD(
         testcursor,   3,      1,      do_lcd_cursor,
        "shows the lcd cursor on a particular location\n",
	"help\n"
);

U_BOOT_CMD(
         showlcdcursor,   1,      1,      do_lcd_mousecursor,
        "shows the lcd cursor with mouse movement on a particular location\n",
        "help\n"
);

U_BOOT_CMD(
         showlcdcursor_help,   1,      1,      do_lcd_mousecursor_help,
        "shows the lcd cursor with mouse movement on a particular location\n",
        "help\n"
);
