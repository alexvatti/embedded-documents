        @ Created by arm_to_gnu.pl from table_data.S
        .syntax unified

        @ Copyright (C) 2010 ARM Limited                           
        
        @ This software is provided 'as-is', without any express or implied
        @ warranties including the implied warranties of satisfactory quality, 
        @ fitness for purpose or non infringement.  In no event will  ARM be 
        @ liable for any damages arising from the use of this software.
        
        @ Permission is granted to anyone to use, copy and modify this software for 
        @ any purpose, and to redistribute the software, subject to the following 
        @ restrictions:
        
        @ 1. The origin of this software must not be misrepresented; you must not
        @    claim that you wrote the original software. If you use this software
        @    in a product, an acknowledgment in the product documentation would be
        @    appreciated but is not required.                                       
        @ 2. Altered source versions must be plainly marked as such, and must not be
        @    misrepresented as being the original software.
        @ 3. This notice may not be removed or altered from any source distribution.
        
        .section APPF_TABLE_DATA,"aw"   
        
        .eabi_attribute Tag_ABI_align_preserved, 1  
        
        .global main_table  
        
main_table:             
        .space 256,0    
        
        
        .section APPF_TRANSLATION_TABLE2,"aw"   
        
        .eabi_attribute Tag_ABI_align_preserved, 1  
        
        .global appf_translation_table2a    
        .global appf_translation_table2b    
        
appf_translation_table2a:               
        .space 1024,0   
appf_translation_table2b:               
        .space 1024,0   
        
        
        .section APPF_TRANSLATION_TABLE1,"aw"   
        
        .eabi_attribute Tag_ABI_align_preserved, 1  
        
        .global appf_translation_table1     
        
appf_translation_table1:                
        .space 16384,0  
        
        .section APPF_DEVICE_MEMORY,"aw"    
        
        .eabi_attribute Tag_ABI_align_preserved, 1  
        
        .global appf_device_memory  
        
appf_device_memory:             
        .space 4096,0   
        
        
        .end    
