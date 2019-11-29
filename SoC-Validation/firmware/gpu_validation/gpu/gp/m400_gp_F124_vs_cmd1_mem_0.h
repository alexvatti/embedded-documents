/*----------------------------------------------------------------------------
*
* The confidential and proprietary information contained in this file may
* only be used by a person authorised under and to the extent permitted
* by a subsisting licensing agreement from ARM Limited.
*
*        (C) COPYRIGHT 2008-2009 ARM Limited.
*             ALL RIGHTS RESERVED
*             
* This entire notice must be reproduced on all copies of this file
* and copies of this file may only be made by a person if such person is
* permitted to do so under the terms of a subsisting license agreement
* from ARM Limited.
*
* Modified  : $Date: 2013/05/14 11:57:51 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#define WT    0
#define WB    0
#define MALI_ATTR (WT | WB)
/* ###########################
 * Data structure definition for: m400_gp_F124_vs_cmd1_mem_0
 * ########################### */
struct t_m400_gp_F124_vs_cmd1_mem_0 {
  unsigned int **ttb_first[1024];
  unsigned int *ttb_second_0[1024];
  unsigned int data_00000000[1024];
  unsigned int data_00001000[1024];
  unsigned int data_00002000[1024];
};
extern volatile struct t_m400_gp_F124_vs_cmd1_mem_0 m400_gp_F124_vs_cmd1_mem_0;

