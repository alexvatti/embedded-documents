/*
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
#include <asm/io.h>
#include <malloc.h>
#include <can.h>
#include <asm/arch/shikhara_can.h>
#include <asm/arch/shikhara_map.h>
#define SHIKHARA_OCR	(CAN_OCR_MODE_NORMAL | CAN_OCR_TX0_PUSHPULL)
#define SHIKHARA_CDR	CAN_CDR_CAN_MODE
#ifdef DEBUG_CAN
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

struct can_dev can_channel[1];
/*
 * Basic functions to access registers
 */
#define shikhara_read_reg(dev, reg)			\
	readb(dev->ioaddr + reg)

#define shikhara_write_reg(dev, reg, value)			\
	writeb(value,(dev->ioaddr + reg))

/*
 * Baudrate table
 */

static u16 shikhara_btr0btr1[] = {
	0x031c,			/* 125K */
	0x011c,			/* 250K */
0x001c,			/* 500K */
};

int shikhara_init (struct can_dev *dev, unsigned int ibaud)
{
	dbg("Shikhara CAN init\n");
	int i, wait = 1000;
	u16 btr0btr1;

	/* Disable the controller's interrupts */
	shikhara_write_reg (dev, CAN_IER, 0x00);

	/* Set reset mode bit */
	shikhara_write_reg (dev, CAN_MOD, CAN_MOD_RM);

	/* Read reset mode bit, multiple tests */
	do {
		udelay (100);
		if (shikhara_read_reg (dev, CAN_MOD) & CAN_MOD_RM)
			break;
	} while (--wait);

	shikhara_write_reg (dev, CAN_CDR, SHIKHARA_CDR);	/* Selecting PeliCAN Mode */
	//shikhara_write_reg (dev, CAN_OCR, SHIKHARA_OCR);	/* This register does not exist in Opencore CAN IP */

	/* Don't care about filtering of Received messages(all received messages are accepted) */
	shikhara_write_reg (dev, CAN_AMR0, 0xFF);
	shikhara_write_reg (dev, CAN_AMR1, 0xFF);
	shikhara_write_reg (dev, CAN_AMR2, 0xFF);
	shikhara_write_reg (dev, CAN_AMR3, 0xFF);

	shikhara_write_reg (dev, CAN_RXERR, 0);
	shikhara_write_reg (dev, CAN_TXERR, 0);

	i = sizeof (shikhara_btr0btr1) / sizeof (u16);
	if (ibaud >= i)
		ibaud = i - 1;
	btr0btr1 = shikhara_btr0btr1[ibaud];
/*	shikhara_write_reg (dev, CAN_BTR0, (btr0btr1 >> 8) & 0xff);
	shikhara_write_reg (dev, CAN_BTR1, (btr0btr1 & 0xff));*/

	shikhara_write_reg (dev, CAN_BTR0, 0x45);	/* Setting the Baudrate prescaler(BRP) and Synchronization Jump width(SJW) */
        shikhara_write_reg (dev, CAN_BTR1, 0x1c);	/* Setting the Segment Timings(TSEG1,TSEG2) & Sample point 
							   http://www.esacademy.com/en/library/calculators/sja1000-timing-calculator.html
							   This link explains the configuration of timing registers for 125kbps */

	/* Clear error code capture (i.e. read it) */
	shikhara_read_reg (dev, CAN_ECC);

	/* Clear reset mode bit in SHIKHARA */
	shikhara_write_reg (dev, CAN_MOD, 0);

	return 0;
}

int shikhara_xmit (struct can_dev *dev, struct can_msg *msg)
{
	int i;
	u8 fir;

	if (msg->dlc > 8)
		msg->dlc = 8;
	fir = msg->dlc;
	
	if(msg->id & CAN_EFF_FLAG){
		fir|= CAN_FIR_EFF;
		shikhara_write_reg (dev, CAN_FIR, fir);
		shikhara_write_reg (dev, CAN_ID1, ((msg->id & 0x1fe00000) >> (5 + 16)));	/* Bits 28 to 21 of ID */	
		shikhara_write_reg (dev, CAN_ID2, ((msg->id & 0x001fe000) >> (5 + 8)));		/* Bits 20 to 13 of ID */	
		shikhara_write_reg (dev, CAN_ID3, ((msg->id & 0x00001fe0) >> 5));		/* Bits 12 to  5 of ID */
		shikhara_write_reg (dev, CAN_ID4, ((msg->id & 0x0000001f) << 3));		/* Bits  4 to  0 of ID */

		for (i = 0; i < msg->dlc; i++)
			shikhara_write_reg (dev, CAN_DATA_EFF(i),msg->data[i]);
	}
	else {
	/* Write frame information register */
		shikhara_write_reg (dev, CAN_FIR, fir);
		shikhara_write_reg (dev, CAN_ID1, (msg->id & 0x000007f8) >> 3);		/* Bits 21 to 28 of ID */
		shikhara_write_reg (dev, CAN_ID2, (msg->id & 0x00000007) << 5); 	/* Bits 18 to 20 of ID */

		for (i = 0; i < msg->dlc; i++)
			shikhara_write_reg (dev, CAN_DATA_SFF (i), msg->data[i]);
	}


	/* Push the 'send' button */
	shikhara_write_reg (dev, CAN_CMR, CAN_CMR_TR);

/* Wait some time */
	for (i = 0; i < CAN_XMIT_TIMEOUT_US; i = 10000) {
		if (shikhara_read_reg (dev, CAN_SR) & CAN_SR_TCS)		/* Tramsmission successful ? */
			return 0;
		if (ctrlc ())
			break;
		udelay (10000);
	}

	return -1;
}

int shikhara_recv (struct can_dev *dev, struct can_msg *msg)
{
	int i;
	u8 fir;

	while (!(shikhara_read_reg (dev, CAN_SR) & CAN_SR_RBS)) {
		if (ctrlc ())
			return -1;
	}

	/* Read out frame information register */
	fir = shikhara_read_reg (dev, CAN_FIR);

	/* Extract data length code */
	msg->dlc = fir & CAN_FIR_DLC_MASK;

	/* If DLC exceeds 8 bytes adjust it to 8 (for the payload size) */
	if (msg->dlc > 8)
		msg->dlc = 8;

	if (fir & CAN_FIR_EFF) {
	/* Extended format */
		msg->id = (shikhara_read_reg(dev, CAN_ID1) << (5 + 16))
			| (shikhara_read_reg(dev, CAN_ID2) << (5 + 8))
			| (shikhara_read_reg(dev, CAN_ID3) << 5)
			| (shikhara_read_reg(dev, CAN_ID4) >> 3);
		msg->id |= CAN_EFF_FLAG;
		if (!(fir & CAN_FIR_RTR)) {
                        for (i = 0; i < msg->dlc; i++)
                                msg->data[i] =
                                    shikhara_read_reg (dev, CAN_DATA_EFF (i));
                }
 
	} else {
	/* Standard format */
		msg->id = shikhara_read_reg (dev, CAN_ID1) << 3;
		msg->id |= shikhara_read_reg (dev, CAN_ID2) >> 5;

		if (!(fir & CAN_FIR_RTR)) {
			for (i = 0; i < msg->dlc; i++)
				msg->data[i] =
				    shikhara_read_reg (dev, CAN_DATA_SFF (i));
		}
}
	if (fir & CAN_FIR_RTR)
		msg->id |= CAN_RTR_FLAG;

	/* Release Receive Buffer */
	shikhara_write_reg (dev, CAN_CMR, CAN_CMR_RRB);

	return 0;
}

int shikhara_status (struct can_dev *dev, int level)
{
	dbg ("SHIKHARA at 0x%p", dev->ioaddr);

	dbg("CAN_MOD:0x%x\n",shikhara_read_reg(dev,CAN_MOD));
        dbg("CAN_CMR:0x%x\n",shikhara_read_reg(dev,CAN_CMR));
        dbg("CAN_SR:0x%x\n",shikhara_read_reg(dev,CAN_SR));
        dbg("CAN_IR:0x%x\n",shikhara_read_reg(dev,CAN_IR));
        dbg("CAN_IER:0x%x\n",shikhara_read_reg(dev,CAN_IER));
        dbg("CAN_BTR0:0x%x\n",shikhara_read_reg(dev,CAN_BTR0));
        dbg("CAN_BTR1:0x%x\n",shikhara_read_reg(dev,CAN_BTR1));
        dbg("CAN_RXERR:0x%x\n",shikhara_read_reg(dev,CAN_RXERR));
        dbg("CAN_TXERR:0x%x\n",shikhara_read_reg(dev,CAN_TXERR));
        dbg("CAN_CDR:0x%x\n",shikhara_read_reg(dev,CAN_CDR));
        dbg("CAN_ALC:0x%x\n",shikhara_read_reg(dev,CAN_ALC));
        dbg("CAN_ECC:0x%x\n",shikhara_read_reg(dev,CAN_ECC));
	
	if (level > 0) {
		int stat = shikhara_read_reg (dev, CAN_SR) & 0xff;
		dbg (", status 0x%02x", stat);
		if (stat & CAN_SR_BS)
			puts (" busoff");
		if (stat & CAN_SR_ES)
			puts (" error");
		if (stat & CAN_SR_TS)
			puts (" txing");
		if (stat & CAN_SR_RS)
			puts (" rxing");
		if (stat & CAN_SR_TCS)
			puts (" txdone");
		if (stat & CAN_SR_TBS)
			puts (" txfree");
		if (stat & CAN_SR_DOS)
			puts (" overrun");
		if (stat & CAN_SR_RBS)
			puts (" rxfull");
	}
	puts ("\n");
	if (level > 1) {
		int i;
		for (i = 0; i < SHIKHARA_SIZE; i++) {
			if ((i % 0x10) == 0)
				dbg ("\n%02x:", i);
			dbg (" %02x", shikhara_read_reg (dev, i));
		}
		puts ("\n");
	}
	return 0;
}

int shikhara_can_initialize (int num)
{
	struct can_dev *dev;
	dev = &can_channel[0];
	dev->name = "SHIKHARA_CAN";
	dev->ioaddr = (void *)SHIKHARA_CAN_BASE;
	dev->init = shikhara_init;
	dev->xmit = shikhara_xmit;
	dev->recv = shikhara_recv;
	dev->status = shikhara_status;

	can_register(dev);
	return 0;
}
