/*
 * Just to satisfy init routines..
 */

#include <common.h>
#include <configs/anusoc.h>
#include <asm/arch/shikhara_map.h>

#define POWER_UP_REQUEST	0
#define POWER_DOWN_REQUEST	1
//BIT Position
#define PWR_DN_HDMI 		0
#define PWR_DN_DISPLAY		11
void hdmi_power_on(void)
{
	u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );                
        reg32 = (reg32 & ~((POWER_DOWN_REQUEST<<PWR_DN_HDMI) | (POWER_DOWN_REQUEST<<PWR_DN_DISPLAY))) ;
        //reg32  &= ~(POWER_DOWN_REQUEST<<PWR_DN_HDMI) ;
        writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);

}
void hdmi_power_off(void)
{
	u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );
    	reg32 |= ((POWER_DOWN_REQUEST<<PWR_DN_HDMI | POWER_DOWN_REQUEST<<PWR_DN_DISPLAY)) ;
    	//reg32 |= (POWER_DOWN_REQUEST<<PWR_DN_HDMI) ;
    	writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);
}

void display_power_on()
{
	u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );                
	//reg32 = (reg32 & ~((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY)));
	reg32 &= ~((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY) | (POWER_DOWN_REQUEST<<PWR_DN_HDMI));
	writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);
}

void display_power_off()
{
	u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );
    	//reg32 |= (POWER_DOWN_REQUEST<<PWR_DN_DISPLAY);
    	reg32 |= ((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY) | (POWER_DOWN_REQUEST<<PWR_DN_HDMI));
    	writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);
}

void csi_power_on()
{
        u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );
        //reg32 = (reg32 & ~((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY)));
        reg32 &= ~((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY));
        writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);
}

void csi_power_off()
{
        u32 reg32 = readl(SHIKHARA_SYS_CTRL_BASEADDR+0xCC );
        //reg32 |= (POWER_DOWN_REQUEST<<PWR_DN_DISPLAY);
        reg32 |= ((POWER_DOWN_REQUEST<<PWR_DN_DISPLAY) );
        writel(reg32, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);
}

