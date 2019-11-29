#include <common.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_clock.h>
void setup_mali_clock(void){

	unsigned int gate;
#ifdef CONFIG_SHIKHARA_ASIC
	writel(0x00000000,SHIKHARA_SYS_CTRL_BASEADDR+0x98);
        udelay(100);
	writel(0x00000005,SHIKHARA_SYS_CTRL_BASEADDR+0x54);
        udelay(100);
#else
	gate=readl(SHIKHARA_SYS_CTRL_BASEADDR+0x0E);
	gate=gate&~(0x00400000);//enable Mali clock
	writel(gate,SHIKHARA_SYS_CTRL_BASEADDR+0x0E);
#endif

}
int test_gpu(int n){
	setup_mali_clock();
	writel((1<<6),SHIKHARA_SYS_CTRL_BASEADDR+0x50);
                udelay(100);
         writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR+0x50);

	gpu_main(n);
}

