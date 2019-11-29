/*
 * Code implementation file for the GPIO (General Purpose In Out) interface.
 */
#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#include <asm/io.h>
#include <asm/arch/shikhara_gpio.h>


#define DEBUG_GPIO
#ifdef DEBUG_GPIO
#define dbg(x...)       printf(x)
#endif

const int patterns[] = { 0x000000FC, 0x000000AA, 0x00000055, 0x00000000 };

int GPIO_TEST(void)
{
    int data, loop, device;
    int failtest,i;

    failtest = FALSE;

    dbg("Please connect a loopback cable between GPIO0 and GPIO2\n");
    dbg("The test uses GPIO0 and GPIO2 to perform a loopback\n");
    dbg("test on each GPIO bit.\n");

    // Output a set of patterns to GPIO0/1/2/4 and check results
    for (device = 0; device <2; device += 2) {
        for (loop = 0; loop <1; loop++) {
            // Set GPIO0/1 or GPIO2/3 as outputs
	      writel(0xff, SHIKHARA_GPIO_BANK_A + (device * 0x1000) + GPIODIR);

            // Set GPIO0/1 or GPIO2/3 as inputs
	      writel(0x00, SHIKHARA_GPIO_BANK_A +((device ^ 0x02)* 0x1000) + GPIODIR);
              udelay(10);

            // Output pattern on GPIO0/1 or GPIO2/3
              writel(patterns[loop], SHIKHARA_GPIO_BANK_A + (device * 0x1000) + GPIODATA); 

            // Read back the pattern from GPIO0/1 or GPIO2/3
	     data = 0xFF &(readl(SHIKHARA_GPIO_BANK_A + ((device ^ 0x02)* 0x1000)+ GPIODATA));
            
		if(data != patterns[loop]) {
                dbg("Error: GPIO%d/%d bit error read:0x%02X, expected:0x%02X\n", device, (device + 2), data, patterns[loop]);
                failtest = TRUE;
            }
        }
    }
    if (!failtest)
        dbg("Passed: GPIO bit test.\n");

    if (failtest)
        return -1;
    else
        return 0;
}

int do_gpio_loopback(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  GPIO_TEST();
  return (0);
}

U_BOOT_CMD(
        gpio_loopback, 1, 1, do_gpio_loopback,
        " Simple GPIO testcase to test loopback connection",
        " "
        );

