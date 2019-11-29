/*
 * system.c
 *
 *  Created on: Jun 25, 2010
 *  Synopsys Inc.
 *  SG DWC PT02
 * 
 * 
 * 	@note: this file should be re-written to match the environment the 
 * 	API is running on
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include <configs/anusoc.h>
#include "system.h"
#include "../util/log.h"
#include "../util/error.h"
#ifdef __XMK__
#include "xmk.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sys/time.h"
#include "sys/init.h"
#include "sys/intr.h"
#include "pthread.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "errno.h"
void xilkernel_init();
void xilkernel_start();
#else
#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
	#ifndef CONFIG_SHIKHARA  /*?? Not including unistd.h, which is not available in U-boot. */
	#include <unistd.h> 
	#endif
#endif
#endif

void system_SleepMs(unsigned ms)
{
#ifdef __XMK__
	sleep(ms);
#else
#ifdef WIN32
	Sleep(ms);
#else
/*	usleep(ms * 1000); ?? commented because function usleep() is not available in U-boot. It is replaced by mdelay */
	mdelay(ms);
#endif
#endif
}

int system_ThreadCreate(void* handle, thread_t pFunc, void* param)
{
#ifdef __XMK__
	pthread_t timerId;
	return pthread_create(&timerId, NULL, pFunc, param);
#else
	#ifdef CONFIG_SHIKHARA
	return TRUE;
	#else
	error_Set(ERR_NOT_IMPLEMENTED);
	return FALSE;
	#endif
#endif
}
int system_ThreadDestruct(void* handle)
{
	#ifdef CONFIG_SHIKHARA
        return TRUE;
        #else
	error_Set(ERR_NOT_IMPLEMENTED);
	return FALSE;
	#endif
}

int system_Start(thread_t thread)
{
#ifdef __XMK__
	xilkernel_init();
	xmk_add_static_thread(thread, 1);
	xilkernel_start();
#else
	#ifdef CONFIG_SHIKHARA
        return TRUE;
        #else
	thread(NULL);
	#endif
#endif
	return 0;
}

static unsigned system_InterruptMap(interrupt_id_t id)
{
#ifdef __XMK__
	switch (id)
	{
		case RX_INT:
			return XPAR_XPS_INTC_0_SYSTEM_INT3_INTR;
		case TX_WAKEUP_INT:
			return XPAR_XPS_INTC_0_SYSTEM_INT2_INTR;
		case TX_INT:
			return XPAR_XPS_INTC_0_SYSTEM_INT1_INTR;
		default:
			return XPAR_XPS_INTC_0_XPS_TIMER_0_INTERRUPT_INTR;
	}
#endif
	return ~0;
}

int system_InterruptDisable(interrupt_id_t id)
{
	if (system_InterruptMap(id) != (unsigned) (~0))
	{
#ifdef __XMK__
		disable_interrupt(system_InterruptMap(id));
		return TRUE;
#else
		#ifdef CONFIG_SHIKHARA
        	return TRUE;
        	#else
		error_Set(ERR_NOT_IMPLEMENTED);
		return FALSE;
		#endif
#endif
	}
	#ifdef CONFIG_SHIKHARA
        return TRUE;
	#endif
	error_Set(ERR_INTERRUPT_NOT_MAPPED);
	LOG_ERROR("Interrupt not mapped");
	return FALSE;
}

int system_InterruptEnable(interrupt_id_t id)
{
	if (system_InterruptMap(id) != (unsigned) (~0))
	{
#ifdef __XMK__
		enable_interrupt(system_InterruptMap(id));
		return TRUE;
#else
		#ifdef CONFIG_SHIKHARA
        	return TRUE;
        	#else
		error_Set(ERR_NOT_IMPLEMENTED);
		return FALSE;
		#endif
#endif
	}
	#ifdef CONFIG_SHIKHARA
        return TRUE;
	#endif
	error_Set(ERR_INTERRUPT_NOT_MAPPED);
	LOG_ERROR("Interrupt not mapped");
	return FALSE;
}

int system_InterruptAcknowledge(interrupt_id_t id)
{
	if (system_InterruptMap(id) != (unsigned) (~0))
	{
#ifdef __XMK__
		acknowledge_interrupt(system_InterruptMap(id));
		return TRUE;
#else
		#ifdef CONFIG_SHIKHARA
        	return TRUE;
        	#else
		error_Set(ERR_NOT_IMPLEMENTED);
		return FALSE;
		#endif
#endif
	}
	error_Set(ERR_INTERRUPT_NOT_MAPPED);
	LOG_ERROR("Interrupt not mapped");
	return FALSE;
}
int system_InterruptHandlerRegister(interrupt_id_t id, handler_t handler,
		void * param)
{
#ifdef __XMK__
	if (register_int_handler(system_InterruptMap(id), handler, param) != XST_SUCCESS)
	{
		LOG_ERROR("register_int_handler");
		return FALSE;
	}
	return TRUE;
#else
	#ifdef CONFIG_SHIKHARA
        return TRUE;
        #else
	error_Set(ERR_NOT_IMPLEMENTED);
	return FALSE;
	#endif
#endif
}

int system_InterruptHandlerUnregister(interrupt_id_t id)
{
#ifdef __XMK__
	unregister_int_handler(system_InterruptMap(id));
	return TRUE;
#else
	#ifdef CONFIG_SHIKHARA
        return TRUE;
        #else
	error_Set(ERR_NOT_IMPLEMENTED);
	return FALSE;
	#endif
#endif
}

#ifdef __cplusplus
}
#endif
