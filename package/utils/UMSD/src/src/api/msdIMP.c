#include <msdCopyright.h>

/********************************************************************************
* msdIMP.c
*
* DESCRIPTION:
*       API definitions for IMP 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdIMP.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
*   msdImpRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*       devNum  - physical devie number
*       addr - specified start address
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpRun
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  addr
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impRun != NULL)
		return dev->SwitchDevObj.IMPObj.impRun(dev, addr);
	else
		return MSD_NOT_SUPPORTED;

}

/*******************************************************************************
*   msdImpReset
*
* DESCRIPTION:
*       This routine is to reset imp
*
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpReset
(
    IN  MSD_U8  devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impReset != NULL)
		return dev->SwitchDevObj.IMPObj.impReset(dev);
	else
		return MSD_NOT_SUPPORTED;

}

/*******************************************************************************
*   msdImpWriteEEPROM
*
* DESCRIPTION:
*       This routine is to write data to EEPROM
*
*
* INPUTS:
*       devNum  - physical devie number
*       addr - EEPROM address
*       data - data to be written
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpWriteEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  addr,
    IN  MSD_U8  data
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impWriteEEPROM != NULL)
		return dev->SwitchDevObj.IMPObj.impWriteEEPROM(dev, addr, data);
	else
		return MSD_NOT_SUPPORTED;

}

/*******************************************************************************
*   msdImpLoadToEEPROM
*
* DESCRIPTION:
*       This routine is to load data to EEPROM devices
*
*
* INPUTS:
*       devNum  - physical devie number
*       datasize - data size
*       data - data to be written
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpLoadToEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  dataSize,
    IN  MSD_U8*  data
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impLoadToEEPROM != NULL)
		return dev->SwitchDevObj.IMPObj.impLoadToEEPROM(dev, dataSize, data);
	else
		return MSD_NOT_SUPPORTED;

}

/*******************************************************************************
*   msdImpLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to RAM.
*
*
* INPUTS:
*       devNum  - physical devie number
*       addr - start address
*       datasize - data size
*       data - data to be written
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpLoadToRAM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  addr,
    IN  MSD_U16  dataSize,
    IN  MSD_U8*  data
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impLoadToRAM != NULL)
		return dev->SwitchDevObj.IMPObj.impLoadToRAM(dev, addr, dataSize, data);
	else
		return MSD_NOT_SUPPORTED;

}

/*******************************************************************************
* msdImpStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       None.
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
MSD_STATUS   msdImpStop
(
    IN  MSD_U8  devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.IMPObj.impStop != NULL)
		return dev->SwitchDevObj.IMPObj.impStop(dev);
	else
		return MSD_NOT_SUPPORTED;

}

