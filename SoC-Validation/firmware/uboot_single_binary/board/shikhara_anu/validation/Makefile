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

LIB	= $(obj)libvalidation.o

ifdef  CONFIG_SHIKHARA_VALIDATION
COBJS-y =cmd_peri_early_start.o
COBJS-y +=cmd_reg.o
COBJS-y +=cmd_hdmi_preview.o
COBJS-y +=cmd_clcd_preview.o
COBJS-y +=cmd_dsi_preview.o
COBJS-$(CONFIG_BLUETOOTH) += cmd_bttest.o
COBJS-$(CONFIG_BLUETOOTH) += cmd_bt.o
COBJS-$(CONFIG_KMI_MIN_TESTS) += cmd_apkybd.o
COBJS-$(CONFIG_KMI_MIN_TESTS) += cmd_apmouse.o
COBJS-$(CONFIG_SHIKHARA_DISPLAYS) += cmd_bmp_slideshow.o
COBJS-$(CONFIG_SHIKHARA_DISPLAYS) += cmd_raw_videoplayer.o
COBJS-$(CONFIG_SHIKHARA_DISPLAYS) += cmd_yuvtobmp.o
COBJS-$(CONFIG_SHIKHARA_DISPLAYS) += cmd_raw_display.o
COBJS-$(CONFIG_SHIKHARA_DISPLAYS) += cmd_shikhara_sw_cursor.o
COBJS-$(CONFIG_PL111) += cmd_clcd_hw_cursor.o
#COBJS-$(CONFIG_USB_HOST_DUMPTEST) += cmd_usbdump.o
#COBJS-$(CONFIG_USB_DEV_DUMPTEST) += cmd_usbdev_dump.o
COBJS-$(CONFIG_USB_INTR_TEST) += cmd_usb_intr.o
COBJS-$(CONFIG_SHIKHARA_GPIO) += cmd_gpio_led.o
#COBJS-$(CONFIG_SHIKHARA_GPIO) += cmd_gpio_input_test.o
COBJS-$(CONFIG_UART_LOOPBACK_TEST) += cmd_serial_pl011_loopback.o
COBJS-$(CONFIG_CMD_DMATEST) += cmd_uart1_dmamin.o
COBJS-$(CONFIG_CMD_DMATEST) += cmd_mem_dma_min.o 
COBJS-$(CONFIG_CMD_DMATEST) += cmd_dma_uartapi.o 
COBJS-$(CONFIG_CMD_DMATEST) += cmd_dmatest.o 
COBJS-$(CONFIG_CMD_DMATEST) += cmd_spi_dmamin.o 
#COBJS-$(CONFIG_SEV_SEG_TEST) += cmd_sev_segment.o
COBJS-$(CONFIG_CMD_DWMMC_TEST) += cmd_dwmmc.o
COBJS-$(CONFIG_CMD_CAN_TEST) += cmd_mincan.o
COBJS-$(CONFIG_SRAM_TEST) += cmd_sramtest.o
COBJS-$(CONFIG_DT_TEST) += cmd_dttest.o
COBJS-$(CONFIG_WDT_TEST) += cmd_wdttest.o
COBJS-$(CONFIG_NAND_TEST) += cmd_nandtest.o
COBJS-$(CONFIG_NAND_TEST) += cmd_nandmintest.o
COBJS-$(CONFIG_GPU_TEST) += cmd_gpu.o
COBJS-$(CONFIG_GPU_SCENE) += cmd_gpu_scene.o
COBJS-$(CONFIG_NORFLASH_TEST) += cmd_nortest.o
COBJS-$(CONFIG_PMU_TEST) += cmd_pmutest.o
COBJS-$(CONFIG_TC) += cmd_ts.o
COBJS-$(CONFIG_TZASC) += cmd_tzasc.o
COBJS-$(CONFIG_USB_MINTEST) += cmd_usbmintest.o
COBJS-$(CONFIG_I2S_DUMP) += cmd_i2sdump.o
COBJS-$(CONFIG_SPI_MINITEST) += cmd_spi_minitest.o
COBJS-$(CONFIG_SPI_MINITEST) += cmd_spi_loopbacktest.o 
COBJS-$(CONFIG_I2C_MINTEST) += cmd_i2c_mintest.o
COBJS-$(CONFIG_SPI_DUMP) += cmd_spidump.o
COBJS-$(CONFIG_I2C_DUMP) += cmd_i2cdump.o
COBJS-$(CONFIG_CMD_AV417) += cmd_av417reset.o
COBJS-$(CONFIG_CMD_IPC) += cmd_arc_ipc.o
COBJS-$(CONFIG_HDMI_DVI_MIN) += cmd_hdmi_dvi_min.o
COBJS-$(CONFIG_HDMI_AUDIO_MIN) += cmd_hdmi_audio_min.o
COBJS-$(CONFIG_GNSS) += cmd_gnss.o
COBJS-$(CONFIG_GNSS) +=cmd_gnss_intr_min.o
COBJS-$(CONFIG_SPI_SLAVETEST) += cmd_spislave.o
COBJS-$(CONFIG_CSI) += cmd_ov5640_preview.o
#COBJS-y += cmd_gpioloopback.o
#COBJS-y += cmd_sevenseg_test.o
COBJS-y += cmd_ddrtest.o
COBJS-$(CONFIG_SHIKHARA_POST) += cmd_post.o
COBJS-y += cmd_atp.o
endif
COBJS	:= $(sort $(COBJS-y))

SOBJS-$(CONFIG_TZASC) := tzasc_mode.o
SOBJS   := $(sort $(SOBJS-y))

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
