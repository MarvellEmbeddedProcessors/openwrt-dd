#include <msdCopyright.h>

/********************************************************************************
* msdBrgFdb.c
*
* DESCRIPTION:
*       API definitions for Address Translation Unit 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdBrgFdb.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
* msdFdbMacEntryAdd
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
*       MSD_OK - On success
*		MSD_FAIL - On error 
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support

*
* COMMENTS:
*       fid in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, fid should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS msdFdbMacEntryAdd
(
    IN MSD_U8        devNum,
    IN MSD_ATU_ENTRY *macEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
    if (dev->SwitchDevObj.ATUObj.gfdbAddMacEntry != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbAddMacEntry(dev, macEntry);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbAllDelete
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS msdFdbAllDelete
(
IN MSD_U8        devNum,
IN MSD_FLUSH_CMD flushCmd
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbFlush != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbFlush(dev, flushCmd);
	else
		return MSD_NOT_SUPPORTED;
	
}

/*******************************************************************************
* msdFdbAllDeleteInDB
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the particular
*       ATU Database (fid). If multiple address databases are being used, this
*       API can be used to flush entries in a particular fid database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       fid     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS msdFdbAllDeleteInDB
(
IN MSD_U8        devNum,
IN MSD_FLUSH_CMD flushCmd,
IN MSD_U32 fid
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbFlushInDB != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbFlushInDB(dev, flushCmd, fid);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbMove
*
* DESCRIPTION:
*        This routine moves all or all non-static addresses from a port to another.
*        This operation will not work for ATU trunk entry.
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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS msdFdbMove
(
IN MSD_U8        devNum,
IN MSD_MOVE_CMD  moveCmd,
IN MSD_LPORT     moveFrom,
IN MSD_LPORT     moveTo
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbMove != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbMove(dev, moveCmd, moveFrom, moveTo);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or all non-static addresses which are in the particular
*        ATU Database (fid) from a port to another.This operation will not work for ATU trunk entry.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        fid    - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS msdFdbMoveInDB
(
IN MSD_U8        devNum,
IN MSD_MOVE_CMD   moveCmd,
IN MSD_U32        fid,
IN MSD_LPORT      moveFrom,
IN MSD_LPORT      moveTo
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbMoveInDB != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbMoveInDB(dev, moveCmd, fid, moveFrom, moveTo);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbPortRemove
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port.
*        This operation will not work for ATU trunk entry.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum - port which will be removed
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS msdFdbPortRemove
(
IN MSD_U8        devNum,
IN MSD_MOVE_CMD  moveCmd,
IN MSD_LPORT     portNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbPortRemove != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbPortRemove(dev, moveCmd,portNum);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbPortRemoveInDB
*
* DESCRIPTION:
*        This routine removes all or all non-static addresses from a port in the
*        specified ATU fid. This operation will not work for ATU trunk entry.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        fid    - ATU MAC Address Database Number.
*        portNum - port which will be removed
*
* OUTPUTS:
*     None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS msdFdbPortRemoveInDB
(
IN MSD_U8        devNum,
IN MSD_MOVE_CMD   moveCmd,
IN MSD_U32        fid,
IN MSD_LPORT      portNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB(dev, moveCmd, fid, portNum);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbMacEntryDelete
*
* DESCRIPTION:
*       Deletes ATU entry.
*
* INPUTS:
*       macAddr - the ATU entry to be deleted.
*       fid     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       fid in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, fid should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS msdFdbMacEntryDelete
(
IN MSD_U8        devNum,
IN MSD_ETHERADDR  *macAddr,
IN MSD_U32   fid
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry(dev, macAddr, fid);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbViolationGet
*
* DESCRIPTION:
*       Gets ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, fid and mac address.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS msdFdbViolationGet
(
IN MSD_U8        devNum,
OUT MSD_ATU_INT_STATUS *atuIntStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbGetViolation != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbGetViolation(dev, atuIntStatus);
	else
		return MSD_NOT_SUPPORTED; 
}

/*******************************************************************************
* msdFdbMacEntryFind
*
* DESCRIPTION:
*       Finds ATU entry for specific MAC address from the ATU.
*
* INPUTS:
*       macAddr - the Mac address to search.
*       fid     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        fid in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, fid should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS msdFdbMacEntryFind
(
IN MSD_U8        devNum,
IN  MSD_ETHERADDR *macAddr,
IN  MSD_U32       fid,
OUT MSD_ATU_ENTRY *atuEntry,
OUT MSD_BOOL         *found
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry(dev, macAddr, fid, atuEntry, found);
	else
		return MSD_NOT_SUPPORTED; 
}

/*******************************************************************************
* msdFdbAgingTimeoutSet
*
* DESCRIPTION:
*        Sets the timeout period in milliseconds for aging out dynamically.
*        The standard recommends 300 sec.
*        For peridot, the time-base is 3750 millisecond, so it can be set to 3750,
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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdFdbAgingTimeoutSet
(
IN MSD_U8  devNum,
IN MSD_U32 timeout
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout(dev, timeout);
	else
		return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbAgingTimeoutGet
*
* DESCRIPTION:
*        Gets the timeout period in milliseconds for aging out dynamically. 
*        The returned value may not be the same as the value
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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdFdbAgingTimeoutGet
(
IN  MSD_U8  devNum,
OUT MSD_U32 *timeout
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout(dev, timeout);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbLearnOnAllDevicesGet
*
* DESCRIPTION:
*        This routine gets learn2All mode status.When more than one Marvell device 
*		 is used to form a single 'switch', it  may be desirable for all devices
*        in the 'switch' to learn any address this device learns. When this bit
*        is set to a one all other devices in the 'switch' learn the same addresses
*        this device learns. When this bit is cleared to a zero, only the devices
*        that actually receive frames will learn from those frames. This mode 
*        typically supports more active MAC addresses at one time as each device
*        in the switch does not need to learn addresses it may nerver use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - MSD_TRUE if Learn2All is enabled, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None.
*
*
*******************************************************************************/
MSD_STATUS msdFdbLearnOnAllDevicesGet
(
IN  MSD_U8  devNum,
OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetLearn2All != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetLearn2All(dev, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbLearnOnAllDevicesSet
*
* DESCRIPTION:
*        This routine typically enable or disable learn2All mode.When more than one 
*		 Marvell device is used to form a single 'switch', it  may be desirable for 
*        all devices in the 'switch' to learn any address this device learns. When 
*        this bit is set to a one all other devices in the 'switch' learn the same 
*        addresses this device learns. When this bit is cleared to a zero, only 
*        the devices that actually receive frames will learn from those frames.This 
*        mode typically supports more active MAC addresses at one time as each 
*        device in the switch does not need to learn addresses it may nerver use.
*
* INPUTS:
*        mode - MSD_TRUE to set Learn2All, MSD_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS msdFdbLearnOnAllDevicesSet
(
IN  MSD_U8  devNum,
IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbSetLearn2All != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbSetLearn2All(dev, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbPortLearnLimitSet
*
* DESCRIPTION:
*        This routine sets auto learning limit for specified port of a specified device. 
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
*        This feature will not work when this port is configured as a LAG port or if
*        the port's PAV has more then 1 bit set to a '1'.
*
*        The following care is needed when enabling this feature:
*            1) disable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*            5) Note: LockedPort needs to be cleared to zero when hardware address learn
*               limiting is enabled on the port, so auto learning wil occure before the 
*               limit is reached.
*
* INPUTS:
*        portNum - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS msdFdbPortLearnLimitSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_U32  limit
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit(dev, portNum, limit);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbPortLearnLimitGet
*
* DESCRIPTION:
*        This routine gets auto learning limit for specified port of a specified device. 
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
*        This feature will not work when this port is configured as a LAG port or if
*        the port's PAV has more then 1 bit set to a '1'.
*
*        The following care is needed when enabling this feature:
*            1) disable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports        

*
* INPUTS:
*        portNum - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS msdFdbPortLearnLimitGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
OUT MSD_U32  *limit
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit(dev, portNum, limit);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbPortLearnCountGet
*
* DESCRIPTION:
*        Read the current number of active unicast MAC addresses associated with
*        the given port. This counter (LearnCnt) is held at zero if learn limit
*        (msdFdbPortLearnLimitSet API) is set to zero.
*
* INPUTS:
*       portNum  - logical port number
*
* OUTPUTS:
*       count - current auto learning count
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS msdFdbPortLearnCountGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
OUT MSD_U32  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount(dev, portNum, count);
    else
        return MSD_NOT_SUPPORTED;
}


/*******************************************************************************
* msdFdbEntryCountGet
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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdFdbEntryCountGet
(
IN  MSD_U8  devNum,
OUT MSD_U32  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetEntryCount != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetEntryCount(dev, count);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbEntryCountPerFidGet
*
* DESCRIPTION:
*       Counts all entries in the specified fid in Address Translation Unit.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdFdbEntryCountPerFidGet
(
IN  MSD_U8  devNum,
IN  MSD_U32  fid,
OUT MSD_U32  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid(dev, fid, count);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbEntryCountNonStaticGet
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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdFdbEntryCountNonStaticGet
(
IN  MSD_U8  devNum,
OUT MSD_U32  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount(dev, count);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbEntryCountNonStaticPerFidGet
*
* DESCRIPTION:
*       Counts all non-static entries in the specified fid in the Address Translation Unit.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdFdbEntryCountNonStaticPerFidGet
(
IN  MSD_U8  devNum,
IN  MSD_U32  fid,
OUT MSD_U32  *count
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid(dev, fid, count);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbDump
*
* DESCRIPTION:
*       Finds all MAC address in the specified fid and print it out.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdFdbDump
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  fid
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.ATUObj.gfdbDump != NULL)
        return dev->SwitchDevObj.ATUObj.gfdbDump(dev, fid);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortLockedGet
*
* DESCRIPTION:
*       This routine gets Locked Port configuration
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortLockedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortLockedSet
*
* DESCRIPTION:
*       This routine enable or disable port locked mode. When set to zero,
*       normal address learning will occur. when set to one, CPU directed
*       learning for 802.1x MAC authentication is enabled on this port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortLockedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortHoldAt1Get
*
* DESCRIPTION:
*       This routine gets HoldAt1 status for specific port on a specific device.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortHoldAt1Get
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortHoldAt1Set
*
* DESCRIPTION:
*       This routine enable or disable HoldAt1 mode for specific port on a 
*       specific device. ATU entries associated with this port will age 
*       down to an Entry State of 0x1, but will not go to 0
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortHoldAt1Set
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortIntOnAgeOutGet
*
* DESCRIPTION:
*       This routine gets Interrupt enable status on Age Out. When aging is enabled,
*       all non-static address entries in the ATU's address database are periodically 
*       aged. When this feature is set to MSD_TRUE and an entry associated with this
*       port is agedout, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortIntOnAgeOutGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortIntOnAgeOutSet
*
* DESCRIPTION:
*       This routine enable or disable Interrupt on Age Out. When aging is enabled,
*       all non-static address entries in the ATU's address database are periodically 
*       aged. When this feature is set to MSD_TRUE and an entry associated with this 
*       port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortIntOnAgeOutSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortRefreshLockedGet
*
* DESCRIPTION:
*       This routine gets auto refresh known addresses when port is locked. Already 
*       known addresses will be auto refreshed when this feature is enabled. When 
*       this feature is disabled, auto refreshing will not occur on locked ports.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortRefreshLockedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortRefreshLockedSet
*
* DESCRIPTION:
*       This routine enable or disable auto refresh known addresses when port is
*       locked. Already known addresses will be auto refreshed when this feature 
*       is enabled. When this feature is disabled, auto refreshing will not occur
*		on Locked ports.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortRefreshLockedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortIgnoreWrongDataGet
*
* DESCRIPTION:
*       this routine gets ignore wrong data mode of specified port on specified
*       device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortIgnoreWrongDataGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdPortIgnoreWrongDataSet
*
* DESCRIPTION:
*       This routine enable or disable ignore wrong data mode of specified port 
*       on specified device. Generate/mask ATU member violation interrupt when 
*       the frame's SA address is found in database but not assigned to the port. 
*       It is valid only when the entry state is static or the port is locked.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortIgnoreWrongDataSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData != NULL)
        return dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData(dev, portNum, en);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
* msdFdbEntryNextGet
*
* DESCRIPTION:
*       Get Next valid atu entry starting from the specified Mac Addr in a
*		particular ATU database
*
* INPUTS:
*       devNum  - physical devie number
*		macAddr - the Mac Address to start the search.
*       fid - ATU MAC Address Database Number.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*       MSD_NO_SUCH - no more entries.
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdFdbEntryNextGet
(
	IN  MSD_U8  devNum,
	IN  MSD_ETHERADDR *macAddr,
	IN  MSD_U32  fid,
	OUT MSD_ATU_ENTRY  *entry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	entry->fid = (MSD_U16)fid;
	msdMemCpy(&entry->macAddr, macAddr, MSD_ETHERNET_HEADER_SIZE);

	if (dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext != NULL)
		return dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext(dev, entry);
	else
		return MSD_NOT_SUPPORTED;
}
