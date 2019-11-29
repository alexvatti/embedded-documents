/***********************************************************************
 *
 * (C) Copyright 2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * PS/2 keyboard driver
 *
 * Originally from linux source (drivers/char/pc_keyb.c)
 *
 ***********************************************************************/

#include <common.h>

#include <keyboard.h>
#include <asm/arch/shikhara_kmi_pc_keyb.h>
#include <asm/arch/shikhara_kmi.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_gic.h>

#undef KBG_DEBUG

#ifdef KBG_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif


/* global variables */
unsigned int buffer_in[KMI_BUFFER_SIZE];
unsigned int buffer_out[KMI_BUFFEROUT_SIZE];
int read_pos_in, write_pos_in;
int read_pos_out, write_pos_out;
unsigned char override_out;  /* code to send immediately - e.g. resent characters */
unsigned int awaiting_ack;  /* flag to say we're waiting for the previous code to ACK */
unsigned int receive_disabled, transmit_disabled;  /* interrupt flags */

/* handle_kmi_event() => KMI interrupt handler
 * This reads the keyboard status, and performs
 * transmission or reception of data.
 *
 */
#ifdef KMI_DRIVER_INTERRUPT_MODE

void handle_kmi_event(void)
{
	unsigned int c;
   	int n;

    /* receive interrupt */
    if (kbd_read_status() & KMI_STAT_RXFULL)
    {
        int par, temp;

        /* check for buffer full condition */
        if (write_pos_in == ((read_pos_in - 1) & KMI_BUFFER_MASK))
        {
            /*disable the receive interrupt and write the flag */
		kbd_write(KMI_CR, 0xEF & kbd_read(KMI_CR));
            receive_disabled = TRUE;
            return ;
        }

        /* read the character */
		temp = c = kbd_read(KMI_DATA) & 0xFF;

        /* check parity */
        par = (kbd_read_status() & KMI_STAT_RX_PARITY) ? 1 : 0;
        for (n = 8; n > 0; n--)
        {
            par ^= (temp & 0x01);
            temp = temp >> 1;
        }
        if (!par)
        {
            /* parity failed - request retransmission */
            if (!(kbd_read_status() & KMI_STAT_TXBUSY))
            {
                /* send a retransmit command(0xFE) if we can */
			kbd_write(KMI_DATA, KBD_REPLY_RESEND);
            }
            else
            {
                /* otherwise put it in the override buffer */
                override_out = KBD_REPLY_RESEND;
            }
            return;
        }
    

        /* look for generic (mouse & keyb) commands */
        if (awaiting_ack)
        {
            switch (c)
            {
                case KBD_REPLY_ACK:
                    /* acknowledge */
                    if (awaiting_ack)
                    {
                        awaiting_ack = FALSE;
                        read_pos_out = (read_pos_out + 1) & KMI_BUFFEROUT_MASK;

                        /* re-enable transmit interrupt for the next character */
                        if (transmit_disabled)
                        {
                            transmit_disabled = FALSE;
				kbd_write(KMI_CR, kbd_read(KMI_CR) | 0x08);
                        }
                    }
                    return;

                case KBD_REPLY_RESEND:
                    /* resend request */
                    if (read_pos_out != write_pos_out)
                    {
                        if (!(kbd_read_status() & KMI_STAT_TXBUSY))
                        {
                            /* resend immediately if we can */
				kbd_write(KMI_DATA, buffer_out[read_pos_out]);
                        }
                        else
                        {
                            /* otherwise put it in the override buffer */
                            override_out = buffer_out[read_pos_out];
                        }
                    }
                    return;
            }
        }
				handle_scancode(c);
				return;
       

        /* no processing was done on byte */
        /* just write it into the buffer */
       	buffer_in[write_pos_in] = c;
        write_pos_in = (write_pos_in + 1) & KMI_BUFFER_MASK;
    }

    /* transmit interrupt */
    if (kbd_read_status() & KMI_STAT_TXEMPTY)
    {
        /* check for immediate transmission */
        if (override_out)
        {
		kbd_write(KMI_DATA, override_out);
            override_out = '\0';
        }

        /* check for buffer empty condition */
        if ((read_pos_out == write_pos_out) || (awaiting_ack))
        {
            /* disable the transmit interrupt and write flag */
		kbd_write(KMI_CR, kbd_read(KMI_CR) & 0xF7);
            transmit_disabled = TRUE;
            return;
        }

        /* read character from buffer */
        c = buffer_out[read_pos_out];
		kbd_write(KMI_DATA, c & 0xFF);
        awaiting_ack = TRUE;
    }
}

#endif



 int kbd_read_data(void)
{
	int val;
	unsigned char status;

	val = -1;
	status = kbd_read_status();
	if (status & KMI_STAT_RXFULL) {
		val = kbd_read_input();
		if (status & KMI_STAT_RX_PARITY)
			val = -2;
	}
	return val;
}

 int kbd_wait_for_input(void)
{
	unsigned long timeout;
	int val;

	timeout = KBD_TIMEOUT;
	val=kbd_read_data();
	while(val < 0) {
		if(timeout--==0)
			return -1;
		udelay(1000);
		val=kbd_read_data();
	}
	return val;
}


static int kb_wait(void)
{
	unsigned long timeout = KBC_TIMEOUT * 10;

	do {
		unsigned char status = 	kbd_read_status();			/*handle_kmi_event();*/
		if (!(status & KMI_STAT_TXBUSY))
			return 0; /* ok */
		udelay(1000);
		timeout--;
	} while (timeout);
	return 1;
}

static void kbd_write_command_w(int data)
{
	if(kb_wait())
		PRINTF("timeout in kbd_write_command_w\n");
	kbd_write_output(data);
}





static char * kbd_initialize(void)
{  
	
	int status;

	
	/*
	 * Reset keyboard. If the read times out
	 * then the assumption is that no keyboard is
	 * plugged into the machine.
	 * This defaults the keyboard to scan-code set 2.
	 *
	 * Set up to try again if the keyboard asks for RESEND.
	 */
	/* ???? Reset for keyboard was disabled because it is not
	   working in FPGA  
	 */
#ifdef CONFIG_SHIKHARA_ASIC
	do {
		kbd_write_output(KBD_CMD_RESET);
		printf("Resting KBD... \n");
		status = kbd_wait_for_input();
		printf("Status : %x  ", status);
		if (status == KBD_REPLY_ACK)
			printf("RESET DONE FOR KEYBOARD\n");
			break;
		if (status != KBD_REPLY_RESEND) {
			PRINTF("status: %X\n",status);
			return "Kbd:   reset failed, no ACK";
		}
	} while (1);

	/* checking for ack of power on reset*/
	if (kbd_wait_for_input() != KBD_REPLY_POR)
		return "Kbd:   reset failed, no POR";
	printf("POR DONE FOR KEYBOARD\n");

	kbd_write_output(KBD_CMD_SET_RATE);
		if (kbd_wait_for_input() != KBD_REPLY_ACK)
			return "Kbd:   Set rate: no ACK";
	kbd_write_output(0x00);
		if (kbd_wait_for_input() != KBD_REPLY_ACK)
			return "Kbd:   Set rate: no ACK";
#endif

	return NULL;


}
#ifdef KMI_DRIVER_INTERRUPT_MODE
static void kmi_intr(void *dev_id)
{
	handle_kmi_event();
}
#endif

void pckbd_leds(unsigned char leds)
{
	kbd_send_data(KBD_CMD_SET_LEDS);
	kbd_send_data(leds);
}

/******************************************************************
 * Init
 ******************************************************************/

int kbd_init_hw()
{
	char* result;

	result=kbd_initialize();
	if (result==NULL) {
		PRINTF("AT Keyboard initialized\n");
#ifdef KMI_DRIVER_INTERRUPT_MODE
		interrupt_handler_set(KMI_IRQ_NUM, kmi_intr);
		int_enable(KMI_IRQ_NUM);
#endif
		return 1;
	} else {
		printf("%s\n",result);
		return (-1);
		}
}


