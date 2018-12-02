#include <msdCopyright.h>

/********************************************************************************
* msdPTP.c
*
* DESCRIPTION:
*       API definitions for PTP 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdPTP.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdPtpIntEnableSet
*
* DESCRIPTION:
*       This routine enables or disables port PTP interrupt.
*
* INPUTS:
*       portNum - The logical port number
*       interruptEn    - Port PTP interrupt Enable/Disable
*                        ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
*                        ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntEnableSet
(
    IN MSD_U8    devNum,
    IN MSD_LPORT portNum,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetIntEnable != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetIntEnable(dev, portNum, interruptEn);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpIntEnableGet
*
* DESCRIPTION:
*       This routine gets port PTP interrupt enable/disable.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*       interruptEn    - Port PTP interrupt Enable/Disable
*                        ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
*                        ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntEnableGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetIntEnable != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetIntEnable(dev, portNum, interruptEn);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPortPTPEnSet
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*       portNum - The logical port number
*       en   - MSD_TRUE to enable PTP, MSD_FALSE to disable PTP
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortPTPEnSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPortPTPEnGet
*
* DESCRIPTION:
*       This routine gets PTP enable status on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*       en   - MSD_TRUE to enable PTP, MSD_FALSE to disable PTP
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortPTPEnGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPortConfigSet
*
* DESCRIPTION:
*       This routine sets PTP port configuration.
*
* INPUTS:
*       portNum    - The logical port number
*       ptpData - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortConfigSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetPortConfig != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetPortConfig(dev, portNum, ptpData);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPortConfigGet
*
* DESCRIPTION:
*       This routine gets PTP port configuration.
*
* INPUTS:
*       portNum    - The logical port number
*
* OUTPUTS:
*       ptpData - PTP port configuration parameters.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortConfigGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetPortConfig != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetPortConfig(dev, portNum, ptpData);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTimeStampGet
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value
*        and sequce Id that are captured by PTP logic for a PTP frame that needs
*        to be time stamped.
*
* INPUTS:
*       portNum - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type);
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTimeStampGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetTimeStamp != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetTimeStamp(dev, portNum, timeToRead, ptpStatus);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTSValidStGet
*
* DESCRIPTION:
*        This routine gets PTP Time valid bit.
*
* INPUTS:
*       portNum       - logical port number.
*       timeToRead - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type)
*
* OUTPUTS:
*    isValid        - MSD_TRUE: valid TS, MSD_FALSE: invalid TS.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTSValidStGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetTSValidSt != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetTSValidSt(dev, portNum, timeToRead, isValid);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTimeStampReset
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       portNum - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type);
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTimeStampReset
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToReset
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpResetTimeStamp != NULL)
        return dev->SwitchDevObj.PTPObj.gptpResetTimeStamp(dev, portNum, timeToReset);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpGlobalConfigSet
*
* DESCRIPTION:
*       This routine sets port PTP global configuration.
*
* INPUTS:
*       ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalConfigSet
(
    IN  MSD_U8    devNum,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig(dev, ptpData);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpGlobalConfigGet
*
* DESCRIPTION:
*       This routine gets port PTP global configuration.
*
* INPUTS:
*
* OUTPUTS:
*       ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalConfigGet
(
    IN  MSD_U8    devNum,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig(dev, ptpData);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpIntStatusGet
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for a port.
*
* INPUTS:
*
* OUTPUTS:
*       ptpIntSt   - interrupt status for
*                       ptpTrigGenInt;      Trigger generate mode Interrupt
*                       ptpEventInt;        Event Capture Interrupt
*                       ptpUpperPortInt;    Upper Ports Interrupt
*                       ptpIntStPortVect
*                         interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetIntStatus != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetIntStatus(dev, ptpIntSt);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpMeanPathDelaySet
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelaySet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	delay
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay(dev, portNum, delay);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpMeanPathDelayGet
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelayGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*delay
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay(dev, portNum, delay);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpIgrPathDelayAsymSet
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym(dev, portNum, asym);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpIgrPathDelayAsymGet
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym(dev, portNum, asym);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpEgrPathDelayAsymSet
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym(dev, portNum, asym);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpEgrPathDelayAsymGet
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym(dev, portNum, asym);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpModeSet
*
* DESCRIPTION:
*        Set PTP mode configuration.
*
* INPUTS:
*        mode -
*          altScheme  - Alternate Scheme
*          grandMstr  - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetMode != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetMode(dev, mode);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpModeGet
*
* DESCRIPTION:
*        Get PTP mode configuration.
*
* INPUTS:
*
* OUTPUTS:
*        mode -
*          altScheme  - Alternate Scheme
*          grandMstr  - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetMode != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetMode(dev, mode);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpMasterSlaveSet
*
* DESCRIPTION:
*        Set PTP as Master or Slave.
*
* INPUTS:
*        role - PTP_ROLE_SLAVE:Set device as slave   PTP_ROLE_MASTER:Set device as master
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MASTER_SLAVE role
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetMasterSlave != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetMasterSlave(dev, role);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpMasterSlaveGet
*
* DESCRIPTION:
*        Get PTP role, Master or Slave.
*
* INPUTS:
*
* OUTPUTS:
*        role - PTP_ROLE_SLAVE:Set device as slave   PTP_ROLE_MASTER:Set device as master
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MASTER_SLAVE *role
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetMasterSlave != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetMasterSlave(dev, role);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTODBusyBitStatusGet
*
* DESCRIPTION:
*        Check the busy bit to see if the hardware is busy. Make sure the busy status is False befor any other ToD operation
*
* INPUTS:
*
* OUTPUTS:
*        busyStatus - MSD_TRUE: Hardware is busy, should wait. MSD_FALSE: busy bit has been self clear, can do other operation
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODBusyBitStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL	*busyStatus
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus(dev, busyStatus);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTODStoreCompensation
*
* DESCRIPTION:
*        Store Compensation register only to selected TimeArray
*
* INPUTS:
*        timeArrayIndex - Time array index
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODStoreCompensation
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation != NULL)
        return dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation(dev, timeArrayIndex, timeArray);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTODStoreAll
*
* DESCRIPTION:
*        Store all register to selected TimeArray
*
* INPUTS:
*        timeArrayIndex - Time array index
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODStoreAll
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpTODStoreAll != NULL)
        return dev->SwitchDevObj.PTPObj.gptpTODStoreAll(dev, timeArrayIndex, timeArray);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpTODCaptureAll
*
* DESCRIPTION:
*        Capture selected TimeArray.
*
* INPUTS:
*        timeArrayIndex - Time array index
*
* OUTPUTS:
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODCaptureAll
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpTODCaptureAll != NULL)
        return dev->SwitchDevObj.PTPObj.gptpTODCaptureAll(dev, timeArrayIndex, timeArray);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPulseSet
*
* DESCRIPTION:
*        Set PTP Pulse parameters.
*
* INPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpSetPulse != NULL)
        return dev->SwitchDevObj.PTPObj.gptpSetPulse(dev, pulsePara);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPtpPulseGet
*
* DESCRIPTION:
*        Get PTP Pulse parameters.
*
* INPUTS:
*
* OUTPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gptpGetPulse != NULL)
        return dev->SwitchDevObj.PTPObj.gptpGetPulse(dev, pulsePara);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*        running clock.
*
* INPUTS:
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdGetPTPGlobalTime
(
    IN  MSD_U8  devNum,
    OUT MSD_U32 *ptpTime
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime != NULL)
        return dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime(dev, ptpTime);
    else
        return MSD_NOT_SUPPORTED;
}