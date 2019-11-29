#ifndef _VBUF_H_
#define _VBUF_H_

#ifdef	__cplusplus
extern "C" {
#endif

/* allocate a buffer for video. Block if 
   no buffer is yet available */
int vbuf_alloc(void);

/* free buffer */
void vbuf_free(int v);

/* send buffer to display queue */
void vbuf_send(int v);

/* return a bitmask of available buffers */
unsigned vbuf_avail(void);

#ifdef	__cplusplus
};
#endif

#endif
