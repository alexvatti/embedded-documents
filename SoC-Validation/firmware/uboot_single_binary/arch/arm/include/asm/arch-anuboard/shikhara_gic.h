/*
 * Header file for SHIKHARA GIC 
 */ 
#include <asm/arch/shikhara_map.h>
#ifndef __SHIKHARA_GIC_H__
#define __SHIKHARA_GIC_H__

#define MAX_NO_OF_INTERRUPTS_IMPLEMENTED 160
#define SPURIOUS_INTERRUPT 1023
#define INTERRUPT_MASK 0x000003FF
#define CPUID_SHIFT    10

typedef void (tHandler)(int, int);

int gic_init(void);

void int_enable(int);

void int_disable(int);

void interrupt_handler_set(int interrupt, tHandler handler);


struct set_and_clear_regs
{
    volatile unsigned int set[32], clear[32];
};

typedef struct
{
    volatile unsigned int control;                //0x000
    volatile unsigned const int controller_type;
    const char padding1[248];
    struct set_and_clear_regs enable;             //0x100
    struct set_and_clear_regs pending;            //0x200
    volatile unsigned const int active[32];       //0x300
    const char padding2[128];
    volatile unsigned int priority[256];          //0x400
    volatile unsigned int target[256];            //0x800
    volatile unsigned int configuration[64];      //0xC00
    const char padding3[512];                     //0xD00
    volatile unsigned int software_interrupt;     //0xF00
    const char padding4[220];
    volatile unsigned const int peripheral_id[4]; //0xFE0
    volatile unsigned const int primecell_id[4]; //0xFF0
} interrupt_distributor;

typedef struct
{
     volatile unsigned int control;        //0x00
     volatile unsigned int priority_mask;  //0x04
     volatile unsigned int binary_point;   //0x08
     volatile unsigned const int interrupt_ack; //0x0C
     volatile unsigned int end_of_interrupt;    //0x10
     volatile unsigned const int running_priority; //0x14
     volatile unsigned const int highest_pending;  //0x18
} gic_cpu_interface;

#endif

