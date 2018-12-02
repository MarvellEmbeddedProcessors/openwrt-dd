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
* msdTrunk.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Trunk functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdTrunk_h
#define __msdTrunk_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdTrunkMaskTableGet
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       devNum  - physical devie number
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    OUT MSD_U32  *trunkMask
);

/*******************************************************************************
* msdTrunkMaskTableSet
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       devNum  - physical devie number
*       maskNum - Mask Number (0~7).
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    IN  MSD_U32  trunkMask
);

/*******************************************************************************
* msdTrunkHashTrunkGet
*
* DESCRIPTION:
*       This routine gets enable status for hash DA & SA for TrunkMask selection.
*       Trunk load balancing is accomplished by using the frame's DA and SA fields
*       to access one of eight Trunk Masks.
*		When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdTrunkHashTrunkSet
*
* DESCRIPTION:
*       This routine enable or disable hash DA & SA for TrunkMask selection. Trunk
*       load balancing is accomplished by using the frame's DA and SA fields to
*		access one of eight Trunk Masks.
*       When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical devie number
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkSet
(
    IN  MSD_U8  devNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdTrunkRoutingGet
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical devie number
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    OUT MSD_U32  *trunkRoute
);

/*******************************************************************************
* msdTrunkRoutingSet
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical devie number
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    IN  MSD_U32  trunkRoute
);

/*******************************************************************************
* msdPortLAGPortGet
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*
* INPUTS:
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a LAG,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
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
MSD_STATUS msdPortLAGPortGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en,
    OUT MSD_U32  *LAGId
);

/*******************************************************************************
* msdPortLAGPortSet
*
* DESCRIPTION:
*        This function enables/disables trunk bit and sets the LAG ID.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       en - MSD_TRUE to make the port be a member of a LAG with the given LAG ID.
*             MSD_FALSE, otherwise.
*       LAGId - valid ID is 0 ~ 31
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
*		 None.
*
*******************************************************************************/
MSD_STATUS msdPortLAGPortSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en,
    IN  MSD_U32  LAGId
);

#ifdef __cplusplus
}
#endif

#endif /* __msdTrunk_h */
