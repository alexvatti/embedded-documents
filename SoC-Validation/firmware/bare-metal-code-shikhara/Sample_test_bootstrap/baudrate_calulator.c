#include<stdio.h>
#define CONFIG_PL011_CLOCK      24000000

#define baudrate 115200
int main()
{
        unsigned int temp;
        unsigned int divider;
        unsigned int remainder;
        unsigned int fraction;

        temp = 16 * baudrate;
        divider = CONFIG_PL011_CLOCK / temp;
        remainder = CONFIG_PL011_CLOCK % temp;
        temp = (8 * remainder) / baudrate;
        fraction = (temp >> 1) + (temp & 1);

        printf("value of divider decimal=%d hexa=0x%x\n",divider,divider);
        printf("value of fraction decimal=%d hexa=0x%x\n",fraction,fraction);
return 0;
}
	


