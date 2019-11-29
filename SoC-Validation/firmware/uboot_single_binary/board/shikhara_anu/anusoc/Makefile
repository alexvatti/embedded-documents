#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
ifneq ($(OBJTREE),$(SRCTREE))
$(shell mkdir -p $(obj)../common)
endif

LIB	= $(obj)lib$(BOARD).o

ifndef CONFIG_BL0_BUILD
ifdef  CONFIG_SHIKHARA_ASIC
ifdef  CONFIG_LPDDR2_INIT
SOBJS   := ddr_init.o
endif
endif
endif

COBJS-y	:= shikhara_board.o 
COBJS-y	+= shikhara_gic.o
ifdef CONFIG_ZYNQ_PRIVATETIMER
COBJS-y += zynq_timer.o
else
COBJS-y += shikhara_timer.o
endif
ifndef CONFIG_BL1_BUILD
SOBJS   += shikhara_pmu_a9.o
COBJS-y += shikhara_pmu.o
endif 
#ifdef CONFIG_SHIKHARA_HELPER
COBJS-y += shikhara_helper.o
COBJS-y += shikhara_pmu_sysctrl.o
#endif

COBJS	:= $(sort $(COBJS-y))

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(call cmd_link_o_target, $(OBJS) $(SOBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
