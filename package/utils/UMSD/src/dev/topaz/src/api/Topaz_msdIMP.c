#include <msdCopyright.h>

/*******************************************************************************
* Topaz_msdIMP.c
*
* DESCRIPTION:
*       API definitions for Marvell IMP related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdIMP.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>


/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Topaz_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
);

static MSD_STATUS Topaz_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
);

static MSD_STATUS Topaz_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Topaz_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Topaz_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Topaz_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Topaz_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
);

static MSD_STATUS Topaz_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
);

/*******************************************************************************
* Topaz_impRun
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
MSD_STATUS Topaz_impRun
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

	MSD_DBG_INFO(("Topaz_impRun Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impRun Topaz_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Topaz_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Topaz_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Topaz_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impRun Topaz_IMP_EXamine returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_IMP_Write(dev, pointer, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impRun Topaz_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impRun Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Topaz_impReset
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
MSD_STATUS Topaz_impReset
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data;

	MSD_DBG_INFO(("Topaz_impReset Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/*Stop the CPU*/
	retVal = Topaz_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impReset Topaz_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impReset Topaz_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_IMP_Read(dev, 0x71, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impReset Topaz_IMP_Read 0x71 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	data |= 0x40;
	retVal = Topaz_IMP_Write(dev, 0x71, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impReset Topaz_IMP_Write 0x71 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impReset Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Topaz_impWriteEEPROM
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
MSD_STATUS Topaz_impWriteEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Topaz_impWriteEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impWriteEEPROM Topaz_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Topaz_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impWriteEEPROM Topaz_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impWriteEEPROM Topaz_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_EEPROM_Write(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impWriteEEPROM Topaz_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impWriteEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Topaz_impLoadToEEPROM
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
MSD_STATUS Topaz_impLoadToEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;
	MSD_U16 Idx;

	MSD_DBG_INFO(("Topaz_impLoadToEEPROM Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToEEPROM Topaz_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Topaz_IMP_Write(dev, 0x2f, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToEEPROM Topaz_IMP_Write 0x2f returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Topaz_IMP_Write(dev, 0x70, 0x08);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToEEPROM Topaz_IMP_Write 0x70 returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	for (Idx = 0; Idx < dataSize; Idx++)
	{
		retVal = Topaz_EEPROM_Write(dev, Idx, data[Idx]);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Topaz_impLoadToEEPROM Topaz_EEPROM_Write returned: %d.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum,  dev->tblRegsSem);
			return MSD_FAIL;
		}
	}
	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToEEPROM Topaz_waitBusyBitSelfClear returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impLoadToEEPROM Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Topaz_impLoadToRAM
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
MSD_STATUS Topaz_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Topaz_impLoadToRAM Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_IMP_Stop(dev);

	/* work around*/
	retVal |= Topaz_IMP_Write(dev, 0x2f, 0);
	retVal |= Topaz_IMP_Write(dev, 0x70, 0x08);		
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToRAM Topaz_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Topaz_IMP_Download(dev, addr, dataSize, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impLoadToRAM Topaz_IMP_Download returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impLoadToRAM Exit.\n"));

	return retVal;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Topaz_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
)
{
	MSD_STATUS retVal;

	MSD_U8 regAddr = 0x13;
	data = 0x8000 | ((0x7F & pointer) << 8) | (data & 0xFF);

	retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, regAddr, data);

	return retVal;
}

static MSD_STATUS Topaz_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U8 regAddr = 0x13;

	pointer = (pointer & 0x7f) << 8;
	retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, regAddr, pointer);
	retVal |= msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, regAddr, data);
	*data = *data & 0xff;

	return retVal;
}

static MSD_STATUS Topaz_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x6;

	retVal = Topaz_IMP_Write(dev, pointer, data);
	return retVal;
}

static MSD_STATUS Topaz_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x3;
	return Topaz_IMP_Write(dev, pointer, data);
}

static MSD_STATUS Topaz_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x2;

	retVal = Topaz_IMP_Write(dev, pointer, data);

	pointer = 0x8;
	data = 0xf;
	Topaz_IMP_Write(dev, pointer, data);

	Topaz_IMP_Write(dev, 0x0A, 0x0);
	Topaz_IMP_Write(dev, 0x0B, 0x0);
	Topaz_IMP_EXamine(dev);

	return retVal;
}

static MSD_STATUS Topaz_waitBusyBitSelfClear
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data = 1;

	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, 20, 15, 1, &data);
		if (retVal != MSD_OK)
			return MSD_FAIL;
	}
	
	return MSD_OK;
}

static MSD_STATUS Topaz_EEPROM_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;

	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (3 << 12) | (1 << 10) | data;

	retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return MSD_OK;
}

static MSD_STATUS Topaz_IMP_Download
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

	retVal = Topaz_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Topaz_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Topaz_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	if (datasize > 0)
	{
		for (Idx = 0; Idx < datasize; Idx++)
		{
			Topaz_IMP_Write(dev, pointer_WrData, (MSD_U16)data[Idx]);
			Topaz_IMP_De_Next(dev);
		}
	}

	return MSD_OK;
}



/*******************************************************************************
* Topaz_impStop
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
MSD_STATUS Topaz_impStop
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Topaz_impStop Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_IMP_Stop(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_impStop Topaz_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_impStop Exit.\n"));

	return retVal;
}
