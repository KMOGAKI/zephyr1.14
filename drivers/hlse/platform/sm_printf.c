/*
 * Copyright 2016 NXP
 *
 * This software is owned or controlled by NXP and may only be used
 * strictly in accordance with the applicable license terms.  By expressly
 * accepting such terms or by downloading, installing, activating and/or
 * otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms.  If
 * you do not agree to be bound by the applicable license terms, then you
 * may not retain, install, activate or otherwise use the software.
 */

#include "sm_printf.h"

#define MAX_SER_BUF_SIZE 	(1024)

void sm_printf(uint8_t dev, const char * format, ...)
{
    uint8_t  buffer[MAX_SER_BUF_SIZE + 1];
    va_list   vArgs;

    dev = dev; // avoids warning; dev can be used to determine output channel

    va_start(vArgs, format);
    vsnprintk((char *)buffer, MAX_SER_BUF_SIZE, (char const *)format, vArgs);
    va_end(vArgs);

    PRINTF("%s", buffer);
}
