#include <keymap.h>
#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_kmi.h>
#include <asm/arch/shikhara_kmi_pc_keyb.h>
/*
 Keyboard Mouse Interface(KMI-PS/2) requires hand-shake mechanism, this mechanism involves acknowledgement(0xFA) for success,
 (0xFE) for negative ack. In this hand-shake, first controller should  reset keyboard/mouse by sending 0xFF to data register
 of KMI. Keyboard/mouse will acknowledge(reply with 0xFA) for receiving the byte, then keyboard/mouse will send 0xAA for 
 power-on-reset reply for 0xFF. 
*/


#define DEBUG_KBD
#ifdef DEBUG_KBD
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

extern void kbd_write(volatile unsigned char *base, volatile unsigned char cmd);
extern unsigned char kbd_read(volatile unsigned char *base);

int gic_check_irq1(int gic){
        unsigned int x=__raw_readl(0xD46F3000 + 0xD10);
        if(0x8 && x){
        __raw_writel(x & ~(1<<4), 0xD46F3000 + 0x18C);//clearing kmi intrpt
        return TRUE;
        }
        return FALSE;
}

// Check for new keyboard data, return FALSE if no data
int apKMI_GetData(void)
{
    int timer1 = 1000;
    while(timer1--){
    	if (kbd_read(KMI_STAT) & KMI_STAT_RXFULL)
        return kbd_read(KMI_DATA);
	udelay(50);
     }
       return apERR_KMI_START; 
}

// Holds the current keyboard status
typedef struct _KMI_State {
    int shift;
    int caps;
} KMI_State;

// Get a character from the keyboard
char apKMI_GetKey(KMI_State *kybdstate)
{
    int data, keynum, keycode, timer;

    keycode = 0;
    timer   = KMI_IDLEMAX;
    do {
        // Read data from the keyboard buffer
        data = apKMI_GetData();

        // Has a key up occurred (key released)
        if (data == KMI_DATA_KEYUP) {
            // Read the key value
            do {
                data = apKMI_GetData();
                timer = (timer) ? timer - 1 : 0;
                udelay(100);
            } while (!data && timer);

            // Was shift lifted
            if ((data == KMI_DATA_SHIFTL) || (data == KMI_DATA_SHIFTR))
                kybdstate->shift = FALSE;
            else {
                // Key number conversion
                keynum = scanmap_mode22[data];
                // Key number to key code conversion (check shift)
                if(kybdstate->shift)
                    keycode = keymap_uk2[keynum].shift;
                else
                    keycode = keymap_uk2[keynum].nomods;
            }
        }
        else if ((data == KMI_DATA_SHIFTL) || (data == KMI_DATA_SHIFTR))
            kybdstate->shift = TRUE;

        // Check timeout
        timer = (timer) ? timer - 1 : 0;
        udelay(1000);
    } while ((keycode < ' ' || keycode > '~') && timer);

    if (timer)
         return keycode;
    else
        return 0;
}

int apKMI_TEST(void)
{
	if(keyboard_init()<0)
		return apERR_KMI_START;
	keyboard_read();

}//end of apKMI_TEST

int keyboard_init()
{
	int counter=0,stat, timer;
        	
	/* Perform a simple keyboard interface which responds to standard
         * characters and shift. Read in three keys and compare values.
         */
        printf("Please plug a Keyboard into PS/2 port (bottom)\n");

        // Set the Clock rate, Enable the interface and Reset the KMI
	kbd_write(KMI_CR, 0);
      	kbd_write(KMI_CLKDIV, KMI_CLKDIV_DIVISOR);
      	kbd_write(KMI_CR, KMI_CR_EN );
      	udelay(1000); 
	kbd_write(KMI_DATA, 0xff);
        kbd_read(KMI_DATA);
	timer = 30;
	stat = apKMI_GetData();
	while (timer-- && (stat != 0xaa))
	{
			stat = apKMI_GetData();
			udelay(100);
	}
	if(timer >= 0)
	{
		dbg("reset done=%d\n",timer);
		return apERR_NONE;
	}
        else
	{
		dbg("Keyboard not responding to reset..\n");
		return apERR_KMI_START;
	}

}

int keyboard_read()
{
	uchar rx_data, key;
        KMI_State key_state;
        key_state.shift    = FALSE;
        key_state.caps     = FALSE;

        // Read from Keyboard
        printf("Please type on the Keyboard (use Space Bar to exit) : \n");
        do 
	{
            key = apKMI_GetKey(&key_state);
            if (key)
                printf ("Key : %c\n", key);
        } while ((key != 0x20));
        return apERR_NONE;


}
int do_apkybd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int status;
	status=apKMI_TEST();
	printf("Test Result is : %s",status ? "Fail" : "Success");
	return 0;

}

int do_kmi_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dbg("KMI(PL-050) Register dump\n");
	dbg("KMI_CR 0x%08x\n",readb(KMI_CR));
	dbg("KMI_STAT 0x%08x\n",readb(KMI_STAT));
	dbg("KMI_DATA 0x%08x\n",readb(KMI_DATA));
	dbg("KMI_CLKDIV 0x%08x\n",readb(KMI_CLKDIV));
	dbg("KMI_IIR 0x%08x\n",readb(KMI_IIR));
	return 0;
}

int kmi_post(void)
{
        if(keyboard_init()== -1)
        {
                dbg("Keyboard not responding...Please check PS/2 port");
                return apERR_KMI_START;
        }
        else
        {
                dbg("Keyboard/Mouse device found\n");
                return apERR_NONE;
        }
}



U_BOOT_CMD(
        apkybd,     1,      1,      do_apkybd,
	"Validation support to test the KMI(pl-50)controller \n",
	"Press any key to display on to console <apkybd><enter>\n"
);

U_BOOT_CMD(
        kmi_dump,     1,      1,      do_kmi_dump,
        "To dump KMI(pl-50)controller register set\n",
        "Press any key to display on to console <kmi_dump><enter>\n"
);


