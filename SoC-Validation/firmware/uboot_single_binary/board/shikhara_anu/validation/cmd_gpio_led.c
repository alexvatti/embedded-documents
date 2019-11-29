#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_gpio.h>


int do_gpio_led(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*gpio pin value '0' represents input and '1' represents output in direction register*/
/*glowing leds of gpio bank GPIO0 */
	int i,j;

	printf("Test is going to Toggle LEDS on PORT-0 & PORT-1 (PORT-A and PORT-B)\n");
	for(i=0; i<8; i++)	
		gpio_direction_output(SHIKHARA_GPIO_PA0 + i, 1);
		
	for(i=0; i<8; i++)
		gpio_set_value(SHIKHARA_GPIO_PA0 + i, 1);	
	
	for(i=0; i<8; i++)      
                gpio_direction_output(SHIKHARA_GPIO_PB0 + i, 1);

	for(i=0; i<8; i++)
                gpio_set_value(SHIKHARA_GPIO_PB0 + i, 1);
	
	//clearing leds
       for(j=0;j<1000;j++)
               udelay(1000);
	for(i=0; i<8; i++)
                gpio_set_value(SHIKHARA_GPIO_PA0 + i, 0);
	for(i=0; i<8; i++)
                gpio_set_value(SHIKHARA_GPIO_PB0 + i, 0);
	
	printf("Toggle LEDS on PORT-0 & PORT-1 (PORT-A and PORT-B)..Completed\n");
}



int do_gpio_output_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*gpio pin value '0' represents input and '1' represents output in direction register*/
/*glowing leds of gpio bank GPIO0 */
       int i,j;

       printf("Test is going to drive high of GPIOs on PORT-2 & PORT-3 (PORT-C and PORT-D)\n");

       for(i=0; i<8; i++)      
               gpio_direction_output(SHIKHARA_GPIO_PC0 + i, 1);
               
       for(i=0; i<8; i++)
               gpio_set_value(SHIKHARA_GPIO_PC0 + i, 1);       
       
       for(i=0; i<8; i++)      
                gpio_direction_output(SHIKHARA_GPIO_PD0 + i, 1);

       for(i=0; i<8; i++)
                gpio_set_value(SHIKHARA_GPIO_PD0 + i, 1);
       
       printf("Drive high the GPIOs on PORT-2 & PORT-3 (PORT-C and PORT-D)..Completed\n");
}
 
int do_gpio_output_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*gpio pin value '0' represents input and '1' represents output in direction register*/
/*glowing leds of gpio bank GPIO0 */
       int i,j;

       printf("Test is going to drive low of GPIOs on PORT-2 & PORT-3 (PORT-C and PORT-D)\n");

       for(i=0; i<8; i++)      
               gpio_direction_output(SHIKHARA_GPIO_PC0 + i, 1);
               
       for(i=0; i<8; i++)
               gpio_set_value(SHIKHARA_GPIO_PC0 + i, 0);       
       
       for(i=0; i<8; i++)      
                gpio_direction_output(SHIKHARA_GPIO_PD0 + i, 1);

       for(i=0; i<8; i++)
                gpio_set_value(SHIKHARA_GPIO_PD0 + i, 0);

       printf("Drive low the GPIOs on PORT-2 & PORT-3 (PORT-C and PORT-D)..Completed\n");
}

unsigned char gpio_port_read(int port)
{
    unsigned char val=0,input=0;
    int pin=0;

    if(port == 0) {
               for(pin=0;pin<8;pin++)
           val=readb(SHIKHARA_GPIO_BANK_A + (1 << (pin + 2)));
           
    }
    if(port == 1){
               for(pin=0;pin<8;pin++)
           val=readb( SHIKHARA_GPIO_BANK_B + (1 << (pin + 2)));
           
    }

    if(port == 2) {
               for(pin=0;pin<8;pin++){
          val=readb( SHIKHARA_GPIO_BANK_C + (1 << (pin + 2)));
               input |=val;
               }
       
           
    }
    if(port == 3){
               for(pin=0;pin<8;pin++){
           val=readb(SHIKHARA_GPIO_BANK_D + (1 << (pin + 2)));
               input |=val;
               }
           
    }
    return input;
    return 0;
}      
int do_gpio_test_input(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*gpio pin value '0' represents input and '1' represents output in direction register*/
/*glowing leds of gpio bank GPIO0 */
       unsigned char input1=0;
       unsigned char input2=0;

       printf("Test is going to read  GPIOs(input) on PORT-2 & PORT-3 (PORT-C and PORT-D)\n");

       writel(0x0,SHIKHARA_GPIO_BANK_C + GPIODIR);
       writel(0x0,SHIKHARA_GPIO_BANK_D + GPIODIR);

       input1=gpio_port_read(2);
        input2=gpio_port_read(3);
       
       printf("PORT-2(PORT-C) GPIO Input value=0x%x\n", input1);
       printf("PORT-3(PORT-D) GPIO Input value=0x%x\n", input2);

       printf("GPIO Input test  on PORT-2 & PORT-3 (PORT-C and PORT-D)..Completed\n");
}

U_BOOT_CMD(
        gpio_led, 1, 1, do_gpio_led,
        "Simple GPIO testcase to test led's connected through GPIO",
        "Toggle leds conneted to gpio banks GPIO-0 and GPIO-1"
        );

U_BOOT_CMD(
        gpio_output_high, 1, 1, do_gpio_output_high,
        "Simple GPIO testcase to test GPIO-2 and GPIO-3 drive high",
        "Drive gpios high connected to banks GPIO-2 and GPIO-3"
        );

U_BOOT_CMD(
        gpio_output_low, 1, 1, do_gpio_output_low,
        "Simple GPIO testcase to test GPIO-2 and GPIO-3 drive low",
        "Drive gpios low connected to banks GPIO-2 and GPIO-3"
        );


U_BOOT_CMD(
        gpio_test_input,     1,      1,      do_gpio_test_input,
        "Simple GPIO testcase to test GPIO-2 and GPIO-3 as input port",
        "Read GPIOs to banks of GPIO-2 and GPIO-3"
);
