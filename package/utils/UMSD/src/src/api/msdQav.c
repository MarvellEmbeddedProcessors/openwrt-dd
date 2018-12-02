#include <msdCopyright.h>

/********************************************************************************
* msdQav.c
*
* DESCRIPTION:
*       API definitions for Qav 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdQav.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/******************************************************************************
* msdQavPortQpriXRateSet
*
* DESCRIPTION:
*        This routine sets queue rate for specified priority queue on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum - the logical port number
*        queue - queue priority
*        rate  - priority queue rate
*                value of 0 will disable this queue's Qav rate shaper
*                rate in kbps and increments of 64kbps, a maximum supported rate is 1000Mbps
*                therefore, the valid value are:
*                64, 128, 192, 256, 320, 384,..., 1000000
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXRateSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    IN  MSD_U32     rate
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	if (dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate != NULL)
        return dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate(dev, portNum, queue, rate);
	else
		return MSD_NOT_SUPPORTED;
}
/******************************************************************************
* msdQavPortQpriXRateGet
*
* DESCRIPTION:
*        This routine gets priority queue rate on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum - the logical port number
*        queue   - queue priority
*
* OUTPUTS:
*        rate  - priority queue rate
*                value of 0 will disable this queue's Qav rate shaper
*                rate in kbps and increments of 64kbps, a maximum supported rate is 1000Mbps
*                therefore, the valid value are:
*                64, 128, 192, 256, 320, 384,..., 1000000
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXRateGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    OUT MSD_U32     *rate
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate != NULL)
        return dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate(dev, portNum, queue, rate);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdQavPortQpriXHiLimitSet
*
* DESCRIPTION:
*        This routine sets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXHiLimitSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    IN  MSD_U16     hiLimit
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit != NULL)
        return dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit(dev, portNum, queue, hiLimit);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdQavPortQpriXHiLimitGet
*
* DESCRIPTION:
*        This routine gets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXHiLimitGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    OUT MSD_U16     *hiLimit
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    if (dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit != NULL)
        return dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit(dev, portNum, queue, hiLimit);
    else
        return MSD_NOT_SUPPORTED;
}