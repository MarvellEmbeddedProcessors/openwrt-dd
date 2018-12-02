#include <msdCopyright.h>
/********************************************************************************
* sample_msdRMU.c
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
*    Before to use these advanced RMU feature to dump ATU, MIB or TCAM, following
*    steps should be done firstly.
*    1. Enable one port's RMU mode through SMI, here take port0 for example
*       a) Configure Global register -> offset 28 -> bits [10:8] = 3b'000
*          Enable port0 as the RMU port
*       b) Configure Port0 register -> offset 4 -> bits [9:8] = 2b'11
*          Set RMU port as Ether type DSA mode
*       c) Connect RJ45 cable form host to device port0
*    2. Register the rmu_tx_rx method when do drvier init, please reference the
*       sample_qdStart_SingleDevice_RMU sample
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdRMU_Setup(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U8 devAddr;
	MSD_U8 regAddr;
	MSD_U16 data;
	MSD_FRAME_MODE mode;

	/*Enable the port0 as the RMU port*/
	devAddr = 0x1B; /*Global register's devAddr is 0x1B*/
	regAddr = 0x1C; /*Global Control 2 register*/
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	/********************************************************
	* Pearl & Agate
	* only support port 4 , 5 , 6
	* G1 - R28 - bit13:12
	********************************************************/
	if ((dev->deviceId == MSD_88E6320) || (dev->deviceId == MSD_88E6321) || (dev->deviceId == MSD_88E6352) || (dev->deviceId == MSD_88E6176))
	{
		if ((status = msdGetAnyReg(devNum, devAddr, regAddr, &data)) != MSD_OK)
		{
			MSG_PRINT(("msdGetAnyReg for Global Control 2 register returned fail.\n"));
			return status;
		}
		/*Set bits [13:12] as 2b'01*/
		data = (data & 0xcfff) | 0x1000;
		if ((status = msdSetAnyReg(devNum, devAddr, regAddr, data)) != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg for Global Control 2 register returned fail.\n"));
			return status;
		}

		/*Set RMU port4 as Ehter Type DSA mode*/
		mode = MSD_FRAME_MODE_ETHER_TYPE_DSA;
		if ((status = msdPortFrameModeSet(devNum, 4, mode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortFrameModeSet returned fail.\n"));
			return status;
		}
	}
	/********************************************************
	* Peridot & Topaz
	* Peridot support port 0 , 1 , 9 , 10
	* Topaz only support port 0 , 1 , 5
	* G1 - R28 - bit10:18
	********************************************************/

	if ((status = msdGetAnyReg(devNum, devAddr, regAddr, &data)) != MSD_OK)
	{
		MSG_PRINT(("msdGetAnyReg for Global Control 2 register returned fail.\n"));
		return status;
	}
	/*Set bits [10:8] as 3b'000*/
	data = data & 0xf8ff;
	if ((status = msdSetAnyReg(devNum, devAddr, regAddr, data)) != MSD_OK)
	{
		MSG_PRINT(("msdSetAnyReg for Global Control 2 register returned fail.\n"));
		return status;
	}

	/*Set RMU port0 as Ehter Type DSA mode*/
	mode = MSD_FRAME_MODE_ETHER_TYPE_DSA;
	if ((status = msdPortFrameModeSet(devNum, 0, mode)) != MSD_OK)
	{
		MSG_PRINT(("msdPortFrameModeSet returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdRMU_Setup success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2:
*    This sample will demonstrate how to
*    1. Get current device Id through RMU
*    2. Dump ATU entry from the specified start number through RMU
*    3. Dump MIB from the specified port through RMU
*    4. Dump valid TCAM entries from the specified entry number through RMU
*    5. Dump the port0 registers using the multiple register access
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdRMU_DumpMethods(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U16 id;

	/*Get current device id*/
	if ((status = msdRMUGetID(devNum, &id)) != MSD_OK)
	{
		MSG_PRINT(("msdRMUGetID returned fail.\n"));
		return status;
	}

	/*Dump ATU entry from the beginning of the ATU's memory*/
	{
		/*To start at the beginning of the ATU's memory use a Continue Code = 0x0000*/
		MSD_U32 ContinueCode = 0x0000;
		/*
		* The request dumps up to 48 valid MAC entries, the APi will not alloc memory, so
		* upper level application should alloc at least 48 entries memory
		*/
		MSD_U32 numofEntry = 48;
		MSD_ATU_ENTRY atuEntry[48];
		MSD_ATU_ENTRY* pAtuEntry = &(atuEntry[0]);

		msdMemSet(pAtuEntry, 0, sizeof(MSD_ATU_ENTRY)*numofEntry);
		if ((status = msdRMUAtuDump(devNum, &ContinueCode, &numofEntry, &pAtuEntry)) != MSD_OK)
		{
			MSG_PRINT(("msdRMUAtuDump returned fail.\n"));
			return status;
		}

		/**********************************************************************************************
		* The numofEntry will demonstrate the valid entries in this respones, if it is 48, that means *
		* may be more than 48 valid entries in the ATU memory, the ContinueCode will demonstrate the  *
		* seacrch point, you can invoke this method again with this ContinueCode to get other valid   *
		* entries                                                                                     *
		**********************************************************************************************/
	}

	/*Dump all the current MIB counters for the port0 and then clears them*/
	
	MSD_BOOL enClearOnRead = MSD_TRUE;
	MSD_LPORT port = 0;
	MSD_STATS_COUNTER_SET statsCounterSet;
	MSD_STATS_COUNTER_SET_LEGACY statsCounterSet_legacy;
	msdMemSet(&statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));
	/********************************************************
	* Agate
	* only support port 4 , 5 , 6
	* G1 - R28 - bit13:12
	********************************************************/
	id = id >> 4;
	if ((id == MSD_88E6352) || (id == MSD_88E6176))
	{
		if ((status = msdRMUMibDump(devNum, enClearOnRead, port, &statsCounterSet_legacy)) != MSD_OK)
		{
			MSG_PRINT(("msdRMUMibDump returned fail.\n"));
			return status;
		}
	}
	else
	{
		if ((status = msdRMUMib2Dump(devNum, enClearOnRead, port, &statsCounterSet)) != MSD_OK)
		{
			MSG_PRINT(("msdRMUMib2Dump returned fail.\n"));
			return status;
		}

	}

	/*Dump TCAM entry from the beginning the of the TCAM table*/
	{
		/*The operation format is same with GetNext, For Peridot, the searching from 0xff*/
		MSD_U32 ContinueCode = 0xFF;
		/*
		* The request dumps up to 3 valid TCAM entries, the APi will not alloc memory, so
		* upper level application should alloc at least 3 entries memory
		*/
		MSD_U32 numofEntry = 3;
		MSD_TCAM_DATA tcamEntry[3];
		MSD_TCAM_DATA* pTcamEntry = &(tcamEntry[0]);

		msdMemSet(pTcamEntry, 0, sizeof(MSD_TCAM_DATA)*numofEntry);
		if ((status = msdRMUTcamDump(devNum, &ContinueCode, &numofEntry, &pTcamEntry)) != MSD_OK)
		{
			MSG_PRINT(("msdRMUTcamDump returned fail.\n"));
			return status;
		}

		/**********************************************************************************************
		* The numofEntry will demonstrate the valid entries in this respones, if it is 3, that means  *
		* may be more than 3 valid entries in the TCAM memory, the ContinueCode will demonstrate the  *
		* search point, you can invoke this method again with this ContinueCode to get other valid   *
		* entries                                                                                     *
		**********************************************************************************************/
	}

	/*Dump the devAddr=0x10 registers using the rmu register dump function*/
	{
        MSD_RegList dataList;
        MSD_U8 devAddr = 0x10;

        msdMemSet(&dataList, 0, sizeof(MSD_RegList));

        if ((status = msdRMURegisterDump(devNum, devAddr, &dataList)) != MSD_OK)
		{
			MSG_PRINT(("msdRMURegisterDump returned fail.\n"));
			return status;
		}
	}

	MSG_PRINT(("sample_msdRMU success.\n"));
	return MSD_OK;
	
}