#include <msdCopyright.h>
/********************************************************************************
* msdOs.c
*
* DESCRIPTION:
*       Semaphore related routines
*
* DEPENDENCIES:
*       OS Dependent.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdSem.h>


/*******************************************************************************
* msdSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*        state - beginning state of the semaphore, either MSD_SEM_EMPTY or MSD_SEM_FULL
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_SEM if success. Otherwise, NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_SEM msdSemCreate
(
	IN MSD_U8    devNum,
    IN MSD_SEM_BEGIN_STATE state
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
		return 1;

    if(dev->semCreate)
        return dev->semCreate(state);

    return 1; /* should return any value other than 0 to let it keep going */
}

/*******************************************************************************
* msdSemDelete
*
* DESCRIPTION:
*       Delete semaphore.
*
* INPUTS:
*       smid - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemDelete
(
    IN MSD_U8    devNum,
    IN MSD_SEM smid
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
		return MSD_FAIL;

    if((dev->semDelete) && (smid))
        return dev->semDelete(smid);

    return MSD_OK;
}


/*******************************************************************************
* msdSemTake
*
* DESCRIPTION:
*       Wait for semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemTake
(
    IN MSD_U8  devNum,
    IN MSD_SEM smid, 
    IN MSD_U32 timeOut
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
		return MSD_FAIL;

    if(dev->semTake)
        return dev->semTake(smid, timeOut);

    return MSD_OK;

}

/*******************************************************************************
* msdSemGive
*
* DESCRIPTION:
*       release the semaphore which was taken previously.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemGive
(
    IN MSD_U8		devNum,
    IN MSD_SEM       smid
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
		return MSD_FAIL;

    if(dev->semGive)
        return dev->semGive(smid);

    return MSD_OK;
}

