#include <asm/io.h>
#include <linux/list.h>
#include <common.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/dma/pl330.h>
#include <asm/arch/dma/shikara-pl330-pdata.h>
#include <asm/arch/shikhara_irq.h>

/**
 * struct shikara_pl330_dmac - Logical representation of a PL330 DMAC.
 * @busy_chan: Number of channels currently busy.
 * @peri: List of IDs of peripherals this DMAC can work with.
 * @node: To attach to the global list of DMACs.
 * @pi: PL330 configuration info for the DMAC.
 * @kmcache: Pool to quickly allocate xfers for all channels in the dmac.
 * @clk: Pointer of DMAC operation clock.
 */
struct shikara_pl330_dmac {
	unsigned		busy_chan;
	enum dma_ch		*peri;
	struct list_head	node;
	struct pl330_info	*pi;
};

/**
 * struct shikara_pl330_xfer - A request submitted by SHIKARA DMA clients.
 * @token: Xfer ID provided by the client.
 * @node: To attach to the list of xfers on a channel.
 * @px: Xfer for PL330 core.
 * @chan: Owner channel of this xfer.
 */
struct shikara_pl330_xfer {
	void			*token;
	struct list_head	node;
	struct pl330_xfer	px;
	struct shikara_pl330_chan	*chan;
};

/**
 * struct shikara_pl330_chan - Logical channel to communicate with
 * 	a Physical peripheral.
 * @pl330_chan_id: Token of a hardware channel thread of PL330 DMAC.
 * 	NULL if the channel is available to be acquired.
 * @id: ID of the peripheral that this channel can communicate with.
 * @options: Options specified by the client.
 * @sdaddr: Address provided via shikara_dma_devconfig.
 * @node: To attach to the global list of channels.
 * @lrq: Pointer to the last submitted pl330_req to PL330 core.
 * @xfer_list: To manage list of xfers enqueued.
 * @req: Two requests to communicate with the PL330 engine.
 * @callback_fn: Callback function to the client.
 * @rqcfg: Channel configuration for the xfers.
 * @xfer_head: Pointer to the xfer to be next executed.
 * @dmac: Pointer to the DMAC that manages this channel, NULL if the
 * 	channel is available to be acquired.
 * @client: Client of this channel. NULL if the
 * 	channel is available to be acquired.
 */
struct shikara_pl330_chan {
	void				*pl330_chan_id;
	enum dma_ch			id;
	unsigned int			options;
	unsigned long			sdaddr;
	struct list_head		node;
	struct pl330_req		*lrq;
	struct list_head		xfer_list;
	struct pl330_req		req[2];
	shikara_dma_cbfn_t		callback_fn;
	struct pl330_reqcfg		rqcfg;
	struct shikara_pl330_xfer		*xfer_head;
	struct shikara_pl330_dmac		*dmac;
	struct shikara_dma_client	*client;
};

/* All DMACs in the platform */
static LIST_HEAD(dmac_list);

/* All channels to peripherals in the platform */
static LIST_HEAD(chan_list);

#ifdef CONFIG_PL330_INTERRUPT_MODE
int dma_interr;
struct pl330_info *dma_inte_pil;
#endif

/* Returns the channel with ID 'id' in the chan_list */
static struct shikara_pl330_chan *id_to_chan(const enum dma_ch id)
{
	struct shikara_pl330_chan *ch;

	list_for_each_entry(ch, &chan_list, node)
		if (ch->id == id)
			return ch;

	return NULL;
}

/* Allocate a new channel with ID 'id' and add to chan_list */
static void chan_add(const enum dma_ch id)
{
	struct shikara_pl330_chan *ch = id_to_chan(id);

	/* Return if the channel already exists */
	if (ch)
		return;

	ch = malloc(sizeof(struct shikara_pl330_chan));
	/* Return silently to work with other channels */
	if (!ch)
		return;

	ch->id = id;
	ch->dmac = NULL;

	list_add_tail(&ch->node, &chan_list);
}

/* If the channel is not yet acquired by any client */
static bool chan_free(struct shikara_pl330_chan *ch)
{
	if (!ch)
		return false;

	/* Channel points to some DMAC only when it's acquired */
	return ch->dmac ? false : true;
}

/*
 * Returns 0 is peripheral i/f is invalid or not present on the dmac.
 * Index + 1, otherwise.
 */
static unsigned iface_of_dmac(struct shikara_pl330_dmac *dmac, enum dma_ch ch_id)
{
	enum dma_ch *id = dmac->peri;
	int i;

	/* Discount invalid markers */
	if (ch_id == DMACH_MAX)
		return 0;

	for (i = 0; i < PL330_MAX_PERI; i++)
		if (id[i] == ch_id)
			return i + 1;

	return 0;
}

/* If all channel threads of the DMAC are busy */
static inline bool dmac_busy(struct shikara_pl330_dmac *dmac)
{
	struct pl330_info *pi = dmac->pi;

	return (dmac->busy_chan < pi->pcfg.num_chan) ? false : true;
}

/*
 * Returns the number of free channels that
 * can be handled by this dmac only.
 */
static unsigned ch_onlyby_dmac(struct shikara_pl330_dmac *dmac)
{
	enum dma_ch *id = dmac->peri;
	struct shikara_pl330_dmac *d;
	struct shikara_pl330_chan *ch;
	unsigned found, count = 0;
	enum dma_ch p;
	int i;

	for (i = 0; i < PL330_MAX_PERI; i++) {
		p = id[i];
		ch = id_to_chan(p);

		if (p == DMACH_MAX || !chan_free(ch))
			continue;

		found = 0;
		list_for_each_entry(d, &dmac_list, node) {
			if (d != dmac && iface_of_dmac(d, ch->id)) {
				found = 1;
				break;
			}
		}
		if (!found)
			count++;
	}

	return count;
}

/*
 * Measure of suitability of 'dmac' handling 'ch'
 *
 * 0 indicates 'dmac' can not handle 'ch' either
 * because it is not supported by the hardware or
 * because all dmac channels are currently busy.
 *
 * >0 vlaue indicates 'dmac' has the capability.
 * The bigger the value the more suitable the dmac.
 */
#define UINT_MAX	(~0U)
#define MAX_SUIT	UINT_MAX
#define MIN_SUIT	0

static unsigned suitablility(struct shikara_pl330_dmac *dmac,
		struct shikara_pl330_chan *ch)
{
	struct pl330_info *pi = dmac->pi;
	enum dma_ch *id = dmac->peri;
	struct shikara_pl330_dmac *d;
	unsigned s;
	int i;

	s = MIN_SUIT;
	/* If all the DMAC channel threads are busy */
	if (dmac_busy(dmac))
		return s;

	for (i = 0; i < PL330_MAX_PERI; i++)
		if (id[i] == ch->id)
			break;

	/* If the 'dmac' can't talk to 'ch' */
	if (i == PL330_MAX_PERI)
		return s;

	s = MAX_SUIT;
	list_for_each_entry(d, &dmac_list, node) {
		/*
		 * If some other dmac can talk to this
		 * peri and has some channel free.
		 */
		if (d != dmac && iface_of_dmac(d, ch->id) && !dmac_busy(d)) {
			s = 0;
			break;
		}
	}
	if (s)
		return s;

	s = 100;

	/* Good if free chans are more, bad otherwise */
	s += (pi->pcfg.num_chan - dmac->busy_chan) - ch_onlyby_dmac(dmac);

	return s;
}

/* More than one DMAC may have capability to transfer data with the
 * peripheral. This function assigns most suitable DMAC to manage the
 * channel and hence communicate with the peripheral.
 */
static struct shikara_pl330_dmac *map_chan_to_dmac(struct shikara_pl330_chan *ch)
{
	struct shikara_pl330_dmac *d, *dmac = NULL;
	unsigned sn, sl = MIN_SUIT;

	list_for_each_entry(d, &dmac_list, node) {
		sn = suitablility(d, ch);

		if (sn == MAX_SUIT)
			return d;

		if (sn > sl)
			dmac = d;
	}

	return dmac;
}

/* Acquire the channel for peripheral 'id' */
static struct shikara_pl330_chan *chan_acquire(const enum dma_ch id)
{
	struct shikara_pl330_chan *ch = id_to_chan(id);
	struct shikara_pl330_dmac *dmac;

	/* If the channel doesn't exist or is already acquired */
	if (!ch || !chan_free(ch)) {
		ch = NULL;
		goto acq_exit;
	}

	dmac = map_chan_to_dmac(ch);
	/* If couldn't map */
	if (!dmac) {
		ch = NULL;
		goto acq_exit;
	}

	dmac->busy_chan++;
	ch->dmac = dmac;

acq_exit:
	return ch;
}

/* Delete xfer from the queue */
static inline void del_from_queue(struct shikara_pl330_xfer *xfer)
{
	struct shikara_pl330_xfer *t;
	struct shikara_pl330_chan *ch;
	int found;

	if (!xfer)
		return;

	ch = xfer->chan;

	/* Make sure xfer is in the queue */
	found = 0;
	list_for_each_entry(t, &ch->xfer_list, node)
		if (t == xfer) {
			found = 1;
			break;
		}

	if (!found)
		return;

	/* If xfer is last entry in the queue */
	if (xfer->node.next == &ch->xfer_list)
		t = list_entry(ch->xfer_list.next,
				struct shikara_pl330_xfer, node);
	else
		t = list_entry(xfer->node.next,
				struct shikara_pl330_xfer, node);

	/* If there was only one node left */
	if (t == xfer)
		ch->xfer_head = NULL;
	else if (ch->xfer_head == xfer)
		ch->xfer_head = t;

	list_del(&xfer->node);
}

/* Provides pointer to the next xfer in the queue.
 * If CIRCULAR option is set, the list is left intact,
 * otherwise the xfer is removed from the list.
 * Forced delete 'pluck' can be set to override the CIRCULAR option.
 */
static struct shikara_pl330_xfer *get_from_queue(struct shikara_pl330_chan *ch,
		int pluck)
{
	struct shikara_pl330_xfer *xfer = ch->xfer_head;

	if (!xfer)
		return NULL;

	/* If xfer is last entry in the queue */
	if (xfer->node.next == &ch->xfer_list)
		ch->xfer_head = list_entry(ch->xfer_list.next,
				struct shikara_pl330_xfer, node);
	else
		ch->xfer_head = list_entry(xfer->node.next,
				struct shikara_pl330_xfer, node);

	if (pluck || !(ch->options & SHIKARA_DMAF_CIRCULAR))
		del_from_queue(xfer);

	return xfer;
}

static inline void add_to_queue(struct shikara_pl330_chan *ch,
		struct shikara_pl330_xfer *xfer, int front)
{
	struct pl330_xfer *xt;

	/* If queue empty */
	if (ch->xfer_head == NULL)
		ch->xfer_head = xfer;

	xt = &ch->xfer_head->px;
	/* If the head already submitted (CIRCULAR head) */
	if (ch->options & SHIKARA_DMAF_CIRCULAR &&
			(xt == ch->req[0].x || xt == ch->req[1].x))
		ch->xfer_head = xfer;

	/* If this is a resubmission, it should go at the head */
	if (front) {
		ch->xfer_head = xfer;
		list_add(&xfer->node, &ch->xfer_list);
	} else {
		list_add_tail(&xfer->node, &ch->xfer_list);
	}
	//printf("SHIKARA-PL330: Xfer is added to channel xfer list \n");
}

static inline void _finish_off(struct shikara_pl330_xfer *xfer,
		enum shikara_dma_buffresult res, int ffree)
{
	struct shikara_pl330_chan *ch;

	if (!xfer)
		return;
	//printf("SHIKARA-PL330: _finish_off request	\n");

	ch = xfer->chan;

	/* Do callback */
	if (ch->callback_fn)
		ch->callback_fn(NULL, xfer->token, xfer->px.bytes, res);

	/* Force Free or if buffer is not needed anymore */
	if (ffree || !(ch->options & SHIKARA_DMAF_CIRCULAR))
		free(xfer);
}

static inline int shikara_pl330_submit(struct shikara_pl330_chan *ch,
		struct pl330_req *r)
{	
	struct shikara_pl330_dmac *shikara_pl330_dmac;
	struct shikara_pl330_xfer *xfer;
	int ret = 0;

	/* If already submitted */
	if (r->x){
		//printf("SHIKARA-PL330: request is already submitted to DMAC\n");
		return 0;
	}
	//printf("SHIKARA-PL330:shikara_pl330_submit  submit request to DMAC\n");

	xfer = get_from_queue(ch, 0);
	if (xfer) {
		r->x = &xfer->px;

		/* Use max bandwidth for M<->M xfers */
		if (r->rqtype == MEMTOMEM) {
			struct pl330_info *pi = xfer->chan->dmac->pi;
			int burst = 1 << ch->rqcfg.brst_size;
			u32 bytes = r->x->bytes;
			int bl;

			bl = pi->pcfg.data_bus_width / 8;
			bl *= pi->pcfg.data_buf_dep;
			bl /= burst;

			/* src/dst_burst_len can't be more than 16 */
			if (bl > 16)
				bl = 16;

			while (bl > 1) {
				if (!(bytes % (bl * burst)))
					break;
				bl--;
			}

			ch->rqcfg.brst_len = bl;
		} else {
			ch->rqcfg.brst_len = 1;
		}

		ret = pl330_submit_req(ch->pl330_chan_id, r);

		/* If submission was successful */
		if (!ret) {
			//printf("SHIKARA-PL330:submit request was Successful \n");
			ch->lrq = r; /* latest submitted req */
			return 0;
		}
		printf("SHIKARA-PL330:submit request was not Successful \n");
		r->x = NULL;

		/* If both of the PL330 ping-pong buffers filled */
		if (ret == -EAGAIN) {

			/* Queue back again */
			printf("SHIKARA-PL330:Error in submitting submit request again \n");
			add_to_queue(ch, xfer, 1);
			ret = 0;
		} else {

			_finish_off(xfer, SHIKARA_RES_ERR, 0);
		}
	}

	return ret;
}

static void shikara_pl330_rq(struct shikara_pl330_chan *ch,
		struct pl330_req *r, enum pl330_op_err err)
{
	struct shikara_pl330_xfer *xfer;
	struct pl330_xfer *xl = r->x;
	enum shikara_dma_buffresult res;

	if (!r->infiniteloop) {
		r->x = NULL;
		//printf("SHIKARA-PL330: request is submitting  in non infinitloop \n");

		shikara_pl330_submit(ch, r);
	}

	/* Map result to SHIKARA DMA API */
	if (err == PL330_ERR_NONE)
		res = SHIKARA_RES_OK;
	else if (err == PL330_ERR_ABORT)
		res = SHIKARA_RES_ABORT;
	else
		res = SHIKARA_RES_ERR;

	/* If last request had some xfer */
	if (!r->infiniteloop) {
		if (xl) {
			xfer = container_of(xl, struct shikara_pl330_xfer, px);
			_finish_off(xfer, res, 0);
		} 
	} else {
		/* Do callback */

		xfer = container_of(xl, struct shikara_pl330_xfer, px);
		if (ch->callback_fn)
			ch->callback_fn(NULL, xfer->token, xfer->px.bytes, res);
	}
}

static void shikara_pl330_rq0(void *token, enum pl330_op_err err)
{
	struct pl330_req *r = token;
	struct shikara_pl330_chan *ch = container_of(r,
			struct shikara_pl330_chan, req[0]);
	shikara_pl330_rq(ch, r, err);
}

static void shikara_pl330_rq1(void *token, enum pl330_op_err err)
{
	struct pl330_req *r = token;
	struct shikara_pl330_chan *ch = container_of(r,
			struct shikara_pl330_chan, req[1]);
	shikara_pl330_rq(ch, r, err);
}

/* Release an acquired channel */
static void chan_release(struct shikara_pl330_chan *ch)
{
	struct shikara_pl330_dmac *dmac;

	if (chan_free(ch))
		return;

	dmac = ch->dmac;
	ch->dmac = NULL;
	dmac->busy_chan--;
}

int shikara_dma_ctrl(enum dma_ch id, enum shikara_chan_op op)
{
	struct shikara_pl330_xfer *xfer;
	enum pl330_chan_op pl330op;
	struct shikara_pl330_chan *ch;
	struct pl330_info * pil;
	struct shikara_pl330_dmac *shikara_pl330_dmac;
	int idx, ret;

	ch = id_to_chan(id);

	shikara_pl330_dmac = ch->dmac;
	pil = shikara_pl330_dmac->pi ;

	if (!ch || chan_free(ch)) {
		ret = -EINVAL;
		printf("SHIKARA-PL330: Channel %u is not free to dma_ctrl  \n",id);
		goto ctrl_exit;
	}

	switch (op) {
		case SHIKARA_DMAOP_START:
			/* Make sure both reqs are enqueued */
			idx = (ch->lrq == &ch->req[0]) ? 1 : 0;
			shikara_pl330_submit(ch, &ch->req[idx]);
			shikara_pl330_submit(ch, &ch->req[1 - idx]);
			pl330op = PL330_OP_START;
			break;

		case SHIKARA_DMAOP_STOP:
			pl330op = PL330_OP_ABORT;
			break;

		case SHIKARA_DMAOP_FLUSH:
			pl330op = PL330_OP_FLUSH;
			break;

		case SHIKARA_DMAOP_PAUSE:
		case SHIKARA_DMAOP_RESUME:
		case SHIKARA_DMAOP_TIMEOUT:
		case SHIKARA_DMAOP_STARTED:
			return 0;

		default:
			return -EINVAL;
	}
	printf("SHIKARA-PL330:call chan_ctrl  to start DMAC:  %d\n",pl330op);

	ret = pl330_chan_ctrl(ch->pl330_chan_id, pl330op);

	if (pl330op == PL330_OP_START) {
		//printf("SHIKARA-PL330: OP_START return \n");
		return ret;
	}
	//printf("SHIKARA-PL330:Finish off the Xfers of channel %d\n",id);
	idx = (ch->lrq == &ch->req[0]) ? 1 : 0;

	/* Abort the current xfer */
	if (ch->req[idx].x) {
		xfer = container_of(ch->req[idx].x,
				struct shikara_pl330_xfer, px);

		/* Drop xfer during FLUSH */
		if (pl330op == PL330_OP_FLUSH)
			del_from_queue(xfer);

		ch->req[idx].x = NULL;

		_finish_off(xfer, SHIKARA_RES_ABORT,
				pl330op == PL330_OP_FLUSH ? 1 : 0);
	}

	/* Flush the whole queue */
	if (pl330op == PL330_OP_FLUSH) {

		if (ch->req[1 - idx].x) {
			xfer = container_of(ch->req[1 - idx].x,
					struct shikara_pl330_xfer, px);

			del_from_queue(xfer);
			//printf("SHIKARA-PL330: flush whole queue\n");

			ch->req[1 - idx].x = NULL;
			_finish_off(xfer, SHIKARA_RES_ABORT, 1);
		}

		/* Finish off the remaining in the queue */
		xfer = ch->xfer_head;
		while (xfer) {

			del_from_queue(xfer);

			_finish_off(xfer, SHIKARA_RES_ABORT, 1);
			xfer = ch->xfer_head;
		}
	}

ctrl_exit:
	//printf("SHIKARA-PL330: return from dma_ctrl exit \n");
	return ret;
}


int shikara_dma_enqueue_ring(enum dma_ch id, void *token,
		dma_addr_t addr, int size, int numofblock)
{
	struct shikara_pl330_chan *ch;
	struct shikara_pl330_xfer *xfer;
	struct shikara_pl330_dmac *shikara_pl330_dmac;
	struct pl330_info * pil;
	//unsigned long flags;
	int idx, ret = 0, res =0;

	ch = id_to_chan(id);

	shikara_pl330_dmac = ch->dmac;
	pil = shikara_pl330_dmac->pi ;

	#ifdef CONFIG_PL330_INTERRUPT_MODE
	dma_inte_pil = pil; //store pil in a globle pointer for interrupt handler
	#endif

	/* Error if invalid or free channel */
	if (!ch || chan_free(ch)) {
		ret = -EINVAL;
		goto enq_exit;
	}
	printf("SHIKARA-PL330: channel got to submit request for  DMAC %X\n", id);

	/* Error if size is unaligned */
	if (ch->rqcfg.brst_size && size % (1 << ch->rqcfg.brst_size)) {
		ret = -EINVAL;
		printf("SHIKARA-PL330: Error in burst size(3) %d\n",ch->rqcfg.brst_size);
		goto enq_exit;
	}

	xfer = malloc(sizeof(struct shikara_pl330_xfer));
	if (!xfer) {
		ret = -ENOMEM;
		printf("SHIKARA-PL330: unable to allocate memory for Xfer\n");
		goto enq_exit;
	}
	//printf("SHIKARA-PL330: Allocated memory for Xfer\n");
	xfer->token = token;
	xfer->chan = ch;
	xfer->px.bytes = size;
	xfer->px.next = NULL; /* Single request */

	/* For SHIKARA DMA API, direction is always fixed for all xfers */
	if (ch->req[0].rqtype == MEMTODEV) {
		xfer->px.src_addr = addr;
		xfer->px.dst_addr = ch->sdaddr;
	} else {
		xfer->px.src_addr = ch->sdaddr;
		xfer->px.dst_addr = addr;
	}

	add_to_queue(ch, xfer, 0);

	/* Try submitting on either request */
	idx = (ch->lrq == &ch->req[0]) ? 1 : 0;

	if (!ch->req[idx].x) {
		ch->req[idx].infiniteloop = numofblock;//0
		res = shikara_pl330_submit(ch, &ch->req[idx]);
	} else {
		ch->req[1 - idx].infiniteloop = numofblock;
		res = shikara_pl330_submit(ch, &ch->req[1 - idx]);
	}
	//if (ch->options & SHIKARA_DMAF_AUTOSTART)
	res= shikara_dma_ctrl(id, SHIKARA_DMAOP_START);

	if(!res){
		#ifdef CONFIG_PL330_INTERRUPT_MODE
		/* waiting for complete interrupts*/

		while(!dma_interr)
			printf("In while waiting for complete the interrupt.\n");
		if(pil->dmac == 2)
		{
			int_disable(DMA2_IRQ_NUM);
		}
		else if(pil->dmac == 0)
		{
                        int_disable(DMA0_IRQ_NUM);
		}
		else
		{
			printf("Invalid DMAC Id, Can't disable interrupt\n");
		}

		#else
		printf("SHIKARA-PL330: Now poll for interrupts\n");

		/* poll for interrupts*/
		pl330_irq_poll(pil);
		#endif
		return 0;
	}

enq_exit:
	//printf("SHIKARA-PL330: exit from enq_exit\n");
	return ret;
}

int shikara_dma_request(enum dma_ch id,
		struct shikara_dma_client *client,
		void *dev)
{
	struct shikara_pl330_dmac *dmac;
	struct shikara_pl330_chan *ch;

	int ret = 0;

	ch = chan_acquire(id);
	if (!ch) {
		ret = -EBUSY;
		printf("SHIKARA-PL330: channel is not free in DMAC\n");
		goto req_exit;
	}

	dmac = ch->dmac;
	//printf("SHIKARA-PL330: channel is assigned in SHIKARA-DMAc\n");

	ch->pl330_chan_id = pl330_request_channel(dmac->pi);
	if (!ch->pl330_chan_id) {
		chan_release(ch);
		ret = -EBUSY;
		printf("SHIKARA-PL330: channel is not free DMAC-pl330\n");
		goto req_exit;
	}

	ch->client = client;
	ch->options = 0; /* Clear any option */
	ch->callback_fn = NULL; /* Clear any callback */
	ch->lrq = NULL;

	ch->rqcfg.brst_size = 2; /* Default word size */
	ch->rqcfg.swap = SWAP_NO;
	ch->rqcfg.scctl = SCCTRL0; /* Noncacheable and nonbufferable */
	ch->rqcfg.dcctl = DCCTRL0; /* Noncacheable and nonbufferable */
	ch->rqcfg.privileged = 0;
	ch->rqcfg.insnaccess = 0;

	/* Set invalid direction */
	ch->req[0].rqtype = DEVTODEV;
	ch->req[1].rqtype = ch->req[0].rqtype;

	ch->req[0].cfg = &ch->rqcfg;
	ch->req[1].cfg = ch->req[0].cfg;

	ch->req[0].peri = iface_of_dmac(dmac, id) - 1; /* Original index */
	ch->req[1].peri = ch->req[0].peri;

	ch->req[0].token = &ch->req[0];
	ch->req[0].xfer_cb = shikara_pl330_rq0;
	ch->req[1].token = &ch->req[1];
	ch->req[1].xfer_cb = shikara_pl330_rq1;

	ch->req[0].x = NULL;
	ch->req[1].x = NULL;

	/* Reset xfer list */
	INIT_LIST_HEAD(&ch->xfer_list);
	ch->xfer_head = NULL;
	printf("SHIKARA-PL330: channel request is successful DMAC\n");

req_exit:
	return ret;
}

int shikara_dma_free(enum dma_ch id, struct shikara_dma_client *client)
{
	struct shikara_pl330_chan *ch;
	struct shikara_pl330_xfer *xfer;
	int ret = 0;
	unsigned idx;

	ch = id_to_chan(id);
	//printf("SHIKARA-PL330: in DMA free  \n");

	if (!ch || chan_free(ch)){
		printf("SHIKARA-PL330:DMA channel not free	\n");
		goto free_exit;
	}

	/* Refuse if someone else wanted to free the channel */
	if (ch->client != client) {
		ret = -EBUSY;
		printf("SHIKARA-PL330: Refuse to free channel \n");
		goto free_exit;
	}

	/* Stop any active xfer, Flushe the queue and do callbacks */
	pl330_chan_ctrl(ch->pl330_chan_id, PL330_OP_FLUSH);

	/* Abort the submitted requests */
	idx = (ch->lrq == &ch->req[0]) ? 1 : 0;

	if (ch->req[idx].x) {
		xfer = container_of(ch->req[idx].x,
				struct shikara_pl330_xfer, px);

		ch->req[idx].x = NULL;
		del_from_queue(xfer);

		_finish_off(xfer, SHIKARA_RES_ABORT, 1);
	}

	if (ch->req[1 - idx].x) {
		xfer = container_of(ch->req[1 - idx].x,
				struct shikara_pl330_xfer, px);

		ch->req[1 - idx].x = NULL;
		del_from_queue(xfer);

		_finish_off(xfer, SHIKARA_RES_ABORT, 1);
	}

	/* Pluck and Abort the queued requests in order */
	do {
		xfer = get_from_queue(ch, 1);

		_finish_off(xfer, SHIKARA_RES_ABORT, 1);
	} while (xfer);

	ch->client = NULL;

	pl330_release_channel(ch->pl330_chan_id);

	ch->pl330_chan_id = NULL;

	chan_release(ch);

free_exit:
	//printf("SHIKARA-PL330: exit from dma_free DMAC\n");

	return ret;
}

int shikara_dma_config(enum dma_ch id, int xferunit)
{
	struct shikara_pl330_chan *ch;
	struct pl330_info *pi;
	int i, dbwidth, ret = 0;

	ch = id_to_chan(id);

	if (!ch || chan_free(ch)) {
		ret = -EINVAL;
		printf("SHIKARA-PL330: channel is not free DMAC\n");
		goto cfg_exit;
	}

	pi = ch->dmac->pi;
	dbwidth = pi->pcfg.data_bus_width / 8;

	/* Max size of xfer can be pcfg.data_bus_width */
	if (xferunit > dbwidth) {
		ret = -EINVAL;
		printf("SHIKARA-PL330: max xfer size is reached %d \n",dbwidth);
		goto cfg_exit;
	}

	i = 0;
	while (xferunit != (1 << i))
		i++;

	/* If valid value */
	if (xferunit == (1 << i))
		ch->rqcfg.brst_size = i;
	else
		ret = -EINVAL;

cfg_exit:
	return ret;
}


/* Options that are supported by this driver */
#define SHIKARA_PL330_FLAGS (SHIKARA_DMAF_CIRCULAR | SHIKARA_DMAF_AUTOSTART)

int shikara_dma_setflags(enum dma_ch id, unsigned int options)
{
	struct shikara_pl330_chan *ch;
	int ret = 0;

	ch = id_to_chan(id);

	if (!ch || chan_free(ch) || options & ~(SHIKARA_PL330_FLAGS))
		ret = -EINVAL;
	else
		ch->options = options;

	return 0;
}

int shikara_dma_getposition(enum dma_ch id, dma_addr_t *src, dma_addr_t *dst)
{
	struct shikara_pl330_chan *ch = id_to_chan(id);
	struct pl330_chanstatus status;
	int ret;

	if (!ch || chan_free(ch)){
		printf("SHIKARA-PL330: channel is not free to get position DMAC\n");
		return -EINVAL;
	}

	ret = pl330_chan_status(ch->pl330_chan_id, &status);
	if (ret < 0){
		printf("Error in channel %d  statcus \n",id);

		return ret;
	}
	//printf("SHIKARA-PL330: return from dma_getposition DMAC\n");
	*src = status.src_addr;
	*dst = status.dst_addr;

	return 0;
}

int shikara_dma_set_buffdone_fn(enum dma_ch id, shikara_dma_cbfn_t rtn)
{
	struct shikara_pl330_chan *ch;
	int ret = 0;

	ch = id_to_chan(id);

	if (!ch || chan_free(ch)){
		ret = -EINVAL;
		printf("SHIKARA-PL330: Error in setting callback function for DMAC\n");

	}
	else
		ch->callback_fn = rtn;
	//printf("SHIKARA-PL330: set callback function for DMAC channel\n");

	return ret;
}

int shikara_dma_devconfig(enum dma_ch id, enum shikara_dmasrc source,
		unsigned long address)
{
	struct shikara_pl330_chan *ch;
	int ret = 0;

	ch = id_to_chan(id);

	if (!ch || chan_free(ch)) {
		ret = -EINVAL;
		printf("SHIKARA-PL330 :Erro in channel request\n");
		goto devcfg_exit;

	}

	switch (source) {
		case SHIKARA_DMASRC_HW: /* P->M */
			ch->req[0].rqtype = DEVTOMEM;
			ch->req[1].rqtype = DEVTOMEM;
			ch->rqcfg.src_inc = 0;
			ch->rqcfg.dst_inc = 1;
			break;
		case SHIKARA_DMASRC_MEM: /* M->P */
			ch->req[0].rqtype = MEMTODEV;
			ch->req[1].rqtype = MEMTODEV;
			ch->rqcfg.src_inc = 1;
			ch->rqcfg.dst_inc = 0;
			break;
		case SHIKARA_DMA_MEM2MEM:
			ch->req[0].rqtype = MEMTOMEM;
			ch->req[1].rqtype = MEMTOMEM;
			ch->rqcfg.src_inc = 1;
			ch->rqcfg.dst_inc = 1;
			break;
		case SHIKARA_DMA_MEM2MEM_SET:
			ch->req[0].rqtype = MEMTOMEM;
			ch->req[1].rqtype = MEMTOMEM;
			ch->rqcfg.src_inc = 0;
			ch->rqcfg.dst_inc = 1;
			break;
		default:
			ret = -EINVAL;
			goto devcfg_exit;
	}

	ch->sdaddr = address;
	//printf("SHIKARA-PL330: DMAC channel configured successfuly with source address %u\n",address);

devcfg_exit:
	return ret;
}

#ifdef CONFIG_PL330_INTERRUPT_MODE
void dma_interrupt(int nu, int val)
{
	printf("dma_interrupt called.\n");

        pl330_update(dma_inte_pil);
	dma_interr = 1;

	return;
}
#endif

int dma_pl330_init(int dmacid)
{
	struct shikara_pl330_dmac *shikara_pl330_dmac;
	struct shikara_pl330_chndata *pl330pd;
	struct pl330_info *pl330_info;
	int i, ret;

	if(dmacid == SHIKHARA_DMAC2){
		/*Reset DMAC */
		writel(0xC,SHIKHARA_SYS_CTRL_BASEADDR + 0x44);
		udelay(10);
		writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR + 0x44);	

		pl330pd = &shikara_mdma;//ch_list
		/* Can't do without the list of _32_ peripherals */
		if (!pl330pd || !pl330pd->peri) {
			printf("SHIKARA-PL330: no pheripherals for DMAC\n");
			return -ENODEV;
		}

		pl330_info = malloc(sizeof(struct pl330_info));
		if (!pl330_info){
			printf("SHIKARA-PL330:No memory for pl330_info\n");
			return -ENOMEM;

		}
		pl330_info->dmac = dmacid;
		pl330_info->pl330_data = NULL;
		pl330_info->dmac_reset = NULL;
		pl330_info->base = SHIKHARA_DMAC2_SBASE;//phyadd  

		#ifdef CONFIG_PL330_INTERRUPT_MODE
		//enable the interrupt.
        	int_enable(DMA2_IRQ_NUM);
	        interrupt_handler_set(DMA2_IRQ_NUM,dma_interrupt);
		#endif
	}
	else if (dmacid == SHIKHARA_DMAC0){
		/*Reset DMAC */
		writel(0x3,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
		udelay(10);
		writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);

		pl330pd = &shikara_pdma;//ch_list
		/* Can't do without the list of _32_ peripherals */
		if (!pl330pd || !pl330pd->peri) {
			printf("SHIKARA-PL330: no pheripherals for DMAC\n");
			return -ENODEV;
		}

		pl330_info = malloc(sizeof(struct pl330_info));
		if (!pl330_info){
			printf("SHIKARA-PL330:No memory for pl330_info\n");
			return -ENOMEM;

		}
		pl330_info->dmac = dmacid;
		pl330_info->pl330_data = NULL;
		pl330_info->dmac_reset = NULL;
		pl330_info->base = SHIKHARA_DMAC0_NSBASE;//phyadd

		#ifdef CONFIG_PL330_INTERRUPT_MODE
		//enable the interrupt.
        	int_enable(DMA0_IRQ_NUM);
	        interrupt_handler_set(DMA0_IRQ_NUM,dma_interrupt);
		#endif
	}
	else{
		printf("Error: INVALID DMAC id : %d\t Use DMAC0 or DMAC2\n",dmacid);
		return -ENODEV;
	}

	/* Allocate a new DMAC */
	shikara_pl330_dmac = malloc(sizeof(struct shikara_pl330_dmac));
	if (!shikara_pl330_dmac) {
		printf("SHIKARA-PL330: No memory for SHIKARA-DMAC\n");
		ret = -ENOMEM;
		goto probe_err6;
	}

	ret = pl330_add(pl330_info);
	if (ret){
		printf("SHIKARA-PL330:Erro in adding DMAC to PL330\n");
		goto probe_err6;
	}
	/* Hook the info */
	shikara_pl330_dmac->pi = pl330_info;

	/* No busy channels */
	shikara_pl330_dmac->busy_chan = 0;

	/* Get the list of peripherals */
	shikara_pl330_dmac->peri = pl330pd->peri;

	/* Attach to the list of DMACs */
	list_add_tail(&shikara_pl330_dmac->node, &dmac_list);

	/* Create a channel for each peripheral in the DMAC
	 * that is, if it doesn't already exist
	 */
	for (i = 0; i < PL330_MAX_PERI; i++)
		if (shikara_pl330_dmac->peri[i] != DMACH_MAX)
			chan_add(shikara_pl330_dmac->peri[i]);

	printf("SHIKARA-PL330 \tDBUFF-%ux%ubytes Num_Chans-%u Num_Peri-%u Num_Events-%u\n",
			pl330_info->pcfg.data_buf_dep,
			pl330_info->pcfg.data_bus_width / 8, pl330_info->pcfg.num_chan,
			pl330_info->pcfg.num_peri, pl330_info->pcfg.num_events);

	return 0;

probe_err6:
	//printf("SHIKARA-PL330:exiting from pl330_init\n");
	free(shikara_pl330_dmac);
	free(pl330_info);

	return ret;
}

int pl330_remove(int dmacno)
{
	struct shikara_pl330_dmac *dmac, *d;
	struct shikara_pl330_chan *ch;
	int del, found;
	//printf("SHIKARA-PL330:release DMAC\n");

	found = 0;
	list_for_each_entry(d, &dmac_list, node)
		if (d->pi->dmac== dmacno) {
			found = 1;
			break;
		}

	if (!found) {
		return 0;
	}

	dmac = d;

	// Remove all Channels that are managed only by this DMAC 
	list_for_each_entry(ch, &chan_list, node) {

		// Only channels that are handled by this DMAC
		if (iface_of_dmac(dmac, ch->id))
			del = 1;
		else
			continue;

		// Don't remove if some other DMAC has it too 
		list_for_each_entry(d, &dmac_list, node)
			if (d != dmac && iface_of_dmac(d, ch->id)) {
				del = 0;
				break;
			}

		if (del) {
			shikara_dma_free(ch->id, ch->client);
			//printf("SHIKARA-PL330:free channel \n");
			list_del(&ch->node);
			free(ch);
		}
	}

	// Remove the DMAC 
	list_del(&dmac->node);
	free(dmac);
	return 0;
}



