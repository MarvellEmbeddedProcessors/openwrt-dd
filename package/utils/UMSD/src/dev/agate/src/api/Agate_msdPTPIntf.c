#include <msdCopyright.h>

/******************************************************************************
* Agate_msdPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <agate/include/api/Agate_msdPTP.h>

MSD_STATUS Agate_gptpSetIntEnableIntf
(
IN MSD_QD_DEV* dev,
IN MSD_LPORT port,
IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
	AGATE_PTP_PORT_INTERRUPT_EN tempIntEn;

	if (interruptEn == NULL)
	{
		MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Agate_gptpSetIntEnableIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
	tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

	return Agate_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Agate_gptpGetIntEnableIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_LPORT port,
OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
	MSD_STATUS retVal;
	AGATE_PTP_PORT_INTERRUPT_EN tempIntEn;

	if (interruptEn == NULL)
	{
		MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Agate_gptpGetIntEnableIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	retVal = Agate_gptpGetIntEnable(dev, port, &tempIntEn);
	if (MSD_OK != retVal)
		return retVal;

	interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
	interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

	return retVal;
}

MSD_STATUS Agate_gptpSetPortConfigIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_LPORT port,
IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
	AGATE_PTP_PORT_CONFIG tempData;

	if (ptpData == NULL)
	{
		MSD_DBG_ERROR(("Input param PTP_PORT_CONFIG in Agate_gptpSetPortConfigIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tempData.arrLedCtrl = ptpData->arrLedCtrl;
	tempData.arrTSMode = ptpData->arrTSMode;
	tempData.depLedCtrl = ptpData->depLedCtrl;
	tempData.disTSOverwrite = ptpData->disTSOverwrite;
	tempData.disTSpec = ptpData->disTSpec;
	tempData.etJump = ptpData->etJump;
	tempData.ipJump = ptpData->ipJump;
	tempData.ptpArrIntEn = ptpData->ptpArrIntEn;
	tempData.ptpDepIntEn = ptpData->ptpDepIntEn;
	tempData.transSpec = ptpData->transSpec;

	return Agate_gptpSetPortConfig(dev, port, &tempData);
}

MSD_STATUS Agate_gptpGetPortConfigIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_LPORT port,
OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
	MSD_STATUS retVal;
	AGATE_PTP_PORT_CONFIG tempData;

	if (ptpData == NULL)
	{
		MSD_DBG_ERROR(("param PTP_PORT_CONFIG in Agate_gptpGetPortConfigIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	retVal = Agate_gptpGetPortConfig(dev, port, &tempData);
	if (retVal != MSD_OK)
		return retVal;

	ptpData->arrLedCtrl = tempData.arrLedCtrl;
	ptpData->arrTSMode = tempData.arrTSMode;
	ptpData->depLedCtrl = tempData.depLedCtrl;
	ptpData->disTSOverwrite = tempData.disTSOverwrite;
	ptpData->disTSpec = tempData.disTSpec;
	ptpData->etJump = tempData.etJump;	
	ptpData->ipJump = tempData.ipJump;
	ptpData->ptpArrIntEn = tempData.ptpArrIntEn;
	ptpData->ptpDepIntEn = tempData.ptpDepIntEn;
	ptpData->transSpec = tempData.transSpec;

	return retVal;
}

MSD_STATUS Agate_gptpGetTimeStampIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_LPORT    port,
IN  MSD_PTP_TIME    timeToRead,
OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
	MSD_STATUS retVal;
	AGATE_PTP_TS_STATUS tempStatus;

	if (ptpStatus == NULL)
	{
		MSD_DBG_ERROR(("param PTP_TS_STATUS in Agate_gptpGetTimeStampIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	retVal = Agate_gptpGetTimeStamp(dev, port, (AGATE_PTP_TIME)timeToRead, &tempStatus);
	if (retVal != MSD_OK)
		return retVal;

	ptpStatus->isValid = tempStatus.isValid;
	ptpStatus->ptpSeqId = tempStatus.ptpSeqId;
	ptpStatus->status = tempStatus.status;
	ptpStatus->timeStamped = tempStatus.timeStamped;

	return retVal;
}

MSD_STATUS Agate_gptpSetGlobalConfigIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
	AGATE_PTP_GLOBAL_CONFIG tempData;

	if (ptpData == NULL)
	{
		MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Agate_gptpSetGlobalConfigIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tempData.msgIdTSEn = ptpData->msgIdTSEn;
	tempData.ptpEType = ptpData->ptpEType;
	tempData.tsArrPtr = ptpData->tsArrPtr;

	return Agate_gptpSetGlobalConfig(dev, &tempData);
}

MSD_STATUS Agate_gptpGetGlobalConfigIntf
(
IN  MSD_QD_DEV* dev,
OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
	MSD_STATUS retVal;
	AGATE_PTP_GLOBAL_CONFIG tempData;

	if (ptpData == NULL)
	{
		MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Agate_gptpGetGlobalConfigIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	retVal = Agate_gptpGetGlobalConfig(dev, &tempData);
	if (retVal != MSD_OK)
		return retVal;

	ptpData->msgIdTSEn = tempData.msgIdTSEn;
	ptpData->ptpEType = tempData.ptpEType;
	ptpData->tsArrPtr = tempData.tsArrPtr;

	return retVal;
}

MSD_STATUS Agate_gptpGetIntStatusIntf
(
IN  MSD_QD_DEV* dev,
OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
	MSD_STATUS retVal;
	AGATE_PTP_INTERRUPT_STATUS tempIntSt;

	if (ptpIntSt == NULL)
	{
		MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Agate_gptpGetIntStatusIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	retVal = Agate_gptpGetIntStatus(dev, &tempIntSt);
	if (retVal != MSD_OK)
		return retVal;

	ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
	ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
	ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

	return retVal;
}