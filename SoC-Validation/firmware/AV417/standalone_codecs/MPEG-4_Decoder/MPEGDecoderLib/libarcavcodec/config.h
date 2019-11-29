
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * config.h
 *
 * December 2005.
 */

/* Protect against multiple inclusion */
#ifndef CONFIG_H
#define CONFIG_H 1

#define ARCH_ARC 1

#define __CPU__ arc

#define EMULATE_INTTYPES_FOR_LIBARCAVCODEC 1

#ifndef __MPEG_HCAC__
/* "restrict" keyword */
#define restrict __restrict
#endif

#define HAVE_FTELLO
#ifndef HAVE_FTELLO
/* Need these for FILE and off_t an config.h is usually before other includes*/
//#include <stdio.h>
#include <sys/types.h>
off_t ftello(FILE *);
#endif

/* Define this if your system has the "malloc.h" header file */
#define HAVE_MALLOC_H 1

/* memalign is mapped to malloc if unsupported */
#define HAVE_MEMALIGN 1
#ifndef HAVE_MEMALIGN
# define memalign(a,b) malloc(b)
#endif

/* Define this if your system has the "alloca.h" header file */
#define HAVE_ALLOCA_H 1

/* ffmpeg's libavcodec support (requires libavcodec source) */
#define USE_LIBAVCODEC 1

/* risky codecs */
#undef CONFIG_RISKY

/* Use libavcodec's decoders */
#define CONFIG_DECODERS 1
#define CONFIG_MPEGAUDIO_HP 1

#endif
