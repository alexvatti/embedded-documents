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

LIB	= $(obj)libhdmi.o

COBJS-$(CONFIG_HDMI) += ./util/log.o
COBJS-$(CONFIG_HDMI) += ./util/error.o
COBJS-$(CONFIG_HDMI) += ./util/bitOperation.o
COBJS-$(CONFIG_HDMI) += ./core/audioParams.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerSpd.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioGenerator.o
COBJS-$(CONFIG_HDMI) += ./core/packets.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioClock.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioDma.o
COBJS-$(CONFIG_HDMI) += ./core/videoParams.o
COBJS-$(CONFIG_HDMI) += ./core/halVideoPacketizer.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerAudioInfo.o
COBJS-$(CONFIG_HDMI) += ./core/halVideoGenerator.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerAcp.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerIsrc.o
COBJS-$(CONFIG_HDMI) += ./core/halIdentification.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerDebug.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioI2s.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerAvi.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerGamut.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioHbr.o
COBJS-$(CONFIG_HDMI) += ./core/halMainController.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioSpdif.o
COBJS-$(CONFIG_HDMI) += ./core/control.o
COBJS-$(CONFIG_HDMI) += ./core/halColorSpaceConverter.o
COBJS-$(CONFIG_HDMI) += ./core/productParams.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerGcp.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerVideo.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerVsd.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerPackets.o
COBJS-$(CONFIG_HDMI) += ./core/video.o
COBJS-$(CONFIG_HDMI) += ./core/audio.o
COBJS-$(CONFIG_HDMI) += ./core/halAudioGpa.o
COBJS-$(CONFIG_HDMI) += ./core/halVideoSampler.o
COBJS-$(CONFIG_HDMI) += ./core/halFrameComposerAudio.o
COBJS-$(CONFIG_HDMI) += ./core/halInterrupt.o
COBJS-$(CONFIG_HDMI) += ./bsp/access.o
COBJS-$(CONFIG_HDMI) += ./bsp/system.o
COBJS-$(CONFIG_HDMI) += ./bsp/mutex.o
COBJS-$(CONFIG_HDMI) += ./bsp/board.o
COBJS-$(CONFIG_HDMI) += ./hdcp/hdcp.o
COBJS-$(CONFIG_HDMI) += ./hdcp/hdcpParams.o
COBJS-$(CONFIG_HDMI) += ./hdcp/halHdcp.o
COBJS-$(CONFIG_HDMI) += ./hdcp/hdcpVerify.o
COBJS-$(CONFIG_HDMI) += ./phy/halI2cMasterPhy.o
COBJS-$(CONFIG_HDMI) += ./phy/halSourcePhy.o
COBJS-$(CONFIG_HDMI) += ./phy/phy.o
COBJS-$(CONFIG_HDMI) += ./edid/videoCapabilityDataBlock.o
COBJS-$(CONFIG_HDMI) += ./edid/monitorRangeLimits.o
COBJS-$(CONFIG_HDMI) += ./edid/halEdid.o
COBJS-$(CONFIG_HDMI) += ./edid/edid.o
COBJS-$(CONFIG_HDMI) += ./edid/dtd.o
COBJS-$(CONFIG_HDMI) += ./edid/colorimetryDataBlock.o
COBJS-$(CONFIG_HDMI) += ./edid/shortVideoDesc.o
COBJS-$(CONFIG_HDMI) += ./edid/speakerAllocationDataBlock.o
COBJS-$(CONFIG_HDMI) += ./edid/shortAudioDesc.o
COBJS-$(CONFIG_HDMI) += ./edid/hdmivsdb.o
COBJS-$(CONFIG_HDMI) += ./api/api.o



COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
