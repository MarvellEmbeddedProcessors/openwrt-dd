#include <msdCopyright.h>
/********************************************************************************
* sample_msdBrgFdb.c
*
* DESCRIPTION:
*    The sample sample_msdBrgFdb will demonstrate how to
*    1. Flush all address from the MAC address table.
*    2. Add a MAC Address into the MAC Address Data Base.
*    3. Move all address from a port to another
*    4. Find one mac address exist or not for a special fid.
*    5. Delete a MAC Address from the MAC Address Data Base.
*    6. Get Violation
*
*    The sample sample_msdBrgFdb_learnToAll will demonstrate how to
*    1. Enable port auto-learning
*    2. Enable learn2All feature
*    3. Enable one port's message port bit
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Basic ATU operation.
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdBrgFdb(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_ATU_ENTRY macEntry;
	MSD_ATU_INT_STATUS atuIntStatus;
	MSD_ATU_ENTRY tempEntry;
	MSD_BOOL found;

	MSG_PRINT(("sample_msdBrgFdb Enter.\n"));

	/*
	*    Flush all address from the MAC address table
	*/
	if ((status = msdFdbAllDelete(devNum, MSD_FLUSH_ALL)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize the ATU entry
	*    Assume the MAC address is
	*    00-50-43-00-01-02.
	*/
	macEntry.macAddr.arEther[0] = 0x00;
	macEntry.macAddr.arEther[1] = 0x50;
	macEntry.macAddr.arEther[2] = 0x43;
	macEntry.macAddr.arEther[3] = 0x00;
	macEntry.macAddr.arEther[4] = 0x01;
	macEntry.macAddr.arEther[5] = 0x02;

	macEntry.trunkMemberOrLAG = 0;  /* No trunk. */

	macEntry.portVec = 1 << 2;        /* Set the portVector matching with Port 2, the frame with DA as above address will be forwarded to port 2. */

	macEntry.fid = 0;

	macEntry.entryState = 0xE;      /* Static entry. */

	macEntry.exPrio.macFPri = 0;    /* If device doesnot support MAC Frame Priority override,
									this field is ignored. */

	macEntry.exPrio.macQPri = 0;    /* If device doesnot support MAC Queue Priority override,
									this field is ignored. */

	/*
	*    Add this MAC Address.
	*/
	if ((status = msdFdbMacEntryAdd(devNum, &macEntry)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbMacEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Move the MAC Address from original port(port 2) to another(port 3).
	*/
	if ((status = msdFdbMove(devNum, MSD_MOVE_ALL, 2, 3)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbMove returned fail.\n"));
		return status;
	}

	/*
	*    find the MAC Address in spcial fid
	*/
	msdMemSet(&tempEntry, 0, sizeof(MSD_ATU_ENTRY));
	if ((status = msdFdbMacEntryFind(devNum, &(macEntry.macAddr), macEntry.fid, &tempEntry, &found)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbMacEntryFind returned fail.\n"));
		return status;
	}
	MSG_PRINT(("ATU entry find result:%d\n", found));

	/*
	*    delete this MAC address.
	*/
	if ((status = msdFdbMacEntryDelete(devNum, &(macEntry.macAddr), macEntry.fid)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbMacEntryDelete returned fail.\n"));
		return status;
	}

	/*
	*    Get Violation.
	*/
	if ((status = msdFdbViolationGet(devNum, &atuIntStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbViolationGet returned fail.\n"));
		return status;
	}

	MSG_PRINT(("ATU_AGE_OUT_VIOLATION: %d\n", atuIntStatus.atuIntCause.ageOutVio));
	MSG_PRINT(("ATU_MEMBER_VIOLATION: %d\n", atuIntStatus.atuIntCause.memberVio));
	MSG_PRINT(("ATU_MISS_VIOLATION: %d\n", atuIntStatus.atuIntCause.missVio));
	MSG_PRINT(("ATU_FULL_VIOLATION: %d\n", atuIntStatus.atuIntCause.fullVio));

	MSG_PRINT(("sample_msdBrgFdb success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2:
*       Configure the DUT to enable learn to all feature, when normal frame entered,
*       it will auto-learn SA, and also sending message frame to the message port,
*       thus, the cascade device can also learn this SA.
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdBrgFdb_learnToAll(MSD_U8 devNum)
{
	MSD_STATUS status;

	/*
	* Enable the auto-learn for port 0
	*/
	if ((status = msdPortLearnEnableSet(devNum, 0, MSD_TRUE)) != MSD_OK)
	{
		MSG_PRINT(("msdPortLearnEnableSet returned fail.\n"));
		return status;
	}

	/*
	* Enable the learn to all feature
	*/
	if ((status = msdFdbLearnOnAllDevicesSet(devNum, MSD_TRUE)) != MSD_OK)
	{
		MSG_PRINT(("msdFdbLearnOnAllDevicesSet returned fail.\n"));
		return status;
	}

	/*
	* Enable port 1 message port feature
	*/
	if ((status = msdPortMessagePortSet(devNum, 1, MSD_TRUE)) != MSD_OK)
	{
		MSG_PRINT(("msdPortMessagePortSet returned fail.\n"));
		return status;
	}


	/********************************************************************************
	*   After this setup, when a normal frame entering the port 0, the SA will be   *
	*   learned in ATU table and message frame will be forwarded out in port 1      *
	********************************************************************************/

	MSG_PRINT(("sample_msdBrgFdb_learnToAll success.\n"));
	return MSD_OK;
}