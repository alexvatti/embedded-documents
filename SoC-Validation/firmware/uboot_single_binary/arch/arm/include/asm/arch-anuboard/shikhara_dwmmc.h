#include <common.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#ifndef __SHIKHARA_DWMMC_H__
#define __SHIKHARA_DWMMC_H__

#define SHIKHARA_SDMMC_DIV_RATIO_0 SHIKHARA_SYS_CTRL_BASEADDR+0x008
#define SHIKHARA_SDMMC_DIV_RATIO_1 SHIKHARA_SYS_CTRL_BASEADDR+0x009

int shikhara_dwmci_init(unsigned int base, int index);

static inline unsigned int shikhara_dwmmc_init(int index)
{
#if 0
        unsigned int base = SHIKHARA_MMC0_BASE + (0x1000 * index);
        return shikhara_dwmci_init(base, index);
#else
        unsigned int base = SHIKHARA_MMC0_BASE;
        if(index == 0 || index==1) {
        printf("SD/MMC Controller index:%d\n",index);
        printf("SD/MMC Controller base address:%x\n",base);
        return shikhara_dwmci_init(base, index);
        } else if(index==2){
         printf("SD/MMC Controller index:%d\n",index);
        printf("SD/MMC Controller base address:0x%4x\n",base+0x1000);
        return shikhara_dwmci_init(base+0x1000, index);
        } else {
        printf("Invalid index for SD/MMC Controller index:%d\n",index);
        return -1;
        }
#endif

}
#endif
