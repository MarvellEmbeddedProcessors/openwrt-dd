#include <msdCopyright.h>

/*******************************************************************************
* Topaz_msdTCAM.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdTCAM.h>


/*******************************************************************************
* Topaz_gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 127)
*        tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gtcamLoadEntryIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    IN  MSD_TCAM_DATA        *tcamData
)
{
	TOPAZ_MSD_TCAM_DATA tcam;
	MSD_U8 i=0;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_gtcamLoadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcam.frameType = tcamData->frameType;
	tcam.frameTypeMask = tcamData->frameTypeMask;
	tcam.spv = tcamData->spv;
	tcam.spvMask = tcamData->spvMask;
	tcam.ppri = tcamData->ppri;
	tcam.ppriMask = tcamData->ppriMask;
	tcam.pvid = tcamData->pvid;
	tcam.pvidMask = tcamData->pvidMask;
	
	tcam.continu = tcamData->continu;
	tcam.interrupt = tcamData->interrupt;
	tcam.IncTcamCtr = tcamData->IncTcamCtr;
	tcam.vidOverride = tcamData->vidOverride;
	tcam.vidData = tcamData->vidData;
	tcam.nextId = tcamData->nextId;
	tcam.qpriOverride = tcamData->qpriOverride;
	tcam.qpriData = tcamData->qpriData;
	tcam.fpriOverride = tcamData->fpriOverride;
	tcam.fpriData = tcamData->fpriData;
	tcam.dpvSF = tcamData->dpvSF;
	tcam.dpvData = tcamData->dpvData;
	tcam.dpvMode = tcamData->dpvMode;
	tcam.unKnownFilter = tcamData->unKnownFilter;
	tcam.ldBalanceOverride = tcamData->ldBalanceOverride;
	tcam.ldBalanceData = tcamData->ldBalanceData;
	tcam.factionOverride = tcamData->factionOverride;
	tcam.factionData = tcamData->factionData;

	for (i = 0; i < 48; i++)
	{
		tcam.frameOctet[i] = tcamData->frameOctet[i];
		tcam.frameOctetMask[i] = tcamData->frameOctetMask[i];
	}


	return Topaz_gtcamLoadEntry(dev, tcamPointer, &tcam);
}
/*******************************************************************************
* Topaz_gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine reads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 127)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gtcamReadTCAMDataIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    OUT MSD_TCAM_DATA        *tcamData
)
{
	TOPAZ_MSD_TCAM_DATA tcam;
	MSD_STATUS retVal;
	MSD_U8 i = 0;

	retVal=Topaz_gtcamReadTCAMData(dev, tcamPointer, &tcam);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_gtcamReadTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)(tcamData), 0, sizeof(MSD_TCAM_DATA));

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;

	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;
	

	for (i = 0; i < 48; i++)
	{
		tcamData->frameOctet[i]=tcam.frameOctet[i]  ;
		tcamData->frameOctetMask[i]= tcam.frameOctetMask[i] ;
	}


	return retVal;
}
/*******************************************************************************
* Topaz_gtcamGetNextTCAMData
*
* DESCRIPTION:
*       This routine  finds the next higher TCAM Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry 
*       register (bits 6:0) is used as the TCAM entry to start from. To find 
*       the lowest number TCAM Entry that is valid, start the Get Next operation 
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 127)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 127)
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gtcamGetNextTCAMDataIntf
(
    IN     MSD_QD_DEV     *dev,
    INOUT  MSD_U32        *tcamPointer,
    OUT    MSD_TCAM_DATA  *tcamData
)
{
	TOPAZ_MSD_TCAM_DATA tcam;
	MSD_STATUS retVal;
	MSD_U8 i = 0;

	retVal=Topaz_gtcamGetNextTCAMData(dev, tcamPointer, &tcam);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_gtcamGetNextTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)(tcamData), 0, sizeof(MSD_TCAM_DATA));

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;

	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;

	for (i = 0; i < 48; i++)
	{
		tcamData->frameOctet[i] = tcam.frameOctet[i];
		tcamData->frameOctetMask[i] = tcam.frameOctetMask[i];
	}


	return retVal;
}
/*******************************************************************************
* Topaz_gtcamFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       tcamPointer - the tcam entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       tcamData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Topaz_gtcamFindEntryIntf
(
	IN	MSD_QD_DEV     *dev,
	IN	MSD_U32        tcamPointer,
	OUT	MSD_TCAM_DATA  *tcamData,
	OUT	MSD_BOOL		  *found
)
{
	TOPAZ_MSD_TCAM_DATA tcam;
	MSD_STATUS retVal;
	MSD_U8 i = 0;

    msdMemSet((void*)(&tcam), 0, sizeof(TOPAZ_MSD_TCAM_DATA));
	retVal = Topaz_gtcamFindEntry(dev, tcamPointer, &tcam, found);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_gtcamFindEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)(tcamData), 0, sizeof(MSD_TCAM_DATA));

    if (*found == MSD_FALSE)
        return retVal;

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;

	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;

	for (i = 0; i < 48; i++)
	{
		tcamData->frameOctet[i] = tcam.frameOctet[i];
		tcamData->frameOctetMask[i] = tcam.frameOctetMask[i];
	}


	return retVal;
}

/*******************************************************************************
* Topaz_gtcamSetModeIntf
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*       Note: Do not chage the value while frames are flowing into this port,
*             You must first put the port in the Disable Port State, then change
*             TCAM mode, and then re-enable the port's Port State again.
*
* INPUTS:
*       portNum  - the logical port number
*       mode     - tcam mode to be set
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
*       None
*******************************************************************************/
MSD_STATUS Topaz_gtcamSetModeIntf
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN MSD_TCAM_MODE mode
)
{
    return Topaz_gtcamSetMode(dev, portNum, (TOPAZ_MSD_TCAM_MODE)mode);
}
/*******************************************************************************
* Topaz_gtcamGetModeIntf
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*
* INPUTS:
*       portNum  - the logical port number
*
* OUTPUTS:
*       mode     - get back tcam mode
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Topaz_gtcamGetModeIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    OUT MSD_TCAM_MODE *mode
)
{
    MSD_STATUS retVal;
    TOPAZ_MSD_TCAM_MODE temp;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("Input param MSD_TCAM_MODE in Topaz_gtcamGetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gtcamGetMode(dev, portNum, &temp);

    *mode = (MSD_TCAM_MODE)temp;

    return retVal;
}

/*******************************************************************************
* Topaz_gtcamAdvConfig
*
* DESCRIPTION:
*       This routine sets tcam entry to match standard IPv4/IPv6, TCP/UDP packets
*       with the given header data and mask.
*
* INPUTS:
*       devNum  - physical devie number
*       pktType - Enumeration of TCAM mapping packet type
*       tcamPointer  - tcam entry number
*       tcamPointer2 - tcam entry2 number for cascade
*       keyMaskPtr   - mask pointer for TCAM entry key
*       keyPtr       - data pointer for TCAM entry key
*       maskPtr      - mask pointer for packet unit
*       patternPtr   - data pointer for packet unit
*       actionPtr    - action pionter
*
* OUTPUTS:
*       entry2Used - MSD_TRUE: two tcam entries been used, MSD_FALSE: only tcamPointer been used, tcamPointer2 not used.
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
MSD_STATUS Topaz_gtcamAdvConfigIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_TCAM_PKT_TYPE pktType,
    IN  MSD_U32 tcamPointer,
    IN  MSD_U32 tcamPointer2,
    IN  MSD_TCAM_ENT_KEY *keyMaskPtr,
    IN  MSD_TCAM_ENT_KEY *keyPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *maskPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *patternPtr,
    IN  MSD_TCAM_ENT_ACT *actionPtr,
    OUT MSD_BOOL *entry2Used
)
{
    TOPAZ_MSD_TCAM_ENT_KEY tmpKeyMask;
    TOPAZ_MSD_TCAM_ENT_KEY tmpKey;
    TOPAZ_MSD_TCAM_ENT_ACT tmpAction;

    if (keyMaskPtr == NULL || keyPtr == NULL || maskPtr == NULL || patternPtr == NULL || actionPtr == NULL || entry2Used == NULL)
    {
        MSD_DBG_ERROR(("Input pointer parameter is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpKeyMask.frameType = keyMaskPtr->frameType;
    tmpKeyMask.ppri = keyMaskPtr->ppri;
    tmpKeyMask.pvid = keyMaskPtr->pvid;
    tmpKeyMask.spv = keyMaskPtr->spv;

    tmpKey.frameType = keyPtr->frameType;
    tmpKey.ppri = keyPtr->ppri;
    tmpKey.pvid = keyPtr->pvid;
    tmpKey.spv = keyPtr->spv;

    tmpAction.dpvData = actionPtr->dpvData;
    tmpAction.dpvMode = actionPtr->dpvMode;
    tmpAction.dpvSF = actionPtr->dpvSF;
    tmpAction.factionData = actionPtr->factionData;
    tmpAction.factionOverride = actionPtr->factionOverride;
    tmpAction.fpriData = actionPtr->fpriData;
    tmpAction.fpriOverride = actionPtr->fpriOverride;
    tmpAction.IncTcamCtr = actionPtr->IncTcamCtr;
    tmpAction.interrupt = actionPtr->interrupt;
    tmpAction.ldBalanceData = actionPtr->ldBalanceData;
    tmpAction.ldBalanceOverride = actionPtr->ldBalanceOverride;
    tmpAction.qpriData = actionPtr->qpriData;
    tmpAction.qpriOverride = actionPtr->qpriOverride;
    tmpAction.unKnownFilter = actionPtr->unKnownFilter;
    tmpAction.vidData = actionPtr->vidData;
    tmpAction.vidOverride = actionPtr->vidOverride;

    return Topaz_gtcamAdvConfig(dev, pktType, tcamPointer, tcamPointer2, &tmpKeyMask, &tmpKey, maskPtr, patternPtr, &tmpAction, entry2Used);
}