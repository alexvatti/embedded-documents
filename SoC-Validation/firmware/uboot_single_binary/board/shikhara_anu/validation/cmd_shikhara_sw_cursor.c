#include <config.h>
#include <common.h>
#include <command.h>
#include <lcd.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_kmi.h>

#define DEBUG_CURSOR
#ifdef DEBUG_CURSOR
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#define CURSOR_WIDTH      16
#define CURSOR_HEIGHT     16

void copy_cursor_background_color(short mousex,short mousey ,uchar *bg_color_data);
void restore_cursor_background_color(short mousex,short mousey ,uchar *bg_color_data);

static void do_lcd_soft_mousecursor(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	short mousex;
        short mousey;
	short mousex_old , mousey_old ;
	int nbytes;
	uchar *bg_color_data;
        uchar buttons=0;
	vidinfo_t *panel_info;

#ifdef CONFIG_PL111
        panel_info = panel_info1;
	nbytes = (1 << LCD_BPP)/8;
#endif

#ifdef CONFIG_HDMI
	panel_info = panel_info3;
#endif

#ifdef CONFIG_DSI
	panel_info = panel_info2;
#endif

#ifdef CONFIG_PL111
	bg_color_data = (uchar *) malloc(CURSOR_WIDTH * CURSOR_HEIGHT * nbytes);
#else
	bg_color_data = (uchar *) malloc(CURSOR_WIDTH * CURSOR_HEIGHT * panel_info->vl_bpix/8);
#endif

	mouse_init();

	mousex=panel_info->vl_col/2 ;
        mousey=panel_info->vl_row/2 ;	
	
	copy_cursor_background_color(mousex, mousey , bg_color_data);
	lcd_draw_rectangle(mousex,mousey,CURSOR_WIDTH,CURSOR_HEIGHT,CONSOLE_COLOR_BLUE);
	
	mousex_old = mousex;
        mousey_old = mousey;	

	while(1)
	{
	    mouse_read(&mousex , &mousey, &buttons, 0);
            dbg("MouseX:%d, MouseY:%d, Buttons:%lx\n", mousex, mousey,buttons);	
	    
	    restore_cursor_background_color(mousex_old, mousey_old ,bg_color_data);

	     if (mousex > (panel_info->vl_col - CURSOR_WIDTH))
                mousex = panel_info->vl_col - CURSOR_WIDTH;
             if (mousex < 0)
                mousex = 0;
             if (mousey > panel_info->vl_row - CURSOR_HEIGHT) 
                mousey = panel_info->vl_row - CURSOR_HEIGHT;  
             if (mousey < 0)
                 mousey = 0;

            dbg("MouseX Adjusted :%d, MouseY Adjusted :%d, Buttons:%lx\n", mousex, mousey,buttons);	

	    copy_cursor_background_color(mousex, mousey , bg_color_data);
	    lcd_draw_rectangle(mousex,mousey,CURSOR_WIDTH,CURSOR_HEIGHT,CONSOLE_COLOR_BLUE);	
	
  		 mousex_old = mousex;
		 mousey_old = mousey;
	
	   if(ctrlc())
            {
		  printf("Ctrl c is pressed to exit from test.\n");
		  lcd_clear();
                  return;
	     }
	}
}	

void copy_cursor_background_color(short mousex,short mousey ,uchar *bg_color_data)
{
	uchar *ptr;
	int i;
	vidinfo_t *panel_info;
	int nbytes;
#ifdef CONFIG_PL111
        panel_info = panel_info1;
	nbytes = (1 << LCD_BPP)/8;
	ptr = (uchar *)(lcd_base + mousey * lcd_line_length + mousex * nbytes);
#endif

#ifdef CONFIG_HDMI
        panel_info = panel_info3;
	nbytes  = panel_info->vl_bpix/8;
	ptr = (uchar *)(lcd_base3 + mousey * lcd_line_length3 + mousex * nbytes);
#endif

#ifdef CONFIG_DSI
        panel_info = panel_info2;
	nbytes  = panel_info->vl_bpix/8;
	ptr = (uchar *)(lcd_base2 + mousey * lcd_line_length2 + mousex * nbytes);
#endif

         for(i=0 ; i< CURSOR_HEIGHT ; i++)
          {
             memcpy( bg_color_data, ptr, CURSOR_WIDTH * nbytes);
             ptr = ptr + CURSOR_WIDTH * nbytes;
             bg_color_data +=  CURSOR_WIDTH * nbytes;
	     ptr += panel_info->vl_col * nbytes - CURSOR_WIDTH * nbytes ;
          }
}


void restore_cursor_background_color(short mousex,short mousey ,uchar *bg_color_data)
{
	uchar *ptr;
        int i, nbytes ;
	 vidinfo_t *panel_info;
	
#ifdef CONFIG_PL111
        panel_info = panel_info1;
        nbytes = (1 << LCD_BPP)/8;
	ptr = (uchar *)(lcd_base + mousey * lcd_line_length + mousex * nbytes);
#endif

#ifdef CONFIG_HDMI
        panel_info = panel_info3;
        nbytes = panel_info->vl_bpix/8;
        ptr = (uchar *)(lcd_base3 + mousey * lcd_line_length3 + mousex * nbytes);
#endif

#ifdef CONFIG_DSI
        panel_info = panel_info2;
        nbytes  = panel_info->vl_bpix/8;
        ptr = (uchar *)(lcd_base2 + mousey * lcd_line_length2 + mousex * nbytes);
#endif	
 
         for(i=0 ; i< CURSOR_HEIGHT ; i++)
          {
              memcpy( ptr, bg_color_data, CURSOR_WIDTH * nbytes);
              ptr = ptr + CURSOR_WIDTH * nbytes;
              bg_color_data +=  CURSOR_WIDTH * nbytes;
              ptr += (panel_info->vl_col * nbytes - CURSOR_WIDTH * nbytes);
          }
}

	

U_BOOT_CMD(
         softcursor,   1,      1,      do_lcd_soft_mousecursor,
        "shows the software cursor with mouse movement on a particular location\n",
        "help\n"
);

