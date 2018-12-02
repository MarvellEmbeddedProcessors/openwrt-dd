#include <msdCopyright.h>

/*******************************************************************************
* Peridot_msdIMP.c
*
* DESCRIPTION:
*       API definitions for Marvell IMP related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdIMP.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Peridot_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
);

static MSD_STATUS Peridot_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
);

static MSD_STATUS Peridot_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Peridot_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Peridot_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Peridot_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Peridot_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
);

static MSD_STATUS Peridot_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
);

/*******************************************************************************
* Peridot_impRun
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
MSD_STATUS Peridot_impRun
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

	MSD_DBG_INFO(("Peridot_impRun Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Peridot_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impRun Peridot_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Peridot_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Peridot_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Peridot_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impRun Peridot_IMP_EXamine returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_IMP_Write(dev, pointer, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impRun Peridot_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impRun Exit.\n"));

	return retVal;		
}

/*******************************************************************************
* Peridot_impReset
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
MSD_STATUS Peridot_impReset
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data;

	MSD_DBG_INFO(("Peridot_impReset Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/*Stop the CPU*/
	retVal = Peridot_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impReset Peridot_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impReset Peridot_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_IMP_Read(dev, 0x71, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impReset Peridot_IMP_Read returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	data |= 0x40;
	retVal = Peridot_IMP_Write(dev, 0x71, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impReset Peridot_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impReset Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Peridot_impWriteEEPROM
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
MSD_STATUS Peridot_impWriteEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Peridot_impWriteEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Peridot_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impWriteEEPROM Peridot_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Peridot_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impWriteEEPROM Peridot_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impWriteEEPROM Peridot_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_EEPROM_Write(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impWriteEEPROM Peridot_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impWriteEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Peridot_impLoadToEEPROM
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
MSD_STATUS Peridot_impLoadToEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;
	MSD_U16 Idx;

	MSD_DBG_INFO(("Peridot_impLoadToEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Peridot_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToEEPROM Peridot_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Peridot_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToEEPROM Peridot_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Peridot_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToEEPROM Peridot_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	for (Idx = 0; Idx < dataSize; Idx++)
	{
		retVal = Peridot_EEPROM_Write(dev, Idx, data[Idx]);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Peridot_impLoadToEEPROM Peridot_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum,  dev->tblRegsSem);
			return MSD_FAIL;
		}
	}
	retVal = Peridot_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToEEPROM Peridot_waitBusyBitSelfClear returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impLoadToEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Peridot_impLoadToRAM
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
MSD_STATUS Peridot_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Peridot_impLoadToRAM Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Peridot_IMP_Stop(dev);

	/* work around*/
	retVal |= Peridot_IMP_Write(dev, 0x2f, 0);
	retVal |= Peridot_IMP_Write(dev, 0x70, 0x08);		
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToRAM Peridot_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Peridot_IMP_Download(dev, addr, dataSize, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impLoadToRAM Peridot_IMP_Download returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impLoadToRAM Exit.\n"));

	return retVal;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Peridot_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
)
{
	MSD_STATUS retVal;

	MSD_U8 regAddr = 0x13;
	data = 0x8000 | ((0x7F & pointer) << 8) | (data & 0xFF);

	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, regAddr, data);

	return retVal;
}

static MSD_STATUS Peridot_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U8 regAddr = 0x13;

	pointer = (pointer & 0x7f) << 8;
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, regAddr, pointer);
	retVal |= msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, regAddr, data);
	*data = *data & 0xff;

	return retVal;
}

static MSD_STATUS Peridot_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x6;

	retVal = Peridot_IMP_Write(dev, pointer, data);
	return retVal;
}

static MSD_STATUS Peridot_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x3;
	return Peridot_IMP_Write(dev, pointer, data);
}

static MSD_STATUS Peridot_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x2;

	retVal = Peridot_IMP_Write(dev, pointer, data);

	pointer = 0x8;
	data = 0xf;
	Peridot_IMP_Write(dev, pointer, data);

	Peridot_IMP_Write(dev, 0x0A, 0x0);
	Peridot_IMP_Write(dev, 0x0B, 0x0);
	Peridot_IMP_EXamine(dev);

	return retVal;
}

static MSD_STATUS Peridot_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data = 1;

	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, 20, 15, 1, &data);
		if (retVal != MSD_OK)
			return MSD_FAIL;
	}
	
	return MSD_OK;
}

static MSD_STATUS Peridot_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;

	retVal = Peridot_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (3 << 12) | (1 << 10) | data;

	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return MSD_OK;
}

static MSD_STATUS Peridot_IMP_Download
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

	retVal = Peridot_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Peridot_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Peridot_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	if (datasize > 0)
	{
		for (Idx = 0; Idx < datasize; Idx++)
		{
			Peridot_IMP_Write(dev, pointer_WrData, (MSD_U16)data[Idx]);
			Peridot_IMP_De_Next(dev);
		}
	}

	return MSD_OK;
}



/*******************************************************************************
* impStop
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
MSD_STATUS Peridot_impStop
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Peridot_impStop Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Peridot_IMP_Stop(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_impStop Peridot_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Peridot_impStop Exit.\n"));

	return retVal;
}
