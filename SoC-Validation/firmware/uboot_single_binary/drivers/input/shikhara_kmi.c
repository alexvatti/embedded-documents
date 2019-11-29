#include <common.h>
#include <asm/arch/shikhara_kmi_pc_keyb.h>
#include <asm/arch/shikhara_kmi.h>
/*Including  in_8 and out_8 routinues  */
#include<asm/io.h>
#include<keymap.h>
#include<keyboard.h>

typedef struct _KMI_State {
    int shift;
    int caps;
} KMI_State;

 void kbd_send_data(unsigned char data)
{
        kbd_write_output(data);
        kbd_wait_for_input();
}

void kbd_write(volatile unsigned char *base, volatile unsigned char cmd)
{
         out_8(base,cmd);
}

void kbd_write_output( unsigned char cmd)
{
        out_8(KMI_DATA, cmd);
}

unsigned char kbd_read(volatile unsigned char *base)
{
        return in_8(base);
}
unsigned char kbd_read_input(void)
{
        return(in_8(KMI_DATA));
}

unsigned char kbd_read_status(void)
{
        return(in_8(KMI_STAT));
}
int KMI_GetData(void)
{
      if (kbd_read(KMI_STAT) & KMI_STAT_RXFULL)
        return kbd_read(KMI_DATA);
    else
        return FALSE;
}

/* Test Rx_FULL status of KMI to read data from data register. If Rx_FULL is set,
 * kmi_check1() will return.
 */
void kmi_check1(void)
{
	 while(!(*KMI_STAT & KMI_STAT_RXFULL));
}

void kmi_check2(void)
{ 
/* Reading scan code from data register if Receiver buffer is full.
 * Send the data to scanmap_mode22[] to get index(keynum) of the ASCII code of key pressed from keyboard.
 * The index is passed to keymap_uk2[] to get the ASCII code which will be queued to kbd_buffer then displayed through standard input.
*/
        int keynum;
        unsigned char data,keycode;
        KMI_State keystate,*kybdstate;
        kybdstate = &keystate;
        kybdstate->shift = FALSE;
        kybdstate->caps = FALSE;

        do{
                  data = KMI_GetData();
                // Has a key up occurred (key released)

                if (data == KMI_DATA_KEYUP) {

                // Read the key value

                 do {
                        data = KMI_GetData();
                        udelay(100);
                        } while (!data);
                // Was shift lifted

                if ((data == KMI_DATA_SHIFTL) || (data == KMI_DATA_SHIFTR))
                kybdstate->shift = FALSE;

                else    {

                        // Key number conversion

                        keynum = scanmap_mode22[data];

                        // Key number to key code conversion (check shift)

                        if(kybdstate->shift)
                            keycode = keymap_uk2[keynum].shift;

                        else
                            keycode = keymap_uk2[keynum].nomods;

                         }//end of else

                  }//end of if

                else if ((data == KMI_DATA_SHIFTL) || (data == KMI_DATA_SHIFTR))

                        kybdstate->shift = TRUE;


          } while ((keycode < '\0' || keycode > '~'));

        kbd_put_queue(keycode);


}//end of kmi_check()

/*******************************init***********************/

int kmi_init(void)
        { 

	printf("kmi_init \n ");
	int data,timer = 1000;       
        kbd_write(KMI_CR, 0);
        /* Writing divisor value to KMI clock divisor register*/
        kbd_write(KMI_CLKDIV, KMI_CLKDIV_DIVISOR);
        /*Enabling KMI controller */
        kbd_write(KMI_CR, KMI_CR_EN );
        udelay(10000);
	/* Reset the keyboard and check the responce (clear KMI_DATA with read)*/
        kbd_write(KMI_DATA, KMI_DATA_RESET);
        timer       = KMI_RESET_TIME;
	data = kbd_read(KMI_DATA);
        while (timer && (KMI_GetData() != KMI_DATA_RTR)) {
            timer--;
            udelay(1000);
        }
		return 0;
 
	}
