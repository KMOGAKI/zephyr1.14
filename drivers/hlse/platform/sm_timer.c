/**
* @file sm_timer.c
* @author NXP Semiconductors
* @version 1.0
* @par License
* Copyright 2017 NXP
*
* This software is owned or controlled by NXP and may only be used
* strictly in accordance with the applicable license terms.  By expressly
* accepting such terms or by downloading, installing, activating and/or
* otherwise using the software, you are agreeing that you have read, and
* that you agree to comply with and are bound by, such license terms.  If
* you do not agree to be bound by the applicable license terms, then you
* may not retain, install, activate or otherwise use the software.
*
* @par Description
* This file implements implements platform independent sleep functionality
* @par History
*
*****************************************************************************/

#include "sm_timer.h"

/* initializes the system tick counter
 * return 0 on succes, 1 on failure */
uint32_t sm_initSleep()
{
    return 0;
}

static void _udelay(s64_t microsec)
{
    u32_t start;

    start = k_cycle_get_32();
    k_busy_wait(microsec);
    microsec -= SYS_CLOCK_HW_CYCLES_TO_NS(k_cycle_get_32() - start) / 1000;
    if (microsec > 0)
        k_busy_wait(microsec);
}

/**
 * Implement a blocking (for the calling thread) wait for a number of milliseconds.
 */
void sm_sleep(uint32_t msec)
{
    _udelay(msec * 1000);
}

/**
 * Implement a blocking (for the calling thread) wait for a number of microseconds
 */
void sm_usleep(uint32_t microsec)
{
    _udelay(microsec);
}
