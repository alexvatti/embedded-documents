
/*
 * implement core Qpel functions
 */

#ifndef _AURORA_QPEL16_H_
#define _AURORA_QPEL16_H_


// src buffer stride = 32
// dst buffer stride = 32

_Asm void shift_pixels16x9_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_SHIFT_PIXELS16x9_SIMD_     ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_shift_pixels16x9_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process first 8 rows
    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;
    vld128   vr09,   [i1, 256]    ;

    vld64   vr11,   [i1,  16]    ;
    vld64   vr12,   [i1,  48]    ;
    vld64   vr13,   [i1,  80]    ;
    vld64   vr14,   [i1, 112]    ;
    vld64   vr15,   [i1, 144]    ;
    vld64   vr16,   [i1, 176]    ;
    vld64   vr17,   [i1, 208]    ;
    vld64   vr18,   [i1, 240]    ;
    vld64   vr19,   [i1, 272]    ;

    // shift input
    vmrb    vr01,   vr11,   vr01  ;
    vmrb    vr02,   vr12,   vr02  ;
    vmrb    vr03,   vr13,   vr03  ;
    vmrb    vr04,   vr14,   vr04  ;
    vmrb    vr05,   vr15,   vr05  ;
    vmrb    vr06,   vr16,   vr06  ;
    vmrb    vr07,   vr17,   vr07  ;
    vmrb    vr08,   vr18,   vr08  ;
    vmrb    vr09,   vr19,   vr09  ;

    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;
    vst128   vr09,   [i2, 256]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg

}


_Asm void shift_pixels16x17_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_SHIFT_PIXELS16x17_SIMD_     ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_shift_pixels16x17_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process first 8 rows
    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    vld64   vr11,   [i1,  16]    ;
    vld64   vr12,   [i1,  48]    ;
    vld64   vr13,   [i1,  80]    ;
    vld64   vr14,   [i1, 112]    ;
    vld64   vr15,   [i1, 144]    ;
    vld64   vr16,   [i1, 176]    ;
    vld64   vr17,   [i1, 208]    ;
    vld64   vr18,   [i1, 240]    ;

    // shift input
    vmrb    vr01,   vr11,   vr01  ;
    vmrb    vr02,   vr12,   vr02  ;
    vmrb    vr03,   vr13,   vr03  ;
    vmrb    vr04,   vr14,   vr04  ;
    vmrb    vr05,   vr15,   vr05  ;
    vmrb    vr06,   vr16,   vr06  ;
    vmrb    vr07,   vr17,   vr07  ;
    vmrb    vr08,   vr18,   vr08  ;

    // store result
    vst128   vr01,   [i2,   0]     ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;



    // process next 9 rows
    // load input
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;
    vld128   vr09,   [i1, 512]    ;

    vld64   vr11,   [i1, 272]    ;
    vld64   vr12,   [i1, 304]    ;
    vld64   vr13,   [i1, 336]    ;
    vld64   vr14,   [i1, 368]    ;
    vld64   vr15,   [i1, 400]    ;
    vld64   vr16,   [i1, 432]    ;
    vld64   vr17,   [i1, 464]    ;
    vld64   vr18,   [i1, 496]    ;
    vld64   vr19,   [i1, 528]    ;

    // shift input
    vmrb    vr01,   vr11,   vr01  ;
    vmrb    vr02,   vr12,   vr02  ;
    vmrb    vr03,   vr13,   vr03  ;
    vmrb    vr04,   vr14,   vr04  ;
    vmrb    vr05,   vr15,   vr05  ;
    vmrb    vr06,   vr16,   vr06  ;
    vmrb    vr07,   vr17,   vr07  ;
    vmrb    vr08,   vr18,   vr08  ;
    vmrb    vr09,   vr19,   vr09  ;

    // store result
    vst128   vr01,   [i2, 256]     ;
    vst128   vr02,   [i2, 288]    ;
    vst128   vr03,   [i2, 320]    ;
    vst128   vr04,   [i2, 352]    ;
    vst128   vr05,   [i2, 384]    ;
    vst128   vr06,   [i2, 416]    ;
    vst128   vr07,   [i2, 448]    ;
    vst128   vr08,   [i2, 480]    ;
    vst128   vr09,   [i2, 512]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg

}

_Asm void put_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x16_SIMD_     ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_put_pixels16x16_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // store at dst
    vst128   vr01,   [i2,   0]     ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

    // load input
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;

    // store result
    vst128   vr01,   [i2, 256]     ;
    vst128   vr02,   [i2, 288]    ;
    vst128   vr03,   [i2, 320]    ;
    vst128   vr04,   [i2, 352]    ;
    vst128   vr05,   [i2, 384]    ;
    vst128   vr06,   [i2, 416]    ;
    vst128   vr07,   [i2, 448]    ;
    vst128   vr08,   [i2, 480]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg

}

_Asm void put_pixels16x16_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x16_L2_SIMD_    ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_put_pixels16x16_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3,   0]     ;
    vst128   vr02,   [i3,  32]    ;
    vst128   vr03,   [i3,  64]    ;
    vst128   vr04,   [i3,  96]    ;
    vst128   vr05,   [i3, 128]    ;
    vst128   vr06,   [i3, 160]    ;
    vst128   vr07,   [i3, 192]    ;
    vst128   vr08,   [i3, 224]    ;


    // process the next 8 rows

    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;


    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    vst128   vr01,   [i3, 256]     ;
    vst128   vr02,   [i3, 288]    ;
    vst128   vr03,   [i3, 320]    ;
    vst128   vr04,   [i3, 352]    ;
    vst128   vr05,   [i3, 384]    ;
    vst128   vr06,   [i3, 416]    ;
    vst128   vr07,   [i3, 448]    ;
    vst128   vr08,   [i3, 480]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels16x17_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x17_L2_SIMD_    ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_put_pixels16x17_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3,   0]     ;
    vst128   vr02,   [i3,  32]    ;
    vst128   vr03,   [i3,  64]    ;
    vst128   vr04,   [i3,  96]    ;
    vst128   vr05,   [i3, 128]    ;
    vst128   vr06,   [i3, 160]    ;
    vst128   vr07,   [i3, 192]    ;
    vst128   vr08,   [i3, 224]    ;



    // process the next 9 rows


    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;
    vld128   vr09,   [i1, 512]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;
    vld128   vr19,   [i2, 512]    ;


    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;
    vavrb   vr09,   vr09,   vr19    ;

    vst128   vr01,   [i3, 256]     ;
    vst128   vr02,   [i3, 288]    ;
    vst128   vr03,   [i3, 320]    ;
    vst128   vr04,   [i3, 352]    ;
    vst128   vr05,   [i3, 384]    ;
    vst128   vr06,   [i3, 416]    ;
    vst128   vr07,   [i3, 448]    ;
    vst128   vr08,   [i3, 480]    ;
    vst128   vr09,   [i3, 512]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg

}


_Asm void put_no_rnd_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS16x16_L2_SIMD_ ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels16x16_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3,   0]     ;
    vst128   vr02,   [i3,  32]    ;
    vst128   vr03,   [i3,  64]    ;
    vst128   vr04,   [i3,  96]    ;
    vst128   vr05,   [i3, 128]    ;
    vst128   vr06,   [i3, 160]    ;
    vst128   vr07,   [i3, 192]    ;
    vst128   vr08,   [i3, 224]    ;



    // process the next 8 rows


    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;


    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;

    vst128   vr01,   [i3, 256]     ;
    vst128   vr02,   [i3, 288]    ;
    vst128   vr03,   [i3, 320]    ;
    vst128   vr04,   [i3, 352]    ;
    vst128   vr05,   [i3, 384]    ;
    vst128   vr06,   [i3, 416]    ;
    vst128   vr07,   [i3, 448]    ;
    vst128   vr08,   [i3, 480]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}



_Asm void put_no_rnd_pixels16x17_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS16x17_L2_SIMD_ ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels16x17_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3,   0]     ;
    vst128   vr02,   [i3,  32]    ;
    vst128   vr03,   [i3,  64]    ;
    vst128   vr04,   [i3,  96]    ;
    vst128   vr05,   [i3, 128]    ;
    vst128   vr06,   [i3, 160]    ;
    vst128   vr07,   [i3, 192]    ;
    vst128   vr08,   [i3, 224]    ;



    // process the next 9 rows


    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;
    vld128   vr09,   [i1, 512]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;
    vld128   vr19,   [i2, 512]    ;


    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;
    vavb    vr09,   vr09,   vr19    ;

    vst128   vr01,   [i3, 256]     ;
    vst128   vr02,   [i3, 288]    ;
    vst128   vr03,   [i3, 320]    ;
    vst128   vr04,   [i3, 352]    ;
    vst128   vr05,   [i3, 384]    ;
    vst128   vr06,   [i3, 416]    ;
    vst128   vr07,   [i3, 448]    ;
    vst128   vr08,   [i3, 480]    ;
    vst128   vr09,   [i3, 512]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS16x16_SIMD_ ;
    vrun    r10                               ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x16_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]     ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;


    // process the next 8 rows

    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;


    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    vst128   vr01,   [i2, 256]     ;
    vst128   vr02,   [i2, 288]    ;
    vst128   vr03,   [i2, 320]    ;
    vst128   vr04,   [i2, 352]    ;
    vst128   vr05,   [i2, 384]    ;
    vst128   vr06,   [i2, 416]    ;
    vst128   vr07,   [i2, 448]    ;
    vst128   vr08,   [i2, 480]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS_16x16_L2_SIMD_   ;
    vrun    r10                                     ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x16_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load input - src2
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // load dst
    vld128   vr11,   [i3,   0]    ;
    vld128   vr12,   [i3,  32]    ;
    vld128   vr13,   [i3,  64]    ;
    vld128   vr14,   [i3,  96]    ;
    vld128   vr15,   [i3, 128]    ;
    vld128   vr16,   [i3, 160]    ;
    vld128   vr17,   [i3, 192]    ;
    vld128   vr18,   [i3, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3,   0]     ;
    vst128   vr02,   [i3,  32]    ;
    vst128   vr03,   [i3,  64]    ;
    vst128   vr04,   [i3,  96]    ;
    vst128   vr05,   [i3, 128]    ;
    vst128   vr06,   [i3, 160]    ;
    vst128   vr07,   [i3, 192]    ;
    vst128   vr08,   [i3, 224]    ;


    // process the next 8 rows

    // load src1
    vld128   vr01,   [i1, 256]    ;
    vld128   vr02,   [i1, 288]    ;
    vld128   vr03,   [i1, 320]    ;
    vld128   vr04,   [i1, 352]    ;
    vld128   vr05,   [i1, 384]    ;
    vld128   vr06,   [i1, 416]    ;
    vld128   vr07,   [i1, 448]    ;
    vld128   vr08,   [i1, 480]    ;

    // load src2
    vld128   vr11,   [i2, 256]    ;
    vld128   vr12,   [i2, 288]    ;
    vld128   vr13,   [i2, 320]    ;
    vld128   vr14,   [i2, 352]    ;
    vld128   vr15,   [i2, 384]    ;
    vld128   vr16,   [i2, 416]    ;
    vld128   vr17,   [i2, 448]    ;
    vld128   vr18,   [i2, 480]    ;


    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // load dst
    vld128   vr11,   [i3, 256]    ;
    vld128   vr12,   [i3, 288]    ;
    vld128   vr13,   [i3, 320]    ;
    vld128   vr14,   [i3, 352]    ;
    vld128   vr15,   [i3, 384]    ;
    vld128   vr16,   [i3, 416]    ;
    vld128   vr17,   [i3, 448]    ;
    vld128   vr18,   [i3, 480]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i3, 256]     ;
    vst128   vr02,   [i3, 288]    ;
    vst128   vr03,   [i3, 320]    ;
    vst128   vr04,   [i3, 352]    ;
    vst128   vr05,   [i3, 384]    ;
    vst128   vr06,   [i3, 416]    ;
    vst128   vr07,   [i3, 448]    ;
    vst128   vr08,   [i3, 480]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

#endif
