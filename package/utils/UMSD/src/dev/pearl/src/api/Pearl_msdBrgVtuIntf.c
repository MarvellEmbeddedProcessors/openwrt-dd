#include <msdCopyright.h>

/*******************************************************************************
* Pearl_msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Vlan Translation Unit for 802.1Q.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <pearl/include/api/Pearl_msdBrgVtu.h>

/*******************************************************************************
* Pearl_gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID and VTU Page, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       vtuEntry - contains the VID and VTU Page to search for.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       vtuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuFindVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16    vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
)
{
    MSD_LPORT            lport;
    PEARL_MSD_VTU_ENTRY tempEntry;
    MSD_STATUS           retVal;

    if (NULL == entry || NULL == found)
    {
        MSD_DBG_ERROR(("Input pointer param MSD_VTU_ENTRY or found in Pearl_gvtuFindVidEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    if (vlanId > 0xfff)
    {
        MSD_DBG_ERROR(("Input bad vlanId. \n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId;


    retVal = Pearl_gvtuFindVidEntry(dev, &tempEntry, found);

    msdMemSet((void*)entry, 0, sizeof(MSD_VTU_ENTRY));

    if (*found == MSD_FALSE)
        return retVal;

    entry->fid = tempEntry.DBNum;
    entry->vid = tempEntry.vid;
    entry->vidPolicy = tempEntry.vidPolicy;
    entry->sid = tempEntry.sid;
    entry->vidExInfo.useVIDFPri = tempEntry.vidExInfo.useVIDFPri;
    entry->vidExInfo.vidFPri = tempEntry.vidExInfo.vidFPri;   

    for (lport = 0; lport<dev->numOfPorts; lport++)
    {
        entry->memberTagP[lport] = tempEntry.memberTagP[lport];
    }
    return retVal;
}

MSD_STATUS Pearl_gvtuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_VTU_ENTRY *vtuEntry
)
{
	PEARL_MSD_VTU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == vtuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Peridot_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(PEARL_MSD_VTU_ENTRY));
	entry.vid = vtuEntry->vid;

	retVal = Pearl_gvtuGetEntryNext(dev, &entry);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)vtuEntry, 0, sizeof(MSD_VTU_ENTRY));
	vtuEntry->vid = entry.vid;
	vtuEntry->fid = entry.DBNum;
	vtuEntry->sid = entry.sid;
	vtuEntry->vidPolicy = entry.vidPolicy;	
	vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
	vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
	msdMemCpy(vtuEntry->memberTagP, entry.memberTagP, 7);

	return retVal;
}

/*******************************************************************************
* Pearl_gvtuExistVidEntryIntf
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
MSD_STATUS Pearl_gvtuExistVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
)
{
    PEARL_MSD_VTU_ENTRY tempEntry;
    MSD_STATUS           retVal;

    if (NULL == isExists)
    {
        MSD_DBG_ERROR(("Input pointer param isExists in Pearl_gvtuExistVidEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    if (vlanId > 0xfff)
    {
        MSD_DBG_ERROR(("Input bad vlanId. \n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId;

    retVal = Pearl_gvtuFindVidEntry(dev, &tempEntry, isExists);

    return retVal;
}
/*******************************************************************************
* Pearl_gvtuFlush
*
* DESCRIPTION:
*       This routine removes all entries from VTU Table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuFlushIntf
(
    IN  MSD_QD_DEV       *dev
)
{
	return Pearl_gvtuFlush(dev);
}
/*******************************************************************************
* Pearl_gvtuAddEntry
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       vtuEntry - vtu entry to insert to the VTU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuAddEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_VTU_ENTRY *vtuEntry
)
{
	MSD_LPORT  lport;
	PEARL_MSD_VTU_ENTRY entry;

	if (NULL == vtuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Pearl_gvtuAddEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.DBNum = vtuEntry->fid;
	entry.vid = vtuEntry->vid;
	entry.vidPolicy = vtuEntry->vidPolicy;
	entry.sid = vtuEntry->sid;
	entry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
	entry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		entry.memberTagP[lport]= vtuEntry->memberTagP[lport] ;
	}

	return Pearl_gvtuAddEntry(dev, &entry);
}
/*******************************************************************************
* Pearl_gvtuDelEntry
*
* DESCRIPTION:
*       Deletes VTU entry specified by user.
*
* INPUTS:
*       vtuEntry - the VTU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - if specified address entry does not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuDelEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_U16 vlanId
)
{
	PEARL_MSD_VTU_ENTRY entry;

    msdMemSet((void*)(&entry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    entry.vid = vlanId;

	return Pearl_gvtuDelEntry(dev, &entry);
}
/*******************************************************************************
* Pearl_gvtuGetViolation
*
* DESCRIPTION:
*       Get VTU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuGetViolationIntf
(
    IN  MSD_QD_DEV         *dev,
    OUT MSD_VTU_INT_STATUS *vtuIntStatus
)
{
	PEARL_MSD_VTU_INT_STATUS vtuint;
	MSD_STATUS           retVal;

	msdMemSet((void*)(&vtuint), 0, sizeof(PEARL_MSD_VTU_INT_STATUS));
	retVal = Pearl_gvtuGetViolation(dev, &vtuint);
	if (MSD_OK != retVal)
		return retVal;
	
	if (NULL == vtuIntStatus)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_INT_STATUS in Pearl_gvtuGetViolationIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)vtuIntStatus, 0, sizeof(MSD_VTU_INT_STATUS));
    vtuIntStatus->vtuIntCause.fullVio = (vtuint.vtuIntCause & MSD_VTU_FULL_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
    vtuIntStatus->vtuIntCause.memberVio = (vtuint.vtuIntCause & MSD_VTU_MEMBER_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
    vtuIntStatus->vtuIntCause.missVio = (vtuint.vtuIntCause & MSD_VTU_MISS_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
	vtuIntStatus->spid = vtuint.spid;
	vtuIntStatus->vid = vtuint.vid;

	return retVal;
}
/*******************************************************************************
* Pearl_gvtuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numEntries  - number of valid entries
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
MSD_STATUS Pearl_gvtuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16    *count
)
{
    MSD_STATUS           retVal = MSD_OK;
    PEARL_MSD_VTU_ENTRY tempEntry;

    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input pointer param count in Pearl_gvtuGetEntryCountIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    *count = 0;
    msdMemSet((void*)(&tempEntry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    tempEntry.vid = 0xfff;

    while (retVal ==MSD_OK)
    {
        retVal = Pearl_gvtuGetEntryNext(dev, &tempEntry);
        if (retVal == MSD_NO_SUCH)
            break;
        (*count)++;
        if (tempEntry.vid == 0xfff)
            break;

        if (retVal != MSD_OK)
            return retVal;
    }

    return MSD_OK;
}
/*******************************************************************************
* Pearl_gvtuSetMemberTagIntf
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
MSD_STATUS Pearl_gvtuSetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
)
{
    MSD_STATUS retVal;
    PEARL_MSD_VTU_ENTRY tempEntry;
    MSD_U8 phyPort; /* Physical port.*/
    MSD_BOOL found;

    phyPort = MSD_LPORT_2_PORT(portNum);
    if (phyPort == MSD_INVALID_PORT || vlanId > 0xfff)
    {
        MSD_DBG_ERROR(("Failed (Bad Port or Bad vlanId) in Pearl_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }
    if (memberTag < MSD_MEMBER_EGRESS_UNMODIFIED || memberTag > MSD_NOT_A_MEMBER)
    {
        MSD_DBG_ERROR(("Failed (Bad memberTag) in Pearl_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId;

    retVal = Pearl_gvtuFindVidEntry(dev, &tempEntry, &found);
    if (MSD_OK != retVal)
        return retVal;

    if (found == MSD_FALSE)
        return MSD_NO_SUCH;

    /*Update the memberTag*/
    tempEntry.memberTagP[phyPort] = memberTag;
    retVal = Pearl_gvtuAddEntry(dev, &tempEntry);

    return retVal;
}
/*******************************************************************************
* Pearl_gvtuGetMemberTagIntf
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
MSD_STATUS Pearl_gvtuGetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
)
{
    MSD_STATUS retVal;
    PEARL_MSD_VTU_ENTRY tempEntry;
    MSD_U8 phyPort; /* Physical port.*/
    MSD_BOOL found;

    phyPort = MSD_LPORT_2_PORT(portNum);
    if (phyPort == MSD_INVALID_PORT || vlanId > 0xfff)
    {
        MSD_DBG_ERROR(("Failed (Bad Port or Bad vlanId) in Pearl_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(PEARL_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId;

    retVal = Pearl_gvtuFindVidEntry(dev, &tempEntry, &found);
    if (MSD_OK != retVal)
        return retVal;

    if (found == MSD_FALSE)
        return MSD_NO_SUCH;

    /*Get the memberTag*/
    *memberTag = tempEntry.memberTagP[phyPort];

    return retVal;
}

