#include <common.h>
#include <command.h>

extern void start_perfmon(void); // in perf.c
extern void stop_perfmon(void);  // in perf.c

int do_pmutest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int a,b,c,i,n=0;
  	a=0;
	b=1;
	start_perfmon();
	printf("\n FIBONACCI SERIES\n");
	printf("\t%d\t%d",a,b);
	for(i=0;i<n;i++)
	{

		c=a+b;
     		a=b;
     		b=c;
     		printf("\t%d",c);

 	}
	stop_perfmon();
	return 0;
}

U_BOOT_CMD(
        pmutest, CONFIG_SYS_MAXARGS, 1,      do_pmutest,
        "pmutest - Command to Print PMU counters information\n", 
	      "  -  Counter0 is configured for I-cache Misses \n"
	      "  - Counter1 is configured for D-cache Misses \n"
	      "  - Counter2 is configured for Instructions Executed \n"
	      "  - Counter3 is configured for PC Changes \n "
	      "  - Counter4 is configured for Branch mispredicted \n"
	      "  - Counter5 is configured for Exception returns \n"
);

