#include <common.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/sev_segment.h>

#define SEVENSEG_CONTROL      SHIKHARA_SYS_CTRL_BASEADDR+0x400

const unsigned short  HEX_DATA[] = {0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6f, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,0xFF };
				     //'0'   '1'   '2'   '3'   '4'   '5'   '6'   '7'   '8'   '9'   'a'   'b'   'c'   'd'   'e'   'f'
int seg_display( int value, int led)
{
	unsigned int i=0;
	if(value > 15){
	if(value>48 && value <=54)
	value= value-49;
	else {
		if (value >= 17 && value < 23)
		value = value -17;
		else{
		printf("please enter value between 0-F \n");	
		return -1 ;	
		}
	     }
	value= value+10;
	}
	writel(HEX_DATA[value],SEVENSEG_CONTROL);
	if(led)
	writel(((HEX_DATA[value])<< 8),SEVENSEG_CONTROL);
	udelay(500000);	
	
	return 0 ;
}

int seg_off(void )
{
	unsigned int i=0;
	writel(0x0000,SEVENSEG_CONTROL);	
	printf("Turned off the all segments \n");
	return 0 ;
}



