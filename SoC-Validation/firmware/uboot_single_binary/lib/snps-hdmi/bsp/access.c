/*
 * access.c
 * 
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include "access.h"
#include "../util/log.h"

#include "../util/error.h"
#include "mutex.h"
#include <asm/io.h>
#include <configs/anusoc.h>
static mutex_t access_mMutex = ~0;
static u8 * access_mBaseAddr = 0;
int access_Initialize(u8 * baseAddr)
{
	access_mBaseAddr = baseAddr;
	if (access_mMutex == ~0)
	{
		mutex_Initialize(&access_mMutex);
	}
	return TRUE;
}

int access_Standby()
{
	if (access_mMutex != ~0)
	{
		mutex_Destruct(&access_mMutex);
		access_mMutex = ~0;
	}
	return TRUE;
}

u8 access_CoreReadByte(u16 addr)
{
	u8 data = 0;
	mutex_Lock(&access_mMutex);
	data = access_Read(addr);
	mutex_Unlock(&access_mMutex);
	return data;
}

u8 access_CoreRead(u16 addr, u8 shift, u8 width)
{
	if (width <= 0)
	{
		error_Set(ERR_DATA_WIDTH_INVALID);
		LOG_ERROR("Invalid parameter: width == 0");
		return 0;
	}
	return (access_CoreReadByte(addr) >> shift) & (BIT(width) - 1);
}

void access_CoreWriteByte(u8 data, u16 addr)
{
	mutex_Lock(&access_mMutex);
	access_Write(data, addr);
	mutex_Unlock(&access_mMutex);
}

void access_CoreWrite(u8 data, u16 addr, u8 shift, u8 width)
{
	u8 temp = 0;
	u8 mask = 0;
	if (width <= 0)
	{
		error_Set(ERR_DATA_WIDTH_INVALID);
		LOG_ERROR("Invalid parameter: width == 0");
		return;
	}
	mask = BIT(width) - 1;
	if (data > mask)
	{
		error_Set(ERR_DATA_GREATER_THAN_WIDTH);
		LOG_ERROR("Invalid parameter: data > width");
		return;
	}
	mutex_Lock(&access_mMutex);
	temp = access_Read(addr);
	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	access_Write(temp, addr);
	mutex_Unlock(&access_mMutex);
}

u8 access_Read(u16 addr)
{
	/* mutex is locked */
#ifdef __XMK__
	return access_mBaseAddr[addr];
#else
	#ifdef CONFIG_SHIKHARA
	u32 base;
	 base = addr | HDMI_BASE_ADDR;
	 return readb(base);
	#else
	return 0;	
	#endif
#endif
}

void access_Write(u8 data, u16 addr)
{
	/* mutex is locked */
	LOG_WRITE(addr, data);
#ifdef __XMK__
	access_mBaseAddr[addr] = data;
#endif
	#ifdef CONFIG_SHIKHARA
	u32 base;
	base = addr | HDMI_BASE_ADDR;
	writeb(data, base);
	#endif
}