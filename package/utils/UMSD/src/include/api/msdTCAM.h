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
* msdTCAM.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell TCAM functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __gtTCAM_h
#define __gtTCAM_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdTcamAllDelete
*
* DESCRIPTION:
*       This routine is to flush all entries. A Flush All command will initialize
*       TCAM Pages 0 and 1, offsets 0x02 to 0x1B to 0x0000, and TCAM Page 2 offset
*       0x02 to 0x1B to 0x0000 for all TCAM entries with the exception that TCAM
*       Page 0 offset 0x02 will be initialized to 0x00FF.
*
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamAllDelete
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdTcamEntryDelete
*
* DESCRIPTION:
*       This routine is to flush a single entry. A Flush a single TCAM entry command
*       will write the same values to a TCAM entry as a Flush All command, but it is
*       done to the selected single TCAM entry only.
*
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamEntryDelete
(
    IN  MSD_U8  devNum,
    IN  MSD_U32 tcamPointer
);

/*******************************************************************************
* msdTcamEntryAdd
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM
*       to operate correctly while frames are flowing through the switch.
*       If the entry is currently valid, it must first be flushed. Then page 2 needs
*       to be loaded first, followed by page 1 and then finally page 0.
*       Each page load requires its own write TCAMOp with these TCAM page bits set
*       accordingly.
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*       tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamEntryAdd
(
    IN  MSD_U8  devNum,
    IN  MSD_U32 tcamPointer,
    IN  MSD_TCAM_DATA *tcamData
);

/*******************************************************************************
* msdTcamEntryRead
*
* DESCRIPTION:
*       This routine reads the TCAM entry.
*
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamEntryRead
(
    IN  MSD_U8  devNum,
    IN  MSD_U32 tcamPointer,
    OUT MSD_TCAM_DATA *tcamData
);

/*******************************************************************************
* msdTcamEntryGetNext
*
* DESCRIPTION:
*       This routine finds the next higher TCAM Entry number that is valid starting
*       from the specified TCAM entry . The TCAM Entry register (bits 6:0) is used
*       as the TCAM entry to start from. To find the lowest number TCAM Entry that
*       is valid, start the Get Next operation with TCAM Entry set to 0xFF.
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*
* OUTPUTS:
*       tcamPointer - pointer to the desired entry of TCAM 
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no valid entry exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamEntryGetNext
(
    IN    MSD_U8  devNum,
    INOUT MSD_U32 *tcamPointer,
    OUT   MSD_TCAM_DATA *tcamData
);

/*******************************************************************************
* msdTcamEntryFind
*
* DESCRIPTION:
*       Finds the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*
* OUTPUTS:
*       tcamData - the entry parameters.
*       found    - MSD_TRUE, if the appropriate entry exists.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdTcamEntryFind
(
    IN  MSD_U8  devNum,
    IN	MSD_U32 tcamPointer,
    OUT	MSD_TCAM_DATA  *tcamData,
    OUT	MSD_BOOL *found
);

/*******************************************************************************
* msdPortTcamModeSet
*
* DESCRIPTION:
*       Sets the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*       Note: Do not chage the value while frames are flowing into this port,
*             You must first put the port in the Disable Port State, then change
*             TCAM mode, and then re-enable the port's Port State again.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - the logical port number
*       mode     - tcam mode to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdPortTcamModeSet
(
    IN MSD_U8    devNum,
    IN MSD_LPORT portNum,
    IN MSD_TCAM_MODE mode
);

/*******************************************************************************
* msdPortTcamModeGet
*
* DESCRIPTION:
*       Gets the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - the logical port number
*
* OUTPUTS:
*       mode     - get back tcam mode 
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdPortTcamModeGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_TCAM_MODE *mode
);
/*******************************************************************************
* msdTcamEntryPortAdd
*
* DESCRIPTION:
*       Adds a port to an existing tcam entry. If the tcam entry does not exist 
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdTcamEntryPortAdd
(
    IN MSD_U8    devNum,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
);
/*******************************************************************************
* msdTcamEntryPortRemove
*
* DESCRIPTION:
*       Removes a port to an existing tcam entry. If the tcam entry does not exist
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM 
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdTcamEntryPortRemove
(
    IN MSD_U8    devNum,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
);

/*******************************************************************************
* msdEgrTcamEntryPerPortDelete
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for a particular port. A
*       Flush a single Egress TCAM entry command will write the same values to a
*       Egress TCAM entry as a Flush All command, but it is done to the selected
*		single egress TCAM entry of the selected single port only.
*
*
* INPUTS:
*       devNum      - physical device number
*		portNum		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEgrTcamEntryPerPortDelete
(
    IN  MSD_U8    devNum,
	IN  MSD_LPORT portNum,
    IN  MSD_U32   tcamPointer
);

/*******************************************************************************
* msdEgrTcamEntryAllPortsDelete
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for all switch ports.
*
* INPUTS:
*       devNum      - physical device number
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEgrTcamEntryAllPortsDelete
(
    IN  MSD_U8  devNum,
    IN  MSD_U32 tcamPointer
);

/*******************************************************************************
* msdEgrTcamEntryAdd
*
* DESCRIPTION:
*       This routine loads a single egress TCAM entry for a specific port.
*
* INPUTS:
*       devNum      - physical device number
*		portNum		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*       tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEgrTcamEntryAdd
(
    IN  MSD_U8    devNum,
	IN  MSD_LPORT portNum,
    IN  MSD_U32   tcamPointer,
    IN  MSD_TCAM_EGR_DATA  *tcamData
);

/*******************************************************************************
* msdEgrTcamEntryGetNext
*
* DESCRIPTION:
*       This routine finds the next higher Egress TCAM Entry number that is valid
*       starting from the specified egress TCAM entry (i.e.,any entry that is
*       non-zero). The TCAM Entry register (bits 5:0) is used as the Egress TCAM
*       entry to start from. To find the lowest number Egress TCAM Entry that is
*       valid, start the Get Next operation with Egress TCAM Entry set to 0x3F.
*
* INPUTS:
*        devNum      - physical device number
*		 portNum		 - switch port
*        tcamPointer - pointer to start search TCAM
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEgrTcamEntryGetNext
(
    IN    MSD_U8     devNum,
	IN    MSD_LPORT  portNum,
    INOUT MSD_U32    *tcamPointer,
    OUT   MSD_TCAM_EGR_DATA  *tcamData
);

/*******************************************************************************
* msdEgrTcamEntryRead
*
* DESCRIPTION:
*       This routine reads a single Egress TCAM entry for a specific port.
*
*
* INPUTS:
*       devNum      - physical device number
*		portNum		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEgrTcamEntryRead
(
    IN  MSD_U8    devNum,
	IN	MSD_LPORT portNum,
    IN  MSD_U32   tcamPointer,
    OUT MSD_TCAM_EGR_DATA  *tcamData
);

/*******************************************************************************
* msdTcamDump
*
* DESCRIPTION:
*       Finds all valid TCAM entries and print it out.
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
MSD_STATUS msdTcamDump
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdTcamAdvConfig
*
* DESCRIPTION:
*       This routine sets tcam entry to match standard IPv4/IPv6, TCP/UDP packets
*       with the given header data and mask.
*
* INPUTS:
*       devNum  - physical devie number
*       pktType - Enumeration of TCAM mapping packet type
*       tcamPointer  - tcam entry number
*       tcamPointer2 - tcam entry2 number for cascade
*       keyMaskPtr   - mask pointer for TCAM entry key
*       keyPtr       - data pointer for TCAM entry key
*       maskPtr      - mask pointer for packet unit
*       patternPtr   - data pointer for packet unit
*       actionPtr    - action pionter
*
* OUTPUTS:
*       entry2Used - MSD_TRUE: two tcam entries been used, MSD_FALSE: only tcamPointer been used, tcamPointer2 not used.
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
MSD_STATUS msdTcamAdvConfig
(
    IN  MSD_U8  devNum,
    IN  MSD_TCAM_PKT_TYPE pktType,
    IN  MSD_U32 tcamPointer,
    IN  MSD_U32 tcamPointer2,
    IN  MSD_TCAM_ENT_KEY *keyMaskPtr,
    IN  MSD_TCAM_ENT_KEY *keyPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *maskPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *patternPtr,
    IN  MSD_TCAM_ENT_ACT *actionPtr,
    OUT MSD_BOOL *entry2Used
);
#ifdef __cplusplus
}
#endif

#endif /* __gtTCAM_h */
