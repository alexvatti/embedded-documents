#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_map.h>
#include <asm/io.h>

int do_gicdump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("***********Dumping GIC Distributor Registers*************\n");
	printf("DIST_CONTROL_REG:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x000));		
	printf("INT_CNTRL_TYPE_REG:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x004));		
	printf("ENABLE_SET_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x104));		
	printf("ENABLE_SET_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x108));		
	printf("ENABLE_SET_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x10c));		
	printf("ENABLE_SET_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x110));		
	printf("ENABLE_SET_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x114));		
	printf("ENABLE_SET_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x118));		
	printf("ENABLE_SET_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x11c));		
	printf("ENABLE_SET_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x120));		
	printf("ENABLE_SET_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x124));		
	printf("ENABLE_SET_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x128));		
	printf("ENABLE_SET_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x12c));		
	printf("ENABLE_SET_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x130));		
	printf("ENABLE_SET_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x134));		
	printf("ENABLE_SET_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x138));		
	printf("ENABLE_SET_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x13c));		
	printf("ENABLE_CLR_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x184));
	printf("ENABLE_CLR_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x188));
	printf("ENABLE_CLR_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x18c));
	printf("ENABLE_CLR_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x190));
	printf("ENABLE_CLR_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x194));
	printf("ENABLE_CLR_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x198));
	printf("ENABLE_CLR_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x19c));
	printf("ENABLE_CLR_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1A0));
	printf("ENABLE_CLR_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1A4));
	printf("ENABLE_CLR_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1A8));
	printf("ENABLE_CLR_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1Ac));
	printf("ENABLE_CLR_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1B0));
	printf("ENABLE_CLR_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1B4));
	printf("ENABLE_CLR_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1B8));
	printf("ENABLE_CLR_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x1Bc));
	printf("PEND_SET_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x204));		
	printf("PEND_SET_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x208));		
	printf("PEND_SET_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x20c));		
	printf("PEND_SET_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x210));		
	printf("PEND_SET_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x214));		
	printf("PEND_SET_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x218));		
	printf("PEND_SET_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x21c));		
	printf("PEND_SET_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x220));		
	printf("PEND_SET_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x224));		
	printf("PEND_SET_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x228));		
	printf("PEND_SET_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x22c));		
	printf("PEND_SET_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x230));		
	printf("PEND_SET_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x234));		
	printf("PEND_SET_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x238));		
	printf("PEND_SET_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x23c));		
	printf("PEND_CLR_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x284));		
	printf("PEND_CLR_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x288));		
	printf("PEND_CLR_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x28c));		
	printf("PEND_CLR_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x290));		
	printf("PEND_CLR_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x294));		
	printf("PEND_CLR_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x298));		
	printf("PEND_CLR_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x29c));		
	printf("PEND_CLR_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2A0));		
	printf("PEND_CLR_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2A4));		
	printf("PEND_CLR_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2A8));		
	printf("PEND_CLR_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2Ac));		
	printf("PEND_CLR_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2B0));		
	printf("PEND_CLR_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2B4));		
	printf("PEND_CLR_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2B8));		
	printf("PEND_CLR_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x2Bc));		
	printf("ACTIVE_STAT_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x304));		
	printf("ACTIVE_STAT_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x308));		
	printf("ACTIVE_STAT_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x30c));		
	printf("ACTIVE_STAT_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x310));		
	printf("ACTIVE_STAT_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x314));		
	printf("ACTIVE_STAT_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x318));		
	printf("ACTIVE_STAT_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x31c));		
	printf("ACTIVE_STAT_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x320));		
	printf("ACTIVE_STAT_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x324));		
	printf("ACTIVE_STAT_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x328));		
	printf("ACTIVE_STAT_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x32c));		
	printf("ACTIVE_STAT_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x330));		
	printf("ACTIVE_STAT_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x334));		
	printf("ACTIVE_STAT_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x338));		
	printf("ACTIVE_STAT_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0x33c));		
	printf("INT_CONF_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc08));		
	printf("INT_CONF_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc0c));		
	printf("INT_CONF_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc10));		
	printf("INT_CONF_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc14));		
	printf("INT_CONF_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc18));		
	printf("INT_CONF_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc1c));		
	printf("INT_CONF_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc20));		
	printf("INT_CONF_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc24));		
	printf("INT_CONF_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc28));		
	printf("INT_CONF_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc2c));		
	printf("INT_CONF_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc30));		
	printf("INT_CONF_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc34));		
	printf("INT_CONF_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc38));		
	printf("INT_CONF_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc3c));		
	printf("INT_CONF_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xc40));		
	printf("SPI_STAT_REG0:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD04));		        
	printf("SPI_STAT_REG1:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD08));		        
	printf("SPI_STAT_REG2:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD0c));		        
	printf("SPI_STAT_REG3:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD10));		        
	printf("SPI_STAT_REG4:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD14));		        
	printf("SPI_STAT_REG5:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD18));		        
	printf("SPI_STAT_REG6:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD1c));		        
	printf("SPI_STAT_REG7:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD20));		        
	printf("SPI_STAT_REG8:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD24));		        
	printf("SPI_STAT_REG9:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD28));		        
	printf("SPI_STAT_REG10:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD2c));		        
	printf("SPI_STAT_REG11:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD30));		        
	printf("SPI_STAT_REG12:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD34));		        
	printf("SPI_STAT_REG13:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD38));		        
	printf("SPI_STAT_REG14:0x%x\n",readl(SHIKHARA_GIC_DISTRIBUTOR+0xD3c));		        
	
	printf("***********Dumping GIC Interface Registers*************\n");
	printf("INTERFACE_CTRL_REG:0x%x\n",readl(SHIKHARA_GIC_INTERFACE+0x000));		
	printf("PRIO_MASK_REG:0x%x\n",readl(SHIKHARA_GIC_INTERFACE+0x004));		
	printf("INT_ACK_REG:0x%x\n",readl(SHIKHARA_GIC_INTERFACE+0x00c));		
	printf("RUNNING_PRIO_REG:0x%x\n",readl(SHIKHARA_GIC_INTERFACE+0x014));		
	printf("HIGH_PEND_INT_REG:0x%x\n",readl(SHIKHARA_GIC_INTERFACE+0x018));		

}

U_BOOT_CMD(
 	gicdump, 1, 0, do_gicdump,
 	"gicdump - display GIC registers dump",
	"Display dump of GIC registers\n"
 	);
