/**************************************************************************
Test Case : SP804 Dual Timer
Author    : Shabbir
Project   : SHIKHARA
Date      : 01/04/2013
***************************************************************************/
#include <common.h>
#include <command.h>
#include <asm/arch-armv7/systimer.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_map.h>

#ifdef CONFIG_DT_TEST
#define  DT0_INTERRUPT_TEST           		1
#define  DT1_INTERRUPT_TEST           		2
#define  TRUE                                   1

void DT0_handler(int,int);
void DT1_handler(int,int);
void (*f1)(int,int);

unsigned char bit32_timer_interrupt_occured = 0;

static struct systimer *dt1timer_base = (struct systimer *)SHIKHARA_DT1_BASE;
static struct systimer *dt0timer_base = (struct systimer *)SHIKHARA_DT0_BASE;

void DT0_handler(int a,int b)
{
#if 0  /*Removed DT0 Timer 0, it uses for board udelay*/
   if(readl(&dt0timer_base->timer0ris)) //reading raw interrupt status register
   {
    printf("DT0 timer's interrupt is fired of counter0\n");
    bit32_timer_interrupt_occured=1;
    writel(0x10, &dt0timer_base->timer0intclr); // clearing status register
   }
   else
#endif
   if(readl(&dt0timer_base->timer1ris)) //reading raw interrupt status register
   {
    printf("DT0 timer's interrupt is fired of counter1\n");
    bit32_timer_interrupt_occured=1;
    writel(0x10, &dt0timer_base->timer1intclr); // clearing status register
   }
}

void DT1_handler(int c,int d)
{
    if(readl(&dt1timer_base->timer0ris)) //reading raw interrupt status register
     {
       printf("DT1 timer's interrupt is fired of counter 0\n");
       bit32_timer_interrupt_occured=1;
       writel(0x10, &dt1timer_base->timer0intclr); // clearing status register
     }
     else
     {
       printf("DT1 timer's interrupt is fired of counter 1\n");
       bit32_timer_interrupt_occured=1;
       writel(0x10, &dt1timer_base->timer1intclr); // clearing status register
     }
}


int do_sysconfig_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
  uchar dt,counter;
  uchar test_flag=0; /* 1 for pass & 0 for fail */
  u32 *timer_control,*timer_load,tcon, *timer_counter;
  uchar  irq_num;
   dt     =  simple_strtoul(argv[1],NULL,10);
  counter =  simple_strtoul(argv[2],NULL,10);
  
  if(argc < 2)
  {
  printf("command usage %s\n", cmdtp->help);
  return(1);
  }
  if(dt ==0)
  {
  // static struct systimer *dt0timer_base = (struct systimer *)SHIKHARA_DT0_BASE;
   if(counter == 0) {
   /*timer_control=&dt0timer_base->timer0control;
   timer_load = &dt0timer_base->timer0load;*/
   printf("This Timer is used for Delay routines\n");
   printf("we are not doing any DT 0 counter 0 test validation\n");
   return 0;
}
   if(counter == 1){
   timer_control=&dt0timer_base->timer1control;
   timer_load = &dt0timer_base->timer1load;
   timer_counter = &dt0timer_base->timer1value;
}
   irq_num=(uchar)DT0_IRQ_NUM;
   f1=DT0_handler;
  }
  else if(dt ==1)
  {
    //static struct systimer *dt1timer_base = (struct systimer *)SHIKHARA_DT1_BASE;
    if(counter == 0){
    timer_control=&dt1timer_base->timer0control;
    timer_load = &dt1timer_base->timer0load;
    timer_counter = &dt1timer_base->timer0value;
}
    if(counter == 1){
    timer_control=&dt1timer_base->timer1control;
    timer_load = &dt1timer_base->timer1load;
    timer_counter = &dt1timer_base->timer1value;
} 
   irq_num=(uchar)DT1_IRQ_NUM;
    f1=DT1_handler;
  }
  else
  printf("command usage %s\n",cmdtp->help);
   
        // Initialise DT timer
	writel(SYSTIMER_XLOAD,timer_load);
	// setting the mode into one shot
	tcon=readl(timer_control);
	tcon |=SYSTIMER_EN | SYSTIMER_IEN | SYSTIMER_32BIT | SYSTIMER_ONESHOT; // enabling the timer
	writel(tcon,timer_control);
  	printf("DT%d Timer value :0x%x\n",dt,*timer_counter);
        int_enable(irq_num);
	interrupt_handler_set(irq_num,f1);
  	printf("DT%d Timer value :0x%x\n",dt,*timer_counter);
  while(TRUE)
 {
       if(bit32_timer_interrupt_occured)
       {
       printf(" 32 bit Timer interrupt occured\n");
       test_flag = 1;
       writel(0x0,timer_control); // clearing the timer control register results default bits are set
       int_disable(irq_num);      // disabling gic interrupt
       break;
       }
       if(ctrlc())
       {
       test_flag = 0;
       writel(0x0,timer_control); // clearing the timer control register results default bits are set
       int_disable(irq_num);      // disabling gic interrupt     
       break;
       }
  }//while

        bit32_timer_interrupt_occured = 0;	
	if(test_flag)
	{
        printf("interrupt test is pass\n");
        test_flag=0;
	}
	return 0;
}// do_sys




U_BOOT_CMD(
        sysconfig,        9,      0,      do_sysconfig_test,
        "sysconfig - Command to test system configuration registers\n",
        "       - sysconfig <sub_test_no> <count_no>\n"
  /*      "       - sysconfig <0> <0>:  32 BIT interrupt test of counter0 of Timer0\n"*/ /*Removed DT0 Timer 0, it uses for udelay*/
	"       - sysconfig <0> <1>:  32 BIT interrupt test of counter1 of Timer1\n"
	"       - sysconfig <1> <0>:  32 BIT interrupt test of counter0 of Timer0\n"
	"       - sysconfig <1> <1>:  32 BIT interrupt test of counter1 of Timer1\n"
);
#endif
