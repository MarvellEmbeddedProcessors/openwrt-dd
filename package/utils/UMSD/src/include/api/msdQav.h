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
* msdQav.h
*
* DESCRIPTION:
*       API definitions for qav feature.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdQav_h
#define __msdQav_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

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
);

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
);

/******************************************************************************
* msdQavPortQpriXHiLimitSet
*
* DESCRIPTION:
*        This routine sets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - queue priority
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
);

/******************************************************************************
* msdQavPortQpriXHiLimitGet
*
* DESCRIPTION:
*        This routine gets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - queue priority
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
);

#ifdef __cplusplus
}
#endif

#endif /* __msdQav_h */
