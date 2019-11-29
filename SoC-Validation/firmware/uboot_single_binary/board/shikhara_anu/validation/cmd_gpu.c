#include<common.h>
#include<command.h>
#include<asm/arch/gpu_test.h>
#include <asm/io.h>
#include<MaliFns.h>

#define  GP_MAX_TEST            16      
#define  PP0_MAX_TEST           34
#define  PP1_MAX_TEST           35
#define  PP2_MAX_TEST           36
#define  PP3_MAX_TEST           37
#define  MP1_MAX_TEST           5
#define  MP2_MAX_TEST           8

#define GP_CONTR_REG_VERSION          MALI_BASE+0x006C
#define L2_CACHECTRL_REG_VERSION      MALI_BASE+0x1000
#define PP0_CONTR_REG_VERSION         MALI_BASE+0x8000+0x1000
#define PP1_CONTR_REG_VERSION         MALI_BASE+0x8000+0x2000+0x1000
#define PP2_CONTR_REG_VERSION         MALI_BASE+0x8000+0x4000+0x1000
#define PP3_CONTR_REG_VERSION         MALI_BASE+0x8000+0x6000+0x1000

extern int Mali_FinishedFlag ; //MOSCHIP
extern int Mali_ReturnCode   ; //MOSCHIP

void  gpu_version(void){
        printf("\n -------------------GPU(Mali-400) Version------------------ \n\n");
        printf("Geometric Processor(GP): 0x%X\n", readl(GP_CONTR_REG_VERSION));
        printf("L2 Cache Controller    : 0x%X\n", readl(L2_CACHECTRL_REG_VERSION));
        printf("Pixel Processor-0(PP0) : 0x%X\n", readl(PP0_CONTR_REG_VERSION));
        printf("Pixel Processor-1(PP1) : 0x%X\n", readl(PP1_CONTR_REG_VERSION));
        printf("Pixel Processor-2(PP2) : 0x%X\n", readl(PP2_CONTR_REG_VERSION));
        printf("Pixel Processor-3(PP3) : 0x%X\n", readl(PP3_CONTR_REG_VERSION));
}

int do_gputest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int n=0;
	char *cmd;
#ifdef CONFIG_SHIKHARA_GPU

	 Mali_FinishedFlag = 0; //MOSCHIP
	 Mali_ReturnCode   = 0; //MOSCHIP

	if(argc < 2)
           goto usage;
	
	cmd = argv[1];
        if (strcmp(cmd,"version")==0){
            gpu_version();
            return 0;
        }
	if (strcmp(cmd,"proxycon")==0 )
	{
		test_gpu(0);
		return 0;
	}
	n = simple_strtoul(argv[2],NULL,10);
        if(n < 0 || n > 154)
        {
           printf("Please enter test numbet between 0 to 154\n");
           return -1;
        }

	else if (strcmp(cmd, "gp") == 0){
            if(n < GP_MAX_TEST )
                test_gpu(n);
            else
                printf(" Please GP test number below %d\n", GP_MAX_TEST);
        }

	#ifdef CONFIG_MALIGPU_PP0
        else if(strcmp(cmd, "pp0") == 0){
                if(n < PP0_MAX_TEST )
                    test_gpu(n+GP_MAX_TEST);
                else
                    printf(" Please PP0 test number below %d\n", PP0_MAX_TEST);
        }
        #endif
        #ifdef CONFIG_MALIGPU_PP1
        else if(strcmp(cmd, "pp1") == 0){
                if(n < PP1_MAX_TEST )
                test_gpu(n+GP_MAX_TEST+PP0_MAX_TEST);
            else
                printf(" Please PP1 test number below %d\n", PP1_MAX_TEST);
        }
        #endif
	#ifdef CONFIG_MALIGPU_MP1
        else if(strcmp(cmd, "mp1") == 0){
                if(n < MP1_MAX_TEST )
                test_gpu(n+GP_MAX_TEST+PP0_MAX_TEST+PP1_MAX_TEST);
            else
                printf(" Please MP1 test number below %d\n", MP1_MAX_TEST);
        }
        #endif
        #ifdef CONFIG_MALIGPU_MP2
        else if(strcmp(cmd, "mp2") == 0){
                if(n < MP2_MAX_TEST )
                test_gpu(n+GP_MAX_TEST+PP0_MAX_TEST+PP1_MAX_TEST+MP1_MAX_TEST);
            else
                printf(" Please MP2 test number below %d\n", MP2_MAX_TEST);
        }
        #endif
       #ifdef CONFIG_MALIGPU_PP2
        else if(strcmp(cmd, "pp2") == 0){
                if(n < PP2_MAX_TEST )
                    test_gpu(n+GP_MAX_TEST);
                else
                    printf(" Please PP2 test number below %d\n", PP2_MAX_TEST);
        }
        #endif
        #ifdef CONFIG_MALIGPU_PP3
        else if(strcmp(cmd, "pp3") == 0){
                if(n < PP3_MAX_TEST )
                test_gpu(n+GP_MAX_TEST+PP0_MAX_TEST);
            else
                printf(" Please PP3 test number below %d\n", PP3_MAX_TEST);
       }
 	#endif
	else
        printf("GPU supports: \n\t No of PP's : %d \n\t No of MP's : %d\n");
        return 0;
usage:
      return cmd_usage(cmdtp);

	
#endif

	return 0;
}

U_BOOT_CMD(
	gputest,3,2,do_gputest,
	"GPU Validation Commands",
	"  version        -Print version of GPU\n"
	"  proxycon       -runs the 1080 proxycon frame render using GPU\n"
        " gp    <test_no> -Test GP \n"
        " pp<n> <test_no> -Test PPn (n=0,1,2,3 for ASIC/FPGA)\n"
        " mp<n> <test_no> -Test MPn (n=1,2 for ASIC/FPGA)\n"
	);
