/* ------------------------------------------------------------------------ */
/* FILE:			H264Warnings.h											*/
/* DESCRIPTION:		BT suppression of warning messages header file			*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
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

#ifndef	H264_WARNINGS_H
#define	H264_WARNINGS_H

#ifdef _MSC_VER  // Defined for Microsoft compiler

#pragma warning( disable : 4127  )  // Disable warning "conditional expression is constant"
#pragma warning( disable : 4291  )  // Disable warning "no matching operator delete found; memory will not be freed if initialization throws an exception"
#pragma warning( disable : 4514  )  // Disable warning "unreferenced inline function has been removed"
#pragma warning( disable : 4701  )  // Disable warning "local variable may be used without having been initialized"

#endif

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
