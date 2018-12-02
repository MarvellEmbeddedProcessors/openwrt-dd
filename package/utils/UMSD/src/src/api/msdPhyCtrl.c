#include <msdCopyright.h>

/********************************************************************************
* msdPhyCtrl.h
*
* DESCRIPTION:
*       API definitions for Phy Control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdPhyCtrl.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdPhyReset
*
* DESCRIPTION:
*       This routine preforms PHY reset.
*       After reset, phy will be in Autonegotiation mode.
*
* INPUTS:
*       devNum  - physical devie number
* 		portNum - The logical PHY port number
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
* 		data sheet register 0.15 - Reset
* 		data sheet register 0.13 - Speed(LSB)
* 		data sheet register 0.12 - Autonegotiation
* 		data sheet register 0.8  - Duplex Mode
*		data sheet register 0.6  - Speed(MSB)
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyReset
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphyReset != NULL)
        return dev->SwitchDevObj.PHYCTRLObj.gphyReset(dev, portNum);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyPortLoopbackSet
*
* DESCRIPTION:
*       This routine enables or disables PHY loopback.
*       For 10/100 Fast Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled, this routine disables Auto-Negotiation and
*         forces speed to be 10Mbps.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         Disabling Loopback simply clears bit 14 of control register(0.14). Therefore,
*         it is recommended to call gprtSetPortAutoMode for PHY configuration after
*         Loopback test.
*       For 10/100/1000 Gigagbit Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled and Link is active, the current speed is used.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         All other cases, default MAC Interface speed is used. Please refer to the data
*         sheet for the information of the default MAC Interface speed.
*
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - The logical PHY port number
*       enable - If MSD_TRUE, enable loopback mode
*       		 If MSD_FALSE, disable loopback mode
*
* OUTPUTS:
* 		  None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.14 - Loop_back
*
*******************************************************************************/
MSD_STATUS msdPhyPortLoopbackSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  enable
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback != NULL)
		return dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback(dev, portNum, enable);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for a specific logical PHY port. This function will keep the duplex
*       mode and loopback mode to the previous value, but disable others, such as
*       Autonegotiation.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - The logical PHY port number
*       speed - port speed.
*               PHY_SPEED_10_MBPS for 10Mbps
*               PHY_SPEED_100_MBPS for 100Mbps
*               PHY_SPEED_1000_MBPS for 1000Mbps
*
* OUTPUTS:
* 		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.13 - Speed Selection (LSB)
* 		data sheet register 0.6  - Speed Selection (MSB)
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortSpeedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PHY_SPEED  speed
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed != NULL)
        return dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed(dev, portNum, speed);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyPortPowerDown
*
* DESCRIPTION:
*       Enable/disable (power down) on specific logical PHY port. Phy configuration
*       remains unchanged after Power down.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - The logical PHY port number
*       state - MSD_TRUE: power down
*                MSD_FALSE: normal operation
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
*       data sheet register 0.11 - Power Down
*
*******************************************************************************/
MSD_STATUS msdPhyPortPowerDown
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  state
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown != NULL)
        return dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown(dev, portNum, state);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyPortDuplexModeSet
*
* DESCRIPTION:
*		Sets duplex mode for a specific logical PHY port. This function will keep
*		the speed and loopback mode to the previous value, but disable others,
*		such as Autonegotiation.
*
* INPUTS:
*       devNum  - physical devie number
*		portNum - The logical PHY port number
*		dMode - dulpex mode
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.8 - Duplex Mode
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortDuplexModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  dMode
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode != NULL)
		return dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode(dev, portNum, dMode);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyPortAutoModeSet
*
* DESCRIPTION:
*		This routine sets the PHY port with given auto mode.
*		Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*
*
* INPUTS:
*       devNum  - physical devie number
*		portNum - The logical PHY port number
*		mode - 	Auto Mode to be written
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		data sheet register 4.6, 4.5 Autonegotiation Advertisement for 10BT
*		data sheet register 4.8, 4.7 Autonegotiation Advertisement for 100BT
*		data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortAutoModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PHY_AUTO_MODE  mode
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode != NULL)
		return dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode(dev, portNum, mode);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPhyEEESet
*
* DESCRIPTION:
*       This routine enables or disables the EEE (Energy efficient ethernet) feature.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - The logical PHY port number
*       en      - MSD_TRUE: enable eee
*                 MSD_FALSE: diable eee
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
*
*******************************************************************************/
MSD_STATUS msdPhyEEESet
(
    IN MSD_U8 devNum,
    IN MSD_LPORT portNum,
    IN MSD_BOOL en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PHYCTRLObj.gphySetEEE != NULL)
        return dev->SwitchDevObj.PHYCTRLObj.gphySetEEE(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}
