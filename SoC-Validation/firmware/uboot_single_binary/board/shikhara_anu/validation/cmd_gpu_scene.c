/*
 *This file is a test to check the complete rendering path of MALI 400,
 *this file is valid only for the Zynq FPGA memory map of Shikhara SoC,
 *the page table for MALI MMU's have been generated for a specific RAM address
 *starting at 0x40000000 therefore the MALI data has been generated w.r.t
 *the same base address 0x40000000.
 *
 * */
#include<common.h>
#include<command.h>
#include<asm/arch/gpu_test.h>
#include <asm/io.h>
#include<shikhara_mali.h>
#include<mc2_zynq.h>
#include<Page_Table_PP0_PP1_GP_13MB.h>
#include "MaliFns.h"


//int Mali_FinishedFlag ; //MOSCHIP
//int Mali_ReturnCode   ; //MOSCHIP

volatile int rdata = 0;
int mali_flag=0,gp_frc_hang=0,plb_end=0,pp1_end_frame=0,pp0_end_frame=0;

void mmu_dump(unsigned int mmu_base)
{
	int i;
	for (i=0;i<=0x20;i+=4)
		printf("\n0x%X",read_data(mmu_base+i));
}

int gpu_scene_int_handler(int a,int b)
{
/*		printf("GP status 0x%X\nPP1 status 0x%X\nPP0 status 0x%X\n", read_data(MALI_GP_CONTR_REG_INT_STAT), read_data(MALI_PP1_INT_STATUS),read_data(MALI_PP0_INT_STATUS));
	//	printf("GP MMU status 0x%X\nPP1 MMU status 0x%X\nPP0 MMU status 0x%X\n", read_data(MALI_GP_MMU_STATUS), read_data(MALI_PP1_MMU_STATUS),read_data(MALI_PP0_MMU_STATUS));
	//	printf("GP MMU INT status 0x%X\nPP1 MMU INT status 0x%X\nPP0 MMU INT status 0x%X\n", read_data(MALI_GP_MMU_INT_STATUS), read_data(MALI_PP1_MMU_INT_STATUS),read_data(MALI_PP0_MMU_INT_STATUS));*/
	//	the following check is to identify forced hang of GP 

	if((read_data(MALI_GP_CONTR_REG_INT_STAT))&0x20){
		gp_frc_hang=1;
		write_data(MALI_GP_CONTR_REG_INT_CLEAR,0x20);
	}
	// the following check is to know when the GP PLB is processed
	if((read_data(MALI_GP_CONTR_REG_INT_STAT))&0x2){
		rdata  = read_data(MALI_GP_CONTR_REG_INT_STAT);
		mali_flag+=1;
		plb_end=1;
	        write_data(MALI_GP_CONTR_REG_INT_CLEAR,0x000003ff); // clearing all interrupts except GP_IRQ_VS_END_CMD_LIST
	}
	// the following check is to know when the PP writeback is done
	if((read_data(MALI_PP1_INT_STATUS))&0x1){
		rdata  = read_data(MALI_PP1_INT_STATUS);
		write_data(MALI_PP1_CTRL_MGMT,0x00000008);
		mali_flag+=1;
		write_data(MALI_PP1_INT_CLEAR,0x1);
		pp1_end_frame=1;
	}
	// the following check is to know when the PP writeback is done
	if((read_data(MALI_PP0_INT_STATUS))&0x1){
		rdata  = read_data(MALI_PP0_INT_STATUS);
		write_data(MALI_PP0_CTRL_MGMT,0x00000008);	
		mali_flag+=1;
		write_data(MALI_PP0_INT_CLEAR,0x1);
		pp0_end_frame=1;
	} 
return 0;
}

int int_handler_GP_IRQ_PLB_END_CMD_LIST_INTR(int a,int b)
{
	rdata  = read_data(MALI_GP_CONTR_REG_INT_STAT);
}

int int_handler_PP1_END_OF_FRAME_INTR(int a, int b)
{
	rdata  = read_data(MALI_PP1_INT_STATUS);
	write_data(MALI_PP1_CTRL_MGMT,0x00000008);
}

int int_handler_PP0_END_OF_FRAME_INTR(int a, int b)
{
	rdata  = read_data(MALI_PP0_INT_STATUS);
	write_data(MALI_PP0_CTRL_MGMT,0x00000008);
}

int do_gpu_scene(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	volatile int rdata = 0;                                 
	void *srcptr;
	setup_mali_clock();
	/*reset MALI */
	writel((1<<6),SHIKHARA_SYS_CTRL_BASEADDR+0x50);
	udelay(100);
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR+0x50);

	/*loading the MALI pagetable to the predefined memory*/
	printf("\ncopying Pagetable...\n");
	memcpy(0x00040000,Page_Table_PP0_PP1_GP_13MB,sizeof(Page_Table_PP0_PP1_GP_13MB));
	printf("\ndone copying Pagetable!\n");
	/*loading the MALI data to the addresses given in the Pagetable*/
	printf("\ncopying data...\n");
	memcpy(0x00146000,mc2_zynq,sizeof(mc2_zynq));
	printf("\ndone copying data!\n");

	/*loading the MALI MMU with the Pagetable information*/
	srcptr=Mali_LdMem(0x00040000,sizeof(Page_Table_PP0_PP1_GP_13MB),0x00040000);
//	mmu_dump(MALI_GP_MMU_DTE_ADDR);
	
	/*registering interrupts*/
	interrupt_handler_set(65,gpu_scene_int_handler);
	//int_enable(65);

	printf("\nstarting test\n");
	//*GP_IRQ_PLB_END_CMD_LIST_INTR = 0;                                 
	// *PP0_END_OF_FRAME_INTR  = 0;
	// *PP1_END_OF_FRAME_INTR  = 0;

	write_data(MALI_L2_MAX_READS,0x0000001c);                  
	write_data(MALI_L2_ENABLE   ,0x00000003);                  

	printf("\nconfiguring performace counters to GP,PP0,PP1..\n");
	write_data(MALI_GP_CONTR_REG_PERF_CNT_0_ENABLE,0x00000001);                  
	write_data(MALI_GP_CONTR_REG_PERF_CNT_1_ENABLE,0x00000001);                  
	write_data(MALI_GP_CONTR_REG_PERF_CNT_0_SRC   ,0x00000001);                  
	write_data(MALI_GP_CONTR_REG_PERF_CNT_1_SRC   ,0x00000006);                  
	write_data(MALI_PP0_PERF_CNT_0_ENABLE         ,0x00000001);                  
	write_data(MALI_PP0_PERF_CNT_1_ENABLE         ,0x00000001);                  
	write_data(MALI_PP0_PERF_CNT_0_SRC            ,0x00000000);                  
	write_data(MALI_PP0_PERF_CNT_1_SRC            ,0x00000002);                  
	write_data(MALI_PP1_PERF_CNT_0_ENABLE         ,0x00000001);                  
	write_data(MALI_PP1_PERF_CNT_1_ENABLE         ,0x00000001);                  
	write_data(MALI_PP1_PERF_CNT_0_SRC            ,0x00000000); 
	write_data(MALI_PP1_PERF_CNT_1_SRC            ,0x00000002); 

	printf("configuring GP...\n");
	write_data(MALI_GP_CONTR_REG_WATCHDOG_DISABLE ,0x00000001); // disabling WDT for GP to prevent forced hang interrupts 
	write_data(MALI_GP_CONTR_REG_INT_MASK         ,0x00707bff); //GP_IRQ_VS_END_CMD_LIST interrupt disabled
	write_data(MALI_GP_CONTR_REG_INT_CLEAR        ,0x00707bff); // clearing all interrupts except GP_IRQ_VS_END_CMD_LIST
	write_data(MALI_GP_CONTR_REG_VSCL_START_ADDR  ,0x000b99c0); // setting vertex shader start virtual address  
	write_data(MALI_GP_CONTR_REG_VSCL_END_ADDR    ,0x000b9ff0); // setting vertex shader stop virtual address
	write_data(MALI_GP_CONTR_REG_PLBCL_START_ADDR ,0x000a4f80); // setting PLB start address (virtual)
	write_data(MALI_GP_CONTR_REG_PLBCL_END_ADDR   ,0x000a5450); // setting PLB stop address (virtual)
	write_data(MALI_GP_CONTR_REG_CMD              ,0x00000003); // start processing VS and PLB
	/*write_data(MALI_GP_CONTR_REG_INT_CLEAR        ,0x00003fff); // clearing all interrupts except GP_IRQ_VS_END_CMD_LIST
	write_data(MALI_GP_CONTR_REG_INT_MASK         ,0x00000be5); //GP_IRQ_VS_END_CMD_LIST interrupt disabled*/

	printf("\nconfiguring PP0..\n");
	write_data(MALI_PP0_INT_MASK                  ,0x00000fff); // all interrupts except reset completed are enabled
	write_data(MALI_PP0_INT_CLEAR                 ,0x00000fff); // clearing all interrupts that are not masked
	write_data(MALI_PP0_REND_LIST_ADDR            ,0x00c75540); 
	write_data(MALI_PP0_REND_RSW_BASE             ,0x00323e80); 
	write_data(MALI_PP0_FEATURE_ENABLE            ,0x00000022); 
	write_data(MALI_PP0_Z_CLEAR_VALUE             ,0x00ffffff); 
	write_data(MALI_PP0_ABGR_CLEAR_VALUE_0        ,0xff000000); 
	write_data(MALI_PP0_ABGR_CLEAR_VALUE_1        ,0xff000000); 
	write_data(MALI_PP0_ABGR_CLEAR_VALUE_2        ,0xff000000); 
	write_data(MALI_PP0_ABGR_CLEAR_VALUE_3        ,0xff000000); 
	write_data(MALI_PP0_FS_STACK_ADDR             ,0x00c78428); 
	write_data(MALI_PP0_FS_STACK_SIZE_AND_INIT_VAL,0x00010001); 
	write_data(MALI_PP0_ORIGIN_OFFSET_X           ,0x00000001); 
	write_data(MALI_PP0_ORIGIN_OFFSET_Y           ,0x000003bf); 
	write_data(MALI_PP0_SUBPIXEL_SPECIFIER        ,0x00000077); 
	write_data(MALI_PP0_TIEBREAK_MODE             ,0x00000001); 
	write_data(MALI_PP0_PLIST_CONFIG              ,0x10010001); 
	write_data(MALI_PP0_SCALING_CONFIG            ,0x00000c0c); 
	write_data(MALI_PP0_WB0_SOURCE_SELECT         ,0x00000002); 
	write_data(MALI_PP0_WB0_TARGET_ADDR           ,0x001ace80); 
	write_data(MALI_PP0_WB0_TARGET_PIXEL_FORMAT   ,0x00000003); 
	//write_data(MALI_PP0_WB0_TARGET_FLAGS          ,0x00000004); 
	write_data(MALI_PP0_WB0_TARGET_SCANLINE_LENGTH,0x00000190); 
	write_data(MALI_PP0_CTRL_MGMT                 ,0x00000040); 

	printf("\nconfiguring PP1..\n");
	write_data(MALI_PP1_INT_MASK                  ,0x00000fff); 
	write_data(MALI_PP1_INT_CLEAR                 ,0x00000fff); 
	write_data(MALI_PP1_REND_LIST_ADDR            ,0x00c78840); 
	write_data(MALI_PP1_REND_RSW_BASE             ,0x00323e80); 
	write_data(MALI_PP1_FEATURE_ENABLE            ,0x00000022); 
	write_data(MALI_PP1_Z_CLEAR_VALUE             ,0x00ffffff); 
	write_data(MALI_PP1_ABGR_CLEAR_VALUE_0        ,0xff000000); 
	write_data(MALI_PP1_ABGR_CLEAR_VALUE_1        ,0xff000000); 
	write_data(MALI_PP1_ABGR_CLEAR_VALUE_2        ,0xff000000); 
	write_data(MALI_PP1_ABGR_CLEAR_VALUE_3        ,0xff000000); 
	write_data(MALI_PP1_FS_STACK_ADDR             ,0x00c7b728); 
	write_data(MALI_PP1_FS_STACK_SIZE_AND_INIT_VAL,0x00010001); 
	write_data(MALI_PP1_ORIGIN_OFFSET_X           ,0x00000001); 
	write_data(MALI_PP1_ORIGIN_OFFSET_Y           ,0x000003bf); 
	write_data(MALI_PP1_SUBPIXEL_SPECIFIER        ,0x00000077); 
	write_data(MALI_PP1_TIEBREAK_MODE             ,0x00000001); 
	write_data(MALI_PP1_PLIST_CONFIG              ,0x10010001); 
	write_data(MALI_PP1_SCALING_CONFIG            ,0x00000c0c); 
	write_data(MALI_PP1_WB0_SOURCE_SELECT         ,0x00000002); 
	write_data(MALI_PP1_WB0_TARGET_ADDR           ,0x001ace80); 
	write_data(MALI_PP1_WB0_TARGET_PIXEL_FORMAT   ,0x00000003); 
	//write_data(MALI_PP1_WB0_TARGET_FLAGS          ,0x00000004); 
	write_data(MALI_PP1_WB0_TARGET_SCANLINE_LENGTH,0x00000190); 
	write_data(MALI_PP1_CTRL_MGMT                 ,0x00000040); 


	int_enable(65);
	printf("\nConfigured MALI..\n");
	while(1){
		printf("mali_flag=%d\n",mali_flag);
		if(gp_frc_hang==1){
			printf("GP status 0x%X\nPP1 status 0x%X\nPP0 status 0x%X\n", read_data(MALI_GP_CONTR_REG_INT_STAT), read_data(MALI_PP1_INT_STATUS),read_data(MALI_PP0_INT_STATUS));
			//break;
		}
		if(mali_flag)
			break;
	}

	int_disable(65);
	printf("plb_end=%d,pp1_end_frame=%d,pp0_end_frame=%d\n",plb_end,pp1_end_frame,pp0_end_frame);

	unsigned char *lcdbace =  CONFIG_HDMI_FB3_ADDR; /* HDMI Frame Buffer address:0x01200000 */
	unsigned char *malifb = 0x002f2e80; // MALI FB base address as generated by the MALI data
	int count1 = 480*3; // not sure why, but its needed to display the full image
	int i1=0;
	int i2=0;
	int i3=0;
	int i4=0;
	/*while loop to copy only BGR values to HDMI Frame Buffer*/
        while(count1--)
        {	
		i4++;
                for (;i1<720*i4;)
                {
                        for(i3=0;i3<9;i3++)
                        {
                                lcdbace[i1++]=malifb[i2++];
                                lcdbace[i1++]=malifb[i2++];
                                lcdbace[i1++]=malifb[i2++];
                                i2++;
                        }
                        i2 +=4;
                }

        }

	printf("count1= %d\n",count1);
	return 0;
}

U_BOOT_CMD(
		gpu_scene,2,0,do_gpu_scene,
		"TestGPU",
		"Simple Test Case"
	  );
