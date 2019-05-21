/**
 * @file tst_sm_time_kinetis.c
 * @author NXP Semiconductors
 * @version 1.0
 * @par License
 * Copyright 2017,2018 NXP
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
 * Utility functions for measurement of test execution time
 *
 */

#include "tst_sm_time.h"
#include <stdint.h>

long getMeasurement(
    axTimeMeasurement_t *mPair)
{
	return (long)mPair->tDelta;
}

void initMeasurement(
    axTimeMeasurement_t *mPair)
{
	mPair->tDelta = 0;
	mPair->tStart = k_uptime_get();
}

void concludeMeasurement(
    axTimeMeasurement_t *mPair)
{
	mPair->tDelta = k_uptime_delta(&mPair->tStart);
}


void axSummarizeMeasurement(
    void *fHandle_NotUsed, char *szMessage, long *msArray, int nMeasurement, int reportMode)
{
	int i;
	long averaged = 0;
	long minValue = 0;
	long maxValue = 0;
	//int fEchoStdout = 0;
	//int fReportVerbose = 0;
	//int nOut;

	//fEchoStdout = ((reportMode & AX_MEASURE_ECHO_MASK) == AX_MEASURE_ECHO_STDOUT);
	//fReportVerbose = ((reportMode & AX_MEASURE_REPORT_MASK) == AX_MEASURE_REPORT_VERBOSE);

	if (nMeasurement > 0)
	{
		minValue = msArray[0];
		maxValue = msArray[0];
	}
	else
	{
		printk("%s: No valid amount of measurements (%d)\n", szMessage, nMeasurement);
		return;
	}

	for (i=0; i<nMeasurement; i++)
	{
		//printk("%s: %ld ms\n", szMessage, msArray[i]);
		averaged += msArray[i];
		minValue = (msArray[i] < minValue) ? msArray[i] : minValue;
		maxValue = (msArray[i] > maxValue) ? msArray[i] : maxValue;
	}
	averaged /= nMeasurement;

	printk("Exec Time: %s:\n\tAverage (%d measurements): %ld ms\n", szMessage, nMeasurement, averaged);
	printk("\tMinimum: %ld ms\n", minValue);
	printk("\tMaximum: %ld ms\n", maxValue);
	return;
}
