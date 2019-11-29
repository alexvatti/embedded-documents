/*----------------------------------------------------------------------------
*
*The confidential and proprietary information contained in this file may
*only be used by a person authorised under and to the extent permitted
*by a subsisting licensing agreement from ARM Limited.
*
*        (C) COPYRIGHT 2008-2009 ARM Limited.
*             ALL RIGHTS RESERVED
*             
*This entire notice must be reproduced on all copies of this file
*and copies of this file may only be made by a person if such person is
*permitted to do so under the terms of a subsisting license agreement
*from ARM Limited.
*Modified  : $Date: 2017/10/06 07:03:53 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract : Mali Performance Counters
*-----------------------------------------------------------------------------*/

#include "MaliFns.h"

void Mali_InitPerfCountersFn(int core, int cnt_num, int cnt_id);

/* Defines for Performance Counters - Cores */
#define  GP_PERF 0
#define PP0_PERF 1
#define PP1_PERF 2
#define PP2_PERF 3
#define PP3_PERF 4
#define L2C_PERF 5


void Mali_InitPerfCounters() {
  Mali_InitPerfCountersFn( GP_PERF, 0, 1);
  Mali_InitPerfCountersFn( GP_PERF, 1, 6);

  Mali_InitPerfCountersFn(PP0_PERF, 0, 0);
  Mali_InitPerfCountersFn(PP0_PERF, 1, 2);

  Mali_InitPerfCountersFn(PP1_PERF, 0, 0);
  Mali_InitPerfCountersFn(PP1_PERF, 1, 2);

  Mali_InitPerfCountersFn(PP2_PERF, 0, 0);
  Mali_InitPerfCountersFn(PP2_PERF, 1, 2);

  Mali_InitPerfCountersFn(PP3_PERF, 0, 0);
  Mali_InitPerfCountersFn(PP3_PERF, 1, 2);

  Mali_InitPerfCountersFn(L2C_PERF, 0, 2);
  Mali_InitPerfCountersFn(L2C_PERF, 1, 10);
};

void Mali_InitPerfCountersFn(int core, int cnt_num, int cnt_id) {

    if (core == GP_PERF){
        /* GP */
        Mali_WrReg(0,0,0x3C + cnt_num*4,1);
        Mali_WrReg(0,0,0x44 + cnt_num*4,cnt_id);
    }

    if (core == PP0_PERF){    
        /* PP0 */
        Mali_WrReg(1,0,0x1080+cnt_num*0x20,1);
        Mali_WrReg(1,0,0x1084+cnt_num*0x20,cnt_id);
    }


  

#if MALI == m400
  
    if (core == PP1_PERF && PP_CORES > 1 ){
        /* PP1 */
        Mali_WrReg(1,1,0x1080+cnt_num*0x20,1);
        Mali_WrReg(1,1,0x1084+cnt_num*0x20,cnt_id);
    }


    if (core == PP2_PERF && PP_CORES > 2){    
        /* PP2 */
        Mali_WrReg(1,2,0x1080+cnt_num*0x20,1);
        Mali_WrReg(1,2,0x1084+cnt_num*0x20,cnt_id);
    }
    
    if (core == PP3_PERF && PP_CORES > 3){    
        /* PP3 */
        Mali_WrReg(1,3,0x1080+cnt_num*0x20,1);
        Mali_WrReg(1,3,0x1084+cnt_num*0x20,cnt_id);
    }

    if (core == L2C_PERF){    
        /* L2 */
        Mali_WrReg(3,0,0x20 + cnt_num*8,1);
        Mali_WrReg(3,0,0x24 + cnt_num*8,cnt_id);
    }
#endif

};


void Mali_ReadPerfCounters() {

  /* This is an example implementation
   * This function needs to be retargeted to any performance counters
   * you wish to read from any of the cores
   */

  unsigned int v;
  printf ("Reading performance counters\n");

  v = Mali_RdReg(2,0,0x14);
  printf ("%-24s, %x\n","MaliMMU  INT_RAWSTAT",v);

  v = Mali_RdReg(0,0,0x24);
  printf ("%-24s, %x\n","MaliGP   INT_RAWSTAT",v);

v = Mali_RdReg(1,0,0x1020);
  printf ("%-24s, %x\n","MaliPP0  INT_RAWSTAT",v);

#if MALI == m400
  if (PP_CORES > 1) {
      v = Mali_RdReg(1,1,0x1020);
      printf ("%-24s, %x\n","MaliPP1  INT_RAWSTAT",v);
  }

  v = Mali_RdReg(2,1,0x14);
  printf ("%-24s, %x\n","MaliMMU-PP0 INT_RAWSTAT",v);

  if (PP_CORES > 1) {
      v = Mali_RdReg(2,2,0x14);
      printf ("%-24s, %x\n","MaliMMU-PP1 INT_RAWSTAT",v);
  }
#endif

  v = Mali_RdReg(0,0,0x4C);
  printf ("%-24s, %u\n","MaliGP Active Cycles",v);

  v = Mali_RdReg(1,0,0x108C);
  printf ("%-24s, %u\n","MaliPP0 Active Cycles",v);

#if MALI == m400
  if (PP_CORES > 1) {
      v = Mali_RdReg(1,1,0x108C);
      printf ("%-24s, %u\n","MaliPP1 Active Cycles",v);
  }

  v = Mali_RdReg(3,0,0x24);
  printf ("%-24s, %u\n","MaliL2 Active Cycles",v);
#endif

  v = Mali_RdReg(0,0,0x50);
  printf ("%-24s, %u\n","MaliGP 64-bit Reads",v);

  v = Mali_RdReg(1,0,0x10AC);
  printf ("%-24s, %u\n","MaliPP0 64-bit Reads",v);

#if MALI == m400
  if (PP_CORES > 1) {
      v = Mali_RdReg(1,1,0x10AC);
      printf ("%-24s, %u\n","MaliPP1 64-bit Reads",v);
  }

  v = Mali_RdReg(3,0,0x2C);
  printf ("%-24s, %u\n","MaliL2 128-bit Reads",v);
  printf ("%-24s, %u\n","MaliL2 128-bit Reads * 2",v*2);
#endif
};
