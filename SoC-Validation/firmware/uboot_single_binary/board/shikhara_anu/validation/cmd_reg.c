/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>

int do_reg(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int a;
        volatile int flags,c;
        printf("..general purpose registers info..");
	__asm__ __volatile__ ("mov %0,r0" : : "r"(a) : );
	printf("\nr0=0x%x",a);
        __asm__ __volatile__ ("mov %0,r1" : : "r"(a) : );
	printf("\nr1=0x%x",a);
        __asm__ __volatile__ ("mov %0,r2" : : "r"(a) : );
	printf("\nr2=0x%x",a);
        __asm__ __volatile__ ("mov %0,r3" : : "r"(a) : );
	printf("\nr3=0x%x",a);
        __asm__ __volatile__ ("mov %0,r4" : : "r"(a) : );
	printf("\nr4=0x%x",a);
        __asm__ __volatile__ ("mov %0,r5" : : "r"(a) : );
	printf("\nr5=0x%x",a);
        __asm__ __volatile__ ("mov %0,r6" : : "r"(a) : );
	printf("\nr6=0x%x",a);
        __asm__ __volatile__ ("mov %0,r7" : : "r"(a) : );
	printf("\nr7=0x%x",a);
        __asm__ __volatile__ ("mov %0,r8" : : "r"(a) : );
	printf("\nr8=0x%x",a);
        __asm__ __volatile__ ("mov %0,r9" : : "r"(a) : );
	printf("\nr9=0x%x",a);
        __asm__ __volatile__ ("mov %0,r10" : : "r"(a) : );
	printf("\nr10=0x%x",a);
        __asm__ __volatile__ ("mov %0,r11" : : "r"(a) : );
	printf("\nr11=0x%x",a);
        __asm__ __volatile__ ("mov %0,r12" : : "r"(a) : );
	printf("\nr12=0x%x",a);
        __asm__ __volatile__ ("mov %0,r13" : : "r"(a) : );
	printf("\nsp=0x%x",a);
        __asm__ __volatile__ ("mov %0,r14" : : "r"(a) : );
	printf("\nlr=0x%x",a);
        __asm__ __volatile__ ("mov %0,r15" : : "r"(a) : );
	printf("\npc=0x%x",a);
        __asm__ __volatile__ ("mrs %0,cpsr" : : "r"(a) : "memory");
	printf("\ncpsr register is  =0x%x",a);
	  c=a;
	printf("\nInstruction mode is %s",(c&(1<<5)) ? "THUMB" : "ARM" );
	printf("\n mode of operation is %s",((c&0x1f) == 0x13 ) ? "SVC" : "NotSVC" );
        printf("\nIRQs are %s",((c&(1<<7))) ? "Off" : "On");
        printf("\nFIQs are %s",((c&(1<<6))) ? "Off" : "On");
        flags=(c>>28);
        printf ("\nFlags: %c %c %c %c",
                ((flags & (1<<0)) ? 'N' : 'n'),
                ((flags & (1<<1)) ? 'Z' : 'z'),
                (flags & (1<<2)) ? 'C' : 'c', flags & (1<<3) ? 'V' : 'v');
        
	printf("\n..Coprocessor control registers..");
        __asm__ __volatile__ ("mrc p15,0,%0,c0,c0,0" : : "r"(a) : );//reads Main ID register
	printf("\n0x%x is value of read id register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c0,c0,1" : : "r"(a) : );//Cache type register
	printf("\n0x%x is value of cache type register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c0,c0,5" : : "r"(a) : );//Mutliprocessor Afinity register
	printf("\n0x%x is value of multiprocessor affinity register",a);
        __asm__ __volatile__ ("mrc p15,1,%0,c0,c0,0" : : "r"(a) : );//Cache size identification register
	printf("\n0x%x is value of Cache size identification register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c1,c0,0" : : "r"(a) : );//system control register
	printf("\n0x%x is value of system control register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c1,c0,1" : : "r"(a) : );//Auxilary control register
	printf("\n0x%x is value of auxilary control register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c1,c0,2" : : "r"(a) : );//Coprocessor Acess control register
	printf("\n0x%x is value of coprocessor access control register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c2,c0,2" : : "r"(a) : );//Translation Base control register
	printf("\n0x%x is value of Translation Base control register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c3,c0,0" : : "r"(a) : );//Domain Acess control register
	printf("\n0x%x is value of Domain access control register",a);
        __asm__ __volatile__ ("mrc p15,0,%0,c12,c0,0" : : "r"(a) : );//Vector Base Address register
	printf("\n0x%x is value of Vector Base Address register\n",a);
	
	return(0);
}

U_BOOT_CMD(
	reginfo,	CONFIG_SYS_MAXARGS,	1,	do_reg,
	"\n",
	"	- print registers dump of processor\n"
);

