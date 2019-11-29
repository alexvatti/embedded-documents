
/*
 * (C) Copyright 2007-2009, Wolfgang Grandegger <wg <at> denx.de>
 *
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

/*
 * CAN device interface library
 */
#include <common.h>
#include <command.h>
#include <can.h>
static struct list_head can_devices;
static int cur_num = -1;
static char *baudrates[] = { "125K", "250K", "500K" };

/* Initalise the CAN Controller Structure */
int can_initialize(bd_t *bis)
{
	INIT_LIST_HEAD (&can_devices);
	cur_num =0;
	
	if (board_can_init(bis))
		printf("Cannot Init CAN controller");

	return 0;	
}

int can_register(struct can_dev *can_dev)
{
	can_dev->dev = cur_num++;	
	INIT_LIST_HEAD (&can_dev->link);
	list_add_tail (&can_dev->link, &can_devices);


	printf ("CAN: %s at 0x%p registered\n", can_dev->name, can_dev->ioaddr);

	return 0;
}

struct can_dev *find_can_device(int dev_num)
{
	struct can_dev *m;
	struct list_head *entry;

	list_for_each(entry, &can_devices) {
		m = list_entry(entry, struct can_dev, link);
		//printf("m->dev:%d\n",m->dev);
		if (m->dev == dev_num)
			return m;
	}

	printf("CAN Device %d not found\n", dev_num);

	return NULL;
}
int can_init (struct can_dev *dev, int ibaud)
{

	printf ("Initializing CAN at 0x%p with baudrate %s\n",
		 dev->ioaddr, baudrates[ibaud]);

	dev->init(dev, ibaud);

}

int get_can_num(void)
{
	return cur_num;
}
void can_list (void)
{
	struct can_dev *dev;
	int i;

	//for (i = 0, dev = can_devs; dev; i++, dev = dev->next)
	//	printf ("CAN%d: %s at 0x%p\n", i, dev->name, dev->ioaddr);
}
