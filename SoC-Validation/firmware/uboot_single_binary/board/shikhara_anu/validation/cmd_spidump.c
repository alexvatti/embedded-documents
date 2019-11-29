#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_spi.h>


int do_spidump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int id=0;

	id= simple_strtoul(argv[1], NULL, 10);

	switch (id){
		case 0 :
			printf("\nSPI-0 Register Dump\n\n");
			printf(" SPCR0  :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_CR0));
			printf(" SPCR1  :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_CR1));
			printf(" SPDR   :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_DR));		
			printf(" SPSR   :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_SR));
			printf(" SPCPSR :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_CPSR));
			printf(" SPIMSC :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_IMSC));
			printf(" SPRIS  :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_RIS));
			printf(" SPMIS  :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_MIS));
			printf(" SPICR  :0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_ICR));
			printf(" SPDMACR:0x%X\n",readl(SHIKHARA_SPI_BASE0+SSP_DMACR));
			break;     
		case 1 :
                        printf("\nSPI-1 Register Dump\n\n");
			printf(" SPCR0  :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_CR0));
                        printf(" SPCR1  :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_CR1));
                        printf(" SPDR   :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_DR));
                        printf(" SPSR   :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_SR));
                        printf(" SPCPSR :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_CPSR));
                        printf(" SPIMSC :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_IMSC));
                        printf(" SPRIS  :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_RIS));
                        printf(" SPMIS  :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_MIS));
                        printf(" SPICR  :0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_ICR));
                        printf(" SPDMACR:0x%X\n",readl(SHIKHARA_SPI_BASE1+SSP_DMACR));
			break;

	}
	return 0;

}

U_BOOT_CMD(
        spi_dump, 2, 1, do_spidump,
        "SPI registers dump, give controller no",
        " "
        );

