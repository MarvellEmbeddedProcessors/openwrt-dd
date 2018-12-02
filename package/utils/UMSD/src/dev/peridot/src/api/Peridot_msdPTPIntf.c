#include <msdCopyright.h>

/******************************************************************************
* Peridot_msdPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <peridot/include/api/Peridot_msdPTP.h>

MSD_STATUS Peridot_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    PERIDOT_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Peridot_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return Peridot_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Peridot_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Peridot_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
        return retVal;

    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS Peridot_gptpSetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
    PERIDOT_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_CONFIG in Peridot_gptpSetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.arrLedCtrl = ptpData->arrLedCtrl;
    tempData.arrTSMode = ptpData->arrTSMode;
    tempData.depLedCtrl = ptpData->depLedCtrl;
    tempData.disTSOverwrite = ptpData->disTSOverwrite;
    tempData.disTSpec = ptpData->disTSpec;
    tempData.etJump = ptpData->etJump;
    tempData.filterAct = ptpData->filterAct;
    tempData.hwAccPtp = ptpData->hwAccPtp;
    tempData.ExthwAcc = ptpData->ExthwAcc;
    tempData.ipJump = ptpData->ipJump;
    tempData.kpFramwSa = ptpData->kpFramwSa;
    tempData.ptpArrIntEn = ptpData->ptpArrIntEn;
    tempData.ptpDepIntEn = ptpData->ptpDepIntEn;
    tempData.transSpec = ptpData->transSpec;

    return Peridot_gptpSetPortConfig(dev, port, &tempData);
}

MSD_STATUS Peridot_gptpGetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_CONFIG in Peridot_gptpGetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetPortConfig(dev, port, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->arrLedCtrl = tempData.arrLedCtrl;
    ptpData->arrTSMode = tempData.arrTSMode;
    ptpData->depLedCtrl = tempData.depLedCtrl;
    ptpData->disTSOverwrite = tempData.disTSOverwrite;
    ptpData->disTSpec = tempData.disTSpec;
    ptpData->etJump = tempData.etJump;
    ptpData->filterAct = tempData.filterAct;
    ptpData->hwAccPtp = tempData.hwAccPtp;
    ptpData->ExthwAcc = tempData.ExthwAcc;
    ptpData->ipJump = tempData.ipJump;
    ptpData->kpFramwSa = tempData.kpFramwSa;
    ptpData->ptpArrIntEn = tempData.ptpArrIntEn;
    ptpData->ptpDepIntEn = tempData.ptpDepIntEn;
    ptpData->transSpec = tempData.transSpec;

    return retVal;
}

MSD_STATUS Peridot_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in Peridot_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetTimeStamp(dev, port, (PERIDOT_PTP_TIME)timeToRead, &tempStatus);
    if (retVal != MSD_OK)
        return retVal;

    ptpStatus->isValid = tempStatus.isValid;
    ptpStatus->ptpSeqId = tempStatus.ptpSeqId;
    ptpStatus->status = tempStatus.status;
    ptpStatus->timeStamped = tempStatus.timeStamped;

    return retVal;
}

MSD_STATUS Peridot_gptpSetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    PERIDOT_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Peridot_gptpSetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.msgIdTSEn = ptpData->msgIdTSEn;
    tempData.ptpEType = ptpData->ptpEType;
    tempData.tsArrPtr = ptpData->tsArrPtr;

    return Peridot_gptpSetGlobalConfig(dev, &tempData);
}

MSD_STATUS Peridot_gptpGetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Peridot_gptpGetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetGlobalConfig(dev, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->msgIdTSEn = tempData.msgIdTSEn;
    ptpData->ptpEType = tempData.ptpEType;
    ptpData->tsArrPtr = tempData.tsArrPtr;

    return retVal;
}

MSD_STATUS Peridot_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Peridot_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
        return retVal;

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}

MSD_STATUS Peridot_gptpSetModeIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_MODE_CONFIG tempMode;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("param PTP_MODE_CONFIG in Peridot_gptpSetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempMode.altScheme = mode->altScheme;
    tempMode.grandMstr = mode->grandMstr;
    tempMode.oneStep = mode->oneStep;
    tempMode.PTPMode = mode->PTPMode;

    retVal = Peridot_gptpSetMode(dev, &tempMode);

    return retVal;
}

MSD_STATUS Peridot_gptpGetModeIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_MODE_CONFIG tempMode;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("param PTP_MODE_CONFIG in Peridot_gptpGetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetMode(dev, &tempMode);
    if (retVal != MSD_OK)
        return retVal;

    mode->altScheme = tempMode.altScheme;
    mode->grandMstr = tempMode.grandMstr;
    mode->oneStep = tempMode.oneStep;
    mode->PTPMode = tempMode.PTPMode;

    return retVal;
}

MSD_STATUS Peridot_gptpSetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE role
)
{
    return Peridot_gptpSetMasterSlave(dev, (PERIDOT_PTP_MASTER_SLAVE)role);
}

MSD_STATUS Peridot_gptpGetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE *role
)
{
    return Peridot_gptpGetMasterSlave(dev, (PERIDOT_PTP_MASTER_SLAVE*)role);
}

MSD_STATUS Peridot_gptpTODStoreCompensationIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    PERIDOT_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Peridot_gptpTODStoreCompensationIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpArray.clkValid = timeArray->clkValid;
    tmpArray.domainNumber = timeArray->domainNumber;
    tmpArray.Nanoseconds1722High = timeArray->Nanoseconds1722High;
    tmpArray.Nanoseconds1722Low = timeArray->Nanoseconds1722Low;
    tmpArray.todCompensation = timeArray->todCompensation;
    tmpArray.todLoadPoint = timeArray->todLoadPoint;
    tmpArray.todNanoseconds = timeArray->todNanoseconds;
    tmpArray.todSecondsHigh = timeArray->todSecondsHigh;
    tmpArray.todSecondsLow = timeArray->todSecondsLow;
        
    return Peridot_gptpTODStoreCompensation(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Peridot_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    PERIDOT_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Peridot_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpArray.clkValid = timeArray->clkValid;
    tmpArray.domainNumber = timeArray->domainNumber;
    tmpArray.Nanoseconds1722High = timeArray->Nanoseconds1722High;
    tmpArray.Nanoseconds1722Low = timeArray->Nanoseconds1722Low;
    tmpArray.todCompensation = timeArray->todCompensation;
    tmpArray.todLoadPoint = timeArray->todLoadPoint;
    tmpArray.todNanoseconds = timeArray->todNanoseconds;
    tmpArray.todSecondsHigh = timeArray->todSecondsHigh;
    tmpArray.todSecondsLow = timeArray->todSecondsLow;

    return Peridot_gptpTODStoreAll(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Peridot_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Peridot_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpTODCaptureAll(dev, timeArrayIndex, &tmpArray);
    if (retVal != MSD_OK)
        return retVal;

    timeArray->clkValid = tmpArray.clkValid;
    timeArray->domainNumber = tmpArray.domainNumber;
    timeArray->Nanoseconds1722High = tmpArray.Nanoseconds1722High;
    timeArray->Nanoseconds1722Low = tmpArray.Nanoseconds1722Low;
    timeArray->todCompensation = tmpArray.todCompensation;
    timeArray->todLoadPoint = tmpArray.todLoadPoint;
    timeArray->todNanoseconds = tmpArray.todNanoseconds;
    timeArray->todSecondsHigh = tmpArray.todSecondsHigh;
    timeArray->todSecondsLow = tmpArray.todSecondsLow;

    return retVal;
}

MSD_STATUS Peridot_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    PERIDOT_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in Peridot_gptpSetPulseIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpData.ptp1ppsPhase = pulsePara->ptp1ppsPhase;
    tmpData.ptp1ppsSelect = pulsePara->ptp1ppsSelect;
    tmpData.ptp1ppsWidthRange = pulsePara->ptp1ppsWidthRange;
    tmpData.ptpPulseWidth = pulsePara->ptpPulseWidth;

    return Peridot_gptpSetPulse(dev, &tmpData);
}

MSD_STATUS Peridot_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    PERIDOT_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in msdPtpPulseGetIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gptpGetPulse(dev, &tmpData);

    pulsePara->ptp1ppsPhase = tmpData.ptp1ppsPhase;
    pulsePara->ptp1ppsSelect = tmpData.ptp1ppsSelect;
    pulsePara->ptp1ppsWidthRange = tmpData.ptp1ppsWidthRange;
    pulsePara->ptpPulseWidth = tmpData.ptpPulseWidth;

    return retVal;
}