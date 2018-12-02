#include <msdCopyright.h>
/*******************************************************************************
* Agate_msdIMP.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell IMP functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Agate_msdIMP_h
#define __Agate_msdIMP_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Agate_impRun
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
MSD_STATUS Agate_impRun
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr
);
MSD_STATUS Agate_impRunIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr
);
/*******************************************************************************
* Agate_impReset
*
* DESCRIPTION:
*       This routine is to reset Agate_imp
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
MSD_STATUS Agate_impReset
(
	IN	MSD_QD_DEV	*dev
);
MSD_STATUS Agate_impResetIntf
(
IN	MSD_QD_DEV	*dev
);
/*******************************************************************************
* Agate_impWriteEEPROM
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
MSD_STATUS Agate_impWriteEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U8	data
);
MSD_STATUS Agate_impWriteEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U8	data
);
/*******************************************************************************
* Agate_impLoadToEEPROM
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
MSD_STATUS Agate_impLoadToEEPROM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
);
MSD_STATUS Agate_impLoadToEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	dataSize,
IN	MSD_U8*	data
);
/*******************************************************************************
* Agate_impLoadToRAM
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
MSD_STATUS Agate_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	addr,
	IN	MSD_U16	dataSize,
	IN	MSD_U8*	data
);
MSD_STATUS Agate_impLoadToRAMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	dataSize,
IN	MSD_U8*	data
);
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
MSD_STATUS Agate_impStop
(
IN	MSD_QD_DEV	*dev
);
MSD_STATUS Agate_impStopIntf
(
IN	MSD_QD_DEV	*dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Agate_msdIMP_h */