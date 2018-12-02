#include <msdCopyright.h>
/*******************************************************************************
* Peridot_msdBrgStu.c
*
* DESCRIPTION:
*       API for Marvell Virtual Cable Tester.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <peridot/include/api/Peridot_msdBrgStu.h>

/*******************************************************************************
* Peridot_gstuAddEntry
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
MSD_STATUS Peridot_gstuAddEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_STU_ENTRY    *stuEntry
)
{
	PERIDOT_MSD_STU_ENTRY entry;
	MSD_LPORT lport = 0;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY in Peridot_gstuAddEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		entry.portState[lport] = stuEntry->portState[lport];
	}
	return Peridot_gstuAddEntry(dev, &entry);
}

MSD_STATUS Peridot_gstuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_STU_ENTRY *stuEntry
)
{
	PERIDOT_MSD_STU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Peridot_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(PERIDOT_MSD_STU_ENTRY));
	entry.sid = stuEntry->sid;

	retVal = Peridot_gstuGetEntryNext(dev, &entry);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;
	msdMemCpy(stuEntry->portState, entry.portState, 11);

	return retVal;
}

/*******************************************************************************
* Peridot_gstuDelEntry
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
MSD_STATUS Peridot_gstuDelEntryIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U16 sid
)
{
	PERIDOT_MSD_STU_ENTRY entry;

    msdMemSet((void*)(&entry), 0, sizeof(PERIDOT_MSD_STU_ENTRY));
	entry.sid = sid;

	return Peridot_gstuDelEntry(dev, &entry);
}

/*******************************************************************************
* Peridot_gstuFindSidEntry
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       stuEntry - contains the SID to searche for
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       stuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no such entry.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        Valid SID is 0 ~ 63.
*
*******************************************************************************/
MSD_STATUS Peridot_gstuFindSidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16 sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
)
{
	MSD_STATUS retVal;
	PERIDOT_MSD_STU_ENTRY entry;
	MSD_LPORT lport = 0;

	if (NULL == stuEntry || NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY or found in Peridot_gstuFindSidEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.sid = sid;

	retVal = Peridot_gstuFindSidEntry(dev, &entry, found);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		stuEntry->portState[lport] = entry.portState[lport];
	}

	return retVal;
}

/*******************************************************************************
* Peridot_gstuGetEntryCountIntf
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
MSD_STATUS Peridot_gstuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16 *count
)
{
    MSD_STATUS retVal = MSD_OK;
    PERIDOT_MSD_STU_ENTRY entry;

    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count in Peridot_gstuGetEntryCountIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    *count = 0;
    msdMemSet((void*)(&entry), 0, sizeof(PERIDOT_MSD_STU_ENTRY));
    entry.sid = 0x3f;

    while (retVal == MSD_OK)
    {
        retVal = Peridot_gstuGetEntryNext(dev, &entry);
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
