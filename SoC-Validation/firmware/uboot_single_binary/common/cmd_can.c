
/*
 * CAN device test command
 */
#include <common.h>
#include <command.h>
#include <can.h>
static int curr_device = 0;
static struct can_dev *c_dev;

int do_can (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct can_msg msg;
	int i,j;
	struct can_dev *dev;

	dev = find_can_device(curr_device);

	if(argc < 1){
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
		
	if (strncmp(argv[1], "list", 4) == 0) {
		can_list ();
		
		return 0;
	}

	if (strncmp(argv[1], "init", 4) == 0) {
		printf("Initialise CAN \n");

		can_init (dev, 0);
		if (!dev){
			printf("CAN Controller cannot be initialised\n");
			return 1;
			}
		
		return 0;	
	}

	 if (strncmp(argv[1], "status", 6) == 0){
		unsigned int level = 0;
		if (argc > 2)
			level = simple_strtoul (argv[2], NULL, 10);
		can_status (dev, level);

		return 0;	
	}

	 if (strncmp(argv[1], "recv", 4) == 0) {
	 	memset (&msg, 0, sizeof (msg));
		if(!(can_recv (dev, &msg))) {
			if(msg.id & CAN_EFF_FLAG)	
				printf ("<0x%x>", msg.id & CAN_EFF_MASK);
			else
				printf("<0x%x>", msg.id & CAN_SFF_MASK);

			printf (" [%d]", msg.dlc);
			
				for (i = 0; i < msg.dlc; i++)
					printf (" %02x", msg.data[i]);
			
			puts ("\n Reception OK\n");
		}
		else
			printf("Reception FAILED \n");

		return 0;
		
	} 
	 if (strncmp(argv[1], "xmit", 4) == 0) {
		memset (&msg, 0, sizeof (msg));
		msg.id = 0x123;
		
		    msg.id = simple_strtoul (argv[2], NULL, 16);
			msg.dlc = simple_strtoul (argv[3], NULL, 16);
			//printf("dlc = %d \n",msg.dlc);
		for (i = 0; argc > (4 + i); i++) {
			msg.data[i] = simple_strtoul (argv[4 + i], NULL, 16);
		}
		
		printf ("Transmitting id %#x dlc %d\n",
				msg.id, msg.dlc);

		if (can_xmit (dev, &msg))
			puts("Transmission FAILED\n");
		else
			puts("Transmission OK\n");
	
		return 0;
	} 
	 
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	

	
}

U_BOOT_CMD(
	can, 12,1, do_can,
	"can	 - CAN bus commands\n",
	"can list - List all CAN controller\n"
	"can status [level] - Status of CAN controller\n"
	"can init - Initialise CAN controller \n"
	"can xmit [id] [dlc] [d0] [d1] ... [d7] - Transmit data to other CAN device \n"
	"can recv, abort with CTRL-C - Receive data from other CAN device\n"
);

