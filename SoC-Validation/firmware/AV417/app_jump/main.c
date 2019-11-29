#include "../headers/shikhara_uart.h"
#include "../headers/shikhara_map.h"

int main (int argc, char*argv[])
{
	print("Jumping to App\n");

	volatile int *p=(volatile int *)SHIKHARA_GPIO_BANK_A_DIR;
	volatile int *q=(volatile int *)SHIKHARA_GPIO_BANK_A_DATA;
	*p=0xff;
	*q=0xff;
	/* Save context */
	_ASM("PUSH %r2");
	_ASM("PUSH %r13");
	_ASM("PUSH %BLINK"); 
	/* Jump to App */
	_ASM("jl 0x01d00000"); 

	// Restore context

	/* Restore context */
	_ASM("POP %BLINK");
	_ASM("POP %r13");
	_ASM("POP %r2");

	return 0;

}

void isr3_dummy(void)
{
        // dummy isr
}
void isr4_dummy(void)
{
        // dummy isr
}
void isr5_dummy(void)
{
        // dummy isr
}
void isr6_dummy(void)
{
        // dummy isr
}
void isr7_dummy(void)
{
        // dummy isr
}
void isr8_dummy(void)
{
        // dummy isr
}
void isr9_dummy(void)
{
        // dummy isr
}
void isr10_dummy(void)
{
        // dummy isr
}
void isr11_dummy(void)
{
}
void isr12_dummy(void)
{
        // dummy isr
}
void isr13_dummy(void)
{
        // dummy isr
}
void isr14_dummy(void)
{
        // dummy isr
}
void isr15_dummy(void)
{
        // dummy isr
}


