/* ------------------------------------------------------------------------ */
/* FILE:			Picture_encoder.cpp										*/
/* DESCRIPTION:		Picture Class											*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec4")
#endif

#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<math.h>

#ifdef _WIN32_WCE
	#include	<winbase.h>
	#include	<tchar.h>
	#include	<afx.h>
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

#include	"picture_encoder.h"

#include "memctl.h"

#ifdef AURORA_DMA
#include "ARCMedia.h"
#include "AuroraH264Dec.h"
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION RE-DEFINITIONS FOR UNIX 										*/
/* ------------------------------------------------------------------------ */

#if defined UNIX
static struct stat fileStats = {0};
#define _access access
#define _fileno fileno
#define _fstat  fstat
#define _stat   stat

#if !defined __ARC__ || defined __ARC_LINUX__ || defined __GNUC__
#define _filelength(X)	((_fstat(X,&fileStats)==0)?fileStats.st_size:-1)
#endif 

#endif

/* ------------------------------------------------------------------------ */
/* DEFINES																	*/
/* ------------------------------------------------------------------------ */

#define FILE_SIZE_Y_4CIF	(704*576)
#define FILE_SIZE_Y_CIF		(352*288)
#define FILE_SIZE_Y_QCIF	(176*144)
#define FILE_SIZE_Y_SQCIF	(128*96)

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture													*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CPicture::CPicture(void)
{
	m_FileHandle = 0;
	m_PictureDataSize = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::fill												*/
/* DESCRIPTION:	fills picture with specified colour							*/
/* RETURNS:		none														*/
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
/* FUNCTION:	CPicture::checksum											*/
/* DESCRIPTION:	calculates picture checksum									*/
/* RETURNS:		picture checksum											*/
/* ------------------------------------------------------------------------ */

unsigned long int CPicture::checksum_y  (void)
{
        register unsigned long int sum = 0;
        volatile register pel_t* y;
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


unsigned long int CPicture::checksum_u  (void)
{
        register unsigned long int sum = 0;
        volatile register pel_t* y;
        long     size;

        size = m_hsize*m_vsize/4;
        y    = m_u;

        while (size-- > 0)
                {
                sum += *y++;
                sum = (((sum >> 31) & 1) | (sum << 1));
                }

        return(sum);
}


unsigned long int CPicture::checksum_v  (void)
{
        register unsigned long int sum = 0;
        volatile register pel_t* y;
        long     size;

        size = m_hsize*m_vsize/4;
        y    = m_v;

        while (size-- > 0)
                {
                sum += *y++;
                sum = (((sum >> 31) & 1) | (sum << 1));
                }

        return(sum);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::snr												*/
/* DESCRIPTION:	calculates the y snr in db's between two pictures			*/
/*				snr is 10*log((255/squared error)^2)						*/
/* RETURNS:		snr															*/
/* ------------------------------------------------------------------------ */

double CPicture::snr (const CBTPicture &ref)
{	
	register pel_t  *ya;
	register pel_t  *yb;
	register int	error;
	register long   size;
	register long	sq_error;
	double			snr;

	size	 = m_hsize * m_vsize;
	ya		 = m_y;
	yb		 = ref.m_y;
	sq_error = 0;

	while (size-- > 0)
		{
		error     = *ya++ - *yb++;
		sq_error += (error*error);
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
/* FUNCTION:	CPicture::yuv_snr											*/
/* DESCRIPTION:	calculates the y, u and v snr in db's between two pictures	*/
/*				snr is 10*log((255/squared error)^2)						*/
/* RETURNS:		void														*/
/* ------------------------------------------------------------------------ */

void CPicture::yuv_snr(const CBTPicture &ref, double& y_snr, double& u_snr, double& v_snr)
{	
	register pel_t  *a;
	register pel_t  *b;
	register int	error;
	register long   size;
	register long	sq_error;

	// calculate y_snr
	size	 = m_hsize * m_vsize;
	a		 = m_y;
	b		 = ref.m_y;
	sq_error = 0;

	while (size-- > 0)
	{
		error     = *a++ - *b++;
		sq_error += (error*error);
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
	size	 = (m_hsize * m_vsize) / 4;
	a		 = m_u;
	b		 = ref.m_u;
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
	size	 = (m_hsize * m_vsize) / 4;
	a		 = m_v;
	b		 = ref.m_v;
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

#ifndef ANALOG_DEVICES_VISUAL_DSP
// Most of the file access stuff is not supported with the Analog Devices compiler yet
// But the picture writing functions are supported

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::read												*/
/* DESCRIPTION:	reads picture from file										*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* Windows CE does not support eg _stat, _fstat() _filelength().  File      */
/* sizes have therefore been hard coded - ONLY QCIF is supported for CE     */
/* ------------------------------------------------------------------------ */
#ifndef _WIN32_WCE
int CPicture::read (char* basename, framenum_t frame_number, long known_hsize, long known_vsize)
#else
int CPicture::read (WCHAR * basename, framenum_t frame_number, long known_hsize, long known_vsize)
#endif
{
	
#ifndef _WIN32_WCE
	FILE*  f;
	char   filename[512];
#else

#ifdef SMART_PHONE
	FILE*  f;
	char   filename[512];
#else
	CFile f;
	WCHAR filename[512];
#endif
#endif
	
	long   ysize,tsize,size;
	long   hsize,vsize;	
	int	   bytes_rd;
	pel_t* pPel;

#ifndef _WIN32_WCE
#ifndef __arm
	int    file_no;
//	struct _stat file_stat;
#endif

	/* construct y filename */
	sprintf(filename,"%s%u.y",basename,(unsigned int)frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}
#else
#ifdef SMART_PHONE
//	struct _stat file_stat;


	/* construct y filename */
	sprintf(filename,"%s%u.y",basename,frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}
#else

	wsprintf(filename,TEXT("%s%u.y"),basename,frame_number);

	if (FALSE == f.Open(filename, CFile::modeRead | CFile::shareDenyNone ))
	{
		return -1;
	}
#endif

#endif

#ifndef _WIN32_WCE
#ifndef __arm

	/* calculate picture dimensions */	
	file_no  = _fileno(f);
	ysize	 = _filelength(file_no); //ARC uncommented

	//_fstat(file_no,&file_stat);	 //ARC commented
	//ysize    = file_stat.st_size;  //ARC commented
	
	tsize	 = (ysize*6)/4;

	if ((0 != known_hsize) && (0 != known_vsize))
	{
		// Use known size, but check it matches the file size
		if (ysize != known_hsize * known_vsize)
		{
			return (-2);
		}

		hsize = known_hsize;
		vsize = known_vsize;
	}
	else
	{
		if (ysize == FILE_SIZE_Y_4CIF)
			{hsize = 704; vsize = 576;}
		else if (ysize == FILE_SIZE_Y_CIF)
			{hsize = 352; vsize = 288;}
		else if (ysize == FILE_SIZE_Y_QCIF)
			{hsize = 176; vsize = 144;}
		else if (ysize == FILE_SIZE_Y_SQCIF)
			{hsize = 128; vsize = 96;}
		else
			{return(-2);}	
	}

#else
	if ((0 != known_hsize) && (0 != known_vsize))
	{
		hsize = known_hsize;
		vsize = known_vsize;
	}
	else
	{
		hsize = 176; 
		vsize = 144;
	}
	ysize = hsize*vsize;
	tsize = (ysize*6)/4;
	
#endif
#else
	if ((0 != known_hsize) && (0 != known_vsize))
	{
		hsize = known_hsize;
		vsize = known_vsize;
	}
	else
	{
		hsize = 176; 
		vsize = 144;
	}
	ysize = hsize*vsize;
	tsize = (ysize*6)/4;
	
#endif



	/* set picture size */
	set_size(hsize,vsize,0);
		
	/* read y data */
	size = m_hsize*m_vsize;	
	pPel = m_y;

#ifndef _WIN32_WCE
	do
		{
		bytes_rd = fread(pPel,1,m_hsize,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);

#else
#ifdef SMART_PHONE
	do
		{
		bytes_rd = fread(pPel,1,m_hsize,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);
#else

	do
	{
		bytes_rd = f.Read(pPel, m_hsize);
		size -= bytes_rd;
		pPel += bytes_rd;
	}
	while ((size > 0) && (bytes_rd > 0));

	f.Close();
#endif
#endif
	
#ifndef _WIN32_WCE
	/* construct u filename */
	sprintf(filename,"%s%u.u",basename,(unsigned int)frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}

	/* read u data */
	size = m_hsize*m_vsize/4;	
	pPel = m_u;								
	do
		{
		bytes_rd = fread(pPel,1,m_hsize/2,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);

#else
#ifdef SMART_PHONE
		/* construct u filename */
	sprintf(filename,"%s%u.u",basename,frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}

	/* read u data */
	size = m_hsize*m_vsize/4;	
	pPel = m_u;								
	do
		{
		bytes_rd = fread(pPel,1,m_hsize/2,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);
#else

	/* construct u filename */
	wsprintf(filename,TEXT("%s%u.u"),basename,frame_number);

	/* try to open file */
	if (FALSE == f.Open(filename, CFile::modeRead | CFile::shareDenyWrite ))
	{
		return(-1);
	}

	/* read u data */
	size = m_hsize*m_vsize/4;	
	pPel = m_u;								
	do
		{
		bytes_rd = f.Read(pPel,m_hsize/2);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	f.Close();

#endif
#endif

#ifndef _WIN32_WCE
	/* construct v filename */
	sprintf(filename,"%s%u.v",basename,(unsigned int)frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}

	/* read v data */
	size = m_hsize*m_vsize/4;	
	pPel = m_v;								
	do
		{
		bytes_rd = fread(pPel,1,m_hsize/2,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);

#else

#ifdef SMART_PHONE
		/* construct v filename */
	sprintf(filename,"%s%u.v",basename,frame_number);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}

	/* read v data */
	size = m_hsize*m_vsize/4;	
	pPel = m_v;								
	do
		{
		bytes_rd = fread(pPel,1,m_hsize/2,f);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	fclose(f);
#else

	/* construct v filename */
	wsprintf(filename,TEXT("%s%u.v"),basename,frame_number);
		/* try to open file */
	if (FALSE == f.Open(filename, CFile::modeRead | CFile::shareDenyWrite ))
	{
		return(-1);
	}

	/* read u data */
	size = m_hsize*m_vsize/4;	
	pPel = m_v;								
	do
		{
		bytes_rd = f.Read(pPel,m_hsize/2);
		size -= bytes_rd;
		pPel += bytes_rd;
		}
	while ((size > 0) && (bytes_rd > 0));

	f.Close();
#endif
#endif
		
	return (0);	
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::read_from_single_file								*/
/* DESCRIPTION:	reads picture from a single yuv file. The size of the 
				picture must be set before this function is called. 		*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

//#define LARGE_FILES

#include "fcntl.h"

int CPicture::read_from_single_file (char* basename, framenum_t frame_number)
{
#ifndef _WIN32_WCE
#ifndef __arm
	char   filename[512];
	FILE*  f;	
	int    file_no;
	long   tsize;
	int	   bytes_rd;

#ifdef LARGE_FILES
	__int64 file_size;
	__int64 file_offset;
	struct _stati64 file_stat;
#else
	long file_size;
	long file_offset;
//	struct _stat file_stat;
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

#ifdef LARGE_FILES
	_fstati64(file_no,&file_stat);

	file_offset = (__int64)(tsize) * (__int64)(frame_number);
	file_size = file_stat.st_size; //ARC added

#else
	//_fstat(file_no,&file_stat); //ARC commented
	file_size = _filelength(file_no); //ARC added
	file_offset = tsize * (long)(frame_number);
#endif

	//file_size = file_stat.st_size; //ARC commented

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

	fclose(f);

#endif		
#endif

	return (0);	
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::write												*/
/* DESCRIPTION:	write picture to file										*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

int	   CPicture::write		(char* basename, framenum_t frame_number)
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
/* FUNCTION:	CPicture::write_to_single_file								*/
/* DESCRIPTION:	writes picture to a single yuv file. The size of the 
				picture must be set before this function is called.
				Pictures must be written in order as appending to file is 
				the only option. 
				'Skipped' pictures must be added by repeatedly adding the 
				previous picture in order to maintain temporal spacing in 
				case of picture 'skipping'.									*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

int CPicture::write_to_single_file (char* basename, bool writeChecksum)
{
#if defined(AURORA_DMA)
    AuroraH264Dec::WaitForSIMDFree();
#endif
    
#ifndef _WIN32_WCE
    char   filename[512];
    FILE*  f;	
    unsigned int size;
    
    /* construct filename */
    if(writeChecksum)
    {
        sprintf(filename,"%s.sum",basename);
        f = fopen(filename,"a");
    }
    else
    {
        sprintf(filename,"%s.yuv",basename);
        f = fopen(filename,"ab");
    }
    
    /* try to open file */
    if (f == NULL)
    {
#if defined AURORA_DMA && !defined __ARC_LINUX__
        FlushDCache();
#endif
        {return(-1);}
    }	
    if (0 == m_hsize * m_vsize)
    {
        fclose(f);
#if defined AURORA_DMA && !defined __ARC_LINUX__
        FlushDCache();
#endif
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
#if defined AURORA_DMA && !defined __ARC_LINUX__
        FlushDCache();
#endif
        return (-1);
    }
#endif
    
    if(writeChecksum)
    {
        fprintf(f,"Luma\t%d\n",checksum_y());
        fprintf(f,"Chroma u\t%d\n",checksum_u());
        fprintf(f,"Chroma v\t%d\n",checksum_v());
    }
    else
    {
        /* write y data */
        size = m_hsize*m_vsize;
        if (size != fwrite(m_y, sizeof(unsigned char), size, f))
        {
            fclose(f);
#if defined AURORA_DMA && !defined __ARC_LINUX__
            FlushDCache();
#endif
            return -1;
        }
        
        /* write u data */
        size = (m_hsize*m_vsize) >> 2;
        if (size != fwrite(m_u, sizeof(unsigned char), size, f))
        {
            fclose(f);
#if defined AURORA_DMA && !defined __ARC_LINUX__
            FlushDCache();
#endif
            return -1;
        }
        
        /* write v data */
        if (size != fwrite(m_v, sizeof(unsigned char), size, f))
        {
            fclose(f);
#if defined AURORA_DMA && !defined __ARC_LINUX__
            FlushDCache();
#endif
            return -1;
        }
    }
    fclose(f);
#if defined AURORA_DMA && !defined __ARC_LINUX__
    FlushDCache();
#endif
#endif		
    return (0);	
}

#ifndef ANALOG_DEVICES_VISUAL_DSP

/* -----------------------------

  Function:		exists
  Parameters:	char *basename, framenum_t frame_number
				used to construct the filename
  Returns:		int
  Description:	calls the _access function, to determine if the file exists. Returns
				-1 if it doesn't
  Author:		Matthew Walker
  Last Altered:	05-09-99
  By:			Matthew Walker

  ------------------------------ */
  
#ifndef _WIN32_WCE
int CPicture::exists(char *basename, framenum_t frame_number)
#else
int CPicture::exists(WCHAR *basename, framenum_t frame_number)
#endif
{
#ifndef _WIN32_WCE
#ifndef __arm
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
#if defined SMART_PHONE || __arm
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
/* FUNCTION:	CPicture::determine_single_file_resolution					*/
/* DESCRIPTION:	tries to determine the resolution of pictures in a single 
				yuv file. The file is assumed to have no header and consist 
				of y followed by u followed by v, followed by the same for 
				the next picture and so on until the end of the file.
				This function assumes that the file is either QCIF (176x144)
				or CIF (352x288) resolution. If the file size is such that 
				a whole number of QCIF are contained but a fractional 
				number of CIF, then it is determined to be QCIF. Otherwise, 
				the two pictures are read as if they were QCIF, and 
				the sum of absolute differences is computed and compared to 
				a threshold. If less than the threshold, QCIF is assumed, 
				else CIF. 													*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

#define MACRO_ABS(a) (((a) > 0) ? (a) : (-(a)))

int CPicture::determine_single_file_resolution (char* basename)
{
#ifndef _WIN32_WCE
#ifndef __arm
	char   filename[512];
	FILE*  f;	
	int    file_no;
	long   file_size;
	long   size, file_offset;
	int	   bytes_rd, total_bytes;
	pel_t* pPel1;
	pel_t* pPel2;
//	struct _stat file_stat;

	/* construct filename */
	sprintf(filename,"%s.yuv",basename);

	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}
		
	/* calculate picture dimensions */	
	file_no  = _fileno(f);
	//ysize	 = _filelength(file_no);

	//_fstat(file_no,&file_stat); //ARC commented
	//file_size = file_stat.st_size; //ARC commented
	file_size = _filelength(file_no);

	if ((file_size % (3 * FILE_SIZE_Y_QCIF / 2)) != 0)
	{
		// Invalid size
		return (-1);
	}

	if ((file_size % (3 * FILE_SIZE_Y_CIF / 2)) != 0)
	{
		// Must be QCIF
		m_hsize = 176; 
		m_vsize = 144;
		return 0;
	}

	// Choose two frames from the middle of the sequence to compare
	// The first two are not used as one of the test sequences used to test this 
	// code started with a number of block frames

	file_offset = (file_size / (3 * FILE_SIZE_Y_QCIF)) * (3 * FILE_SIZE_Y_QCIF / 2);

	if (fseek(f, file_offset, SEEK_SET) != 0)
	{
		return (-3);
	}

	/* read two frames of QCIF y data */
	size = FILE_SIZE_Y_QCIF;

	pPel1 = new pel_t[size];
	pPel2 = new pel_t[size];
	if ((pPel1 == 0) || (pPel2 == 0))
	{
		if (pPel1 != 0) delete pPel1;
		if (pPel2 != 0) delete pPel2;
		return (-2);
	}

	total_bytes = 0;
	do
	{
		bytes_rd = fread(pPel1 + total_bytes,1,176,f);
		size -= bytes_rd;
		total_bytes += bytes_rd;
	}
	while ((size > 0) && (bytes_rd > 0));

	size = FILE_SIZE_Y_QCIF;

	if (fseek(f, file_offset + (3 * size / 2), SEEK_SET) != 0)
	{
		return (-3);
	}

	total_bytes = 0;
	do
	{
		bytes_rd = fread(pPel2 + total_bytes,1,176,f);
		size -= bytes_rd;
		total_bytes += bytes_rd;
	}
	while ((size > 0) && (bytes_rd > 0));

	unsigned int soad = 0;
	size = FILE_SIZE_Y_QCIF;
	total_bytes = 0;
	while (total_bytes < size)
	{
		soad += MACRO_ABS(*(pPel1 + total_bytes) - *(pPel2 + total_bytes));
		total_bytes++;
	}
	
	delete pPel1;
	delete pPel2;
	
	fclose(f);

	if (soad < 16 * FILE_SIZE_Y_QCIF)
	{
		// Guess QCIF
		m_hsize = 176; 
		m_vsize = 144;
	}
	else
	{
		// Guess CIF
		m_hsize = 352; 
		m_vsize = 288;
	}
	return (0);	
#else
	return (-1);
#endif		
#else
	return (-1);
#endif		
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::single_file_sequence_length						*/
/* DESCRIPTION:	Determines the length of the sequence for yuv single file 
				sequences. The size of the picture must be set before this 
				function is called.											*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

int CPicture::single_file_sequence_length (char* basename)
{
#ifndef _WIN32_WCE
#ifndef __arm
	char   filename[512];
	FILE*  f;	
	int    file_no;
	long   file_size;
//	struct _stat file_stat;
	long	tsize;

	/* construct filename */
	sprintf(filename,"%s.yuv",basename);

	tsize    = 3 * m_hsize * m_vsize / 2;
	if (tsize == 0) return (-1);
	
	/* try to open file */
	f = fopen(filename,"rb");
	if (f == NULL)
		{return(-1);}
		
	/* calculate picture dimensions */	
	file_no  = _fileno(f);
	//ysize	 = _filelength(file_no);

	//_fstat(file_no,&file_stat); //ARC commented
	//file_size = file_stat.st_size; //ARC commented
        file_size = _filelength(file_no); //ARC added
	fclose(f);

	int sequence_length = file_size / (tsize);

	return sequence_length;
#else
	return 0;
#endif
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::open_single_file									*/
/* DESCRIPTION:	Opens a single file for subsequent calls to 
				read_next_from_single_file()
				The size of the picture must be set before this 
				function is called.											*/
/* RETURNS:		true if successful, false if not.							*/
/* ------------------------------------------------------------------------ */
bool CPicture::open_single_file(char* basename)
{
	if (0 != m_FileHandle)
	{
		return false;
	}
	
	m_PictureDataSize = (6 * m_hsize * m_vsize) / 4;
	if (m_PictureDataSize == 0) 
	{
		return(false);
	}

	
	char   filename[512];

	/* construct filename */
	sprintf(filename,"%s.yuv",basename);

	/* try to open file */
	m_FileHandle = fopen(filename,"rb");
	if (m_FileHandle == NULL)
	{
		return(false);
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CPicture::read_next_from_single_file						*/
/* DESCRIPTION:	Reads the next picture from a single file.
				The file must have been opened with open_single_file and 
				the size of the picture must be set before this 
				function is called.											*/
/* RETURNS:		true if successful, false if not.							*/
/* ------------------------------------------------------------------------ */
bool CPicture::read_next_from_single_file()
{
	long	bytes_rd;

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
/* FUNCTION:	CPicture::close_single_file						*/
/* DESCRIPTION:	Determines the length of the sequence for yuv single file 
				sequences. The size of the picture must be set before this 
				function is called.											*/
/* RETURNS:		true if successful else false								*/
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


#endif	//def ANALOG_DEVICES


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif