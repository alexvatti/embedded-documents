/*
 * Copyright (C) 2008-2010 ARM Limited                           
 *
 * This software is provided 'as-is', without any express or implied
 * warranties including the implied warranties of satisfactory quality, 
 * fitness for purpose or non infringement.  In no event will  ARM be 
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use, copy and modify this software for 
 * any purpose, and to redistribute the software, subject to the following 
 * restrictions: 
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.                                       
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * ----------------------------------------------------------------
 * File:     aem/power.c
 * ----------------------------------------------------------------
 */
 
#include "appf_types.h"
#include "appf_platform_api.h"
#include "appf_internals.h"
#include "appf_helpers.h"

/**
 * This function powers up a CPU
 *
 * It is entered with cluster->lock held.
 */
int appf_platform_power_up_cpu(struct appf_cpu *cpu,
                               struct appf_cluster *cluster)
{
    if (cpu->power_state == 1)
    {
        cpu->power_state = 0;
        dsb();
//        send_interrupt_gic(cluster->ic_address);  /* TODO: will this work between clusters? */
        /* TODO: cluster->power_state = rstate; */
    }
    else
    {
        /* Wake another CPU from deeper sleep */
        
#ifdef USE_REALVIEW_EB_RESETS
        int cpu_index = cpu - cluster->cpu_table;
        cpu->power_state = 0;
        dsb();
        /* Unlock system registers */
        *(volatile unsigned *)0x10000020 = 0xa05f;
        /* Cause the model to take the CPU out of reset */
        *(volatile unsigned *)0x10000074 |= 1 << (6 + cpu_index);
#endif
    }
    /* TODO: Add code to handle a cpu in another cluster */
    
    return APPF_OK;
}

/**
 * This function tells the PCU to power down the executing CPU, and possibly the whole cluster.
 *
 * It is entered with cluster->lock held.
 */
int appf_platform_enter_cstate(unsigned cpu_index, struct appf_cpu *cpu, struct appf_cluster *cluster)
{
    /*
     * This is where the code to talk to the PCU goes. We could check the value
     * of cpu->power_state, but we don't, since we can only do run or shutdown.
     */
    if (cluster->scu_address)
    {
//        set_status_a9_scu(cpu_index, STATUS_SHUTDOWN, cluster->scu_address);
    }

    return APPF_OK;
}

/**
 * This function tells the PCU this CPU has finished powering up.
 *
 * It is entered with cluster->lock held.
 */
int appf_platform_leave_cstate(unsigned cpu_index, struct appf_cpu *cpu, struct appf_cluster *cluster)
{
    /*
     * Housekeeping...
     */
    ++cluster->active_cpus;
    cluster->power_state = 0;
    cpu->power_state = 0;

    /*
     * We need to ensure that a CPU coming out of reset with caches off can see
     *  -  cluster->active_cpus
     *  -  cluster->power_state
     * so that it can tell whether it is the first CPU up. Note that mva==pa here.
     */
    dsb();
    clean_mva_dcache_v7_l1((void *)&(cluster->active_cpus));
    clean_mva_dcache_v7_l1((void *)&(cluster->power_state));
    if (cluster->l2_address && is_enabled_pl310(cluster->l2_address))
    {
        dsb();
        clean_range_pl310((void *)&(cluster->active_cpus), 4, cluster->l2_address);
        clean_range_pl310((void *)&(cluster->power_state), 4, cluster->l2_address);
    }

    /*
     * We could check the value in the SCU power status register as it is written at 
     * power up by the PCU. But we don't, since we know we're always powering up.
     */
    if (cluster->scu_address)
    {
//        set_status_a9_scu(cpu_index, STATUS_RUN, cluster->scu_address);
    }
    
    return APPF_OK;
}

/**
 * This function puts the executing CPU, and possibly the whole cluster, into STANDBY
 *
 * It is entered with cluster->lock held.
 */
int appf_platform_enter_cstate1(unsigned cpu_index, struct appf_cpu *cpu, struct appf_cluster *cluster)
{
//    if (cluster->power_state == 1 && cluster->l2_address)
//    {
//        set_status_pl310(STATUS_STANDBY, cluster->l2_address);
//    }

    return APPF_OK;
}
    
/**
 * This function removes the executing CPU, and possibly the whole cluster, from STANDBY
 *
 * It is entered with cluster->lock held.
 */
int appf_platform_leave_cstate1(unsigned cpu_index, struct appf_cpu *cpu, struct appf_cluster *cluster)
{
//    if (cluster->l2_address)
//    {
//        set_status_pl310(STATUS_RUN, cluster->l2_address);
//    }

    return APPF_OK;
}

