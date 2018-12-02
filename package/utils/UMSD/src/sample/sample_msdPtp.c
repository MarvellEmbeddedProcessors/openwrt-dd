#include <msdCopyright.h>
/********************************************************************************
* sample_msdPtp.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Enable Port PTP.
*    2. Enable time stamp capture for IEEE 802.1as or IEEE 1588 PTP frame
*    3. Capture the arrival time and departure time.
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"
#include "time.h"

typedef struct {
	unsigned long long seconds;
	MSD_U32 nanoseconds;
}TimeInfo;
/*Suppose the ToD time is loaded at loadPoint time = Current Global time + TOD_LOAD_DELAY ns*/
#define TOD_LOAD_DELAY (2 * 1000000000)
/*Suppose the divice PTP module using the internal 125MHz clock, so the clock period is 8 ns*/
#define TAI_CLOCK_PERIOD 8

/********************************************************************************
* Sample #1:
*       enable port0 and port1 PTP feature and support the IEEE 802.1as frame time
*       capture both for arrival time and departure time
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdPtp(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_PTP_GLOBAL_CONFIG ptpGlobalCfg;
	MSD_PTP_PORT_CONFIG ptpPortCfg;
	MSD_PTP_TS_STATUS ptpStatus;
	MSD_PTP_PORT_INTERRUPT_EN ptpInt;
	MSD_LPORT port;
	MSD_QD_DEV *dev;
	MSD_U8 data;

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	msdMemSet(&ptpGlobalCfg, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));
	msdMemSet(&ptpPortCfg, 0, sizeof(MSD_PTP_PORT_CONFIG));
	ptpInt.ptpArrIntEn = MSD_TRUE;
	ptpInt.ptpDepIntEn = MSD_TRUE;

	/*Program PTPEType as 0x88F7, enable all PTP message type time stamp capture and using port Arr0 time register to capture*/
	ptpGlobalCfg.ptpEType = 0x88F7;
	ptpGlobalCfg.msgIdTSEn = 0xFFFF;
	ptpGlobalCfg.tsArrPtr = 0;
	if ((status = msdPtpGlobalConfigSet(devNum, &ptpGlobalCfg)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpGlobalConfigSet returned fail.\n"));
		return status;
	}

	for (port = 0; port < 2; port++)
	{
		/*Enable the PTP for port 0 and port 1*/
		if ((status = msdPtpPortPTPEnSet(devNum, port, MSD_TRUE)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortPTPEnSet returned fail.\n"));
			return status;
		}

		/*Enable the PTP interrupt for port 0 and port 1*/
		if ((status = msdPtpIntEnableSet(devNum, port, &ptpInt)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpIntEnableSet returned fail.\n"));
			return status;
		}

		/*Configure device to support 802.1as */
		if ((status = msdPtpPortConfigGet(devNum, port, &ptpPortCfg)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortConfigGet returned fail.\n"));
			return status;
		}
		ptpPortCfg.transSpec = 1;
		if ((status = msdPtpPortConfigSet(devNum, port, &ptpPortCfg)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortConfigSet returned fail.\n"));
			return status;
		}
	}

	/*Set the port1 as the PTP destination Port for Peridot*/
	if (dev->devName == MSD_PERIDOT)
	{
		if ((status = msdSysMonitorMgmtCtrlGet(devNum, 0x32, &data)) != MSD_OK)
		{
			MSG_PRINT(("msdSysMonitorMgmtCtrlGet returned fail.\n"));
			return status;
		}
		data = (data & 0xe0) | 0x1;
		if ((status = msdSysMonitorMgmtCtrlSet(devNum, 0x32, data)) != MSD_OK)
		{
			MSG_PRINT(("msdSysMonitorMgmtCtrlSet returned fail.\n"));
			return status;
		}
	}

	/**************************************************************************************
	*   After this setup, if one PTP frame entered to port 0 and been forward to port 1   *
	*   the arrival time stamp be captured by the PTP logic will be indicated in port 0   *
	*   PTPArr0Time and the departure time stamp will indicated in port 1 PTPDepTime      *
	**************************************************************************************/

	/*Get port0 Arr0 ptp status*/
	if ((status = msdPtpTimeStampGet(devNum, 0, MSD_PTP_ARR0_TIME, &ptpStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpTimeStampGet returned fail.\n"));
		return status;
	}
	MSG_PRINT(("PTPArr0Time Valid: %d\n", ptpStatus.isValid));
	MSG_PRINT(("PTPArr0Time SeqId: %d\n", ptpStatus.ptpSeqId));
	MSG_PRINT(("PTPArr0Time Stamp: 0x%x\n", ptpStatus.timeStamped));

	/*Get port1 Departure ptp status*/
	if ((status = msdPtpTimeStampGet(devNum, 1, MSD_PTP_DEP_TIME, &ptpStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpTimeStampGet returned fail.\n"));
		return status;
	}
	MSG_PRINT(("PTPDepTime Valid: %d\n", ptpStatus.isValid));
	MSG_PRINT(("PTPDepTime SeqId: %d\n", ptpStatus.ptpSeqId));
	MSG_PRINT(("PTPDepTime Stamp: 0x%x\n", ptpStatus.timeStamped));

	MSG_PRINT(("sample_msdPtp success.\n"));
	return MSD_OK;
}

/**************************************************************************************
*                                                                                     *
*              Following Sample only supported by Peridot                             *
*                                                                                     *
**************************************************************************************/
/********************************************************************************
* setCurrentTime
*
*	This sample API will demonstrate how to load one time array to device:
*	Suppose the divice PTP module using the internal 125MHz clock, so the Global Time is incremented of 8ns
*	The time array load occurs at the instant the PTP Global Time matched the load point time
*
*******************************************************************************/
static MSD_STATUS setCurrentTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, MSD_U8 domainNumber, TimeInfo *timeinfo)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;
	TimeInfo temptime;

	temptime.nanoseconds = (timeinfo->nanoseconds + TOD_LOAD_DELAY) % 1000000000;
	temptime.seconds = timeinfo->seconds + (timeinfo->nanoseconds + TOD_LOAD_DELAY) / 1000000000;

	/*Initialise the time array*/
	timeArray.clkValid = MSD_TRUE;
	timeArray.domainNumber = domainNumber;
	timeArray.todSecondsHigh = (temptime.seconds >> 32) & 0xffff;
	timeArray.todSecondsLow = temptime.seconds & 0xffffffff;
	timeArray.todNanoseconds = temptime.nanoseconds;
	timeArray.Nanoseconds1722High = 0;
	timeArray.Nanoseconds1722Low = 0;
	timeArray.todCompensation = 0;
	/*Get the current Global time and add a delay for loadPoint, the value is incremented onece by 8n, so the loadPoint value = TOD_LOAD_DELAY / TAI_CLOCK_PERIOD*/
	msdGetPTPGlobalTime(devNum, &timeArray.todLoadPoint);
	timeArray.todLoadPoint = timeArray.todLoadPoint + TOD_LOAD_DELAY / TAI_CLOCK_PERIOD;

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	return status;
}
/********************************************************************************
* getCurrentTime
*
*	This sample API will demonstrate how to get time array form device:
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*
*******************************************************************************/
static MSD_STATUS getCurrentTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, TimeInfo *timeinfo)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODCaptureAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	timeinfo->nanoseconds = timeArray.todNanoseconds;
	timeinfo->seconds = timeArray.todSecondsHigh;
	timeinfo->seconds = (timeinfo->seconds << 32) | timeArray.todSecondsLow;

	return status;
}
/********************************************************************************
* adjustTime
*
*	This sample API will demonstrate how to adjust device time of day based on master's time:
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*	timeoffset - nanoseconds based
*	direction - 0 = Local time is slow than master time
*				1 = Local time is fast than master time
*
*******************************************************************************/
static MSD_STATUS adjustTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, MSD_BOOL direction, unsigned long long timeoffset)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;
	unsigned long long temp;

	///////*Adjust the ToD based on the delta*//////
	/*Get current ToD firstly*/
	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODCaptureAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	timeArray.todLoadPoint = timeArray.todLoadPoint + TOD_LOAD_DELAY / TAI_CLOCK_PERIOD;
	temp = timeArray.todSecondsHigh;
	if (direction == 0)
		temp = (temp << 32 | timeArray.todSecondsLow) * 1000000000 + timeArray.todNanoseconds + timeoffset + TOD_LOAD_DELAY;
	else
		temp = (temp << 32 | timeArray.todSecondsLow) * 1000000000 + timeArray.todNanoseconds - timeoffset + TOD_LOAD_DELAY;

	timeArray.todNanoseconds = temp % 1000000000;
	timeArray.todSecondsHigh = ((temp / 1000000000) >> 32) & 0xffffffff;
	timeArray.todSecondsLow = (temp / 1000000000) & 0xffffffff;

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	return status;
}
/********************************************************************************
* adjustFrequency
*
*	This sample API will demonstrate how to adjust PPM difference between the local crystal clocking this PTP block and the PTP Grand Master device that this Time Array is tracking
*	Suppose the divice PTP module using the internal 125MHz clock
*	The compensation register is incremented of 465.661 zeptoseconds(4.65661E-19 seconds). A difference of 1 PPM for a 125 MHz local PTP clock is 8 femtoseconds(8.0E-15 seconds)
*	And the value of compensation register should be 0x431E
*	direction - 0 = Local clock is slow than master clock
*				1 = Local clock is fast than master clock
*
*******************************************************************************/
static MSD_STATUS adjustFrequency(MSD_U8 devNum, MSD_U8 timeArrayIndex, MSD_BOOL direction, MSD_U32 ppm)
{
	MSD_STATUS status = MSD_OK;
	MSD_BOOL busyStatus;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_U32 temp;

	temp = ppm * 0x431E >= 0x80000000 ? 0x7fffffff : ppm * 0x431E;
	timeArray.clkValid = MSD_TRUE;
	if (direction == 0)
	{
		timeArray.todCompensation = temp;
	}
	else
	{
		timeArray.todCompensation = 0x80000000 | temp;
	}

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreCompensation(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	return status;
}
/********************************************************************************
* becomeMaster
*
*	This sample API will set the device to Master
*
*******************************************************************************/

MSD_STATUS becomeMaster(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_MASTER_SLAVE role = MSD_PTP_ROLE_MASTER;

	if ((status = msdPtpMasterSlaveSet(devNum, role)) != MSD_OK)
	{
		MSG_PRINT(("Set device to Master error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* becomeSlave
*
*	This sample API will set the device to Slave
*
*******************************************************************************/
MSD_STATUS becomeSlave(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_MASTER_SLAVE role = MSD_PTP_ROLE_SLAVE;

	if ((status = msdPtpMasterSlaveSet(devNum, role)) != MSD_OK)
	{
		MSG_PRINT(("Set device to Slave error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* ptpSetCurTime
*
*	This sample API will demonstrate how to set current ToD (timeofday) in domain 0
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*	1. Get the current time, this sample using the c function time() to get the system time
*	2. Initialise timeArrayIndex and domainNumber as 0 for sample
*	3. Store the time array to device
*
*******************************************************************************/
MSD_STATUS ptpSetCurTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	MSD_U8 domainNumber;
	time_t rawtime;
	TimeInfo timeinfo;

	phyAddr = 0;
	timeArrayIndex = 0;
	domainNumber = 0;
	time(&rawtime);
	timeinfo.seconds = rawtime;
	timeinfo.nanoseconds = 0;

	status = setCurrentTime(devNum, timeArrayIndex, domainNumber, &timeinfo);

	return status;
}

/********************************************************************************
* ptpGetCurTime
*
*	This sample API will demonstrate how to get current ToD (timeofday) in domain 0
*
*******************************************************************************/
MSD_STATUS ptpGetCurTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	TimeInfo timeinfo;
	struct tm *ptime;

	phyAddr = 0;
	timeArrayIndex = 0;

	if ((status = getCurrentTime(devNum, timeArrayIndex, &timeinfo)) != MSD_OK)
		return status;
	ptime = localtime(&timeinfo.seconds);
	MSG_PRINT(("ToD time: %d-%d-%d %d:%d:%d  %d ns\n", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday,
		ptime->tm_hour, ptime->tm_min, ptime->tm_sec, timeinfo.nanoseconds));
	return status;
}

/********************************************************************************
* ptpAdjTime
*
*	This sample API will demonstrate how to adjust the ToD
*	This sample using the c function time() to get the system time, real system should get the time form grand master
*	Suppose our device time is slower than master time with offset = 1 second
*
*******************************************************************************/
MSD_STATUS ptpAdjTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	MSD_BOOL direction;
	unsigned long long offset;

	timeArrayIndex = 0;
	phyAddr = 0;
	direction = 0;
	offset = 1000000000;

	status = adjustTime(devNum, timeArrayIndex, direction, offset);

	return status;
}

/********************************************************************************
* ptpAdjFreq
*
*	This sample API will demonstrate how to adjust the PPM difference between the local crystal clocking this PTP block and the PTP Grand Master device that this Time Array is tracking
*	Suppose our device clock is faster than master clock with 20 PPM difference
*******************************************************************************/
MSD_STATUS ptpAdjFreq(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	int ppm, direction;

	timeArrayIndex = 0;
	phyAddr = 0;
	direction = 1;
	ppm = 20;

	status = adjustFrequency(devNum, timeArrayIndex, direction, ppm);

	return status;
}

/********************************************************************************
* ptpGen1PPS
*
*	This sample API will demonstrate how to generate one pulse per second based on time array 0
*	   Configure device to generate one pulse per second
*	   Configure the 1PPS width as 0xf and width Range as 0x7, so the pulse width is 15*16.777ms = 251.655ms
*	   Select the time array 0, make sure the time array 0 is actived
*	   The selected 1 PPS signal is available on GPIO pin
*
*******************************************************************************/
MSD_STATUS ptpGen1PPS(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_PULSE_STRUCT pulsePara;

	/*Configure device to generate one pulse per second*/
	pulsePara.ptpPulseWidth = 0xf;
	pulsePara.ptp1ppsWidthRange = 0x7;
	pulsePara.ptp1ppsPhase = 0; /*the leading edge of the 1 PPS signal is the rising edge of the signal*/
	pulsePara.ptp1ppsSelect = 0;
	if ((status = msdPtpPulseSet(devNum, &pulsePara)) != MSD_OK)
	{
		MSG_PRINT(("Configure 1PPS error, error code: %d\n", status));
		return status;
	}

	return status;
}
