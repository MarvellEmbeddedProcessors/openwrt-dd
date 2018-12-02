#include <msdCopyright.h>
/*******************************************************************************
* Pearl_msdIMP.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell IMP functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Pearl_msdIMP_h
#define __Pearl_msdIMP_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* impRun
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
);

/*******************************************************************************
* impReset
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
);

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
);

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
);

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
);
MSD_STATUS Pearl_impStop
(
IN	MSD_QD_DEV	*dev
);
/*****************************/
MSD_STATUS Pearl_impRunIntf
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr
);

MSD_STATUS Pearl_impResetIntf
(
	IN	MSD_QD_DEV	*dev
);

MSD_STATUS Pearl_impWriteEEPROMIntf
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
);

MSD_STATUS Pearl_impLoadToEEPROMIntf
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
);

MSD_STATUS Pearl_impLoadToRAMIntf
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
);
MSD_STATUS Pearl_impStopIntf
(
IN	MSD_QD_DEV	*dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdIMP_h */