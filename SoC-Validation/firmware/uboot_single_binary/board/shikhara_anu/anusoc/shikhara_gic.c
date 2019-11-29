/*
 * Source code for Shikhara GIC initialization
 */

#include <common.h>
#include <asm/arch/nand.h>
#include <netdev.h>
#include <configs/anusoc.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_irq.h>

static interrupt_distributor * id = (interrupt_distributor *)SHIKHARA_GIC_DISTRIBUTOR;
static gic_cpu_interface     * ci = (gic_cpu_interface *)SHIKHARA_GIC_INTERFACE;

void int_enable(int i)
{
  int word;
  word  = i / 32; // gives the offset
  i    %= 32;     // gets the bit position value 
  i     = 1 << i; 
  id->enable.set[word] = i; //place the value into the spi enable reigster
}

void int_disable(int j)
{
  int word;
  word  = j / 32; // gives the offset
  j    %= 32;     // gets the bit position value
  j     = 1 << j;
  id->enable.clear[word] = j; //place the value into the spi clear reigster
}

int gic_init(void)
{
        int max_irq,i;
        /* Disable the whole thing */
        id->control = 0x00000000;

        max_irq = SHIKHARA_MAX_IRQ;

        printf("%x is the value of irq\n",max_irq);

        /* Disable all interrupts */
        for (i=0; i < max_irq / 32; i++)
          {
            id->enable.clear[i] = 0xffffffff;
          }

        /* Disable all pending interrupts */
        for (i=1; i < max_irq / 32; i++)
          {
            id->pending.clear[i] = 0xffffffff;
          }

        /* Reset interrupt priorities */
        for (i=8; i < max_irq / 4; i++)
          {
            id->priority[i] = 0xa0a0a0a0;
          }

        /* Reset interrupt targets */
        for (i=0; i < max_irq / 4; i++)
          {
            id->target[i] = 0x01010101;
          }

        /* Set correct interrupt configuration (level sensitive, 1-N) */
        for (i=2; i < max_irq / 16; i++)
        {
            if((i ==(GNSS_INT/16))|| (i == (GPIO2_IRQ_NUM/16)))
	     id->configuration[i] = 0x5a5a5555; /*Edge sensitive for gnss and gpio interrupt line*/
	     else
	     id->configuration[i] = 0x55555555;
        }

#if 1
        /* GNSS Interrupt configured as edge triggered */
        #define GICD_ICFG_IDX(id)     ((id) >> 4)         /* address register[id]... */
        #define GICD_ICFG_SHFT(id)    (((id) & 0xf) << 1)
        #define GICD_ICFG_EDGE_INT    0x02
        id->configuration[GICD_ICFG_IDX(GNSS_INT)] |= GICD_ICFG_EDGE_INT << GICD_ICFG_SHFT(GNSS_INT);
#endif



        // Distributor Enable interrupts
        id->control = 0x00000001;

         // Interrupt CPU Interface
        // Set priority mask to enable all interrupts
        ci->priority_mask = 0xF0;

	 // Enable interrupts
        ci->control = 0x00000001;

	return(0);
}

