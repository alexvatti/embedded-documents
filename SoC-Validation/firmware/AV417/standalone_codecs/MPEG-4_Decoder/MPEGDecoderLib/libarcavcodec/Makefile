#
# libarcavcodec Makefile (based on libavcodec Makefile)
# (c) 2000-2005 Fabrice Bellard
#
include config.mak

VPATH=$(SRC_PATH)/libarcavcodec
DESTPATH=.

# NOTE: -I.. is needed to include config.h
ISO_FLAGS = -DMPEG4_ISO_COMPLIANCE -DMPEG2_ISO_COMPLIANCE -DSIMD_MPEG4_DEQUANT
CFLAGS=$(ISO_FLAGS) $(OPTFLAGS) $(ASM_CFLAGS) -Hnocrt -DHAVE_AV_CONFIG_H -DLIBARCAVCODEC_INTERNAL_COMPILE -I.. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -Dno_MEMALIGN_HACK
ASM_CFLAGS+= $(ISO_FLAGS) -Hhostlib= -Hnocrt -DHAVE_AV_CONFIG_H -DLIBARCAVCODEC_INTERNAL_COMPILE -I.. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE

OBJS= bitstream.o dsputil.o h263.o h263dec.o mpeg12.o      \
      mpegvideo.o rational.o simple_idct.o mem.o utils.o   \
      imgconvert.o integer.o parser.o allcodecs.o          \
      eval.o resample2.o error_resilience.o mjpeg.o


ifeq ($(FFMPEG_STANDALONE),yes)
OBJS+= raw.o
endif

ifeq ($(HAVE_PTHREADS),yes)
OBJS+= pthread.o
endif
ASM_OBJS=

ifeq ($(TARGET_ARCH_ARC),yes)
OBJS+= arc/dsputil_arc.o arc/subpel_arc.o arc/aurora_memory.o \
	arc/dsputil_qpel.o arc/aurora_qpel.o arc/aurora_subpel.o arc/aurora_macro.o
ifeq ($(TARGET_ARCH_X86),yes)
OBJS+= arc/aurora_model.o arc/cmodel_tables.o
endif
endif

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.c)
OBJS := $(OBJS) $(ASM_OBJS)

LIB= $(DESTPATH)/$(LIBPREF)arcavcodec$(LIBSUF)
ifeq ($(BUILD_SHARED),yes)
SLIB= $(DESTPATH)/$(SLIBPREF)arcavcodec$(SLIBSUF)
endif

all: $(LIB)

#	rm -f $@
$(LIB): $(OBJS) $(AMRLIBS)
	$(AR) rc $@ $(OBJS) $(AMREXTRALIBS)
	$(RANLIB) $@

$(SLIB): $(OBJS)
ifeq ($(CONFIG_PP),yes)
	$(MAKE) -C libpostproc
endif
ifeq ($(CONFIG_WIN32),yes)
	$(CC) $(SHFLAGS) -Wl,--output-def,$(@:.dll=.def) -o $@ $(OBJS) $(EXTRALIBS) $(AMREXTRALIBS)
	-lib /machine:i386 /def:$(@:.dll=.def)
else
	$(CC) $(SHFLAGS) -o $@ $(OBJS) $(EXTRALIBS) $(AMREXTRALIBS) $(LDFLAGS)
endif

dsputil.o: dsputil.c dsputil.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

arc/dsputil_arc.o: arc/aurora_asm.h arc/dsputil_arc.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/dsputil_arc.o arc/dsputil_arc.c
arc/aurora_memory.o: arc/aurora_memory.h arc/aurora_memory.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/aurora_memory.o arc/aurora_memory.c
arc/dsputil_qpel.o: dsputil.h arc/dsputil_qpel.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/dsputil_qpel.o arc/dsputil_qpel.c
arc/aurora_qpel.o: dsputil.h arc/aurora_qpel.h arc/aurora_qpel.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/aurora_qpel.o arc/aurora_qpel.c
arc/aurora_macro.o:dsputil.h arc/aurora_qpel.h arc/aurora_macro.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/aurora_macro.o arc/aurora_macro.c
arc/aurora_subpel.o:dsputil.h arc/aurora_subpel.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o arc/aurora_subpel.o arc/aurora_subpel.c

mpegvideo.o: arc/aurora_asm.h  mpegvideo.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o mpegvideo.o mpegvideo.c

mpeg12.o: arc/aurora_asm.h mpeg12.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o mpeg12.o mpeg12.c
h263dec.o: arc/aurora_asm.h h263dec.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o h263dec.o h263dec.c
h263.o: arc/aurora_asm.h  h263.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o $@ h263.c
bitstream.o: arc/aurora_asm.h bitstream.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o $@ bitstream.c
parser.o: arc/aurora_asm.h parser.c
	$(ASM_CC) $(ASM_CFLAGS) -c -o $@ parser.c

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean: $(CLEANAMR)
	rm -f *.o *.d *.s *~ .depend $(LIB) $(SLIB) *.so \
       arc/*.o \
	   apiexample $(TESTS)

distclean: clean
	rm -f Makefile.bak .depend


#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
