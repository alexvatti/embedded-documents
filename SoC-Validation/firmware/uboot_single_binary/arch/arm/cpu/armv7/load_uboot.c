#include <configs/anusoc.h>
#include<nand.h>
#include<common.h>
#include<part.h>
#include<mmc.h>
#include<spi_flash.h>
#include<asm/arch/shikhara_map.h>
#include<asm/arch/shikhara_clock.h>
#include<asm/gpio.h>
#include<asm/io.h>
#include<image.h>
extern int smc_init_nor(void);

extern int mmc_initialize(bd_t *bis);

extern int timer_init(void);

extern int serial_init(void);

extern int nand_read_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,u_char *buffer);

extern int mmc_read_blocks(struct mmc *mmc, void *dst, ulong start, unsigned long blkcnt);

extern struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,unsigned int max_hz, unsigned int spi_mode);

extern void mem_malloc_init(ulong start, ulong size);

#ifdef CONFIG_BL0_BUILD
	long long offset    = 0x0;
	unsigned int length = 0x8C00;
#else
	long long offset    = 0x80000;
	unsigned int length = 0x60000;
#endif

nand_info_t *nand = &nand_info[0];
unsigned char *boot_strap,*sram_addr,*nor_addr,*ddr_addr,*nor_bl1;
struct spi_flash *flash;

#ifdef CONFIG_BL1_IMAGE	
	image_header_t  *bl1_hdr;
#endif
int load_uboot(void)
{
	block_dev_desc_t *dev_desc = NULL;
  	int dev = 0;
 	ulong blk_start,blk_cnt;
 	char i;
 	ulong malloc_len;

 unsigned gpio_pins[]={SHIKHARA_GPIO_PA1,SHIKHARA_GPIO_PA2,SHIKHARA_GPIO_PA3,SHIKHARA_GPIO_PA4,SHIKHARA_GPIO_PA5,SHIKHARA_GPIO_PA6};

	ulong malloc_start;

	#ifdef CONFIG_BL0_BUILD
	#ifdef CONFIG_BL1_IMAGE	
    		bl1_hdr = (image_header_t *)(CONFIG_V7ONCHIP_SRAM_BASE);
	#endif
	#endif

   	#ifdef CONFIG_BL0_BUILD
   		writel(0x7C,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
   		malloc_start = (ulong)(CONFIG_BL0_MALLOC_START);
   		malloc_len = (ulong)(CONFIG_BL_MALLOC_LEN);
   	#else
   		writel(0x39,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
   		malloc_start = (ulong)(CONFIG_BL1_MALLOC_START);
   		malloc_len = (ulong)(CONFIG_BL_MALLOC_LEN);
   	#endif

	sram_addr = (unsigned char*)(CONFIG_V7ONCHIP_SRAM_BASE);
	nor_addr  = (unsigned char*)(CONFIG_NORFLASH0);
  	nor_bl1   = (unsigned char*) (CONFIG_NORFLASH1);
	ddr_addr  = (unsigned char*)(CONFIG_SYS_TEXT_BASE);

	 #ifdef CONFIG_BL0_BUILD
        	blk_start = 0x11;
        	blk_cnt   = 0x57;
  	#else
        	blk_start = 0x80;
        	blk_cnt   = 0x33E;
  	#endif

    	serial_init();
    	writel(0x5E,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
    	printf("after serial init\n");
    	timer_init();
	writel(0x79,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
    	printf("after timer init\n");
    	mem_malloc_init(malloc_start,malloc_len);
    	writel(0x71,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
   	printf("after malloc init\n");

   	#ifdef CONFIG_BL0_BUILD
   		gpio_direction_output(SHIKHARA_GPIO_PA1,0);
   		gpio_set_value(SHIKHARA_GPIO_PA1,0x1);
   	#else
   		writel(0xFF,(SHIKHARA_SYS_CTRL_BASEADDR+0x400));
  		gpio_direction_output(SHIKHARA_GPIO_PA2,0);
   		gpio_set_value(SHIKHARA_GPIO_PA2,0x1);
   	#endif

   	#ifdef CONFIG_DEBUG
   	#ifdef CONFIG_BL0_BUILD
   		for(i=0;i<=2;i++)
   		{	
   			gpio_direction_output(gpio_pins[i],0); //setting the output direction for gpio pins
   			gpio_set_value(gpio_pins[i],0x1);
   		}	
   	#else
   		for(i=3;i<=5;i++)
   		{
   			gpio_direction_output(gpio_pins[i],0); //setting the output direction for gpio pins
   			gpio_set_value(gpio_pins[i],0x1);
   		}
   	#endif
   	#endif

	boot_strap=(unsigned char*)(BOOT_STRAP);

  	switch(*boot_strap)
  	{
  		case 0xB:
		{
	  		printf("NAND is initializing\n");
	  		nand_init();
	  		#ifdef CONFIG_BL0_BUILD
	  			printf("copying bl1 from nand flash to sram by bl0\n");
	  			nand_read_skip_bad(nand,offset,&length,sram_addr);
   			#ifdef CONFIG_BL1_IMAGE	
          			printf("after nand read checking the bl1 image verification\n");
         			printf("%x magic\n",be32_to_cpu(bl1_hdr->ih_magic));
         			printf("%x load_addr\n",be32_to_cpu(bl1_hdr->ih_load));
          			printf("%x data_size\n",be32_to_cpu(bl1_hdr->ih_size));
          			if(be32_to_cpu(bl1_hdr->ih_magic) == IH_MAGIC && image_check_dcrc(bl1_hdr))
          				printf("bl1 image is verified with the magic number and data checksum\n");
          			else
          			{
           				printf("image is not verified and enter into failed state please reset board\n");
           				return(-1);
          			}
     			#endif
     			#else
	  			printf("copying u-boot from nand flash to ddr by bl1\n");
	 			nand_read_skip_bad(nand,offset,&length,ddr_addr);
	 			printf("After u-boot read\n");
     			#endif
         	}
          	break;
 
  		case 0x9:
	 	{
	 		 printf("NOR is initializing\n");
     			#ifdef CONFIG_BL0_BUILD
	 			 printf("copying bl1 from nor flash to sram by bl0\n");
	  			 memcpy(sram_addr,nor_addr,length);
    		        #ifdef CONFIG_BL1_IMAGE	
          			printf("after nor read checking the bl1 image verification\n");
         			printf("%x magic\n",be32_to_cpu(bl1_hdr->ih_magic));
        			printf("%x load_addr\n",be32_to_cpu(bl1_hdr->ih_load));
          			printf("%x data_size\n",be32_to_cpu(bl1_hdr->ih_size));
          			if(be32_to_cpu(bl1_hdr->ih_magic) == IH_MAGIC && image_check_dcrc(bl1_hdr))
        				printf("bl1 image is verified with the magic number and data checksum\n");
         			else
          			{
          				 printf("image is not verified and enter into failed state please reset board\n");
           				 return(-1);
          			}		
     			#endif
    		 	#else
	  			printf("copying u-boot from nor flash to ddr2 by bl1\n");
				memcpy(ddr_addr,nor_bl1,length);
	  			printf("After u-boot read\n");
	  		#endif
	 	}	
	  	break;
 
	 	case 0xA:
	 	{
			#ifdef CONFIG_BL0_BUILD
	  		mmc_initialize(0);
			

	 		dev_desc = get_dev("mmc",dev);
	        	if (dev_desc == NULL) {
        	        	puts("\n** Invalid boot device **\n");
                	break;
         		}

    			#ifdef CONFIG_BL0_BUILD
       	 			printf("copying BL1 from mmc to SRAM\n");
       			 	dev_desc->block_read(dev_desc->dev, blk_start, blk_cnt, (ulong *)CONFIG_V7ONCHIP_SRAM_BASE);
    			#ifdef CONFIG_BL1_IMAGE	
          			printf("after mmc read checking the bl1 image verification\n");
          			printf("%x magic\n",be32_to_cpu(bl1_hdr->ih_magic));
          			printf("%x load_addr\n",be32_to_cpu(bl1_hdr->ih_load));
          			printf("%x data_size\n",be32_to_cpu(bl1_hdr->ih_size));
          			if(be32_to_cpu(bl1_hdr->ih_magic) == IH_MAGIC && image_check_dcrc(bl1_hdr))
          				printf("bl1 image is verified with the magic number and data checksum\n");
          			else
          			{
           				printf("image is not verified and enter into failed state please reset board\n");
           				return(-1);
          			}	
    			#endif
   			#else
        			printf("copying u-boot from mmc to DDR2\n");
        			dev_desc->block_read(dev_desc->dev, blk_start, blk_cnt, (ulong *)CONFIG_SYS_TEXT_BASE);
	 			printf("After u-boot read\n");
   			#endif
			#endif

		}
	 	break;

  		case 0xC:
         	{
         
    			#ifdef CONFIG_BL0_BUILD
         			printf("copying bl1 from spi to sram by bl0\n");
	 	 		flash= spi_flash_probe(0, 0, SSPCLK, 0);
          			spi_flash_read(flash,offset,length,sram_addr);
   			#ifdef CONFIG_BL1_IMAGE	
          			printf("after spi read checking the bl1 image verification\n");
          			printf("%x magic\n",be32_to_cpu(bl1_hdr->ih_magic));
          			printf("%x load_addr\n",be32_to_cpu(bl1_hdr->ih_load));
          			printf("%x data_size\n",be32_to_cpu(bl1_hdr->ih_size));
          			if(be32_to_cpu(bl1_hdr->ih_magic) == IH_MAGIC && image_check_dcrc(bl1_hdr))
          				printf("bl1 image is verified with the magic number and data checksum\n");
         			 else
          			{
          				 printf("image is not verified and enter into failed state please reset board\n");
           				return(-1);
          			}
    			#endif
    			#else
          			printf("copying u-boot from spi to ddr by bl1\n");
          			flash= spi_flash_probe(0, 0, SSPCLK, 0);
          			spi_flash_read(flash,offset,length,ddr_addr);
	  			printf("After u-boot read\n");
    			#endif
          	}
  		default:         
	  		return(0); 
   	}
  	return (0);
}
                
