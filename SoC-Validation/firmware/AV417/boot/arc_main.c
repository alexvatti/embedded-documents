/*********************************************************************
  (C) Copyright 2002-2007 ARC International (ARC);  Santa Cruz, CA 95060
  This program is the unpublished property and trade secret of ARC.   It
  is to be  utilized  solely  under  license  from  ARC  and it is to be
  maintained on a confidential basis for internal company use only.  The
  security  and  protection  of  the program is paramount to maintenance
  of the trade secret status.  It is to  be  protected  from  disclosure
  to unauthorized parties, both within the Licensee company and outside,
  in a manner  not less stringent than  that utilized for Licensee's own
  proprietary internal information.  No  copies of  the source or Object
  Code are to leave the premises of Licensee's business except in strict
  accordance with the license agreement signed by Licensee with ARC.
 *********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <arc/arc_timer.h>
#include "../headers/shikhara_uart.h"

extern main(int, char *[]);
extern _Far void _init(void);
extern void _fini(void);
extern char *__argv(int i);         /* from hostlink support */
extern int __argc(void);

// turn this off to improve call stack shown for runtime initialization
// since otherwise the call stack would terminate early due to calling
// exit() which is known to never return.  epilog_merge may also interfere
// with a clean call stack.
pragma off(propagate_never_returns);
pragma off(epilog_merge);

// Set by angeltim.s if interrupts are to be enabled to support profiling.
// Normally we would enable them in angeltim.s, but this fails if the
// library is compiled to preserve flags on calls, so to make it possible to
// compile the library this way, angeltim.s sets this boolean and we do
// the enabling here. Since we aren't returning at this level the enable
// bit settings won't be un-done when restoring flags from blink.  This is
// of course an unwelcome hack.
unsigned char __enable_prof_interrupt = 0;

/* We provide logic to zero our own BSS.  Normally the ELF loader would
   do this, but this requires the zeroed BSS section to be downloaded
   over the parallel port.  We can save time when running on the hardware
   by zeroing our own BSS section.  When on the hardware, the MetaWare
   debugger sets __INITBSS__ to 1, triggering us to do zeroing.  When
   running on the simulator, the debugger sets __INITBSS__ to 0, meaning
   that we don't have to do it: the BSS is zeroed for us.
 */
int __INITBSS__ = 1; 		/* note that it's in the .data section */

// from sbrk.c, will be 0 if running on Argonaut debugger.
extern unsigned __MEMSIZE__;

int __main_at = 0;	// line number where main called, for iprof_ct.c

extern char _fbss[], _ebss[];   /* defined by the linker */
extern char _fsbss[], _esbss[]; /* defined by the linker, or weak */

extern char __PICTABLE__[];
#pragma weak __PICTABLE__

void _timer_default_reset(void) {}
#pragma weak _timer_default_reset

// hook for RTOS initialization.  The RTOS needs to be sufficiently
// initialized that mutual exclusion primitives can be allocated and used.
extern void __start_kernel(void);

// This is the second part of the MetaWare runtime initialization.  This
// is called either from __arc_main() below or from somewhere in the RTOS.
void _mwrtl_init() {
	// atexit(_fini), to get global static destructors called
	atexit(_fini);
	// _init(), to register and call global static constructors
	// When linking for an RTOS, mutexes and/or semaphores will be created
	// here, too.
	_init();
}

void _mwcall11_main() {
	char **argv, *argp, *p;
	int argc, i;

	// Enable prof timer interrupt if instructed to do so.
	// We do it here since if mwrtl_init were compiled for
	// calls preserving flags the enable from there would be ineffectual.
	if (__enable_prof_interrupt)
		_enable();  // from stdlib.h

	// now construct argument vector.
	argc = __argc();
	argv = (char **) alloca( (argc+1) * sizeof(char *) );
	for (i = 0; i < argc; i++) {
		argp = __argv(i);
		p = alloca(strlen(argp) + 1);
		strcpy(p, argp);
		argv[i] = p;
	}
	argv[argc] = NULL;

	__main_at = __LINE__ + 1;
	exit(main(argc, argv));
}

#if _DSPC
/*
 * Linker generates these symbols to mark the bounds of X/Y memories
 * I assume I can make them int* because the linker options file should
 * also say ALIGN(4) BLOCK(4).
 */
extern int _fXdata[], _eXdata[], _fYdata[], _eYdata[];
#pragma weak _fXdata
#pragma weak _eXdata
#pragma weak _fYdata
#pragma weak _eYdata

#define XYCONFIG	0x98
#define BURSTSYS	0x99
#define BURSTXYM	0x9a
#define BURSTSZ		0x9b
#define BURST_LOAD	(1<<30)

// Transfers in 32 bit mode so halve xy address
#define dsp_blmem2xy(bank,xy,dest,src,size) _sr((unsigned long)src,BURSTSYS); \
	_sr(dest,BURSTXYM); \
_sr((size | (bank<<24) | (xy<<29) | BURST_LOAD),BURSTSZ)

#define dsp_bwait() while((_lr(XYCONFIG) & 0x10) != 0)

/*
 * This routine copies the contents of the .Xdata section to X memory
 * and the contents of the .Ydata section to Y memory.
 *
 * NOTE:  We have to be carefull not to touch any X/Y aux registers
 * unless we absolutely need them.  Otherwise this code will screw
 * up on an ARC that wasn't configured with X/Y memory.
 */
static void copy_xdata() {
	int elements = ((char*)_eXdata - (char*)_fXdata) - 1;
	dsp_blmem2xy(0,0,0,_fXdata, elements);
	dsp_bwait();
}
#pragma off_inline(copy_xdata);
static void copy_ydata() {
	int elements = ((char*)_eYdata - (char*)_fYdata) - 1;
	dsp_blmem2xy(0,1,0,_fYdata, elements);
	dsp_bwait();
}
#pragma off_inline(copy_ydata);
static void init_xy_memory_bank_0() {
	if (_fXdata != 0 && _fXdata != _eXdata) copy_xdata();
	if (_fYdata != 0 && _fYdata != _eYdata) copy_ydata();
}
#endif

//
// This is the first C entry point of the ARC runtime library initialization.
//
void __arc_main() {

	serial_init();   

	// Support for linker's INITDATA feature to copy ROM data to RAM.  This can
	// also be used to zero BSS. We'll ignore the unlikely error result (see
	// declaration and comments in stdlib.h).
	// If __PICTABLE__ is defined, then initcopy has already been run by the
	// assembly language entry code.
	if (__PICTABLE__ == 0){
		_initcopy();
	}

	// Zero the BSS before first access unless we've been instructed not to do so
	// by the debugger.
	if (__INITBSS__) {
		memset( _fbss, 0, _ebss - _fbss );
		if (_fsbss != NULL)
			memset( _fsbss, 0, _esbss - _fsbss );
	}

	;
	// after initcopy and memset, invalidate the dcache to make sure the
	// data goes to memory in case of subsequent cache-bypass accesses.
	_invalidate_dcache();

#if _DSPC
	init_xy_memory_bank_0();
#endif
	__enable_prof_interrupt = 0;
	_timer_default_reset();

	// call hook to initialize RTOS.  Depending on the RTOS, this call
	// may or may not return.  If it doesn't return, the RTOS must
	// call _mwrtl_init() from somewhere, before main() or some other
	// user-level entry point is called.
	__start_kernel();

	// call constructors and other explicit module initialization
	_mwrtl_init();

	// call out to main.  This routine never returns.
	_mwcall11_main();
}

