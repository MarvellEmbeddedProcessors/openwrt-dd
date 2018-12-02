#include <msdCopyright.h>

/********************************************************************************
* msdTrunk.c
*
* DESCRIPTION:
*       API definitions for Marvell Trunk functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdTrunk.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdTrunkMaskTableGet
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       devNum  - physical devie number
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    OUT MSD_U32  *trunkMask
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable != NULL)
        return dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable(dev, maskNum, trunkMask);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdTrunkMaskTableSet
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       devNum  - physical devie number
*       maskNum - Mask Number (0~7).
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    IN  MSD_U32  trunkMask
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable != NULL)
		return dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable(dev, maskNum, trunkMask);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdTrunkHashTrunkGet
*
* DESCRIPTION:
*       This routine gets enable status for hash DA & SA for TrunkMask selection. 
*       Trunk load balancing is accomplished by using the frame's DA and SA fields 
*       to access one of eight Trunk Masks.
*		When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk != NULL)
        return dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk(dev, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdTrunkHashTrunkSet
*
* DESCRIPTION:
*       This routine enable or disable hash DA & SA for TrunkMask selection. Trunk 
*       load balancing is accomplished by using the frame's DA and SA fields to 
*		access one of eight Trunk Masks.
*       When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical devie number
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkSet
(
    IN  MSD_U8  devNum,
    IN  MSD_BOOL  en
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk != NULL)
		return dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk(dev, en);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdTrunkRoutingGet
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical devie number
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    OUT MSD_U32  *trunkRoute
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting != NULL)
        return dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting(dev, trunkId, trunkRoute);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdTrunkRoutingSet
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical devie number
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    IN  MSD_U32  trunkRoute
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting != NULL)
		return dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting(dev, trunkId, trunkRoute);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortLAGPortGet
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*
* INPUTS:
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a LAG,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
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
MSD_STATUS msdPortLAGPortGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en,
    OUT MSD_U32  *LAGId
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort != NULL)
        return dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort(dev, portNum, en, LAGId);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortLAGPortSet
*
* DESCRIPTION:
*        This function enables/disables trunk bit and sets the LAG ID.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       en - MSD_TRUE to make the port be a member of a LAG with the given LAG ID.
*             MSD_FALSE, otherwise.
*       LAGId - valid ID is 0 ~ 31
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
*		 None.
*
*******************************************************************************/
MSD_STATUS msdPortLAGPortSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en,
    IN  MSD_U32  LAGId
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort != NULL)
        return dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort(dev, portNum, en, LAGId);
    else
        return MSD_NOT_SUPPORTED;
}


