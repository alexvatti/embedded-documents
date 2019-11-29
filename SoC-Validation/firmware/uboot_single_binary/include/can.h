
/*(C) Copyright 2007-2009, Wolfgang Grandegger <wg <at> denx.de>
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

#ifndef __CAN_H__
#define __CAN_H__
#include <linux/list.h>
#define CAN_XMIT_TIMEOUT_US 1000000
#define CAN_EFF_MASK  0x1FFFFFFF
#define CAN_SFF_MASK  0x000007FF

#define CAN_EFF_FLAG  0x80000000
#define CAN_RTR_FLAG  0x40000000

struct can_msg {
	u32 id;
	u8  data[8];
	u8  dlc;
	u8  flags;
};

struct can_dev {
	struct list_head link;
	int dev;
	char *name;
	void *ioaddr;
	int (*init) (struct can_dev *dev, unsigned int ibaud);
	int (*xmit) (struct can_dev *dev, struct can_msg *msg);
	int (*recv) (struct can_dev *dev, struct can_msg *msg);
	int (*status) (struct can_dev *dev, int level);
	struct can_dev *next;
};

static inline int can_status (struct can_dev *dev, int level)
{
	return dev->status (dev, level);
}

static inline int can_recv (struct can_dev *dev, struct can_msg *msg)
{
	return dev->recv (dev, msg);
}

static inline int can_xmit (struct can_dev *dev, struct can_msg *msg)
{
	return dev->xmit(dev, msg);
}

extern int can_initialize(bd_t *bis);
int board_can_init(bd_t *bis);

int can_register (struct can_dev *dev);
int can_init (struct can_dev *dev, int ibaud);
void can_list (void);
struct can_dev *find_can_device(int dev_num);
#endif /* __CAN_H__ */
