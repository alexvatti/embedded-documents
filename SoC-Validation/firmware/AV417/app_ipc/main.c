#include "../headers/shikhara_ipc.h"
#include "../headers/shikhara_uart.h"
#include "../headers/shikhara_map.h"

int read(volatile unsigned int *ptr)
{
	unsigned int ret;
	ret = *ptr;
	return ret;
}

void write(volatile unsigned int *ptr, int value)
{
	*ptr=value; 
}

int main(void)
{
	unsigned int a=10;
	unsigned int b=20;
	print("Enabling interrupts\n");

	_enable();

	/* Dummy Operation for ARC700 Processor */
	a = a + b;

	/* infinite loop Waiting for ARM Processor Interrupt */
	while(1);

	return 0;
}

int isr_ipc(void)
{

	print("int came\n");
	volatile int *p=(volatile int *)SHIKHARA_GPIO_BANK_A_DIR;
	volatile int *q=(volatile int *)SHIKHARA_GPIO_BANK_A_DATA;
	*p=0xff;
	*q=0x0;

	/* Clearing arc interrupt */  
	write(IPC_REG4,0x00000000);

	/* writing into shared memory */
	write(IPC_SHMEM,0x00000002);

	/* Write 0xFFFFFFFF to IPC_BASE_ADDR+0x10 to interrupt ARM processor */
	write(IPC_REG7, 0xFFFFFFFF);

	*q=0xff;

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
	isr_ipc();
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
