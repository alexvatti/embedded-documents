//***************************************************************************
// IPC(Inter Processor Communication) base address and IPC register base address
//***************************************************************************
/*#define FPGA
#define ZYNQ*/
#ifdef FPGA
#ifdef ZYNQ
#define IPC_BASE_ADDR     0x5D5A0000     //IPC base address
#else         
#define IPC_BASE_ADDR  	  0xFD5A0000     // IPC base address
#endif
#else
#define IPC_BASE_ADDR  	  0xD45A0000     // IPC base address
#endif 

#ifdef ZYNQ
#define IPC_SHMEM        (volatile unsigned int *)(0x41D00000)     //IPC shared address
#else
#define IPC_SHMEM        (volatile unsigned int *)(0x0D100000)     //IPC shared address
#endif


#define IPC_REG0         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x000)) // Dummy Register
#define IPC_REG1         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x004)) // Dummy Register
#define IPC_REG2         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x008)) // Dummy Register
#define IPC_REG3         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x00C)) // VIDEO Application start Address Register
#define IPC_REG4         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x010)) // ARC IPC Interrupt generation register
#define IPC_REG5         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x014)) // STATUS Update Register
#define IPC_REG6         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x018)) // Dummy Register
#define IPC_REG7         ((volatile unsigned int *)(IPC_BASE_ADDR + 0x01C)) // ARM IPC Interrupt generation register


