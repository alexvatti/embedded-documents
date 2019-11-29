#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/list.h>
#include <asm/arch/dma/shikara-dma-pl330.h>

#define XFER_UNIT	1024
#define XFER_SIZE	10
static unsigned int xfer_size = (XFER_SIZE * XFER_SIZE) ;
static unsigned short burst = 1;

struct shikara_dma_test {
	int size;
	enum dma_ch chan;
	unsigned int *dst_buff;
	unsigned int *src_buff;
	enum shikara_dma_buffresult res;
	struct shikara_dma_client cl;
};
struct shikara_dma_test	   *thread;

static bool dmatest_buf_same(u32 src[], u32 dst[], unsigned int bytes);

static void shikara_dma_cb(enum dma_ch *chan, void *buf_id,
		int size, enum shikara_dma_buffresult res)
{
	struct shikara_dma_test *thread = buf_id;
	#ifdef CONFIG_PL330_INTERRUPT_MODE
	/*Check the result */
	if (res != SHIKARA_RES_OK || size != xfer_size) {
		printf("SHIKARA DMA M2M Test: xfer is not successful Thrd-%u: Res-%u Xfer_size-%d!\n", thread->res, thread->size);
		return;
	}

        /* Check Transaction is successful or not */

        if(!dmatest_buf_same(thread->src_buff, thread->dst_buff, xfer_size)) {
                printf("SHIKARA DMA M2M Test:  Xfer_cmp failed! exiting\n");
	}
        else {
                printf("SHIKARA_DMA_TEST : Xfer is successful and Test is exiting\n");
        }
	#else
	thread->res = res;
	thread->size = size;
	//printf("SHIKARA DMA TEST : callback to cmd_dma res= %d \n",res);
	#endif
}

static void dmatest_init_buf(u32 buf[], int clr, unsigned int bytes)
{
	unsigned int i;

	for (i = 0; i < bytes / 4; i++)
		buf[i] = clr ? 0 : 0x1234;
}

static bool dmatest_buf_same(u32 src[], u32 dst[], unsigned int bytes)
{
	unsigned int i;

	for (i = 0; i < (bytes) / 4; i++)
		if (src[i] != dst[i])
			return false;

	printf("SHIKARA_DMA_TEST: Both buffers are having  same data \n");
	return true;
}
static int dmatest_func()
{
	int ret = 0 ;

	ret = shikara_dma_devconfig(thread->chan, SHIKARA_DMA_MEM2MEM,thread->src_buff);
	if(ret)
	{
		printf("SHIKARA DMA M2M Test: unable to configure the DMAC channel\n");
		return -1;
	}
	/*Generate Request for DMAC 2*/
	writel(0x800,SHIKHARA_SYS_CTRL_BASEADDR + 0x068);
	
	ret = shikara_dma_enqueue_ring(thread->chan, (void *)thread,thread->dst_buff, xfer_size,0);
	if(ret){
		printf("SHIKARA DMA M2M Test: unable to configure the DMAC channel\n");
		return -1;
	}
	

	//printf("SHIKARA DMA M2M Test: Control the DMAC channel\n");
	//shikara_dma_ctrl(chan, SHIKARA_DMAOP_START);
//	shikara_dma_getposition(DMACH_MTOM_0 + thread->id, &src, &dst);

	#ifndef CONFIG_PL330_INTERRUPT_MODE
	/*Check the result */			
	if (thread->res != SHIKARA_RES_OK || thread->size != xfer_size) {
		printf("SHIKARA DMA M2M Test: xfer is not successful Thrd-%u: Res-%u Xfer_size-%d!\n", thread->res, thread->size);
		return -1;
	}

	/* Check Transaction is successful or not */

	if(!dmatest_buf_same(thread->src_buff, thread->dst_buff, xfer_size))
		printf("SHIKARA DMA M2M Test:  Xfer_cmp failed! exiting\n");
	else {
		printf("SHIKARA_DMA_TEST : Xfer is successful and Test is exiting\n");	 	

	}
	shikara_dma_free(thread->chan, &thread->cl); 
	#endif
	return 0;

}
int do_dma_m2m(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	//struct shikara_dma_test *thread;
	int ret, i = 0;	

	thread = malloc(sizeof(struct shikara_dma_test));
	thread->chan = DMACH_M2M_SR; 
	thread->cl.name = "dma2 test";
	/*Allocate Memory for SRC buffer*/
	thread->src_buff = (unsigned int *)malloc(xfer_size);
	if (!thread->src_buff) {
		printf("SHIKARA DMA M2M Test: Thrd-%u No memory for src buff\n", i);
		goto src_alloc_err;
	}
	printf("SHIKARA DMA M2M Test: bufferSRC - %p  memory size : %u\n",thread->src_buff,xfer_size);

	/*Allocate Memory for DST buffer*/
	thread->dst_buff = (unsigned int *)malloc(xfer_size);
	if (!thread->dst_buff) {
		printf("SHIKARA DMA M2M Test: Thrd-%u No memory for dst buff\n", i);
		goto dst_alloc_err;
	}
	printf("SHIKARA DMA M2M Test: buffer DST - %p  memory size : %u\n",thread->dst_buff,xfer_size);
	
	dmatest_init_buf(thread->src_buff, 0, xfer_size);
	dmatest_init_buf(thread->dst_buff, 1, xfer_size);
	
	/*Initialise DMAC2*/
	if(dma_pl330_init(SHIKHARA_DMAC2)){
             printf("DMAC not initialised, exiting from DMA Test\n ");
             return 0;
         }
	printf("DMAC2 is initialised..\n");
	ret = shikara_dma_request(thread->chan,&thread->cl, NULL);
	if (ret) {
		printf("SHIKARA DMA M2M Test:Error Acquire in channel %d\n",ret);
		goto thrd_dma_acq_err;
	}

	shikara_dma_set_buffdone_fn(thread->chan, shikara_dma_cb);

	ret = shikara_dma_config(thread->chan, burst);
	if (ret) {
		printf("SHIKARA DMA M2M Test: Error config(%d)\n",ret);
		goto thrd_dma_acq_err;
	}
	ret= dmatest_func();

thrd_dma_acq_err:
	free(thread->dst_buff);
dst_alloc_err:
	free(thread->src_buff);
src_alloc_err:
	free(thread);
	return 0;
thread_exit : 
	printf("SHIKARA DMA M2M Test: Exiting...  \n");
	return 0;
}


U_BOOT_CMD(
		dma_m2m, 1, 1, do_dma_m2m,
		"Test DMA with memory to memory transfers",
		"DMA2 Controller Validation"
	  );

