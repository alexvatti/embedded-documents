#
# (C) Copyright 2000-2011
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2011
# Daniel Schwierzeck, daniel.schwierzeck@googlemail.com.
#
# (C) Copyright 2011
# Texas Instruments Incorporated - http://www.ti.com/
# Aneesh V <aneesh@ti.com>
#
# This file is released under the terms of GPL v2 and any later version.
# See the file COPYING in the root directory of the source tree for details.
#
# Based on top-level Makefile.
#

CONFIG_BL0_BUILD := y
CONFIG_BL1_BUILD += y

export CONFIG_BL0_BUILD
export CONFIG_BL1_BUILD

include $(TOPDIR)/config.mk

# We want the final binaries in this directory
obj := $(OBJTREE)/bl0_spl/

ifdef  CONFIG_SPL_START_S_PATH
START_PATH := $(subst ",,$(CONFIG_SPL_START_S_PATH))
else
START_PATH := $(CPUDIR)
endif

START := $(START_PATH)/start.o

LIBS-y = $(CPUDIR)/lib$(CPU).o
LIBS-y += arch/arm/lib/libarm.o
LIBS-y += board/$(BOARDDIR)/lib$(BOARD).o
LIBS-y += drivers/mmc/libmmc.o
LIBS-y += drivers/mtd/nand/libnand.o
LIBS-y += drivers/mtd/libmtd.o
LIBS-y += drivers/mtd/spi/libspi_flash.o
LIBS-y += drivers/spi/libspi.o
LIBS-y += drivers/serial/libserial.o
LIBS-y += drivers/gpio/libgpio.o
LIBS-y += common/libcommon.o
LIBS-y += disk/libdisk.o
LIBS-y += lib/libgeneric.o
# Add GCC lib
ifeq ("$(USE_PRIVATE_LIBGCC)", "yes")
PLATFORM_LIBGCC = $(SPL0TREE)/arch/$(ARCH)/lib/libgcc.o
PLATFORM_LIBS := $(filter-out %/libgcc.o, $(filter-out -lgcc, $(PLATFORM_LIBS))) $(PLATFORM_LIBGCC)
endif

START := $(addprefix $(SPL0TREE)/,$(START))
OBJS := $(addprefix $(SPL0TREE)/,$(sort $(OBJS-y)))
LIBS := $(addprefix $(SPL0TREE)/,$(sort $(LIBS-y)))

__START := $(subst $(obj),,$(START))
__OBJS := $(subst $(obj),,$(OBJS))
__LIBS := $(subst $(obj),,$(LIBS))

# Linker Script
ifdef CONFIG_SPL_LDSCRIPT
# need to strip off double quotes
LDSCRIPT := $(addprefix $(SRCTREE)/,$(subst ",,$(CONFIG_SPL_LDSCRIPT)))
endif

ifeq ($(wildcard $(LDSCRIPT)),)
LDSCRIPT := $(TOPDIR)/board/$(BOARDDIR)/u-boot-bl0.lds
endif
ifeq ($(wildcard $(LDSCRIPT)),)
	LDSCRIPT := $(TOPDIR)/$(CPUDIR)/u-boot-spl.lds
endif
ifeq ($(wildcard $(LDSCRIPT)),)
	LDSCRIPT := $(TOPDIR)/arch/$(ARCH)/cpu/u-boot-spl.lds
endif
ifeq ($(wildcard $(LDSCRIPT)),)
$(error could not find linker script)
endif

# Special flags for CPP when processing the linker script.
# Pass the version down so we can handle backwards compatibility
# on the fly.
LDPPFLAGS += \
	-include $(SRCTREE)/include/u-boot/u-boot.lds.h  \
	-DCPUDIR=$(CPUDIR) \
	$(shell $(LD) --version | \
	  sed -ne 's/GNU ld version \([0-9][0-9]*\)\.\([0-9][0-9]*\).*/-DLD_MAJOR=\1 -DLD_MINOR=\2/p')

ALL-y	+= $(obj)bl0.bin  $(obj)bl0.hex

all:	$(ALL-y) 

$(obj)bl0.hex:	$(obj)bl0
		$(OBJCOPY) $(OBJCFLAGS) -O ihex $< $@


$(obj)bl0.bin:	$(obj)bl0
		$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@

GEN_UBOOT = \
		cd $(obj) && $(LD) $(LDFLAGS) $(LDFLAGS_u-boot-bl0) $(__START) \
		--start-group $(__LIBS) --end-group $(PLATFORM_LIBS) \
		-Map bl0.map -o bl0


$(obj)bl0:	depend $(START) $(LIBS) $(obj)u-boot-bl0.lds
		$(GEN_UBOOT)

$(START):	depend
		$(MAKE) -C $(SRCTREE)/$(START_PATH) $@

$(LIBS):        depend
		$(MAKE) -C $(SRCTREE)$(dir $(subst $(SPL0TREE),,$@))

$(obj)u-boot-bl0.lds: $(LDSCRIPT) 
		 $(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - < $< > $@


depend:	$(obj).depend
.PHONY: depend

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

