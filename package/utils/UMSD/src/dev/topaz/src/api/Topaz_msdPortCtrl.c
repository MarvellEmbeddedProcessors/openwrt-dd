#include <msdCopyright.h>

/********************************************************************************
* Topaz_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdPortCtrl.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


#define TOPAZ_MSD_GET_RATE_LIMIT_PER_FRAME(_frames, _dec)    \
        ((_frames)?(1000000000 / (32 * (_frames)) + ((1000000000 % (32 * (_frames)) > (32*(_frames)/2))?1:0)):0)

#define TOPAZ_MSD_GET_RATE_LIMIT_PER_BYTE(_kbps, _dec)    \
        ((_kbps)?((8000000*(_dec)) / (32 * (_kbps)) + ((8000000*(_dec)) % (32 * (_kbps))?1:0)):0)


/*******************************************************************************
* Topaz_gvlnSetPortVid
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
MSD_STATUS Topaz_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gvlnSetPortVid Called.\n"));
    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
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

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_PVID,0,12, vid);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PVID Register.\n"));
        return retVal;
    }
    MSD_DBG_INFO(("Topaz_gvlnSetPortVid Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gvlnGetPortVid
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
MSD_STATUS Topaz_gvlnGetPortVid
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

    MSD_DBG_INFO(("Topaz_gvlnGetPortVid Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PVID, 0, 12, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PVID Register.\n"));
        return retVal;
    }

    *vid = data;

    MSD_DBG_INFO(("Topaz_gvlnGetPortVid Exit.\n"));
	return MSD_OK;
}

/********************************************************************
* Topaz_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Topaz_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN TOPAZ_MSD_8021Q_MODE    mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gvlnSetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    switch (mode)
    {
    case TOPAZ_MSD_DISABLE:
    case TOPAZ_MSD_FALLBACK:
    case TOPAZ_MSD_CHECK:
    case TOPAZ_MSD_SECURE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_PORT_CONTROL2,10,2,(MSD_U16)mode );
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gvlnSetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Topaz_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Topaz_get.
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
MSD_STATUS Topaz_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT TOPAZ_MSD_8021Q_MODE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gvlnGetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
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

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_PORT_CONTROL2,10,2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = data;
    MSD_DBG_INFO(("Topaz_gvlnGetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetDiscardTagged
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
MSD_STATUS Topaz_gprtSetDiscardTagged
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

    MSD_DBG_INFO(("Topaz_gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 9, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDiscardTagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Topaz_gets DiscardTagged bit for the given port
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
MSD_STATUS Topaz_gprtGetDiscardTagged
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

    MSD_DBG_INFO(("Topaz_gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 9, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetDiscardTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetDiscardUntagged
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
MSD_STATUS Topaz_gprtSetDiscardUntagged
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

    MSD_DBG_INFO(("Topaz_gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardUnTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 8, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDiscardUntagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Topaz_gets DiscardUntagged bit for the given port
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
MSD_STATUS Topaz_gprtGetDiscardUntagged
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

    MSD_DBG_INFO(("Topaz_gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardUnTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 8, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetDiscardUnTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetUnicastFloodBlock
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
MSD_STATUS Topaz_gprtSetUnicastFloodBlock
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

    MSD_DBG_INFO(("Topaz_gprtSetUnicastFloodBlock Called.\n"));

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);
    data ^= 1;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 2, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetUnicastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Topaz_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Topaz_gprtGetUnicastFloodBlock
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

	MSD_DBG_INFO(("Topaz_gprtGetUnicastFloodBlock Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 2, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }
    data ^= 1;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetUnicastFloodBlock Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetMulticastFloodBlock
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
MSD_STATUS Topaz_gprtSetMulticastFloodBlock
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

    MSD_DBG_INFO(("Topaz_gprtSetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    data ^= 1;

    /* Set DefaultForward. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 3, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetMulticastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Topaz_gets DefaultForward bit for the given port
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
MSD_STATUS Topaz_gprtGetMulticastFloodBlock
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

    MSD_DBG_INFO(("Topaz_gprtGetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DefaultForward. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 3, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    data ^= 1;

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetMulticastFloodBlock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetDiscardBCastMode
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
MSD_STATUS Topaz_gprtSetDiscardBCastMode
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

    MSD_DBG_INFO(("Topaz_gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, TOPAZ_QD_REG_PORT_CONTROL1));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDiscardBCastMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Topaz_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Topaz_gprtGetDiscardBCastMode
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

	MSD_DBG_INFO(("Topaz_gprtGetDiscardBCastMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read Register, phyAddr: %x, regAddr:%x.\n", phyAddr, TOPAZ_QD_REG_PORT_CONTROL1));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetDiscardBCastMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetIGMPSnoopEnable
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
MSD_STATUS Topaz_gprtSetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Topaz_gprtSetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 10, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_CT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetIGMPSnoopEnable
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
MSD_STATUS Topaz_gprtGetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Topaz_gprtGetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetHeaderMode
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
MSD_STATUS Topaz_gprtGetHeaderMode
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

    MSD_DBG_INFO(("Topaz_gprtGetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetHeaderMode
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
MSD_STATUS Topaz_gprtSetHeaderMode
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

    MSD_DBG_INFO(("Topaz_gprtSetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_CT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetDropOnLock
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
MSD_STATUS Topaz_gprtGetDropOnLock
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

    MSD_DBG_INFO(("Topaz_gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetDropOnLock
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
MSD_STATUS Topaz_gprtSetDropOnLock
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

    MSD_DBG_INFO(("Topaz_gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - TOPAZ_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Topaz_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  TOPAZ_MSD_MTU_SIZE   mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetJumboMode Called.\n"));

    if (mode > TOPAZ_MSD_MTU_SIZE_10240)
    {
        MSD_DBG_ERROR(("Bad Parameter\n"));
        return MSD_BAD_PARAM;
    }

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}


    switch (mode)
    {
    case TOPAZ_MSD_MTU_SIZE_1522:
    case TOPAZ_MSD_MTU_SIZE_2048:
    case TOPAZ_MSD_MTU_SIZE_10240:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }
    /* Set the Jumbo Fram Size bit.               */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 12, 2, (MSD_U16)mode);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Topaz_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - TOPAZ_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Topaz_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT TOPAZ_MSD_MTU_SIZE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gsysGetJumboMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get Jumbo Frame Mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_MTU_SIZE)data;

    MSD_DBG_INFO(("Topaz_gsysGetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetLearnEnable
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
MSD_STATUS Topaz_gprtSetLearnEnable
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

    MSD_DBG_INFO(("Topaz_gprtSetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 0, 7, pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PAV Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetLearnEnable Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Topaz_gets LearnDisable setup
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
MSD_STATUS Topaz_gprtGetLearnEnable
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

    MSD_DBG_INFO(("Topaz_gprtGetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 0, 7, &pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PAV Register.\n"));
        return retVal;
    }
    if (pav)
		data = 1;
	else
		data = 0;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetLearnEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT TOPAZ_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtGetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 10, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Topaz_gprtGetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  TOPAZ_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtSetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case TOPAZ_PRI_OVERRIDE_NONE:
    case TOPAZ_PRI_OVERRIDE_FRAME:
    case TOPAZ_PRI_OVERRIDE_QUEUE:
    case TOPAZ_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 10, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT TOPAZ_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtGetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Topaz_gprtGetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  TOPAZ_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtSetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case TOPAZ_PRI_OVERRIDE_NONE:
    case TOPAZ_PRI_OVERRIDE_FRAME:
    case TOPAZ_PRI_OVERRIDE_QUEUE:
    case TOPAZ_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 12, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT TOPAZ_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtGetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Topaz_gprtGetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  TOPAZ_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gprtSetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case TOPAZ_PRI_OVERRIDE_NONE:
    case TOPAZ_PRI_OVERRIDE_FRAME:
    case TOPAZ_PRI_OVERRIDE_QUEUE:
    case TOPAZ_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PRI_OVERRIDE, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetMessagePort
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
MSD_STATUS Topaz_gprtGetMessagePort
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

    MSD_DBG_INFO(("Topaz_gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetMessagePort
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
MSD_STATUS Topaz_gprtSetMessagePort
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

    MSD_DBG_INFO(("Topaz_gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetEtherType
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
MSD_STATUS Topaz_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetEtherType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ETH_TYPE, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtGetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetEtherType
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
MSD_STATUS Topaz_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetEtherType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ETH_TYPE, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetTrunkPort
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
MSD_STATUS Topaz_gprtSetTrunkPort
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

    MSD_DBG_INFO(("Topaz_gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
        if (!TOPAZ_IS_TRUNK_ID_VALID(dev, trunkId))
        {
            MSD_DBG_ERROR(("Failed(bad trunkId).\n"));
            return MSD_BAD_PARAM;
        }

        /* Set TrunkId. */
        retVal = msdSetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 8, 5, (MSD_U16)trunkId);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
            return retVal;
        }
    }

    /* Set TrunkPort bit. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetTrunkPort Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetTrunkPort
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
MSD_STATUS Topaz_gprtGetTrunkPort
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

    MSD_DBG_INFO(("Topaz_gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL1, 8, 5, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    *trunkId = (MSD_U32)data;

    MSD_DBG_INFO(("Topaz_gprtGetTrunkPort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetFlowCtrl
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
MSD_STATUS Topaz_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  TOPAZ_MSD_PORT_FC_MODE      mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			data;
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetFlowCtrl Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (en)
    {
        switch (mode)
        {
        case Topaz_PORT_FC_TX_RX_ENABLED:
            data = 3;
            break;
        default:
            MSD_DBG_ERROR(("Failed (Bad mode).\n"));
            return MSD_BAD_PARAM;            
        }
    }
    else
        data = 0;
    
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 6, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_PHY_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Topaz_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Topaz_get switch port flow control enable/disable status and return 
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
MSD_STATUS Topaz_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT TOPAZ_MSD_PORT_FC_MODE      *mode
)
{
	MSD_U16			data;
    MSD_STATUS       retVal;    
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetFlowCtrl Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 6, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PHY_CONTROL Register.\n"));
        return retVal;
    }

    if (data == 3)
    {
        *en = MSD_TRUE;
        *mode = Topaz_PORT_FC_TX_RX_ENABLED;
    }
    else
        *en = MSD_FALSE;

    MSD_DBG_INFO(("Topaz_gprtGetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Topaz_gprtSetEgressMonitorSource
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
MSD_STATUS Topaz_gprtSetEgressMonitorSource
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

    MSD_DBG_INFO(("Topaz_gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set EgressMonitorSource. */
	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 5, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set EgressMonitorSource bit error\n"));
        return retVal;
    }
    
    MSD_DBG_INFO(("Topaz_gprtSetEgressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine Topaz_gets EgressMonitorSource bit for the given port
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
MSD_STATUS Topaz_gprtGetEgressMonitorSource
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

    MSD_DBG_INFO(("Topaz_gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the EgressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 5, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get EgressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetEgressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetIngressMonitorSource
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
MSD_STATUS Topaz_gprtSetIngressMonitorSource
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

    MSD_DBG_INFO(("Topaz_gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set IngressMonitorSource. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 4, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetIngressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine Topaz_gets IngressMonitorSource bit for the given port
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
MSD_STATUS Topaz_gprtGetIngressMonitorSource
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

    MSD_DBG_INFO(("Topaz_gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the IngressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 4, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetIngressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the TOPAZ_MSD_ELIMIT_MODE of 
*                                TOPAZ_MSD_ELIMIT_LAYER1,
*                                TOPAZ_MSD_ELIMIT_LAYER2, or 
*                                TOPAZ_MSD_ELIMIT_LAYER3 (see Topaz_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with TOPAZ_MSD_ELIMIT_MODE of 
*                                TOPAZ_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Topaz_grcSetEgressRate
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN TOPAZ_MSD_ELIMIT_MODE mode,
    IN MSD_U32	rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data;
    MSD_U32        eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Topaz_grcSetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    switch (mode)
    {
    case TOPAZ_MSD_ELIMIT_FRAME:
    case TOPAZ_MSD_ELIMIT_LAYER1:
    case TOPAZ_MSD_ELIMIT_LAYER2:
    case TOPAZ_MSD_ELIMIT_LAYER3:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    if((retVal = Topaz_grcSetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_grcSetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (mode == TOPAZ_MSD_ELIMIT_FRAME)    
    {
        if (rate == 0) /* disable egress rate limit */
        {
            eDec = 0;
            data = 0;
        }
        else if ((rate < 7600) || (rate > 1488000))
        {
            MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
            return MSD_BAD_PARAM;
        }
        else
        {
            eDec = 1;
            data = (MSD_U16)TOPAZ_MSD_GET_RATE_LIMIT_PER_FRAME(rate,eDec);
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
            data = (MSD_U16)TOPAZ_MSD_GET_RATE_LIMIT_PER_BYTE(rate,eDec);
        }
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_EGRESS_RATE_CTRL, 0, 7, (MSD_U16)eDec);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_EGRESS_RATE_CTRL2, 0, 14, (MSD_U16)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write TOPAZ_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_grcSetEgressRate Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Topaz_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the TOPAZ_MSD_ELIMIT_MODE of
*                                TOPAZ_MSD_ELIMIT_LAYER1,
*                                TOPAZ_MSD_ELIMIT_LAYER2, or
*                                TOPAZ_MSD_ELIMIT_LAYER3 (see Topaz_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with TOPAZ_MSD_ELIMIT_MODE of
*                                TOPAZ_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Topaz_grcGetEgressRate
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT TOPAZ_MSD_ELIMIT_MODE *mode,
	OUT MSD_U32	*rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        eRate, eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Topaz_grcGetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    if((retVal = Topaz_grcGetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_grcGetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_EGRESS_RATE_CTRL,0,7,&eDec);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_EGRESS_RATE_CTRL2,0,12,&eRate );
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    if (*mode == TOPAZ_MSD_ELIMIT_FRAME)    
    {
        *rate = TOPAZ_MSD_GET_RATE_LIMIT_PER_FRAME(eRate, eDec);
    }
    else
    {
        /* Count Per Byte */
        *rate = TOPAZ_MSD_GET_RATE_LIMIT_PER_BYTE(eRate, eDec);
    }

    MSD_DBG_INFO(("Topaz_grcGetEgressRate Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            TOPAZ_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            TOPAZ_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            TOPAZ_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            TOPAZ_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - TOPAZ_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Topaz_grcSetELimitMode
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
    IN  TOPAZ_MSD_ELIMIT_MODE      mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_grcSetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    data = (MSD_U16)mode & 0x3;

    /* Set the Elimit mode.            */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_EGRESS_RATE_CTRL2, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_grcSetELimitMode Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Topaz_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            TOPAZ_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            TOPAZ_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            TOPAZ_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            TOPAZ_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - TOPAZ_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Topaz_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT TOPAZ_MSD_ELIMIT_MODE  *mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_grcGetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    /* Get the Elimit mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_EGRESS_RATE_CTRL2, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    *mode = data;

    MSD_DBG_INFO(("Topaz_grcGetELimitMode Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT TOPAZ_MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case TOPAZ_MSD_DUPLEX_HALF:
        data = 0x1;
        break;
    case TOPAZ_MSD_DUPLEX_FULL:
        data = 0x3;
        break;
    case TOPAZ_MSD_DUPLEX_AUTO:
        data = 0x0;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 2, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetDuplex Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT TOPAZ_MSD_PORT_FORCE_DUPLEX_MODE  *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 2, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    switch (data)
    {
    case 0x1:
        *mode = TOPAZ_MSD_DUPLEX_HALF;
        break;
    case 0x3:
        *mode = TOPAZ_MSD_DUPLEX_FULL;
        break;
    case 0:
    case 0x2:
        *mode = TOPAZ_MSD_DUPLEX_AUTO;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }


    MSD_DBG_INFO(("Topaz_gprtGetDuplex Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetDuplexStatus
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
MSD_STATUS Topaz_gprtGetDuplexStatus
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
	
	MSD_DBG_INFO(("Topaz_gprtGetDuplexStatus Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PORT_STATUS, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gprtGetDuplexStatus Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Topaz_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT TOPAZ_MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 4, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    if ((data & 0x1) == 0)
        *mode = TOPAZ_PORT_DO_NOT_FORCE_LINK;
    else if ((data & 0x3) == 3)
        *mode = TOPAZ_PORT_FORCE_LINK_UP;
    else if ((data & 0x3) == 1)
        *mode = TOPAZ_PORT_FORCE_LINK_DOWN;

    MSD_DBG_INFO(("Topaz_gprtGetForceLink Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT TOPAZ_MSD_PORT_FORCE_LINK_MODE  mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case TOPAZ_PORT_DO_NOT_FORCE_LINK:
        data = 0;
        break;
    case TOPAZ_PORT_FORCE_LINK_UP:
        data = 3;
        break;
    case TOPAZ_PORT_FORCE_LINK_DOWN:
        data = 1;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mpde).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 4, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetForceLink Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetLinkState
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
MSD_STATUS Topaz_gprtGetLinkState
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
	
	MSD_DBG_INFO(("Topaz_gprtGetLinkState Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PORT_STATUS, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *state);

    MSD_DBG_INFO(("Topaz_gprtGetLinkState Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - TOPAZ_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Topaz_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN TOPAZ_MSD_PORT_FORCED_SPEED_MODE  mode
)
{
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;
    MSD_U16			data1;
    MSD_U16			data2;
    MSD_U16			linkStatus;

    MSD_DBG_INFO(("Topaz_gprtSetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /*Get the force link status and then force link down*/
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 4, 2, &linkStatus)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get link status Failed\n"));
        return retVal;
    }
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 4, 2, 1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Force Link down failed\n"));
        return retVal;
    }


    /* Set the ForceSpeed bit.  */
    if (mode == Topaz_PORT_DO_NOT_FORCE_SPEED)
    {
        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 13, 1, 0)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Force Speed bit error\n"));
            return retVal;
        }
    }
    else
    {
        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 13, 1, 1)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Force Speed bit error\n"));
            return retVal;
        }
        switch (mode)
        {
        case Topaz_PORT_FORCE_SPEED_10M:
            data1 = 0;
            data2 = 0;
            break;
        case Topaz_PORT_FORCE_SPEED_100M:
            data1 = 0;
            data2 = 1;
            break;
        case Topaz_PORT_FORCE_SPEED_200M:
            if (port != 0)
            {
                MSD_DBG_ERROR(("Failed (Bad Port), Only port 0 support 200M.\n"));
				return MSD_NOT_SUPPORTED;
            }
            data1 = 1;
            data2 = 1;
            break;
        case Topaz_PORT_FORCE_SPEED_1000M:
            data1 = 0;
            data2 = 2;
            break;
        case Topaz_PORT_FORCE_SPEED_2_5G:
            if (port != 5)
            {
                MSD_DBG_ERROR(("Failed (Bad Port), Only port 5 support 2.5G.\n"));
				return MSD_NOT_SUPPORTED;
            }
            data1 = 1;
            data2 = 3;
            break;
        default:
            MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
            return MSD_BAD_PARAM;
        }

        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 12, 1, data1)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Alternate Speed bit error\n"));
            return retVal;
        }
        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 0, 2, data2)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Speed bit error\n"));
            return retVal;
        }
    }

    /*Set back the force link status*/
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 4, 2, linkStatus)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Set back force link status error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetForceSpeed Called.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - TOPAZ_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Topaz_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT TOPAZ_MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
    MSD_U16          data1;
    MSD_U16          data2;
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the ForceSpeed bit.  */
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 13, 1, &data1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get force Speed bit error\n"));
        return retVal;
    }

    if (data1 == 0)
    {
        *mode = Topaz_PORT_DO_NOT_FORCE_SPEED;
    }
    else
    {
        if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 12, 1, &data1)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Get Alternate Speed bit error\n"));
            return retVal;
        }
        if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 0, 2, &data2)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Get Speed bit error\n"));
            return retVal;
        }
        if (data1 == 1)
        {
            switch (data2)
            {
            case 0:
                *mode = Topaz_PORT_FORCE_SPEED_10M;
                break;
            case 1:
                *mode = Topaz_PORT_FORCE_SPEED_200M;
                break;
            case 2:
                *mode = Topaz_PORT_FORCE_SPEED_1000M;
                break;
            case 3:
                *mode = Topaz_PORT_FORCE_SPEED_2_5G;
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
                *mode = Topaz_PORT_FORCE_SPEED_10M;
                break;
            case 1:
                *mode = Topaz_PORT_FORCE_SPEED_100M;
                break;
            case 2:
                *mode = Topaz_PORT_FORCE_SPEED_1000M;
                break;
            case 3:
                *mode = Topaz_PORT_FORCE_SPEED_2_5G;
                break;
            default:
                MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
                return MSD_FAIL;
            }
        }
    }

    MSD_DBG_INFO(("Topaz_gprtGetForceSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - TOPAZ_MSD_PORT_SPEED type.
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
MSD_STATUS Topaz_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT TOPAZ_MSD_PORT_SPEED   *speed
)
{
	MSD_U16          data,data1,data2;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Topaz_gprtGetSpeed Called.\n"));	

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);


	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, TOPAZ_QD_REG_PORT_STATUS, 8, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/*Get AltSpeed */
	retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PHY_CONTROL, 12, 1, &data1);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PHY_CONTROL Register.\n"));
		return retVal;
	}

	/*Get C_Mode */
	data2 = 0;
	retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_STATUS, 0, 4, &data2);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to read TOPAZ_QD_REG_PHY_CONTROL Register.\n"));
		return retVal;
	}

	if (data1 == 0)
	{
		switch (data)
		{
		case 0:
			*speed = Topaz_PORT_SPEED_10_MBPS;
			break;
		case 1:
			if (port == 0)
			{
				if (data2 == 0 || data2 == 1)
				{
					*speed = Topaz_PORT_SPEED_200_MBPS;
					break;
				}
			}
			*speed = Topaz_PORT_SPEED_100_MBPS;
			break;
		case 2:
			*speed = Topaz_PORT_SPEED_1000_MBPS;
			break;
		case 3:
			*speed = Topaz_PORT_SPEED_2_5_GBPS;
			break;
		default:
			*speed = Topaz_PORT_SPEED_UNKNOWN;
			break;
		}
	}
	else
	{
		switch (data)
		{
		case 0:
			*speed = Topaz_PORT_SPEED_10_MBPS;
			break;
		case 1:
			*speed = Topaz_PORT_SPEED_200_MBPS;
			break;
		case 2:
			*speed = Topaz_PORT_SPEED_1000_MBPS;
			break;
		case 3:
			*speed = Topaz_PORT_SPEED_2_5_GBPS;
			break;
		default:
			*speed = Topaz_PORT_SPEED_UNKNOWN;
			break;
		}
	}

    MSD_DBG_INFO(("Topaz_gprtGetSpeed Called.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetVlanPorts
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
MSD_STATUS Topaz_gprtGetVlanPorts
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

    MSD_DBG_INFO(("Topaz_gprtGetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_VLAN_MAP, 0, 7, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PORT_VLAN_MAP register error\n"));
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

    MSD_DBG_INFO(("Topaz_gprtGetVlanPorts Exit.\n"));
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
MSD_STATUS Topaz_gprtSetVlanPorts
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

    MSD_DBG_INFO(("Topaz_gprtSetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_VLAN_MAP, 0, 7, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_VLAN_MAP register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetVlanPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetFrameMode
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
MSD_STATUS Topaz_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT TOPAZ_MSD_FRAME_MODE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 8, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_FRAME_MODE)data;

    MSD_DBG_INFO(("Topaz_gprtGetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetFrameMode
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
MSD_STATUS Topaz_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  TOPAZ_MSD_FRAME_MODE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case TOPAZ_FRAME_MODE_NORMAL:
    case TOPAZ_FRAME_MODE_DSA:
    case TOPAZ_FRAME_MODE_PROVIDER:
    case TOPAZ_FRAME_MODE_ETHER_TYPE_DSA:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 8, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Topaz_gprtSetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - TOPAZ_PORT_STP_STATE
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
MSD_STATUS Topaz_gprtGetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT TOPAZ_MSD_PORT_STP_STATE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 0, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (TOPAZ_MSD_PORT_STP_STATE)data;

    MSD_DBG_INFO(("Topaz_gprtGetPortState Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - TOPAZ_PORT_STP_STATE
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
MSD_STATUS Topaz_gprtSetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  TOPAZ_MSD_PORT_STP_STATE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case TOPAZ_MSD_PORT_DISABLE:
    case TOPAZ_MSD_PORT_BLOCKING:
    case TOPAZ_MSD_PORT_LEARNING:
    case TOPAZ_MSD_PORT_FORWARDING:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }


    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL, 0, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Topaz_gprtSetPortState Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetCutThrouthEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  TOPAZ_CUT_THROUGH  *cutThru
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetCutThrouthEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if(cutThru->cutThruQueue > 0xf)
    {
        MSD_DBG_ERROR(("Failed (Bad Cut Through Queue).\n"));
        return MSD_BAD_PARAM;
    }

    data = ((cutThru->enable & 0x1) << 8) | cutThru->cutThruQueue;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_CT_CONTROL, 0, 9, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_CT_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetCutThrouthEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetCutThrouthEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT  TOPAZ_CUT_THROUGH  *cutThru
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetCutThrouthEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_CT_CONTROL, 0, 9, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_CT_CONTROL register error\n"));
        return retVal;
    }

    cutThru->enable = (data >> 8) & 0x1;
    cutThru->cutThruQueue = data & 0xf;

    MSD_DBG_INFO(("Topaz_gprtGetCutThrouthEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetIgnoreFcs
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

    MSD_DBG_INFO(("Topaz_gprtSetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetIgnoreFcs
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

    MSD_DBG_INFO(("Topaz_gprtGetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetAllowBadFcs
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

    MSD_DBG_INFO(("Topaz_gprtSetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetAllowBadFcs
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

    MSD_DBG_INFO(("Topaz_gprtGetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtGetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtSetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetPauseLimitIn Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Topaz_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_LIMIT_PAUSE_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtGetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetOutQueueSize
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

    MSD_DBG_INFO(("Topaz_gprtGetOutQueueSize Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

	if (queue >= TOPAZ_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_Q_COUNTER, 12, 4, queue);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_Q_COUNTER, 0, 9, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtGetOutQueueSize Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetMapDA
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

    MSD_DBG_INFO(("Topaz_gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 7, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetMapDA
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

    MSD_DBG_INFO(("Topaz_gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 7, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetPortLocked
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

    MSD_DBG_INFO(("Topaz_gprtSetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetPortLocked
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

    MSD_DBG_INFO(("Topaz_gprtGetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetHoldAt1
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

    MSD_DBG_INFO(("Topaz_gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetHoldAt1
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

    MSD_DBG_INFO(("Topaz_gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetInt0nAgeOut
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

    MSD_DBG_INFO(("Topaz_gprtSetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetInt0nAgeOut
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

    MSD_DBG_INFO(("Topaz_gprtGetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetRefreshLocked
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

    MSD_DBG_INFO(("Topaz_gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetRefreshLocked
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

    MSD_DBG_INFO(("Topaz_gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetIgnoreWrongData
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

    MSD_DBG_INFO(("Topaz_gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 12, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetIgnoreWrongData
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

    MSD_DBG_INFO(("Topaz_gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PAV, 12, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtSetARPMirror
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

    MSD_DBG_INFO(("Topaz_gprtSetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 6, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gprtSetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtGetARPMirror
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

    MSD_DBG_INFO(("Topaz_gprtGetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_CONTROL2, 6, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get TOPAZ_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Topaz_gprtGetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Topaz_gprtRegDump
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
		{ "Reserved(0x10)" },
		{ "Reserved(0x11)" },
		{ "Reserved(0x12)" },
		{ "Reserved(0x13)" },
		{ "Reserved(0x14)" },
		{ "Reserved(0x15)" },
		{ "LED Control(0x16)" },
		{ "Reserved(0x17)" },
		{ "Port IEEE Priority Remapping Registers(0x18)" },
		{ "Port IEEE Priority Remapping Registers(0x19)" },
		{ "Reserved(0x1a)" },
		{ "Queue Counter Registers(0x1b)" },
		{ "Port Queue Control register(0x1c)" },
		{ "Port Queue Control 2 register(0x1d)" },
		{ "Cut Through Register(0x1e)" },
		{ "Debug Counter(0x1f)" }
	};

	MSD_DBG_INFO(("Topaz_gprtRegDump Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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

	MSD_DBG_INFO(("Topaz_gprtRegDump Exit.\n"));
	return MSD_OK;
}

