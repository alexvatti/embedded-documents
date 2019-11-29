
/*                                                         -*-C-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2006-2007 ARC International (Unpublished)             *
 * All Rights Reserved.                                            *
 *                                                                 *
 * This document, material and/or software contains confidential   *
 * and proprietary information of ARC International and is         *
 * protected by copyright, trade secret and other state, federal,  *
 * and international laws, and may be embodied in patents issued   *
 * or pending.  Its receipt or possession does not convey any      *
 * rights to use, reproduce, disclose its contents, or to          *
 * manufacture, or sell anything it may describe.  Reverse         *
 * engineering is prohibited, and reproduction, disclosure or use  *
 * without specific written authorization of ARC International is  *
 * strictly forbidden.  ARC and the ARC logotype are trademarks of *
 * ARC International.                                              *
 *                                                                 *
 *                                                                 *
 * arc_enc_utils.c                                                 *
 *                                                                 *
 * ARC Video Encoder API test harness support functions            *
 *                                                                 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arc_encoder.h>
#include <arc_encoder_malloc.h>

/*----------------------------------------------------------------------------------*/

void display_usage_info (char *progname)
{
  API_LOG_STDERR("ARC Video Encoder and API test harness\n");

#ifndef OUTPUT_DEBUG_INFO
  API_LOG_STDERR("Usage: %s -c <config file> [-i <input yuv>] [-s <output stats>] [-o <output h264>] [-w <width>] [-h <height>]\n",
		 progname);
#else // OUTPUT_DEBUG_INFO
  API_LOG_STDERR("Usage: %s -c <config file> [-i <input yuv>] [-s <output stats>] [-o <output h264>] [-r <output yuv>] [-w <width>] [-h <height>]\n",
		 progname);
#endif // OUTPUT_DEBUG_INFO

  exit (1);
}

/*----------------------------------------------------------------------------------*/

void parse_config_file (char *progname, 
			char *cfg_fn,
			void (*set_fn) (char *, char *, void *), 
			void *context)
{
  FILE *cfg_fp;
  unsigned char ch;
  char lval[128];
  char rval[128];
  int line = 1;
  int state = 0;
  int i;

  if (!set_fn)
  {
    API_LOG_STDERR( "%s: Error - no config setting fn\n", progname);
    exit (1);
  }

  if ((cfg_fp = fopen (cfg_fn, "rb")) == NULL)
  {
    API_LOG_STDERR("%s: Error - Cannot open input configuration file %s\n",
	     progname, cfg_fn);
    exit (1);
  }

  ch = fgetc (cfg_fp);

  /* main parsing loop */
  while (ch != (unsigned char) EOF)
  {
    if (ch == '#')
    {
      /* if comment then skip to the end of the line */
      while (ch != 0x0a && ch != (unsigned char) EOF)
	ch = fgetc (cfg_fp);
      if (ch == 0x0a)		/* line feed = last newline char for dos and last (and only) newline char for unix */
      {
	state = 0;
	ch = fgetc (cfg_fp);
	line++;
      }
    }
    else if (ch == ' ' || ch == '\t')
      while ((ch == ' ' || ch == '\t') && ch != (unsigned char) EOF)
      {
	/* if white space then skip */
	ch = fgetc (cfg_fp);
      }
    else if (ch == 0x0d)	/* carriage return = first newline char for dos */
    {
      /* if newline then check we've got all an lvalue, an '=' and an rvalue.    */
      /* then go to next line                                                */
      ch = fgetc (cfg_fp);
    }
    else if (ch == 0x0a)	/* line feed = last newline char for dos and last (and only) newline char for unix */
    {
      state = 0;
      ch = fgetc (cfg_fp);
      line++;
    }
    else if (state == 0)	/* state 0 = get lvalue */
    {
      i = 0;
      while (ch != ' ' && ch != '\t' && ch != '='
	     && ch != (unsigned char) EOF)
      {
	lval[i++] = tolower (ch);
	ch = fgetc (cfg_fp);
      }
      lval[i] = '\0';
      state++;
    }
    else if (state == 1)	/* state 1 = assignment operator */
    {
      if (ch == '=')
	state++;
      ch = fgetc (cfg_fp);
    }
    else if (state == 2)	/* state 2 = get rvalue */
    {
      i = 0;
      /* if rvalue is in quotes then fetch until closing quotes */
      if (ch == '"')
      {
	/* remove first quote */
	ch = fgetc (cfg_fp);

	while (ch != '"' && ch != (unsigned char) EOF)
	{
	  rval[i++] = /*tolower*/ (ch);
	  ch = fgetc (cfg_fp);
	}

	/* remove last quote */
	ch = fgetc (cfg_fp);
      }
      else
      {
	while (ch != ' ' && ch != '\t' && ch != 0x0a && ch != 0x0d
	       && ch != '#' && ch != (unsigned char) EOF)
	{
	  rval[i++] = /* tolower */ (ch);
	  ch = fgetc (cfg_fp);
	}
      }
      rval[i] = '\0';

      set_fn (lval, rval, context);
    }

  }				/* while (ch!=EOF) */
  fclose(cfg_fp);
}

/*----------------------------------------------------------------------------------*/

int get_frame_from_file (int fd, RawFrame * inframe, int size)
{
  int sofar = 0;
  int result;


  // Try and get all the data.
  while (sofar < size)
  {
    result = read (fd, (char *) (inframe->data.Y_base), size);
    if (result == 0)
    {
      API_LOG_STDERR( "EOF when reading from file\n");
      return sofar ? sofar : 0;
    }
    if (result == -1)
    {
#ifdef __ARC_LINUX__
      API_LOG_STDERR("Error reading from file: %s\n", strerror (errno));
#else
      API_LOG_STDERR("Error reading from file\n");
#endif
      return -1;
    }
    sofar += result;
    size -= result;
  }

  return sofar;
}

/*----------------------------------------------------------------------------------*/

#if !defined(__ARC_LINUX__) && defined(USE_ARC_TIMERS)

_Asm void DisableInterrupts( )
{
  FLAG 0
}

_Asm void EnableInterrupts( )
{
  FLAG 6
}

void Reset_Timer(void)
{
   DisableInterrupts();
  _sr(2,              0x22);       // CONTROL0 = 2 -- NH
  _sr(0xffffffff,     0x23);       // LIMIT0 = -1
  _sr(0,              0x21);       // COUNT0 = 0
}

#ifdef __USE_ARC_HOSTLIB__
void GetTimer(FILE* timerTickFile)
{
  if(timerTickFile)
  {
    unsigned int val = _lr(0x21);
    fAPI_LOG(timerTickFile,"%u\n",val);
    fflush(timerTickFile);
  }
}
#else
void GetTimer(unsigned int* ticksBuf, int* offset)
{
      unsigned int val_t = _lr(0x21);
      ticksBuf[*offset] = val_t;
      *offset++;
      _sr(0x1,0x47);
}
#endif

#endif

/*----------------------------------------------------------------------------------*/

