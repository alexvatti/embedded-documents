/*
Non-Volatile Multi Time programmable memory Driver.

In ANUSOC 8 NVM's Each of size : 128 32-words, connected with APB-Wrapper.
Total Addresses are : 0x000 to 0x3FF (8* 128).
*/

#include<common.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_nvm.h>

/* Registers*/
#define NVM_ADDRESS_REG		NVM_CTRL_BASE + 0x200
#define NVM_DATA_IN_REG		NVM_CTRL_BASE + 0x204
#define NVM_CONTROL_REG		NVM_CTRL_BASE + 0x208
#define NVM_DATA_OUT_REG	NVM_CTRL_BASE + 0x20C
#define NVM_STATUS_REG		NVM_CTRL_BASE + 0x210

/* Command Set*/
#define   NVM_PROG_CMD		0x09
#define   NVM_ERASE_CMD		0x05
#define   NVM_READ_CMD		0x03
#define   NVM_ENABLE_CMD	0x01

#define CHIP(x, i)      (i =((x & 0x380)>> 7))

int nvm_enable (unsigned int add){
    unsigned int n, rdata;
    CHIP(add,n);
//Enabling NVM 0 
    writel(NVM_CONTROL_REG,NVM_ENABLE_CMD);  
    while(1)
       {
         rdata = readl(NVM_STATUS_REG); //Read status register
         rdata = rdata & 0xFF;
         if(rdata>>n)                  //Check READY STATUS BIT
         break;
       }
    return 0;
}


int nvm_erase_word(unsigned int add){
    unsigned int n , rdata;
    CHIP(add,n);
    printf("Erasing Address: %X\n", add);
// Erase Command  
    writel(NVM_ADDRESS_REG,add);   
    writel(NVM_CONTROL_REG,NVM_ERASE_CMD);  //Set ERASE COMMAND
    while(1)
       {
         rdata = readl(NVM_STATUS_REG); 
         rdata = rdata & 0xFF;
         if(rdata >> n)                  
         break;
       }
    writel(NVM_CONTROL_REG,NVM_ENABLE_CMD); 
    return 0;
}

int nvm_write_word(unsigned int add, const unsigned int val){
    unsigned int n, rdata;
    CHIP(add,n);
/* Before Writing, Erase the NVM Location*/
    nvm_erase_word(add);
    printf("Writing into NVM Chip %d \n",n); 
    writel(NVM_ADDRESS_REG,add);  
    writel(NVM_DATA_IN_REG,val);  
    writel(NVM_CONTROL_REG,NVM_PROG_CMD);  //Set PROGRAM COMMAND
    while(1)
       {
         rdata = readl(NVM_STATUS_REG); 
         rdata = rdata & 0xFF;
         if(rdata >> n)              
         break;
       }
    writel(NVM_CONTROL_REG,NVM_ENABLE_CMD);  
    return 0;
}
int nvm_read_word(unsigned int add, unsigned int *buf )
{
    unsigned int rdata, temp;	
    unsigned int n;
    CHIP(add,n);

    printf(" Reading form Chip No: %d at location: 0x%X\n", n, add);
    writel(NVM_ADDRESS_REG, add);   
    writel(NVM_CONTROL_REG,NVM_READ_CMD);  //Set READ COMMAND
    while(1)
       {
         temp = readl(NVM_STATUS_REG); 
         rdata = temp & 0xFF;
         if(rdata >> n )                
         break;
       }

     *buf = readl(NVM_DATA_OUT_REG);     
     writel(NVM_CONTROL_REG,NVM_ENABLE_CMD);
// ECC Check 
    if(temp & 0xFF0000)
	printf("ECC operation performed\n"); 
    
    if(temp & 0xFF00 ){
	printf("ECC Check Failed, Aborting request\n");
	*buf = NULL;
	return -1;
	}
    return 0;
}

int nvm_read_buff(unsigned int *buf, unsigned int add, unsigned int len)
{
    unsigned int i ;
    
    add &= 0xFFF;
    printf("Reading %d words from address 0x%X \n", len, add);
    for(i=0; i < len; i++){
	nvm_read_word(add++,*(buf++));
	}
    return 0;
}
int nvm_write_buff (const unsigned int *buf, unsigned int add, unsigned int len)
{
    unsigned int i ;

    add &= 0xFFF;
    printf("Writing %d words to address 0x%X \n", len, add);
    for(i=0; i < len; i++){
        nvm_write_word(add++,*(buf++));
        }
    return 0;
}






