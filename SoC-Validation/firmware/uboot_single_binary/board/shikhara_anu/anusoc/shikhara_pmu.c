//#include <stdio.h>
#include "asm/arch/shikhara_pmu.h"

/* The Cortex-A9 processor PMU provides six counters to gather statistics on the
   operation of the processor and memory system. Each counter can count any of the 58
   events available in the Cortex-A9 processor
*/
/* We Configured 6 Counters for the evevnts of Our Choice.We can change event numbers accoringly */

void start_perfmon(void)
{
    enable_pmu_user_access();	// Allow access to PMU from User Mode
    enable_pmu();              	// Enable the PMU
    pmn_config(0, event1);	// Configure counter 0 to count I-Cache Misses
    pmn_config(1, event3);      // Configure counter 1 to count D-Cache Misses
// Alternatively...
//    pmn_config(0, 0x06);      // Configure counter 0 to count Data Reads
//    pmn_config(1, 0x07);      // Configure counter 1 to count Data Writes
    pmn_config(2, event8);      // Configure counter 2 to count Instructions Executed
    pmn_config(3, event12);     // Configure counter 3 to count PC Changes
    pmn_config(4, event17);     // Configure counter 4 for Branch mispredicted or not predicted
    pmn_config(5, event11);     // Configure counter 3 for Exception returns

    enable_pmn(0);             // Enable counter 0
    enable_pmn(1);             // Enable counter 1
    enable_pmn(2);             // Enable counter 2
    enable_pmn(3);             // Enable counter 3
    enable_pmn(4);             // Enable counter 4
    enable_pmn(5);             // Enable counter 5
    enable_ccnt();             // Enable CCNT
    reset_ccnt();              // Reset the CCNT (cycle counter)
    reset_pmn();               // Reset the configurable counters
}

void stop_perfmon(void)
{
    disable_ccnt();            // Stop CCNT
    disable_pmn(0);            // Stop counter 0
    disable_pmn(1);            // Stop counter 1
    disable_pmn(2);            // Stop counter 2
    disable_pmn(3);            // Stop counter 3
    disable_pmn(4);            // Stop counter 4
    disable_pmn(5);            // Stop counter 5

    printf("\nPerformance monitor results");
    printf("\n---------------------------\n");
    printf("I-Cache Misses = %u\n",        read_pmn(0) );
    printf("D-Cache Misses = %u\n",        read_pmn(1) );
    printf("Instructions Executed = %u\n", read_pmn(2) );
    printf("PC Changes = %u\n",            read_pmn(3) );
    printf("Branch mispredicted = %u\n",     read_pmn(4) );
    printf("Exception returns = %u\n", read_pmn(5) );
    printf("Cycle Count (CCNT) = %u\n",    read_ccnt() );
}

