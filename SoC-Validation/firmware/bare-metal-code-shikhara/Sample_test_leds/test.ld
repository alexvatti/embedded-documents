ENTRY(_Reset)
SECTIONS
{
 . = 0xD45B0000; /* On-Chip Memory Address 64KB*/
 .text : {
 	startup.o(INTERRUPT_VECTOR)
 	*(.text)
 }
 .data : { 
	*(.data) 
 }
 sbss = .;
 .bss : { 
      *(.bss COMMON) 
  }
  ebss = .;
  bss_size = ebss - sbss;

  . = ALIGN(8);
  . = . + 0x1000; /* 4kB of stack memory */
  stack_top = .;
}
