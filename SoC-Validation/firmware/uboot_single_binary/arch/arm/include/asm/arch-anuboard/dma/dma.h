
#define SHIKHARA_DMAC0		1
#define SHIKHARA_DMAC2		2


enum shikara_dma_buffresult {
	SHIKARA_RES_OK,
	SHIKARA_RES_ERR,
	SHIKARA_RES_ABORT
};

enum shikara_dmasrc {
	SHIKARA_DMASRC_HW,		/* source is memory */
	SHIKARA_DMASRC_MEM,		/* source is hardware */
	SHIKARA_DMA_MEM2MEM,
	SHIKARA_DMA_MEM2MEM_SET,
};

/* enum shikara_chan_op
 *
 * operation codes passed to the DMA code by the user, and also used
 * to inform the current channel owner of any changes to the system state
*/

enum shikara_chan_op {
	SHIKARA_DMAOP_START,
	SHIKARA_DMAOP_STOP,
	SHIKARA_DMAOP_PAUSE,
	SHIKARA_DMAOP_RESUME,
	SHIKARA_DMAOP_FLUSH,
	SHIKARA_DMAOP_TIMEOUT,		/* internal signal to handler */
	SHIKARA_DMAOP_STARTED,		/* indicate channel started */
};

struct shikara_dma_client {
	char                *name;
};

enum dma_ch;

/* shikara_dma_cbfn_t
 *
 * buffer callback routine type
*/

typedef void (*shikara_dma_cbfn_t)(enum dma_ch *,
				   void *buf, int size,
				   enum shikara_dma_buffresult result);

typedef int  (*shikara_dma_opfn_t)(enum dma_ch *,
				   enum shikara_chan_op );



/* shikara_dma_request
 *
 * request a dma channel exclusivley
*/

extern int shikara_dma_request(enum dma_ch channel,
			       struct shikara_dma_client *, void *dev);


/* shikara_dma_ctrl
 *
 * change the state of the dma channel
*/

extern int shikara_dma_ctrl(enum dma_ch channel, enum shikara_chan_op op);

/* shikara_dma_setflags
 *
 * set the channel's flags to a given state
*/

extern int shikara_dma_setflags(enum dma_ch channel,
				unsigned int flags);

/* shikara_dma_free
 *
 * free the dma channel (will also abort any outstanding operations)
*/

extern int shikara_dma_free(enum dma_ch channel, struct shikara_dma_client *);

/* shikara_dma_enqueue
 *
 * place the given buffer onto the queue of operations for the channel.
 * The buffer must be allocated from dma coherent memory, or the Dcache/WB
 * drained before the buffer is given to the DMA system.
*/

#define shikara_dma_enqueue(id, token, addr, size)		\
			shikara_dma_enqueue_ring(id, token, addr, size, 0)

/* shikara_dma_enqueue_ring
 *
 * place the given buffer onto the queue of operations for the channel.
 * The buffer must be allocated from dma coherent memory, or the Dcache/WB
 * drained before the buffer is given to the DMA system.
*/

extern int shikara_dma_enqueue_ring(enum dma_ch channel, void *id,
			       dma_addr_t data, int size, int numofblock);

/* shikara_dma_config
 *
 * configure the dma channel
*/

extern int shikara_dma_config(enum dma_ch channel, int xferunit);

/* shikara_dma_devconfig
 *
 * configure the device we're talking to
*/

extern int shikara_dma_devconfig(enum dma_ch channel,
		enum shikara_dmasrc source, unsigned long devaddr);

/* shikara_dma_getposition
 *
 * get the position that the dma transfer is currently at
*/

extern int shikara_dma_getposition(enum dma_ch channel,
				   dma_addr_t *src, dma_addr_t *dest);

//extern int shikara_dma_set_opfn(enum dma_ch, shikara_dma_opfn_t rtn);
extern int shikara_dma_set_buffdone_fn(enum dma_ch, shikara_dma_cbfn_t rtn);
extern int dma_pl330_init(int dmacid);
extern int pl330_remove(int dmacno);


