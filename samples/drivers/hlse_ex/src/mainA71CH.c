/***************************************************************************
 * Copyright 2016 NXP
 *
 * This software is owned or controlled by NXP and may only be used
 * strictly in accordance with the applicable license terms.  By expressly
 * accepting such terms or by downloading, installing, activating and/or
 * otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms.  If
 * you do not agree to be bound by the applicable license terms, then you
 * may not retain, install, activate or otherwise use the software.
 * **************************************************************************
 *
 *  Name: mainA71CH.c
 *
 *  This file contains the main entry for the host library example application
 *
 *  Define '#define WALKTHROUGH_ONLY' to limit the number of example functions
 *  invoked
 ****************************************************************************/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "tst_sm_time.h"

#include "a71ch_ex.h"
#include "ax_api.h"

#include "axHostCrypto.h"
#include "sm_timer.h"
#include "sm_printf.h"

#include "global_platf.h"
#include "app_boot.h"

// -----------------------------------
// Start of END-USER relevant defines
// #define WALKTHROUGH_ONLY
// End of END-USER relevant defines
// -----------------------------------

// ---------------------------------------------------------------------
//           Please do not change the following defines
// ---------------------------------------------------------------------

#define EX_APP_VERSION "1.30:1.31"  // App-version:Preferred-applet-version


//TODO: to be moved to common Header file later
#define PRINT_HOSTLIB_VER       (sm_printf(CONSOLE, "HostLib Version  : 0x%04X\n", commState.hostLibVersion))

#define PRINT_APPLET_VER        (sm_printf(CONSOLE, "Applet Version   : 0x%04X\n", commState.appletVersion))
#define PRINT_SECUREBOX_VER       (sm_printf(CONSOLE, "SecureBox Version: 0x%04X\n", commState.sbVersion))

#define SET_TIME(now)
#define TIME_TO_STRING(time)  ("-")


/*******************************************************************************
 **   Main Function  main()
 *******************************************************************************/
int main(int argc, char ** argv)
{
    U8 result = 1;
    int connectStatus = 0;
    SmCommState_t commState;
#ifndef WALKTHROUGH_ONLY
    U8 dbgTstMode = 0x00;
    // U8 dbgTstMode = EXTENDED_TEST | MEASURE_EXEC_TIME;
    U8 gpStorageTstMode = 0x00;
    // U8 gpStorageTstMode = EXTENDED_TEST;
#endif
    axTimeMeasurement_t execTime;
#ifdef TDA8029_UART
    Scp03SessionState_t sessionState;
#endif
    // int i = 0;

    app_boot_Init();

    sm_printf(DBGOUT, "a71ch HostLibrary example application (Rev %s)\n", EX_APP_VERSION);
    sm_printf(DBGOUT, "**********************************************\n");
    sm_printf(DBGOUT, "Connect to A71CH-SM. Chunksize at link layer = %d.\n", MAX_CHUNK_LENGTH_LINK);

    APP_BOOT_RESUME_HANDLING(exBoot);

    connectStatus = app_boot_Connect(&commState, NULL);

    if (connectStatus != 0) {
        sm_printf(CONSOLE, "Connection failed. SW = %d\n", connectStatus);
        return connectStatus;
    }

    initMeasurement(&execTime);
#ifdef WALKTHROUGH_ONLY
    result &= exWalkthrough(commState.appletVersion);
#else
    result &= exAes();
    result &= exConfig(commState.appletVersion);
    result &= exDebugMode(dbgTstMode);
    result &= exEccNohc();
    result &= exWalkthrough(commState.appletVersion);
    result &= exGPStorage(gpStorageTstMode);
    result &= exMisc();
    result &= exPsk(commState.appletVersion);
#ifndef NO_SECURE_CHANNEL_SUPPORT
    result &= exScp(commState.appletVersion);
    result &= exSst(commState.appletVersion);
    result &= exBoot(BOOT_SIMULATED_CYCLE);
    result &= exSstKp(commState.appletVersion);
#endif
#endif

#if defined(TDA8029_UART) || defined(I2C) || defined(PCSC) || defined(SPI) || defined(IPC)
LBL_REPORT_STATUS:
#endif
    concludeMeasurement(&execTime);
    SET_TIME(now);
    app_test_status(result);

    sm_printf(CONSOLE, "\nCompiled for Device Type %c\n", A71CH_DEVICE_TYPE);
    sm_printf(CONSOLE, "# Key Pairs = %d.\n", A71CH_KEY_PAIR_MAX);
    sm_printf(CONSOLE, "# Pub Key   = %d.\n", A71CH_PUBLIC_KEY_MAX);
    sm_printf(CONSOLE, "# Sym Key   = %d.\n", A71CH_SYM_KEY_MAX);
    sm_printf(CONSOLE, "Gp Storage  = %d byte\n", A71CH_GP_STORAGE_SIZE);
    if (commState.appletVersion < 0x0130)
    {
        sm_printf(CONSOLE, "Warning: Please switch to latest A71CH version, attached version is obsolete (attached version=0x%04X)\n", commState.appletVersion);
    }
    sm_printf(CONSOLE, "\n-----------\nExample Set A71CH finished (Rev %s) on 0x%04X, overall result = %s\n%sExec time: %ld ms\n------------\n",
        EX_APP_VERSION,
        commState.appletVersion,
        ((result == 1) ? "OK" : "FAILED"),
        TIME_TO_STRING(&now),
        getMeasurement(&execTime));

    return 0;
}
