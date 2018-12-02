#include <msdCopyright.h>

/********************************************************************************
* msdAdvVct.c
*
* DESCRIPTION:
*       API definitions for Virtual Cable Test
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdAdvVct.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdVctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
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
MSD_STATUS msdVctGetAdvCableDiag
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_ADV_VCT_MODE  mode,
	OUT MSD_ADV_CABLE_STATUS  *cableStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag != NULL)
        return dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag(dev, portNum, mode, cableStatus);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdVctGetAdvExtendStatus
*
* DESCRIPTION:
*       This routine retrieves extended cable status, such as Pair Poloarity,
*       Pair Swap, and Pair Skew. Note that this routine will be success only
*       if 1000Base-T Link is up.
*       DSP based cable lenPeridot_gth is also provided.
*
* INPUTS:
*       devNum  - physical devie number
*       dev  - pointer to Peridot_gt driver structure returned from mdLoadDriver
*       portNum - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdVctGetAdvExtendStatus
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	OUT MSD_ADV_EXTENDED_STATUS  *extendedStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus != NULL)
		return dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus(dev, portNum, extendedStatus);
	else
		return MSD_NOT_SUPPORTED; 
}
