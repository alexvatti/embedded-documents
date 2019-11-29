/*---------------------------------------------------------------*/
/* Performance Monitoring Unit (PMU) Example Code for Cortex-A/R */
/*                                                               */
/* Copyright (C) ARM Limited, 2010-2012. All rights reserved.    */
/*---------------------------------------------------------------*/


#ifndef _V7_PMU_H
#define _V7_PMU_H

typedef enum pmu_events
{
        event0  = 0x0, //software increment
        event1,   //Instruction cache refill
        event2,   //Instruction micro TLB miss
        event3,   //Data cache miss
        event4,   //Data cache access
        event5,   //Data micro TLB miss
        event6,   //Data read
        event7,   //Data writes
        event8  = 0x68, //Instruction architecturally executed
        event9  = 0x68, 
        event10 = 0x09, //Exception taken
        event11,  //Exception returns
        event12, //Write context id
        event13, //Software change of PC
        event14, //Immediate branch
        event15 = 0x6E, //Predictable function return
        event16 = 0x0F, //Unaligned
        event17, //Branch mispredicted or not predicted
        event18 = 0x12, //Predictable branches
        event19 = 0x40, //Java bytecode
        event20, //Software Java bytecode
        event21, //Jazelle backward branch
        event22 = 0x50, //Coherent linefill miss
        event23, //Coherent linefill hit
        event24 = 0x60,//Instruction cache dependent stall
        event25, //Data cache dependent stall
        event26, //Main TLB miss stall
        event27, //STREX passed
        event28, //STREX failed
        event29, //Data eviction
        event30, //Issue does not dispatch any instruction
        event31, //Issue is empty
        event32 = 0x70, //Main Execution Unit pipe
        event33, //Second Execution Unit pipe
        event34, //Load/Store pipe
        event35,
        event36 = 0x73,
        event37,
        event38 = 0x74,
        event39 = 0x80,// PLD stall
        event40, //Write stall 
        event41, //Instruction main TLB miss stall 
        event42, //Data main TLB miss stall
        event43, //Instruction micro TLB miss stall
        event44, //Data micro TLB miss stall
        event45, //DMB stall
	event46, //Integer core clock disabled
        event47, //Data Engine clock disabled
        event48 = 0x90,//ISB
        event49, //DSB
        event50, //DMB
        event51, //External interrupt
        event52 = 0xA0, //PLE cache line request completed
        event53, //PLE cache line request skipped
        event54, //PLE FIFO Flush
        event55, //PLE request completed
        event56, //PLE FIFO Overflow
        event57  //PLE request programmed
}pmu_evnts;

// Returns the number of progammable counters
unsigned int getPMN(void);

// Sets the event for a programmable counter to record
// counter = Which counter to program (e.g. 0 for PMN0, 1 for PMN1)
// event   = The event code (from appropriate TRM or ARM Architecture Reference Manual)
void pmn_config(unsigned int counter, unsigned int event);

// Enables/disables the divider (1/64) on CCNT
// divider = If 0 disable divider, else enable divider
void ccnt_divider(int divider);


//
// Enables and disables
//


// Global PMU enable
// On ARM11 this enables the PMU, and the counters start immediately
// On Cortex this enables the PMU, there are individual enables for the counters
void enable_pmu(void);

// Global PMU disable
// On Cortex, this overrides the enable state of the individual counters
void disable_pmu(void);

// Enable the CCNT
void enable_ccnt(void);

// Disable the CCNT
void disable_ccnt(void);

// Enable PMN{n}
// counter = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
void enable_pmn(unsigned int counter);

// Enable PMN{n}
// counter = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
void disable_pmn(unsigned int counter);


//
// Read counter values
//


// Returns the value of CCNT
unsigned int read_ccnt(void);

// Returns the value of PMN{n}
// counter = The counter to read (e.g. 0 for PMN0, 1 for PMN1)
unsigned int read_pmn(unsigned int counter);


//
// Overflow and interrupts
//


// Returns the value of the overflow flags
unsigned int read_flags(void);

// Writes the overflow flags
void write_flags(unsigned int flags);

// Enables interrupt generation on overflow of the CCNT
void enable_ccnt_irq(void);

// Disables interrupt generation on overflow of the CCNT
void disable_ccnt_irq(void);

// Enables interrupt generation on overflow of PMN{x}
// counter = The counter to enable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
void enable_pmn_irq(unsigned int counter);

// Disables interrupt generation on overflow of PMN{x}
// counter =  The counter to disable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
void disable_pmn_irq(unsigned int counter);


//
// Counter reset functions
//


// Resets the programmable counters
void reset_pmn(void);

// Resets the CCNT
void reset_ccnt(void);


//
// Software Increment
//


// Writes to software increment register
// counter = The counter to increment (e.g. 0 for PMN0, 1 for PMN1)
void pmu_software_increment(unsigned int counter);


//
// User mode access
//


// Enables User mode access to the PMU (must be called in a privileged mode)
void enable_pmu_user_access(void);

// Disables User mode access to the PMU (must be called in a privileged mode)
void disable_pmu_user_access(void);

#endif // _V7_PMU_H
