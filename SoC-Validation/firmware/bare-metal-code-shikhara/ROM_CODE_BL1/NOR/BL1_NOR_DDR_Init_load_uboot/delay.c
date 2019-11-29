
#include<common.h>

static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}
void udelay(unsigned long us)
{
	delay(us * 200); /* approximate */
}
void sdelay(unsigned long s)
{

	udelay(s * 1000); /* approximate */
}
/*void memcpy(void *dest, void *src, int n)
{
   // Typecast src and dest addresses to (char *)

   char *csrc = (char *)src;
   char *cdest = (char *)dest;
   int i=0;

   // Copy contents of src[] to dest[]
   for (i=0; i<n; i++)
       cdest[i] = csrc[i];
}*/
