

LANG = C
LANGUAGES = en
TARGET_OS = elf32
DESTDIR = .
BINDIR = $(DESTDIR)
LIBDIR = $(DESTDIR)/lib



TARGET_ARCH_ARC = yes

AR = arac
CC = hcac
ASM_CC = hcac
LD = hcac
AWK =
RANLIB = arac -s
INSTALL = install

HCAC_DEP_FLAGS=-Hsilent -Hrel -Hmake

EXTRA_INC =
ARC_FLAGS = -D__ARC__ -DARCH_ARC -D__signed__=signed -DCONFIG_ALIGN -D__MPEG_HCAC__ -DHAVE_AURORA ${OPT_FLAGS}
OPTFLAGS =-O6 -DNDEBUG  -arc700  -Xsimd -Xbarrel_shifter -Xlib -Xea -Xmin_max -Xnorm -Xswap -Xmpy -w0 -Dinline=_Inline -Hsdata0 
STRIPBINARIES = yes
ASM_CFLAGS = $(OPTFLAGS) $(ARC_FLAGS) -I. $(EXTRA_INC) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEFINE_INT64_T
LDFLAGS = -Hhostlib=

# for libavcodec:
LIBPREF=lib
LIBSUF=.a
SLIBPREF=lib
SLIBSUF=.so

ifdef ENABLE_LINUX

ifneq (${ENABLE_LINUX},true)
ASM_CFLAGS += -D__ARC_STANDALONE_
FFMPEG_STANDALONE = yes
else
FFMPEG_STANDALONE = no
endif

else

#assume this is not being built for Linux integration
ASM_CFLAGS += -D__ARC_STANDALONE_
FFMPEG_STANDALONE = yes
endif
