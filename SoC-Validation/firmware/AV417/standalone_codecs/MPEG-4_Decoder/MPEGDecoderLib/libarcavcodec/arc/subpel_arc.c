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
 */

//#include <stdio.h>
#ifdef VERIFY_AURORA
    #include <string.h>
    
#endif
#include "aurora_asm.h"
#include "../dsputil.h"


/*--- Pseudo Instructions ---------------------------------------------------*/
//////////////////////////////////////////////////////////////////////////////
//
// VAVRB4 - Returns the avgerage of two vectors and uses V21-V23 as temporary
//          registers.
//          This instruction does never climbs off its memory boundary so can
//          be used by 8-bit, 16-bit or any set of bits.
//		    *  This macro uses V21-V23 as temporary so has side effects and
//             one should not use V21-V23 when this is called
//		    *  Output is equavalent to (a+b+c+d+2)/4.
#define VAVRB4(a,b,c,d,e)    \
  vavrb %vr##a,%vr##b,%vr##c ` \
  vavrb %vr21,%vr##d,%vr##e ` \
  vavb  %vr##a,%vr##a,%vr21 ` \
  vavb  %vr22,%vr##b,%vr##c ` \
  vavb  %vr23,%vr##d,%vr##e ` \
  vavrb %vr21,%vr22,%vr23 ` \
  vavrb %vr##a,%vr##a,%vr21

//////////////////////////////////////////////////////////////////////////////
//
// VNAVRB4 - Returns the non rounding avgerage of two vectors and uses
//           V21-V23 as temporary registers
// 		  *  This instruction does never climbs off its memory boundary so
//           can be used by 8-bit, 16-bit or any set of bits.
//		  *  This macro uses V21-V23 as temporary so has side effects and one
//           should not use V21-V23 when this is called
//		  *  Output is equavalent to (a+b+c+d+2)/4.
#define VNAVRB4(a,b,c,d,e)   \
  vavrb %vr##a,%vr##b,%vr##c ` \
  vavrb %vr21,%vr##d,%vr##e ` \
  vavb  %vr##a,%vr##a,%vr21 ` \
  vavb  %vr22,%vr##b,%vr##c ` \
  vavb  %vr23,%vr##d,%vr##e ` \
  vavrb %vr21,%vr22,%vr23 ` \
  vavb  %vr##a,%vr##a,%vr21

/* This macro reverses the contents of register. Operand c is temporary vector
 * taken*/
#define VREVRS(a,b,c) \
  vxor    %vr##a,%vr##a,%vr##a ` \
  vor     %vr##a,%vr##a,%vr##b ` \
  vxor    %vr##c,%vr##c,%vr##c ` \
  vexch1  %vr##a,%vr##c ` \
  vmr7w   %vr##a,%vr##a,%vr##a ` \
  vor     %vr##a,%vr##a,%vr##c ` \
  vxor    %vr##c,%vr##c,%vr##c ` \
  vexch2  %vr##a,%vr##c ` \
  vmr6w   %vr##a,%vr##a,%vr##a ` \
  vor     %vr##a,%vr##a,%vr##c ` \
  vor     %vr##c,%vr##a,%vr##c ` \
  vmr4w   %vr##a,%vr##a,%vr##c


/* Transpose an 8x8 matrix in vector registers R1 to R8 */
#define TRANSPOSE(VR1,VR2,VR3,VR4,VR5,VR6,VR7,VR8) \
  vexch1  %vr##VR1,%vr##VR2 ` \
  vexch1  %vr##VR3,%vr##VR4 ` \
  vexch1  %vr##VR5,%vr##VR6 ` \
  vexch1  %vr##VR7,%vr##VR8 ` \
  vexch2  %vr##VR1,%vr##VR3 ` \
  vexch2  %vr##VR2,%vr##VR4 ` \
  vexch2  %vr##VR5,%vr##VR7 ` \
  vexch2  %vr##VR6,%vr##VR8 ` \
  vexch4  %vr##VR1,%vr##VR5 ` \
  vexch4  %vr##VR2,%vr##VR6 ` \
  vexch4  %vr##VR3,%vr##VR7 ` \
  vexch4  %vr##VR4,%vr##VR8

//#define DEBUG_OUTPUT
#ifdef DEBUG_OUTPUT

#ifdef HAVE_AURORA_MODEL
/* This is C99 so doesn't compile with hcac. */
#define ENTER_DEBUG                                    \
    //printf("Entering function --- %s ---\n",__func__); \
    fflush(stdout);
#define LEAVE_DEBUG                                    \
   // printf("Leaving function  --- %s ---\n",__func__); \
    fflush(stdout);
#else
#define ENTER_DEBUG                                            \
   // printf("Entering function at line --- %d ---\n",__LINE__); \
    fflush(stdout);
    /* Do nothing */
#define LEAVE_DEBUG                                           \
   // printf("Leaving function at line  --- %d ---\n",__LINE__); \
    fflush(stdout);
    /* Do nothing */
#endif

#else
#define ENTER_DEBUG /* Do nothing */
#define LEAVE_DEBUG /* Do nothing */
#endif

#ifdef VERIFY_AURORA

/* Set R0-R36 in the model to odd values, to simulate arbitary C code having
 * been run. */
void inline randomise_scalar_reg() {
#ifdef HAVE_AURORA_MODEL
    int loop;
    for (loop = 0; loop <= 12; loop++) {
        CHANGE_SCALAR_REG(loop,137+7*loop);
    }
    /* R13-R25 are callee saved, i.e. preserved by the compiler. */
    for (loop = 26; loop <= 36; loop++) {
        CHANGE_SCALAR_REG(loop,137+7*loop);
    }
#endif
    return;
}


/* Some pair of macros should be placed before and after the assembly in each
 * aurora specific function so they are all checked when VERIFY_AURORA
 * is defined.
 */
#define VERIFY_AURORA_MPEG2_PRE_ASM(WIDTH)                                    \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x, v_y;                                                             \
    uint8_t *start_of_block = (int)block + SDM_Bytes;                         \
    const uint8_t *start_of_pixels = (int)pixels + SDM_Bytes;                 \
    uint8_t *ref_block = (uint8_t *)                                          \
                           MALLOC( (line_size * h * sizeof(*block)) );        \
    if ((WIDTH == 8) && (line_size != 16)) {                                  \
      //  printf("WARNING: An mpeg2 function of width 8 was called with a "     \
            //   "stride of %d, this shall probabally not work, stride is "     \
              // "assumed to be %d.\n",line_size,16);                           \
    } else if ((WIDTH == 16) && (line_size != 32)) {                          \
       // printf("WARNING: An mpeg2 function of width 16 was called with a "    \
            //   "stride of %d, this shall probabally not work, stride is "     \
              // "assumed to be %d.\n",line_size,32);                           \
    }                                                                         \
    /* Copy block into ref_block: */                                          \
    for (v_y = 0; v_y < h; v_y++ ) {                                          \
        for (v_x = 0; v_x < line_size; v_x++) {                               \
            *(ref_block+v_y*line_size+v_x) =                                  \
                *(start_of_block+v_y*line_size+v_x);                          \
        }                                                                     \
    }
#define VERIFY_AURORA_MPEG2_POST_ASM(FUNCNAME, WIDTH)                         \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_block, start_of_pixels, line_size, h);               \
    compare(#FUNCNAME "_aurora", WIDTH, start_of_block, ref_block,            \
            line_size, line_size, h, start_of_pixels);                        \
    FREE(ref_block);                                                          \
    LEAVE_DEBUG;

#define VERIFY_AURORA_PIXELS_PRE_ASM                                          \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x, v_y;                                                             \
    DCTELEM *start_of_block = block;                                          \
    const uint8_t *start_of_pixels = pixels;                                  \
    DCTELEM *ref_block = (DCTELEM *)                                          \
                           MALLOC( (line_size * 8 * sizeof(*block)) );        \
    /* Copy block into ref_block: */                                          \
    for (v_y = 0; v_y < 8; v_y++ ) {                                          \
        for (v_x = 0; v_x < line_size; v_x++) {                               \
            *(ref_block+v_y*line_size+v_x) = *(block+v_y*line_size+v_x);      \
        }                                                                     \
    }
#define VERIFY_AURORA_PIXELS_POST_ASM(FUNCNAME, WIDTH)                        \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_block, start_of_pixels, line_size);                  \
    dcompare(#FUNCNAME "_aurora", WIDTH, start_of_block, ref_block,           \
            line_size, line_size, 8, start_of_pixels);                        \
    FREE(ref_block);                                                          \
    LEAVE_DEBUG;

#define VERIFY_AURORA_DPIXELS_PRE_ASM                                         \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x, v_y;                                                             \
    DCTELEM *start_of_block = block;                                          \
    const uint8_t *start_of_pixels1 = s1;                                     \
    const uint8_t *start_of_pixels2 = s2;                                     \
    DCTELEM *ref_block = (DCTELEM *) MALLOC( (stride * 8 * sizeof(*block)) ); \
    /* Copy block into ref_block: */                                          \
    for (v_y = 0; v_y < 8; v_y++ ) {                                          \
        for (v_x = 0; v_x < stride; v_x++) {                                  \
            *(ref_block+v_y*stride+v_x) = *(block+v_y*stride+v_x);            \
        }                                                                     \
    }
#define VERIFY_AURORA_DPIXELS_POST_ASM(FUNCNAME, WIDTH)                       \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_block, start_of_pixels1, start_of_pixels2, stride);  \
    dcompare(#FUNCNAME "_aurora", WIDTH, start_of_block, ref_block,           \
             stride, stride, 8, start_of_pixels1);                            \
    FREE(ref_block);                                                          \
    LEAVE_DEBUG;

#define VERIFY_AURORA_SINGLE_DBLOCK_PRE_ASM                                   \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x;                                                                  \
    DCTELEM *start_of_block = (DCTELEM *)(SDM_Bytes + (int)blocks);         \
    DCTELEM *ref_block = (DCTELEM *) MALLOC( (6*64*sizeof(DCTELEM)) );\
    /* Copy block into ref_block: */                                          \
    for (v_x = 0; v_x < (6*64); v_x++) {                                      \
        *(ref_block+v_x) = *(start_of_block+v_x);                             \
    }
#define VERIFY_AURORA_SINGLE_DBLOCK_POST_ASM(FUNCNAME)                        \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_block);                                              \
    dcompare(#FUNCNAME "_aurora", 6, start_of_block, ref_block,               \
             6, 12, 64, (uint8_t *)start_of_block);                           \
    FREE(ref_block);                                                          \
    LEAVE_DEBUG;

#define VERIFY_AURORA_BYTES_PRE_ASM                                           \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x, v_y;                                                             \
    uint8_t *start_of_src = src;                                              \
    uint8_t *start_of_dst = dst;                                              \
    uint8_t *ref_dst = (uint8_t *)MALLOC( (8*w*sizeof(*dst)) );               \
    for (v_y = 0; v_y < 8; v_y++ ) {                                          \
        for (v_x = 0; v_x < w; v_x++) {                                       \
            *(ref_dst+v_y*w+v_x) = *(dst+v_y*w+v_x);                          \
        }                                                                     \
    }
#define VERIFY_AURORA_BYTES_POST_ASM(FUNCNAME)                                \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_dst,start_of_src,w);                                 \
    compare(#FUNCNAME "_aurora", w, start_of_dst, ref_dst, w, w,              \
            8, start_of_src);                                                 \
    FREE(ref_dst);                                                            \
    LEAVE_DEBUG;

#define VERIFY_AURORA_QPEL_PRE_ASM                                            \
    ENTER_DEBUG;                                                              \
    randomise_scalar_reg();                                                   \
    int v_x, v_y;                                                             \
    uint8_t *start_of_src = (int)src + SDM_Bytes;                             \
    uint8_t *start_of_dst = (int)dst + SDM_Bytes;                             \
    uint8_t *ref_dst = (uint8_t *)MALLOC( (16*stride*sizeof(*start_of_dst)) );\
    if (stride != 32)                                                \
       // printf("WARNING: A qpel function was called with a stride of %d, "    \
             //  "this shall probabally not work, stride is assumed to be "     \
             //  "%d.\n",stride,32);                                   \
    for (v_y = 0; v_y < 16; v_y++ ) {                                         \
        for (v_x = 0; v_x < stride; v_x++) {                                  \
            *(ref_dst+v_y*stride+v_x) = *(start_of_dst+v_y*stride+v_x);       \
        }                                                                     \
    }
#define VERIFY_AURORA_QPEL_POST_ASM(FUNCNAME)                                 \
    randomise_scalar_reg();                                                   \
    FUNCNAME ## _ref(ref_dst,start_of_src,stride);                            \
    compare(#FUNCNAME "_aurora", 16, start_of_dst, ref_dst, stride, stride,   \
            16, start_of_src);                                                \
    FREE(ref_dst);                                                            \
    LEAVE_DEBUG;

#else

#define VERIFY_AURORA_MPEG2_PRE_ASM(WIDTH) ENTER_DEBUG;
#define VERIFY_AURORA_MPEG2_POST_ASM(FUNCNAME,WIDTH) LEAVE_DEBUG;
#define VERIFY_AURORA_PIXELS_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_PIXELS_POST_ASM(FUNCNAME,WIDTH) LEAVE_DEBUG;
#define VERIFY_AURORA_DPIXELS_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_DPIXELS_POST_ASM(FUNCNAME,WIDTH) LEAVE_DEBUG;
#define VERIFY_AURORA_DPIXELS_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_DPIXELS_POST_ASM(FUNCNAME,WIDTH) LEAVE_DEBUG;
#define VERIFY_AURORA_SINGLE_DBLOCK_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_SINGLE_DBLOCK_POST_ASM(FUNCNAME) LEAVE_DEBUG;
#define VERIFY_AURORA_BYTES_DBLOCK_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_BYTES_DBLOCK_POST_ASM(FUNCNAME) LEAVE_DEBUG;
#define VERIFY_AURORA_QPEL_PRE_ASM ENTER_DEBUG;
#define VERIFY_AURORA_QPEL_POST_ASM(FUNCNAME) LEAVE_DEBUG;

#endif

#ifdef VERIFY_AURORA
static void compare ( char* function_name, int width,
                      const uint8_t *aurora_block, const uint8_t *ref_block,
                      int dst_line_size, int src_line_size,
                      int h, const uint8_t *pixels )
{
    int x, y;
    int error = 0;

    if (   ((compare_calls != 0)
            && (compare_calls < COMPARE_GRAIN_AURORA)
            && ((compare_calls % (COMPARE_GRAIN_AURORA / COMPARE_EARLY))==0))
        || (((compare_calls != 0)
            && (compare_calls % COMPARE_GRAIN_AURORA)==0))) {
      /*  printf("\nThere have been %d compare calls so far with %d big errors "
               "and %d small errors. Meaning there have been %d errors in "
               "total (%.2f%%).\n",
               compare_calls, compare_big_errors, compare_small_errors,
               (compare_big_errors+compare_small_errors),
               (((compare_big_errors+compare_small_errors)
                 /((float)compare_calls))*100));*/
        print_function_call_info();
    }
    ++compare_calls;

    for (y = 0; y < h ; y++) {
        for (x =  0; x < dst_line_size; x++) {
            int difference =   *(aurora_block+y*dst_line_size+x)
                             - *(   ref_block+y*dst_line_size+x);
            if ((difference > MONITOR_COMPARES_AURORA_SMALL_ERROR_RANGE) ||
                ((0-difference) > MONITOR_COMPARES_AURORA_SMALL_ERROR_RANGE)){
                error = 2;
                #ifdef VERBOSE_COMPARE_ERRORS
                if (x > width)
                   /* printf("Error outwith the area this function should be "
                           "operating in; x: %d, y: %d, pixels: %d, aurora: %d"
                           ", reference: %d\n",
                           x,y,*(pixels+y*src_line_size+x),
                           *(aurora_block+y*dst_line_size+x),
                           *(ref_block+y*dst_line_size+x));*/
                #endif
            } else if ((error != 2) && (difference != 0)) {
                error = 1;
            }
        }
    }

    if (error == 1) {
        ++compare_small_errors;
        increment_function_tab_call_and_error(function_name);
    } else if (error == 2) {
        ++compare_big_errors;
        increment_function_tab_call_and_error(function_name);
    } else {
        increment_function_tab_call(function_name);
    }

#ifndef VERBOSE_COMPARE_ERRORS
    return;
#endif


#ifdef VERIFY_HIDE_SMALL_ERRORS_AURORA
    if (error != 2)
        return;
#else
    if (error == 0)
        return;
#endif

    if (error == 2) {
       // printf("\nBig ");
    } else {
       // printf("\nSmall ");
    }

  //  printf ("block error detected in %s (%d big errors, %d small errors, "
           /*"meaning %d errors in %d compares, %.2f%%).\n",
			
            function_name, compare_big_errors, compare_small_errors,
            (compare_big_errors+compare_small_errors),compare_calls,
            (((compare_big_errors+compare_small_errors)
              /((float)compare_calls))*100));*/
    //printf("Aurora data pointer: %p, Reference Data Pointer: %p\n",
           aurora_block, ref_block);
    for (x = 0; x < width; x++)
       // printf("( P , A , R )");
    if (width <= 14)
       // printf("    ( P         )");
    //printf("\n");
    for (y = 0; y < h; y++) {
        for (x =  0; x < width; x++) {
            uint8_t aur = *(aurora_block+y*dst_line_size+x);
            uint8_t ref = *(   ref_block+y*dst_line_size+x);
            uint8_t pix = *(      pixels+y*src_line_size+x);
            if (aur != ref)
               // printf ("(%3d,%3d,%3d)", pix, aur, ref);
            else
                //printf ("(%3d,%3d    )", pix, aur);
        }
        if (width <= 14)
           // printf("    (%3d        )",*(pixels+y*src_line_size+width));
        //printf ("\n");
    }
   // printf ("\n");
    for (x =  0; x < width; x++) {
        //printf("(%3d        )", *(pixels+((h+1)*src_line_size)+x));
    }
    if (width <= 14)
       // printf("    (%3d        )",*(pixels+((h+1)*src_line_size)+width));
    //printf("\n\n");
}

static void dcompare ( char* function_name, int width,
                       const DCTELEM *daurora_block, const DCTELEM *dref_block,
                       int dst_line_size, int src_line_size,
                       int h, const uint8_t *pixels )
{
    int loop1,loop2;

    uint8_t *aurora_block = (uint8_t *)
                              MALLOC(dst_line_size * h * sizeof(uint8_t));
    uint8_t *ref_block = (uint8_t *)
                           MALLOC(dst_line_size * h * sizeof(uint8_t));

    for (loop1 = 0; loop1 < h; loop1++) {
        for (loop2 = 0; loop2 < dst_line_size; loop2++) {
            *(aurora_block+(loop1*dst_line_size)+loop2)
                = (uint8_t) (*(daurora_block+(loop1*dst_line_size)+loop2)%256);
            *(ref_block+(loop1*dst_line_size)+loop2)
                = (uint8_t) (*(dref_block+(loop1*dst_line_size)+loop2) % 256);
        }
    }

    compare(function_name, width, aurora_block, ref_block, dst_line_size,
            src_line_size, h, pixels);

    FREE(aurora_block);
    FREE(ref_block);
}
#endif

#ifdef HAVE_AURORA_MODEL /* VR[i][j] doesn't exist without the model. */
#if 0
/* Broken... Possibly can't use byte addressing on a short array. */
static void print_vr1 (int r)
{
    int i;
    uint8_t *test=(uint8_t)VR[r];
    for (i = 15; i >= 0; i--)
    {
        //printf ("%6d ", (uint8_t)test[i]);
    }
}
#endif
static void print_vr (int r)
{
    int i;

    if (r < 10)
       // printf("VR0%d:\n",r);
    else
       // printf("VR%d:\n",r);
    for (i = 7; i >= 0; i--)
    {
     //   printf ("%6d ", i);
    }
   // printf("\n");
    for (i = 7; i >= 0; i--)
    {
       // printf ("%6d ", VR[r][i]);
    }
   // printf("\n"); fflush(stdout);
}

static void print_vr_byte (int r)
{
    int i;

    if (r < 10)
      //  printf("VR0%d:\n",r);
    else
        //printf("VR%d:\n",r);
    for (i = 7; i >= 0; i--)
    {
        //printf ("%8d", i);
    }
    
    uint8_t *tmp = (uint8_t *)VR[r];
    for (i = 15; i >= 0; i--)
    {
       // printf("%4d",*(tmp+i));
    }
}
#endif

/*
 * Function name explanation:
 *
 * Format: ACTION_pixelsN_DIR_ARCH
 *
 * ACTION: put - Copy pixels from input block to output block, with averaging
 *               if a DIR is in the function name.
 *         avg - Does the same as put but then averages that value with the
 *               blocks current value.
 *
 * N: Deal with rows of N pixels starting at the given pointer. We only deal
 *    with the first N pixels and then move onto the next row.
 *
 * DIR: x2 - Average based on current pixel + one to the right.
 *      y2 - Average based on current pixel + one below.
 *      xy2 - Average based on a 4-pixel square with current pixel being the
 *            top left corner.
 *
 * ARCH: ref - A basic (not at all optimal) C implementation of the function,
 *             used to verify the aurora version.
 *       aurora - Hand-coded assembly version of the function.
 */

/*--------------------------------------------------------------------------*/
/*---------------------------------Put Functions----------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels16_y2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels (*(pixels+x), *(pixels+x+line_size));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

/* This Is Scheduled and urolled three times for optimization and it gives
   eight averages using in total six vectors yet one step is required for
   reducing Scaler instructions
 * In This function after each iteration of loop, V01-V04 act as refrence and
   we load again four vectors to do arithmatic.
 * In this function I have used V03 and V04 are used up again for optimization
*/
_Asm void put_pixels16_y2_prolog_asm (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovzw %vr22,block,0x01
  vld128 %vr01,[%i1,0x00]
  vld128 %vr02,[%i1,0x20]
  vbmulw %vr23,%vr23,0x04
  vld128 %vr03,[%i1,0x40]
  vld128 %vr04,[%i1,0x60]
  vaddw  %vr00,%vr23,%vr00
  vmvw   %vr00,%vr22,0x01
  vaddw  %vr23,%vr23,%vr23
}

_Asm void put_pixels16_y2_asm (void)
{
  /* First Block Now
	 * V01-V04 act as reference
	 * vectors which have been pre loaded
	 * and will load V05,V06,V03,V04 which
	 * will act as reference to next block.
  */
  vld128 %vr05,[%i1,0x0]
  vavrb  %vr01,%vr01,%vr02
  vld128 %vr06,[%i1,0x20]
  vavrb  %vr02,%vr03,%vr02
  vst128 %vr01,[%i0,0x00]
  // Though V01 and V02 are now free and should have been used but this was breaking up
  // the generality of loop ie refrence vector's order should be V01-V04 and not V03,V04,V01,V02
  vavrb  %vr03,%vr03,%vr04
  vavrb  %vr04,%vr04,%vr05
  vst128 %vr02,[%i0,0x20]
  vst128 %vr03,[%i0,0x40] // V03 is now free so is used
  vld128 %vr03,[%i1,0x40]
  vst128 %vr04,[%i0,0x60] // V04 is now free so is used
  vld128 %vr04,[%i1,0x60]
  /* Second Block
	 * V05,V06,V03,V04 respectively act
	 * as reference to this block as they
	 * are preloaded and we'll load V01-V04
	 * which will act as reference to next
	 * time first block.
  */
  vld128 %vr01,[%i1,0x80]
  vavrb  %vr05,%vr05,%vr06
  vld128 %vr02,[%i1,0xA0]
  vavrb  %vr06,%vr03,%vr06
  vst128 %vr05,[%i0,0x80]
  vavrb  %vr03,%vr03,%vr04
  vavrb  %vr04,%vr04,%vr01
  vst128 %vr06,[%i0,0xA0]
  vst128 %vr03,[%i0,0xC0]
  vld128 %vr03,[%i1,0xC0]
  vst128 %vr04,[%i0,0xE0]
  vld128 %vr04,[%i1,0xE0]
  vaddw  %vr00,%vr23,%vr00
}

static void put_pixels16_y2_aurora (uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VLD128 (V01, I1, 0x0);
    VLD128 (V02, I1, 0x20);
    VBMULWi(V23,V23,4);
    VLD128 (V03, I1, 0x40);
    VLD128 (V04, I1, 0x60);
    VADDW(V00,V23,V00);
    VMVW(V00,V22,0x1);
    VADDW(V23,V23,V23);
#else
  put_pixels16_y2_prolog_asm(block,pixels,line_size);
#endif
  int y= (h>>3);
  int i;
  for (i = 0; i < y; i++)
  {
    put_pixels16_y2_asm();
  }
#ifdef VERIFY_AURORA
  VERIFY_AURORA_MPEG2_POST_ASM(put_pixels16_y2, 16)
#endif
}


/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels16_x2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels (*(pixels+x), *(pixels+x+1));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

/* This Is Scheduled and urolled three times for optimization yet one step is
 * required for reducing Scaler instructions */

_Asm void put_pixels16_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x06
  vmovzw %vr23,line_size,0x03
  vmovw  %vr00,block,0x01
  vbmulw %vr23,%vr23,0x04
  vld128 %vr01,[%i2,0x00]
}

_Asm void put_pixels16_x2_asm (void)
{
  vld128 %vr02,[%i1,0x10]
  vld128 %vr03,[%i1,0x20]
  vld128 %vr04,[%i1,0x30]
  vmrb   %vr02,%vr02,%vr01
  vld128 %vr05,[%i1,0x40]
  vld128 %vr06,[%i1,0x50]
  vld128 %vr07,[%i1,0x60]
  vld128 %vr08,[%i1,0x70]
  vmrb   %vr04,%vr04,%vr03
  vavrb  %vr02,%vr01,%vr02
  vmrb   %vr06,%vr06,%vr05
  vmrb   %vr08,%vr08,%vr07
  vavrb  %vr04,%vr03,%vr04
  vst128 %vr02,[%i0,0x00]
  vavrb  %vr06,%vr05,%vr06
  vavrb  %vr08,%vr07,%vr08
  vst128 %vr04,[%i0,0x20]
  vld128 %vr01,[%i1,0x80]
  // One instruction was required here so filled
  // it up by next loop's iterations instruction.
  vst128 %vr06,[%i0,0x40]
  vst128 %vr08,[%i0,0x60]
  vaddw  %vr00,%vr23,%vr00
  // Two stalls after this instruction and start of next loop.
}

static void put_pixels16_x2_aurora (uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
#ifdef VERIFY_AURORA
  VMOVZW(V00,R1,0x6);
  VMOVZW(V23,R2,0x3);
  VMOVW(V00,R0,0x1);    //Two Stalls
  VERIFY_AURORA_MPEG2_PRE_ASM(16)
  VBMULWi(V23,V23,4);
  VLD128(V01,I2,0x0); //  Initial load is done in order to schedule properly
#else
  put_pixels16_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      put_pixels16_x2_asm();
    }
#ifdef VERIFY_AURORA
  VERIFY_AURORA_MPEG2_POST_ASM(put_pixels16_x2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels16_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <16; x++) {
            *(block+x) = avg_pixels4 (*(pixels+x),
                                      *(pixels+x+1),
                                      *(pixels+x+line_size),
                                      *(pixels+x+line_size+1));
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void put_pixels16_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x06
  vmovzw %vr20,line_size,0x03
  vmovzw %vr22,block,0x01
  vld128 %vr02,[%i1,0x10]
  vaddw  %vr20,%vr20,%vr20
  vld128 %vr03,[%i1,0x20]
  vld128 %vr04,[%i1,0x30]
  vaddw  %vr00,%vr20,%vr00
  vld128 %vr01,[%i2,0x00]
  vaddw  %vr20,%vr20,%vr20
  vmvw   %vr00,%vr22,0x01
  vmrb   %vr02,%vr02,%vr01
  vmrb   %vr04,%vr04,%vr03
}

_Asm void put_pixels16_xy2_asm (void)
{
  vld128 %vr05,[%i1,0x00]
  vld128 %vr06,[%i1,0x10]
  vld128 %vr07,[%i1,0x20]
  vld128 %vr08,[%i1,0x30]
  vmrb   %vr06,%vr06,%vr05
  VAVRB4(09,01,02,03,04)
  VAVRB4(10,05,06,03,04)
  vst128 %vr09,[%i0,0x00]
  vmrb   %vr08,%vr08,%vr07
  vst128 %vr10,[%i0,0x20]
  vld128 %vr01,[%i1,0x40]
  vld128 %vr02,[%i1,0x50]
  vld128 %vr03,[%i1,0x60]
  vld128 %vr04,[%i1,0x70]
  vmrb   %vr02,%vr02,%vr01
  VAVRB4(09,05,06,07,08)
  VAVRB4(10,01,02,07,08)
  vst128 %vr09,[%i0,0x40]
  vmrb   %vr04,%vr04,%vr03
  vst128 %vr10,[%i0,0x60]
  vaddw  %vr00,%vr20,%vr00
}

static void put_pixels16_xy2_aurora( uint8_t *block, const uint8_t *pixels,
                                     int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x6);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V22,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VLD128(V02,I1,0x10);
    VADDW(V20,V20,V20);
    VLD128(V03,I1,0x20);
    VLD128(V04,I1,0x30);
    VADDW(V00,V20,V00);
    VLD128(V01,I2,0x0);
    VADDW(V20,V20,V20);
    VMVW(V00,V22,0x1);
    VMRB(V02,V02,V01);
    VMRB(V04,V04,V03);
#else
    put_pixels16_xy2_prolog(block,pixels,line_size);
#endif

    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      put_pixels16_xy2_asm();
      //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels16_xy2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels8_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <8; x++) {
            *(block+x) = avg_pixels4 (*(pixels+x),
                                      *(pixels+x+1),
                                      *(pixels+x+line_size),
                                      *(pixels+x+line_size+1));
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void put_pixels8_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw  %vr00,pixels,0x02
  vmovzw  %vr20,line_size,0x03
  vmovzw  %vr22,block,0x01
  vld64w  %vr01,[%i1,0x00]
  vaddw   %vr20,%vr20,%vr20
  vld64w  %vr02,[%i1,0x08]
  vld64w  %vr03,[%i1,0x10]
  vld64w  %vr04,[%i1,0x18]
  vaddw   %vr00,%vr20,%vr00
  vaddw   %vr20,%vr20,%vr20
  vmvw    %vr00,%vr22,0x01
  vmr1w   %vr02,%vr02,%vr01
  vmr1w   %vr04,%vr04,%vr03
  vasrpwb %vr01,%vr01,0x00
  vasrpwb %vr02,%vr02,0x00
}

_Asm void put_pixels8_xy2_asm (void)
{
  vld64w  %vr05,[%i1,0x00]
  vld64w  %vr06,[%i1,0x08]
  vasrpwb %vr03,%vr03,0x00
  vasrpwb %vr04,%vr04,0x00
  vmr1w   %vr06,%vr06,%vr05
  vld64w  %vr07,[%i1,0x10]
  vld64w  %vr08,[%i1,0x18]
  vasrpwb %vr05,%vr05,0x00
  vasrpwb %vr06,%vr06,0x00
  VAVRB4(09,01,02,03,04)
  VAVRB4(10,05,06,03,04)
  vst64   %vr09,[%i0,0x00]
  vmr1w   %vr08,%vr08,%vr07
  vst64   %vr10,[%i0,0x10]
  vld64w  %vr01,[%i1,0x20]
  vld64w  %vr02,[%i1,0x28]
  vasrpwb %vr07,%vr07,0x00
  vasrpwb %vr08,%vr08,0x00
  vmr1w   %vr02,%vr02,%vr01
  vld64w  %vr03,[%i1,0x30]
  vld64w  %vr04,[%i1,0x38]
  vasrpwb %vr01,%vr01,0x00
  vasrpwb %vr02,%vr02,0x00
  VAVRB4(09,05,06,07,08)
  VAVRB4(10,07,08,01,02)
  vst64   %vr09,[%i0,0x20]
  vmr1w   %vr04,%vr04,%vr03
  vst64   %vr10,[%i0,0x30]
  vaddw   %vr00,%vr20,%vr00
}

/* This is Scheduled with Stalls mentioned. */
static void put_pixels8_xy2_aurora( uint8_t *block, const uint8_t *pixels,
                                    int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V22,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VLD64W(V01,I1, 0x0);
    VADDW(V20,V20,V20);
    VLD64W(V02,I1,0x8);
    VLD64W(V03,I1,0x10);
    VLD64W(V04,I1,0x18);
    VADDW(V00,V20,V00);
    VADDW(V20,V20,V20);
    VMVW(V00,V22,0x1);
    VMR1W(V02,V02,V01);
    VMR1W(V04,V04,V03);
    VASRPWB(V01,V01,0);
    VASRPWB(V02,V02,0);
#else
    put_pixels8_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      put_pixels8_xy2_asm();
      //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels8_xy2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels8_x2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels (*(pixels+x), *(pixels+x+1));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void put_pixels8_x2_prolog (uint8_t *block,const uint8_t *pixels,int line_size)
{
  % reg block, pixels, line_size

  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovw  %vr00,block,0x01
  vbmulw %vr23,%vr23,0x04
  vld64w %vr01,[%i1,0x00] // Initial load is done in order to schedule properly
  vld64w %vr02,[%i1,0x08]
}

_Asm void put_pixels8_x2_asm (void)
{
  vld64w %vr03,[%i1,0x10]
  vld64w %vr04,[%i1,0x18]
  vmr1w  %vr02,%vr02,%vr01
  vld64w %vr05,[%i1,0x20]
  vld64w %vr06,[%i1,0x28]
  vld64w %vr07,[%i1,0x30]
  vld64w %vr08,[%i1,0x38]
  vmr1w  %vr04,%vr04,%vr03
  vavrb  %vr02,%vr01,%vr02
  vmr1w  %vr06,%vr06,%vr05
  vmr1w  %vr08,%vr08,%vr07
  vasrpwb %vr02,%vr02,0x00
  vavrb  %vr04,%vr03,%vr04
  vavrb  %vr06,%vr05,%vr06
  vst64  %vr02,[%i0,0x00]
  vasrpwb %vr04,%vr04,0x00
  vasrpwb %vr06,%vr06,0x00
  vavrb  %vr08,%vr07,%vr08
  vld64w %vr01,[%i1,0x40]  // One instruction was required here so filled
                           // it up by next loop's iterations instruction.
  vst64  %vr04,[%i0,0x10]
  vasrpwb %vr08,%vr08,0x00
  vst64  %vr06,[%i0,0x20]
  vld64w %vr02,[%i1,0x48]
  vst64  %vr08,[%i0,0x30]
  vaddw  %vr00,%vr23,%vr00
  // Two stalls after this instruction and start of next loop
}

/* This function is scheduled as possible i could have done that but still
 * there are stalls and those are properly mentioned.
 */
static void put_pixels8_x2_aurora( uint8_t *block, /* R0 */
                                   const uint8_t *pixels, /* R1 */
                                   int line_size, /* R2 */
                                   int h) /* R3 */
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);  // Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V23,V23,4);
    VLD64W(V01, I1, 0x0); // Initial load is done in order to schedule properly
    VLD64W(V02, I1, 0x8);
#else
    put_pixels8_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      put_pixels8_x2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels8_x2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels8_y2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels (*(pixels+x), *(pixels+x+line_size));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

_Asm void put_pixels8_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovw  %vr00,block,0x01
  vbmulw %vr23,%vr23,0x04
  vld64w %vr01,[%i1,0x00]
}

_Asm void put_pixels8_y2_asm (void)
{
  vld64w %vr02,[%i1,0x10]
  vld64w %vr03,[%i1,0x20]
  vld64w %vr04,[%i1,0x30]
  vavrb  %vr01,%vr01,%vr02
  vavrb  %vr02,%vr03,%vr02
  vasrpwb %vr01,%vr01,0x00
  vasrpwb %vr02,%vr02,0x00
  vavrb  %vr03,%vr03,%vr04
  vst64  %vr01,[%i0,0x00]
  vld64w %vr01,[%i1,0x40]
  vasrpwb %vr03,%vr03,0x00
  vavrb  %vr04,%vr04,%vr01
  vst64  %vr02,[%i0,0x10]
  vasrpwb %vr04,%vr04,0x00
  vst64  %vr03,[%i0,0x20]
  vst64  %vr04,[%i0,0x30]
  vaddw  %vr00,%vr23,%vr00
}

static void put_pixels8_y2_aurora( uint8_t *block, const uint8_t *pixels,
                                   int line_size, int h )
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V23,V23,4);
    VLD64W (V01, I1, 0x0);
 #else
    put_pixels8_y2_prolog(block,pixels,line_size);
 #endif
    int y=( h >> 2);
    int i;
    for (i = 0; i < y; i++)
    {
      put_pixels8_y2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels8_y2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels8_ref( uint8_t *block, const uint8_t *pixels,
                             int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = *(pixels+x);
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

/////////////////////////////////////////////////////////////////
/*Cut Short This function by simply giving a DMA_out from here */
/////////////////////////////////////////////////////////////////

_Asm void put_pixels8_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size

  vmovzw %vr15,line_size,0x03
  vmovzw %vr00,pixels,0x02
  vmovw  %vr00,block,0x01
  vbmulw %vr15,%vr15,0x04
}

_Asm void put_pixels8_asm (void)
{
  vld64  %vr01,[%i1,0x00]
  vld64  %vr02,[%i1,0x10]
  vld64  %vr03,[%i1,0x20]
  vld64  %vr04,[%i1,0x30]
  vst64  %vr01,[%i0,0x00]
  vst64  %vr02,[%i0,0x10]
  vst64  %vr03,[%i0,0x20]
  vst64  %vr04,[%i0,0x30]
  vaddw  %vr00,%vr15,%vr00
}

/* This Function has been Scheduled  with stalls mentioned. */
static void put_pixels8_aurora( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V15,R2,0x3);
    VMOVZW(V00,R1,0x2);
    VMOVW(V00,R0,0x1);
    //Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V15,V15,4);
#else
    put_pixels8_prolog (block,pixels,line_size);
#endif
    int i;
    for(i=0 ; i<(h>>2) ; i++)
    {
      put_pixels8_asm();
        //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels8, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_pixels16_ref( uint8_t *block, const uint8_t *pixels,
                             int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = *(pixels+x);
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

/////////////////////////////////////////////////////////////////
/*Cut Short This function by simply giving a DMA_out from here */
/////////////////////////////////////////////////////////////////

_Asm void put_pixels16_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovw  %vr00,block,0x01
  vbmulw %vr23,%vr23,0x08
}

_Asm void put_pixels16_asm (void)
{
  vld128 %vr01,[%i1,0x00]
  vld128 %vr02,[%i1,0x20]
  vld128 %vr03,[%i1,0x40]
  vld128 %vr04,[%i1,0x60]
  vld128 %vr05,[%i1,0x80]
  vld128 %vr06,[%i1,0xA0]
  vld128 %vr07,[%i1,0xC0]
  vld128 %vr08,[%i1,0xE0]
  vst128 %vr01,[%i0,0x00]
  vst128 %vr02,[%i0,0x20]
  vst128 %vr03,[%i0,0x40]
  vst128 %vr04,[%i0,0x60]
  vst128 %vr05,[%i0,0x80]
  vst128 %vr06,[%i0,0xA0]
  vst128 %vr07,[%i0,0xC0]
  vst128 %vr08,[%i0,0xE0]
  vaddw  %vr00,%vr23,%vr00
}

static void put_pixels16_aurora( uint8_t *block, const uint8_t *pixels,
                                 int line_size,int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    //Two Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VBMULWi(V23,V23,8);
#else
    put_pixels16_prolog(block,pixels,line_size);
#endif
    int i;
    for(i=0 ; i<(h>>3) ; i++)
    {
      put_pixels16_asm();
    }

#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_pixels16, 16)
#endif
}


/*------------------------------Average Functions----------------------------*/

/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels8_ref( uint8_t *block, const uint8_t *pixels,
                             int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels(*(block+x),*(pixels+x));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

_Asm void avg_pixels8_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  %reg block, pixels, line_size
  vmovzw %vr23,line_size,0x03
  vmovzw %vr00,pixels,0x02
  vmovw %vr00,block,0x01
  //Three Stalls
  vbmulw %vr23,%vr23,0x04
}

_Asm void avg_pixels8_asm (void)
{
  vld64w %vr01,[%i1,0x00]
  vld64w %vr02,[%i1,0x10]
  vld64w %vr03,[%i1,0x20]
  vld64w %vr04,[%i1,0x30]
  vld64w %vr05,[%i0,0x00]
  vld64w %vr06,[%i0,0x10]
  vld64w %vr07,[%i0,0x20]
  vld64w %vr08,[%i0,0x30]
  vavrb  %vr01,%vr01,%vr05
  vavrb  %vr02,%vr02,%vr06
  vavrb  %vr03,%vr03,%vr07
  vavrb  %vr04,%vr04,%vr08
  vasrpwb %vr01,%vr01,0
  vasrpwb %vr02,%vr02,0
  vasrpwb %vr03,%vr03,0
  vasrpwb %vr04,%vr04,0
  vst64  %vr01,[%i0,0x00]
  vst64  %vr02,[%i0,0x10]
  vst64  %vr03,[%i0,0x20]
  vst64  %vr04,[%i0,0x30]
  vaddw  %vr00,%vr23,%vr00
}

static void avg_pixels8_aurora( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V23,R2,0x3);
    VMOVZW(V00,R1,0x2);
    VMOVW(V00,R0,0x1);
    //Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V23,V23,4);
#else
    avg_pixels8_prolog(block,pixels,line_size);
#endif
    int i;
    for(i=0 ; i<(h>>2) ; i++)
    {
      avg_pixels8_asm();
        //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels8, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels16_ref( uint8_t *block, const uint8_t *pixels,
                             int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels (*(block+x), *(pixels+x));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

/* This function has been unrolled two times and is scheduled  with stalls
 * mentioned. */

_Asm void avg_pixels16_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size

  vmovzw %vr23,line_size,0x03
  vmovzw %vr00,pixels,0x02
  vmovw %vr00,block,0x01
  //Three Stalls
  vbmulw %vr23,%vr23,0x08
}

_Asm void avg_pixels16_asm (void)
{
  vld128 %vr01,[%i1,0x00]
  vld128 %vr02,[%i1,0x20]
  vld128 %vr03,[%i1,0x40]
  vld128 %vr04,[%i1,0x60]
  vld128 %vr05,[%i1,0x80]
  vld128 %vr06,[%i1,0xA0]
  vld128 %vr07,[%i1,0xC0]
  vld128 %vr08,[%i1,0xE0]
  vld128 %vr09,[%i0,0x00]
  vld128 %vr10,[%i0,0x20]
  vld128 %vr11,[%i0,0x40]
  vld128 %vr12,[%i0,0x60]
  vld128 %vr13,[%i0,0x80]
  vld128 %vr14,[%i0,0xA0]
  vld128 %vr15,[%i0,0xC0]
  vld128 %vr16,[%i0,0xE0]
  vavrb  %vr01,%vr09,%vr01
  vavrb  %vr02,%vr02,%vr10
  vavrb  %vr03,%vr03,%vr11
  vavrb  %vr04,%vr04,%vr12
  vavrb  %vr05,%vr05,%vr13
  vavrb  %vr06,%vr06,%vr14
  vavrb  %vr07,%vr07,%vr15
  vavrb  %vr08,%vr08,%vr16
  vst128 %vr01,[%i0,0x00]
  vst128 %vr02,[%i0,0x20]
  vst128 %vr03,[%i0,0x40]
  vst128 %vr04,[%i0,0x60]
  vst128 %vr05,[%i0,0x80]
  vst128 %vr06,[%i0,0xA0]
  vst128 %vr07,[%i0,0xC0]
  vst128 %vr08,[%i0,0xE0]
  vaddw  %vr00,%vr23,%vr00
}

static void avg_pixels16_aurora( uint8_t *block, const uint8_t *pixels,
                                 int line_size,int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V23,R2,0x3);
    VMOVZW(V00,R1,0x2);
    VMOVW(V00,R0,0x1);
    //Three stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VBMULWi(V23,V23,8);
#else
    avg_pixels16_prolog(block,pixels,line_size);
#endif
    int i;
    for(i=0 ; i<(h>>3) ; i++)
    {
      avg_pixels16_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels16, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels8_y2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     avg_pixels(*(pixels+x),
                                                *(pixels+x+line_size)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

/* This function is unrolled only two times since i was not sure h can never
 * be 4 and is scheduled and stalls have been marked. */
_Asm void avg_pixels8_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size

  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovzw %vr22,block,0x01
  vld64  %vr01,[%i1,0x00]
  vaddw  %vr23,%vr23,%vr23
  vld64  %vr02,[%i1,0x10]
  vaddw  %vr00,%vr23,%vr00
  vmvw   %vr00,%vr22,0x01
  vaddw  %vr23,%vr23,%vr23
}

_Asm void avg_pixels8_y2_asm (void)
{
  vavrb  %vr01,%vr01,%vr02
  //two stalls
  vld64  %vr03,[%i1,0x00]
  vld64  %vr05,[%i0,0x00]
  vld64  %vr06,[%i0,0x10]
  vavrb  %vr02,%vr02,%vr03
  vavrb  %vr01,%vr01,%vr05
  vld64  %vr04,[%i1,0x10]
  vavrb  %vr02,%vr02,%vr06
  //one stall
  vst64  %vr01,[%i0,0x00]
  vst64  %vr02,[%i0,0x10]
  vavrb  %vr03,%vr03,%vr04
  vld64  %vr01,[%i1,0x20]
  vld64  %vr05,[%i0,0x20]
  vld64  %vr06,[%i0,0x30]
  vavrb  %vr04,%vr04,%vr01
  vavrb  %vr03,%vr03,%vr05
  vld64  %vr02,[%i1,0x30]
  vavrb  %vr04,%vr04,%vr06
  //one stall
  vst64  %vr03,[%i0,0x20]
  vst64  %vr04,[%i0,0x30]
  vaddw  %vr00,%vr00,%vr23
}

static void avg_pixels8_y2_aurora( uint8_t *block, const uint8_t *pixels,
                                   int line_size, int h )
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VLD64(V01, I1, 0x0);
    VADDW(V23,V23,V23);
    VLD64(V02, I1, 0x10);
    VADDW(V00,V23,V00);
    VMVW(V00,V22,0x1);
    VADDW(V23,V23,V23);
#else
    avg_pixels8_y2_prolog (block,pixels,line_size);
#endif
    int i;
    for(i=0;i<(h>>2);i++)
    {
      avg_pixels8_y2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels8_y2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels16_y2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++) {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     avg_pixels(*(pixels+x),
                                                *(pixels+x+line_size)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_pixels16_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovzw %vr22,block,0x01
  vld128 %vr01,[%i1,0x00]
  vbmulw %vr23,%vr23,0x04
  vld128 %vr02,[%i1,0x20]
  vld128 %vr03,[%i1,0x40]
  vld128 %vr04,[%i1,0x60]
  vaddw  %vr00,%vr23,%vr00
  vaddw  %vr23,%vr23,%vr23
  vmvw   %vr00,%vr22,0x01
}

_Asm void avg_pixels16_y2_asm (void)
{
  vavrb  %vr01,%vr01,%vr02
  vavrb  %vr02,%vr02,%vr03
  vavrb  %vr03,%vr03,%vr04
  vld128 %vr05,[%i1,0x00]
  vld128 %vr09,[%i0,0x00]
  vld128 %vr10,[%i0,0x20]
  vavrb  %vr04,%vr04,%vr05
  vld128 %vr11,[%i0,0x40]
  vld128 %vr12,[%i0,0x60]
  vld128 %vr06,[%i1,0x20]
  vld128 %vr07,[%i1,0x40]
  vld128 %vr08,[%i1,0x60]
  vavrb  %vr01,%vr01,%vr09
  vavrb  %vr02,%vr02,%vr10
  vavrb  %vr03,%vr03,%vr11
  vavrb  %vr04,%vr04,%vr12
  vst128 %vr01,[%i0,0x00]
  vst128 %vr02,[%i0,0x20]
  vst128 %vr03,[%i0,0x40]
  vst128 %vr04,[%i0,0x60]
  vavrb  %vr05,%vr05,%vr06
  vavrb  %vr06,%vr06,%vr07
  vavrb  %vr07,%vr07,%vr08
  vld128 %vr01,[%i1,0x80]
  vld128 %vr09,[%i0,0x80]
  vld128 %vr10,[%i0,0xA0]
  vavrb  %vr08,%vr08,%vr01
  vld128 %vr11,[%i0,0xC0]
  vld128 %vr12,[%i0,0xE0]
  vavrb  %vr05,%vr05,%vr09
  vavrb  %vr06,%vr06,%vr10
  vavrb  %vr07,%vr07,%vr11
  vavrb  %vr08,%vr08,%vr12
  vld128 %vr02,[%i1,0xA0]
  vld128 %vr03,[%i1,0xC0]
  vld128 %vr04,[%i1,0xE0]
  vst128 %vr05,[%i0,0x80]
  vst128 %vr06,[%i0,0xA0]
  vst128 %vr07,[%i0,0xC0]
  vst128 %vr08,[%i0,0xE0]
  vaddw  %vr00,%vr23,%vr00  //If confident that h=8 always then omit this instruction
}

static void avg_pixels16_y2_aurora( uint8_t *block, const uint8_t *pixels,
                                    int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VLD128(V01, I1, 0x0);
    VBMULWi(V23,V23,4);
    VLD128(V02, I1, 0x20);
    VLD128(V03, I1, 0x40);
    VLD128(V04, I1, 0x60);
    VADDW(V00,V23,V00);
    VADDW(V23,V23,V23);
    VMVW(V00,V22,0x1);
#else
    avg_pixels16_y2_prolog (block,pixels,line_size);
#endif
    int y=( h>> 3);
    int i;
    for (i = 0; i < y; i++)
    {
      avg_pixels16_y2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels16_y2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels16_x2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ ) {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     avg_pixels(*(pixels+x), *(pixels+x+1)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_pixels16_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr23,line_size,0x03
  vmovw  %vr00,block,0x01
  vbmulw %vr23,%vr23,0x04
}

_Asm void avg_pixels16_x2_asm (void)
{
  vld128 %vr01,[%i1,0x00]
  vld128 %vr02,[%i1,0x10]
  vld128 %vr03,[%i1,0x20]
  vld128 %vr04,[%i1,0x30]
  vld128 %vr05,[%i1,0x40]
  vld128 %vr06,[%i1,0x50]
  vld128 %vr07,[%i1,0x60]
  vld128 %vr08,[%i1,0x70]
  vmrb   %vr02,%vr02,%vr01
  vmrb   %vr04,%vr04,%vr03
  vmrb   %vr06,%vr06,%vr05
  vmrb   %vr08,%vr08,%vr07
  vld128 %vr09,[%i0,0x00]
  vld128 %vr10,[%i0,0x20]
  vld128 %vr11,[%i0,0x40]
  vld128 %vr12,[%i0,0x60]
  vavrb  %vr01,%vr01,%vr02
  vavrb  %vr03,%vr03,%vr04
  vavrb  %vr05,%vr05,%vr06
  vavrb  %vr07,%vr07,%vr08
  vavrb  %vr01,%vr01,%vr09
  vavrb  %vr03,%vr03,%vr10
  vavrb  %vr05,%vr05,%vr11
  vavrb  %vr07,%vr07,%vr12
  vst128 %vr01,[%i0,0x00]
  vst128 %vr03,[%i0,0x20]
  vst128 %vr05,[%i0,0x40]
  vst128 %vr07,[%i0,0x60]
  vaddw  %vr00,%vr23,%vr00
}


static void avg_pixels16_x2_aurora( uint8_t *block, const uint8_t *pixels,
                                    int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    //Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VBMULWi(V23,V23,4);
#else
    avg_pixels16_x2_prolog (block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      avg_pixels16_x2_asm();
      //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels16_x2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels16_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <16; x++) {
            *(block+x) = avg_pixels ( *(block+x),
                                      avg_pixels4 (*(pixels+x),
                                                   *(pixels+x+1),
                                                   *(pixels+x+line_size),
                                                   *(pixels+x+line_size+1)) );
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void avg_pixels16_xy2_prolog (uint8_t *block, const uint8_t *pixels,int line_size)
{
  % reg block, pixels, line_size
  vmovzw %vr00,pixels,0x02
  vmovzw %vr20,line_size,0x03
  vmovzw %vr21,block,0x01

  vld128 %vr01,[%i1,0x00]
  vaddw  %vr20,%vr20,%vr20
  vld128 %vr02,[%i1,0x10]
  vld128 %vr03,[%i1,0x20]
  vld128 %vr04,[%i1,0x30]
  vaddw  %vr00,%vr20,%vr00
  vaddw  %vr20,%vr20,%vr20
  vmvw   %vr00,%vr21,0x01
  vmrb   %vr02,%vr02,%vr01
  vmrb   %vr04,%vr04,%vr03
}

_Asm void avg_pixels16_xy2_asm (void)
{
  vld128 %vr05,[%i1,0x00]
  vld128 %vr06,[%i1,0x10]
  vld128 %vr07,[%i1,0x20]
  vld128 %vr09,[%i0,0x00]
  vmrb   %vr06,%vr06,%vr05
  vld128 %vr10,[%i0,0x20]
  VAVRB4(11,01,02,03,04)
  VAVRB4(12,03,04,05,06)
  vavrb  %vr11,%vr11,%vr09
  vld128 %vr08,[%i1,0x30]
  vavrb  %vr12,%vr12,%vr10
  vst128 %vr11,[%i0,0x00]
  vmrb   %vr08,%vr08,%vr07
  vst128 %vr12,[%i0,0x20]
  vld128 %vr01,[%i1,0x40]
  vld128 %vr02,[%i1,0x50]
  vld128 %vr03,[%i1,0x60]
  vmrb   %vr02,%vr02,%vr01
  vld128 %vr09,[%i0,0x40]
  vld128 %vr10,[%i0,0x60]
  VAVRB4(11,05,06,07,08)
  VAVRB4(12,07,08,01,02)
  vavrb  %vr11,%vr11,%vr09
  vld128 %vr04,[%i1,0x70]
  vavrb  %vr12,%vr12,%vr10
  vst128 %vr11,[%i0,0x40]
  vmrb   %vr04,%vr04,%vr03
  vst128 %vr12,[%i0,0x60]
  vaddw  %vr00,%vr20,%vr00
}

static void avg_pixels16_xy2_aurora( uint8_t *block, const uint8_t *pixels,
                                     int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V21,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VLD128(V01, I1, 0x0);
    VADDW(V20,V20,V20);
    VLD128(V02, I1, 0x10);
    VLD128(V03, I1, 0x20);
    VLD128(V04, I1, 0x30);
    VADDW(V00,V20,V00);
    VADDW(V20,V20,V20);
    VMVW(V00,V21,0x1);
    VMRB(V02,V02,V01);
    VMRB(V04,V04,V03);
#else
    avg_pixels16_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      avg_pixels16_xy2_asm();
        //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels16_xy2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels8_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                 int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <8; x++) {
            *(block+x) = avg_pixels ( *(block+x),
                                      avg_pixels4 (*(pixels+x),
                                                   *(pixels+x+1),
                                                   *(pixels+x+line_size),
                                                   *(pixels+x+line_size+1)) );
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void avg_pixels8_xy2_prolog (uint8_t *block, const uint8_t *pixels,int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr20,line_size,0x3
    vmovzw   %vr21,block,0x1
    vld64w    %vr01,[%i1,0x0]
    vaddw    %vr20,%vr20,%vr20
    vld64w    %vr02,[%i1,0x8]
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vaddw    %vr00,%vr20,%vr00
    vaddw    %vr20,%vr20,%vr20
    vmvw    %vr00,%vr21,0x1
    vmr1w    %vr02,%vr02,%vr01
    vmr1w    %vr04,%vr04,%vr03
}

_Asm void avg_pixels8_xy2_asm (void)
{
    vld64w    %vr05,[%i1,0x0]
    vld64w    %vr06,[%i1,0x8]
    vld64    %vr09,[%i0,0x0]
    vld64    %vr10,[%i0,0x10]
    vmr1w    %vr06,%vr06,%vr05
    VAVRB4(11,01,02,03,04)
    VAVRB4(12,03,04,05,06)
    vasrpwb    %vr11,%vr11,0
    vld64w    %vr07,[%i1,0x10]
    vasrpwb    %vr12,%vr12,0
    vavrb    %vr11,%vr11,%vr09
    vld64w    %vr08,[%i1,0x18]
    vavrb    %vr12,%vr12,%vr10
    vst64    %vr11,[%i0,0x0]
    vmr1w    %vr08,%vr08,%vr07
    vst64    %vr12,[%i0,0x10]
    vld64w    %vr01,[%i1,0x20]
    vld64w    %vr02,[%i1,0x28]
    vld64    %vr09,[%i0,0x20]
    vld64    %vr10,[%i0,0x30]
    vmr1w    %vr02,%vr02,%vr01
    VAVRB4(11,05,06,07,08)
    VAVRB4(12,07,08,01,02)
    vasrpwb    %vr11,%vr11,0
    vld64w    %vr03,[%i1,0x30]
    vasrpwb    %vr12,%vr12,0
    vavrb    %vr11,%vr11,%vr09
    vld64w    %vr04,[%i1,0x38]
    vavrb    %vr12,%vr12,%vr10
    vst64    %vr11,[%i0,0x20]
    vmr1w    %vr04,%vr04,%vr03
    vst64    %vr12,[%i0,0x30]
    vaddw    %vr00,%vr20,%vr00
    //Two Stalls
}

static void avg_pixels8_xy2_aurora( uint8_t *block, const uint8_t *pixels,
                                    int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V21,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VLD64W(V01, I1, 0x0);
    VADDW(V20,V20,V20);
    VLD64W(V02, I1, 0x8);
    VLD64W(V03, I1, 0x10);
    VLD64W(V04, I1, 0x18);
    VADDW(V00,V20,V00);
    VADDW(V20,V20,V20);
    VMVW(V00,V21,0x1);
    VMR1W(V02,V02,V01);
    VMR1W(V04,V04,V03);
#else
    avg_pixels8_xy2_prolog(block,pixels,line_size);
#endif
    //Two Stalls
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      avg_pixels8_xy2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels8_xy2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_pixels8_x2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ ) {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     avg_pixels(*(pixels+x), *(pixels+x+1)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_pixels8_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1
    //three stalls
    vbmulw    %vr23,%vr23,4

}

_Asm void avg_pixels8_x2_asm (void)
{
    vld64w    %vr01,[%i1,0x0]
    vld64w    %vr02,[%i1,0x8]
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vld64w    %vr05,[%i1,0x20]
    vld64w    %vr06,[%i1,0x28]
    vld64w    %vr07,[%i1,0x30]
    vld64w    %vr08,[%i1,0x38]
    vmr1w    %vr02,%vr02,%vr01
    vmr1w    %vr04,%vr04,%vr03
    vmr1w    %vr06,%vr06,%vr05
    vmr1w    %vr08,%vr08,%vr07
    vld64    %vr09,[%i0,0x0]
    vld64    %vr10,[%i0,0x10]
    vld64    %vr11,[%i0,0x20]
    vld64    %vr12,[%i0,0x30]
    vavrb    %vr01,%vr01,%vr02
    vavrb    %vr03,%vr03,%vr04
    vavrb    %vr05,%vr05,%vr06
    vavrb    %vr07,%vr07,%vr08
    vasrpwb    %vr01,%vr01,0
    vasrpwb    %vr03,%vr03,0
    vasrpwb    %vr05,%vr05,0
    vasrpwb    %vr07,%vr07,0
    vavrb    %vr01,%vr01,%vr09
    vavrb    %vr03,%vr03,%vr10
    vavrb    %vr05,%vr05,%vr11
    vavrb    %vr07,%vr07,%vr12
    vst64    %vr01,[%i0,0x0]
    vst64    %vr03,[%i0,0x10]
    vst64    %vr05,[%i0,0x20]
    vst64    %vr07,[%i0,0x30]
    vaddw    %vr00,%vr23,%vr00
}

static void avg_pixels8_x2_aurora( uint8_t *block, const uint8_t *pixels,
                                   int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    //tHree Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V23,V23,4);
#else
    avg_pixels8_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      avg_pixels8_x2_asm();
        //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_pixels8_x2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#if 0 /* We do this at the end of idct's instead. */
#ifdef VERIFY_AURORA
static void clear_blocks_ref(DCTELEM *blocks)
{
    memset(blocks, 0, sizeof(DCTELEM)*6*64);
}
#endif

static void clear_blocks_aurora(DCTELEM *blocks)
{
    VMOVZW(V00,R0,0x1);

    VERIFY_AURORA_SINGLE_DBLOCK_PRE_ASM

    VXOR(V01,V01,V01);

    /* sizeof(DCTELEM) = 2 taken */
    ASSIGN_SCALAR_REG(10,128);
    VMOVZW(V23,R10,0x1);

    int i;
    for( i =0; i < 6; i++)
    {
        VST128(V01, I0, 0x0);
        VST128(V01, I0, 0x10);
        VST128(V01, I0, 0x20);
        VST128(V01, I0, 0x30);
        VST128(V01, I0, 0x40);
        VST128(V01, I0, 0x50);
        VST128(V01, I0, 0x60);
        VST128(V01, I0, 0x70);
        VADDW(V00,V00,V23);
    }

    VERIFY_AURORA_SINGLE_DBLOCK_POST_ASM(clear_blocks)
}
#endif

/*--------------------------------------------------------------------------*/



/*---------------------------------------------*/
/*---------No Rounding functions---------------*/
/*---------------------------------------------*/

/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels16_y2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = no_rnd_avg_pixels (*(pixels+x), *(pixels+x+line_size));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

_Asm void put_no_rnd_pixels16_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovzw   %vr22,block,0x1
    vld128    %vr01,[%i1,0x0]
    vld128    %vr02,[%i1,0x20]
    vbmulw    %vr23,%vr23,4
    vld128    %vr03,[%i1,0x40]
    vld128    %vr04,[%i1,0x60]
    vaddw    %vr00,%vr23,%vr00
    vaddw    %vr23,%vr23,%vr23
    vmvw    %vr00,%vr22,0x1
}

_Asm void put_no_rnd_pixels16_y2_asm (void)
{
    /* First Block Now
     * V01-V04 act as reference vectors which have been pre loaded  and will
     * load V05,V06,V03,V04 which  will act as refrence to next block
     */
    vavb    %vr01,%vr01,%vr02
    vavb    %vr02,%vr03,%vr02
    vld128    %vr05,[%i1,0x0]
    vld128    %vr06,[%i1,0x20]
    vst128    %vr01,[%i0,0x0]
    // Though V01 and V02 are now free
    // and should have been used but
    // this was breaking up the generality
    // of loop ie refrence vector's order
    // should be V01-V04 and not V03,V04,V01,V02
    vavb    %vr03,%vr03,%vr04
    vavb    %vr04,%vr04,%vr05
    vst128    %vr02,[%i0,0x20]
    vst128    %vr03,[%i0,0x40]
    vld128    %vr03,[%i1,0x40]
    vst128    %vr04,[%i0,0x60]
    vld128    %vr04,[%i1,0x60]
    /*Second Block
     * V05,V06,V03,V04 respectively act as refernce to this block as they
     * are preloaded and we'll load V01-V04 which will act as refrence to next
     * time first block. */
    vld128    %vr01,[%i1,0x80]
    vavb    %vr05,%vr05,%vr06
    vld128    %vr02,[%i1,0xA0]
    vavb    %vr06,%vr03,%vr06
    vst128    %vr05,[%i0,0x80]
    vavb    %vr03,%vr03,%vr04
    vavb    %vr04,%vr04,%vr01
    vst128    %vr06,[%i0,0xA0]
    vst128    %vr03,[%i0,0xC0]
    vld128    %vr03,[%i1,0xC0]
    vst128    %vr04,[%i0,0xE0]
    vld128    %vr04,[%i1,0xE0]
    vaddw    %vr00,%vr23,%vr00
}

static void put_no_rnd_pixels16_y2_aurora( uint8_t *block,
                                           const uint8_t *pixels,
                                           int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(16)

    VLD128 (V01, I1, 0x0);
    VLD128 (V02, I1, 0x20);
    VBMULWi(V23,V23,4);
    VLD128 (V03, I1, 0x40);
    VLD128 (V04, I1, 0x60);
    VADDW(V00,V23,V00);
    VADDW(V23,V23,V23);
    VMVW(V00,V22,0x1); // Two stalls
#else
    put_no_rnd_pixels16_y2_prolog(block,pixels,line_size);
#endif
    int y=( h>> 3);
    int i;
    for (i = 0; i < y; i++)
    {
      put_no_rnd_pixels16_y2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels16_y2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels16_x2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 16; x++) {
            *(block+x) = no_rnd_avg_pixels (*(pixels+x), *(pixels+x+1));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void put_no_rnd_pixels16_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1
    vbmulw    %vr23,%vr23,4
    vld128    %vr01,[%i1,0x0]
    // Initial load is done in order to schedule properly
}

_Asm void put_no_rnd_pixels16_x2_asm (void)
{
    vld128    %vr02,[%i1,0x10]
    vld128    %vr03,[%i1,0x20]
    vld128    %vr04,[%i1,0x30]
    vmrb    %vr02,%vr02,%vr01
    vld128    %vr05,[%i1,0x40]
    vld128    %vr06,[%i1,0x50]
    vld128    %vr07,[%i1,0x60]
    vld128    %vr08,[%i1,0x70]
    vmrb    %vr04,%vr04,%vr03
    vavb    %vr02,%vr01,%vr02
    vmrb    %vr06,%vr06,%vr05
    vmrb    %vr08,%vr08,%vr07
    vavb    %vr04,%vr03,%vr04
    vst128    %vr02,[%i0,0x0]
    vavb    %vr06,%vr05,%vr06
    vavb    %vr08,%vr07,%vr08
    vst128    %vr04,[%i0,0x20]
    vld128    %vr01,[%i1,0x80]
  // One instruction was required here so filled
  // it up by next loop's iterations instruction
    vst128    %vr06,[%i0,0x40]
    vst128    %vr08,[%i0,0x60]
    vaddw    %vr00,%vr23,%vr00
  // Two stalls after this instruction and start

}

static void put_no_rnd_pixels16_x2_aurora( uint8_t *block,
                                           const uint8_t *pixels,
                                           int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);  // Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VBMULWi(V23,V23,4);
    VLD128(V01, I1, 0x0); // Initial load is done in order to schedule properly
#else
    put_no_rnd_pixels16_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      put_no_rnd_pixels16_x2_asm();
                            // of next loop
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels16_x2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels16_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                         int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <16; x++) {
            *(block+x) = no_rnd_avg_pixels4 (*(pixels+x),
                                             *(pixels+x+1),
                                             *(pixels+x+line_size),
                                             *(pixels+x+line_size+1));
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void put_no_rnd_pixels16_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x6
    vmovzw   %vr20,line_size,0x3
    vmovzw   %vr22,block,0x1

    vld128    %vr02,[%i1,0x10]
    vaddw    %vr20,%vr20,%vr20
    vld128    %vr03,[%i1,0x20]
    vld128    %vr04,[%i1,0x30]
    vaddw    %vr00,%vr20,%vr00
    vld128    %vr01,[%i2,0x0]
    vmvw    %vr00,%vr22,0x1
    vaddw    %vr20,%vr20,%vr20
    vmrb    %vr02,%vr02,%vr01
    vmrb    %vr04,%vr04,%vr03
}

_Asm void put_no_rnd_pixels16_xy2_asm (void)
{
    vld128    %vr05,[%i1,0x0]
    vld128    %vr06,[%i1,0x10]
    vld128    %vr07,[%i1,0x20]
    vld128    %vr08,[%i1,0x30]
    vmrb    %vr06,%vr06,%vr05
    VNAVRB4(09,01,02,03,04)
    VNAVRB4(10,05,06,03,04)
    vst128    %vr09,[%i0,0x0]
    vmrb    %vr08,%vr08,%vr07
    vst128    %vr10,[%i0,0x20]
    vld128    %vr01,[%i1,0x40]
    vld128    %vr02,[%i1,0x50]
    vld128    %vr03,[%i1,0x60]
    vld128    %vr04,[%i1,0x70]
    vmrb    %vr02,%vr02,%vr01
    VNAVRB4(09,05,06,07,08)
    VNAVRB4(10,01,02,07,08)
    vst128    %vr09,[%i0,0x40]
    vmrb    %vr04,%vr04,%vr03
    vst128    %vr10,[%i0,0x60]
    vaddw    %vr00,%vr20,%vr00
}

static void put_no_rnd_pixels16_xy2_aurora( uint8_t *block,
                                             const uint8_t *pixels,
                                             int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x6);
    VMOVZW(V20,R2,0x3);
    //VMOVZW(V21,R1,0x4);
    VMOVZW(V22,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(16)

    VLD128(V02,I1,0x10);
    VADDW(V20,V20,V20);
    VLD128(V03,I1,0x20);
    VLD128(V04,I1,0x30);
    ////VMVW(V00,V21,0x4);  // Three stalls
    VADDW(V00,V20,V00);
    VLD128(V01,I2,0x0);
    VMVW(V00,V22,0x1);  // Stall
    VADDW(V20,V20,V20);
    VMRB(V02,V02,V01);
    VMRB(V04,V04,V03);
#else
    put_no_rnd_pixels16_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      put_no_rnd_pixels16_xy2_asm();
        //Two Stalls
    }

#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels16_xy2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels8_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ ) {
        for (x = 0; x < 8; x++) {
            *(block+x) = no_rnd_avg_pixels4 (*(pixels+x),
                                             *(pixels+x+1),
                                             *(pixels+x+line_size),
                                             *(pixels+x+line_size+1));
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void put_no_rnd_pixels8_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr20,line_size,0x3
    vmovzw   %vr21,block,0x1
    vld64w    %vr01,[%i1,0X0]
    vaddw    %vr20,%vr20,%vr20
    vld64w    %vr02,[%i1,0x8]
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vaddw    %vr00,%vr20,%vr00
    vaddw    %vr20,%vr20,%vr20
    vmvw    %vr00,%vr21,0x1
    vmr1w    %vr02,%vr02,%vr01
    vmr1w    %vr04,%vr04,%vr03
    vasrpwb    %vr01,%vr01,0
    vasrpwb    %vr02,%vr02,0
}

_Asm void put_no_rnd_pixels8_xy2_asm (void)
{
    vld64w    %vr05,[%i1,0x0]
    vld64w    %vr06,[%i1,0x8]
    vasrpwb    %vr03,%vr03,0
    vasrpwb    %vr04,%vr04,0
    vmr1w    %vr06,%vr06,%vr05
    vld64w    %vr07,[%i1,0x10]
    vld64w    %vr08,[%i1,0x18]
    vasrpwb    %vr05,%vr05,0
    vasrpwb    %vr06,%vr06,0
    VNAVRB4(09,01,02,03,04)
    VNAVRB4(10,05,06,03,04)
    vst64    %vr09,[%i0,0x0]
    vmr1w    %vr08,%vr08,%vr07
    vst64    %vr10,[%i0,0x10]
    vld64w    %vr01,[%i1,0x20]
    vld64w    %vr02,[%i1,0x28]
    vasrpwb    %vr07,%vr07,0
    vasrpwb    %vr08,%vr08,0
    vmr1w    %vr02,%vr02,%vr01
    vld64w    %vr03,[%i1,0x30]
    vld64w    %vr04,[%i1,0x38]
    vasrpwb    %vr01,%vr01,0
    vasrpwb    %vr02,%vr02,0
    VNAVRB4(09,05,06,07,08)
    VNAVRB4(10,07,08,01,02)
    vst64    %vr09,[%i0,0x20]
    vmr1w    %vr04,%vr04,%vr03
    vst64    %vr10,[%i0,0x30]
    vaddw    %vr00,%vr20,%vr00
}

static void put_no_rnd_pixels8_xy2_aurora( uint8_t *block,
                                           const uint8_t *pixels,
                                           int line_size, int h)
{
 #ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V21,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(8)

    VLD64W(V01,I1, 0X0);
    VADDW(V20,V20,V20);
    VLD64W(V02,I1,0x8);
    VLD64W(V03,I1,0x10);
    VLD64W(V04,I1,0x18);
    VADDW(V00,V20,V00);
    VADDW(V20,V20,V20);
    VMVW(V00,V21,0x1);
    VMR1W(V02,V02,V01);
    VMR1W(V04,V04,V03);
    VASRPWB(V01,V01,0);
    VASRPWB(V02,V02,0);
#else
    put_no_rnd_pixels8_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      put_no_rnd_pixels8_xy2_asm();
        // Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels8_xy2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels8_x2_ref( uint8_t *block, const uint8_t *pixels,
                                       int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = no_rnd_avg_pixels (*(pixels+x), *(pixels+x+1));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void put_no_rnd_pixels8_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1
    vbmulw    %vr23,%vr23,4
    vld64w    %vr01,[%i1,0x0]
    // Initial load is done in order to schedule properly
    vld64w    %vr02,[%i1,0x8]
}

_Asm void put_no_rnd_pixels8_x2_asm (void)
{
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vmr1w    %vr02,%vr02,%vr01
    vld64w    %vr05,[%i1,0x20]
    vld64w    %vr06,[%i1,0x28]
    vld64w    %vr07,[%i1,0x30]
    vld64w    %vr08,[%i1,0x38]
    vmr1w    %vr04,%vr04,%vr03
    vavb    %vr02,%vr01,%vr02
    vmr1w    %vr06,%vr06,%vr05
    vmr1w    %vr08,%vr08,%vr07
    vasrpwb    %vr02,%vr02,0
    vavb    %vr04,%vr03,%vr04
    vavb    %vr06,%vr05,%vr06
    vst64    %vr02,[%i0,0x0]
    vasrpwb    %vr04,%vr04,0
    vasrpwb    %vr06,%vr06,0
    vavb    %vr08,%vr07,%vr08
    vld64w    %vr01,[%i1,0x40]
    // One instruction was required here so
    // filled it up by next loop's iterations
    // instruction.
    vst64    %vr04,[%i0,0x10]
    vasrpwb    %vr08,%vr08,0
    vst64    %vr06,[%i0,0x20]
    vld64w    %vr02,[%i1,0x48]
    // One instruction was required here so filled
    // it up by next loop's iterations instruction
    vst64    %vr08,[%i0,0x30]
    vaddw    %vr00,%vr23,%vr00
    // Two stalls after this instruction and start of

}

static void put_no_rnd_pixels8_x2_aurora(uint8_t *block, const uint8_t *pixels,
                                         int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);      //Three Stalls

    VERIFY_AURORA_MPEG2_PRE_ASM(8)

    VBMULWi(V23,V23,4);
    VLD64W(V01, I1, 0x0); // Initial load is done in order to schedule properly
    VLD64W(V02, I1, 0x8);
#else
    put_no_rnd_pixels8_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      put_no_rnd_pixels8_x2_asm();
                            // next loop.
     }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels8_x2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void put_no_rnd_pixels8_y2_ref( uint8_t *block, const uint8_t *pixels,
                                       int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = no_rnd_avg_pixels (*(pixels+x), *(pixels+x+line_size));
        }
        block += line_size;
        pixels += line_size;

    }
}
#endif

_Asm void put_no_rnd_pixels8_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1
    vbmulw    %vr23,%vr23,4

    vld64w    %vr01,[%i1,0x0]
}

_Asm void put_no_rnd_pixels8_y2_asm (void)
{
    vld64w    %vr02,[%i1,0x10]
    vld64w    %vr03,[%i1,0x20]
    vld64w    %vr04,[%i1,0x30]
    vavb    %vr01,%vr01,%vr02
    vavb    %vr02,%vr03,%vr02
    vasrpwb    %vr01,%vr01,0
    vasrpwb    %vr02,%vr02,0
    vavb    %vr03,%vr03,%vr04
    vst64    %vr01,[%i0,0x0]
    vld64w    %vr01,[%i1,0x40]
    vasrpwb    %vr03,%vr03,0
    vavb    %vr04,%vr04,%vr01
    vst64    %vr02,[%i0,0x10]
    vasrpwb    %vr04,%vr04,0
    vst64    %vr03,[%i0,0x20]
    vst64    %vr04,[%i0,0x30]
    vaddw    %vr00,%vr23,%vr00
}

static void put_no_rnd_pixels8_y2_aurora( uint8_t *block, const uint8_t *pixels,
                                   int line_size, int h )
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(8)

    VBMULWi(V23,V23,4);

    VLD64W (V01, I1, 0x0);
#else
    put_no_rnd_pixels8_y2_prolog(block,pixels,line_size);
#endif
    int y=( h >> 2);
    int i;
    for (i = 0; i < y; i++)
    {
      put_no_rnd_pixels8_y2_asm();

    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(put_no_rnd_pixels8_y2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels8_y2_ref( uint8_t *block, const uint8_t *pixels,
                                       int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     no_rnd_avg_pixels(*(pixels+x),
                                                       *(pixels+x+line_size)));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels8_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovzw   %vr22,block,0x1
    vld64    %vr01,[%i1,0x0]
    vaddw    %vr23,%vr23,%vr23
    vld64    %vr02,[%i1,0x10]
    vaddw    %vr00,%vr23,%vr00
    vmvw    %vr00,%vr22,0x1
    vaddw    %vr23,%vr23,%vr23
}

_Asm void avg_no_rnd_pixels8_y2_asm (void)
{
    vavb    %vr01,%vr01,%vr02
    vld64    %vr03,[%i1,0x0]
    vld64    %vr05,[%i0,0x0]
    vld64    %vr06,[%i0,0x10]
    vavb    %vr02,%vr02,%vr03
    vavrb    %vr01,%vr01,%vr05
    vld64    %vr04,[%i1,0x10]
    vavrb    %vr02,%vr02,%vr06

    vst64    %vr01,[%i0,0x0]
    vst64    %vr02,[%i0,0x10]

    vavb    %vr03,%vr03,%vr04
    vld64    %vr01,[%i1,0x20]
    vld64    %vr05,[%i0,0x20]
    vld64    %vr06,[%i0,0x30]
    vavb    %vr04,%vr04,%vr01
    vavrb    %vr03,%vr03,%vr05
    vld64    %vr02,[%i1,0x30]
    vavrb    %vr04,%vr04,%vr06

    vst64    %vr03,[%i0,0x20]
    vst64    %vr04,[%i0,0x30]
    vaddw    %vr00,%vr00,%vr23
}

static void avg_no_rnd_pixels8_y2_aurora( uint8_t *block,
                                          const uint8_t *pixels,
                                          int line_size, int h )
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(8)

    VLD64(V01, I1, 0x0);
    VADDW(V23,V23,V23);
    VLD64(V02, I1, 0x10);
    VADDW(V00,V23,V00);
    VMVW(V00,V22,0x1); // Two stalls.
    VADDW(V23,V23,V23);
#else
    avg_no_rnd_pixels8_y2_prolog(block,pixels,line_size);
#endif
    int i;
    for(i=0;i<(h>>2);i++)
    {
      avg_no_rnd_pixels8_y2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels8_y2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels16_y2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++) {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     no_rnd_avg_pixels(*(pixels+x),
                                                       *(pixels+x+line_size)));
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels16_y2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovzw   %vr22,block,0x1
    vld128    %vr01,[%i1,0x0]
    vbmulw    %vr23,%vr23,4
    vld128    %vr02,[%i1,0x20]
    vld128    %vr03,[%i1,0x40]
    vld128    %vr04,[%i1,0x60]
    vaddw    %vr00,%vr23,%vr00
    vaddw    %vr23,%vr23,%vr23
    vmvw    %vr00,%vr22,0x1
}

_Asm void avg_no_rnd_pixels16_y2_asm (void)
{
    vavb    %vr01,%vr01,%vr02
    vavb    %vr02,%vr02,%vr03
    vavb    %vr03,%vr03,%vr04
    vld128    %vr05,[%i1,0x0]
    vld128    %vr09,[%i0,0x0]
    vld128    %vr10,[%i0,0x20]
    vavb    %vr04,%vr04,%vr05
    vld128    %vr11,[%i0,0x40]
    vld128    %vr12,[%i0,0x60]
    vld128    %vr06,[%i1,0x20]
    vld128    %vr07,[%i1,0x40]
    vld128    %vr08,[%i1,0x60]
    vavrb    %vr01,%vr01,%vr09
    vavrb    %vr02,%vr02,%vr10
    vavrb    %vr03,%vr03,%vr11
    vavrb    %vr04,%vr04,%vr12
    vst128    %vr01,[%i0,0x0]
    vst128    %vr02,[%i0,0x20]
    vst128    %vr03,[%i0,0x40]
    vst128    %vr04,[%i0,0x60]

    vavb    %vr05,%vr05,%vr06
    vavb    %vr06,%vr06,%vr07
    vavb    %vr07,%vr07,%vr08
    vld128    %vr01,[%i1,0x80]
    vld128    %vr09,[%i0,0x80]
    vld128    %vr10,[%i0,0xA0]
    vavb    %vr08,%vr08,%vr01
    vld128    %vr11,[%i0,0xC0]
    vld128    %vr12,[%i0,0xE0]
    vavrb    %vr05,%vr05,%vr09
    vavrb    %vr06,%vr06,%vr10
    vavrb    %vr07,%vr07,%vr11
    vavrb    %vr08,%vr08,%vr12
    vld128    %vr02,[%i1,0xA0]
    vld128    %vr03,[%i1,0xC0]
    vld128    %vr04,[%i1,0xE0]
    vst128    %vr05,[%i0,0x80]
    vst128    %vr06,[%i0,0xA0]
    vst128    %vr07,[%i0,0xC0]
    vst128    %vr08,[%i0,0xE0]
    vaddw    %vr00,%vr23,%vr00
    // If confident that h=8 always then omit this instruction.
}

static void avg_no_rnd_pixels16_y2_aurora( uint8_t *block,
                                           const uint8_t *pixels,
                                           int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVZW(V22,R0,0x1);

    VERIFY_AURORA_MPEG2_PRE_ASM(16)

    VLD128(V01, I1, 0x0);
    VBMULWi(V23,V23,4);
    VLD128(V02, I1, 0x20);
    VLD128(V03, I1, 0x40);
    VLD128(V04, I1, 0x60);
    VADDW(V00,V23,V00);
    VADDW(V23,V23,V23);
    VMVW(V00,V22,0x1);
    //One Stall
#else
    avg_no_rnd_pixels16_y2_prolog(block,pixels,line_size);
#endif
    int y=( h>> 3);
    int i;
    for (i = 0; i < y; i++)
    {
      avg_no_rnd_pixels16_y2_asm();
     }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels16_y2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels16_x2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ ) {
        for (x = 0; x < 16; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     no_rnd_avg_pixels(*(pixels+x),
                                                       *(pixels+x+1)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels16_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1

    vbmulw    %vr23,%vr23,4
}

_Asm void avg_no_rnd_pixels16_x2_asm (void)
{
    vld128    %vr01,[%i1,0x0]
    vld128    %vr02,[%i1,0x10]
    vld128    %vr03,[%i1,0x20]
    vld128    %vr04,[%i1,0x30]
    vld128    %vr05,[%i1,0x40]
    vld128    %vr06,[%i1,0x50]
    vld128    %vr07,[%i1,0x60]
    vld128    %vr08,[%i1,0x70]
    vmrb    %vr02,%vr02,%vr01
    vmrb    %vr04,%vr04,%vr03
    vmrb    %vr06,%vr06,%vr05
    vmrb    %vr08,%vr08,%vr07
    vld128    %vr09,[%i0,0x0]
    vld128    %vr10,[%i0,0x20]
    vld128    %vr11,[%i0,0x40]
    vld128    %vr12,[%i0,0x60]
    vavb    %vr01,%vr01,%vr02
    vavb    %vr03,%vr03,%vr04
    vavb    %vr05,%vr05,%vr06
    vavb    %vr07,%vr07,%vr08
    vavrb    %vr01,%vr01,%vr09
    vavrb    %vr03,%vr03,%vr10
    vavrb    %vr05,%vr05,%vr11
    vavrb    %vr07,%vr07,%vr12
    vst128    %vr01,[%i0,0x0]
    vst128    %vr03,[%i0,0x20]
    vst128    %vr05,[%i0,0x40]
    vst128    %vr07,[%i0,0x60]
    vaddw    %vr00,%vr23,%vr00
}

static void avg_no_rnd_pixels16_x2_aurora( uint8_t *block,
                                           const uint8_t *pixels,
                                           int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    //Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VBMULWi(V23,V23,4);
#else
    avg_no_rnd_pixels16_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
       avg_no_rnd_pixels16_x2_asm();
        //Two Stalls
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels16_x2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels16_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                         int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <16; x++) {
            *(block+x) = avg_pixels ( *(block+x),
                                      no_rnd_avg_pixels4 (*(pixels+x),
                                                          *(pixels+x+1),
                                                         *(pixels+x+line_size),
                                                       *(pixels+x+line_size+1))
                                    );
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels16_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr20,line_size,0x3
    vmovzw   %vr21,block,0x1
    vld128    %vr01,[%i1,0x0]
    vaddw    %vr20,%vr20,%vr20
    vld128    %vr02,[%i1,0x10]
    vld128    %vr03,[%i1,0x20]
    vld128    %vr04,[%i1,0x30]
    vaddw    %vr00,%vr20,%vr00

    vaddw    %vr20,%vr20,%vr20
    vmvw    %vr00,%vr21,0x1

    vmrb    %vr02,%vr02,%vr01
    vmrb    %vr04,%vr04,%vr03
}

_Asm void avg_no_rnd_pixels16_xy2_asm (void)
{
    vld128    %vr05,[%i1,0x0]
    vld128    %vr06,[%i1,0x10]
    vld128    %vr07,[%i1,0x20]
    vld128    %vr09,[%i0,0x0]
    vmrb    %vr06,%vr06,%vr05
    vld128    %vr10,[%i0,0x20]
    VNAVRB4(11,01,02,03,04)
    VNAVRB4(12,03,04,05,06)
    vavrb    %vr11,%vr11,%vr09
    vld128    %vr08,[%i1,0x30]
    vavrb    %vr12,%vr12,%vr10
    vst128    %vr11,[%i0,0x0]
    vmrb    %vr08,%vr08,%vr07
    vst128    %vr12,[%i0,0x20]

    vld128    %vr01,[%i1,0x40]
    vld128    %vr02,[%i1,0x50]
    vld128    %vr03,[%i1,0x60]
    vmrb    %vr02,%vr02,%vr01
    vld128    %vr09,[%i0,0x40]
    vld128    %vr10,[%i0,0x60]
    VNAVRB4(11,05,06,07,08)
    VNAVRB4(12,07,08,01,02)
    vavrb    %vr11,%vr11,%vr09
    vld128    %vr04,[%i1,0x70]
    vavrb    %vr12,%vr12,%vr10
    vst128    %vr11,[%i0,0x40]
    vmrb    %vr04,%vr04,%vr03
    vst128    %vr12,[%i0,0x60]
    vaddw    %vr00,%vr20,%vr00
    // Two Stalls
}

static void avg_no_rnd_pixels16_xy2_aurora( uint8_t *block,
                                             const uint8_t *pixels,
                                             int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V21,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(16)
    VLD128(V01, I1, 0x0);
    VADDW(V20,V20,V20);
    VLD128(V02, I1, 0x10);
    VLD128(V03, I1, 0x20);
    VLD128(V04, I1, 0x30);
    VADDW(V00,V20,V00);
    //One Stall
    VADDW(V20,V20,V20);
    VMVW(V00,V21,0x1);
    //Two Stalls
    VMRB(V02,V02,V01);
    VMRB(V04,V04,V03);
#else
    avg_no_rnd_pixels16_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      avg_no_rnd_pixels16_xy2_asm();
    }

#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels16_xy2, 16)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels8_xy2_ref( uint8_t *block, const uint8_t *pixels,
                                        int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ )    {
        for (x = 0; x <8; x++) {
            *(block+x) = avg_pixels ( *(block+x),
                                      no_rnd_avg_pixels4 (*(pixels+x),
                                                          *(pixels+x+1),
                                                         *(pixels+x+line_size),
                                                       *(pixels+x+line_size+1))
                                    );
        }
        pixels += line_size;
        block+=line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels8_xy2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr20,line_size,0x3
    vmovzw   %vr21,block,0x1
    vld64w    %vr01,[%i1,0x0]
    vaddw    %vr20,%vr20,%vr20
    vld64w    %vr02,[%i1,0x8]
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vaddw    %vr00,%vr20,%vr00
    vaddw    %vr20,%vr20,%vr20
    vmvw    %vr00,%vr21,0x1
    vmr1w    %vr02,%vr02,%vr01
    vmr1w    %vr04,%vr04,%vr03
}

_Asm void avg_no_rnd_pixels8_xy2_asm (void)
{
    vld64w    %vr05,[%i1,0x0]
    vld64w    %vr06,[%i1,0x8]
    vld64    %vr09,[%i0,0x0]
    vld64    %vr10,[%i0,0x10]
    vmr1w    %vr06,%vr06,%vr05
    VNAVRB4(11,01,02,03,04)
    VNAVRB4(12,03,04,05,06)
    vasrpwb    %vr11,%vr11,0
    vld64w    %vr07,[%i1,0x10]
    vasrpwb    %vr12,%vr12,0
    vavrb    %vr11,%vr11,%vr09
    vld64w    %vr08,[%i1,0x18]
    vavrb    %vr12,%vr12,%vr10
    vst64    %vr11,[%i0,0x0]
    vmr1w    %vr08,%vr08,%vr07
    vst64    %vr12,[%i0,0x10]

    vld64w    %vr01,[%i1,0x20]
    vld64w    %vr02,[%i1,0x28]
    vld64    %vr09,[%i0,0x20]
    vld64    %vr10,[%i0,0x30]
    vmr1w    %vr02,%vr02,%vr01
    VNAVRB4(11,05,06,07,08)
    VNAVRB4(12,07,08,01,02)
    vasrpwb    %vr11,%vr11,0
    vld64w    %vr03,[%i1,0x30]
    vasrpwb    %vr12,%vr12,0
    vavrb    %vr11,%vr11,%vr09
    vld64w    %vr04,[%i1,0x38]
    vavrb    %vr12,%vr12,%vr10
    vst64    %vr11,[%i0,0x20]
    vmr1w    %vr04,%vr04,%vr03
    vst64    %vr12,[%i0,0x30]
    vaddw    %vr00,%vr20,%vr00
        //Two Stalls
}

static void avg_no_rnd_pixels8_xy2_aurora( uint8_t *block,
                                            const uint8_t *pixels,
                                            int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V20,R2,0x3);
    VMOVZW(V21,R0,0x1);
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VLD64W(V01, I1, 0x0);
    VADDW(V20,V20,V20);
    VLD64W(V02, I1, 0x8);
    VLD64W(V03, I1, 0x10);
    VLD64W(V04, I1, 0x18);
    VADDW(V00,V20,V00);
    VADDW(V20,V20,V20);
    VMVW(V00,V21,0x1); // Two Stalls
    VMR1W(V02,V02,V01);
    VMR1W(V04,V04,V03);
#else
    avg_no_rnd_pixels8_xy2_prolog(block,pixels,line_size);
#endif
    int i;
    for( i =0 ; i<(h>>2) ; i++)
    {
      avg_no_rnd_pixels8_xy2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels8_xy2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

#ifdef VERIFY_AURORA
static void avg_no_rnd_pixels8_x2_ref( uint8_t *block, const uint8_t *pixels,
                                int line_size, int h)
{
    int x, y;

    for (y = 0; y < h; y++ ) {
        for (x = 0; x < 8; x++) {
            *(block+x) = avg_pixels( *(block+x),
                                     no_rnd_avg_pixels(*(pixels+x),
                                                       *(pixels+x+1)) );
        }
        block += line_size;
        pixels += line_size;
    }
}
#endif

_Asm void avg_no_rnd_pixels8_x2_prolog (uint8_t *block, const uint8_t *pixels, int line_size)
{
  % reg block, pixels, line_size
    vmovzw   %vr00,pixels,0x2
    vmovzw   %vr23,line_size,0x3
    vmovw    %vr00,block,0x1
    //three stalls
    vbmulw    %vr23,%vr23,4
}

_Asm void avg_no_rnd_pixels8_x2_asm (void)
{
    vld64w    %vr01,[%i1,0x0]
    vld64w    %vr02,[%i1,0x8]
    vld64w    %vr03,[%i1,0x10]
    vld64w    %vr04,[%i1,0x18]
    vld64w    %vr05,[%i1,0x20]
    vld64w    %vr06,[%i1,0x28]
    vld64w    %vr07,[%i1,0x30]
    vld64w    %vr08,[%i1,0x38]
    vmr1w    %vr02,%vr02,%vr01
    vmr1w    %vr04,%vr04,%vr03
    vmr1w    %vr06,%vr06,%vr05
    vmr1w    %vr08,%vr08,%vr07
    vld64    %vr09,[%i0,0x0]
    vld64    %vr10,[%i0,0x10]
    vld64    %vr11,[%i0,0x20]
    vld64    %vr12,[%i0,0x30]
    vavb    %vr01,%vr01,%vr02
    vavb    %vr03,%vr03,%vr04
    vavb    %vr05,%vr05,%vr06
    vavb    %vr07,%vr07,%vr08
    vasrpwb    %vr01,%vr01,0
    vasrpwb    %vr03,%vr03,0
    vasrpwb    %vr05,%vr05,0
    vasrpwb    %vr07,%vr07,0
    vavrb    %vr01,%vr01,%vr09
    vavrb    %vr03,%vr03,%vr10
    vavrb    %vr05,%vr05,%vr11
    vavrb    %vr07,%vr07,%vr12
    vst64    %vr01,[%i0,0x0]
    vst64    %vr03,[%i0,0x10]
    vst64    %vr05,[%i0,0x20]
    vst64    %vr07,[%i0,0x30]
    vaddw    %vr00,%vr23,%vr00
    //Two Stalls
}

static void avg_no_rnd_pixels8_x2_aurora( uint8_t *block,
                                          const uint8_t *pixels,
                                          int line_size, int h)
{
#ifdef VERIFY_AURORA
    VMOVZW(V00,R1,0x2);
    VMOVZW(V23,R2,0x3);
    VMOVW(V00,R0,0x1);
    //Three Stalls
    VERIFY_AURORA_MPEG2_PRE_ASM(8)
    VBMULWi(V23,V23,4);
#else
    avg_no_rnd_pixels8_x2_prolog(block,pixels,line_size);
#endif
    int i;
    for (i = 0; i < (h>>2); i++)
    {
      avg_no_rnd_pixels8_x2_asm();
    }
#ifdef VERIFY_AURORA
    VERIFY_AURORA_MPEG2_POST_ASM(avg_no_rnd_pixels8_x2, 8)
#endif
}

/*--------------------------------------------------------------------------*/



/*---De-interleaving functions for interlaced video-------------------------*/

/*         AB   ->  A
 *         CD       B
 *                  C
 *                  D
 */

//  TODO: makes this static and part of MpegEncContext struct.
/* Interleaves two 16 pixel columns of height 8. */

_Asm void deinterleave_luma_asm(uint8_t *block)
{
  % reg block

    vmovzw   %vr00,block,0x1

    vld128    %vr01,[%i0,0x00]
    vld128    %vr02,[%i0,0x20]
    vld128    %vr03,[%i0,0x40]
    vld128    %vr04,[%i0,0x60]
    vld128    %vr05,[%i0,0x80]
    vld128    %vr06,[%i0,0xA0]
    vld128    %vr07,[%i0,0xC0]
    vld128    %vr08,[%i0,0xE0]

    vld128    %vr11,[%i0,0x10]
    vld128    %vr12,[%i0,0x30]
    vld128    %vr13,[%i0,0x50]
    vld128    %vr14,[%i0,0x70]
    vld128    %vr15,[%i0,0x90]
    vld128    %vr16,[%i0,0xB0]
    vld128    %vr17,[%i0,0xD0]
    vld128    %vr18,[%i0,0xF0]

    vst128    %vr01,[%i0,0x00]
    vst128    %vr11,[%i0,0x20]
    vst128    %vr02,[%i0,0x40]
    vst128    %vr12,[%i0,0x60]
    vst128    %vr03,[%i0,0x80]
    vst128    %vr13,[%i0,0xA0]
    vst128    %vr04,[%i0,0xC0]
    vst128    %vr14,[%i0,0xE0]
    vst128    %vr05,[%i0,0x100]
    vst128    %vr15,[%i0,0x120]
    vst128    %vr06,[%i0,0x140]
    vst128    %vr16,[%i0,0x160]
    vst128    %vr07,[%i0,0x180]
    vst128    %vr17,[%i0,0x1A0]
    vst128    %vr08,[%i0,0x1C0]
    vst128    %vr18,[%i0,0x1E0]

}

void deinterleave_luma_aurora(uint8_t *block)
{
  deinterleave_luma_asm(block);
}

//  TODO: makes this static and part of MpegEncContext struct.
/* Interleaves two 8 pixel columns of height 4. */
_Asm void deinterleave_chroma_asm (uint8_t *block)
{
  % reg block
    vmovzw   %vr00,block,0x1

    vld64    %vr01,[%i0,0x00]
    vld64    %vr02,[%i0,0x20]
    vld64    %vr03,[%i0,0x40]
    vld64    %vr04,[%i0,0x60]

    vld64    %vr11,[%i0,0x08]
    vld64    %vr12,[%i0,0x28]
    vld64    %vr13,[%i0,0x48]
    vld64    %vr14,[%i0,0x68]

    vst64    %vr01,[%i0,0x00]
    vst64    %vr11,[%i0,0x20]
    vst64    %vr02,[%i0,0x40]
    vst64    %vr12,[%i0,0x60]
    vst64    %vr03,[%i0,0x80]
    vst64    %vr13,[%i0,0xA0]
    vst64    %vr04,[%i0,0xC0]
    vst64    %vr14,[%i0,0xE0]
}

void deinterleave_chroma_aurora(uint8_t *block)
{
  deinterleave_chroma_asm(block);
}

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/

/*-------------------------------------------------------*/
/*-------------------QPel Functions----------------------*/
/*-------------------------------------------------------*/

/*---QPel Ref Sub-functions.  do not require aurora implementations.--------*/

#ifdef __GNUC__

#define ST32(a, b) (((struct unaligned_32 *) (a))->l) = (b)
#define LD32(a) (((const struct unaligned_32 *) (a))->l)

#else /* __GNUC__ */

#define ST32(a, b) *((uint32_t*)(a)) = (b)
#define LD32(a) (*((uint32_t*)(a)))

#endif /* !__GNUC__ */

#ifdef VERIFY_AURORA
static inline void copy_block9( uint8_t *dst, uint8_t *src, int dstStride,
                                int srcStride, int h)
{
    int i;
    for(i=0; i<h; i++)
    {
        ST32(dst   , LD32(src   ));
        ST32(dst+4 , LD32(src+4 ));
        dst[8]= src[8];
        dst+=dstStride;
        src+=srcStride;
    }
}

static inline void copy_block17(uint8_t *dst, uint8_t *src,
                                int dstStride, int srcStride, int h)
{
    int i;
    for(i=0; i<h; i++)
    {
        ST32(dst   , LD32(src   ));
        ST32(dst+4 , LD32(src+4 ));
        ST32(dst+8 , LD32(src+8 ));
        ST32(dst+12, LD32(src+12));
        dst[16]= src[16];
        dst+=dstStride;
        src+=srcStride;
    }
}
#endif


/*--------------------------------------------------------------------------*/

void aurora_subpel_init(DSPContext* c)
{

    c->put_pixels_tab[1][0] = put_pixels8_aurora;
    c->put_pixels_tab[1][1] = put_pixels8_x2_aurora;
    c->put_pixels_tab[1][2] = put_pixels8_y2_aurora;
    c->put_pixels_tab[1][3] = put_pixels8_xy2_aurora;
    c->put_pixels_tab[0][0] = put_pixels16_aurora;
    c->put_pixels_tab[0][1] = put_pixels16_x2_aurora;
    c->put_pixels_tab[0][2] = put_pixels16_y2_aurora;
    c->put_pixels_tab[0][3] = put_pixels16_xy2_aurora;

    c->avg_pixels_tab[1][0] = avg_pixels8_aurora;
    c->avg_pixels_tab[1][1] = avg_pixels8_x2_aurora;
    c->avg_pixels_tab[1][2] = avg_pixels8_y2_aurora;
    c->avg_pixels_tab[1][3] = avg_pixels8_xy2_aurora;
    c->avg_pixels_tab[0][0] = avg_pixels16_aurora;
    c->avg_pixels_tab[0][1] = avg_pixels16_x2_aurora;
    c->avg_pixels_tab[0][2] = avg_pixels16_y2_aurora;
    c->avg_pixels_tab[0][3] = avg_pixels16_xy2_aurora;

    c->put_no_rnd_pixels_tab[1][0] = put_pixels8_aurora;
    c->put_no_rnd_pixels_tab[1][1] = put_no_rnd_pixels8_x2_aurora;
    c->put_no_rnd_pixels_tab[1][2] = put_no_rnd_pixels8_y2_aurora;
    c->put_no_rnd_pixels_tab[1][3] = put_no_rnd_pixels8_xy2_aurora;
    c->put_no_rnd_pixels_tab[0][0] = put_pixels16_aurora;
    c->put_no_rnd_pixels_tab[0][1] = put_no_rnd_pixels16_x2_aurora;
    c->put_no_rnd_pixels_tab[0][2] = put_no_rnd_pixels16_y2_aurora;
    c->put_no_rnd_pixels_tab[0][3] = put_no_rnd_pixels16_xy2_aurora;

    c->avg_no_rnd_pixels_tab[1][0] = avg_pixels8_aurora;
    c->avg_no_rnd_pixels_tab[1][1] = avg_no_rnd_pixels8_x2_aurora;
    c->avg_no_rnd_pixels_tab[1][2] = avg_no_rnd_pixels8_y2_aurora;
    c->avg_no_rnd_pixels_tab[1][3] = avg_no_rnd_pixels8_xy2_aurora;
    c->avg_no_rnd_pixels_tab[0][0] = avg_pixels16_aurora;
    c->avg_no_rnd_pixels_tab[0][1] = avg_no_rnd_pixels16_x2_aurora;
    c->avg_no_rnd_pixels_tab[0][2] = avg_no_rnd_pixels16_y2_aurora;
    c->avg_no_rnd_pixels_tab[0][3] = avg_no_rnd_pixels16_xy2_aurora;

}
