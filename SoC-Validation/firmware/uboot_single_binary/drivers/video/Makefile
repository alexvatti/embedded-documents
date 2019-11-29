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

LIB	:= $(obj)libvideo.o

COBJS-$(CONFIG_PL111) += amba.o 
COBJS-$(CONFIG_LCD) += shikhara_fb.o  
COBJS-$(CONFIG_DSI_TRULY) += shikhara_mipi_dsi.o shikhara_mipi_dsi_hal.o shikhara_mipi_dsi_bridge.o  
COBJS-$(CONFIG_CSI) += shikhara_mipi_csi.o shikhara_mipi_csi_bridge.o shikhara_csi_dsi_helper.o	
COBJS-$(CONFIG_HDMI)+=  shikhara_hdmi_new.o shikhara_hdmi_bridge.o shikhara_hdmi_helper.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend  $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
