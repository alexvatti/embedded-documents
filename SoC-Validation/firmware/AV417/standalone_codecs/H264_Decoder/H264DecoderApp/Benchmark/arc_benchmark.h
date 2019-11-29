/*
 * Copyright (c) 2006 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* BPR register struct */
typedef struct { 
  unsigned int ID           ;
  unsigned int BPR_CTRL     ;
  unsigned int BPR_SETUP1   ;
  unsigned int BPR_SETUP2   ;
  unsigned int BPR_CYCCNT   ;
  unsigned int BPR_INDEX    ;
  unsigned int BPR_VACYCCNT ;
  unsigned int BPR_IDLCYCCNT;
  unsigned int BPR_PKTCNT   ;
  unsigned int BPR_SUMLAT   ;
  unsigned int BPR_LAT      ;
  unsigned int BPR_CYCHI    ;
  unsigned int BPR_VACYCHI  ;
  unsigned int BPR_IDLCYCHI ;
  unsigned int BPR_PKTHI    ;
  unsigned int BPR_SUMLATHI ;
  unsigned int BPR_VACYCCAP ;
  unsigned int BPR_IDLCYCCAP;
  unsigned int BPR_PKTCAP   ;
  unsigned int BPR_SUMLATCAP;
  unsigned int BPR_LATCAP   ;
} bpr_reg_struct;
