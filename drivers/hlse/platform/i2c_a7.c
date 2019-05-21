/**
 * @file i2c_a7.c
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
 * i.MX6UL board specific i2c code
 * @par History
 *
 **/
#include "i2c_a7.h"
#include "sm_timer.h"

#include <i2c.h>

// #define LOG_I2C 1

#define TRY_NUM 500
#define RETRY_DELAY_US 200

static struct device *axSmDevice;
static int axSmDevice_addr = 0x48;      // 7-bit address
static char devName[] = "BITBANG_I2C_0";        // Change this when connecting to another host i2c master portstatic char devName[] = "BITBANG_I2C_0";        // Change this when connecting to another host i2c master port

/**
* Opens the communication channel to I2C device
*/
i2c_error_t axI2CInit()
{
    printk("I2CInit: opening %s\n", devName);

    if (!(axSmDevice = device_get_binding(devName)))
    {
        printk("opening failed...\n");
        return I2C_FAILED;
    }

    if (i2c_configure(axSmDevice, I2C_SPEED_SET(I2C_SPEED_FAST)) < 0)
    {
        printk("I2C driver failed configure\n");
    }

    return I2C_OK;
}

/**
* Closes the communication channel to I2C device (not implemented)
*/
void axI2CTerm(int mode)
{
    AX_UNUSED_ARG(mode);
    printk("axI2CTerm: not implemented.\n");
    return;
}

/**
 * Write a single byte to the slave device.
 * In the context of the SCI2C protocol, this command is only invoked
 * to trigger a wake-up of the attached secure module. As such this
 * wakeup command 'wakes' the device, but does not receive a valid response.
 * \note \par bus is currently not used to distinguish between I2C masters.
*/
i2c_error_t axI2CWriteByte(unsigned char bus, unsigned char addr, unsigned char *pTx)
{
    int r = 0;
    i2c_error_t rv;
#ifdef TRY_NUM
    int remain = TRY_NUM;
#endif

    if (bus != I2C_BUS_0)
    {
        printk("axI2CWriteByte on wrong bus %x (addr %x)\n", bus, addr);
    }

#ifdef TRY_NUM
retry_byte:
#endif
    r = i2c_write(axSmDevice, pTx, 1, axSmDevice_addr);
    if (r < 0)
    {
#ifdef TRY_NUM
        if (remain--) {
#ifdef RETRY_DELAY_US
            sm_usleep(RETRY_DELAY_US);
#endif
            goto retry_byte;
        }
#endif
        printk("Failed writing byte data (r=%d).\n", r);
        rv = I2C_FAILED;
    }
    else
    {
        rv = I2C_OK;
    }

    return rv;
}

i2c_error_t axI2CWrite(unsigned char bus, unsigned char addr, unsigned char * pTx, unsigned short txLen)
{
    int r = 0;
    i2c_error_t rv;
#ifdef LOG_I2C
    int i = 0;
#endif
#ifdef TRY_NUM
    int remain = TRY_NUM;
#endif

    if (bus != I2C_BUS_0)
    {
        printk("axI2CWrite on wrong bus %x (addr %x)\n", bus, addr);
    }
#ifdef LOG_I2C
    printk("TX (axI2CWrite): ");
    for (i = 0; i < txLen; i++)
    {
        printk("%02X ", pTx[i]);
    }
    printk("\n");
#endif

#ifdef TRY_NUM
retry:
#endif
    r = i2c_write(axSmDevice, pTx, txLen, axSmDevice_addr);
    if (r < 0)
    {
#ifdef TRY_NUM
        if (remain--) {
#ifdef RETRY_DELAY_US
            sm_usleep(RETRY_DELAY_US);
#endif
            goto retry;
        }
#endif
        printk("Failed writing data (r=%d).\n", r);
        rv = I2C_FAILED;
    }
    else
    {
        rv = I2C_OK;
    }
#ifdef LOG_I2C
    printk("    Done with rv = %02x ", rv);
    printk("\n");
#endif

   return rv;
}

i2c_error_t axI2CWriteRead(unsigned char bus, unsigned char addr, unsigned char * pTx,
      unsigned short txLen, unsigned char * pRx, unsigned short * pRxLen)
{
    struct i2c_msg messages[2];
    int r = 0;
#ifdef LOG_I2C
    int i = 0;
#endif

    if (bus != I2C_BUS_0) // change if bus 0 is not the correct bus
    {
        printk("axI2CWriteRead on wrong bus %x (addr %x)\n", bus, addr);
    }

    messages[0].flags = I2C_MSG_WRITE;
    messages[0].len   = txLen;
    messages[0].buf   = pTx;

    messages[1].flags = I2C_MSG_READ | I2C_MSG_RECV_LEN | I2C_MSG_STOP;
    messages[1].len   = 256;
    messages[1].buf   = pRx;
    messages[1].buf[0] = 1;

#ifdef LOG_I2C
    printk("TX (%d byte): ", txLen);
    for (i = 0; i < txLen; i++)
    {
        printk("%02X ", messages[0].buf[i]);
    }
    printk("\n");
#endif

    // Send the request to the kernel and get the result back
    r = i2c_transfer(axSmDevice, &messages[0], 2, axSmDevice_addr);

    if (r < 0)
    {
#ifdef LOG_I2C
        printk("axI2CWriteRead: i2c_transfer fails with value %d (hex: 0x%08X)\n", r, r);
#endif
        // printk("axI2CWriteRead: i2c_transfer value %d (hex: 0x%08X)\n", r, r);
        return I2C_FAILED;
    }
    else
    {
        int rlen = pRx[0]+1;

        // printk("messages[1].len is %d \n", messages[1].len);
#ifdef LOG_I2C
        printk("RX  (%d): ", rlen);
        for (i = 0; i < rlen; i++)
        {
            printk("%02X ", pRx[i]);
        }
        printk("\n");
#endif
        *pRxLen = rlen;
    }

    return I2C_OK;
}
