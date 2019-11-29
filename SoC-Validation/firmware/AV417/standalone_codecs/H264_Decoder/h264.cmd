SECTIONS {
	GROUP ADDR(0x01D00000): {
               .rom_vectors?:{}
	       .boot?:{}
              * (TEXT): {}
              * (LIT): {}
              .sdata?: {}
	      .sbss?: {}
	      .Xdata? ALIGN(4) BLOCK(4): { * (.Xdata*) }
	      .Ydata? ALIGN(4) BLOCK(4): { * (.Ydata*) }
	      *(DATA): {}
              *(BSS): {}
              .heap? ALIGN(4) SIZE(DEFINED _HEAPSIZE?_HEAPSIZE:0x1000000): {}
              .stack ALIGN(4) SIZE(DEFINED _STACKSIZE?_STACKSIZE:163840): {}
              .hostlink ALIGN(256) : {}
       	}
      }
      
