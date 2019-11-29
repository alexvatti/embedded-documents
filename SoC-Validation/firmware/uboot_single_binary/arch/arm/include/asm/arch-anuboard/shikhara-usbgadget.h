
#define __REG(x)	(*(unsigned int *)(x))

#define ANUSOC_CLOCK_BASE	        0x10010000		//SAMSUNG Clock base
#define CLK_DIV_FSYS0_OFFSET            0x10548			//SAMSUNG Clock Register
#define CLK_DIV_FSYS0			__REG(ANUSOC_CLOCK_BASE+CLK_DIV_FSYS0_OFFSET)
