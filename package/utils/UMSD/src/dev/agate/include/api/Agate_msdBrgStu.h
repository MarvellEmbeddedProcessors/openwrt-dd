#include <msdCopyright.h>
/*******************************************************************************
*              Copyright 2017, MARVELL SEMICONDUCTOR, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* Agate_msdBrgFdb.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell STU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Agate_msdBrgStu_h
#define __Agate_msdBrgStu_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************************/
	/* Exported STU Types			 			                                   */
	/****************************************************************************/
	/*
	* typedef: enum AGATE_MSD_PORT_STP_STATE
	*
	* Description: Enumeration of the port spanning tree state.
	*
	* Enumerations:
	*   AGATE_MSD_PORT_DISABLE    - port is disabled.
	*   AGATE_MSD_PORT_BLOCKING   - port is in blocking/listening state.
	*   AGATE_MSD_PORT_LEARNING   - port is in learning state.
	*   AGATE_MSD_PORT_FORWARDING - port is in forwarding state.
	*/
#ifndef AGATE_STP_STATE
	typedef enum
	{
		AGATE_MSD_PORT_DISABLE = 0,
		AGATE_MSD_PORT_BLOCKING,
		AGATE_MSD_PORT_LEARNING,
		AGATE_MSD_PORT_FORWARDING
	} AGATE_MSD_PORT_STP_STATE;
#define AGATE_STP_STATE
#endif

	/*
	*  typedef: struct AGATE_MSD_STU_ENTRY
	*
	*  Description: 802.1s Port State Information Database (STU) Entry
	*
	*  Fields:
	*      sid       - STU ID
	*      portState - Per VLAN Port States for each port.
	*/
	typedef struct
	{
		MSD_U16                sid;
		AGATE_MSD_PORT_STP_STATE    portState[MSD_MAX_SWITCH_PORTS];
	} AGATE_MSD_STU_ENTRY;

	/****************************************************************************/
	/* Exported STU Functions		 			                                   */
	/****************************************************************************/
	/*******************************************************************************
	* Agate_gstuGetEntryNext
	*
	* DESCRIPTION:
	*       Gets next valid STU entry from the specified SID.
	*
	* INPUTS:
	*       stuEntry - the SID to start the search.
	*
	* OUTPUTS:
	*       stuEntry - next STU entry.
	*
	* RETURNS:
	*       MSD_OK      - on success.
	*       MSD_FAIL    - on error
	*       MSD_NO_SUCH - no more entries.
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*
	*******************************************************************************/
	MSD_STATUS Agate_gstuGetEntryNext
		(
		IN  MSD_QD_DEV       *dev,
		INOUT AGATE_MSD_STU_ENTRY  *stuEntry
		);
	MSD_STATUS Agate_gstuGetEntryNextIntf
		(
		IN  MSD_QD_DEV       *dev,
		INOUT MSD_STU_ENTRY  *stuEntry
		);
	/*******************************************************************************
	* Agate_gstuAddEntry
	*
	* DESCRIPTION:
	*       Creates or update the entry in STU table based on SID.
	*
	* INPUTS:
	*       stuEntry    - stu entry to insert to the STU.
	*
	* OUTPUTS:
	*       None
	*
	* RETURNS:
	*       MSD_OK   - on success
	*       MSD_FAIL - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*       None.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gstuAddEntry
		(
		IN  MSD_QD_DEV       *dev,
		IN  AGATE_MSD_STU_ENTRY    *stuEntry
		);
	MSD_STATUS Agate_gstuAddEntryIntf
		(
		IN  MSD_QD_DEV       *dev,
		IN  MSD_STU_ENTRY    *stuEntry
		);
	/*******************************************************************************
	* Agate_gstuDelEntry
	*
	* DESCRIPTION:
	*       Deletes STU entry specified by SID.
	*
	* INPUTS:
	*       stuEntry - the STU entry to be deleted
	*
	* OUTPUTS:
	*       None.
	*
	* RETURNS:
	*       MSD_OK   - on success
	*       MSD_FAIL - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*       None.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gstuDelEntry
		(
		IN  MSD_QD_DEV       *dev,
		IN  AGATE_MSD_STU_ENTRY    *stuEntry
		);
	MSD_STATUS Agate_gstuDelEntryIntf
		(
		IN  MSD_QD_DEV       *dev,
		IN  MSD_U16 sid
		);

	/*******************************************************************************
	* Agate_gstuFindSidEntry
	*
	* DESCRIPTION:
	*       Find STU entry for a specific SID, it will return the entry, if found,
	*       along with its associated data
	*
	* INPUTS:
	*       stuEntry - contains the SID to searche for
	*
	* OUTPUTS:
	*       found    - MSD_TRUE, if the appropriate entry exists.
	*       stuEntry - the entry parameters.
	*
	* RETURNS:
	*       MSD_OK      - on success.
	*       MSD_FAIL    - on error or entry does not exist.
	*       MSD_NO_SUCH - no such entry.
	*       MSD_NOT_SUPPORTED - if current device does not support this feature.
	*
	* COMMENTS:
	*        Valid SID is 0 ~ 63.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gstuFindSidEntry
		(
		IN  MSD_QD_DEV       *dev,
		INOUT AGATE_MSD_STU_ENTRY  *stuEntry,
		OUT MSD_BOOL         *found
		);
	MSD_STATUS Agate_gstuFindSidEntryIntf
		(
		IN  MSD_QD_DEV *dev,
		IN  MSD_U16    sid,
		OUT MSD_STU_ENTRY *stuEntry,
		OUT MSD_BOOL *found
		);

	/*******************************************************************************
	* Agate_gstuGetEntryCountIntf
	*
	* DESCRIPTION:
	*       Get number of valid entries in the STU table
	*
	* INPUTS:
	*       devNum - physical device number
	*
	* OUTPUTS:
	*       count  - number of valid entries
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*       MSD_NOT_SUPPORTED - device not supported
	*
	* COMMENTS:
	*
	*******************************************************************************/
	MSD_STATUS Agate_gstuGetEntryCountIntf
		(
		IN  MSD_QD_DEV *dev,
		OUT MSD_U16 *count
		);
	MSD_STATUS Agate_gstuDump
		(
		IN MSD_QD_DEV    *dev
		);
#ifdef __cplusplus
}
#endif

#endif /* __Agate_msdBrgStu_h */
