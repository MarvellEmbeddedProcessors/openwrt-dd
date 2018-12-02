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
* Topaz_msdBrgFdb.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell ATU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz_msdBrgFdb_h
#define __Topaz_msdBrgFdb_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported ATU Types			 			                                   */
/****************************************************************************/
/*
 * typedef: enum TOPAZ_MSD_FLUSH_CMD
 *
 * Description: Enumeration of the address translation unit flush operation.
 *
 * Enumerations:
 *   TOPAZ_MSD_FLUSH_ALL       - flush all entries.
 *   TOPAZ_MSD_FLUSH_ALL_NONSTATIC - flush all non-static entries.
 */
typedef enum
{
    TOPAZ_MSD_FLUSH_ALL       = 1,
    TOPAZ_MSD_FLUSH_ALL_NONSTATIC = 2,
} TOPAZ_MSD_FLUSH_CMD;

/*
 * typedef: enum TOPAZ_MSD_MOVE_CMD
 *
 * Description: Enumeration of the address translation unit move or remove operation.
 *     When destination port is set to 0xF, Remove operation occurs.
 *
 * Enumerations:
 *   TOPAZ_MSD_MOVE_ALL       - move all entries.
 *   TOPAZ_MSD_MOVE_ALL_NONSTATIC - move all non-static entries.
 */
typedef enum
{
    TOPAZ_MSD_MOVE_ALL       = 1,
    TOPAZ_MSD_MOVE_ALL_NONSTATIC = 2,
} TOPAZ_MSD_MOVE_CMD;

/*
 *  typedef: struct TOPAZ_MSD_ATU_EXT_PRI
 *
 *  Description: 
 *        Extanded priority information for the address tarnslaton unit entry.
 *
 *        macFPri data is used to override the frame priority on any frame associated
 *        with this MAC, if the useMacFPri is MSD_TRUE and the port's SA and/or 
 *        DA FPriOverride are enabled. SA Frame Priority Overrides can only occur on 
 *        MAC addresses that are Static or where the Port is Locked, and where the port 
 *        is mapped source port for the MAC address.
 *
 *  Fields:
 *      macFPri    - MAC frame priority data (0 ~ 7).
 *
 */
typedef struct
{
    MSD_U8             macFPri;
} TOPAZ_MSD_ATU_EXT_PRI;


/*
 *  typedef: struct TOPAZ_MSD_ATU_ENTRY
 *
 *  Description: address translation unit Entry
 *
 *  Fields:
 *      macAddr    - mac address
 *      trunkMember- MSD_TRUE if entry belongs to a Trunk. This field will be
 *                     ignored if device does not support Trunk.
 *      portVec    - port Vector. 
 *                     If trunkMember field is MSD_TRUE, this value represents trunk ID.
 *      entryState - the entry state.
 *      DBNum       - ATU MAC Address Database number. If multiple address 
 *                     databases are not being used, DBNum should be zero.
 *                     If multiple address databases are being used, this value
 *                     should be set to the desired address database number.
 *      exPrio     - extended priority information. 
 *
 *  Comment:
 *      The entryState union Type is determine according to the Mac Type.
 */
typedef struct
{
    MSD_ETHERADDR      macAddr;
    MSD_BOOL           trunkMember;
    MSD_U32            portVec;
    MSD_U8             reserved;
    MSD_U16            DBNum;
    MSD_U8			  entryState;
    TOPAZ_MSD_ATU_EXT_PRI    exPrio;
} TOPAZ_MSD_ATU_ENTRY;

/*
 *  typedef: struct TOPAZ_MSD_ATU_INT_STATUS
 *
 *  Description: VLAN tarnslaton unit interrupt status
 *
 *  Fields:
 *      intCause  - ATU Interrupt Cause
 *                    TOPAZ_MSD_AGE_OUT_VIOLATION,TOPAZ_MSD_MEMEBER_VIOLATION,
 *                    TOPAZ_MSD_MISS_VIOLATION, TOPAZ_MSD_FULL_VIOLATION
 *                    
 *      SPID      - source port number
 *                     if intCause is TOPAZ_MSD_FULL_VIOLATION, it means nothing
 *      DBNum     - DB Num (or FID)
 *                     if intCause is TOPAZ_MSD_FULL_VIOLATION, it means nothing
 *      macAddr      - MAC Address
 */
typedef struct
{
    MSD_U16   atuIntCause;
    MSD_U8    spid;
    MSD_U8    dbNum;
    MSD_ETHERADDR  macAddr;
} TOPAZ_MSD_ATU_INT_STATUS;

/*
 * Bit Vector Definition for ATU Violation
 */
#define TOPAZ_MSD_ATU_AGE_OUT_VIOLATION		0x8
#define TOPAZ_MSD_ATU_MEMBER_VIOLATION         0x4
#define TOPAZ_MSD_ATU_MISS_VIOLATION           0x2
#define TOPAZ_MSD_ATU_FULL_VIOLATION       	0x1

/*
* Maximum LearnLimit Counter
*/
#define TOPAZ_MAX_ATU_PORT_LEARNLIMIT  0x3FF

/****************************************************************************/
/* Exported ATU Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Topaz_gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFlush
(
    IN MSD_QD_DEV *dev,
    IN TOPAZ_MSD_FLUSH_CMD flushCmd
);
MSD_STATUS Topaz_gfdbFlushIntf
(
    IN MSD_QD_DEV *dev,
    IN MSD_FLUSH_CMD flushCmd
);
/*******************************************************************************
* Topaz_gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*       API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       DBNum     - ATU MAC Address Database Number.
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
*		none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFlushInDB
(
    IN MSD_QD_DEV *dev,
    IN TOPAZ_MSD_FLUSH_CMD flushCmd,
    IN MSD_U32 DBNum
);
MSD_STATUS Topaz_gfdbFlushInDBIntf
(
    IN MSD_QD_DEV *dev,
    IN MSD_FLUSH_CMD flushCmd,
    IN MSD_U32 DBNum
);
/*******************************************************************************
* Topaz_gfdbMove
*
* DESCRIPTION:
*        This routine moves all or unblocked addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbMove
(
    IN MSD_QD_DEV     *dev,
    IN TOPAZ_MSD_MOVE_CMD    moveCmd,
    IN MSD_U32        moveFrom,
    IN MSD_U32        moveTo
);
MSD_STATUS Topaz_gfdbMoveIntf
(
    IN MSD_QD_DEV     *dev,
    IN MSD_MOVE_CMD    moveCmd,
    IN MSD_U32        moveFrom,
    IN MSD_U32        moveTo
);

/*******************************************************************************
* Topaz_gfdbPortRemoveIntf
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbPortRemoveIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_MOVE_CMD  moveCmd,
IN  MSD_LPORT portNum
);

/*******************************************************************************
* Topaz_gfdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or unblocked addresses which are in the particular
*        ATU Database (DBNum) from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        DBNum         - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbMoveInDB
(
    IN MSD_QD_DEV	*dev,
    IN TOPAZ_MSD_MOVE_CMD	moveCmd,
    IN MSD_U32       DBNum,
    IN MSD_U32       moveFrom,
    IN MSD_U32       moveTo
);
MSD_STATUS Topaz_gfdbMoveInDBIntf
(
    IN MSD_QD_DEV	*dev,
    IN MSD_MOVE_CMD	moveCmd,
    IN MSD_U32       DBNum,
    IN MSD_U32       moveFrom,
    IN MSD_U32       moveTo
);

/*******************************************************************************
* Topaz_gfdbPortRemoveInDBIntf
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port in the
*        specified ATU DBNum.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*        DBNum    - fid
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbPortRemoveInDBIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_MOVE_CMD  moveCmd,
IN  MSD_U32  DBNum,
IN  MSD_LPORT  portNum
);

/*******************************************************************************
* Topaz_gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
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
*       DBNum in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbAddMacEntry
(
    IN MSD_QD_DEV    *dev,
    IN TOPAZ_MSD_ATU_ENTRY *macEntry
);
MSD_STATUS Topaz_gfdbAddMacEntryIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_ATU_ENTRY *macEntry
);
/*******************************************************************************
* Topaz_gfdbDelAtuEntry
*
* DESCRIPTION:
*       Deletes ATU entry.
*
* INPUTS:
*       macEntry - the ATU entry to be deleted.
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
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbDelAtuEntry
(
    IN MSD_QD_DEV     *dev,
    IN MSD_ETHERADDR  *macAddr,
    IN MSD_U32   fid
);

/*******************************************************************************
* Topaz_gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address from the specified Mac Addr.
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetAtuEntryNext
(
    IN MSD_QD_DEV *dev,
    INOUT TOPAZ_MSD_ATU_ENTRY  *atuEntry
);
MSD_STATUS Topaz_gfdbGetAtuEntryNextIntf
(
    IN MSD_QD_DEV *dev,
    INOUT MSD_ATU_ENTRY  *atuEntry
);
/*******************************************************************************
* Topaz_gfdbGetViolation
*
* DESCRIPTION:
*       Get ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT TOPAZ_MSD_ATU_INT_STATUS *atuIntStatus
);
MSD_STATUS Topaz_gfdbGetViolationIntf
(
    IN  MSD_QD_DEV         *dev,
    OUT MSD_ATU_INT_STATUS *atuIntStatus
);

/*******************************************************************************
* Topaz_gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       atuEntry - the Mac address to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM  - on bad parameter
*
* COMMENTS:
*        DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFindAtuMacEntry
(
	IN MSD_QD_DEV    *dev,
	INOUT TOPAZ_MSD_ATU_ENTRY  *atuEntry,
	OUT MSD_BOOL         *found
);
MSD_STATUS Topaz_gfdbFindAtuMacEntryIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_ETHERADDR *macAddr,
    IN  MSD_U32       fid,
    OUT MSD_ATU_ENTRY *atuEntry,
    OUT MSD_BOOL      *found
);

/*******************************************************************************
* Topaz_gfdbSetAgingTimeout
*
* DESCRIPTION:
*        Sets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The standard recommends 300 sec.
*        For Topaz, the time-base is 3750 milliseconds, so it can be set to 3750,
*        3750*2, 3750*3,...3750*0xff (almost 16 minites)
*        Since in this API, parameter 'timeout' is milliseconds based,we will set the value
*        rounded to the nearest supported value smaller than the given timeout.
*        If the given timeout is less than 3750, minimum timeout value
*        3750 will be used instead. E.g.) 3800 becomes 3750 and 3000 becomes 3750.
*
* INPUTS:
*       timeout - aging time in milliseconds.
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
MSD_STATUS Topaz_gfdbSetAgingTimeout
(
	IN MSD_QD_DEV    *dev,
	IN MSD_U32 timeout
);

/*******************************************************************************
* Topaz_gfdbGetAgingTimeout
*
* DESCRIPTION:
*        Gets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The returned value may not be the same as the value
*        programmed with <gfdbSetAgingTimeout>. Please refer to the description of
*        <gfdbSetAgingTimeout>.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       timeout - aging time in milliseconds.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetAgingTimeout
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  *timeout
);

/*******************************************************************************
* Topaz_gfdbGetLearn2All
*
* DESCRIPTION:
*        When more than one Marvell device is used to form a single 'switch', it
*        may be desirable for all devices in the 'switch' to learn any address this
*        device learns. When this bit is set to a one all other devices in the
*        'switch' learn the same addresses this device learns. When this bit is
*        cleared to a zero, only the devices that actually receive frames will learn
*        from those frames. This mode typically supports more active MAC addresses
*        at one time as each device in the switch does not need to learn addresses
*        it may nerver use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - MSD_TRUE if Learn2All is enabled, MSD_FALSE otherwise
*
* RETURNS:
*        MSD_OK           - on success
*        MSD_FAIL         - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetLearn2All
(
IN  MSD_QD_DEV  *dev,
OUT MSD_BOOL  *mode
);

/*******************************************************************************
* Topaz_gfdbSetLearn2All
*
* DESCRIPTION:
*        Enable or disable Learn2All mode.
*
* INPUTS:
*        mode - MSD_TRUE to set Learn2All, MSD_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbSetLearn2All
(
IN  MSD_QD_DEV  *dev,
IN  MSD_BOOL  mode
);

/*******************************************************************************
* Topaz_gfdbSetPortLearnLimit
*
* DESCRIPTION:
*        Set auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbSetPortLearnLimit
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U32  limit
);

/*******************************************************************************
* Topaz_gfdbGetPortLearnLimit
*
* DESCRIPTION:
*        Get auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetPortLearnLimit
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
OUT MSD_U32  *limit
);

/*******************************************************************************
* Topaz_gfdbGetPortLearnCount
*
* DESCRIPTION:
*        Read the current number of active unicast MAC addresses associated with
*        the given port. This counter (LearnCnt) is held at zero if learn limit
*        (gfdbSetPortAtuLearnLimit API) is set to zero.
*
* INPUTS:
*       port  - logical port number
*
* OUTPUTS:
*       count - current auto learning count
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetPortLearnCount
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U32  *count
);

/*******************************************************************************
* Topaz_gfdbGetEntryCount
*
* DESCRIPTION:
*       Counts all entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetEntryCount
(
IN  MSD_QD_DEV  *dev,
OUT MSD_U32  *count
);

/*******************************************************************************
* Topaz_gfdbGetEntryCountPerFid
*
* DESCRIPTION:
*       Counts all entries in the specified fid in Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetEntryCountPerFid
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  fid,
OUT MSD_U32  *count
);

/*******************************************************************************
* Topaz_gfdbGetNonStaticEntryCount
*
* DESCRIPTION:
*       Counts all non-static entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetNonStaticEntryCount
(
IN  MSD_QD_DEV  *dev,
OUT MSD_U32  *count
);

/*******************************************************************************
* Topaz_gfdbGetNonStaticEntryCountPerFid
*
* DESCRIPTION:
*       Counts all non-static entries in the specified fid in the Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetNonStaticEntryCountPerFid
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  fid,
OUT MSD_U32  *count
);

/*******************************************************************************
* gfdbDump
*
* DESCRIPTION:
*       Find all MAC address in the specified fid and print it out.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbDump
(
IN MSD_QD_DEV    *dev,
IN MSD_U32       fid
);

#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdBrgFdb_h */
