//***************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : MIPI-DSI Bridge Controller
//Title           : Shikhara Display Configuration
//Project         : SHIKHARA
//Owner           : Kaushal 
//Date Created    : 7/10/2013
//**************************************************************************************
//Description : Generic Layer which decides which display interface is to be Initilized
//**************************************************************************************

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <linux/types.h>
#include <common.h>
#include <lcd.h>
#include <amba_clcd.h>

#if defined(CONFIG_LCD) 
void *lcd_base;
#endif

#if defined(CONFIG_DSI) 
void *lcd_base2;
#endif

#if 0
#if defined(CONFIG_HDMI)  
void *lcd_base3;
#endif
#endif

struct clcd_config *con;
unsigned int fb_size, fb_size2, fb_size3; 	

/* Function: 
 * lcd_enable() ---- Display interface initilization 
 * Funtion checks the availability of display interfaces (panel_info) and calls 
 * the low level initilization routine. 
 */	
void lcd_enable(void)
{
#if defined(CONFIG_PL111)
		 shikhara_pl111_lcd_init(lcd_base);
#endif

#if defined(CONFIG_DSI)
		shikhara_mipi_dsi_init(lcd_base2);
#endif
#if 0
#if defined(CONFIG_HDMI)
		shikhara_hdmi_init(lcd_base3);
#endif
#endif
}

/*Called from lcd.c(Generic)*/
#if defined(CONFIG_PL111)	
void lcd_ctrl_init(void *lcdbase)
{
	con = (struct clcd_config *) panel_info1->priv;
 	
	fb_size = panel_info1->vl_row * (panel_info1->vl_col * panel_info1->vl_bpix)/8;
	
	printf("fb_size for PL111 is 0x%x\n", fb_size);

        lcd_base = lcdbase;
	
}
#endif

#if defined(CONFIG_DSI) 
void mipi_init(void *lcdbase2, vidinfo_t *vid){
        fb_size2 = vid->vl_row * (vid->vl_col * vid->vl_bpix)/8;
        printf("fb_size for DSI is 0x %x\n", fb_size2);
	lcd_base2 = lcdbase2;
}
#endif

#if defined(CONFIG_HDMI)
void hdmi_init(void *lcdbase3, vidinfo_t *vid){
        fb_size3 = vid->vl_row * (vid->vl_col * vid->vl_bpix)/ 8;
        printf("fb_size for HDMI is 0x %x\n", fb_size3);
	lcd_base3 = lcdbase3;
}
#endif
