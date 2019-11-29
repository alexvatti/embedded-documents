; Copyright (c) 2002-2005 ARC International
	.file	"crti.s"
	.option	%reg
	.global _init, _fini
	.extern __mw_cpp_init, __mw_cpp_exit
	.macro jmpl,target
	    .if $core_version > 5
		jl	target		; "large" model call
	    .else
		lr	%blink, [%status]
		add	%blink, %blink, 3
		j	target
	    .endif
	.endm

	.section ".init",text
_init:
	.if	$isa == "ARC"
	st	%fp, [%sp, 0]
	st	%blink, [%sp, 4]
	mov	%fp, %sp
	sub	%sp, %sp, 16
	.else
	.cfa_bf	_init
	push	%blink
	.cfa_push	{%blink}
	.endif

	.section ".init$999999", text, 1, 2, check_text_align=0
	jmpl	__mw_cpp_init
	.if	$isa == "ARC"
        ld      %blink, [%fp, 4]
        j.d     [%blink]
        ld.a    %fp, [%sp, 16]
	.else
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef
	.endif

	.section ".fini", text
_fini:
	.if	$isa == "ARC"
	st	%fp, [%sp, 0]
	st	%blink, [%sp, 4]
	mov	%fp, %sp
	sub	%sp, %sp, 16
	.else
	.cfa_bf	_fini
	push	%blink
	.cfa_push	{%blink}
	.endif

	.section ".fini$999999", text, 1, 2, check_text_align=0
	jmpl	__mw_cpp_exit
	.if	$isa == "ARC"
        ld      %blink, [%fp, 4]
        j.d     [%blink]
        ld.a    %fp, [%sp, 16]
	.else
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef
	.endif
	.end

