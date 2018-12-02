#include <msdCopyright.h>

/********************************************************************************
* msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Marvell VTU functionality 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <msdBrgVtu.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdVlanEntryFind
*
* DESCRIPTION:
*       Find VTU entry for a specific VID(13bits data for Peridot and 12bits for others),
*       it will return the entry, if found, along with its associated data
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       entry  - pointer to VTU entry structure
*       found  - 1/0 for found / not found
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVlanEntryFind
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuFindVidEntry != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuFindVidEntry(dev, vlanId, entry, found);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdVlanExists
*
* DESCRIPTION:
*       Check if the vlan entry with a specific vlan id exist or not in VTU table.
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       isExists   - 1/0 for exist / not exist
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanExists
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.VTUObj.gvtuExistVidEntry != NULL)
        return dev->SwitchDevObj.VTUObj.gvtuExistVidEntry(dev, vlanId, isExists);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdVlanEntryNextGet
*
* DESCRIPTION:
*       Gets next valid vlan entry from the specified vid.
*
* INPUTS:
*       vlanId  - the vlan id to start the search
*
* OUTPUTS:
*       vlanEntry - next valid vlan entry
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*       MSD_NO_SUCH - no more entries
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryNextGet
(
	IN  MSD_U8 devNum,
    IN  MSD_U16 vlanId,
	OUT MSD_VTU_ENTRY *vlanEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
    vlanEntry->vid = vlanId;
	if (dev->SwitchDevObj.VTUObj.gvtuGetEntryNext != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuGetEntryNext(dev, vlanEntry);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdVlanAllDelete
*
* DESCRIPTION:
*       Removes all entries from VTU Table.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanAllDelete
(
    IN  MSD_U8  devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuFlush != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuFlush(dev);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanEntryAdd
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       devNum - physical device number
*       entry  - pointer to VTU entry structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryAdd
(
    IN  MSD_U8   devNum,
    IN  MSD_VTU_ENTRY *vtuEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuAddEntry != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuAddEntry(dev, vtuEntry);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanEntryDelete
*
* DESCRIPTION:
*       This routine deletes VTU entry specified by VID in vtuEntry.
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryDelete
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuDelEntry != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuDelEntry(dev, vlanId);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanModeSet
*
* DESCRIPTION:
*       Sets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*       MSD_VTU_MODE - VTU Mode specified in the enumeration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_VTU_MODE  mode
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuSetMode != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuSetMode(dev, mode);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanModeGet
*
* DESCRIPTION:
*       Gets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       MSD_VTU_MODE - VTU Mode specified in the enumeration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_MODE  *mode
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuGetMode != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuGetMode(dev, mode);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanViolationGet
*
* DESCRIPTION:
*       Gets VTU Violation data
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS msdVlanViolationGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_INT_STATUS  *vtuIntStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuGetViolation != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuGetViolation(dev, vtuIntStatus);
	else
		return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanEntryCountGet
*
* DESCRIPTION:
*       Gets number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       count  - number of valid entries
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS msdVlanEntryCountGet
(
    IN  MSD_U8   devNum,
    OUT MSD_U16  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.VTUObj.gvtuGetEntryCount != NULL)
        return dev->SwitchDevObj.VTUObj.gvtuGetEntryCount(dev, count);
    else
        return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanMemberTagSet
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, update the memberTag
*       to Allow/Block vlan on the port. If it not exist, return MSD_NO_SUCH
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*       memberTag - enum element of MSD_PORT_MEMBER_TAG
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanMemberTagSet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.VTUObj.gvtuSetMemberTag != NULL)
        return dev->SwitchDevObj.VTUObj.gvtuSetMemberTag(dev, vlanId, portNum, memberTag);
    else
        return MSD_NOT_SUPPORTED;
}
/*******************************************************************************
* msdVlanMemberTagGet
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, get the memberTag
*       on the port. If it not exist, return MSD_NO_SUCH.
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*
* OUTPUTS:
*       memberTag - enum element of MSD_VLAN_MEMBER_TAG
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanMemberTagGet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.VTUObj.gvtuGetMemberTag != NULL)
        return dev->SwitchDevObj.VTUObj.gvtuGetMemberTag(dev, vlanId, portNum, memberTag);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdVtuDump
*
* DESCRIPTION:
*       Finds all VTU Entries and print it out.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVtuDump
(
	IN  MSD_U8  devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.VTUObj.gvtuDump != NULL)
		return dev->SwitchDevObj.VTUObj.gvtuDump(dev);
	else
		return MSD_NOT_SUPPORTED;
}

