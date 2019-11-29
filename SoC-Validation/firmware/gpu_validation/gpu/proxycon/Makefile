#
# (C) Copyright 2000-2007
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libgpu_proxycon.o

COBJS-$(CONFIG_MALIGPU_PROXYCON)+=proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_a.o  proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_d.o  proxycon_etc_fsaa4_1080p_Average_f75_mc4_mem_0.o proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_b.o  proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_e.o  proxycon_etc_fsaa4_1080p_Average_f75_mc4_run.o proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_c.o  proxycon_etc_fsaa4_1080p_Average_f75_mc4_check_0_f.o


COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
