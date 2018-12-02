#include <msdCopyright.h>
/********************************************************************************
* sample_msdSysConfig.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Initialize driver for single device @ SMI single-chip mode
*    2. Initialize driver for single device @ SMI multi-chip mode
*    3. Initialize driver for single device @ RMU access mode
*    4. Initialize driver for multiple device
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/*
 *    	sample_msdStart is the main function of this sample and customer should
 *    	register the required functions.(msdRegister API)
 *		1) readMii      -  BSP specific MII read function
 *						   (provided by BSP and required if you select SMI bus to configure DUT)
 *		2) writeMii     -  BSP specific MII write function
 *						   (provided by BSP and required if you select SMI bus to configure DUT)
 *      3) msdBspRmuTxRx -  BSP specific RMU Tx/Rx function
 *                         (provided by BSP and required if you select RMU bus to configure DUT)
 *		3) semCreate    -  OS specific semaphore create function.
 *						   (provided by BSP and required by All Device API)
 *		4) semDelete    -  OS specific semaphore delete function.
 *						   (provided by BSP and required by All Device API)
 *		5) semTake      -  OS specific semaphore take function.
 *						   (provided by BSP and required by All Device API)
 *		6) semGive      -  OS specific semaphore give function.
 *						   (provided by BSP and required by All Device API)
 */
MSD_STATUS msdBspReadMii(MSD_U8 devNum, MSD_U8 devAddr, MSD_U8 MIIReg,
                        MSD_U16* value);
MSD_STATUS msdBspWriteMii(MSD_U8 devNum, MSD_U8 devAddr, MSD_U8 MIIReg,
                        MSD_U16 value);

MSD_STATUS msdBspRmuTxRx(MSD_U8 *req_packet,MSD_U32 req_pktlen,MSD_U8 **rsp_packet,MSD_U32 *rsp_pktlen);

MSD_SEM osSemCreate( MSD_SEM_BEGIN_STATE state);
MSD_STATUS osSemDelete(MSD_SEM smid);
MSD_STATUS osSemWait(  MSD_SEM smid, MSD_U32 timeOut);
MSD_STATUS osSemSignal(MSD_SEM smid);


/********************************************************************************
* Sample #1: Initialize driver for single device @ SMI single-chip mode.
********************************************************************************/
MSD_STATUS sample_qdStart_SingleDevice_SMI_SC (MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_SYS_CONFIG   cfg;
	MSD_QD_DEV       *dev;

	/* Check if device number our of range, it should be [0,31] */
	if (devNum > 31)
	{
		MSG_PRINT(("devNum %d is our of range, larger than 31. \n", devNum));
		return MSD_FAIL;
	}

    /*
     *  Register all the required functions to Driver.
    */
    msdMemSet((char*)&cfg,0,sizeof(MSD_SYS_CONFIG));

	cfg.InterfaceChannel = MSD_INTERFACE_SMI;
	cfg.devNum = devNum;		/* Device Number, 0~31 */
    cfg.baseAddr = 0;		    /* Physical Device Address. This value should be zero if the device is in single-chip mode. */

	cfg.BSPFunctions.readMii = msdBspReadMii;
    cfg.BSPFunctions.writeMii = msdBspWriteMii;
	
	/*
	* If this method been registered, the advance RMU feature (msdRMUAtuDump, msdRMUMib2Dump ...) can be used in this interface 
	* channel, the RMU setup should be done firstly, please reference the sample_msdRMU.c
	*/
	cfg.BSPFunctions.rmu_tx_rx = NULL;

#ifdef USE_SEMAPHORE
    cfg.BSPFunctions.semCreate = osSemCreate;
    cfg.BSPFunctions.semDelete = osSemDelete;
    cfg.BSPFunctions.semTake   = osSemWait;
    cfg.BSPFunctions.semGive   = osSemSignal;
#else
    cfg.BSPFunctions.semCreate = NULL;
    cfg.BSPFunctions.semDelete = NULL;
    cfg.BSPFunctions.semTake   = NULL;
    cfg.BSPFunctions.semGive   = NULL;
#endif

    if((status=msdLoadDriver(&cfg)) != MSD_OK)
    {
        MSG_PRINT(("msdLoadDriver return Failed\n"));
        return status;
    }

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. Driver load failed. \n", devNum));
		return MSD_FAIL;
	}

    MSG_PRINT(("Device ID     : 0x%x\n",dev->deviceId));
    MSG_PRINT(("Base Reg Addr : 0x%x\n",dev->baseRegAddr));
    MSG_PRINT(("No of Ports   : %d\n",dev->numOfPorts));

    MSG_PRINT(("Driver has been started.\n"));
	MSG_PRINT(("sample_qdStart_SingleDevice_SMI_SC sample code success.\n"));

    return MSD_OK;
}


/********************************************************************************
* Sample #2: Initialize driver for single device @ SMI Multi-chip mode.
********************************************************************************/
MSD_STATUS sample_qdStart_SingleDevice_SMI_MC(MSD_U8 devNum, MSD_U8 devAddr)
{
	MSD_STATUS status;
	MSD_SYS_CONFIG   cfg;
	MSD_QD_DEV       *dev;

	/* Check if device number our of range, it should be [0,31] */
	if (devNum > 31)
	{
		MSG_PRINT(("devNum %d is our of range, larger than 31. \n", devNum));
		return MSD_FAIL;
	}

	/* Check if devAddr is non-zero */
	if (0 == devAddr)
	{
		MSG_PRINT(("devAddr is zero, only support single-chip mode. \n"));
		return MSD_FAIL;
	}

	/*
	*  Register all the required functions to Driver.
	*/
	msdMemSet((char*)&cfg, 0, sizeof(MSD_SYS_CONFIG));

	cfg.InterfaceChannel = MSD_INTERFACE_SMI_MULTICHIP;
	cfg.devNum = devNum;		/* Device Number, 0~31 */
	cfg.baseAddr = devAddr;		    /* Physical Device Address. This value should be non-zero if the device is in multi-chip mode. */

	cfg.BSPFunctions.readMii = msdBspReadMii;
	cfg.BSPFunctions.writeMii = msdBspWriteMii;

	/*
	* If this method been registered, the advance RMU feature (msdRMUAtuDump, msdRMUMib2Dump ...) can be used in this interface
	* channel, the RMU setup should be done firstly, please reference the sample_msdRMU.c
	*/
	cfg.BSPFunctions.rmu_tx_rx = NULL;

#ifdef USE_SEMAPHORE
	cfg.BSPFunctions.semCreate = osSemCreate;
	cfg.BSPFunctions.semDelete = osSemDelete;
	cfg.BSPFunctions.semTake = osSemWait;
	cfg.BSPFunctions.semGive = osSemSignal;
#else
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake = NULL;
	cfg.BSPFunctions.semGive = NULL;
#endif

	if ((status = msdLoadDriver(&cfg)) != MSD_OK)
	{
		MSG_PRINT(("msdLoadDriver return Failed\n"));
		return status;
	}

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. Driver load failed. \n", devNum));
		return MSD_FAIL;
	}

	MSG_PRINT(("Device ID     : 0x%x\n", dev->deviceId));
	MSG_PRINT(("Base Reg Addr : 0x%x\n", dev->baseRegAddr));
	MSG_PRINT(("No of Ports   : %d\n", dev->numOfPorts));

	MSG_PRINT(("Driver has been started.\n"));
	MSG_PRINT(("sample_qdStart_SingleDevice_SMI_MC sample code success.\n"));

	return MSD_OK;
}

/********************************************************************************
* Sample #3: Initialize driver for single device @ RMU bus.
********************************************************************************/
MSD_STATUS sample_qdStart_SingleDevice_RMU(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_SYS_CONFIG   cfg;
	MSD_QD_DEV       *dev;

	/* Check if device number our of range, it should be [0,31] */
	if (devNum > 31)
	{
		MSG_PRINT(("devNum %d is our of range, larger than 31. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*  Register all the required functions to Driver.
	*/
	msdMemSet((char*)&cfg, 0, sizeof(MSD_SYS_CONFIG));

	cfg.InterfaceChannel = MSD_INTERFACE_RMU;
	cfg.devNum = devNum;		                /* Device Number, 0~31 */
    cfg.rmuMode = MSD_RMU_ETHERT_TYPE_DSA_MODE; /* RMU mode*/
    cfg.eTypeValue = 0x9100;                    /* Ether Type value*/

	cfg.BSPFunctions.readMii = NULL;
	cfg.BSPFunctions.writeMii = NULL;

	cfg.BSPFunctions.rmu_tx_rx = msdBspRmuTxRx;

#ifdef USE_SEMAPHORE
	cfg.BSPFunctions.semCreate = osSemCreate;
	cfg.BSPFunctions.semDelete = osSemDelete;
	cfg.BSPFunctions.semTake = osSemWait;
	cfg.BSPFunctions.semGive = osSemSignal;
#else
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake = NULL;
	cfg.BSPFunctions.semGive = NULL;
#endif

	if ((status = msdLoadDriver(&cfg)) != MSD_OK)
	{
		MSG_PRINT(("msdLoadDriver return Failed\n"));
		return status;
	}

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. Driver load failed. \n", devNum));
		return MSD_FAIL;
	}

	MSG_PRINT(("Device ID     : 0x%x\n", dev->deviceId));
	MSG_PRINT(("Base Reg Addr : 0x%x\n", dev->baseRegAddr));
	MSG_PRINT(("No of Ports   : %d\n", dev->numOfPorts));

	MSG_PRINT(("Driver has been started.\n"));
	MSG_PRINT(("sample_qdStart_SingleDevice_RMU sample code success.\n"));

	return MSD_OK;
}

/********************************************************************************
* Sample #4: Initialize driver for multi device @ SMI Multi-chip mode.
********************************************************************************/
MSD_STATUS sample_qdStart_MultiDevice_SMI_MC(MSD_U8 numOfDevice)
{
	MSD_STATUS status;
	MSD_SYS_CONFIG   cfg;
	MSD_QD_DEV       *dev;
	MSD_U8	i;

	/* Maximum support 32 devices */
	if (numOfDevice > 32)
	{
		MSG_PRINT(("numOfDevice %d is our of range, larger than 32. \n", numOfDevice));
		return MSD_FAIL;
	}

	/*
	*  Register all the required functions to Driver. 
	*  Here supporse all device share the same hardware access bus: SMI Multi-chip access mode!
	*/
	for (i = 0; i < numOfDevice; i++)
	{
		MSG_PRINT(("\n------- Init Device Number %u -------\n", i));

		msdMemSet((char*)&cfg, 0, sizeof(MSD_SYS_CONFIG));

		cfg.InterfaceChannel = MSD_INTERFACE_SMI_MULTICHIP;
		cfg.devNum = i;		           /* Device Number, 0~31 */
		cfg.baseAddr = (i + 1);		   /* Physical Device Address. This value should be non-zero if the device is in multi-chip mode. */

		cfg.BSPFunctions.readMii = msdBspReadMii;
		cfg.BSPFunctions.writeMii = msdBspWriteMii;

		/*
		* If this method been registered, the advance RMU feature (msdRMUAtuDump, msdRMUMib2Dump ...) can be used in this interface
		* channel, the RMU setup should be done firstly, please reference the sample_msdRMU.c
		*/
		cfg.BSPFunctions.rmu_tx_rx = NULL;

#ifdef USE_SEMAPHORE
		cfg.BSPFunctions.semCreate = osSemCreate;
		cfg.BSPFunctions.semDelete = osSemDelete;
		cfg.BSPFunctions.semTake = osSemWait;
		cfg.BSPFunctions.semGive = osSemSignal;
#else
		cfg.BSPFunctions.semCreate = NULL;
		cfg.BSPFunctions.semDelete = NULL;
		cfg.BSPFunctions.semTake = NULL;
		cfg.BSPFunctions.semGive = NULL;
#endif

		if ((status = msdLoadDriver(&cfg)) != MSD_OK)
		{
			MSG_PRINT(("msdLoadDriver return Failed\n"));
			return status;
		}

		dev = sohoDevGet(i);
		if (NULL == dev)
		{
			MSG_PRINT(("devNum %d is NULL. Driver load failed. \n", i));
			return MSD_FAIL;
		}

		MSG_PRINT(("Device ID     : 0x%x\n", dev->deviceId));
		MSG_PRINT(("Base Reg Addr : 0x%x\n", dev->baseRegAddr));
		MSG_PRINT(("No of Ports   : %d\n", dev->numOfPorts));
		MSG_PRINT(("DevNum %u driver has been started.\n", i));
	}

	MSG_PRINT(("sample_qdStart_MultiDevice_SMI_MC sample code success.\n"));

	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_STATUS msdBspReadMii(MSD_U8 devNum, MSD_U8 devAddr, MSD_U8 MIIReg,
                        MSD_U16* value)
{
	/* TODO  */
	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_STATUS msdBspWriteMii(MSD_U8 devNum, MSD_U8 devAddr, MSD_U8 MIIReg,
                        MSD_U16 value)
{
	/* TODO  */
	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*
* Description: 
*     This API is used to send RMU request and receive related RMU response frame.
*     Here is the detail requirement:
*     1) Update req_packet byte array, then send out packet
*        Override SA in req_packet with current CPU's source MAC address
*     2) Filter incoming ethernet packets and find our the right RMU response frame.       
*        req_packet: SA = CPU Mac, From_DSA_Tag: Trg_Dev+SeqNum
*        rsp_packet: DA = CPU Mac, To_DSA_Tag: Src_Dev+SeqNum
*        Check:
*          a) rsp_packet DA == CPU Mac
*          b) rsp_packet Src_Dev == req_packet Trg_Dev
*          c) rsp_packet SeqNum == req_packet SeqNum
*          d) rsp_packet RMU Format/Code = req_packet RMU Format/Code
*        If not match, return failed with empty resp packets length!
*
*/
MSD_STATUS msdBspRmuTxRx
(
	MSD_U8 *req_packet,
	MSD_U32 req_pktlen,
	MSD_U8 **rsp_packet,
	MSD_U32 *rsp_pktlen
)
{
	/* TODO  */
	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_SEM osSemCreate( MSD_SEM_BEGIN_STATE state)
{
	/* TODO  */
	return 1;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_STATUS osSemDelete(MSD_SEM smid)
{
	/* TODO  */
	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_STATUS osSemWait(  MSD_SEM smid, MSD_U32 timeOut)
{
	/* TODO  */
	return MSD_OK;
}

/*****************************************************************************
*
* TODO by customers
*/
MSD_STATUS osSemSignal(MSD_SEM smid)
{
	/* TODO  */
	return MSD_OK;
}
