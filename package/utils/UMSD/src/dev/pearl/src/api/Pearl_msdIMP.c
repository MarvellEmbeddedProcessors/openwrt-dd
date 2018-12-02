#include <msdCopyright.h>

/*******************************************************************************
* Pearl_msdIMP.c
*
* DESCRIPTION:
*       API definitions for Marvell IMP related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdIMP.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>


/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Pearl_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
);

static MSD_STATUS Pearl_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
);

static MSD_STATUS Pearl_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Pearl_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Pearl_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Pearl_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Pearl_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
);

static MSD_STATUS Pearl_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
);

/*******************************************************************************
* Pearl_impRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*        addr - specified start address
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impRun
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x1;

	MSD_U16 pointer_AddrLo = 0x0A;
	MSD_U16 LowAddr = addr & 0xff;

	MSD_U16 pointer_AddrHi = 0x0B;
	MSD_U16 HighAddr = (addr & 0xff00) >> 8;

	MSD_DBG_INFO(("Pearl_impRun Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Pearl_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impRun Pearl_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Pearl_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Pearl_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Pearl_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impRun Pearl_IMP_EXamine returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_IMP_Write(dev, pointer, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impRun Pearl_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impRun Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_impReset
*
* DESCRIPTION:
*       This routine is to reset imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impReset
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data;

	MSD_DBG_INFO(("Pearl_impReset Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/*Stop the CPU*/
	retVal = Pearl_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impReset Pearl_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impReset Pearl_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_IMP_Read(dev, 0x71, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impReset Pearl_IMP_Read 0x71 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	data |= 0x40;
	retVal = Pearl_IMP_Write(dev, 0x71, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impReset Pearl_IMP_Write 0x71 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impReset Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_impWriteEEPROM
*
* DESCRIPTION:
*       This routine is to write data to EEPROM
*
*
* INPUTS:
*        addr - EEPROM address
*        data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impWriteEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Pearl_impWriteEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Pearl_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impWriteEEPROM Pearl_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Pearl_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impWriteEEPROM Pearl_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impWriteEEPROM Pearl_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_EEPROM_Write(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impWriteEEPROM Pearl_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impWriteEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_impLoadToEEPROM
*
* DESCRIPTION:
*       This routine is to load data to eeprom devices
*
*
* INPUTS:
*        datasize - data size
*        data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impLoadToEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;
	MSD_U16 Idx;

	MSD_DBG_INFO(("Pearl_impLoadToEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Pearl_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToEEPROM Pearl_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Pearl_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToEEPROM Pearl_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Pearl_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToEEPROM Pearl_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	for (Idx = 0; Idx < dataSize; Idx++)
	{
		retVal = Pearl_EEPROM_Write(dev, Idx, data[Idx]);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Pearl_impLoadToEEPROM Pearl_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum,  dev->tblRegsSem);
			return MSD_FAIL;
		}
	}
	retVal = Pearl_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToEEPROM Pearl_waitBusyBitSelfClear returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impLoadToEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_impLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to memory
*
*
* INPUTS:
*        addr - start address
*        datasize - data size
*        data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Pearl_impLoadToRAM Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Pearl_IMP_Stop(dev);

	/* work around*/
	retVal |= Pearl_IMP_Write(dev, 0x2f, 0);
	retVal |= Pearl_IMP_Write(dev, 0x70, 0x08);		
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToRAM Pearl_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Pearl_IMP_Download(dev, addr, dataSize, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impLoadToRAM Pearl_IMP_Download returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impLoadToRAM Exit.\n"));

	return retVal;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Pearl_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
)
{
	MSD_STATUS retVal;

	MSD_U8 regAddr = 0x13;
	data = 0x8000 | ((0x7F & pointer) << 8) | (data & 0xFF);

	retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, regAddr, data);

	return retVal;
}

static MSD_STATUS Pearl_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U8 regAddr = 0x13;

	pointer = (pointer & 0x7f) << 8;
	retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, regAddr, pointer);
	retVal |= msdGetAnyReg(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, regAddr, data);
	*data = *data & 0xff;

	return retVal;
}

static MSD_STATUS Pearl_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x6;

	retVal = Pearl_IMP_Write(dev, pointer, data);
	return retVal;
}

static MSD_STATUS Pearl_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x3;
	return Pearl_IMP_Write(dev, pointer, data);
}

static MSD_STATUS Pearl_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x2;

	retVal = Pearl_IMP_Write(dev, pointer, data);

	pointer = 0x8;
	data = 0xf;
	Pearl_IMP_Write(dev, pointer, data);

	Pearl_IMP_Write(dev, 0x0A, 0x0);
	Pearl_IMP_Write(dev, 0x0B, 0x0);
	Pearl_IMP_EXamine(dev);

	return retVal;
}

static MSD_STATUS Pearl_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data = 1;

	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, 20, 15, 1, &data);
		if (retVal != MSD_OK)
			return MSD_FAIL;
	}
	
	return MSD_OK;
}

static MSD_STATUS Pearl_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;

	retVal = Pearl_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (3 << 12) | (1 << 10) | data;

	retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return MSD_OK;
}

static MSD_STATUS Pearl_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
)
{
	MSD_STATUS retVal;
	MSD_U32 Idx;

	MSD_U16 pointer_AddrLo = 0x0A;
	MSD_U16 LowAddr = (MSD_U16)(addr & 0xff);

	MSD_U16 pointer_AddrHi = 0x0B;
	MSD_U16 HighAddr = (MSD_U16)((addr & 0xff00) >> 8);

	MSD_U16 pointer_WrData = 0x0c;

	retVal = Pearl_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Pearl_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Pearl_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	if (datasize > 0)
	{
		for (Idx = 0; Idx < datasize; Idx++)
		{
			Pearl_IMP_Write(dev, pointer_WrData, (MSD_U16)data[Idx]);
			Pearl_IMP_De_Next(dev);
		}
	}

	return MSD_OK;
}



/*******************************************************************************
* Pearl_impStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_impStop
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Pearl_impStop Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Pearl_IMP_Stop(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_impStop Pearl_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Pearl_impStop Exit.\n"));

	return retVal;
}
