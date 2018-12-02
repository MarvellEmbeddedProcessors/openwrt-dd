/*******************************************************************************
*                 Copyright 2015, MARVELL TECHNOLOGY GROUP LTD                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
*
* init.c
*
* DESCRIPTION:
*       QD initialization module
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "Wrapper.h"
#include "msdApi.h"
#include "rmuPcap.h"
 
#define MSG_PRINT printf

#define SOHO_DEV_NUM	0 /* update by application*/

#ifdef USE_SMI_MULTI_ADDR
  #define SOHO_DEV_ADDR	2 /* depends on hardware configuration, non-zero if dut is in multi-chip mode*/
#else
  #define SOHO_DEV_ADDR	0
#endif

/*MSD_SYS_CONFIG   cfg;*/
MSD_QD_DEV       QdDev;
MSD_QD_DEV       *qddev = &QdDev;
MSD_U8	sohoDevNum = SOHO_DEV_NUM;
MSD_RMU_MODE gRmuMode = MSD_RMU_DSA_MODE;
MSD_U32 gEtherTypeValue = 0x9100;

static char* SWGetDeviceName(int devId)
{
	char* ret;
	switch(devId)
	{
		case MSD_88E6190:
		case MSD_88E6190X:
		case MSD_88E6390:
		case MSD_88E6390X:
		case MSD_88E6290:
			ret = "Peridot";
            break;    

        case MSD_88E6141:
        case MSD_88E6341:
            ret = "Topaz";
            break;
		case MSD_88E6352:
		case MSD_88E6172:
		case MSD_88E6176:
		case MSD_88E6240:
			ret = "Agate";
			break;
		case MSD_88E6320:
		case MSD_88E6321:
			ret = "Pearl";
			break;
		default:
			ret = "Unknown";
			break;
	}
	return ret;
}

static MSD_STATUS SMIRead(MSD_U8 devNum, MSD_U8 phyAddr,
               MSD_U8 miiReg, MSD_U16* value)
{
	int ret = 0;

	ret=readRegister(phyAddr, miiReg);
	*value = (MSD_U16)ret;
	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
                                                                                
static MSD_STATUS SMIWrite(MSD_U8 devNum, MSD_U8 phyAddr,
                      MSD_U8 miiReg, MSD_U16 value)
{
	int ret = 0;
	ret = writeRegister(phyAddr, miiReg, value);

	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}


MSD_STATUS qdStart(MSD_U8 devNum, MSD_U8 baseAddr, MSD_U32 phyAddr, MSD_U32 cpuPort, MSD_32 bus_interface)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_SYS_CONFIG   cfg;

	memset((char*)&cfg,0,sizeof(MSD_SYS_CONFIG));

	cfg.BSPFunctions.rmu_tx_rx = send_and_receive_packet;
	cfg.BSPFunctions.readMii   = SMIRead;
	cfg.BSPFunctions.writeMii  = SMIWrite;
	cfg.InterfaceChannel = (MSD_INTERFACE)bus_interface;
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake   = NULL;
	cfg.BSPFunctions.semGive   = NULL;

	cfg.devNum = devNum;
	cfg.baseAddr = baseAddr;
    cfg.rmuMode = gRmuMode;
    cfg.eTypeValue = gEtherTypeValue;

	if((status=msdLoadDriver(&cfg)) != MSD_OK)
	{
		return status;
	}

	printf("Driver Load Success\n");

	return MSD_OK;
}

void qdClose() 
{
	if (sohoDevGet(sohoDevNum))
		msdUnLoadDriver(sohoDevNum);
}

/* 
 *	Look for any SOHO Switch devices that are attached to the SMI pair.
*/
MSD_STATUS qdInit(int baseAddr, int bus_interface)
{
	MSD_U8 devNum, devAddr;
	MSD_U32		phyAddr, cpuPort;
	MSD_STATUS	status;

	cpuPort = 0;  /* Not use so far  */
	phyAddr = 0;  /* Not use so far  */

	devNum = sohoDevNum;
	/*devAddr = SOHO_DEV_ADDR;*/
    devAddr = baseAddr;
	
    status = qdStart(devNum, devAddr, phyAddr, cpuPort, bus_interface);
	if (MSD_OK != status)
	{
		return -1;
	}

	qddev = sohoDevGet(sohoDevNum);

	return MSD_OK;
}

void showDeviceInformation()
{
	printf("\r\nCurrent device information as follows:\n");
	printf("\tDevice Name      = %s\n", SWGetDeviceName((int) qddev->deviceId));
	printf("\tRevision         = %#x\n", qddev->revision);
	printf("\tDevice Enabled   = %s\n",  qddev->devEnabled? "Yes" : "No");
	printf("\tSMI Address      = %#x\n", qddev->baseRegAddr);
	printf("\tNumber of Ports  = %d\n",  qddev->numOfPorts);
	printf("Type any key to return to main menu.");
	getchar();
}
void clean_stdin(void)
{
	int c;
	do{
		c = getchar();
	}while(c != '\n' && c != EOF);
}
