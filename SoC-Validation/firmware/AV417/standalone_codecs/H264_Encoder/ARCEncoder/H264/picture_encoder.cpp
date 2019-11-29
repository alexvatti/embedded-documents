/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */

/* May contain code (c) British Telecommunications plc 2005        */

/* ------------------------------------------------------------------------ */
/* FILE:            Picture_encoder.cpp                                     */
/* DESCRIPTION:     Picture Class                                           */
/* VERSION:         v2.10                                                   */
/* DATE:            16 August 2005                                          */
/*                                                                          */
/*               (c) British Telecommunications plc                         */
/*                      All rights reserved 2005                            */
/*                          COPYRIGHT NOTICE                                */
/*                                                                          */
/*  British Telecommunications plc owns the sole copyright to this          */
/*  software. You may not (1) make a copy of this software except for the   */
/*  purposes of maintaining a single archive copy (2) derive works          */
/*  herefrom (3) distribute this work to others. This notice is provided    */
/*  for information and other rights may also apply.                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include    <stdlib.h>
#include    <string.h>
#include    <stdio.h>
#include    <math.h>
#include    <assert.h>
#include    "defines.h"
#include "VRaptorInfo.h"

#pragma warning(disable : 4996)     // disable deprecation warning

#ifdef _WIN32_WCE
    #include    <winbase.h>
    #include    <tchar.h>
    #include    <afx.h>
#endif

#ifndef _WIN32_WCE
    #if defined WIN32 || WIN16 || D_DOS
        #include    <io.h>
        #include    <sys/types.h>
        #include    <sys/stat.h>
    #endif
#endif

#if defined UNIX
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#endif
#ifdef __ARC_
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include    "picture_encoder.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION RE-DEFINITIONS FOR UNIX                                         */
/* ------------------------------------------------------------------------ */

#if defined UNIX

#define _fileno fileno
#define _fstat  fstat
#define _stat   stat

#endif

/* ------------------------------------------------------------------------ */
/* DEFINES                                                                  */
/* ------------------------------------------------------------------------ */

#define FILE_SIZE_Y_4CIF    (704*576)
#define FILE_SIZE_Y_CIF     (352*288)
#define FILE_SIZE_Y_QCIF    (176*144)
#define FILE_SIZE_Y_SQCIF   (128*96)

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture                                                    */
/* DESCRIPTION: constructor                                                 */
/* ------------------------------------------------------------------------ */

CPicture::CPicture(void)
{
    m_FileHandle = 0;
    m_PictureDataSize = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::fill                                              */
/* DESCRIPTION: fills picture with specified colour                         */
/* RETURNS:     none                                                        */
/* ------------------------------------------------------------------------ */

void   CPicture::fill (pel_t y, pel_t u, pel_t v)
{
    long ysize  = m_hsize*m_vsize;
    long uvsize = ysize/4;

    memset(m_y,y,ysize);
    memset(m_u,u,uvsize);
    memset(m_v,v,uvsize);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::checksum                                          */
/* DESCRIPTION: calculates picture checksum                                 */
/* RETURNS:     picture checksum                                            */
/* ------------------------------------------------------------------------ */

unsigned long int CPicture::checksum    (void)
{
    register unsigned long int sum = 0;
    register pel_t* y;
    long     size;

    size = m_hsize*m_vsize;
    y    = m_y;

    while (size-- > 0)
        {
        sum += *y++;
        sum = (((sum >> 31) & 1) | (sum << 1));
        }

    return(sum);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::snr                                               */
/* DESCRIPTION: calculates the y snr in db's between two pictures           */
/*              snr is 10*log((255/squared error)^2)                        */
/* RETURNS:     snr                                                         */
/* ------------------------------------------------------------------------ */
#ifdef STATISTICS
double CPicture::snr (const CBTPicture &ref)
{
    register pel_t  *ya;
    register pel_t  *yb;
    register int    error;
    register long   size;
    register long   sq_error;
    double          snr;

    size     = m_hsize * m_vsize;
    ya       = m_y;
    yb       = ref.m_y;
    sq_error = 0;

    // MeBusConv:
    int yPos = 0;
    int lumaStrideDiff = 0;
    if (HwMe == 1 && (m_hsize & 0x10))
    {
        lumaStrideDiff = 16;
    }

    while (size-- > 0)
        {
        error     = *ya++ - *yb++;
        sq_error += (error*error);
        // MeBusConv:
        if (++yPos == m_hsize)
        {
            yPos = 0;
            ya += lumaStrideDiff;
        }
        }

    snr = (double)sq_error/(double)(m_hsize*m_vsize);

    if (snr != 0)
        {
        snr = (double)(255*255)/snr;
        snr = 10 * (double)log10(snr);
        }
    else
        {snr = (double)99.99;}

    return(snr);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::yuv_snr                                           */
/* DESCRIPTION: calculates the y, u and v snr in db's between two pictures  */
/*              snr is 10*log((255/squared error)^2)                        */
/* RETURNS:     void                                                        */
/* ------------------------------------------------------------------------ */

void CPicture::yuv_snr(const CBTPicture &ref, double& y_snr, double& u_snr, double& v_snr)
{
    register pel_t  *a;
    register pel_t  *b;
    register int    error;
    register long   size;
    register long   sq_error;

    // calculate y_snr
    size     = m_hsize * m_vsize;
    a        = m_y;
    b        = ref.m_y;
    sq_error = 0;

    // MeBusConv:
    int yPos = 0;
    int lumaStrideDiff = 0;
    if (HwMe == 1 && (m_hsize & 0x10))
    {
        lumaStrideDiff = 16;
    }

    while (size-- > 0)
    {
        error     = *a++ - *b++;
        sq_error += (error*error);
        // MeBusConv:
        if (++yPos == m_hsize)
        {
            yPos = 0;
            a += lumaStrideDiff;
        }
    }

    y_snr = (double)sq_error/(double)(m_hsize*m_vsize);

    if (y_snr != 0)
    {
        y_snr = (double)(255*255)/y_snr;
        y_snr = 10 * (double)log10(y_snr);
    }
    else
    {
        y_snr = (double)99.99;
    }

    // calculate u_snr
    size     = (m_hsize * m_vsize) / 4;
    a        = m_u;
    b        = ref.m_u;
    sq_error = 0;

    while (size-- > 0)
    {
        error     = *a++ - *b++;
        sq_error += (error*error);
    }

    u_snr = (double)sq_error/(double)((m_hsize*m_vsize)/4);

    if (u_snr != 0)
    {
        u_snr = (double)(255*255)/u_snr;
        u_snr = 10 * (double)log10(u_snr);
    }
    else
    {
        u_snr = (double)99.99;
    }

    // calculate v_snr
    size     = (m_hsize * m_vsize) / 4;
    a        = m_v;
    b        = ref.m_v;
    sq_error = 0;

    while (size-- > 0)
    {
        error     = *a++ - *b++;
        sq_error += (error*error);
    }

    v_snr = (double)sq_error/(double)((m_hsize*m_vsize)/4);

    if (v_snr != 0)
    {
        v_snr = (double)(255*255)/v_snr;
        v_snr = 10 * (double)log10(v_snr);
    }
    else
    {
        v_snr = (double)99.99;
    }
}
#endif // STATISTICS

#ifndef ANALOG_DEVICES_VISUAL_DSP
// Most of the file access stuff is not supported with the Analog Devices compiler yet
// But the picture writing functions are supported

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::read_from_single_file                             */
/* DESCRIPTION: reads picture from a single yuv file. The size of the
                picture must be set before this function is called.         */
/* RETURNS:     none                                                        */
/* ------------------------------------------------------------------------ */

int CPicture::read_from_single_file (char* basename, framenum_t frame_number)
{
#ifndef _WIN32_WCE
#ifndef __arm
    char   filename[512];
    FILE*  f;
    int    file_no;
    long   tsize;
    int    bytes_rd;

#ifdef LARGE_FILES
    __int64 file_size;
    __int64 file_offset;
    struct _stati64 file_stat;
#else
    long file_offset;
#ifndef __ARC_

    struct _stat file_stat;
#endif // __ARC_
#endif

    /* construct filename */
    sprintf(filename,"%s.yuv",basename);

    /* try to open file */
    f = fopen(filename,"rb");
    if (f == NULL)
        {return(-1);}

    /* calculate picture dimensions */
    file_no  = _fileno(f);

    tsize    = (6 * m_hsize * m_vsize) / 4;
    if (tsize == 0) return (-1);
#ifndef __ARC_
#ifdef LARGE_FILES
    _fstati64(file_no,&file_stat);

    file_offset = (__int64)(tsize) * (__int64)(frame_number);

#else
    _fstat(file_no,&file_stat);

    file_offset = tsize * (long)(frame_number);
#endif

    file_size = file_stat.st_size;

    // Move the file pointer to the correct position
    if (file_offset + tsize > file_size)
    {
        // The end of the file has been reached
        return (-1);
    }

#ifdef LARGE_FILES
    if (-1 == _lseeki64(file_no, file_offset, SEEK_SET))
    {
        return (-3);
    }
#else
    if (fseek(f, file_offset, SEEK_SET) != 0)
    {
        return (-3);
    }
#endif

    bytes_rd = fread(m_data,1,tsize,f);
#else // __ARC_

    file_offset = tsize * (long)(frame_number);
    if (fseek(f, file_offset, SEEK_SET) != 0)
    {
            return (-3);
    }
    if((bytes_rd = fread(m_data, 1, tsize, f)) != tsize)
    {
        return(-1);
    }
#endif // not __ARC_
    fclose(f);

#endif
#endif

    return (0);
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::write                                             */
/* DESCRIPTION: write picture to file                                       */
/* RETURNS:     none                                                        */
/* ------------------------------------------------------------------------ */

int    CPicture::write      (char* basename, framenum_t frame_number)
{
#ifndef _WIN32_WCE
    char filename[512];
    FILE* f;

    /* construct y filename */
    sprintf(filename,"%s%u.y",basename,(unsigned int)frame_number);

    /* try to open file */
    f = fopen(filename,"wb");
    if (f == NULL)
        {return(-1);}

    /* write y data */
    fwrite(m_y,1,m_hsize*m_vsize,f);
    fclose(f);

    /* construct u filename */
    sprintf(filename,"%s%u.u",basename,(unsigned int)frame_number);

    /* try to open file */
    f = fopen(filename,"wb");
    if (f == NULL)
        {return(-1);}

    /* write u data */
    fwrite(m_u,1,(m_hsize*m_vsize)/4,f);
    fclose(f);

    /* construct v filename */
    sprintf(filename,"%s%u.v",basename,(unsigned int)frame_number);

    /* try to open file */
    f = fopen(filename,"wb");
    if (f == NULL)
        {return(-1);}

    /* write v data */
    fwrite(m_v,1,(m_hsize*m_vsize)/4,f);
    fclose(f);
#endif
    return(0);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::write_to_single_file                              */
/* DESCRIPTION: writes picture to a single yuv file. The size of the
                picture must be set before this function is called.
                Pictures must be written in order as appending to file is
                the only option.
                'Skipped' pictures must be added by repeatedly adding the
                previous picture in order to maintain temporal spacing in
                case of picture 'skipping'.                                 */
/* RETURNS:     none                                                        */
/* ------------------------------------------------------------------------ */

int CPicture::write_to_single_file (char* basename)
{
#ifndef _WIN32_WCE
    FILE*  f;
    unsigned int size;


    /* try to open file */
    f = fopen(basename,"ab");
    if (f == NULL)
    {
        ENC_PRINTF(("picture_encoder: Error - Cannot open output yuv file for append %s\n", basename));
        assert(false);
        return -1;
    }

    if (0 == m_hsize * m_vsize)
    {
        fclose(f);
        return (-1);
    }

#ifdef LARGE_FILES
    // Seek to end of file
    int    file_no;
    file_no  = _fileno(f);
    __int64 offset;
    offset = _lseeki64(file_no, 0, SEEK_END );
    if (-1 == offset)
    {
        fclose(f);
        return (-1);
    }
#endif

    // MeBusConv:
    unsigned char * writePtr;
    int lumaStride = m_hsize;
    if (HwMe == 1 && (m_hsize & 0x10))
    {
        lumaStride += 16;
    }

    /* write y data */
    // MeBusConv:
    writePtr = m_y;
    for (int j = 0; j < m_vsize; j++)
    {
        size = m_hsize;
        if (size != fwrite(writePtr, sizeof(unsigned char), size, f))
        {
            fclose(f);
            return -1;
        }
        writePtr += lumaStride;
    }

    /* write u data */
    size = (m_hsize*m_vsize) >> 2;
    if (size != fwrite(m_u, sizeof(unsigned char), size, f))
    {
        fclose(f);
        return -1;
    }

    /* write v data */
    if (size != fwrite(m_v, sizeof(unsigned char), size, f))
    {
        fclose(f);
        return -1;
    }

    fclose(f);
#endif
    return (0);
}

#ifndef ANALOG_DEVICES_VISUAL_DSP

/* -----------------------------

  Function:     exists
  Parameters:   char *basename, framenum_t frame_number
                used to construct the filename
  Returns:      int
  Description:  calls the _access function, to determine if the file exists. Returns
                -1 if it doesn't
  Author:       Matthew Walker
  Last Altered: 05-09-99
  By:           Matthew Walker

  ------------------------------ */

#ifndef _WIN32_WCE
int CPicture::exists(char *basename, framenum_t frame_number)
#else
int CPicture::exists(WCHAR *basename, framenum_t frame_number)
#endif
{
#ifndef _WIN32_WCE
#if !defined ( __arm) && !defined (__ARC_)
    char filename[512];

    /* construct y filename */
    sprintf(filename,"%s%u.y",basename,(unsigned int)frame_number);

    // check if the file exists
#ifdef D_UNIX
    return( access( filename, 0));
#else
    return( _access( filename, 0 ));
#endif
#else
#if defined SMART_PHONE || __arm || __ARC_
    char filename[512];

    /* construct y filename */
    sprintf(filename,"%s%u.y",basename,(unsigned int)frame_number);

    // check if the file exists
    //return( _access( filename, 0 ));
    return 1;
#else

    WCHAR filename[512];

    /* construct y filename */
    wsprintf(filename,TEXT("%s%u.y"),basename,(unsigned int)frame_number);

    // check if the file exists
    CFile f;
    if (f.Open(filename, CFile::modeRead | CFile::shareDenyNone) != FALSE)
    {
        f.Close();
        return 0;
    }
    else
    {
        return -1;
    }
#endif
#endif
#endif


}


/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::read_next_from_single_file                        */
/* DESCRIPTION: Reads the next picture from a single file.
                The file must have been opened with open_single_file and
                the size of the picture must be set before this
                function is called.                                         */
/* RETURNS:     true if successful, false if not.                           */
/* ------------------------------------------------------------------------ */
bool CPicture::read_next_from_single_file()
{
    long    bytes_rd;

    if (0 == m_FileHandle)
    {
        return false;
    }

    bytes_rd = fread(m_data,1,m_PictureDataSize,m_FileHandle);
    if (bytes_rd != m_PictureDataSize)
    {
        return false;
    }

    return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:    CPicture::close_single_file                     */
/* DESCRIPTION: Determines the length of the sequence for yuv single file
                sequences. The size of the picture must be set before this
                function is called.                                         */
/* RETURNS:     true if successful else false                               */
/* ------------------------------------------------------------------------ */
bool CPicture::close_single_file()
{
    if (0 != fclose(m_FileHandle))
    {
        return true;
    }
    else
    {
        return false;
    }
}


#endif  //def ANALOG_DEVICES


/* ------------------------------------------------------------------------ */
/* END OF FILE                                                              */
/* ------------------------------------------------------------------------ */
