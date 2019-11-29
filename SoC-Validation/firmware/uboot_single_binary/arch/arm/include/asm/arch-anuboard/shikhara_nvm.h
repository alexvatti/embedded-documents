#ifndef _SHIKHARA__NVM_H_
#define __SHIKHARA_NVM_H_

int nvm_enable( unsigned int add);
int nvm_erase_word(unsigned int add);

int nvm_write_word(unsigned int add, const unsigned int val);

int nvm_read_word(unsigned int add, unsigned int *buf );

int nvm_read_buff(unsigned int *buf, unsigned int add, unsigned int len);
int nvm_write_buff (const unsigned int *buf, unsigned int add, unsigned int len);

/*Dump total NVM Memory */
int nvm_dump(unsigned int *buf);

#endif

