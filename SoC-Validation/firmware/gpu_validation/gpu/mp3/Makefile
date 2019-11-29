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

LIB	:= $(obj)libgpu_mp3.o


COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F000_MMU_APB_check_0_a.o m400_mp_3_F000_MMU_APB_check_0_b.o m400_mp_3_F000_MMU_APB_check_0_c.o m400_mp_3_F000_MMU_APB_run.o
 
COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F010_PP0_MMU_APB_check_0_a.o m400_mp_3_F010_PP0_MMU_APB_check_0_b.o m400_mp_3_F010_PP0_MMU_APB_check_0_c.o m400_mp_3_F010_PP0_MMU_APB_run.o 

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F011_PP1_MMU_APB_check_0_a.o m400_mp_3_F011_PP1_MMU_APB_check_0_b.o m400_mp_3_F011_PP1_MMU_APB_check_0_c.o m400_mp_3_F011_PP1_MMU_APB_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F013_MMU_GP_IRQ_check_0_a.o m400_mp_3_F013_MMU_GP_IRQ_check_0_b.o m400_mp_3_F013_MMU_GP_IRQ_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F014_MMU_PP0_IRQ_check_0_a.o m400_mp_3_F014_MMU_PP0_IRQ_check_0_b.o m400_mp_3_F014_MMU_PP0_IRQ_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F015_MMU_PP1_IRQ_check_0_a.o m400_mp_3_F015_MMU_PP1_IRQ_check_0_b.o m400_mp_3_F015_MMU_PP1_IRQ_run.o


COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F023_GP_PP0_check_0_a.o m400_mp_3_F023_GP_PP0_check_0_b.o m400_mp_3_F023_GP_PP0_check_0_c.o m400_mp_3_F023_GP_PP0_check_0_d.o m400_mp_3_F023_GP_PP0_mem_0.o m400_mp_3_F023_GP_PP0_run.o 

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F024_GP_PP0_PP1_check_0_a.o m400_mp_3_F024_GP_PP0_PP1_check_0_b.o m400_mp_3_F024_GP_PP0_PP1_check_0_c.o m400_mp_3_F024_GP_PP0_PP1_check_0_d.o m400_mp_3_F024_GP_PP0_PP1_check_0_e.o m400_mp_3_F024_GP_PP0_PP1_check_0_f.o m400_mp_3_F024_GP_PP0_PP1_mem_0.o m400_mp_3_F024_GP_PP0_PP1_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_a.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_b.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_c.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_d.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_e.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_f.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_g.o m400_mp_3_F028_GP_PP0_PP1_PP2_check_0_h.o m400_mp_3_F028_GP_PP0_PP1_PP2_mem_0.o m400_mp_3_F028_GP_PP0_PP1_PP2_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F030_PP2_MMU_APB_check_0_a.o m400_mp_3_F030_PP2_MMU_APB_check_0_b.o m400_mp_3_F030_PP2_MMU_APB_check_0_c.o m400_mp_3_F030_PP2_MMU_APB_run.o

COBJS-$(CONFIG_MALIGPU_MP3) +=m400_mp_3_F032_MMU_PP2_IRQ_check_0_a.o m400_mp_3_F032_MMU_PP2_IRQ_check_0_b.o m400_mp_3_F032_MMU_PP2_IRQ_run.o

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
