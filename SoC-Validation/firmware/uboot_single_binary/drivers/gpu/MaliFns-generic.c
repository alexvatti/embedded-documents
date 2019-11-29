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
*Modified  : $Date: 2017/10/06 07:03:52 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract :Implements all the generic APIs used for mali memory and register
* compares
*-----------------------------------------------------------------------------
* Overview
*----------
* 
**************************************************************/

#include "MaliFns.h"
//##include <stdio.h>
//#include <string.h>


int Mali_MemCmp(unsigned int *malidata_page,
                unsigned int mali_va,
                unsigned int *refdata,
                unsigned int refoffset,
                unsigned int len) 
{
  int res;

#ifdef MALINOCHECK
  return 0;
#endif

  res = memcmp((void *)((char *)malidata_page + (mali_va % 4096)),
               (void *)((char *)refdata + (refoffset)),
               len * sizeof(unsigned int));

  if (res) {
    printf("Error during check of %x bytes from address PA:%x, VA:%x with reference data at address %x\n",
           len * 4,
           ((unsigned int)malidata_page + (mali_va % 4096)),
           mali_va,
           refdata);
    return -1;
  } else {
    return 0;
  };
};

int Mali_MemCmpMasked(unsigned int *malidata_page,
                      unsigned int mali_va,
                      unsigned int *refdata,
                      unsigned int *refmask,
                      unsigned int refoffset,
                      unsigned int len) 
{
  int res = 0;

  unsigned int *maskptr = (unsigned int *)((char *)refmask + (refoffset));
  unsigned int *refptr  = (unsigned int *)((char *)refdata + (refoffset));
  unsigned int *dataptr = (unsigned int *)((char *)malidata_page + (mali_va % 4096));
  unsigned int len2 = len;
  unsigned int m;

#ifdef MALINOCHECK
  return 0;
#endif

  while (len2--) {
    m   = *maskptr++;
    if ((*dataptr++ & m) != (*refptr++ & m)) {
      res  = 1;
      len2 = 0;
    };
  };

  if (res) {
    printf("Error during check of %x bytes from address PA:%x, VA:%x with reference data at address %x\n",
           len * 4,
           ((unsigned int)malidata_page + (mali_va % 4096)),
           mali_va,
           refdata);
    return -1;
  } else {
    return 0;
  };
};

int Mali_CompareRegs(unsigned int *reference_ptr, 
                      int mali_unit,
                     int mali_core,
                     int lowreg, 
                     int highreg)
{
  unsigned int reference_base = (unsigned int)reference_ptr;
  unsigned int *p;
  int i;
  unsigned int v_mali;
  unsigned int v_testvalue;

#ifdef MALINOCHECK
  return 0;
#endif

  reference_base = reference_base + (lowreg % 0x10);
  p = (unsigned int *)reference_base;

  for (i=0; i < (highreg-lowreg); i += 4) {
    v_mali = Mali_RdReg(mali_unit,mali_core,lowreg+i);
    v_testvalue = *(p + (i/4));
    printf ("Checking Register: %08x is value %08x\n",(mali_unit << 28) + (mali_core << 16) + lowreg + i,v_testvalue);
    if ( v_mali != v_testvalue) {
      printf ("Reg Compare Fail: %08x; value=%08x should be=%08x\n",(mali_unit << 28) + (mali_core << 16) + lowreg + i,v_mali,v_testvalue);
      return -1;
    };
  };
  return 0;
};

void Mali_DisplayReg(int unit,int core, int regnum) {
  int v = Mali_RdReg(unit,core,regnum);
  printf ("Reg: 0x%08x Value: %08x\n",(unit <<28)+(core<<16)+regnum,v);
};
