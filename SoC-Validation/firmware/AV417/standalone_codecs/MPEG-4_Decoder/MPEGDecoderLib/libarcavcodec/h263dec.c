/*
 * H.263 decoder
 * Copyright (c) 2001 Fabrice Bellard.
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 * MODIFIED BY: ARC International (UK) Limited
 * MODIFIED DATE: 31 March 2006
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
 
/**
 * @file h263dec.c
 * H.263 decoder.
 */
 
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"

#include "arc/aurora_asm.h"
#include "arc/aurora_memory.h"
#include "arc/aurora_dma.h"

#if defined __BENCHMARK__
extern void print_mpeg_skipped_frame_flag();
#endif

int ff_h263_decode_init(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;

    vlc_init();

    s->avctx = avctx;
    s->out_format = FMT_H263;

    s->width  = avctx->coded_width;
    s->height = avctx->coded_height;
    s->workaround_bugs= avctx->workaround_bugs;

    // set defaults
    MPV_decode_defaults(s);
    s->quant_precision=5;
    s->decode_mb= ff_h263_decode_mb;
    s->low_delay= 1;
    avctx->pix_fmt= PIX_FMT_YUV420P;
    s->unrestricted_mv= 1;

    /* select sub codec */
    switch(avctx->codec->id) {
    case CODEC_ID_H263:
        s->unrestricted_mv= 0;
        break;
    case CODEC_ID_MPEG4:
        s->decode_mb= ff_mpeg4_decode_mb;
        s->time_increment_bits = 4; /* default value for broken headers */
        s->h263_pred = 1;
        s->low_delay = 0; //default, might be overriden in the vol header during header parsing
        break;
    #if 0 /* not required in libarcavcodec */
    case CODEC_ID_MSMPEG4V1:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=1;
        break;
    case CODEC_ID_MSMPEG4V2:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=2;
        break;
    case CODEC_ID_MSMPEG4V3:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=3;
        break;
    case CODEC_ID_WMV1:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=4;
        break;
    case CODEC_ID_WMV2:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=5;
        break;
    case CODEC_ID_WMV3:
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->msmpeg4_version=6;
        break;
    #endif
    case CODEC_ID_H263I:
        break;
    case CODEC_ID_FLV1:
        s->h263_flv = 1;
        break;
    default:
        return -1;
    }
    s->codec_id= avctx->codec->id;

    /* for h263, we allocate the images after having read the header */
    if (avctx->codec->id != CODEC_ID_H263 && avctx->codec->id != CODEC_ID_MPEG4)
        if (MPV_common_init(s) < 0)
            return -1;

    /* Not required in libarcavcodec. */
    /*
    if (s->h263_msmpeg4)
        ff_msmpeg4_decode_init(s);
    else*/
        h263_decode_init_vlc(s);
    
    return 0;
}

int ff_h263_decode_end(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;

    MPV_common_end(s);
    return 0;
}

/**
 * retunrs the number of bytes consumed for building the current frame
 */
static int get_consumed_bytes(MpegEncContext *s, int buf_size){
    int pos= (get_bits_count(&s->gb)+7)>>3;
    
    if(s->divx_packed){
        //we would have to scan through the whole buf to handle the weird reordering ...
        return buf_size; 
    }else if(s->flags&CODEC_FLAG_TRUNCATED){
        pos -= s->parse_context.last_index;
        if(pos<0) pos=0; // padding is not really read so this might be -1
        return pos;
    }else{
        if(pos==0) pos=1; //avoid infinite loops (i doubt thats needed but ...)
        if(pos+10>buf_size) pos=buf_size; // oops ;)

        return pos;
    }
}

static int decode_slice(MpegEncContext *s){
    const int part_mask= s->partitioned_frame ? (AC_END|AC_ERROR) : 0x7F;
    const int mb_size= 16>>s->avctx->lowres;
    s->last_resync_gb= SAVE_BS_POS(s->gb);
    s->first_slice_line= 1;
        
    s->resync_mb_x= s->mb_x;
    s->resync_mb_y= s->mb_y;

    ff_set_qscale(s, s->qscale);
#if defined(MPEG4_ISO_COMPLIANCE)||defined(USE_INTRA_DC_FIX)
    s->first_mb_in_slice = 1;
    s->prev_qscale = s->qscale;
#endif        
    
    if(s->partitioned_frame){
        const int qscale= s->qscale;

        if(s->codec_id==CODEC_ID_MPEG4){
            if(ff_mpeg4_decode_partitions(s) < 0)
                return -1; 
        }
        
        /* restore variables which were modified */
        s->first_slice_line=1;
        s->mb_x= s->resync_mb_x;
        s->mb_y= s->resync_mb_y;
        ff_set_qscale(s, qscale);
#if defined(MPEG4_ISO_COMPLIANCE)||defined(USE_INTRA_DC_FIX)
    s->first_mb_in_slice = 1;
    s->prev_qscale = s->qscale;
#endif        
    }

    //invalidating and flushing cache before MB loop.
    _sr(0x1,0x47);

    for(; s->mb_y < s->mb_height; s->mb_y++)
    {
        ff_init_block_index(s);
        for(; s->mb_x < s->mb_width; s->mb_x++)
        {
            int ret;

            int block_index;
//        SET_NEW_CLEAR_SDM_BLOCK(s->block);
//        CHECKED_ALLOCZ(s->blocks, 64*12*2 * sizeof(DCTELEM));
            
            s->blocks = (DCTELEM(*)[6][64])GetDctBuffer();
            s->block= s->blocks[0];
            for(block_index=0; block_index<12; block_index++)
            {  /* note that this assumes 12 arrays though we have only allocated 6!!*/
                s->pblocks[block_index] = (short *)(&s->block[block_index]);
            }

            ff_update_block_index(s);

            if(s->resync_mb_x == s->mb_x && s->resync_mb_y+1 == s->mb_y ){
                s->first_slice_line=0; 
            }

            /* DCT & quantize */

            s->mv_dir = MV_DIR_FORWARD;
            s->mv_type = MV_TYPE_16X16;
//            s->mb_skiped = 0;
//printf("%d %d %06X\n", ret, get_bits_count(&s->gb), show_bits(&s->gb, 24));
            ret= s->decode_mb(s, s->block);

            if (s->pict_type!=B_TYPE)
                ff_h263_update_motion_val(s);

            if(ret<0){
                const int xy= s->mb_x + s->mb_y*s->mb_stride;
                if(ret==SLICE_END){
                    MPV_decode_mb(s, s->block);
                    
                    // Free DCT buffer
                    FreeDctBuffer();
                    
                    if(s->loop_filter)
                    {
                      int temp = waitforDCTbufferFree();
                      
                      if( temp > 0)
                      {
                        ff_h263_loop_filter(s);
                      }
                    }

//printf("%d %d %d %06X\n", s->mb_x, s->mb_y, s->gb.size*8 - get_bits_count(&s->gb), show_bits(&s->gb, 24));
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_END|DC_END|MV_END)&part_mask);

                    s->padding_bug_score--;
                        
                    if(++s->mb_x >= s->mb_width){
                        s->mb_x=0;
                        ff_draw_horiz_band(s, s->mb_y*mb_size, mb_size);
                        s->mb_y++;
                    }
                    return 0; 
                }else if(ret==SLICE_NOEND){
                    av_log(s->avctx, AV_LOG_ERROR, "Slice mismatch at MB: %d\n", xy);
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x+1, s->mb_y, (AC_END|DC_END|MV_END)&part_mask);
                    return -1;
                }
                av_log(s->avctx, AV_LOG_ERROR, "Error at MB: %d\n", xy);
                ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR|DC_ERROR|MV_ERROR)&part_mask);
    
                return -1;
            }

            MPV_decode_mb(s, s->block);
            
            // Free DCT buffer
            FreeDctBuffer();
             
            if(s->loop_filter)
            {
               
                int temp = waitforDCTbufferFree();
                      
               if( temp > 0)
               {
                   ff_h263_loop_filter(s);
               }
            }

            // Free DCT buffer
            //FreeDctBuffer();
        }
        
        ff_draw_horiz_band(s, s->mb_y*mb_size, mb_size);
        
        s->mb_x= 0;
    }
    
    assert(s->mb_x==0 && s->mb_y==s->mb_height);

    /* try to detect the padding bug */
    if(      s->codec_id==CODEC_ID_MPEG4
       &&   (s->workaround_bugs&FF_BUG_AUTODETECT) 
       &&    s->gb.size_in_bits - get_bits_count(&s->gb) >=0
       &&    s->gb.size_in_bits - get_bits_count(&s->gb) < 48
//       &&   !s->resync_marker
       &&   !s->data_partitioning){
        
        const int bits_count= get_bits_count(&s->gb);
        const int bits_left = s->gb.size_in_bits - bits_count;
        
        if(bits_left==0){
            s->padding_bug_score+=16;
        } else if(bits_left != 1){
            int v= show_bits(&s->gb, 8);
            v|= 0x7F >> (7-(bits_count&7));

            if(v==0x7F && bits_left<=8)
                s->padding_bug_score--;
            else if(v==0x7F && ((get_bits_count(&s->gb)+8)&8) && bits_left<=16)
                s->padding_bug_score+= 4;
            else
                s->padding_bug_score++;            
        }                          
    }
    
    if(s->workaround_bugs&FF_BUG_AUTODETECT){
        if(s->padding_bug_score > -2 && !s->data_partitioning /*&& (s->divx_version || !s->resync_marker)*/)
            s->workaround_bugs |=  FF_BUG_NO_PADDING;
        else
            s->workaround_bugs &= ~FF_BUG_NO_PADDING;
    }

    // handle formats which dont have unique end markers
    if(s->msmpeg4_version || (s->workaround_bugs&FF_BUG_NO_PADDING)){ //FIXME perhaps solve this more cleanly
        int left= s->gb.size_in_bits - get_bits_count(&s->gb);
        int max_extra=7;
        
        /* no markers in M$ crap */
        if(s->msmpeg4_version && s->pict_type==I_TYPE)
            max_extra+= 17;
        
        /* buggy padding but the frame should still end approximately at the bitstream end */
        if((s->workaround_bugs&FF_BUG_NO_PADDING) && s->error_resilience>=3)
            max_extra+= 48;
        else if((s->workaround_bugs&FF_BUG_NO_PADDING))
            max_extra+= 256*256*256*64;
        
        if(left>max_extra){
            av_log(s->avctx, AV_LOG_ERROR, "discarding %d junk bits at end, next would be %X\n", left, show_bits(&s->gb, 24));
        }
        else if(left<0){
            av_log(s->avctx, AV_LOG_ERROR, "overreading %d bits\n", -left);
        }else
            ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x-1, s->mb_y, AC_END|DC_END|MV_END);
        
        return 0;
    }

    av_log(s->avctx, AV_LOG_ERROR, "slice end not reached but screenspace end (%d left %06X, score= %d)\n", 
            s->gb.size_in_bits - get_bits_count(&s->gb),
            show_bits(&s->gb, 24), s->padding_bug_score);
            
    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_END|DC_END|MV_END)&part_mask);

    return -1;
}

/**
 * finds the end of the current frame in the bitstream.
 * @return the position of the first byte of the next frame, or -1
 */
int ff_mpeg4_find_frame_end(ParseContext *pc, const uint8_t *buf, int buf_size){
    int vop_found, i;
    uint32_t state;
    
    vop_found= pc->frame_start_found;
    state= pc->state;
    
    i=0;
    if(!vop_found){
        for(i=0; i<buf_size; i++){
            state= (state<<8) | buf[i];
            if(state == 0x1B6){
                i++;
                vop_found=1;
                break;
            }
        }
    }

    if(vop_found){
        /* EOF considered as end of frame */
        if (buf_size == 0)
            return 0;
        for(; i<buf_size; i++){
            state= (state<<8) | buf[i];
            if((state&0xFFFFFF00) == 0x100){
                pc->frame_start_found=0;
                pc->state=-1; 
                return i-3;
            }
        }
    }
    pc->frame_start_found= vop_found;
    pc->state= state;
    return END_NOT_FOUND;
}

static int h263_find_frame_end(ParseContext *pc, const uint8_t *buf, int buf_size){
    int vop_found, i;
    uint32_t state;
    
    vop_found= pc->frame_start_found;
    state= pc->state;
    
    i=0;
    if(!vop_found){
        for(i=0; i<buf_size; i++){
            state= (state<<8) | buf[i];
            if(state>>(32-22) == 0x20){
                i++;
                vop_found=1;
                break;
            }
        }
    }

    if(vop_found){    
      for(; i<buf_size; i++){
        state= (state<<8) | buf[i];
        if(state>>(32-22) == 0x20){
            pc->frame_start_found=0;
            pc->state=-1; 
            return i-3;
        }
      }
    }
    pc->frame_start_found= vop_found;
    pc->state= state;
    
    return END_NOT_FOUND;
}

static int h263_parse(AVCodecParserContext *s,
                           AVCodecContext *avctx,
                           uint8_t **poutbuf, int *poutbuf_size, 
                           const uint8_t *buf, int buf_size)
{
    ParseContext *pc = s->priv_data;
    int next;
    
    next= h263_find_frame_end(pc, buf, buf_size);

    if (ff_combine_frame(pc, next, (uint8_t **)&buf, &buf_size) < 0) {
        *poutbuf = NULL;
        *poutbuf_size = 0;
        return buf_size;
    }

    *poutbuf = (uint8_t *)buf;
    *poutbuf_size = buf_size;
    return next;
}

int ff_h263_decode_frame(AVCodecContext *avctx, 
                             void *data, int *data_size,
                             uint8_t *buf, int buf_size)
{
    MpegEncContext *s = avctx->priv_data;
    int ret;
    AVFrame *pict = data; 
    
#ifdef PRINT_FRAME_TIME
uint64_t time= rdtsc();
#endif
#ifdef DEBUG
   // printf("*****frame %d size=%d\n", avctx->frame_number, buf_size);
  //  printf("bytes=%x %x %x %x\n", buf[0], buf[1], buf[2], buf[3]);
#endif
    s->flags= avctx->flags;
    s->flags2= avctx->flags2;

    /* no supplementary picture */
    if (buf_size == 0) {
        /* special case for last picture */
        if (s->low_delay==0 && s->next_picture_ptr) {
            *pict= *(AVFrame*)s->next_picture_ptr;
            s->next_picture_ptr= NULL;

            *data_size = sizeof(AVFrame);
        }

        return 0;
    }

    if(s->flags&CODEC_FLAG_TRUNCATED){
        int next;
        
        if(s->codec_id==CODEC_ID_MPEG4){
            next= ff_mpeg4_find_frame_end(&s->parse_context, buf, buf_size);
        }else if(s->codec_id==CODEC_ID_H263){
            next= h263_find_frame_end(&s->parse_context, buf, buf_size);
        }else{
            av_log(s->avctx, AV_LOG_ERROR, "this codec doesnt support truncated bitstreams\n");
            return -1;
        }
        
        if( ff_combine_frame(&s->parse_context, next, &buf, &buf_size) < 0 )
            return buf_size;
    }

    
retry:
    
    if(s->bitstream_buffer_size && (s->divx_packed || buf_size<20)){ //divx 5.01+/xvid frame reorder
        init_get_bits(&s->gb, s->bitstream_buffer, s->bitstream_buffer_size*8);
    }else
        init_get_bits(&s->gb, buf, buf_size*8);
    s->bitstream_buffer_size=0;

    if (!s->context_initialized) {
        if (MPV_common_init(s) < 0) //we need the idct permutaton for reading a custom matrix
            return -1;
    }
    
    //we need to set current_picture_ptr before reading the header, otherwise we cant store anyting im there
    if(s->current_picture_ptr==NULL || s->current_picture_ptr->data[0]){
        int i= ff_find_unused_picture(s, 0);
        s->current_picture_ptr= &s->picture[i];
    }
      
    /* let's go :-) */
    /* Not required in libarcavcodec. */
    /*if (s->msmpeg4_version==5) {
        ret= ff_wmv2_decode_picture_header(s);
    } else if (s->msmpeg4_version) {
        ret = msmpeg4_decode_picture_header(s);
    } else*/ if (s->h263_pred) {
        if(s->avctx->extradata_size && s->picture_number==0){
            GetBitContext gb;
            
            init_get_bits(&gb, s->avctx->extradata, s->avctx->extradata_size*8);
            ret = ff_mpeg4_decode_picture_header(s, &gb);
        }
        ret = ff_mpeg4_decode_picture_header(s, &s->gb);

        if(s->flags& CODEC_FLAG_LOW_DELAY)
            s->low_delay=1;
    } else if (s->codec_id == CODEC_ID_H263I) {
        ret = intel_h263_decode_picture_header(s);
    } else if (s->h263_flv) {
        ret = flv_h263_decode_picture_header(s);
    } else {
        ret = h263_decode_picture_header(s);
    }
    
    if(ret==FRAME_SKIPED)
    {
#if defined(MPEG4_ISO_COMPLIANCE)
        if(s->current_picture.data[0]!=NULL)
        {
            *pict= *(AVFrame*)&s->current_picture;
            *data_size = sizeof(AVFrame);   //set got_picture
        }
#endif
#if defined __BENCHMARK__
	print_mpeg_skipped_frame_flag();    
#endif
        return get_consumed_bytes(s, buf_size);
    }

    /* skip if the header was thrashed */
    if (ret < 0){
        av_log(s->avctx, AV_LOG_ERROR, "header damaged\n");
        return -1;
    }
    
    avctx->has_b_frames= !s->low_delay;
    
    if(s->xvid_build==0 && s->divx_version==0 && s->lavc_build==0){
        if(s->avctx->stream_codec_tag == ff_get_fourcc("XVID") || 
           s->avctx->codec_tag == ff_get_fourcc("XVID") || s->avctx->codec_tag == ff_get_fourcc("XVIX"))
            s->xvid_build= -1;
    }

    if(s->xvid_build==0 && s->divx_version==0 && s->lavc_build==0){
        if(s->avctx->codec_tag == ff_get_fourcc("DIVX") && s->vo_type==0 && s->vol_control_parameters==0)
            s->divx_version= 400; //divx 4
    }
    
    if(s->xvid_build && s->divx_version){
        s->divx_version=
        s->divx_build= 0;
    }

    if(s->workaround_bugs&FF_BUG_AUTODETECT){
        if(s->avctx->codec_tag == ff_get_fourcc("XVIX")) 
            s->workaround_bugs|= FF_BUG_XVID_ILACE;

        if(s->avctx->codec_tag == ff_get_fourcc("UMP4")){
            s->workaround_bugs|= FF_BUG_UMP4;
        }

        if(s->divx_version>=500){
            s->workaround_bugs|= FF_BUG_QPEL_CHROMA;
        }

        if(s->divx_version>502){
            s->workaround_bugs|= FF_BUG_QPEL_CHROMA2;
        }

        if(s->xvid_build && s->xvid_build<=3)
            s->padding_bug_score= 256*256*256*64;
        
        if(s->xvid_build && s->xvid_build<=1)
            s->workaround_bugs|= FF_BUG_QPEL_CHROMA;

        if(s->xvid_build && s->xvid_build<=12)
            s->workaround_bugs|= FF_BUG_EDGE;

        if(s->xvid_build && s->xvid_build<=32)
            s->workaround_bugs|= FF_BUG_DC_CLIP;

#define SET_QPEL_FUNC(postfix1, postfix2) \
    s->dsp.put_ ## postfix1 = ff_put_ ## postfix2;\
    s->dsp.put_no_rnd_ ## postfix1 = ff_put_no_rnd_ ## postfix2;\
    s->dsp.avg_ ## postfix1 = ff_avg_ ## postfix2;

        if(s->lavc_build && s->lavc_build<4653)
            s->workaround_bugs|= FF_BUG_STD_QPEL;
        
        if(s->lavc_build && s->lavc_build<4655)
            s->workaround_bugs|= FF_BUG_DIRECT_BLOCKSIZE;

        if(s->lavc_build && s->lavc_build<4670){
            s->workaround_bugs|= FF_BUG_EDGE;
        }
        
        if(s->lavc_build && s->lavc_build<=4712)
            s->workaround_bugs|= FF_BUG_DC_CLIP;

        if(s->divx_version)
            s->workaround_bugs|= FF_BUG_DIRECT_BLOCKSIZE;
//printf("padding_bug_score: %d\n", s->padding_bug_score);
        if(s->divx_version==501 && s->divx_build==20020416)
            s->padding_bug_score= 256*256*256*64;

        if(s->divx_version && s->divx_version<500){
            s->workaround_bugs|= FF_BUG_EDGE;
        }
        
        if(s->divx_version)
            s->workaround_bugs|= FF_BUG_HPEL_CHROMA;

        // do not detect stuffing bits for DIVX
        if((s->xvid_build!=0)||(s->divx_build!=0))
             s->workaround_bugs|= FF_BUG_STUFFING_BITS;
    }
    
    if(s->workaround_bugs& FF_BUG_STD_QPEL){
        SET_QPEL_FUNC(qpel_pixels_tab[0][ 5], qpel16_mc11_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[0][ 7], qpel16_mc31_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[0][ 9], qpel16_mc12_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[0][11], qpel16_mc32_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[0][13], qpel16_mc13_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[0][15], qpel16_mc33_old_c)

        SET_QPEL_FUNC(qpel_pixels_tab[1][ 5], qpel8_mc11_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[1][ 7], qpel8_mc31_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[1][ 9], qpel8_mc12_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[1][11], qpel8_mc32_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[1][13], qpel8_mc13_old_c)
        SET_QPEL_FUNC(qpel_pixels_tab[1][15], qpel8_mc33_old_c)
    }

    if(avctx->debug & FF_DEBUG_BUGS)
        av_log(s->avctx, AV_LOG_DEBUG, "bugs: %X lavc_build:%d xvid_build:%d divx_version:%d divx_build:%d %s\n", 
               s->workaround_bugs, s->lavc_build, s->xvid_build, s->divx_version, s->divx_build,
               s->divx_packed ? "p" : "");
    

#ifdef HAVE_MMX
    if(s->codec_id == CODEC_ID_MPEG4 && s->xvid_build && avctx->idct_algo == FF_IDCT_AUTO && (mm_flags & MM_MMX) && !(s->flags&CODEC_FLAG_BITEXACT)){
        avctx->idct_algo= FF_IDCT_LIBMPEG2MMX;
        avctx->coded_width= 0; // force reinit
    }
#endif

        /* After H263 & mpeg4 header decode we have the height, width,*/
        /* and other parameters. So then we could init the picture   */
        /* FIXME: By the way H263 decoder is evolving it should have */
        /* an H263EncContext                                         */
    
    if (   s->width  != avctx->coded_width 
        || s->height != avctx->coded_height) {
        /* H.263 could change picture size any time */
        ParseContext pc= s->parse_context; //FIXME move these demuxng hack to avformat
        s->parse_context.buffer=0;
        MPV_common_end(s);
        s->parse_context= pc;
    }
    if (!s->context_initialized) {
        avcodec_set_dimensions(avctx, s->width, s->height);

        goto retry;
    }

    if((s->codec_id==CODEC_ID_H263 || s->codec_id==CODEC_ID_H263P))
        s->gob_index = ff_h263_get_gob_height(s);
    
    // for hurry_up==5
    s->current_picture.pict_type= s->pict_type;
    s->current_picture.key_frame= s->pict_type == I_TYPE;

    /* skip b frames if we dont have reference frames */
    if(s->last_picture_ptr==NULL && (s->pict_type==B_TYPE || s->dropable)) return get_consumed_bytes(s, buf_size);
    /* skip b frames if we are in a hurry */
    if(avctx->hurry_up && s->pict_type==B_TYPE) return get_consumed_bytes(s, buf_size);
    /* skip everything if we are in a hurry>=5 */
    if(avctx->hurry_up>=5) return get_consumed_bytes(s, buf_size);
    
    if(s->next_p_frame_damaged){
        if(s->pict_type==B_TYPE)
            return get_consumed_bytes(s, buf_size);
        else
            s->next_p_frame_damaged=0;
    }

    if(MPV_frame_start(s, avctx) < 0)
        return -1;

#ifdef DEBUG
  //  printf("qscale=%d\n", s->qscale);
#endif

    ff_er_frame_start(s);
    
    /* decode each macroblock */
    s->mb_x=0; 
    s->mb_y=0;
    
    decode_slice(s);

    // free the last buffer if it wasn't done
    FreeDctBuffer();

    while(s->mb_y<s->mb_height)
    {
        if(ff_h263_resync(s)<0)
            break;
        
        if(s->msmpeg4_version<4 && s->h263_pred)
            ff_mpeg4_clean_buffers(s);

        decode_slice(s);

        // free the last buffer if it wasn't done
        FreeDctBuffer();
    }

   
    /* divx 5.01+ bistream reorder stuff */
    if(s->codec_id==CODEC_ID_MPEG4 && s->bitstream_buffer_size==0 && s->divx_packed){
        int current_pos= get_bits_count(&s->gb)>>3;
        int startcode_found=0;
        
        if(buf_size - current_pos > 5){
            int i;
            for(i=current_pos; i<buf_size-3; i++){
                if(buf[i]==0 && buf[i+1]==0 && buf[i+2]==1 && buf[i+3]==0xB6){
                    startcode_found=1;
                    break;
                }
            }
        }
        if(s->gb.buffer == s->bitstream_buffer && buf_size>20){ //xvid style
            startcode_found=1;
            current_pos=0;
        }

        if(startcode_found){
#if !defined(ARC_BITSTREAM_DMA)
            s->bitstream_buffer= av_fast_realloc(
                s->bitstream_buffer, 
                (unsigned int *)&(s->allocated_bitstream_buffer_size), 
                buf_size - current_pos + FF_INPUT_BUFFER_PADDING_SIZE);
#else
	    /* We have to keep the bitstream data in the uncached area above baseVirtualAddress
	       otherwise DMA assist will not be able to reach it */
            if (buf_size - current_pos + FF_INPUT_BUFFER_PADDING_SIZE > 0x10000)
            {
                av_log(s->avctx, AV_LOG_ERROR, "Attempt to allocate too much uncached memory!\n"); 
                return -1;
            }
            extern char *baseVirtualAddress;
            s->bitstream_buffer= baseVirtualAddress + 0xF0000;
#endif
            memcpy(s->bitstream_buffer, buf + current_pos, buf_size - current_pos);
            s->bitstream_buffer_size= buf_size - current_pos;
        }
    }

    ff_er_frame_end(s);

    MPV_frame_end(s);

    assert(s->current_picture.pict_type == s->current_picture_ptr->pict_type);
    assert(s->current_picture.pict_type == s->pict_type);

    if(s->pict_type==B_TYPE || s->low_delay){
        *pict= *(AVFrame*)&s->current_picture;
        ff_print_debug_info(s, pict);
    } else {
        *pict= *(AVFrame*)&s->last_picture;
        if(pict)
            ff_print_debug_info(s, pict);
    }

    /* Return the Picture timestamp as the frame number */
    /* we substract 1 because it is added on utils.c    */
    avctx->frame_number = s->picture_number - 1;

    /* dont output the last pic after seeking */
    if(s->last_picture_ptr || s->low_delay)
        *data_size = sizeof(AVFrame);
#ifdef PRINT_FRAME_TIME
printf("%Ld\n", rdtsc()-time);
#endif

    return get_consumed_bytes(s, buf_size);
}

AVCodec mpeg4_decoder = {
    "mpeg4",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MPEG4,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1 | CODEC_CAP_TRUNCATED | CODEC_CAP_DELAY,
    NULL,
    NULL,
    ff_mpeg_flush
};

AVCodec h263_decoder = {
    "h263",
    CODEC_TYPE_VIDEO,
    CODEC_ID_H263,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1 | CODEC_CAP_TRUNCATED | CODEC_CAP_DELAY,
    NULL,
    NULL,
    ff_mpeg_flush
};

AVCodec msmpeg4v1_decoder = {
    "msmpeg4v1",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MSMPEG4V1,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};

AVCodec msmpeg4v2_decoder = {
    "msmpeg4v2",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MSMPEG4V2,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};

AVCodec msmpeg4v3_decoder = {
    "msmpeg4",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MSMPEG4V3,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};

AVCodec wmv1_decoder = {
    "wmv1",
    CODEC_TYPE_VIDEO,
    CODEC_ID_WMV1,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};

AVCodec h263i_decoder = {
    "h263i",
    CODEC_TYPE_VIDEO,
    CODEC_ID_H263I,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};

AVCodec flv_decoder = {
    "flv",
    CODEC_TYPE_VIDEO,
    CODEC_ID_FLV1,
    sizeof(MpegEncContext),
    ff_h263_decode_init,
    NULL,
    ff_h263_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1
};

AVCodecParser h263_parser = {
    { CODEC_ID_H263 },
    sizeof(ParseContext),
    NULL,
    h263_parse,
    ff_parse_close,
};
