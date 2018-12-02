#include <msdCopyright.h>

/********************************************************************************
* msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID Translation Unit
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdBrgStu.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdStuEntryAdd
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on user input.
*
* INPUTS:
*       stuEntry  - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
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
MSD_STATUS msdStuEntryAdd
(
    IN MSD_U8 devNum,
    IN MSD_STU_ENTRY *stuEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.STUObj.gstuAddEntry != NULL)
		return dev->SwitchDevObj.STUObj.gstuAddEntry(dev, stuEntry);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdStuEntryNextGet
*
* DESCRIPTION:
*       Gets next valid STU entry from the specified SID.
*
* INPUTS:
*       sid  - the SID to start the search
*
* OUTPUTS:
*       stuEntry - next valid stu entry
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
MSD_STATUS msdStuEntryNextGet
(
	IN  MSD_U8 devNum,
	IN  MSD_U16 sid,
	OUT MSD_STU_ENTRY *stuEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	stuEntry->sid = sid;
	if (dev->SwitchDevObj.STUObj.gstuGetEntryNext != NULL)
		return dev->SwitchDevObj.STUObj.gstuGetEntryNext(dev, stuEntry);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdStuEntryDelete
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       devNum - physical device number
*       sid    - SID ID, valid value is 0 ~ 63
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
MSD_STATUS msdStuEntryDelete
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 sid
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.STUObj.gstuDelEntry != NULL)
		return dev->SwitchDevObj.STUObj.gstuDelEntry(dev, sid);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdStuEntryFind
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       devNum - physical device number
*       sid    - SID ID, valid value is 0 ~ 63
*
* OUTPUTS:
*       stuEntry - pointer to the STU entry structure
*       found    - 1/0 for found / not found
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
MSD_STATUS msdStuEntryFind
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.STUObj.gstuFindSidEntry != NULL)
		return dev->SwitchDevObj.STUObj.gstuFindSidEntry(dev, sid, stuEntry, found);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdStuEntryCountGet
*
* DESCRIPTION:
*       Gets number of valid entries in the STU table
*
* INPUTS:
*       devNum - physical device number
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
*
*******************************************************************************/
MSD_STATUS msdStuEntryCountGet
(
    IN  MSD_U8  devNum,
    OUT MSD_U16 *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.STUObj.gstuGetEntryCount != NULL)
        return dev->SwitchDevObj.STUObj.gstuGetEntryCount(dev, count);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdStuDump
*
* DESCRIPTION:
*       Finds all STU Entries and print it out.
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
MSD_STATUS msdStuDump
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

	if (dev->SwitchDevObj.STUObj.gstuDump != NULL)
		return dev->SwitchDevObj.STUObj.gstuDump(dev);
	else
		return MSD_NOT_SUPPORTED;
}