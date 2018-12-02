#include <msdCopyright.h>

/********************************************************************************
* Peridot_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdPortCtrl.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

/********************************************************************
* Peridot_gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the IEEE 802.1q mode for this port
*
* INPUTS:
*       port    - logical port number to set.
*       mode     - 802.1q mode for this port 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gvlnSetPortVlanDot1qModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
	IN MSD_8021Q_MODE    mode
)
{
	return Peridot_gvlnSetPortVlanDot1qMode(dev, port, (PERIDOT_MSD_8021Q_MODE)mode);
}
/*******************************************************************************
* Peridot_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Peridot_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Peridot_get.
*
* OUTPUTS:
*       mode     - 802.1q mode for this port 
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gvlnGetPortVlanDot1qModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
	OUT MSD_8021Q_MODE   *mode
)
{
	return Peridot_gvlnGetPortVlanDot1qMode(dev, port, (PERIDOT_MSD_8021Q_MODE*)mode);
}
/*******************************************************************************
* Peridot_gprtSetDiscardTagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as 
*       Tagged will be discarded as they enter this switch port. Priority only 
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard tagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetDiscardTaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Peridot_gprtSetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Peridot_gets DiscardTagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardTagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetDiscardTaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Peridot_gprtGetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtSetDiscardUntagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as 
*       Untagged will be discarded as they enter this switch port. Priority only 
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard untagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetDiscardUntaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Peridot_gprtSetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Peridot_gets DiscardUntagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardUntagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetDiscardUntaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Peridot_gprtGetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtSetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine set Forward Unknown mode of a switch port. 
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetUnicastFloodBlockIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Peridot_gprtSetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Peridot_gets Forward Unknown mode of a switch port.
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetUnicastFloodBlockIntf
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
)
{
	return Peridot_gprtGetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Peridot_gprtSetMulticastFloodBlock
*
* DESCRIPTION:
*       When this bit is set to a one, normal switch operation will occurs and 
*       multicast frames with unknown DA addresses are allowed to egress out this 
*       port (assuming the VLAN settings allow the frame to egress this port too).
*       When this bit is cleared to a zero, multicast frames with unknown DA 
*       addresses will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetMulticastFloodBlockIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Peridot_gprtSetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Peridot_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Peridot_gets DefaultForward bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetMulticastFloodBlockIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Peridot_gprtGetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Peridot_gprtSetDiscardBCastMode
*
* DESCRIPTION:
*       This routine sets the Discard Broadcast mode.
*       If the mode is enabled, all the broadcast frames to the given port will 
*       be discarded.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetDiscardBCastModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
	return Peridot_gprtSetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Peridot_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Peridot_gets the Discard Broadcast Mode. If the mode is enabled,
*       all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetDiscardBCastModeIntf
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
)
{
	return Peridot_gprtGetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Peridot_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - MSD_MTU_SIZE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	IN  MSD_MTU_SIZE   mode
)
{
	return Peridot_gprtSetJumboMode(dev, port, (PERIDOT_MSD_MTU_SIZE)mode);
}

/*******************************************************************************
* Peridot_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Peridot_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_MTU_SIZE (1522, 2048, or 10240)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	OUT MSD_MTU_SIZE   *mode
)
{
	return Peridot_gprtGetJumboMode(dev, port, (PERIDOT_MSD_MTU_SIZE*)mode);
}

/*******************************************************************************
* Peridot_gprtSetLearnEnable
*
* DESCRIPTION:
*       This routine enables/disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for disable or MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetLearnEnableIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Peridot_gprtSetLearnEnable(dev, port, mode);
}

/*******************************************************************************
* Peridot_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Peridot_gets LearnDisable setup
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE: Learning disabled on the given port ingress frames,
*                 MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetLearnEnableIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
	return Peridot_gprtGetLearnEnable(dev, port, mode);
}

MSD_STATUS Peridot_gprtGetVTUPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Peridot_gprtGetVTUPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Peridot_gprtSetVTUPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Peridot_gprtSetVTUPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Peridot_gprtGetSAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Peridot_gprtGetSAPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Peridot_gprtSetSAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Peridot_gprtSetSAPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Peridot_gprtGetDAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Peridot_gprtGetDAPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Peridot_gprtSetDAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Peridot_gprtSetDAPriorityOverride(dev, port, (PERIDOT_MSD_PRI_OVERRIDE)mode);
}


/*******************************************************************************
* Peridot_gprtSetFlowCtrl
*
* DESCRIPTION:
*       This routine enable/disable port flow control and set flow control mode
*          mode - PORT_FC_TX_RX_ENABLED,
*                 PORT_RX_ONLY,
*                 PORT_TX_ONLY,
*                 PORT_PFC_ENABLED
*
* INPUTS:
*       port - the logical port number.
*		en - enable/disable the flow control
*       mode - flow control mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*        
* COMMENTS:
*        Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  MSD_PORT_FC_MODE      mode
)
{
	return Peridot_gprtSetFlowCtrl(dev, port, en, (PERIDOT_MSD_PORT_FC_MODE)mode);
}
/*******************************************************************************
* Peridot_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Peridot_get switch port flow control enable/disable status and return 
*       flow control mode
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*		en - enable/disable the flow control
*       mode - flow control mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*        
*******************************************************************************/
MSD_STATUS Peridot_gprtGetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT MSD_PORT_FC_MODE      *mode
)
{
	return Peridot_gprtGetFlowCtrl(dev, port, en, (PERIDOT_MSD_PORT_FC_MODE*)mode);
}

/*******************************************************************************
* Peridot_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed. 
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetForceSpeedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
	IN MSD_PORT_FORCED_SPEED_MODE  mode
)
{
	return Peridot_gprtSetForceSpeed(dev, port, (PERIDOT_MSD_PORT_FORCED_SPEED_MODE)mode);
}

/*******************************************************************************
* Peridot_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetForceSpeedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
	OUT MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
	return Peridot_gprtGetForceSpeed(dev, port, (PERIDOT_MSD_PORT_FORCED_SPEED_MODE*)mode);
}

/*******************************************************************************
* Peridot_grcSetEgressRate
*
* DESCRIPTION:
*       This routine sets the port's egress data limit.
*        
*
* INPUTS:
*       port      - logical port number.
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type). 
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the MSD_ELIMIT_MODE of 
*                                MSD_ELIMIT_LAYER1,
*                                MSD_ELIMIT_LAYER2, or 
*                                MSD_ELIMIT_LAYER3 (see Peridot_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with MSD_ELIMIT_MODE of 
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_grcSetEgressRateIntf
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN MSD_ELIMIT_MODE   mode,
    IN MSD_U32   rate
)
{
    return Peridot_grcSetEgressRate(dev, port, (PERIDOT_MSD_ELIMIT_MODE)mode, rate);
}

/*******************************************************************************
* Peridot_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Peridot_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the MSD_ELIMIT_MODE of
*                                MSD_ELIMIT_LAYER1,
*                                MSD_ELIMIT_LAYER2, or
*                                MSD_ELIMIT_LAYER3 (see Peridot_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_grcGetEgressRateIntf
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_ELIMIT_MODE   *mode,
    OUT MSD_U32   *rate
)
{
    return Peridot_grcGetEgressRate(dev, port, (PERIDOT_MSD_ELIMIT_MODE*)mode, rate);
}


/*******************************************************************************
* Peridot_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            MSD_ELIMIT_FRAME -
*                Count the number of frames
*            MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - MSD_ELIMIT_MODE enum type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None. 
*
*******************************************************************************/
MSD_STATUS Peridot_grcSetELimitModeIntf
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
	IN  MSD_ELIMIT_MODE      mode
)
{
	return Peridot_grcSetELimitMode(dev, port, (PERIDOT_MSD_ELIMIT_MODE)mode);
}


/*******************************************************************************
* Peridot_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Peridot_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            MSD_ELIMIT_FRAME -
*                Count the number of frames
*            MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_ELIMIT_MODE enum type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_grcGetELimitModeIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
	OUT MSD_ELIMIT_MODE  *mode
)
{
	return Peridot_grcGetELimitMode(dev, port, (PERIDOT_MSD_ELIMIT_MODE*)mode);
}

MSD_STATUS Peridot_gprtSetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    return Peridot_gprtSetDuplex(dev, port, (PERIDOT_MSD_PORT_FORCE_DUPLEX_MODE)mode);
}

MSD_STATUS Peridot_gprtGetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
)
{
    return Peridot_gprtGetDuplex(dev, port, (PERIDOT_MSD_PORT_FORCE_DUPLEX_MODE*)mode);
}

/*******************************************************************************
* Peridot_gprtGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrives the port duplex mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for Full-duplex  or MSD_FALSE for Half-duplex
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetDuplexStatusIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	return Peridot_gprtGetDuplexStatus(dev, port, mode);
}

MSD_STATUS Peridot_gprtGetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    return Peridot_gprtGetForceLink(dev, port, (PERIDOT_MSD_PORT_FORCE_LINK_MODE*)mode);
}

MSD_STATUS Peridot_gprtSetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE  mode
)
{
    return Peridot_gprtSetForceLink(dev, port, (PERIDOT_MSD_PORT_FORCE_LINK_MODE)mode);
}

/*******************************************************************************
* Peridot_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_SPEED type.
*                (PORT_SPEED_10_MBPS,PORT_SPEED_100_MBPS, PORT_SPEED_1000_MBPS,
*                etc.)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetSpeedIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_PORT_SPEED   *speed
)
{
	return Peridot_gprtGetSpeed(dev, port, (PERIDOT_MSD_PORT_SPEED*)speed);
}


/*******************************************************************************
* Peridot_gprtGetFrameMode
*
* DESCRIPTION:
*       this routine Get frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_FRAME_MODE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
)
{
    return Peridot_gprtGetFrameMode(dev, port, (PERIDOT_MSD_FRAME_MODE*)mode);
}

/*******************************************************************************
* Peridot_gprtSetFrameMode
*
* DESCRIPTION:
*       this routine Set frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - MSD_FRAME_MODE
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
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
)
{
    return Peridot_gprtSetFrameMode(dev, port, (PERIDOT_MSD_FRAME_MODE)mode);
}

MSD_STATUS Peridot_gprtGetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_STP_STATE  *mode
)
{
    return Peridot_gprtGetPortState(dev, port, (PERIDOT_MSD_PORT_STP_STATE*)mode);
}

MSD_STATUS Peridot_gprtSetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  MSD_PORT_STP_STATE  mode
)
{
    return Peridot_gprtSetPortState(dev, port, (PERIDOT_MSD_PORT_STP_STATE)mode);
}

MSD_STATUS Peridot_gprtSetCutThrouthEnableIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_CUT_THROUGH  *cutThru
)
{
    return Peridot_gprtSetCutThrouthEnable(dev, port, (PERIDOT_CUT_THROUGH*)cutThru);
}

MSD_STATUS Peridot_gprtGetCutThrouthEnableIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_CUT_THROUGH  *cutThru
)
{
    return Peridot_gprtGetCutThrouthEnable(dev, port, (PERIDOT_CUT_THROUGH*)cutThru);
}
