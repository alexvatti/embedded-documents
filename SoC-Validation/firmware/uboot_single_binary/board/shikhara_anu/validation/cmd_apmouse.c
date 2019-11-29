/*
 * Copyright: 
 * ----------------------------------------------------------------
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 *   (C) COPYRIGHT 2009 ARM Limited
 *       ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 * ----------------------------------------------------------------
 * File:     apmouse.c
 * Release:  Version 1.0
 * ----------------------------------------------------------------
 * 
 */
  
/*
 * Revision $Revision: 1.1 $
 * Checkin $Date: 2017/10/06 06:48:08 $
 */

/*
 * Code implementation file for the Mouse interface.
 */

//#include "keymap.h"
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_kmi.h>

#define DEBUG_MOUSE
#ifdef DEBUG_MOUSE
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
extern void kbd_write(volatile unsigned char *base, volatile unsigned char cmd);
extern unsigned char kbd_read(volatile unsigned char *base);
extern void int_enable(int i);
int mouse_init();
int mouse_read(short *mousex , short *mousey, uchar *buttons, int repeat_flag);

int gic_check_irq(int gic){
	unsigned int x=__raw_readl(0xD46F3000 + 0xD10);
	if(0x8 && x){
	__raw_writel(x & ~(1<<4), 0xD46F3000 + 0x18C);//clearing kmi intrpt
	return TRUE;
	}
	return FALSE;
}


// Check for new mouse data, return FALSE if no data
int apMOUSE_GetData(int *data)
{
    if (*KMI_STAT & KMI_STAT_RXFULL) {
        *data = *KMI_DATA;
        
        return TRUE;
    }
    else {
        *data = 0;
        
        return FALSE;
    }
}

// Read a specified number of data bytes from the mouse
int apMOUSE_GetString(int nbytes, int timer, unsigned char databuf[])
{
    int data, count;

    count = 0;
    do {    
        if (apMOUSE_GetData(&data)) {
            databuf[count] = data;
            count++;
        }
        //apSleep(1);
	udelay(1000);
        timer--;
    } while (timer && (count < nbytes));

    if (timer)
        return TRUE;
    else
        return FALSE;
}


// Convert three mouse bytes into X,Y and button values (X and Y are 16 bit signed values)
int apMOUSE_GetXYB(unsigned char databuf[], short *mousex, short *mousey, char *buttons)
{
    int   align;
    short data;
    
    // Simple error check
    if (databuf[0] & MOUSE_EVENT) {
        *buttons = databuf[0] & 0x07;
        data = *mousex;
        data = (databuf[0] & MOUSE_XSIGN) ? (data + (0xFF00 | databuf[1])) : (data + databuf[1]);
        *mousex = data;
    
/*?? Corrected mousey value temporarly because hardware(mouse) takes negative of input value for Y-coordinate */    
        data = *mousey;
        data = (databuf[0] & MOUSE_YSIGN) ? (data - (0xFF00 | databuf[2])) : (data - databuf[2]);
        *mousey = data;
        
        return TRUE;
    }
    else {
        // Sync error, try to realign
        apMOUSE_GetData(&align);
        
        return FALSE;
    }
}


/* Mouse startup sequence is as follows,
 * Command : reset      ,enable
 * Send    : FF,--,--,--,F4,--,
 * Receive : --,FA,AA,00,--,FA,
 *
 * The mouse will then send three bytes for each event,
 * Control -> X -> Y
 *
 * The control byte format is as follows,
 * Bit 0 - Left   Button
 * Bit 1 - Right  Button
 * Bit 2 - Middle Button
 * Bit 3 - Event has occurred (always 1)
 * Bit 4 - X sign bit
 * Bit 5 - Y sign bit
 */
int apMOUSE_TEST()
{
	short  mousex,mousey;
	uchar buttons;
	printf("Please plug a Mouse into J16 (top)\n");
        printf("The test displays the mouse X,Y and button values.\n");
        printf("Use the right button to exit the test\n\n");
	if(mouse_init()<0)
		return apERR_KMI_START;

	mouse_read(&mousex,&mousey,&buttons,1);
}


int mouse_init()
{ 
    int  failtest;
    int timer ;
    char  data;	
    
    failtest = FALSE;
        

    // Set the clock rate and enable the interface
    kbd_write(KMI_CR, 0);
    kbd_write(KMI_CLKDIV, KMI_CLKDIV_DIVISOR);
    kbd_write(KMI_CR, KMI_CR_EN );
    udelay(10000);//10ms

    // Reset the mouse and check the response (clear the data register first)
        kbd_write(KMI_DATA, MOUSE_DATA_RESET);
        timer = 100;
        udelay(1000);
        while (timer-- && (data != MOUSE_DATA_ACK))
        {
		data = apKMI_GetData();
                udelay(100);
        }
        dbg("data 1 is %d\n", data);
	/*?? mouse not getting clock if it is in Tx busy state*/
	if(kbd_read(KMI_STAT) & KMI_STAT_TXBUSY)
	{
		printf("Please re-connect the mouse\n");
		return apERR_KMI_START;
	}
        timer =100;
        while (timer-- && (data != MOUSE_DATA_RTR))
        {
		data = apKMI_GetData();
                udelay(100);
        }
        dbg("data 2 is %d\n", data);
        if(timer>=0)
                dbg("reset done=%d\n",timer);
        else
        {
                failtest = TRUE;
                dbg("Mouse not responding to reset.., timer %d\n", timer);	
		return apERR_KMI_START;	
        }


    // Enable the mouse and check the response
        kbd_write(KMI_DATA, MOUSE_DATA_ENABLE);
	timer = 100;
        while (timer-- && (data != MOUSE_DATA_ACK))
        {
		data = apKMI_GetData();
                udelay(100);
        }
	  dbg("data 3 is %d\n", data);
        if(timer>=0)
                dbg("enable done=%d\n",timer);
        else
        {
                failtest = TRUE;
                dbg("Mouse not responding to enable.., timer %d\n", timer);
		return apERR_KMI_START;
        }
}

int mouse_read(short  *mousex, short *mousey, uchar *buttons, int repeat_flag)
{

    volatile int    data;
    unsigned char   databuf[8];
    int             failtest, result;

	failtest = FALSE;

    if (!failtest) {
        do {
       
            // Read three bytes from the mouse (normally done in interrupt routine)
            result = apMOUSE_GetString(3, MOUSE_IDLEMAX, databuf);
            if (result) {
                // Convert bytes to X,Y and button values (errors are not checked here)
                result = apMOUSE_GetXYB(databuf, mousex, mousey, buttons);
                printf("MouseX:%d, MouseY:%d, Buttons:%1X\n", *mousex, *mousey, *buttons);

            }


        } while (result && !((*buttons) & MOUSE_RIGHT) && repeat_flag);
        if (!result) {
            printf("Error: Mouse timeout\n");
            failtest = TRUE;
        }
    }
    
    if (failtest)
        return apERR_KMI_START;
    else
        return apERR_NONE;
}


do_apmouse(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        int status;
        status=apMOUSE_TEST();
        printf("Test Result is : %s\n",status ? "Fail" : "Success");
}






U_BOOT_CMD(
        apmouse,     1,      1,      do_apmouse,
        "Validation support to test the KMI(pl-50)controller \n",
        "The test displays the mouse X,Y and button values.\n"
	"Use the right button to exit the test"
);

