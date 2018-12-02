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

#include <topaz/include/api/Topaz_msdRMU.h>


/*******************************************************************************
* msdRmuGetID
*
* DESCRIPTION:
*       Get the current Device ID.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       id - DUT Id.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuGetIDIntf
(
IN  MSD_QD_DEV *dev,
OUT MSD_U16 *id
)
{
	return Topaz_msdRmuGetID(dev, id);
}

/*******************************************************************************
* msdRmuAtuDump
*
* DESCRIPTION:
*       Dump ATU entry from the specified starting address.
*
* INPUTS:
*       startAddr - starting address to search the valid ATU entryies
*
* OUTPUTS:
*       atuEntry - array of atuEntries.
*		numOfEntry - number of returned valued entries
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuAtuDumpIntf
(
	IN MSD_QD_DEV *dev,
	INOUT MSD_U32 *startAddr,
	OUT MSD_U32 *numOfEntry,
	OUT MSD_ATU_ENTRY **atuEntry
)
{
	TOPAZ_MSD_ATU_ENTRY entry[MSD_RMU_MAX_ATUS];
	TOPAZ_MSD_ATU_ENTRY *entryPtr = &(entry[0]);
	MSD_STATUS retVal;
	int i;

	for (i = 0; i < MSD_RMU_MAX_ATUS; i++)
	{
		msdMemSet((void*)(entry+i), 0, sizeof(TOPAZ_MSD_ATU_ENTRY));
	}

	retVal = Topaz_msdRmuAtuDump(dev, startAddr, numOfEntry, &entryPtr);
	if (MSD_OK != retVal)
		return retVal;

	if ((*numOfEntry > 0) && (NULL == atuEntry))
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY** in Topaz_msdRmuAtuDumpIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	for (i = 0; i < (int)(*numOfEntry); i++)
	{
		if (NULL == ((*atuEntry)+i))
		{
			MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY* for index %d in Topaz_msdRmuAtuDumpIntf is NULL. \n", i));
			return MSD_BAD_PARAM;
		}
		msdMemSet((void*)(*atuEntry + i), 0, sizeof(MSD_ATU_ENTRY));

		((*atuEntry) + i)->fid = ((entryPtr) + i)->DBNum;
		((*atuEntry) + i)->portVec = ((entryPtr) + i)->portVec;
		((*atuEntry) + i)->entryState = ((entryPtr) + i)->entryState;
		((*atuEntry) + i)->exPrio.macFPri = ((entryPtr) + i)->exPrio.macFPri;
		((*atuEntry) + i)->trunkMemberOrLAG = ((entryPtr) + i)->trunkMember;
		msdMemCpy(((*atuEntry) + i)->macAddr.arEther, ((entryPtr) + i)->macAddr.arEther, 6);
	}

	return retVal;
}

/*******************************************************************************
* msdRmuMib2Dump
*
* DESCRIPTION:
*       Dump MIB from the specified port.
*
* INPUTS:
*       enClearOnRead - indicate dump mib clear or not
*		port - request port
*
* OUTPUTS:
*       statsCounterSet - mib info.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuMib2DumpIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_BOOL enClearOnRead,
	IN MSD_LPORT port,
	OUT MSD_STATS_COUNTER_SET *statsCounterSet
)
{
	TOPAZ_MSD_STATS_COUNTER_SET  statsCounter;
	MSD_STATUS    retVal;

	retVal = Topaz_msdRmuMib2Dump(dev, enClearOnRead, port, &statsCounter);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("Input param MSD_STATS_COUNTER_SET in Topaz_msdRmuMib2DumpIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));

	statsCounterSet->InGoodOctetsLo = statsCounter.InGoodOctetsLo;
	statsCounterSet->InGoodOctetsHi = statsCounter.InGoodOctetsHi;
	statsCounterSet->InBadOctets = statsCounter.InBadOctets;
	statsCounterSet->OutFCSErr = statsCounter.OutFCSErr;
	statsCounterSet->InUnicasts = statsCounter.InUnicasts;
	statsCounterSet->Deferred = statsCounter.Deferred;
	statsCounterSet->InBroadcasts = statsCounter.InBroadcasts;
	statsCounterSet->InMulticasts = statsCounter.InMulticasts;

	statsCounterSet->Octets64 = statsCounter.Octets64;
	statsCounterSet->Octets127 = statsCounter.Octets127;
	statsCounterSet->Octets255 = statsCounter.Octets255;
	statsCounterSet->Octets511 = statsCounter.Octets511;
	statsCounterSet->Octets1023 = statsCounter.Octets1023;
	statsCounterSet->OctetsMax = statsCounter.OctetsMax;
	statsCounterSet->OutOctetsLo = statsCounter.OutOctetsLo;
	statsCounterSet->OutOctetsHi = statsCounter.OutOctetsHi;
	statsCounterSet->OutUnicasts = statsCounter.OutUnicasts;
	statsCounterSet->Excessive = statsCounter.Excessive;
	statsCounterSet->OutMulticasts = statsCounter.OutMulticasts;
	statsCounterSet->OutBroadcasts = statsCounter.OutBroadcasts;
	statsCounterSet->Single = statsCounter.Single;

	statsCounterSet->OutPause = statsCounter.OutPause;
	statsCounterSet->InPause = statsCounter.InPause;
	statsCounterSet->Multiple = statsCounter.Multiple;
	statsCounterSet->InUndersize = statsCounter.InUndersize;
	statsCounterSet->InFragments = statsCounter.InFragments;
	statsCounterSet->InOversize = statsCounter.InOversize;
	statsCounterSet->InJabber = statsCounter.InJabber;
	statsCounterSet->InRxErr = statsCounter.InRxErr;
	statsCounterSet->InFCSErr = statsCounter.InFCSErr;
	statsCounterSet->Collisions = statsCounter.Collisions;
	statsCounterSet->Late = statsCounter.Late;

	statsCounterSet->InDiscards = statsCounter.InDiscards;
	statsCounterSet->InFiltered = statsCounter.InFiltered;
	statsCounterSet->InAccepted = statsCounter.InAccepted;
	statsCounterSet->InBadAccepted = statsCounter.InBadAccepted;
	statsCounterSet->InGoodAvbClassA = statsCounter.InGoodAvbClassA;
	statsCounterSet->InGoodAvbClassB = statsCounter.InGoodAvbClassB;
	statsCounterSet->InBadAvbClassA = statsCounter.InBadAvbClassA;
	statsCounterSet->InBadAvbClassB = statsCounter.InBadAvbClassB;
	statsCounterSet->TCAMCounter0 = statsCounter.TCAMCounter0;
	statsCounterSet->TCAMCounter1 = statsCounter.TCAMCounter1;
	statsCounterSet->TCAMCounter2 = statsCounter.TCAMCounter2;
	statsCounterSet->TCAMCounter3 = statsCounter.TCAMCounter3;
	statsCounterSet->InDroppedAvbA = statsCounter.InDroppedAvbA;
	statsCounterSet->InDroppedAvbB = statsCounter.InDroppedAvbB;
	statsCounterSet->InDaUnknown = statsCounter.InDaUnknown;
	statsCounterSet->InMGMT = statsCounter.InMGMT;
	statsCounterSet->OutQueue0 = statsCounter.OutQueue0;
	statsCounterSet->OutQueue1 = statsCounter.OutQueue1;
	statsCounterSet->OutQueue2 = statsCounter.OutQueue2;
	statsCounterSet->OutQueue3 = statsCounter.OutQueue3;
	statsCounterSet->OutQueue4 = statsCounter.OutQueue4;
	statsCounterSet->OutQueue5 = statsCounter.OutQueue5;
	statsCounterSet->OutQueue6 = statsCounter.OutQueue6;
	statsCounterSet->OutQueue7 = statsCounter.OutQueue7;
	statsCounterSet->OutCutThrough = statsCounter.OutCutThrough;
	statsCounterSet->reserved_19 = statsCounter.reserved_19;
	statsCounterSet->OutOctetsA = statsCounter.OutOctetsA;
	statsCounterSet->OutOctetsB = statsCounter.OutOctetsB;

	statsCounterSet->OutDroppedYel = statsCounter.OutDroppedYel;
	statsCounterSet->OutDiscards = statsCounter.OutDiscards;
	statsCounterSet->OutMGMT = statsCounter.OutMGMT;


	return retVal;
}


/*******************************************************************************
* msdRmuReadRegister
*
* DESCRIPTION:
*       single register read.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*
* OUTPUTS:
*       data - return register value.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuReadRegisterIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	OUT MSD_U16 *data
)
{
	return Topaz_msdRmuReadRegister(dev,devAddr,regAddr,data);
}

/*******************************************************************************
* msdRmuWriteRegister
*
* DESCRIPTION:
*       single register write.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*		data - register value.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuWriteRegisterIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	IN MSD_U16 data
)
{
	return Topaz_msdRmuWriteRegister(dev, devAddr, regAddr, data);
}

/*******************************************************************************
* msdRmuMultiRegAccess
*
* DESCRIPTION:
*       multiple register read/write.
*
* INPUTS:
*       RegCmd - array of Read/Write requests
*		nCmd - number of register access commands
*
* OUTPUTS:
*       RegCmd - update read-back register value if it has register read request
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuMultiRegAccessIntf
(
IN MSD_QD_DEV *dev,
INOUT MSD_RegCmd **regCmdList,
IN MSD_U32 nCmd
)
{
	return Topaz_msdRmuMultiRegAccess(dev, regCmdList, nCmd);
}

/*******************************************************************************
* msdRmuTCAMDump
*
* DESCRIPTION:
*       Dump valid TCAM entries.
*
* INPUTS:
*       startAddr - starting address to search the valid tcam entryies
*
* OUTPUTS:
*       tcamEntry - array of tcam Entries.
*		numOfEntry - number of returned valued entries
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuTCAMDumpIntf
(
	IN MSD_QD_DEV *dev,
	INOUT MSD_U32 *startAddr,
	INOUT MSD_U32 *numOfEntry,
	OUT MSD_TCAM_DATA **tcamEntry
)
{
	MSD_STATUS           retVal;
	TOPAZ_MSD_TCAM_DATA tcam[MSD_RMU_MAX_TCAMS];
	TOPAZ_MSD_TCAM_DATA *tcamPtr = &(tcam[0]);

	MSD_U8 i = 0, j=0;

	for (i = 0; i < MSD_RMU_MAX_TCAMS; i++)
		msdMemSet((void*)(tcam + i), 0, sizeof(TOPAZ_MSD_TCAM_DATA));


	retVal = Topaz_msdRmuTCAMDump(dev, startAddr, numOfEntry, &tcamPtr);
	if (MSD_OK != retVal)
		return retVal;

	if ((*numOfEntry > 0) && (NULL == tcamEntry))
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA** in Topaz_msdRmuTCAMDumpIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	for (i = 0; i < (int)(*numOfEntry); i++)
	{
		if (NULL == ((*tcamEntry) + i))
		{
			MSD_DBG_ERROR(("Input param MSD_TCAM_DATA* for index %d in Topaz_msdRmuTCAMDumpIntf is NULL. \n", i));
			return MSD_BAD_PARAM;
		}
		msdMemSet((void*)((*tcamEntry) + i), 0, sizeof(MSD_TCAM_DATA));

		((*tcamEntry) + i)->frameType = ((tcamPtr) + i)->frameType;
		((*tcamEntry) + i)->frameTypeMask = ((tcamPtr) + i)->frameTypeMask;
		((*tcamEntry) + i)->spv = ((tcamPtr) + i)->spv;
		((*tcamEntry) + i)->spvMask = ((tcamPtr) + i)->spvMask;
		((*tcamEntry) + i)->ppri = ((tcamPtr) + i)->ppri;
		((*tcamEntry) + i)->ppriMask = ((tcamPtr) + i)->ppriMask;
		((*tcamEntry) + i)->pvid = ((tcamPtr) + i)->pvid;
		((*tcamEntry) + i)->pvidMask = ((tcamPtr) + i)->pvidMask;
		((*tcamEntry) + i)->continu = ((tcamPtr) + i)->continu;
		((*tcamEntry) + i)->interrupt = ((tcamPtr) + i)->interrupt;
		((*tcamEntry) + i)->IncTcamCtr = ((tcamPtr) + i)->IncTcamCtr;
		((*tcamEntry) + i)->vidOverride = ((tcamPtr) + i)->vidOverride;
		((*tcamEntry) + i)->vidData = ((tcamPtr) + i)->vidData;
		((*tcamEntry) + i)->nextId = ((tcamPtr) + i)->nextId;
		((*tcamEntry) + i)->qpriOverride = ((tcamPtr) + i)->qpriOverride;
		((*tcamEntry) + i)->qpriData = ((tcamPtr) + i)->qpriData;
		((*tcamEntry) + i)->fpriOverride = ((tcamPtr) + i)->fpriOverride;
		((*tcamEntry) + i)->fpriData = ((tcamPtr) + i)->fpriData;
		((*tcamEntry) + i)->dpvSF = ((tcamPtr) + i)->dpvSF;
		((*tcamEntry) + i)->dpvData = ((tcamPtr) + i)->dpvData;
		((*tcamEntry) + i)->dpvMode = ((tcamPtr) + i)->dpvMode;

		((*tcamEntry) + i)->unKnownFilter = ((tcamPtr) + i)->unKnownFilter;

		((*tcamEntry) + i)->ldBalanceOverride = ((tcamPtr) + i)->ldBalanceOverride;
		((*tcamEntry) + i)->ldBalanceData = ((tcamPtr) + i)->ldBalanceData;

		((*tcamEntry) + i)->factionOverride = ((tcamPtr) + i)->factionOverride;
		((*tcamEntry) + i)->factionData = ((tcamPtr) + i)->factionData;
		for (j = 0; j < 48; j++)
		{
			((*tcamEntry) + i)->frameOctet[j] = ((tcamPtr) + i)->frameOctet[j];
			((*tcamEntry) + i)->frameOctetMask[j] = ((tcamPtr) + i)->frameOctetMask[j];
		}
	}

	return retVal;
}

/*******************************************************************************
* msdRmuTCAMWrite
*
* DESCRIPTION:
*       single TCAM entry load.
*
* INPUTS:
*       tcamEntry - entry
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuTCAMWriteIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U32 tcamPointer,
	IN MSD_TCAM_DATA *tcamData
)
{
	TOPAZ_MSD_TCAM_DATA tcam;
	MSD_U8 i = 0;
	
	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_msdRmuTCAMWriteIntf is NULL. \n"));
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


	return Topaz_msdRmuTCAMWrite(dev, tcamPointer, &tcam);
}

/*******************************************************************************
* msdRmuTCAMRead
*
* DESCRIPTION:
*       single TCAM entry read-back.
*
* INPUTS:
*       tcamPointer - tcam pointer
*
* OUTPUTS:
*       tcamEntry - entry
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_msdRmuTCAMReadIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U32 tcamPointer,
	OUT MSD_TCAM_DATA *tcamEntry
)
{
	MSD_STATUS           retVal;
	TOPAZ_MSD_TCAM_DATA tcam;

	MSD_U8 j = 0;

	msdMemSet((void*)(&tcam), 0, sizeof(TOPAZ_MSD_TCAM_DATA));

	retVal = Topaz_msdRmuTCAMRead(dev, tcamPointer, &tcam);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == tcamEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Topaz_msdRmuTCAMReadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)tcamEntry, 0, sizeof(MSD_TCAM_DATA));

	tcamEntry->frameType = tcam.frameType;
	tcamEntry->frameTypeMask = tcam.frameTypeMask;
	tcamEntry->spv = tcam.spv;
	tcamEntry->spvMask = tcam.spvMask;
	tcamEntry->ppri = tcam.ppri;
	tcamEntry->ppriMask = tcam.ppriMask;
	tcamEntry->pvid = tcam.pvid;
	tcamEntry->pvidMask = tcam.pvidMask;
	tcamEntry->continu = tcam.continu;
	tcamEntry->interrupt = tcam.interrupt;
	tcamEntry->IncTcamCtr = tcam.IncTcamCtr;
	tcamEntry->vidOverride = tcam.vidOverride;
	tcamEntry->vidData = tcam.vidData;
	tcamEntry->nextId = tcam.nextId;
	tcamEntry->qpriOverride = tcam.qpriOverride;
	tcamEntry->qpriData = tcam.qpriData;
	tcamEntry->fpriOverride = tcam.fpriOverride;
	tcamEntry->fpriData = tcam.fpriData;
	tcamEntry->dpvSF = tcam.dpvSF;
	tcamEntry->dpvData = tcam.dpvData;
	tcamEntry->dpvMode = tcam.dpvMode;

	tcamEntry->unKnownFilter = tcam.unKnownFilter;

	tcamEntry->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamEntry->ldBalanceData = tcam.ldBalanceData;

	tcamEntry->factionOverride = tcam.factionOverride;
	tcamEntry->factionData = tcam.factionData;
	for (j = 0; j < 48; j++)
	{
		tcamEntry->frameOctet[j] = tcam.frameOctet[j];
		tcamEntry->frameOctetMask[j] = tcam.frameOctetMask[j];
	}

	return retVal;
}