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
* Modified  : $Date: 2013/05/14 11:58:21 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#define WT    0
#define WB    0
#define MALI_ATTR (WT | WB)
/* ###########################
 * Data structure definition for: m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0
 * ########################### */
struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 {
  unsigned int **ttb_first[1024];
  unsigned int *ttb_second_0[1024];
  unsigned int *ttb_second_10000[1024];
  unsigned int *ttb_second_20000[1024];
  unsigned int *ttb_second_30000[1024];
  unsigned int *ttb_second_f0000[1024];
  unsigned int data_00004000[1024];
  unsigned int data_10000000[1024];
  unsigned int data_10001000[1024];
  unsigned int data_10002000[1024];
  unsigned int data_10003000[1024];
  unsigned int data_10004000[1024];
  unsigned int data_10005000[1024];
  unsigned int data_10006000[1024];
  unsigned int data_10007000[1024];
  unsigned int data_10008000[1024];
  unsigned int data_10009000[1024];
  unsigned int data_1000a000[1024];
  unsigned int data_1000b000[1024];
  unsigned int data_1000c000[1024];
  unsigned int data_1000d000[1024];
  unsigned int data_1000e000[1024];
  unsigned int data_1000f000[1024];
  unsigned int data_10010000[1024];
  unsigned int data_10011000[1024];
  unsigned int data_10012000[1024];
  unsigned int data_10013000[1024];
  unsigned int data_10014000[1024];
  unsigned int data_10015000[1024];
  unsigned int data_10016000[1024];
  unsigned int data_10017000[1024];
  unsigned int data_10018000[1024];
  unsigned int data_10019000[1024];
  unsigned int data_1001a000[1024];
  unsigned int data_1001b000[1024];
  unsigned int data_1001c000[1024];
  unsigned int data_1001d000[1024];
  unsigned int data_1001e000[1024];
  unsigned int data_1001f000[1024];
  unsigned int data_10020000[1024];
  unsigned int data_10021000[1024];
  unsigned int data_10022000[1024];
  unsigned int data_10023000[1024];
  unsigned int data_10024000[1024];
  unsigned int data_10025000[1024];
  unsigned int data_10026000[1024];
  unsigned int data_10027000[1024];
  unsigned int data_10028000[1024];
  unsigned int data_10029000[1024];
  unsigned int data_1002a000[1024];
  unsigned int data_1002b000[1024];
  unsigned int data_1002c000[1024];
  unsigned int data_1002d000[1024];
  unsigned int data_1002e000[1024];
  unsigned int data_1002f000[1024];
  unsigned int data_10030000[1024];
  unsigned int data_10031000[1024];
  unsigned int data_10032000[1024];
  unsigned int data_10033000[1024];
  unsigned int data_10034000[1024];
  unsigned int data_10035000[1024];
  unsigned int data_10036000[1024];
  unsigned int data_10037000[1024];
  unsigned int data_10038000[1024];
  unsigned int data_10039000[1024];
  unsigned int data_1003a000[1024];
  unsigned int data_1003b000[1024];
  unsigned int data_1003c000[1024];
  unsigned int data_1003d000[1024];
  unsigned int data_1003e000[1024];
  unsigned int data_1003f000[1024];
  unsigned int data_10040000[1024];
  unsigned int data_10041000[1024];
  unsigned int data_10042000[1024];
  unsigned int data_10043000[1024];
  unsigned int data_10044000[1024];
  unsigned int data_10045000[1024];
  unsigned int data_10046000[1024];
  unsigned int data_10047000[1024];
  unsigned int data_10048000[1024];
  unsigned int data_10049000[1024];
  unsigned int data_20000000[1024];
  unsigned int data_20001000[1024];
  unsigned int data_20002000[1024];
  unsigned int data_20003000[1024];
  unsigned int data_20004000[1024];
  unsigned int data_20005000[1024];
  unsigned int data_20006000[1024];
  unsigned int data_20007000[1024];
  unsigned int data_20008000[1024];
  unsigned int data_20009000[1024];
  unsigned int data_30000000[1024];
  unsigned int data_30001000[1024];
  unsigned int data_30002000[1024];
  unsigned int data_30003000[1024];
  unsigned int data_30004000[1024];
  unsigned int data_30005000[1024];
  unsigned int data_30006000[1024];
  unsigned int data_30007000[1024];
  unsigned int data_30008000[1024];
  unsigned int data_30009000[1024];
  unsigned int data_3000a000[1024];
  unsigned int data_3000b000[1024];
  unsigned int data_3000c000[1024];
  unsigned int data_3000d000[1024];
  unsigned int data_3000e000[1024];
  unsigned int data_3000f000[1024];
  unsigned int data_30010000[1024];
  unsigned int data_30011000[1024];
  unsigned int data_30012000[1024];
  unsigned int data_30013000[1024];
  unsigned int data_30014000[1024];
  unsigned int data_30015000[1024];
  unsigned int data_30016000[1024];
  unsigned int data_30017000[1024];
  unsigned int data_30018000[1024];
  unsigned int data_30019000[1024];
  unsigned int data_3001a000[1024];
  unsigned int data_3001b000[1024];
  unsigned int data_3001c000[1024];
  unsigned int data_3001d000[1024];
  unsigned int data_3001e000[1024];
  unsigned int data_3001f000[1024];
  unsigned int data_f0000000[1024];
  unsigned int data_f0001000[1024];
  unsigned int data_f0002000[1024];
  unsigned int data_f0003000[1024];
  unsigned int data_f0004000[1024];
  unsigned int data_f0005000[1024];
};
extern volatile struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0;

