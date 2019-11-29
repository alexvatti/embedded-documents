/**************************************************************************
Test Case : SP805 Watchdog Timer
Author    : Shabbir
Project   : SHIKHARA
Date      : 09/05/2013
***************************************************************************/
#include <common.h>
#include <command.h>
#include <asm/arch-armv7/wdt.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_clock.h>

#ifdef  CONFIG_WDT_TEST          
#define  TRUE	1
#define INTERRUPT_INTERVAL  1 // Assuming Interval of 1sec
static struct wdt *wdt_base = (struct wdt *)SHIKHARA_WDT_BASE;
void watchdog_test(int, int);
unsigned char watchdog_timer_interrupt_occured = 0;
int do_wdttest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uchar test_flag=0; /* 1 for pass & 0 for fail */
	u32 wload,wtcon;

	/* Watchdog timer initialization code */

	/* calculating the count value for watchdog timer for 1msec:
	   Interrupt interval = (WdogLoad+1) x effective watchdog clock period
	   Let Interrupt Interval be for 1sec,
	   Effective watchdog clock period=1/frequency of watchdog,
	   In our case frequency of watchdog=50MHz,
	   Therefore (wdogLoad)=(Interrupt interval /effective watchdog clock period)-1
	   For 1sec,we get wdogLoad value=49999999(In Decimal) or 0x2FAF07F(In Hex)
	*/
         
	/* Count value for 1sec,as calculated above  */
	//wload = 0x2FAF07F;
 	wload = (int)((INTERRUPT_INTERVAL)/(1/(float)WDT_CLOCK))-1;// Here 1/WDT_CLOCK = Effective watchdog clock period

	/* set count value by writing to wdogload Reg */
	writel(wload, &wdt_base->wdogload);

	/* Enabling Interrupt bit of control Reg. No Prescaler or divider bits are present in this register.
	  Watchdog control Reg bits Description:0->Interrupt, 1-> Reset, [31:2]->Reserved */

	wtcon=readl(&wdt_base->wdogcontrol);
	wtcon |= WDT_INT_EN;
	//wtcon |= WDT_EN; /* Reset Enable */
        writel(wtcon, &wdt_base->wdogcontrol);
	
        /*Install WDT timer interrupt (Num 78) at GIC level and Register it */
        int_enable(WDT_IRQ_NUM);
	interrupt_handler_set(WDT_IRQ_NUM,watchdog_test);
        while(TRUE)
            {
              if(watchdog_timer_interrupt_occured)
              {
                printf(" watchdog_timer interrupt occured\n");
                test_flag = 1;
		writel(0x0,&wdt_base->wdogcontrol); // clearing the WDT control register results default bits to be set
      		int_disable(WDT_IRQ_NUM);      // Disabling the interrupt at GIC level
                break;
              }
              if(ctrlc())
              {
                test_flag = 0;
		writel(0x0,&wdt_base->wdogcontrol); // clearing the WDT control register results default bits to be set
	        int_disable(WDT_IRQ_NUM);      // Disabling the interrupt at GIC level
                break;
              }
            }

             watchdog_timer_interrupt_occured = 0;
	if(test_flag)
        {
        printf("WDT interrupt test pass\n");
        test_flag=0;
        }
	
	return 0;
}

void watchdog_test(int a,int b)
{
	u32 wtclr;
	/* WDT Interrupt Clear Reg:A write of any value to this location clears the Watchdog module interrupt, and reloads
	the counter from the value in the WdogLoad Register.*/
	wtclr=readl(&wdt_base->wdogintclr);
	wtclr |= WT_INTCLR;
	watchdog_timer_interrupt_occured=1;
	writel(wtclr,&wdt_base->wdogintclr);
	writel(0x0,&wdt_base->wdogcontrol);
	printf("watchdog fired\n");
}

U_BOOT_CMD(
        wdttest,        9,      0,      do_wdttest,
        "wdttest - Command to test WDT interrupt and reset\n",
        "       - wdttest <sub_test_no> [iteration_count] \n"
);
#endif
