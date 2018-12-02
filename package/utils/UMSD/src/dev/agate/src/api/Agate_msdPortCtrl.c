#include <msdCopyright.h>

/********************************************************************************
* Agate_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdPortCtrl.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


#define AGATE_MSD_GET_RATE_LIMIT_PER_FRAME(_frames, _dec)    \
        ((_frames)?(1000000000 / (32 * (_frames)) + ((1000000000 % (32 * (_frames)) > (32*(_frames)/2))?1:0)):0)

#define AGATE_MSD_GET_RATE_LIMIT_PER_BYTE(_kbps, _dec)    \
        ((_kbps)?((8000000*(_dec)) / (32 * (_kbps)) + ((8000000*(_dec)) % (32 * (_kbps))?1:0)):0)



/*******************************************************************************
* Agate_gvlnSetPortVid
*
* DESCRIPTION:
*       This routine Set the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       vid  - the port vlan id.
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
MSD_STATUS Agate_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gvlnSetPortVid Called.\n"));
    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}	

    if (vid > 0xFFF)
    {
        MSD_DBG_ERROR(("Bad vid. It should be within [0, 0xFFF]\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,AGATE_QD_REG_PVID,0,12, vid);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PVID Register.\n"));
        return retVal;
    }
    MSD_DBG_INFO(("Agate_gvlnSetPortVid Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gvlnGetPortVid
*
* DESCRIPTION:
*       This routine Get the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       vid  - the port vlan id.
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
MSD_STATUS Agate_gvlnGetPortVid
(
	IN MSD_QD_DEV *dev,
	IN  MSD_LPORT port,
	OUT MSD_U16   *vid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gvlnGetPortVid Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PVID, 0, 12, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PVID Register.\n"));
        return retVal;
    }

    *vid = data;

    MSD_DBG_INFO(("Agate_gvlnGetPortVid Exit.\n"));
	return MSD_OK;
}

/********************************************************************
* Agate_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Agate_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN AGATE_MSD_8021Q_MODE    mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gvlnSetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    
    switch (mode)
    {
    case AGATE_MSD_DISABLE:
    case AGATE_MSD_FALLBACK:
    case AGATE_MSD_CHECK:
    case AGATE_MSD_SECURE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,AGATE_QD_REG_PORT_CONTROL2,10,2,(MSD_U16)mode );
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gvlnSetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Agate_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Agate_get.
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
MSD_STATUS Agate_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT AGATE_MSD_8021Q_MODE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gvlnGetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    if(mode == NULL)
    {
        MSD_DBG_ERROR(("Failed(bad mode input).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AGATE_QD_REG_PORT_CONTROL2,10,2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = data;
    MSD_DBG_INFO(("Agate_gvlnGetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetDiscardTagged
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
MSD_STATUS Agate_gprtSetDiscardTagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 9, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDiscardTagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Agate_gets DiscardTagged bit for the given port
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
MSD_STATUS Agate_gprtGetDiscardTagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 9, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetDiscardTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetDiscardUntagged
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
MSD_STATUS Agate_gprtSetDiscardUntagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardUnTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 8, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDiscardUntagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Agate_gets DiscardUntagged bit for the given port
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
MSD_STATUS Agate_gprtGetDiscardUntagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardUnTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 8, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetDiscardUnTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetUnicastFloodBlock
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
MSD_STATUS Agate_gprtSetUnicastFloodBlock
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetUnicastFloodBlock Called.\n"));

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);
    data ^= 1;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 2, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetUnicastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Agate_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Agate_gprtGetUnicastFloodBlock
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetUnicastFloodBlock Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 2, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }
    data ^= 1;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetUnicastFloodBlock Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetMulticastFloodBlock
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
MSD_STATUS Agate_gprtSetMulticastFloodBlock
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    data ^= 1;

    /* Set DefaultForward. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 3, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetMulticastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Agate_gets DefaultForward bit for the given port
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
MSD_STATUS Agate_gprtGetMulticastFloodBlock
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtGetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DefaultForward. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 3, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    data ^= 1;

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetMulticastFloodBlock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetDiscardBCastMode
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
MSD_STATUS Agate_gprtSetDiscardBCastMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDiscardBCastMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Agate_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Agate_gprtGetDiscardBCastMode
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetDiscardBCastMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	data = 0;

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetDiscardBCastMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine sets the Enable/disable trapping all IPV4 IGMP packets to 
*       the CPU,based on their ingress port.
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
MSD_STATUS Agate_gprtSetIGMPSnoopEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 10, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_CT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine gets the Enable/disable trapping all IPV4 IGMP packets to
*       the CPU,based on their ingress port.
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
MSD_STATUS Agate_gprtGetIGMPSnoopEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetHeaderMode
*
* DESCRIPTION:
*       This routine gets the ingress and egress header mode of a switch 
*       port. 
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
MSD_STATUS Agate_gprtGetHeaderMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetHeaderMode
*
* DESCRIPTION:
*       This routine sets the ingress and egress header mode of a switch 
*       port. To be used only for port connected to cpu mainly used for layer3
*       32bit alignment.
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
MSD_STATUS Agate_gprtSetHeaderMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetDropOnLock
*
* DESCRIPTION:
*       This routine gets the DropOnLock mode of specified port on specified device
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
MSD_STATUS Agate_gprtGetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetDropOnLock
*
* DESCRIPTION:
*       This routine sets the Drop on Lock. When set to one, Ingress frames
*       will be discarded if their SA field is not in the ATU's address database
*       of specified port on specified device
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
MSD_STATUS Agate_gprtSetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - AGATE_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Agate_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  AGATE_MSD_MTU_SIZE   mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetJumboMode Called.\n"));

    if (mode > AGATE_MSD_MTU_SIZE_10240)
    {
        MSD_DBG_ERROR(("Bad Parameter\n"));
        return MSD_BAD_PARAM;
    }

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_MSD_MTU_SIZE_1522:
    case AGATE_MSD_MTU_SIZE_2048:
    case AGATE_MSD_MTU_SIZE_10240:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    /* Set the Jumbo Fram Size bit.               */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 12, 2, (MSD_U16)mode);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Agate_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Agate_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT AGATE_MSD_MTU_SIZE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gsysGetJumboMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get Jumbo Frame Mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_MTU_SIZE)data;

    MSD_DBG_INFO(("Agate_gsysGetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetLearnEnable
*
* DESCRIPTION:
*       This routine enables/disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for enable or MSD_FALSE otherwise
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
MSD_STATUS Agate_gprtSetLearnEnable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{          
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          pav;

    MSD_DBG_INFO(("Agate_gprtSetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	/* Set the port's PAV to all zeros */
    if (mode)
        pav = (MSD_U16)(1 << port);
    else
        pav = 0;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 0, 7, pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PAV Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetLearnEnable Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Agate_gets LearnDisable setup
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
MSD_STATUS Agate_gprtGetLearnEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U16          pav;

    MSD_DBG_INFO(("Agate_gprtGetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 0, 7, &pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PAV Register.\n"));
        return retVal;
    }
    if (pav)
		data = 1;
	else
		data = 0;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetLearnEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtGetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 10, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Agate_gprtGetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AGATE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtSetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_PRI_OVERRIDE_NONE:
    case AGATE_PRI_OVERRIDE_FRAME:
    case AGATE_PRI_OVERRIDE_QUEUE:
    case AGATE_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 10, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtGetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Agate_gprtGetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AGATE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtSetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_PRI_OVERRIDE_NONE:
    case AGATE_PRI_OVERRIDE_FRAME:
    case AGATE_PRI_OVERRIDE_QUEUE:
    case AGATE_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 12, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtGetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Agate_gprtGetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AGATE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Agate_gprtSetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_PRI_OVERRIDE_NONE:
    case AGATE_PRI_OVERRIDE_FRAME:
    case AGATE_PRI_OVERRIDE_QUEUE:
    case AGATE_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PRI_OVERRIDE, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetMessagePort
*
* DESCRIPTION:
*       This routine gets message port mode for specific port on a specified device.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetMessagePort
*
* DESCRIPTION:
*       This routine Set message port for specific port on a specific device.
*       Learning and learn refresh message frames will be sent out to this port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
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
MSD_STATUS Agate_gprtSetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetEtherType
*
* DESCRIPTION:
*       This routine gets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode
*       API) of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       eType - Ethernet type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetEtherType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyReg(dev->devNum, phyAddr, AGATE_QD_REG_PORT_ETH_TYPE, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtGetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetEtherType
*
* DESCRIPTION:
*       This routine sets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API)
*       of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       eType - Ethernet type
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
MSD_STATUS Agate_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetEtherType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyReg(dev->devNum, phyAddr, AGATE_QD_REG_PORT_ETH_TYPE, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetTrunkPort
*
* DESCRIPTION:
*       This function enables/disables and sets the trunk ID.
*        
* INPUTS:
*       port - the logical port number.
*       en - MSD_TRUE to make the port be a member of a trunk with the given trunkId.
*             MSD_FALSE, otherwise.
*       trunkId - valid ID is 0 ~ 31
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
MSD_STATUS Agate_gprtSetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      en,
    IN MSD_U32       trunkId
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    if(en == MSD_TRUE)
    {
        /* need to enable trunk. so check the trunkId */
        if (!AGATE_IS_TRUNK_ID_VALID(dev, trunkId))
        {
            MSD_DBG_ERROR(("Failed(bad trunkId).\n"));
            return MSD_BAD_PARAM;
        }

        /* Set TrunkId. */
        retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PORT_CONTROL1, 8, 5, (MSD_U16)trunkId);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
            return retVal;
        }
    }

    /* Set TrunkPort bit. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetTrunkPort Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetTrunkPort
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a trunk,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
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
MSD_STATUS Agate_gprtGetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    OUT MSD_BOOL     *en,
    OUT MSD_U32        *trunkId
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL1, 8, 5, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    *trunkId = (MSD_U32)data;

    MSD_DBG_INFO(("Agate_gprtGetTrunkPort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetFlowCtrl
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
MSD_STATUS Agate_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  AGATE_MSD_PORT_FC_MODE      mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			data;
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtSetFlowCtrl Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	if (en)
	{
		switch (mode)
		{
		case Agate_PORT_FC_TX_RX_ENABLED:
			data = 3;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			return MSD_BAD_PARAM;
		}
	}
	else
		data = 0;

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 6, 2, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_PHY_CONTROL Register.\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtSetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Agate_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Agate_get switch port flow control enable/disable status and return 
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
MSD_STATUS Agate_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT AGATE_MSD_PORT_FC_MODE      *mode
)
{
	MSD_U16			data;
	MSD_STATUS       retVal;
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetFlowCtrl Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 6, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PHY_CONTROL Register.\n"));
		return retVal;
	}

	if (data == 3)
	{
		*en = MSD_TRUE;
		*mode = Agate_PORT_FC_TX_RX_ENABLED;
	}
	else
		*en = MSD_FALSE;

	MSD_DBG_INFO(("Agate_gprtGetFlowCtrl Exit.\n"));
	return retVal;
}


/*******************************************************************************
* Agate_gprtSetEgressMonitorSource
*
* DESCRIPTION:
*       When this bit is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that egresses out this port will
*       also be sent to the EgressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set EgressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS Agate_gprtSetEgressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set EgressMonitorSource. */
	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PORT_CONTROL2, 5, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set EgressMonitorSource bit error\n"));
        return retVal;
    }
    
    MSD_DBG_INFO(("Agate_gprtSetEgressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine Agate_gets EgressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if EgressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS Agate_gprtGetEgressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the EgressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 5, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get EgressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetEgressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetIngressMonitorSource
*
* DESCRIPTION:
*       When this be is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that ingresses in this port will
*       also be sent to the IngressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set IngressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS Agate_gprtSetIngressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set IngressMonitorSource. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 4, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetIngressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine Agate_gets IngressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if IngressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS Agate_gprtGetIngressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the IngressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 4, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetIngressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the AGATE_MSD_ELIMIT_MODE of 
*                                AGATE_MSD_ELIMIT_LAYER1,
*                                AGATE_MSD_ELIMIT_LAYER2, or 
*                                AGATE_MSD_ELIMIT_LAYER3 (see Agate_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AGATE_MSD_ELIMIT_MODE of 
*                                AGATE_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Agate_grcSetEgressRate
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN AGATE_MSD_ELIMIT_MODE mode,
    IN MSD_U32	rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data;
    MSD_U32        eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Agate_grcSetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    switch (mode)
    {
    case AGATE_MSD_ELIMIT_FRAME:
    case AGATE_MSD_ELIMIT_LAYER1:
    case AGATE_MSD_ELIMIT_LAYER2:
    case AGATE_MSD_ELIMIT_LAYER3:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    if((retVal = Agate_grcSetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Agate_grcSetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (mode == AGATE_MSD_ELIMIT_FRAME)    
    {
        if (rate == 0) /* disable egress rate limit */
        {
            eDec = 0;
            data = 0;
        }
        else if((rate < 7600)  || (rate > 1488000))
        {
            MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
            return MSD_BAD_PARAM;
        }
        else
        {
            eDec = 1;
            data = (MSD_U16)AGATE_MSD_GET_RATE_LIMIT_PER_FRAME(rate,eDec);
        }
    }
    else
    {
        if(rate == 0)
        {
            eDec = 0;
        }
        else if(rate < 1000)    /* less than 1Mbps */
        {
            /* it should be divided by 64 */
            if (rate % 64)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/64;
        }
        else if(rate <= 100000)    /* less than or equal to 100Mbps */
        {
            /* it should be divided by 1000 */
            if (rate % 1000)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/1000;
        }
        else if(rate <= 1000000)    /* less than or equal to 1000Mbps */
        {
            /* it should be divided by 10000 */
            if (rate % 10000)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/10000;
        }
        else
        {
            MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
            return MSD_BAD_PARAM;
        }

        if(rate == 0)
        {
            data = 0;
        }
        else
        {
            data = (MSD_U16)AGATE_MSD_GET_RATE_LIMIT_PER_BYTE(rate,eDec);
        }
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL, 0, 7, (MSD_U16)eDec);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL2, 0, 14, (MSD_U16)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AGATE_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_grcSetEgressRate Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Agate_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the AGATE_MSD_ELIMIT_MODE of
*                                AGATE_MSD_ELIMIT_LAYER1,
*                                AGATE_MSD_ELIMIT_LAYER2, or
*                                AGATE_MSD_ELIMIT_LAYER3 (see Agate_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AGATE_MSD_ELIMIT_MODE of
*                                AGATE_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Agate_grcGetEgressRate
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT AGATE_MSD_ELIMIT_MODE *mode,
	OUT MSD_U32	*rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        eRate, eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Agate_grcGetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    if((retVal = Agate_grcGetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Agate_grcGetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AGATE_QD_REG_EGRESS_RATE_CTRL,0,7,&eDec);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AGATE_QD_REG_EGRESS_RATE_CTRL2,0,14,&eRate );
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    if (*mode == AGATE_MSD_ELIMIT_FRAME)    
    {
        *rate = AGATE_MSD_GET_RATE_LIMIT_PER_FRAME(eRate, eDec);
    }
    else
    {
        /* Count Per Byte */
        *rate = AGATE_MSD_GET_RATE_LIMIT_PER_BYTE(eRate, eDec);
    }

    MSD_DBG_INFO(("Agate_grcGetEgressRate Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Agate_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AGATE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AGATE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AGATE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AGATE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - AGATE_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Agate_grcSetELimitMode
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
    IN  AGATE_MSD_ELIMIT_MODE      mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_grcSetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    data = (MSD_U16)mode & 0x3;

    /* Set the Elimit mode.            */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL2, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_grcSetELimitMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Agate_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AGATE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AGATE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AGATE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AGATE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AGATE_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Agate_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT AGATE_MSD_ELIMIT_MODE  *mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_grcGetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    /* Get the Elimit mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL2, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    *mode = data;

    MSD_DBG_INFO(("Agate_grcGetELimitMode Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AGATE_MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case AGATE_MSD_DUPLEX_HALF:
        data = 0x1;
        break;
    case AGATE_MSD_DUPLEX_FULL:
        data = 0x3;
        break;
    case AGATE_MSD_DUPLEX_AUTO:
        data = 0x0;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 2, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetDuplex Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AGATE_MSD_PORT_FORCE_DUPLEX_MODE  *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 2, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    switch (data)
    {
    case 0x1:
        *mode = AGATE_MSD_DUPLEX_HALF;
        break;
    case 0x3:
        *mode = AGATE_MSD_DUPLEX_FULL;
        break;
    case 0:
    case 0x2:
        *mode = AGATE_MSD_DUPLEX_AUTO;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }


    MSD_DBG_INFO(("Agate_gprtGetDuplex Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetDuplexStatus
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
MSD_STATUS Agate_gprtGetDuplexStatus
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	
	MSD_DBG_INFO(("Agate_gprtGetDuplexStatus Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PORT_STATUS, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Agate_gprtGetDuplexStatus Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AGATE_MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 4, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    if ((data & 0x1) == 0)
        *mode = AGATE_PORT_DO_NOT_FORCE_LINK;
    else if ((data & 0x3) == 3)
        *mode = AGATE_PORT_FORCE_LINK_UP;
    else if ((data & 0x3) == 1)
        *mode = AGATE_PORT_FORCE_LINK_DOWN;

    MSD_DBG_INFO(("Agate_gprtGetForceLink Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_FORCE_LINK_MODE  mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case AGATE_PORT_DO_NOT_FORCE_LINK:
        data = 0;
        break;
    case AGATE_PORT_FORCE_LINK_UP:
        data = 3;
        break;
    case AGATE_PORT_FORCE_LINK_DOWN:
        data = 1;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mpde).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 4, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetForceLink Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetLinkState
*
* DESCRIPTION:
*       This routine retrives the link state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - MSD_TRUE for Up  or MSD_FALSE for Down
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
MSD_STATUS Agate_gprtGetLinkState
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *state
)
{
	MSD_U16          data;           /* Used to poll the SWReset bit */
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	
	MSD_DBG_INFO(("Agate_gprtGetLinkState Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PORT_STATUS, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *state);

    MSD_DBG_INFO(("Agate_gprtGetLinkState Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - AGATE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000 or No Speed Force)
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
MSD_STATUS Agate_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN AGATE_MSD_PORT_FORCED_SPEED_MODE  mode
)
{
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;
    MSD_U16			data1;
    MSD_U16			data2;
    MSD_U16			linkStatus;

    MSD_DBG_INFO(("Agate_gprtSetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /*Get the force link status and then force link down*/
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 4, 2, &linkStatus)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get link status Failed\n"));
        return retVal;
    }
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 4, 2, 1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Force Link down failed\n"));
        return retVal;
    }


	/* Set the ForceSpeed bit.  */
	if (mode == Agate_PORT_DO_NOT_FORCE_SPEED)
	{
		data1 = 0;
		data2 = 3;
	}
	else
	{
		switch (mode)
		{
		case Agate_PORT_FORCE_SPEED_10M:
			data1 = 0;
			data2 = 0;
			break;
		case Agate_PORT_FORCE_SPEED_100M:
			data1 = 0;
			data2 = 1;
			break;
		case Agate_PORT_FORCE_SPEED_200M:
			if (port != 5 && port != 6)
			{
				MSD_DBG_ERROR(("Failed (Bad Port), Only port 5 & 6 support 200M.\n"));
				return MSD_NOT_SUPPORTED;
			}
			data1 = 1;
			data2 = 1;
			break;
		case Agate_PORT_FORCE_SPEED_1000M:
			data1 = 0;
			data2 = 2;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
			return MSD_BAD_PARAM;
		}
	}
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 12, 1, data1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Set Alternate Speed bit error\n"));
        return retVal;
    }
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 0, 2, data2)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Set Speed bit error\n"));
        return retVal;
    }
    

    /*Set back the force link status*/
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 4, 2, linkStatus)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Set back force link status error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetForceSpeed Called.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Agate_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
    MSD_U16          data1;
    MSD_U16          data2;
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the ForceSpeed bit.  */
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 12, 1, &data1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get Alternate Speed bit error\n"));
        return retVal;
    }
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 0, 2, &data2)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get Speed bit error\n"));
        return retVal;
    }
    if (data1 == 1)
    {
        switch (data2)
        {
        case 0:
            *mode = Agate_PORT_FORCE_SPEED_10M;
            break;
        case 1:
            *mode = Agate_PORT_FORCE_SPEED_200M;
            break;
        case 2:
            *mode = Agate_PORT_FORCE_SPEED_1000M;
            break;
        case 3:
			*mode = Agate_PORT_DO_NOT_FORCE_SPEED;
            break;
        default:
            MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
            return MSD_FAIL;
        }
    }
    else
    {
        switch (data2)
        {
        case 0:
            *mode = Agate_PORT_FORCE_SPEED_10M;
            break;
        case 1:
            *mode = Agate_PORT_FORCE_SPEED_100M;
            break;
        case 2:
            *mode = Agate_PORT_FORCE_SPEED_1000M;
            break;
        case 3:
			*mode = Agate_PORT_DO_NOT_FORCE_SPEED;
            break;
        default:
            MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
            return MSD_FAIL;
        }
    }
    

    MSD_DBG_INFO(("Agate_gprtGetForceSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_PORT_SPEED type.
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
MSD_STATUS Agate_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT AGATE_MSD_PORT_SPEED   *speed
)
{
	MSD_U16          data,data1;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetSpeed Called.\n"));	

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);


	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AGATE_QD_REG_PORT_STATUS, 8, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* Get 200BASE bit */
	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PHY_CONTROL, 12, 1, &data1);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to read AGATE_QD_REG_PHY_CONTROL Register.\n"));
		return retVal;
	}

	if (data1)
	{
		switch (data)
		{
		case 0:
			*speed = Agate_PORT_SPEED_10_MBPS;
			break;
		case 1:
			*speed = Agate_PORT_SPEED_200_MBPS;
			break;
		case 2:
			*speed = Agate_PORT_SPEED_1000_MBPS;
			break;
		default:
			*speed = Agate_PORT_SPEED_UNKNOWN;
			break;
		}    
    }
	else
	{
		switch (data)
		{
		case 0:
			*speed = Agate_PORT_SPEED_10_MBPS;
			break;
		case 1:
			*speed = Agate_PORT_SPEED_100_MBPS;
			break;
		case 2:
			*speed = Agate_PORT_SPEED_1000_MBPS;
			break;
		default:
			*speed = Agate_PORT_SPEED_UNKNOWN;
			break;
		}
	}

    MSD_DBG_INFO(("Agate_gprtGetSpeed Called.\n"));
	return MSD_OK;
}


/*******************************************************************************
* Agate_gprtGetVlanPorts
*
* DESCRIPTION:
*       this routine Get port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       memPorts - vlan ports
*       memPortsLen - number of vlan ports
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
MSD_STATUS Agate_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;
    MSD_U8           i;
    MSD_32           portLen = 0;

    MSD_DBG_INFO(("Agate_gprtGetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_VLAN_MAP, 0, 11, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PORT_VLAN_MAP register error\n"));
        return retVal;
    }
    for (i = 0; i < dev->numOfPorts; i++)
    {
        if (data & (1 << i))
        {
            memPorts[portLen++] = i;
        }
    }

    *memPortsLen = (MSD_U8)portLen;

    MSD_DBG_INFO(("Agate_gprtGetVlanPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* msdPortVlanPortsSet
*
* DESCRIPTION:
*       this routine Set port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       memPorts - vlan ports to set
*       memPortsLen - number of vlan ports
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
MSD_STATUS Agate_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;
    MSD_U8           i;

    MSD_DBG_INFO(("Agate_gprtSetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (memPortsLen > dev->numOfPorts)
    {
        MSD_DBG_ERROR(("Failed (Bad num of vlan ports).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;
    for (i = 0; i < memPortsLen; i++)
    {
        if (memPorts[i] < dev->numOfPorts)
            data |= (1 << memPorts[i]);
        else
        {
            MSD_DBG_ERROR(("Failed (Bad member port).\n"));
            return MSD_BAD_PARAM;
        }
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_VLAN_MAP, 0, 11, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_VLAN_MAP register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetVlanPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetFrameMode
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
MSD_STATUS Agate_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_FRAME_MODE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 8, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_FRAME_MODE)data;

    MSD_DBG_INFO(("Agate_gprtGetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetFrameMode
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
MSD_STATUS Agate_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AGATE_MSD_FRAME_MODE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_FRAME_MODE_NORMAL :
    case AGATE_FRAME_MODE_DSA:
    case AGATE_FRAME_MODE_PROVIDER:
    case AGATE_FRAME_MODE_ETHER_TYPE_DSA:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 8, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Agate_gprtSetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AGATE_PORT_STP_STATE
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
MSD_STATUS Agate_gprtGetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT AGATE_MSD_PORT_STP_STATE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 0, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (AGATE_MSD_PORT_STP_STATE)data;

    MSD_DBG_INFO(("Agate_gprtGetPortState Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - AGATE_PORT_STP_STATE
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
MSD_STATUS Agate_gprtSetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  AGATE_MSD_PORT_STP_STATE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AGATE_MSD_PORT_DISABLE:
    case AGATE_MSD_PORT_BLOCKING:
    case AGATE_MSD_PORT_LEARNING:
    case AGATE_MSD_PORT_FORWARDING:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL, 0, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Agate_gprtSetPortState Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtSetPauseLimitOut Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Set AGATE_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtSetPauseLimitOut Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetPauseLimitOut Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Get AGATE_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtGetPauseLimitOut Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtSetPauseLimitIn Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Set AGATE_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtSetPauseLimitIn Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetPauseLimitOut Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Get AGATE_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtGetPauseLimitOut Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gprtGetOutQueueSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8 queue,
OUT MSD_U16  *count
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetOutQueueSize Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (queue >= AGATE_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_Q_COUNTER, 12, 4, queue);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_Q_COUNTER, 0, 9, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtGetOutQueueSize Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 7, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 7, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 12, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PAV, 12, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtSetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtSetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 6, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gprtSetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtGetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Agate_gprtGetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_PORT_CONTROL2, 6, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AGATE_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gprtGetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Agate_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8   hwPort;         /* the physical port number     */
	MSD_U8	phyAddr;
	MSD_U8 i = 0;
	char desc[32][48] =
	{
		{ "Port Status Register(0x0)" },
		{ "Physical Control Register(0x1)" },
		{ "Jamming Control Register(0x2)" },
		{ "Switch Identifier Register(0x3)" },
		{ "Port Control Register(0x4)" },
		{ "Port Control 1(0x5)" },
		{ "Port Based VLAN Map(0x6)" },
		{ "Default Port VLAN ID & Priority(0x7)" },
		{ "Port Control 2 Register(0x8)" },
		{ "Egress Rate Control(0x9)" },
		{ "Egress Rate Control 2(0xa)" },
		{ "Port Association Vector(0xb)" },
		{ "Port ATU Control(0xc)" },
		{ "Override Register(0xd)" },
		{ "Policy Control Register(0xe)" },
		{ "Port E Type(0xf)" },
		{ "InDiscardsLo Frame Counter(0x10)" },
		{ "InDiscardsHi Frame Counter(0x11)" },
		{ "InFiltered/TcamCtr Frame Counter(0x12)" },
		{ "Rx Frame Counter(0x13)" },
		{ "Reserved(0x14)" },
		{ "Reserved(0x15)" },
		{ "LED Control(0x16)" },
		{ "Reserved(0x17)" },
		{ "Port IEEE Priority Remapping Registers(0x18)" },
		{ "Port IEEE Priority Remapping Registers(0x19)" },
		{ "Reserved(0x1a)" },
		{ "Queue Counter Registers(0x1b)" },
		{ "Reserved(0x1c)" },
		{ "Reserved(0x1d)" },
		{ "Reserved(0x1e)" },
		{ "Reserved(0x1f)" }
	};

	MSD_DBG_INFO(("Agate_gprtRegDump Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	MSG(("\n------------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, phyAddr, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-48s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Agate_gprtRegDump Exit.\n"));
	return MSD_OK;
}