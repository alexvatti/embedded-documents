#include <common.h>
#include <asm/arch/shikhara_sgtl5000.h>
#include <sound.h> 
#include <i2c.h>
#include <asm/arch/shikhara_i2c.h>

static u16 codec_reg[] = {
	0x0000,
	0x0002,
	0x0004,
	0x0006,
	0x000a,
	0x000e,
	0x0010,
	0x0014,
	0x0020,
	0x0022,
	0x0024,
	0x0026,
	0x0028,
	0x002a,
	0x002c,
	0x002e,
	0x0030,
	0x0032,
	0x0034,
	0x0036,
	0x003c,
	0x003a,
};

static unsigned int master=0;
struct sgtl5000_priv {
	int sysclk;	/* sysclk rate */
	int master;	/* i2s master or not */
	int fmt;	/* i2s data format */
};
        
static unsigned int sgtl5000_i2c_read(unsigned int reg, unsigned short *data)
{
        int ret;
        //printf(" reading register %x\n", reg);

        ret = i2c_read(SGTL_CODEC_I2C_SLAVE_ADDR, reg, 2, (unsigned char *)data, 2);
	//printf("\t\treg=0x%x\t\tdata=%x\n",reg,*data);
        if (ret != 0) {
                printf("Error while reading register %#04x\n", reg);
                return -1;
        }

        return 0;
}

static int sgtl5000_i2c_write(unsigned int reg, unsigned short data)
{
            //printf("%s: Write Addr  yours: 0x%02X, Data :  0x%02X\n",
             //   __func__, reg, data);
        return i2c_write(SGTL_CODEC_I2C_SLAVE_ADDR,reg,2,(unsigned char *)&data,2);
}



int sgtl5000_update(unsigned short reg,
                                unsigned int mask, unsigned int value)
{                                       
        int change;
        unsigned int old, new;
        unsigned short ret;
                
        sgtl5000_i2c_read(reg,&ret);
        if (ret < 0)
                return ret;

        old = ret;
        new = (old & ~mask) | value;
        change = old != new;
        if (change) {
                ret = sgtl5000_i2c_write(reg, new);
                if (ret < 0) 
                        return ret;
        }       
                
        return change;
}

/* set codec format */
static int sgtl5000_set_dai_fmt(unsigned int fmt)
{       

        unsigned int i2sctl = 0;
        switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
        case SND_SOC_DAIFMT_CBS_CFS:              /* Codec Configured as Slave */    
                break;
        case SND_SOC_DAIFMT_CBM_CFM:
                i2sctl |= SGTL5000_I2S_MASTER;
                master = 1;
                break;
        default:
                return -EINVAL;
        }

        /* setting i2s data format */
        switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
        case SND_SOC_DAIFMT_DSP_A:
                i2sctl |= SGTL5000_I2S_MODE_PCM;
                break;
        case SND_SOC_DAIFMT_DSP_B:
                i2sctl |= SGTL5000_I2S_MODE_PCM;
                i2sctl |= SGTL5000_I2S_LRALIGN;
                break;
        case SND_SOC_DAIFMT_I2S:
                i2sctl |= SGTL5000_I2S_MODE_I2S_LJ; /*I2S mode */
                break;
        case SND_SOC_DAIFMT_RIGHT_J:
                       i2sctl |= SGTL5000_I2S_MODE_RJ;
                i2sctl |= SGTL5000_I2S_LRPOL;
                break;
        case SND_SOC_DAIFMT_LEFT_J:
                i2sctl |= SGTL5000_I2S_MODE_I2S_LJ;
                i2sctl |= SGTL5000_I2S_LRALIGN;
                break;
        default:
                return -EINVAL;
        }


        /* Clock inversion */
        switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
        case SND_SOC_DAIFMT_NB_NF:               /* Data is valid on rising edge of the SCLK */ 
                break;
        case SND_SOC_DAIFMT_IB_NF:
                i2sctl |= SGTL5000_I2S_SCLK_INV;
                break;
        default:
                return -EINVAL;
        }

        sgtl5000_i2c_write(SGTL5000_CHIP_I2S_CTRL, i2sctl);

        return 0;
}

static int sgtl5000_codec_id(void)
{
	unsigned short id;
        unsigned int ret;

	/* read chip id */
        ret = sgtl5000_i2c_read(SGTL5000_CHIP_ID,&id);
	if(ret == 0)
        printf("SGTL5000 CODEC CHIP ID: %x\n",id);
	return ret;
}

int i2s_codec_post(void)
{
	unsigned int reg;	

	/* Set Codec on I2C Bus no 1 */
	i2c_set_bus_num(SGTL_CODEC_I2C_BUS_NO);
		
	/* Init I2C controller */
	i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);

	/* read chip id */
        reg = sgtl5000_codec_id();
	if(reg < 0){
		return -1;
	}

	return reg;
}

int sgtl5000_do_init(void)
{
    	unsigned short reg1;
    	unsigned int reg;
    	int vdda;
    	int vddio;
    	int vddd;
    	unsigned int ret;
    	unsigned int vag;
    	unsigned short ana_pwr;

        /* read chip information */
   	reg = sgtl5000_codec_id();
      	if(reg < 0){
		printf("Codec id read failed\n");
		return -1;
	} 
        /* Sets the VDDD linear regulator output voltage in 50 mV steps. Must clear the
	LINREG_SIMPLE_POWERUP in 0x0030 register after power-up, for this setting to produce the proper VDDD voltage.
 	*/
        sgtl5000_i2c_write(SGTL5000_CHIP_LINREG_CTRL,VDD_LINEAR);
       
        /*Power up the primary VDDD linear regulator and Power down the simple (low power) digital supply regulator
        when internal ldo enabled, simple digital power can be disabled */
        sgtl5000_i2c_read(SGTL5000_CHIP_ANA_POWER,&reg1);
        reg1 |= SGTL5000_LINEREG_D_POWERUP;
        sgtl5000_i2c_write(SGTL5000_CHIP_ANA_POWER, reg1);
        reg1 &= ~SGTL5000_LINREG_SIMPLE_POWERUP;
	sgtl5000_i2c_write(SGTL5000_CHIP_ANA_POWER, reg1);

        vdda=1800;
        vddio=1800;
        vddd=1200;
        vdda  = vdda / 1000;
        vddio = vddio / 1000;
        vddd  = vddd / 1000;

        sgtl5000_i2c_read(SGTL5000_CHIP_ANA_POWER,&ana_pwr);
        ana_pwr |= SGTL5000_DAC_STEREO | SGTL5000_ADC_STEREO | SGTL5000_REFTOP_POWERUP;

	if (vddio < 3100 && vdda < 3100) {
		sgtl5000_update( SGTL5000_CHIP_CLK_TOP_CTRL,
					SGTL5000_INT_OSC_EN,
					SGTL5000_INT_OSC_EN);
	ana_pwr |= SGTL5000_VDDC_CHRGPMP_POWERUP;	
	}
       /*When DAC_POWERUP is set, this allows the DAC to be put into left only mono
        operation for power savings, ADC to be put into left only mono
        operation for power savings and Power up the reference bias currents
        here we are using stereo type so DAC is configured for stereo*/ 

        sgtl5000_i2c_write(SGTL5000_CHIP_ANA_POWER, ana_pwr);
       
       /* set voltage to register */
        sgtl5000_update(SGTL5000_CHIP_LINREG_CTRL,(0x1 << 4) - 1, 0x8);
       /*
         if vddd linear reg has been enabled,
         simple digital supply should be clear to get
         proper VDDD voltage.         */
        sgtl5000_update(SGTL5000_CHIP_ANA_POWER,
                        SGTL5000_LINREG_SIMPLE_POWERUP,
                        0);

        /*
         set ADC/DAC VAG to vdda / 2,
         should stay in range (0.8v, 1.575v)
         */
        vag = vdda / 2;
        if (vag <= SGTL5000_ANA_GND_BASE)
                vag = 0x4;
        else if (vag >= SGTL5000_ANA_GND_BASE + SGTL5000_ANA_GND_STP *
                 (SGTL5000_ANA_GND_MASK >> SGTL5000_ANA_GND_SHIFT))
                vag = SGTL5000_ANA_GND_MASK >> SGTL5000_ANA_GND_SHIFT;
        else
                vag = (vag - SGTL5000_ANA_GND_BASE) / SGTL5000_ANA_GND_STP;

        /* These bits control the analog ground voltage in 25 mV steps. This should usually be
           set to VDDA/2 or lower for best performance */ 
        sgtl5000_update(SGTL5000_CHIP_REF_CTRL,
                        SGTL5000_ANA_GND_MASK,
                        vag << SGTL5000_ANA_GND_SHIFT);
	
	vag = vddio / 2;
	if (vag <= SGTL5000_LINE_OUT_GND_BASE)
		vag = 0x4;
	else if (vag >= SGTL5000_LINE_OUT_GND_BASE +
		SGTL5000_LINE_OUT_GND_STP * SGTL5000_LINE_OUT_GND_MAX)
		vag = SGTL5000_LINE_OUT_GND_MAX;
	else
		vag = (vag - SGTL5000_LINE_OUT_GND_BASE) /
		    SGTL5000_LINE_OUT_GND_STP;

	sgtl5000_update( SGTL5000_CHIP_LINE_OUT_CTRL,
			SGTL5000_LINE_OUT_GND_MASK |
			SGTL5000_LINE_OUT_CURRENT_MASK,
			vag << SGTL5000_LINE_OUT_GND_SHIFT |
			SGTL5000_LINE_OUT_CURRENT_360u <<
				SGTL5000_LINE_OUT_CURRENT_SHIFT);

      
 	/* enable small pop, introduce 400ms delay in turning off */
        sgtl5000_update(SGTL5000_CHIP_REF_CTRL,
                        SGTL5000_SMALL_POP,
                        SGTL5000_SMALL_POP);
      
	/* Line Out Volage */
	sgtl5000_i2c_write( SGTL5000_CHIP_LINE_OUT_VOL,0x1f1f);

        /* disable short cut detector */
        sgtl5000_i2c_write(SGTL5000_CHIP_SHORT_CTRL, 0);

        /* i2s_in is selected as data source for DAC */
        sgtl5000_i2c_write(SGTL5000_CHIP_SSS_CTRL,
                           SGTL5000_DAC_SEL_I2S_IN << SGTL5000_DAC_SEL_SHIFT);
        
        /* Enabling DAC block */ 
        sgtl5000_i2c_write(SGTL5000_CHIP_DIG_POWER,SGTL5000_DAC_EN|SGTL5000_ADC_EN);

        /* Enables volume ramp, this field affects DAC_VOL. The volume ramp effects both volume settings and mute.
           When set to 1 a soft mute is enabled. DAC Right Mute and DAC Left Mute are enabled when VOL_RAMP_EN = 1
        */
        sgtl5000_i2c_write(SGTL5000_CHIP_ADCDAC_CTRL,
                        SGTL5000_DAC_VOL_RAMP_EN |
                        SGTL5000_DAC_MUTE_RIGHT |
                        SGTL5000_DAC_MUTE_LEFT);
  
        /* This register Sets drive strength for output pads of I2S_LRCLK, I2S_SCLK, CTRL_DATA and CTRL_CLK */
        sgtl5000_i2c_write(SGTL5000_CHIP_PAD_STRENGTH, PAD_STRENGTH);
                                                                          
        /* Enables Headphone zero cross detect */
        sgtl5000_i2c_write(SGTL5000_CHIP_ANA_CTRL,
                      SGTL5000_HP_ZCD_EN|SGTL5000_ADC_ZCD_EN);

        /* Setting DAC right and left channel volume to 0db */
        sgtl5000_i2c_write(SGTL5000_CHIP_DAC_VOL,DAC_VOL);
         
        /* Disabling DAP(Digital Audio Processing) and Mixer path */
        sgtl5000_i2c_write(SGTL5000_DAP_CTRL, 0);

	sgtl5000_i2c_write(SGTL5000_CHIP_ANA_ADC_CTRL,
		(0xf << SGTL5000_ADC_VOL_LEFT_SHIFT) |\
		(0xf << SGTL5000_ADC_VOL_RIGHT_SHIFT));

	/* MIC settings */
	sgtl5000_i2c_write(SGTL5000_CHIP_MIC_CTRL,0x111);

        /* Power up the VAG reference buffer. Setting this bit starts the power up ramp for the
        headphone. The headphone powerup should be set BEFORE clearing this bit. When this bit is cleared the power-down ramp is started */
        ret = sgtl5000_update(SGTL5000_CHIP_ANA_POWER,SGTL5000_VAG_POWERUP, 0);
        mdelay(500);    
       
        return reg;
}


static int sgtl5000_set_clock(int frame_rate)
{       
        int clk_ctl = 0;
        //int sys_fs;     /* sample freq */
       

	clk_ctl |= SGTL5000_RATE_MODE_DIV_1 << SGTL5000_RATE_MODE_SHIFT;

        /* set the sys_fs according to frame rate */
        switch (frame_rate) {
        case 32000:
                clk_ctl |= SGTL5000_SYS_FS_32k << SGTL5000_SYS_FS_SHIFT;
                break;
        case 44100:
                clk_ctl |= SGTL5000_SYS_FS_44_1k << SGTL5000_SYS_FS_SHIFT;
                break;
        case 48000:
                clk_ctl |= SGTL5000_SYS_FS_48k << SGTL5000_SYS_FS_SHIFT;
                break;
        case 96000:
                clk_ctl |= SGTL5000_SYS_FS_96k << SGTL5000_SYS_FS_SHIFT;
                break;
        default:
                printf("frame rate %d not supported\n",frame_rate);
                return -EINVAL;
        }

        /*
         * calculate the divider of mclk/sample_freq,
         * factor of freq =96k can only be 256, since mclk in range (12m,27m)
         */
        switch (SYS_CLK/frame_rate) {
        case 256:
                clk_ctl |= SGTL5000_MCLK_FREQ_256FS <<
                        SGTL5000_MCLK_FREQ_SHIFT;
                break;
        case 384:
                clk_ctl |= SGTL5000_MCLK_FREQ_384FS <<
                        SGTL5000_MCLK_FREQ_SHIFT;
                break;
        case 512:
                clk_ctl |= SGTL5000_MCLK_FREQ_512FS <<
                        SGTL5000_MCLK_FREQ_SHIFT;
                break;
        default:

                /* if mclk not satisify the divider, use pll */
                if (master) {
                        clk_ctl |= SGTL5000_MCLK_FREQ_PLL <<
                                SGTL5000_MCLK_FREQ_SHIFT;
                } else {
                        printf(
                                "PLL not supported in slave mode\n");
                        return -EINVAL;
                }
        }

        sgtl5000_i2c_write(SGTL5000_CHIP_CLK_CTRL, clk_ctl);

        return 0;
}



static int sgtl5000_hw_params(int sample_freq,int bits_per_sample)
{
        int i2s_ctl = 0;
        //int stereo;
        int ret;
        ret = sgtl5000_set_clock(sample_freq);
        if (ret)
                return ret;

        /* set i2s data format */
        switch (bits_per_sample) {
        case 16:
                i2s_ctl |= SGTL5000_I2S_DLEN_16 << SGTL5000_I2S_DLEN_SHIFT;
                i2s_ctl |= SGTL5000_I2S_SCLKFREQ_32FS <<
                    SGTL5000_I2S_SCLKFREQ_SHIFT;
                break;
        case 20:
                i2s_ctl |= SGTL5000_I2S_DLEN_20 << SGTL5000_I2S_DLEN_SHIFT;
                i2s_ctl |= SGTL5000_I2S_SCLKFREQ_64FS <<
                    SGTL5000_I2S_SCLKFREQ_SHIFT;
                break;
        case 24:

                i2s_ctl |= SGTL5000_I2S_DLEN_24 << SGTL5000_I2S_DLEN_SHIFT;
                i2s_ctl |= SGTL5000_I2S_SCLKFREQ_64FS <<
                    SGTL5000_I2S_SCLKFREQ_SHIFT;
                break;
        case 32:
                i2s_ctl |= SGTL5000_I2S_DLEN_32 << SGTL5000_I2S_DLEN_SHIFT;
                i2s_ctl |= SGTL5000_I2S_SCLKFREQ_64FS <<
                    SGTL5000_I2S_SCLKFREQ_SHIFT;
                break;
        default:
                return -EINVAL;
        }

        sgtl5000_update( SGTL5000_CHIP_I2S_CTRL,
                        SGTL5000_I2S_DLEN_MASK | SGTL5000_I2S_SCLKFREQ_MASK,
                        i2s_ctl);

        return 0;
}
	    
int sgtl5000_init()
{
        int ret;
	unsigned short reg;
        unsigned int dai_format;
        unsigned int sample_freq;
        unsigned int bits_per_sample;
	int i,size;
	unsigned short data;
        sample_freq=sound_params.freq;
        bits_per_sample=sound_params.bps;
        dai_format = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |SND_SOC_DAIFMT_CBS_CFS;
	i2c_set_bus_num(SGTL_CODEC_I2C_BUS_NO);
        ret = sgtl5000_do_init();
        sgtl5000_set_dai_fmt(dai_format);   
        sgtl5000_hw_params(sample_freq,bits_per_sample);
       	 
        /*Enabling I2S data input */
        sgtl5000_i2c_read(SGTL5000_CHIP_DIG_POWER,&reg);
        reg=reg|SGTL5000_I2S_IN_POWERUP|SGTL5000_I2S_OUT_POWERUP;	/* Enable I2S IN/OUT */
        sgtl5000_i2c_write(SGTL5000_CHIP_DIG_POWER,reg); 
        sgtl5000_i2c_read(SGTL5000_CHIP_ANA_POWER,&reg);
        reg=reg | SGTL5000_DAC_POWERUP | SGTL5000_HP_POWERUP | SGTL5000_CAPLESS_HP_POWERUP|SGTL5000_VAG_POWERUP|SGTL5000_ADC_POWERUP;	/* 0x5AFE */ 
        sgtl5000_i2c_write(SGTL5000_CHIP_ANA_POWER,reg);
        sgtl5000_i2c_write(SGTL5000_CHIP_ADCDAC_CTRL,DAC_UNMUTE);
	
	size = ARRAY_SIZE(codec_reg);
	 printf("REGISTERS\n");
	for(i=0;i<size;i++){
		sgtl5000_i2c_read(codec_reg[i],&data);
	}
        return ret;
}  
