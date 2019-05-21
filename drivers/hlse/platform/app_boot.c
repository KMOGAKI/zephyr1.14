/**
 * @file app_boot.c
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
 * Implementation of the App booting time initilization functions
 */

#include "app_boot.h"

#include "ax_api.h"
#include "global_platf.h"

#include "sm_printf.h"
#include "sm_timer.h"

static U16 establishConnnection(
    SmCommState_t* pCommState, const char* pConnectionParam);
static int translateCommunicationStatus(
    U16 connectStatus);

int app_boot_Init()
{
    return 0;
}

int app_boot_Init_RTOS()
{
    sm_initSleep();

    return 0;
}

int app_boot_Connect(
    SmCommState_t* pCommState, const char* pConnectionParam)
{
    int retVal;

    U16 comStatus = establishConnnection(pCommState, pConnectionParam);
    retVal = translateCommunicationStatus(comStatus);

    if (retVal == 0) {
#if defined(I2C)
        PRINTF("SCI2C_"); // To highlight the ATR format for SCI2C deviates from ISO7816-3
#elif defined(SPI)
        PRINTF("SCSPI_");
#endif

#if defined(TDA8029_UART)
        PRINTF("UART Baudrate Idx: 0x%02X\n", pCommState->param2);
        PRINTF("T=1           TA1: 0x%02X\n", pCommState->param1);
#endif
        PRINTF("HostLib Version  : 0x%04X\n", pCommState->hostLibVersion);

        PRINTF("Applet Version   : 0x%04X\n", pCommState->appletVersion);
        PRINTF("SecureBox Version: 0x%04X\n", pCommState->sbVersion);

        PRINTF("\n");
        PRINTF("==========SELECT-DONE=========\n");
    }
    else
    {
        PRINTF("\n");
        PRINTF("==========SELECT-FAILED=========\n");
    }

#ifdef TARGET_PLATFORM
    PRINTF("Platform: %s\n",TARGET_PLATFORM);
#endif

    return retVal;
}

/**
 * Local static function to handle communication status
 * @param[in] connectStatus The status of the communication.
 * @return returns the legacy error code
 */

static int translateCommunicationStatus(
    U16 connectStatus)
{
    if ((connectStatus == ERR_CONNECT_LINK_FAILED) || (connectStatus == ERR_CONNECT_SELECT_FAILED))
    {
        PRINTF("SM_Connect failed with status 0x%04X\n", connectStatus);
        return 2;
    }
    else if (connectStatus == SMCOM_COM_FAILED)
    {
        PRINTF("SM_Connect failed with status 0x%04X (Could not open communication channel)\n",
            connectStatus);
        return 4;
    }
    else if (connectStatus == SMCOM_PROTOCOL_FAILED)
    {
        PRINTF("SM_Connect failed with status 0x%04X (Could not establish communication protocol)\n",
            connectStatus);
        return 5;
    }
    else if (connectStatus == ERR_NO_VALID_IP_PORT_PATTERN)
    {
        PRINTF("Pass the IP address and port number as arguments, e.g. \"127.0.0.1:8050\"!\n");
        return 3;
    }
    else if (connectStatus != SW_OK)
    {
        // return error code asis to handle AX layer errors
        return connectStatus;
    }
    else
    {
        return 0;
    }
}
/**
 * Local static function to establish a connection based on the communication port selected
 * @param[in] pCommState pointer where the communication state is updated.
 * @param[in] argc varibale length arg count based on the communication channel used.
 * @param[in] argv variable arguments based on the communication channel used.
 * @return returns the status of connection establishment
 */
static U16 establishConnnection(
    SmCommState_t* pCommState, const char * pConnectionParam)
{
    U16 connectStatus = 0;

    U8 Atr[64];
    U16 AtrLen = sizeof(Atr);

#if defined(TDA8029_UART) || defined(I2C) || defined(PCSC) || defined(SPI) || defined(IPC)
    connectStatus = SM_Connect(pCommState, Atr, &AtrLen);
#elif defined(RJCT_SOCKET) || defined(RJCT_JRCP)
    if (pConnectionParam == NULL)
    {
        PRINTF("Pass the IP address and port number as arguments, e.g. \"127.0.0.1:8050\"!\n");
        return 4;
    }
    else
    {
        connectStatus = SM_RjctConnect(pConnectionParam, pCommState, Atr, &AtrLen);
    }
#elif defined(RJCT_VCOM)
    if ( pConnectionParam == NULL )
    {
        PRINTF("Pass the COM Port as arguments, e.g. \"COM3\" or \"\\\\.\\COM18\" !\n");
        return 4;
    }
    else
    {
        connectStatus = SM_RjctConnect(pConnectionParam,pCommState, Atr, &AtrLen);
    }
#else
#error "No communication channel defined"
#endif // RJCT_SOCKET

    if (AtrLen > 0)
    {
        int i = 0;
        PRINTF("ATR=0x");
        for (i = 0; i < AtrLen; i++)
        {
            PRINTF("%02X.", Atr[i]);
        }
        PRINTF("\n");
    }

#if defined(TDA8029_UART)
    PRINTF("UART Baudrate Idx: 0x%02X\n", pCommState->param2);
    PRINTF("T=1           TA1: 0x%02X\n", pCommState->param1);
#endif

    return connectStatus;
}

void app_test_status(U8 result) {
    PRINTF("END. Status %s (Compiled on %s %s)\n",
            ((result == 1) ? "OK" : "FAILED"),
            __DATE__, __TIME__);
}
