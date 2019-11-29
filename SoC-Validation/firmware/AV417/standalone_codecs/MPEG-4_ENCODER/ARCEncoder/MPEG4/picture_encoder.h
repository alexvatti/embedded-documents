/* ------------------------------------------------------------------------ */
/* FILE:			Picture_encoder.h										*/
/* DESCRIPTION:		Picture Class											*/
/* VERSION:			v2.10													*/
/* DATE:			16 August 2005											*/
/* MODIFIED BY:	    ARC International (UK) Limited							*/
/* MODIFIED DATE:	31 July 2007											*/
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

#ifndef _PICTURE_H_
#define _PICTURE_H_

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
	#include "typdef.h"
#endif

#include <stdio.h>

#include "BTPicture.h"
#include "defines.h"
/* ------------------------------------------------------------------------ */
/* CLASS CPicture															*/
/* ------------------------------------------------------------------------ */

class CPicture : public CBTPicture
{
public:				
					CPicture(void);
private:
	FILE *			m_FileHandle;
	long			m_PictureDataSize;
public:
	void			fill		(pel_t y, pel_t u, pel_t v);

	unsigned long int	checksum	(void);
#ifdef STATISTICS
	double			snr			(const CBTPicture &ref);
	void			yuv_snr		(const CBTPicture &ref, double& y_snr, double& u_snr, double& v_snr);
#endif // STATISTICS
#ifndef _WIN32_WCE
	int				read		(char* basename, framenum_t frame_number, long known_hsize=0, long known_vsize=0);
#else
	int				read		(unsigned short * basename, framenum_t frame_number, long known_hsize=0, long known_vsize=0);
#endif

	int				read_from_single_file (char* basename, framenum_t frame_number);

	int				write		(char* basename, framenum_t frame_number);

	int				write_to_single_file (char* basename);
	int				write_to_single_file2 (FILE*  f);

#ifndef _WIN32_WCE
	int				exists		(char *basename, framenum_t frame_number);
#else
	int				exists		( unsigned short *basename, framenum_t frame_number);
#endif

	int				determine_single_file_resolution(char* basename);
	int				single_file_sequence_length(char* basename);
	bool			open_single_file(char* basename);
	bool			read_next_from_single_file();
	bool			close_single_file();

};


#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
