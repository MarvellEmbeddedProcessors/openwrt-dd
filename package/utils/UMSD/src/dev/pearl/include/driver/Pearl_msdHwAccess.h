#include <msdCopyright.h>

/********************************************************************************
* msdHwAccess.h
*
* DESCRIPTION:
*       Functions declarations for Hw accessing switch register, smi phy registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __Pearl_gtHwAccessh
#define __Pearl_gtHwAccessh

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdHwAccess.h>

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                         */
/****************************************************************************/

/*****************************************************************************
* Pearl_msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdSetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* Pearl_msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       value    - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdGetSMIPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT MSD_U16  *data
);
/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdSetSMIC45PhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address to read.
*
* OUTPUTS:
*       data    - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdGetSMIC45PhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
);

/*****************************************************************************
* PEARL_msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/

MSD_STATUS PEARL_msdSetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN MSD_U16   data
);

/*****************************************************************************
* PEARL_msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS PEARL_msdGetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT  MSD_U16   *data
);
/*****************************************************************************
* msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdSetSMIPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    devAddr,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN MSD_U16   data
);
/*****************************************************************************
* msdGetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_msdGetSMIPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    devAddr,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	OUT  MSD_U16   *data
);

MSD_STATUS Pearl_msdWritePagedPhyReg
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U16   data
);

MSD_STATUS Pearl_msdReadPagedPhyReg
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	OUT MSD_U16   *data
);

MSD_STATUS Pearl_msdSetPagedPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN  MSD_U16   data
);

MSD_STATUS Pearl_msdGetPagedPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN  MSD_U16   *data
);

MSD_STATUS Pearl_msdSetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    IN MSD_U16 data
);

MSD_STATUS Pearl_msdGetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    OUT MSD_U16 *data
);

MSD_STATUS Pearl_msdPhyReset
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_U8        portNum,
	IN  MSD_U8		 pageNum,
	IN  MSD_U16       u16Data
);
#ifdef __cplusplus
}
#endif
#endif /* __Pearl_gtHwAccessh */
