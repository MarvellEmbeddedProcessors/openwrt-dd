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
* Agate_msdBrgVtu.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell VTU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Agate_msdBrgVtu_h
#define __Agate_msdBrgVtu_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************************/
	/* Exported VTU Types			 			                                   */
	/****************************************************************************/
	/*
	*  typedef: struct AGATE_MSD_VTU_EXT_INFO
	*
	*  Description:
	*        Extanded VTU Entry information for Priority Override and Non Rate Limit.
	*        Frame Priority is used to as the tag's PRI bits if the frame egresses
	*        the switch tagged. The egresss queue the frame is switch into is not
	*        modified by the Frame Priority Override.
	*        Queue Priority is used to determine the egress queue the frame is
	*        switched into. If the frame egresses tagged, the priority in the frame
	*        will not be modified by a Queue Priority Override.
	*        NonRateLimit for VID is used to indicate any frames associated with this
	*        VID are to bypass ingress and egress rate limiting, if the ingress
	*        port's VID NRateLimit is also enabled.
	*
	*  Fields:
	*       useVIDFPri - Use VID frame priority override. When this is MSD_TRUE and
	*                     VIDFPriOverride of the ingress port of the frame is enabled,
	*                     vidFPri data is used to override the frame priority on
	*                     any frame associated with this VID.
	*       vidFPri    - VID frame priority data (0 ~ 7).
	*       dontLearn  - Don's learn. When this bit is set to one and this VTU entry is
	*					   accessed by a frame, then learning will not take place on this frame
	*       filterUC   - Filter Unicast frames.
	*       filterBC   - Filter Broadcast frames.
	*       filterMC   - Filter Multicast frames.
	*
	*/
	typedef struct
	{
		MSD_BOOL           useVIDFPri;
		MSD_U8             vidFPri;
	} AGATE_MSD_VTU_EXT_INFO;

	/*
	*  definition for MEMBER_TAG
	*/
	typedef enum
	{
		Agate_MEMBER_EGRESS_UNMODIFIED = 0,
		Agate_MEMBER_EGRESS_UNTAGGED,
		Agate_MEMBER_EGRESS_TAGGED,
		Agate_NOT_A_MEMBER
	}AGATE_MSD_MEMTAGP;

	/*
	*  typedef: struct AGATE_MSD_VTU_ENTRY
	*
	*  Description: VLAN tarnslaton unit Entry
	*        Each field in the structure is device specific, i.e., some fields may not
	*        be supported by the used switch device. In such case, those fields are
	*        ignored by the DSDT driver. Please refer to the datasheet for the list of
	*        supported fields.
	*
	*  Fields:
	*      DBNum      - database number or FID (forwarding information database)
	*      vid        - VLAN ID
	*      sid        - 802.1s Port State Database ID
	*		memberTagP - Membership and Egress Tagging
	*                   memberTagP[0] is for Port 0, MemberTagP[1] is for port 1, and so on
	*      vidPolicy  - indicate that the frame with this VID uses VID Policy
	*                     (see gprtSetPolicy API).
	*      vidExInfo  - extanded information for VTU entry. If the device supports extanded
	*                     information, vidPriorityOverride and vidPriority values are
	*                     ignored.
	*/
	typedef struct
	{
		MSD_U16        DBNum;
		MSD_U16        vid;
		MSD_U8         sid;
		AGATE_MSD_MEMTAGP    memberTagP[MSD_MAX_SWITCH_PORTS];
		MSD_BOOL          vidPolicy;
		AGATE_MSD_VTU_EXT_INFO    vidExInfo;
	} AGATE_MSD_VTU_ENTRY;

	/*
	*  typedef: struct AGATE_MSD_VTU_INT_STATUS
	*
	*  Description: VLAN tarnslaton unit interrupt status
	*
	*  Fields:
	*      intCause  - VTU Interrupt Cause
	*                    MSD_VTU_FULL_VIOLATION,MSD_MEMEBER_VIOLATION,or
	*                    MSD_MISS_VIOLATION
	*      SPID      - source port number
	*                     if intCause is MSD_VTU_FULL_VIOLATION, it means nothing
	*      vid       - VLAN ID
	*                     if intCause is MSD_VTU_FULL_VIOLATION, it means nothing
	*/
	typedef struct
	{
		MSD_U16   vtuIntCause;
		MSD_U8    spid;
		MSD_U16   vid;
	} AGATE_MSD_VTU_INT_STATUS;

	/*
	* Bit Vector Definition for VTU Violation
	*/
#define AGATE_MSD_VTU_MEMBER_VIOLATION            	0x4
#define AGATE_MSD_VTU_MISS_VIOLATION              	0x2
#define AGATE_MSD_VTU_FULL_VIOLATION       	 	0x1       

	/*
	*  typedef: struct AGATE_MSD_STU_ENTRY
	*
	*  Description: 802.1s Port State Information Database (STU) Entry
	*
	*  Fields:
	*      sid       - STU ID
	*        portState - Per VLAN Port States for each port.
	*/

	/****************************************************************************/
	/* Exported VTU Functions		 			                                   */
	/****************************************************************************/

	/*******************************************************************************
	* Agate_gvtuGetEntryNext
	*
	* DESCRIPTION:
	*       Gets next valid VTU entry from the specified VID.
	*
	* INPUTS:
	*       vtuEntry - the VID to start the search.
	*
	* OUTPUTS:
	*       vtuEntry - match VTU  entry.
	*
	* RETURNS:
	*       MSD_OK      - on success.
	*       MSD_FAIL    - on error
	*       MSD_NO_SUCH - no more entries.
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*       Search starts from the VID in the vtuEntry specified by the user.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuGetEntryNext
		(
		IN  MSD_QD_DEV         *dev,
		INOUT AGATE_MSD_VTU_ENTRY    *vtuEntry
		);
	MSD_STATUS Agate_gvtuGetEntryNextIntf
		(
		IN MSD_QD_DEV    *dev,
		INOUT MSD_VTU_ENTRY *vtuEntry
		);

	/*******************************************************************************
	* Agate_gvtuFlush
	*
	* DESCRIPTION:
	*       This routine removes all entries from VTU Table.
	*
	* INPUTS:
	*       None
	*
	* OUTPUTS:
	*       None
	*
	* RETURNS:
	*       MSD_OK  - on success
	*       MSD_FAIL- on error
	*
	* COMMENTS:
	*		None
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuFlush
		(
		IN MSD_QD_DEV *dev
		);

	MSD_STATUS Agate_gvtuFlushIntf
		(
		IN MSD_QD_DEV *dev
		);
	/*******************************************************************************
	* Agate_gvtuAddEntry
	*
	* DESCRIPTION:
	*       Creates the new entry in VTU table based on user input.
	*
	* INPUTS:
	*       vtuEntry - vtu entry to insert to the VTU.
	*
	* OUTPUTS:
	*       None
	*
	* RETURNS:
	*       MSD_OK  - on success
	*       MSD_FAIL- on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*       None.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuAddEntry
		(
		IN MSD_QD_DEV     *dev,
		IN AGATE_MSD_VTU_ENTRY *vtuEntry
		);

	MSD_STATUS Agate_gvtuAddEntryIntf
		(
		IN MSD_QD_DEV     *dev,
		IN MSD_VTU_ENTRY *vtuEntry
		);
	/*******************************************************************************
	* Agate_gvtuDelEntry
	*
	* DESCRIPTION:
	*       Deletes VTU entry specified by user.
	*
	* INPUTS:
	*       vtuEntry - the VTU entry to be deleted
	*
	* OUTPUTS:
	*       None.
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_NO_SUCH - if specified address entry does not exist
	*       MSD_BAD_PARAM - if invalid parameter is given
	*
	* COMMENTS:
	*       None.
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuDelEntry
		(
		IN MSD_QD_DEV     *dev,
		IN AGATE_MSD_VTU_ENTRY  *vtuEntry
		);

	MSD_STATUS Agate_gvtuDelEntryIntf
		(
		IN MSD_QD_DEV     *dev,
		IN MSD_U16 vlanId
		);

	/*******************************************************************************
	* Agate_gvtuGetViolation
	*
	* DESCRIPTION:
	*       Get VTU Violation data
	*
	* INPUTS:
	*       None.
	*
	* OUTPUTS:
	*       vtuIntStatus - interrupt cause, source portID, and vid.
	*
	* RETURNS:
	*       MSD_OK  - on success
	*       MSD_FAIL- on error
	*
	* COMMENTS:
	*       None
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuGetViolation
		(
		IN  MSD_QD_DEV         *dev,
		OUT AGATE_MSD_VTU_INT_STATUS *vtuIntStatus
		);

	MSD_STATUS Agate_gvtuGetViolationIntf
		(
		IN  MSD_QD_DEV         *dev,
		OUT MSD_VTU_INT_STATUS *vtuIntStatus
		);

	/*******************************************************************************
	* Agate_gvtuFindVidEntry
	*
	* DESCRIPTION:
	*       Find VTU entry for a specific VID, it will return the entry, if found,
	*       along with its associated data
	*
	* INPUTS:
	*       vtuEntry - contains the VID to searche for
	*
	* OUTPUTS:
	*       found    - MSD_TRUE, if the appropriate entry exists.
	*       vtuEntry - the entry parameters.
	*
	* RETURNS:
	*       MSD_OK      - on success.
	*       MSD_FAIL    - on error or entry does not exist.
	*       MSD_NO_SUCH - no more entries.
	*
	* COMMENTS:
	*
	* GalTis:
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuFindVidEntry
		(
		IN  MSD_QD_DEV       *dev,
		INOUT AGATE_MSD_VTU_ENTRY  *vtuEntry,
		OUT MSD_BOOL         *found
		);
	MSD_STATUS Agate_gvtuFindVidEntryIntf
		(
		IN  MSD_QD_DEV *dev,
		IN  MSD_U16    vlanId,
		OUT MSD_VTU_ENTRY *entry,
		OUT MSD_BOOL *found
		);
	/*******************************************************************************
	* Agate_gvtuExistVidEntryIntf
	*
	* DESCRIPTION:
	*       Check if the vlan entry with a specific vlan id exist or not in VTU table.
	*
	* INPUTS:
	*       devNum - physical device number
	*       vlanId - vlan id.
	*
	* OUTPUTS:
	*       isExists   - 1/0 for exist / not exist
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*       MSD_NOT_SUPPORTED - device not supported
	*
	* COMMENTS:
	*		None
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuExistVidEntryIntf
		(
		IN  MSD_QD_DEV *dev,
		IN  MSD_U16  vlanId,
		OUT MSD_BOOL *isExists
		);
	/*******************************************************************************
	* Agate_gvtuGetEntryCountIntf
	*
	* DESCRIPTION:
	*       Get number of valid entries in the VTU table
	*
	* INPUTS:
	*       devNum      - physical device number
	*
	* OUTPUTS:
	*       numEntries  - number of valid entries
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*       MSD_NOT_SUPPORTED - device not supported
	*
	* COMMENTS:
	*       none
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuGetEntryCountIntf
		(
		IN  MSD_QD_DEV *dev,
		OUT MSD_U16    *count
		);
	/*******************************************************************************
	* Agate_gvtuSetMemberTagIntf
	*
	* DESCRIPTION:
	*       If the entry with this vlan id exist in the VTU table, update the memberTag
	*       to Allow/Block vlan on the port. If it not exist, return MSD_NO_SUCH
	*
	* INPUTS:
	*       devNum  - physical device number
	*       vlanId  - vlan id
	*       portNum - port number
	*       memberTag - enum element of MSD_PORT_MEMBER_TAG
	*
	* OUTPUTS:
	*       None
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*       MSD_NO_SUCH   - the entry with this valn id not exist
	*       MSD_NOT_SUPPORTED - device not supported
	*
	* COMMENTS:
	*		None
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuSetMemberTagIntf
		(
		IN  MSD_QD_DEV *dev,
		IN  MSD_U16  vlanId,
		IN  MSD_LPORT  portNum,
		IN  MSD_PORT_MEMBER_TAG memberTag
		);
	/*******************************************************************************
	* Agate_gvtuGetMemberTagIntf
	*
	* DESCRIPTION:
	*       If the entry with this vlan id exist in the VTU table, get the memberTag
	*       on the port. If it not exist, return MSD_NO_SUCH.
	*
	* INPUTS:
	*       devNum  - physical device number
	*       vlanId  - vlan id
	*       portNum - port number
	*
	* OUTPUTS:
	*       memberTag - enum element of MSD_VLAN_MEMBER_TAG
	*
	* RETURNS:
	*       MSD_OK      - on success
	*       MSD_FAIL    - on error
	*       MSD_BAD_PARAM - if invalid parameter is given
	*       MSD_NO_SUCH   - the entry with this valn id not exist
	*       MSD_NOT_SUPPORTED - device not supported
	*
	* COMMENTS:
	*		None
	*
	*******************************************************************************/
	MSD_STATUS Agate_gvtuGetMemberTagIntf
		(
		IN  MSD_QD_DEV *dev,
		IN  MSD_U16  vlanId,
		IN  MSD_LPORT  portNum,
		OUT MSD_PORT_MEMBER_TAG *memberTag
		);

	MSD_STATUS Agate_gvtuDump
		(
		IN MSD_QD_DEV    *dev
		);
#ifdef __cplusplus
}
#endif

#endif /* __Agate_msdBrgVtu_h */
