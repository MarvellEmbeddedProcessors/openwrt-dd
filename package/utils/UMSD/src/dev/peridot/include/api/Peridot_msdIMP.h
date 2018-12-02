#include <msdCopyright.h>
/*******************************************************************************
* Peridot_msdIMP.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell IMP functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_msdIMP_h
#define __Peridot_msdIMP_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

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
);
MSD_STATUS Peridot_impRunIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr
);
/*******************************************************************************
* Peridot_impReset
*
* DESCRIPTION:
*       This routine is to reset Peridot_imp
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
);
MSD_STATUS Peridot_impResetIntf
(
IN	MSD_QD_DEV	*dev
);
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
);
MSD_STATUS Peridot_impWriteEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U8	data
);
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
);
MSD_STATUS Peridot_impLoadToEEPROMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	dataSize,
IN	MSD_U8*	data
);
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
);
MSD_STATUS Peridot_impLoadToRAMIntf
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
MSD_STATUS Peridot_impStop
(
IN	MSD_QD_DEV	*dev
);
MSD_STATUS Peridot_impStopIntf
(
IN	MSD_QD_DEV	*dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdIMP_h */