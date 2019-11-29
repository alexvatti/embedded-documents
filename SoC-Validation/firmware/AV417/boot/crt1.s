; Copyright (c) 2000-2009 ARC International
	.file	"crt1.s"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Startup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.extern __arc_main
	.extern	main; in case main is in user library (like eembc tests)
	.text
	.weak	_SDA_BASE_

	.macro jmpl,target
	    .if $core_version > 5
		jl	target		; "large" model call
	    .else
		lr	%blink, [%status]
		add	%blink, %blink, 3
		j	target
	    .endif
	.endm

	.global	_start
	.type	_start, @function
_start:
.if $arc_family != "ARC700"
	; for ram-based registers files! If you use flop-based, then
	; this code can be commented out. A700 family is always flop-based
	; and those come up in a known state.

	; Initialize the register file.  This is necessary because
	; of an idiosyncracy of the register file implementation.
	; Compiled code may generate sub  r0, r1, r1, expecting to
	; load 0 into r0, but the individual reads of r1 for the two source
	; operands may return different (garbage) values if r1 had never
	; been written to since reset.

    .ifdef _ARC_RF16
	.irep num, 1, 2, 3, 10, 11, 12, 13, 14, 15
		mov	r\&num, 0
	.endr
    .else
	.irep num, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
		mov	r\&num, 0
	.endr
    .endif

.endif

.ifndef _PICTABLE
	mov	r0, 0
	jmpl	__xcheck	; check compiled extensions against ARC build
.else
	; under PICTABLE, r0 is assumed to contain the base of the data segment
	; so must preserve it.  __xcheck modifies only r0 and r1 and is
	; position-independent
    .if $on(delay_slots)
	bl.d	__xcheck
	mov	r2, r0
    .else
	mov	r2, r0
	bl	__xcheck
    .endif
	mov	r0, r2

	.weak	__PICTABLE__
	; In PIC mode, we assume that we were loaded by a loader and may not
	; reside at the addresses that the ELF file was linked with.  In
	; this case, the address of where the data segment has been loaded
	; is passed to _start in the r0 register.  We figure out the new code
	; segment base ourselves.
	mov	r2, __PICTABLE__
	sub.f	0, r2, 0
	nop		; required for -a4 -core5
	beq	no_pic
	bl	_init_pic

no_pic:
.endif

	mov	sp, _estack	; initialize stack pointer
	mov	gp, _SDA_BASE_	; initialize small-data base register
    .if $isa == "ARCompact"
	.cfa_bf	_start

	.if $is_arcv2
	    .define INIT_JLI, _init_jli
	.else
	    .define INIT_JLI, _init_spe
	.endif
	.weak	INIT_JLI
	; must do this after %sp %gp are setup
	mov.f	r2,INIT_JLI
	jlnz	[%r2]

	.if $is_arcv2
	.weak	_init_ldi
	mov.f	r2,_init_ldi
	jlnz	[%r2]

	.weak	_init_ei
	mov.f	r2,_init_ei
	jlnz	[%r2]
	.endif


    .else
	sub	sp, sp, 16	; allocate expected call frame
    .endif
	mov	fp, 0		; initialize frame pointer
	jmpl	__arc_main
	b	_exit_halt
	.4byte	main; force main in if its in a library
	.if $isa == "ARCompact"
	.cfa_ef
	.endif


	.global	_exit_halt
	.type	_exit_halt, @function
_exit_halt:
	.if $isa == "ARCompact"
	.cfa_bf	_exit_halt
	.endif

	flag	0x01
	; ARC 700 serializes on 'flag', so no way to get back to here.
	nop
	nop
	nop
	b	_exit_halt

	.if $isa == "ARCompact"
	.cfa_ef
	.endif

	; weak versions of small data symbols normally defined by the linker.
	.weak	_fsbss
	.weak	_esbss
	.set	_fsbss,0
	.set	_esbss,0

	; weak versions of heap section boundaries.  If a .heap section
	; is provided, our low-level allocator "sbrk" allocates within it.
	; If no .heap is provided, we allocate from _end to the end of memory.
	.weak	_fheap
	.weak	_eheap
	.set	_fheap,0
	.set	_eheap,0

	; reference the beginning of the stack for debugger's stack checking
	.weak	_fstack
	.set	_fstack,0

	.end

