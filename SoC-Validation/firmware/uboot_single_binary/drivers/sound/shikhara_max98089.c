
/*
 * max98088.c -- MAX98088 ALSA SoC Audio driver
 *
 * Copyright 2010 Maxim Integrated Products
 *
 * Modified for uboot by Chih-Chung Chang (chihchung@chromium.org),
 * following the changes made in max98095.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <i2c.h>
#include <sound.h>
#include <asm/arch/shikhara_i2s.h>
#include <asm/arch/shikhara_max98089.h>

DECLARE_GLOBAL_DATA_PTR;
int global =1;
enum max98088_type {
	MAX98088,
};

struct max98088_priv {
	enum max98088_type devtype;
	unsigned int sysclk;
	unsigned int rate;
	unsigned int fmt;
};

struct max98088_priv g_max98088_info;
unsigned int g_max98088_i2c_dev_addr;

static u8 codec_reg[] = {
0x00,           
0x01,
0x02,
0x03,
0x0F,
0x10,
0x11,
0x12,
0x13,
0x14,
0x15,
0x16,
0x17,
0x18,
0x19,
0x1A,
0x1B,
0x1C,
0x1D,
0x1E,
0x1F,
0x20,
0x21,
0x22,
0x23,
0x24,
0x25,
0x26,
0x27,
0x28,
0x29,
0x2A,
0x2B,
0x2C,
0x2D,
0x2E,
0x2F,
0x30,
0x31,
0x32,
0x33,
0x34,
0x35,
0x36,
0x37,
0x38,
0x39,
0x3A,
0x3B,
0x3C,
0x3D,
0x3E,
0x3F,
0x40,
0x41,
0x42,
0x43,
0x44,
0x45,
0x46,
0x47,
0x48,
0x49,
0x4A,
0x4B,
0x4C,
0x4D,
0x4E,
0x4F,
0x50,
0x51,
0x52,
0x84,
0xB6,
0xC0,
0xFF,

};

/* codec mclk clock divider coefficients. Index 0 is reserved. */
static const int rate_table[] = {0, 8000, 11025, 16000, 22050, 24000, 32000,
				 44100, 48000, 88200, 96000};

/*
 * Writes value to a device register through i2c
 *
 * @param reg	reg number to be write
 * @param data	data to be writen to the above registor
 *
 * @return	int value 1 for change, 0 for no change or negative error code.
 */
static int max98088_i2c_write(unsigned int reg, unsigned char data)
{
//	printf(" Write Addr : 0x%02X, Data :  0x%02X\n", reg, data);
	return i2c_write(g_max98088_i2c_dev_addr, reg, 1, &data, 1);
}

/*
 * Read a value from a device register through i2c
 *
 * @param reg	reg number to be read
 * @param data	address of read data to be stored
 *
 * @return	int value 0 for success, -1 in case of error.
 */
static unsigned int max98088_i2c_read(unsigned int reg, unsigned char *data)
{
	int ret;

	ret = i2c_read(g_max98088_i2c_dev_addr, reg, 1, data, 1);
//	printf("\t\treg=0x%x\t\tdata=%x\n",reg,*data);
	if (ret != 0) {
		printf(" Error while reading register %#04x\n", reg);
		return -1;
	}

	return 0;
}

/*
 * update device register bits through i2c
 *
 * @param reg	codec register
 * @param mask	register mask
 * @param value	new value
 *
 * @return int value 0 for success, non-zero error code.
 */
static int max98088_update_bits(unsigned int reg, unsigned char mask,
				unsigned char value)
{
	int change, ret = 0;
	unsigned char old, new;

	if (max98088_i2c_read(reg, &old) != 0)
		return -1;
	new = (old & ~mask) | (value & mask);
	change  = (old != new) ? 1 : 0;
	if (change)
		ret = max98088_i2c_write(reg, new);
	if (ret < 0)
		return ret;

	return change;
}

/*
 * codec mclk clock divider coefficients based on sampling rate
 *
 * @param rate sampling rate
 * @param value address of indexvalue to be stored
 *
 * @return	0 for success or negative error code.
 */
static int rate_value(int rate, u8 *value)
{
	int i;

	for (i = 1; i < ARRAY_SIZE(rate_table); i++) {
		if (rate_table[i] >= rate) {
			*value = i;
			return 0;
		}
	}
	*value = 1;

	return -1;
}

/*
 * Sets hw params for max98088
 *
 * @param max98088	max98088 information pointer
 * @param rate		Sampling rate
 * @param bits_per_sample	Bits per sample
 *
 * @return -1 for error  and 0  Success.
 */
static int max98088_dai1_hw_params(unsigned int rate,unsigned int bits_per_sample)
{
	u8 regval;
	int error;

	switch (bits_per_sample) {
	case 16:
		error = max98088_update_bits(M98088_REG_14_DAI1_FORMAT,
					     M98088_DAI_WS, 0);
		break;
	case 24:
		error = max98088_update_bits(M98088_REG_14_DAI1_FORMAT,
					     M98088_DAI_WS, M98088_DAI_WS);
		break;
	default:
		debug("%s: Illegal bits per sample %d.\n",
		      __func__, bits_per_sample);
		return -1;
	}

	error |= max98088_update_bits(M98088_REG_51_PWR_SYS, M98088_SHDNRUN, 0);

	if (rate_value(rate, &regval)) {
		debug("%s: Failed to set sample rate to %d.\n",
		      __func__, rate);
		return -1;
	}

	error |= max98088_update_bits(M98088_REG_11_DAI1_CLKMODE,
                                      M98088_CLKMODE_MASK, 0x80);


	/* Update sample rate mode */
	if (rate < 50000)
		error |= max98088_update_bits(M98088_REG_18_DAI1_FILTERS,
					      M98088_DAI_DHF, 0);
	else
		error |= max98088_update_bits(M98088_REG_18_DAI1_FILTERS,
					      M98088_DAI_DHF, M98088_DAI_DHF);

	error |= max98088_update_bits(M98088_REG_51_PWR_SYS,
				      M98088_SHDNRUN, M98088_SHDNRUN);

	if (error < 0) {
		debug("%s: Error setting hardware params.\n", __func__);
		return -1;
	}

	return 0;
}

/*
 * Configures Audio interface system clock for the given frequency
 *
 * @param max98088	max98088 information
 * @param freq		Sampling frequency in Hz
 *
 * @return -1 for error and 0 success.
 */
static int max98088_dai_set_sysclk(unsigned int freq)
{
	unsigned char pwr;
	int error = 0;

	/* Setup clocks for slave mode, and using the PLL
	 * PSCLK = 0x01 (when master clk is 10MHz to 20MHz)
	 *         0x02 (when master clk is 20MHz to 30MHz)..
	 */
	if ((freq >= 10000000) && (freq < 20000000)) {
		error = max98088_i2c_write(M98088_REG_10_SYS_CLK, 0x10);
	} else if ((freq >= 20000000) && (freq < 30000000)) {
		error = max98088_i2c_write(M98088_REG_10_SYS_CLK, 0x20);
	} else {
		printf(" Invalid master clock frequency\n");
		return -1;
	}
	global = 1;
	error |= max98088_i2c_read(M98088_REG_51_PWR_SYS, &pwr);
	if (pwr & M98088_SHDNRUN) {
		error |= max98088_update_bits(M98088_REG_51_PWR_SYS,
					      M98088_SHDNRUN, 0);
		error |= max98088_update_bits(M98088_REG_51_PWR_SYS,
					      M98088_SHDNRUN, M98088_SHDNRUN);
	}


	if (error < 0)
		return -1;

	return 0;
}

/*
 * Sets Max98088 I2S format
 *
 * @param max98088	max98088 information
 * @param fmt		i2S format - supports a subset of the options defined
 *			in i2s.h.
 *
 * @return -1 for error and 0  Success.
 */
static int max98088_dai1_set_fmt(int fmt)
{
	u8 reg15val;
	u8 reg14val = 0;
	int error = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		/* Slave mode PLL */
		error |= max98088_i2c_write(M98088_REG_12_DAI1_CLKCFG_HI,
					    0x80);
		error |= max98088_i2c_write(M98088_REG_13_DAI1_CLKCFG_LO,
					    0x00);
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		/* Set to master mode */
		reg14val |= M98088_DAI_MAS;
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
	case SND_SOC_DAIFMT_CBM_CFS:
	default:
		debug("%s: Clock mode unsupported\n", __func__);
		return -1;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		reg14val |= M98088_DAI_DLY;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		break;
	default:
		debug("%s: Unrecognized format.\n", __func__);
		return -1;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_NB_IF:
		reg14val |= M98088_DAI_WCI;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		reg14val |= M98088_DAI_BCI;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		reg14val |= M98088_DAI_BCI|M98088_DAI_WCI;
		break;
	default:
		debug("%s: Unrecognized inversion settings.\n",  __func__);
		return -1;
	}

	error |= max98088_update_bits(M98088_REG_14_DAI1_FORMAT,
                                      M98088_DAI_MAS | M98088_DAI_DLY |
                                      M98088_DAI_BCI | M98088_DAI_WCI,
                                      reg14val);

	
	reg15val = M98088_DAI_BSEL64;
	error |= max98088_i2c_write(M98088_REG_15_DAI1_CLOCK, reg15val);

	if (error < 0) {
		debug("%s: Error setting i2s format.\n", __func__);
		return -1;
	}

	return 0;
}

/*
 * Intialise max98088 codec device
 *
 * @param max98088	max98088 information
 *
 * @returns -1 for error  and 0 Success.
 */
static int max98088_device_init(void)
{
	unsigned char id;
	int error = 0;

	/* initialize private data */

	error = max98088_i2c_read(M98088_REG_FF_REV_ID, &id);
	debug("%s: Hardware revision: %c\n", __func__, (id - 0x40) + 'A');
	
	if (error < 0) {
		printf("ERROR: Failure reading hardware revision: %d\n", error);
		goto err_access;
	}

	
	error |= max98088_i2c_write(M98088_REG_51_PWR_SYS, 0x0);
	error |= max98088_i2c_write(M98088_REG_0F_IRQ_ENABLE, 0x00);

	/* power enable */
	error |= max98088_i2c_write(M98088_REG_4D_PWR_EN_OUT,0xc3);
	/*
	 * initialize registers to hardware default configuring audio
	 * interface2 to DAI1
	 */

	error |= max98088_i2c_write(M98088_REG_22_MIX_DAC,
				    M98088_DAI1L_TO_DACL|M98088_DAI1R_TO_DACR);
	error |= max98088_i2c_write(M98088_REG_4E_BIAS_CNTL, 0xF0);
	error |= max98088_i2c_write(M98088_REG_50_DAC_BIAS2, 0x05);
	error |= max98088_i2c_write(M98088_REG_16_DAI1_IOCFG,0x43);

	/* route DACL and DACR output to Speaker and Headphone */
	error |= max98088_i2c_write(M98088_REG_25_MIX_HP_LEFT, 0);   /* DACL */
	error |= max98088_i2c_write(M98088_REG_26_MIX_HP_RIGHT, 0);  /* DACR */
	error |= max98088_i2c_write(M98088_REG_2B_MIX_SPK_LEFT, 1);  /* DACL */
        error |= max98088_i2c_write(M98088_REG_2C_MIX_SPK_RIGHT, 1); /* DACR */

	/* set volume */
	error |= max98088_i2c_write(M98088_REG_39_LVL_HP_L, 0x18);
	error |= max98088_i2c_write(M98088_REG_3A_LVL_HP_R, 0x18);
	error |= max98088_i2c_write(M98088_REG_3D_LVL_SPK_L, 0x00);
        error |= max98088_i2c_write(M98088_REG_3E_LVL_SPK_R, 0x00);

	error |= max98088_i2c_write(M98088_REG_23_MIX_ADC_LEFT,0x40);
        error |= max98088_i2c_write(M98088_REG_24_MIX_ADC_RIGHT,0x40);

        error |= max98088_i2c_write(M98088_REG_28_MIX_REC_LEFT,0xf);
        error |= max98088_i2c_write(M98088_REG_29_MIX_REC_RIGHT,0x0);

        error |= max98088_i2c_write(M98088_REG_33_LVL_ADC_L,0x3);
        error |= max98088_i2c_write(M98088_REG_34_LVL_ADC_R,0x3);
        
        error |= max98088_i2c_write(M98088_REG_35_LVL_MIC1,0x40);
        error |= max98088_i2c_write(M98088_REG_36_LVL_MIC2,0x40);

	error |= max98088_i2c_write(M98088_REG_4C_PWR_EN_IN,0xb);

        error |= max98088_i2c_write(M98088_REG_B6_DAI1_BIQUAD_BASE,0xc0);

	error |= max98088_i2c_write(M98088_REG_27_MIX_HP_CNTL, 0x30);
	error |= max98088_i2c_write(M98088_REG_2F_LVL_DAI1_PLAY,0x0);

	error |= max98088_i2c_write(M98088_REG_51_PWR_SYS, 0x80);
err_access:
	if (error < 0)
		return -1;

	return 0;
}

static int max98088_do_init(int sampling_rate, int mclk_freq,
			    int bits_per_sample)
{
	int ret;

	/* shift the device address by 1 for 7 bit addressing */
	g_max98088_i2c_dev_addr = 0x10;

	ret = max98088_device_init();
	if (ret < 0) {
		printf("ERROR: max98088 codec device init failed\n");
		return ret;
	}

	
	ret = max98088_dai1_hw_params(sampling_rate,bits_per_sample);
	if (ret != 0){
		printf("ERROR: max98088 codec dai hw params failed \n");
		return ret;
	}
	if (ret == 0) {
		ret = max98088_dai1_set_fmt(SND_SOC_DAIFMT_I2S |
					    SND_SOC_DAIFMT_NB_NF |
					    SND_SOC_DAIFMT_CBS_CFS);
	}

	ret = max98088_dai_set_sysclk(mclk_freq);
	if (ret < 0) {
		printf("ERROR: max98088 codec set sys clock failed\n");
		return ret;
	}

	return ret;
}

/* max98088 Device Initialisation */
int max98088_init(void)
{
	int ret;
        unsigned int sampling_rate;
        unsigned int mclk_freq;
        unsigned int bits_per_sample;
	int i;
	unsigned char data;
	int old_bus = i2c_get_bus_num();
        sampling_rate=sound_params.freq;
        mclk_freq=(sound_params.rfs)*(sampling_rate);
        bits_per_sample=sound_params.bps;     
	i2c_set_bus_num(1);
	ret = max98088_do_init(sampling_rate, mclk_freq,
			       bits_per_sample);
/*	printf("REGISTERS\n");
        for(i=0;i<76;i++){
                max98088_i2c_read(codec_reg[i],&data);
        }
*/
	i2c_set_bus_num(old_bus);

	return ret;
}

