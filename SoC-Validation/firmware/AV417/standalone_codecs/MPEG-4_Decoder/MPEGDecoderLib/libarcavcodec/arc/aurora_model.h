/*
 * Copyright (c) 2005 ARC International (UK) Ltd
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
 * aurora_model.h
 *
 * Nigel Topham, April 2005.
 * Rishabh Uppal, June 2005.
 * Alastair Murray, June 2005.
 */

#ifndef _AURORA_MODEL_H_
#define _AURORA_MODEL_H_

/* So warnings may be printed. */
#undef printf

// Instruction latencies

#define LAT_MUL         3
#define LAT_ADD         2
#define LAT_SUB         2
#define LAT_LOGIC       2
#define LAT_ADDSUB      2
#define LAT_SHIFT       2
#define LAT_LOAD        2
#define LAT_MOV         2
#define LAT_EXCH        3
#define LAT_MINMAX      2
#define LAT_TRAN        1
#define LAT_TRAN_RETAIN 4

/*--- Scalar register loading macros --------------------------------------*/
#define ACTIVATE_SCALAR_REG(REG) /* Do nothing */
#define ASSIGN_SCALAR_REG(REG,VAL)                                        \
    R[REG] = (int)VAL;
#define ASSIGN_UNSIGNED_SCALAR_REG(REG,VAL)                                        \
    R[REG] = (int)VAL;
#define CHANGE_SCALAR_REG(REG,VAL)                                        \
    R[REG] = (int)VAL;

#define GRAB_SDM_BLOCK(NAME,REG)                 \
    int NAME = sdm_standard_malloc();            \
    ASSIGN_SCALAR_REG(REG,NAME);

// Define pseudonyms for vector registers as a notational convenience
//
#define V00 0
#define V01 1
#define V02 2
#define V03 3
#define V04 4
#define V05 5
#define V06 6
#define V07 7
#define V08 8
#define V09 9
#define V10 10
#define V11 11
#define V12 12
#define V13 13
#define V14 14
#define V15 15
#define V16 16
#define V17 17
#define V18 18
#define V19 19
#define V20 20
#define V21 21
#define V22 22
#define V23 23
#define I0 0
#define I1 1
#define I2 2
#define I3 3
#define I4 4
#define I5 5
#define I6 6
#define I7 7
extern int R[37];
#define R0 R[0]
#define R1 R[1]
#define R2 R[2]
#define R3 R[3]
#define R4 R[4]
#define R5 R[5]
#define R6 R[6]
#define R7 R[7]
#define R8 R[8]
#define R9 R[9]
#define R10 R[10]
#define R11 R[11]
#define R12 R[12]
#define R13 R[13]
#define R14 R[14]
#define R15 R[15]
#define R16 R[16]
#define R17 R[17]
#define R18 R[18]
#define R19 R[19]
#define R20 R[20]
#define R21 R[21]
#define R22 R[22]
#define R23 R[23]
#define R24 R[24]
#define R25 R[25]
#define R26 R[26]
#define R27 R[27]
#define R28 R[28]
#define R29 R[29]
#define R30 R[30]
#define R31 R[31]
#define R32 R[32]
#define R33 R[33]
#define R34 R[34]
#define R35 R[35]
#define R36 R[36]
extern int DMA_RegIn[8];         // DMA registers for inbound port
extern int DMA_RegOut[8];        // DMA registers for outbound port
#define DI0 DMA_RegIn[0]
#define DI1 DMA_RegIn[1]
#define DI2 DMA_RegIn[2]
#define DI3 DMA_RegIn[3]
#define DI4 DMA_RegIn[4]
#define DI5 DMA_RegIn[5]
#define DI6 DMA_RegIn[6]
#define DI7 DMA_RegIn[7]
#define DO0 DMA_RegOut[0]
#define DO1 DMA_RegOut[1]
#define DO2 DMA_RegOut[2]
#define DO3 DMA_RegOut[3]
#define DO4 DMA_RegOut[4]
#define DO5 DMA_RegOut[5]
#define DO6 DMA_RegOut[6]
#define DO7 DMA_RegOut[7]


#ifndef _AURORA_MODEL_C_
///////////////////////////////////////////////////////////////////////////////////////////
//
// Declare vector registers VR[0] to VR[23] as an array of 8 integers. In fact, only 16
// bits will ever be used. Every assignment to a VR is checked to see that the values
// assigned are always in range for signed 16-bit integers. This catches overflow.
//
// These are addressed as VR[vreg_num][vreg_element]
//
extern short VR[24][8];                // Was a bug updated on 15 june 

///////////////////////////////////////////////////////////////////////////////////////////
//
// Declare vector registers range-tracking variables
//
// These are subscripted with [vreg_num][vreg_element] and store the maximum and minimum
// value (as a 32-bit range) that any computation has attempted to assign to that 
// vector element.
//
extern int vmin[24][8], vmax[24][8];

///////////////////////////////////////////////////////////////////////////////////////////
//
// Performance performance monitoring functions
//
extern void aurora_reset ();
extern void aurora_stats ();
extern int  aurora_cycles ();
extern int  aurora_stalls ();
extern int  aurora_instructions ();

#if 0
extern void dyadic_op (int, int, int, int);
extern void dual_dest_op (int, int, int);
extern void monadic_op (int, int, int);
extern void store_op (int);
extern void load_op (int);
extern void debug_timing ();
#endif

extern unsigned char * SDM_Bytes;

void DMA_ctor();  /* Should always be declared? */
#endif


///////////////////////////////////////////////////////////////////////////////////////////
//
// Check the elements of a vector register for values being out of 16-bit range
//
#define ONE 1
#define REG_BITS 16
#define HALF_REG_BIT (REG_BITS-1)
#define NEGINT_16 (-(ONE << HALF_REG_BIT))
#define POSINT_16 ((ONE << HALF_REG_BIT) - ONE)

#define MIN(a,b) (((a) > (b)) ? (b) : (a))
#define MAX(a,b) (((a) < (b)) ? (b) : (a))

#define ITER(_n_) int i; for (i = 0; i < (_n_); i++)
#define ITER1(_n_) int j; for (j = 0; j < (_n_); j++)

///////////////////////////////////////////////////////////////////////////////////////////
//
// The ASSIGN macro is called when assigning a new value to the element of a vector
// register. It checks that the signed integer value is within the 16-bit range of
// a vector element, and also keeps track of the maximum and minimum values of the
// designated computation which is passed as an argument to the macro.
//
// Arguments:
//   r is the vector register number
//   e is the vector element number
//   v is the new value
//
// Macros used:
//   ENABLE_CHECK  if defined then vreg range checking is turned on.
//   ENABLE_TRACK  if defined then vreg values and context tracking is on.
//   TRACKING_MIN  defines an l-value integer that will track minimum values.
//   TRACKING_MAX  defines an l-value integer that will track maximum values.
//
#ifdef ENABLE_CHECK
#define RANGE_CHECK(v,r,e)                                                         \
    if (((v) < NEGINT_16) || ((v) > POSINT_16)) {                            \
    av_log ("Error: out-of-range value %d asigned to V%02d[%d]\n",    \
     (v), (r), (e));                                                                      \
  }
#else
#define RANGE_CHECK(v,r,e)
#endif

#ifdef ENABLE_TRACK
#define TRACK_MINMAX(v,r,e)                                                        \
  if ((v) < vmin[(r)][(e)]) vmin[(r)][(e)] = (v);                           \
  if ((v) > vmax[(r)][(e)]) vmax[(r)][(e)] = (v);                           \
  if ((v) > TRACKING_MAX) TRACKING_MAX = (v);                                \
  if ((v) < TRACKING_MIN) TRACKING_MIN = (v);
#else
#define TRACK_MINMAX(v,r,e)
#endif

#define ASSIGN(r,e,v)                                                                \
{                                                                                            \
    RANGE_CHECK(v,r,e)                                                                \
    TRACK_MINMAX(v,r,e)                                                                \
    VR[(r)][(e)] = (v);                                                                \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// Unsigned saturation
//
#define USAT8(x) ( ((x) > 255) ? 255 : ((x) < 0 ? 0 : (x) ) )

///////////////////////////////////////////////////////////////////////////////////////////
//
// Shift without rounding
//
#define SRA(x,d) ((x) >> (d))

///////////////////////////////////////////////////////////////////////////////////////////
//
// Shift with asymmetric rounding
//
// This is sufficiently accurate to be used for the re-alignment of fractional values in
// the VMULFR instructions, but has a rounding bias that makes it unsuitable for use in
// the final right shift at the end of the IDCT. For that reason, it is recommended that
// the symmetric rounding method in the SRAR macro is used with rounding right shifts.
//
#define SRAA(x,d) (((x)+(1<<((d)-1))) >> (d))

///////////////////////////////////////////////////////////////////////////////////////////
//
// Shift with symmetric rounding, as used with a normalising right-shift operation.
// This is a little more complex, but avoids the rounding bias implicit in SRAA.
//
// short SRAR(short x, uint5 d) {
//    if (x < 0)
//        return (x-1 + 2^(d-1)) >> d
//    else
//        return ( x + 2^(d-1))  >> d
// }
//
#define SRAR(x,d) ((d>0)?(((x<0)?(x+(1<<(d-1))-1):(x+(1<<(d-1)))) >> d) : x)
// #define SRAR(x,d) (((x<0) ? (x+(1<<(d-1))-1) : (x+(1<<(d-1)))) >> d)

// The SRA2 macro is a literal implementation of the SRAR rounding shift operation in
// which simple logical operations are used to determine whether the right-shifted
// result should be incremented.
//
#define INC1(x,d) (((x & ((1<<(d-1))-1)) != 0) || (x > 0))
#define INCR(x,d) (INC1(x,d) && ((x & (1<<(d-1))) == (1<<(d-1))))
#define SRA2(x,d) ((d>0) ? ((x >> d) + (INCR(x,d) ? 1 : 0)) : x)

///////////////////////////////////////////////////////////////////////////////////////////
//
// VBMULFW - Multiply vector by scalar, fractional 16-bit values, rounded result
//
// Fractional 'vbmulfr' instruction computes the element-wise product of two vectors of 
// 16-bit values. The results are rounded and then shifted right by 15 bit-positions.
// Changing the shift from SRAR to SRAA does not have a significant effect on accuracy.
//
#define FSHIFT 15

#define VBMULFW(z,x,s) {                                                \
  ITER(8) ASSIGN(z,i,(int)(SRAA( (VR[x][i]*(s)), FSHIFT)));             \
  /*monadic_op (z, x, LAT_MUL);*/                                       \
}

///////////////////////////////////////////////////////////////////////////////
//
// VBADDW - Add a scalar to each element in a vector.
#define VBADDW(z,x,s) {                                                     \
    ITER(8) ASSIGN(z,i,(int)(VR[x][i]+s));                                  \
    /*monadic_op (z, x, LAT_ADD);*/                                         \
}

///////////////////////////////////////////////////////////////////////////////
//
// VBMULW - Multiply each element in a vector by a scalar.
#define VBMULW(z,x,s) {                                                                  \
    ITER(8) ASSIGN(z,i,(short)(VR[x][i]*s));                                \
    /*monadic_op (z, x, LAT_ADD);*/                                         \
}
#define VBMULWi(z,x,s) {                                                     \
    VBMULW(z,x,s)                                                            \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// VASRSRW - Shift vector elements to the right and round the result symmetrically
//
#define VASRSRW(z,x,s) {                                               \
  ITER(8) ASSIGN(z,i,(int)(SRA2(VR[x][i], s)));                        \
  /*monadic_op (z, x, LAT_SHIFT);*/                                    \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// VASRPWB - Shift vector elements to the right, pack to unsigned 8-bits with saturation
//
#define VASRPWB(z,x,s) {                                                \
  ITER(4) ASSIGN(z,i,                                                   \
      ((USAT8(SRA(VR[x][2*i+1],s)) << 8) |                              \
          USAT8(SRA(VR[x][2*i],s))) );                                  \
  /*monadic_op (z, x, LAT_SHIFT);*/                                     \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VADDW - Add two vector registers and place result in a third
//
#define VADDW(z,x,y) {                                                 \
  ITER(8) ASSIGN(z,i,(VR[x][i]+VR[y][i]));                             \
  /*dyadic_op (z, x, y, LAT_ADD);*/                                    \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VSUBW - Subtract two vector registers and place result in a third
//
#define VSUBW(z,x,y) {                                                    \
  ITER(8) ASSIGN(z,i,(VR[x][i]-VR[y][i]));                                \
  /*dyadic_op (z, x, y, LAT_SUB);*/                                       \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VADDSUW - Add and subtract two vector registers and place results in source operands
//
#define VADDSUW(x,y) {                                                    \
   ITER(8) {                                                              \
      int tmp = VR[x][i]-VR[y][i];                                        \
      ASSIGN(x,i,(VR[x][i]+VR[y][i]));                                    \
      ASSIGN(y,i,tmp);                                                    \
   }                                                                      \
   /*dual_dest_op (x,y, LAT_ADDSUB);*/                                    \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMOV - Assign source vector registers to destination vector
//
#define VMOV(z,x) {                                                        \
  ITER(8) ASSIGN(z,i,VR[x][i]);                                            \
  /*monadic_op (z, x, LAT_MOV);*/                                          \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VEXCH1 - Exchange permutation on vector elements
//
// Given two vector, Va and Vb, arranged as follows, with indices decreasing from left
// to right.
//
//          i7  i6  i5  i4  i3  i2  i1  i0
//   ----------------------------------------
//   Va:  { aa, bb, cc, dd, ee, ff, gg, hh }
//   Vb:  { AA, BB, CC, DD, EE, FF, GG, HH }
//
// They are permuted to produce the following outputs, which are re-assigned to v0 and v1
//
//   Va:  { BB, bb, DD, dd, FF, ff, HH, hh }
//   Vb:  { AA, aa, CC, cc, EE, ee, GG, gg }
//
#define VEXCH1(a,b) {                                                       \
  ITER(4) {                                                                 \
     int T = VR[a][2*i+1];                                                  \
     VR[a][2*i+1] = VR[b][2*i];                                             \
     VR[b][2*i]   = T;                                                      \
  }                                                                         \
  /*dual_dest_op (a, b, LAT_EXCH);*/                                        \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VEXCH2 - Exchange permutation on pairs of vector elements
//
// Given two vector, Va and Vb, arranged as follows:
//
//          i7  i6  i5  i4  i3  i2  i1  i0
//   ----------------------------------------
//   Va:  { aa, bb, cc, dd, ee, ff, gg, hh }
//   Vb:  { AA, BB, CC, DD, EE, FF, GG, HH }
//
// They are permuted to produce the following outputs, which are re-assigned to v0 and v1
//
//   Va:  { CC, DD, cc, dd, GG, HH, gg, hh }
//   Vb:  { AA, BB, aa, bb, EE, FF, ee, ff }
//
#define VEXCH2(a,b) {                                                      \
  ITER(2) {                                                                \
     int T0 = VR[a][4*i+2];                                                \
     int T1 = VR[a][4*i+3];                                                \
     VR[a][4*i+2] = VR[b][4*i];                                            \
     VR[a][4*i+3] = VR[b][4*i+1];                                          \
     VR[b][4*i]   = T0;                                                    \
     VR[b][4*i+1] = T1;                                                    \
  }                                                                        \
  /*dual_dest_op (a, b, LAT_EXCH);*/                                       \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VEXCH3 - Exchange permutation on quads of vector elements
//
// Given two vector, Va and Vb, arranged as follows:
//
//          i7  i6  i5  i4  i3  i2  i1  i0
//   ----------------------------------------
//   Va:  { aa, bb, cc, dd, ee, ff, gg, hh }
//   Vb:  { AA, BB, CC, DD, EE, FF, GG, HH }
//
// They are permuted to produce the following outputs, which are re-assigned to v0 and v1
//
//   Va:  { EE, FF, GG, HH, ee, ff, gg, hh }
//   Vb:  { AA, BB, CC, DD, aa, bb, cc, dd }
//
#define VEXCH3(a,b) {                                                       \
  ITER(4) {                                                                 \
     int T = VR[a][i+4];                                                    \
     VR[a][i+4] = VR[b][i];                                                 \
     VR[b][i]   = T;                                                        \
  }                                                                         \
  /*dual_dest_op (a, b, LAT_EXCH);*/                                        \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// VBMINW - Returns minimum of each vector element and a signed integer 16-bit constant
//
#define VBMINW(y,x,s) {                                                     \
  ITER(8) ASSIGN(y,i,MIN(VR[x][i],s));                                      \
  /*monadic_op (y, x, LAT_MINMAX);*/                                        \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VBMAXW - Returns maximum of each vector element and a signed integer 16-bit constant
//
#define VBMAXW(y,x,s) {                                                 \
  ITER(8) ASSIGN(y,i,MAX(VR[x][i],s));                                  \
  /*monadic_op (y, x, LAT_MINMAX);*/                                    \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VLD64W - Load a vector register as 8 byte values arranged consecutively in memory
//          and expand each one to 16 bits
//
// This instruction assumes there are 8 byte values in an array of 8 bytes, that is
// expected to be aligned to a 8-bit boundary.
//
#define VLD64W(r,a,o) {                                                       \
  if (((o) % 8) != 0)                                                         \
      /*printf("WARNING: VLD64W was used with an offset of %d at line %d, it "  \
             "should be divisible by 8 or this code shall not compile for "   \
             "ARC.\n",o,__LINE__); */                                           \
  else if (((o + VR[0][a]) % 8) != 0)                                         \
      /*printf("WARNING: VLD64W was used with an SDM address of %d at line %d, "\
             "it should be divisible by 8 or this code shall not run on "     \
             "ARC.\n",o+VR[0][a],__LINE__); */                                  \
  ITER(8) VR[r][i] = (short)SDM_Bytes[o+VR[0][a]+i];                          \
  /*load_op (r);*/                                                            \
}

////////////////////////////////////////////////////////////////////////////////
//
// VLD128 - Load a vector register as 8 short values arranged consecutively
//          in memory.
//
// *This instruction assumes there are 8 short values in an array of 16 bytes,
//  that is expected to be aligned to a 16-bit boundary.
//
#define VLD128(r,a,o) {                                                        \
  if (((o) % 16) != 0)                                                         \
      /*printf("WARNING: VLD128 was used with an offset of %d at line %d, it "   \
             "should be divisible by 16 or this code shall not compile for "   \
             "ARC.\n",o,__LINE__); */                                            \
  else if (((o + VR[0][a]) % 16) != 0)                                         \
     /* printf("WARNING: VLD128 was used with an SDM address of %d at line %d, " \
             "it should be divisible by 16 or this code shall not run on "     \
             "ARC.\n", (o+VR[0][a]),__LINE__);  */                               \
  short *m = (short *)(SDM_Bytes+o+VR[0][a]);                                  \
  ITER(8) VR[r][i] = m[i];                                                     \
  /*load_op (r);*/                                                             \
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
// VLD128R - Load a vector register as 8 short values arranged consecutively in memory
//
// *This instruction assumes there are 8 short values in an array of 16 bytes, that is
//  expected to be aligned to a 16-bit boundary.
//
#define VLD128R(r,a,o) {                                                       \
  if (((o) % 16) != 0)                                                         \
   /*   printf("WARNING: VLD128R was used with an offset of %d, it should be "   \
             "divisible by 16 or this code shall not compile for ARC.\n",o);*/   \
  else if (((a + o) % 16) != 0)                                                \
      /*printf("WARNING: VLD128R was used with an SDM address of %d, it should " \
             "be divisible by 16 or this code shall not run on ARC.\n",        \
             (a + o)); */                                                        \
  short *m = (short *)(SDM_Bytes+a+o);                                         \
  ITER(8) (short)VR[r][i] = m[i];                                              \
  /*load_op (r);*/                                                             \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VST64 - Store lower 64 bits of a vreg as 8 short values arranged consecutively in memory
//
// *This instruction assumes there are 4 short values in an array of 8 bytes, that is
//  expected to be aligned to an 8-byte boundary.
//

#define VST64(r,a,o) {                                                        \
  if (((o) % 8) != 0)                                                         \
     /* printf("WARNING: VST64 was used with an offset of %d at line %d, it "   \
             "should be divisible by 8 or this code shall not compile for "   \
             "ARC.\n",o,__LINE__);  */                                          \
  else if (((o + VR[0][a]) % 8) != 0)                                         \
    /*  printf("WARNING: VST64 was used with an SDM address of %d at line %d, " \
             "it should be divisible by 8 or this code shall not run on "     \
             "ARC.\n",o+VR[0][a],__LINE__);*/                                   \
  short *m = (short *)(SDM_Bytes+o+VR[0][a]);                                 \
  ITER(4) m[i] = VR[r][i];                                                    \
  /*store_op (r);*/                                                           \
 }

///////////////////////////////////////////////////////////////////////////////
//
// VST128 - Store a vector register as 8 short values arranged consecutively
//          in memory
//
// *This instruction assumes there are 8 short values in an array of 16 bytes,
//  that is expected to be aligned to a 16-bit boundary.
//
#define VST128(r,a,o) {                                                        \
  if (((o) % 16) != 0)                                                         \
     /* printf("WARNING: VST128was used with an offset of %d at line %d, it "    \
             "should be divisible by 16 or this code shall not compile for "   \
             "ARC.\n",o,__LINE__); */                                            \
  else if (((o + VR[0][a]) % 16) != 0)                                         \
    /*  printf("WARNING: VST128 was used with an SDM address of %d at line %d, " \
             "it should be divisible by 16 or this code shall not run on "     \
             "ARC.\n",o+VR[0][a],__LINE__);  */                                  \
  short *m = (short *)(SDM_Bytes+o+VR[0][a]);                                  \
  ITER(8) m[i] = (short)(VR[r][i]);                                            \
  /*store_op (r);*/                                                            \
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
// VST128R - Store a vector register as 8 short values arranged consecutively in memory
//
// *This instruction assumes there are 8 short values in an array of 16 bytes, that is
//  expected to be aligned to a 16-bit boundary.
//
#define VST128R(r,a,o) {                                                       \
  if (((o) % 16) != 0)                                                         \
      /*printf("WARNING: VST128R was used with an offset of %d, it should be "   \
             "divisible by 16 or this code shall not compile for ARC.\n",o); */  \
  if (((a + o) % 16) != 0)                                                     \
    /*  printf("WARNING: VST128R was used with an SDM address of %d, it should " \
             "be divisible by 16 or this code shall not run on ARC.\n",        \
             (a+o)); */                                                          \
  short *m = (short *)(SDM_Bytes+a+o);                                         \
  ITER(8) m[i] = (short)(VR[r][i]);                                            \
  /*store_op (r);*/                                                            \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VOR - Returns the bitwise OR of two  vector register 
//
#define VOR(a,b,c){                             \
    ITER(8)(VR[a][i]=VR[b][i]|VR[c][i]);        \
    /*dyadic_op (a,b,c,LAT_LOGIC);*/            \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VAND - Returns the bitwise AND of two  vector register 
//
#define VAND(a,b,c){                            \
    ITER(8)(VR[a][i]=VR[b][i]&VR[c][i]);        \
    /*dyadic_op (a,b,c,LAT_LOGIC); */           \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VXOR - Returns the bitwise XOR of two  vector register 
//
#define VXOR(a,b,c){                            \
    ITER(8)(VR[a][i]=VR[b][i]^VR[c][i]);        \
    /*dyadic_op (a,b,c,LAT_LOGIC);*/            \
}
///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR1W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR1W(V03,V01,V02) 
//	      V03={h,A,B,C,D,E,F,G}

#define VMR1W(a,b,c){                                                         \
    short temp[8]={VR[c][1],VR[c][2],VR[c][3],VR[c][4],VR[c][5],VR[c][6],     \
                   VR[c][7],VR[b][0]};                                        \
    ITER(8)(VR[a][i]=temp[i]);                                                \
    /*dyadic_op (a,b,c,LAT_LOGIC);*/                                          \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR7W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR7W(V03,V01,V02) 
//	      V03={b,c,d,e,f,g,h,A}
#define VMR7W(a,b,c){                                     \
    short tempc = VR[c][7];                               \
    short tempb[7];                                       \
    {ITER(7) tempb[i]=VR[b][i];}                          \
    ITER(7)(VR[a][i+1] = tempb[i]);                       \
    VR[a][0] = tempc;                                     \
    /*dyadic_op (a,b,c,LAT_LOGIC);*/                      \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// VAVRB - Returns the avgerage of two vectors
// 		   * This instruction does never climbs off its memory boundary so can be used by 8-bit, 16-bit or any set of bits.
//		   *  Output is equavalent to (a+b+1)/2  
#define VAVRB(a,b,c){                                                         \
    uint8_t *a1=(uint8_t *)VR[a];                                             \
    uint8_t *b1=(uint8_t *)VR[b];                                             \
    uint8_t *c1=(uint8_t *)VR[c];                                             \
    ITER(16)(a1[i]=((b1[i]|c1[i])-((((b1[i]^c1[i]))&0xfe)>>1)));              \
    /*dyadic_op (a,b,c,LAT_ADD);*/                                            \
}

///////////////////////////////////////////////////////////////////////////////
//
// VAVB - Returns the non rounding avgerage of two vectors
// 		  *  This instruction does never climbs off its memory boundary so can
//           be used by 8-bit, 16-bit or any set of bits.
//		  *  Output is equavalent to (a+b)/2  
#define VAVB(a,b,c){                                                        \
    uint8_t *a1=(uint8_t *)VR[a];                                           \
    uint8_t *b1=(uint8_t *)VR[b];                                           \
    uint8_t *c1=(uint8_t *)VR[c];                                           \
    ITER(16)(a1[i]=((b1[i]&c1[i])+((((b1[i]^c1[i]))&0xfe)>>1)));            \
    /*dyadic_op (a,b,c,LAT_ADD);*/                                          \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMRB - Returns the shifted result of two vectors
// 		   * This instruction considers we have 16 elements of 8-bits in our vectors.
// e.g. V01={a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p} and V02={A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P}
//         VMRB(V03,V02,V01) 
//	      V03={b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,A}
#define VMRB(a,b,c){                             \
    uint8_t *a1=(uint8_t *)VR[a];                \
    uint8_t *b1=(uint8_t *)VR[b];                \
    uint8_t *c1=(uint8_t *)VR[c];                \
    uint8_t d= *(b1);                            \
    ITER(15)(*(a1+i)=*(c1+i+1));                 \
    *(a1+15)=d;                                  \
    /*dyadic_op (a,b,c,LAT_LOGIC);*/             \
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// VLD32 - Loads 32-bit and expands to 64-bits and stores in lower half of output vector
//
#define VLD32WL(r,a,o) {                                                      \
  if (((o) % 4) != 0)                                                         \
    /*  printf("WARNING: VLD32 was used with an offset of %d, it should be "    \
             "divisible by 4 or this code shall not compile for ARC.\n",o);*/   \
  else if (((o + VR[0][a]) % 4) != 0)                                         \
      /*printf("WARNING: VLD32 was used with an SDM address of %d, it should be"\
             " divisible by 4 or this code shall not run on ARC.\n",          \
             o+VR[0][a]);*/                                                     \
  ITER(4) VR[r][i] = *(SDM_Bytes+o+VR[0][a]+i);                               \
  /*load_op (r);*/                                                            \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VST32 - Stores 64-bit by packing them to 32-bits storing that to destination
//
#define VST32(r,a,o) {                                                        \
  if (((o) % 4) != 0)                                                         \
      /*printf("WARNING: VST32 was used with an offset of %d, it should be "    \
             "divisible by 4 or this code shall not compile for ARC.\n",o);*/   \
  else if (((o + VR[0][a]) % 4) != 0)                                         \
      /*printf("WARNING: VST32 was used with an SDM address of %d, it should be"\
             " divisible by 4 or this code shall not run on ARC.\n",          \
             o+VR[0][a]); */                                                    \
  short *m = (short *)(SDM_Bytes+VR[0][a]+o);                                 \
  ITER(2) m[i] = (short)(VR[r][i]);                                           \
  /*store_op (r);*/                                                           \
 }
 

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMVW - transfers one specified element of one vector to other and else is not changed
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMVW(V01,V02,0x3) 
//	      V01={a,b,C,d,e,f,g,h}
#define VMVW(z,x,s){                                                    \
    ITER(8) if ((s>>(i))&1){ ASSIGN(z,i,VR[x][i]); }                    \
    /*monadic_op (z, x, LAT_TRAN_RETAIN);*/                             \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR6W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR6W(V03,V01,V02) 
//	      V03={c,d,e,f,g,h,A,B}
#define VMR6W(a,b,c){                                                    \
    short tempc[2] = {VR[c][6],VR[c][7]};                                \
    short tempb[6];                                                      \
    {ITER(6) tempb[i]=VR[b][i];}                                         \
    ITER(6)(VR[a][i+2] = tempb[i]);                                      \
    VR[a][0] = tempc[0];                                                 \
    VR[a][1] = tempc[1];                                                 \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR5W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR5W(V03,V01,V02) 
//	      V03={d,e,f,g,h,A,B,C}
#define VMR5W(a,b,c){                                                   \
    short temp[3] = {VR[c][5],VR[c][6],VR[c][7]};                       \
    short tempb[5];                                                     \
    {ITER(5) tempb[i]=VR[b][i];}                                        \
    ITER(5)(VR[a][i+3] = tempb[i]);                                     \
    ITER1(3)(VR[a][j] = temp[j]);                                       \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR4W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR4W(V03,V01,V02) 
//	      V03={e,f,g,h,A,B,C,D}
#define VMR4W(a,b,c){                                                   \
    short temp[4] = {VR[c][4],VR[c][5],VR[c][6],VR[c][7]};              \
    short tempb[4];                                                     \
    {ITER(4) tempb[i]=VR[b][i];}                                        \
    ITER(4)(VR[a][i+4] = tempb[i]);                                     \
    ITER1(4)(VR[a][j] = temp[j]);                                       \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR3W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR3W(V03,V01,V02) 
//	      V03={f,g,h,A,B,C,D,E}
#define VMR3W(a,b,c){                                                   \
    short temp[5] = {VR[c][3],VR[c][4],VR[c][5],VR[c][6],VR[c][7]};     \
    short tempb[3];                                                     \
    {ITER(3) tempb[i]=VR[b][i];}                                        \
    ITER(3)(VR[a][i+5] = tempb[i]);                                     \
    ITER1(5)(VR[a][j] = temp[j]);                                       \
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// VMR2W - Returns the shifted result of two vectors
// 
// e.g. V01={a,b,c,d,e,f,g,h} and V02={A,B,C,D,E,F,G,H}
//         VMR2W(V03,V01,V02) 
//	      V03={g,h,A,B,C,D,E,F}
#define VMR2W(a,b,c){                                                        \
    short temp[6] = {VR[c][2],VR[c][3],VR[c][4],VR[c][5],VR[c][6],VR[c][7]}; \
    short tempb[2];                                                     \
    {ITER(2) tempb[i]=VR[b][i];}                                        \
    ITER(2)(VR[a][i+6] = tempb[i]);                                          \
    ITER1(6)(VR[a][j] = temp[j]);                                            \
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// VSUMMW- Returns the addision of bitwise AND of two vectors and stores that result into all the elements of output vector
// 
#define VSUMMW(a,b,c){                                                  \
    short temp=0;                                                       \
    ITER(8)(temp+=(VR[b][i]&VR[c][i]));                                 \
    VR[a][0]=temp;                                                      \
    ITER1(7) VR[a][j+1]=0;                                              \
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VMULFW- Returns the multiplication of two vectors and storing the higher 16-bits into output vector with symmentric rounding
// 
#define VMULFW(a,b,c) {                                                 \
    ITER(8)VR[a][i]=(((VR[b][i]*VR[c][i]+(1<<14))>>15));                \
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VMULW- Returns the multiplication of two vectors and storing the lower 16-bits into output vector with symmentric rounding
// 
#define VMULW(a,b,c){                                                   \
    int temp;                                                           \
    ITER(8){    temp=VR[b][i]*VR[c][i];                                 \
                VR[a][i] = (short)temp;                                 \
    }                                                                   \
}

//////////////////////////////////////////////////////////////////////////////
//
// VLD64 - Load a lower half vector register as 4 short values arranged
//         consecutively in memory
//
// * This instruction assumes there are 4 short values in an array of 16
//   bytes, that is expected to be aligned to a 16-bit boundary.
//
#define VLD64(r,a,o) {                                                        \
  if (((o) % 8) != 0)                                                         \
      /*printf("WARNING: VLD64 was used with an offset of %d, it should be "    \
             "divisible by 8 or this code shall not compile for ARC.\n",o); */  \
  else if (((o + VR[0][a]) % 8) != 0)                                         \
      /*printf("WARNING: VLD64 was used with an SDM address of %d, it should be"\
             " divisible by 8 or this code shall not run on ARC.\n",          \
             o+VR[0][a]); */                                                    \
    short *m = (short *)(SDM_Bytes+o+VR[0][a]);                               \
    ITER(4) (short)VR[r][i] = m[i];                                           \
    /*load_op (r);*/                                                          \
}

///////////////////////////////////////////////////////////////////////////////
//
// VMOVZW - Copy the value for an ARC scalar register into the elements defined
//          by the bitmask within a vector register and sets every other
//          element to be 0.
#define VMOVZW(z,x,s) {                                                      \
    ITER(8) if ( (s>>(i))&1){ ASSIGN(z,i,x); } else { ASSIGN(z,i,0); }       \
    /*monadic_op (z, x, LAT_TRAN);*/                                         \
}

///////////////////////////////////////////////////////////////////////////////
//
// VMOVZW - Copy the value for an ARC scalar register into the elements defined
//          by the bitmask within a vector register and leave every other
//          element untouched.
#define VMOVW(z,x,s) {                                                       \
    ITER(8) if ((s>>(i))&1){ ASSIGN(z,i,x); }                                \
    /*monadic_op (z, x, LAT_TRAN_RETAIN);*/                                  \
}

//////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//
// DMA model
//
///////////////////////////////////////////////////////////////////////////////////////////

// DMA registers full names

static const int SDM_DEST_ADR = 0;
static const int SDM_STRIDE   = 1;
static const int DMA_BLK      = 2;
static const int DMA_LOC      = 3;
static const int SYS_DEST_ADR = 4;
static const int SYS_STRIDE   = 5;
static const int DMA_CFG      = 6;
static const int FT_BASE_ADR  = 7;

// DMA registers short names

static const int d0 = 0;
static const int d1 = 1;
static const int d2 = 2;
static const int d3 = 3;
static const int d4 = 4;
static const int d5 = 5;
static const int d6 = 6;
static const int d7 = 7;

// Structure of a DMA frame

typedef short SIMD_word[8];


//------------------------ DMA functional API --------------------------------------

// Initialise SDM base pointer within the DMA model
void DMA_SetSDM (unsigned char *sdm);

// Setup frame structure at SDM offset
void DMA_SetFrame (int offset, unsigned char *baseAddr, int stride, int xDim, int yDim);

// Perform DMA move from SDM to main memory
void DMA_RunOut();

// Perform DMA move from main memory to SDM

void DMA_RunIn();

#define VDIWRi(DR,RR) {                                                        \
    DR = RR;                                                                  \
}

#define VDOWRi(DR,RR) {                                                        \
    DR = RR;                                                                  \
}                                                                             \

#define VDIWRr(DR,RR) {                                                        \
    DR = RR;                                                                  \
}

#define VDOWRr(DR,RR) {                                                        \
    DR = RR;                                                                  \
}                                                                             \

#define VDIRD(VReg,DReg) {                   \
    int *m = (int *)VR[VReg];                \
    *m = (int)DReg;                           \
}

#define VDORD(VReg,DReg) {                   \
    int *m = (int *)VR[VReg];                \
    *m = (int)DReg;                           \
}

#define VDIRUN(X_COORD,Y_COORD) {                                       \
    DI3 = (int)((X_COORD) | ((Y_COORD) << 16));                         \
    DMA_RunIn();                                                        \
}

#define VDORUN(X_COORD,Y_COORD) {                                       \
    DO3 = (int)((X_COORD) | ((Y_COORD) << 16));                         \
    DMA_RunOut();                                                       \
}

#endif
