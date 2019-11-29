OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 . = ALIGN(4);
 .text :
 {
  __image_copy_start = .;

  arch/arm/cpu/armv7/start.o    (.text)
  *(.text)
   *(asm)
 }
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
 . = ALIGN(4);
 .data : {
  *(.data)
 }
 . = ALIGN(4);
 . = .;

 __u_boot_cmd_start = .;
 .u_boot_cmd : { *(.u_boot_cmd) }
 __u_boot_cmd_end = .;

  . = ALIGN(1024);  /* Used to align appf_cluster and appf_cpu  structures on 1024-byte boundary. */

  .APPF_TABLE_DATA :
  {
  *(.APPF_TABLE_DATA)
  }

  . = ALIGN(1024); /* Used to align functions appf_entry_point,appf_platform_runtime_init,appf_platform_get_cpu_index on 1024-byte boundary.*/

 .APPF_ENTRY_POINT_CODE :
  {
 *(.APPF_ENTRY_POINT_CODE)
  }

  . = ALIGN(4);

  .APPF_ENTRY_POINT_DATA :
   {
  *(.APPF_ENTRY_POINT_DATA)
   }
   
  . = ALIGN(1024); /* Used to align mmu's level 2 table with 1024 bye boundary */

  .APPF_TRANSLATION_TABLE2 :
   {
  *(.APPF_TRANSLATION_TABLE2)
   }

   . = ALIGN(16384);  /* Used to align mmu's level 1 table with 16384 bye boundary */

  .APPF_TRANSLATION_TABLE1 :
   {
  *(.APPF_TRANSLATION_TABLE1)
   }

   . = ALIGN(4096);   
   .APPF_DEVICE_MEMORY :
    {
    *(.APPF_DEVICE_MEMORY)
    }

 . = ALIGN(4);
 __image_copy_end = .;
 .rel.dyn : {
  __rel_dyn_start = .;
  *(.rel*)
  __rel_dyn_end = .;
 }
 .dynsym : {
  __dynsym_start = .;
  *(.dynsym)
 }
 _end = .;
 . = ALIGN(4096);

 .mmutable : {
  *(.mmutable)
 }

 .bss __rel_dyn_start (OVERLAY) : {
  __bss_start = .;
  __bss_end = .;
  *(.bss*)
  *(COMMON)
   . = ALIGN(4);
  __bss_end__ = .;
 }
 . += 0x100;
 . = ALIGN(4096);
 .mali_data_check : 
  { 
 *(.mali_data_check) 
  }
 . += 0x100;
 . = ALIGN(4096);

 .mali_data_result : 
  { 
  *(.mali_data_result) 
  }


 /DISCARD/ : { *(.dynstr*) }
 /DISCARD/ : { *(.dynamic*) }
 /DISCARD/ : { *(.plt*) }
 /DISCARD/ : { *(.interp*) }
 /DISCARD/ : { *(.gnu*) }
}
