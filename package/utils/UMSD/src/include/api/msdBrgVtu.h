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
* msdBrgVtu.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell VTU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdBrgVtu_h
#define __msdBrgVtu_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdVlanEntryFind
*
* DESCRIPTION:
*       Find VTU entry for a specific VID(13bits data for Peridot and 12bits for others), 
*       it will return the entry, if found, along with its associated data       
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       entry  - pointer to VTU entry structure
*       found  - 1/0 for found / not found
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVlanEntryFind
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
);
/*******************************************************************************
* msdVlanExists
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
MSD_STATUS msdVlanExists
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
);
/*******************************************************************************
* msdVlanEntryNextGet
*
* DESCRIPTION:
*       Gets next valid vlan entry from the specified vid.
*
* INPUTS:
*       vlanId  - the vlan id to start the search
*
* OUTPUTS:
*       vlanEntry - next valid vlan entry
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*       MSD_NO_SUCH - no more entries
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryNextGet
(
    IN  MSD_U8 devNum,
    IN  MSD_U16 vlanId,
    OUT MSD_VTU_ENTRY *vlanEntry
);
/*******************************************************************************
* msdVlanAllDelete
*
* DESCRIPTION:
*       Removes all entries from VTU Table.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
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
MSD_STATUS msdVlanAllDelete
(
    IN  MSD_U8  devNum
);
/*******************************************************************************
* msdVlanEntryAdd
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       devNum - physical device number
*       entry  - pointer to VTU entry structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryAdd
(
    IN  MSD_U8   devNum,
    IN  MSD_VTU_ENTRY *vtuEntry
);
/*******************************************************************************
* msdVlanEntryDelete
*
* DESCRIPTION:
*       This routine deletes VTU entry specified by VID in vtuEntry.
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryDelete
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId
);
/*******************************************************************************
* msdVlanModeSet
*
* DESCRIPTION:
*       Sets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*       MSD_VTU_MODE - VTU Mode specified in the enumeration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_VTU_MODE  mode
);
/*******************************************************************************
* msdVlanModeGet
*
* DESCRIPTION:
*       Gets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       MSD_VTU_MODE - VTU Mode specified in the enumeration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_MODE  *mode
);
/*******************************************************************************
* msdVlanViolationGet
*
* DESCRIPTION:
*       Gets VTU Violation data
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid
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
MSD_STATUS msdVlanViolationGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_INT_STATUS  *vtuIntStatus
);
/*******************************************************************************
* msdVlanEntryCountGet
*
* DESCRIPTION:
*       Gets number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
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
*       none
*
*******************************************************************************/
MSD_STATUS msdVlanEntryCountGet
(
    IN  MSD_U8   devNum,
    OUT MSD_U16  *count
);

/*******************************************************************************
* msdVlanMemberTagSet
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
MSD_STATUS msdVlanMemberTagSet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
);
/*******************************************************************************
* msdVlanMemberTagGet
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
MSD_STATUS msdVlanMemberTagGet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
);

/*******************************************************************************
* msdVtuDump
*
* DESCRIPTION:
*       Finds all VTU Entries and print it out.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
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
MSD_STATUS msdVtuDump
(
    IN  MSD_U8  devNum
);
#ifdef __cplusplus
}
#endif

#endif /* __msdBrgVtu_h */
