#include <msdCopyright.h>

/*******************************************************************************
* Pearl_msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID (VTU 802.1s Port State Information Database) 
*        Translation Unit.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdBrgStu.h>

/*******************************************************************************
* Pearl_gstuAddEntry
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on SID.
*
* INPUTS:
*       stuEntry    - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_gstuAddEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_STU_ENTRY    *stuEntry
)
{
    PEARL_MSD_STU_ENTRY entry; 
    MSD_LPORT lport = 0;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY in Pearl_gstuAddEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;
	for (lport = 0; lport < dev->numOfPorts; lport++)
	{
		entry.portState[lport] = stuEntry->portState[lport];
	}
	return Pearl_gstuAddEntry(dev, &entry);
}

MSD_STATUS Pearl_gstuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_STU_ENTRY *stuEntry
)
{
	PEARL_MSD_STU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Peridot_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(PEARL_MSD_STU_ENTRY));
	entry.sid = stuEntry->sid;

	retVal = Pearl_gstuGetEntryNext(dev, &entry);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;
	msdMemCpy(stuEntry->portState, entry.portState, 7);

	return retVal;
}


/*******************************************************************************
* Pearl_gstuDelEntry
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       stuEntry - the STU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_gstuDelEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_U16 sid
)
{
	PEARL_MSD_STU_ENTRY entry;

    msdMemSet((void*)(&entry), 0, sizeof(PEARL_MSD_STU_ENTRY));
	entry.sid = sid;

	return Pearl_gstuDelEntry(dev, &entry);
}

/*******************************************************************************
* msdStuEntryFind
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
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
MSD_STATUS Pearl_gstuFindSidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16    sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
)
{
	MSD_STATUS retVal;
	PEARL_MSD_STU_ENTRY entry;
	MSD_LPORT lport = 0;

	if (NULL == stuEntry || NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY or found in Pearl_gstuFindSidEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
    msdMemSet((void*)(&entry), 0, sizeof(PEARL_MSD_STU_ENTRY));
	entry.sid = sid;

	retVal = Pearl_gstuFindSidEntry(dev, &entry, found);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;
	for (lport = 0; lport < dev->numOfPorts; lport++)
	{
		stuEntry->portState[lport] = entry.portState[lport];
	}

	return retVal;
}

/*******************************************************************************
* Pearl_gstuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the STU table
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
MSD_STATUS Pearl_gstuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16 *count
)
{
    MSD_STATUS retVal = MSD_OK;
    PEARL_MSD_STU_ENTRY entry;

    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count in Pearl_gstuGetEntryCountIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    *count = 0;
    msdMemSet((void*)(&entry), 0, sizeof(PEARL_MSD_STU_ENTRY));
    entry.sid = 0x3f;

    while (retVal == MSD_OK)
    {
        retVal = Pearl_gstuGetEntryNext(dev, &entry);
        if (retVal == MSD_NO_SUCH)
            break;
        (*count)++;
        if (entry.sid == 0x3f)
            break;

        if (retVal != MSD_OK)
            return retVal;
    }

    return MSD_OK;
}

