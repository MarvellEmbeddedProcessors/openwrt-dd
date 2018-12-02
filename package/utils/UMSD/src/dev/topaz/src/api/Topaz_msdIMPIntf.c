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
MSD_STATUS Topaz_impRunIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr
)
{
	return Topaz_impRun(dev, addr);
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
MSD_STATUS Topaz_impResetIntf
(
IN	MSD_QD_DEV	*dev
)
{
	return Topaz_impReset(dev);
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
MSD_STATUS Topaz_impWriteEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U8	data
)
{
	return Topaz_impWriteEEPROM(dev, addr, data);
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
MSD_STATUS Topaz_impLoadToEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	dataSize,
IN	MSD_U8*	data
)
{
	return Topaz_impLoadToEEPROM(dev, dataSize, data);
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
MSD_STATUS Topaz_impLoadToRAMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	dataSize,
IN	MSD_U8*	data
)
{
	return Topaz_impLoadToRAM(dev, addr, dataSize, data);
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
MSD_STATUS Topaz_impStopIntf
(
IN	MSD_QD_DEV	*dev
)
{
	return Topaz_impStop(dev);
}
