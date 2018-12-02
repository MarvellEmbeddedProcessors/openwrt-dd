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
* msdPortCtrl.h
*
* DESCRIPTION:
*       API definitions for Switch Port Functions and Registers 
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdPortCtrl_h
#define __msdPortCtrl_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* msdPortDefaultVlanIdGet
*
* DESCRIPTION:
*       This routine gets the port default vlan id.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       vlanId  - the port vlan id.
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
MSD_STATUS msdPortDefaultVlanIdGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *vlanId
);

/*******************************************************************************
* msdPortDefaultVlanIdSet
*
* DESCRIPTION:
*       This routine sets the port default vlan id.
*
* INPUTS:
*       portNum - logical port number to set.
*       vlanId  - the port vlan id.
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
MSD_STATUS msdPortDefaultVlanIdSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  vlanId
);

/*******************************************************************************
* msdPort8021qModeGet
*
* DESCRIPTION:
*       This routine gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       mode  - 802.1q mode for this port
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
MSD_STATUS msdPort8021qModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_8021Q_MODE  *mode
);

/********************************************************************
* gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the IEEE 802.1q mode for this port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       mode  - 802.1q mode for this port
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
MSD_STATUS msdPort8021qModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_8021Q_MODE  mode
);

/*******************************************************************************
* msdPortDiscardTaggedGet
*
* DESCRIPTION:
*       This routine gets DiscardTagged bit for the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE if DiscardTagged bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardTaggedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortDiscardTaggedSet
*
* DESCRIPTION:
*       This routine sets DiscardTagged bit for the given port. When this bit is
*       set to a one, all non-MGMT frames that are processed as Tagged will be
*       discarded as they enter this switch port. Priority only tagged frames
*		(with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE to discard tagged frame, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardTaggedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortDiscardUntaggedGet
*
* DESCRIPTION:
*       This routine gets DiscardUntagged bit for the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       en  - MSD_TRUE if DiscardUntagged bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardUntaggedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortDiscardUntaggedSet
*
* DESCRIPTION:
*       This routine sets DiscardUntagged bit for the given port. When this bit
*       is set to a one, all non-MGMT frames that are processed as Untagged will
*       be discarded as they enter this switch port. Priority only tagged frames
*		(with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       en - MSD_TRUE to discard untagged frame, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardUntaggedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortUnicastFloodBlockGet
*
* DESCRIPTION:
*       This routine gets egress flooding mode for unitcast frame of a switch
*       port.
*		When this mode is MSD_TRUE, unicast frame with unknown DA addresses
*       will not egress out this port.
*       When this mode is MSD_FALSE, normal switch operation occurs.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE to do not egress out the unknown DA unicast frames
*            MSD_FALSE for normal operation
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
MSD_STATUS msdPortUnicastFloodBlockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortUnicastFloodBlockSet
*
* DESCRIPTION:
*       This routine sets egress flooding mode for unknown unitcast frame of a switch
*       port. When this mode is set to MSD_TRUE, unicast frame with unknown DA addresses
*       will not egress out this port.
*       When this mode is set to MSD_FALSE, normal switch operation occurs.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE to do not egress out the unknown DA unicast frames
*            MSD_FALSE for normal operation
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
MSD_STATUS msdPortUnicastFloodBlockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortMulticastFloodBlockGet
*
* DESCRIPTION:
*       This routine Sets Egress Flooding mode of unknown multicast of a switch port.
*       When this mode is set to MSD_TRUE, multicast frames with unknown DA
*       addresses will not egress out this port.
*       when this mode is set to MSD_FALSE, normal switch operation will occurs and
*       multicast frames with unknown DA addresses are allowed to egress out this
*       port (assuming the VLAN settings allow the frame to egress this port too).
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_TRUE  to do not egress out the unknown DA multicast frames and
*               MSD_FALSE for normal switch operation
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
MSD_STATUS msdPortMulticastFloodBlockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortMulticastFloodBlockSet
*
* DESCRIPTION:
*       This routine sets egress flooding mode of unknown multicast of a switch port.
*       When this mode is set to MSD_TRUE, multicast frames with unknown DA
*       addresses will not egress out this port.
*       when this mode is set to MSD_FALSE, normal switch operation will occurs and
*       multicast frames with unknown DA addresses are allowed to egress out this
*       port (assuming the VLAN settings allow the frame to egress this port too).
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       mode - MSD_TRUE  to do not egress out the unknown DA multicast frames and
*               MSD_FALSE for normal switch operation
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
MSD_STATUS msdPortMulticastFloodBlockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortDiscardBcastModeGet
*
* DESCRIPTION:
*       This routine gets the discard broadcast mode. If the mode is enabled,
*       all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
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
MSD_STATUS msdPortDiscardBcastModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortDiscardBcastModeSet
*
* DESCRIPTION:
*       This routine sets the discard broadcast mode.
*       If the mode is enabled, all the broadcast frames to the given port will
*       be discarded.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortDiscardBcastModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortIGMPSnoopEnableGet
*
* DESCRIPTION:
*       This routine gets the enable status for IPV4 IGMP packets snooping.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
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
MSD_STATUS msdPortIGMPSnoopEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortIGMPSnoopEnableSet
*
* DESCRIPTION:
*       This routine enable or disable IPV4 IGMP packets snooping.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortIGMPSnoopEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortHeaderModeGet
*
* DESCRIPTION:
*       This routine gets the ingress and egress header mode of a switch port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
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
MSD_STATUS msdPortHeaderModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortHeaderModeSet
*
* DESCRIPTION:
*       This routine sets the ingress and egress header mode of a switch
*       port. To be used only for port connected to cpu mainly used for layer3
*       32bit alignment.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortHeaderModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortDropOnLockGet
*
* DESCRIPTION:
*       This routine gets the DropOnLock mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
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
MSD_STATUS msdPortDropOnLockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortDropOnLockSet
*
* DESCRIPTION:
*       This routine sets the Drop on Lock mode of specified port on specified device.
*       When set to one, Ingress frames will be discarded if their SA field is not in
*       the ATU's address database of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortDropOnLockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortMtuSet
*
* DESCRIPTION:
*       This routine sets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       portNum - the logical port number
*       mode - PERIDOT_MSD_MTU_SIZE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortMtuSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_MTU_SIZE  mode
);

/*******************************************************************************
* msdPortMtuGet
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       devNum  - physical devie number
*       port  - the logical port number.
*
* OUTPUTS:
*       size - MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS msdPortMtuGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_MTU_SIZE  *size
);

/*******************************************************************************
* msdPortLearnEnableGet
*
* DESCRIPTION:
*       This routine gets Learn disable setup of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       en - MSD_TRUE: Learning enabled on the given port ingress frames,
*                 MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortLearnEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortLearnEnableSet
*
* DESCRIPTION:
*       This routine enables or disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       en - MSD_TRUE: Learning enabled on the given port ingress frames,
*               MSD_FALSE otherwise
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
MSD_STATUS msdPortLearnEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortVTUPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets VTU priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortVTUPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
);

/*******************************************************************************
* msdPortVTUPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets VTU priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortVTUPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
);

/*******************************************************************************
* msdPortSAPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets SA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortSAPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
);

/*******************************************************************************
* msdPortSAPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets SA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortSAPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
);

/*******************************************************************************
* msdPortDAPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets DA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortDAPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
);

/*******************************************************************************
* msdPortDAPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets DA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortDAPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
);

/*******************************************************************************
* msdPortMessagePortGet
*
* DESCRIPTION:
*       This routine gets message port mode for specific port on a specified device.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortMessagePortGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortMessagePortSet
*
* DESCRIPTION:
*       This routine enable or disable message port for specific port on a specific device.
*       Learning and learn refresh message frames will be sent out to this port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
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
MSD_STATUS msdPortMessagePortSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortEtherTypeGet
*
* DESCRIPTION:
*       This routine gets the port's special Ether Type. This Ether Type is used 
*       for Policy and FrameMode of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       eType - Ethernet type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortEtherTypeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *eType
);

/*******************************************************************************
* msdPortEtherTypeSet
*
* DESCRIPTION:
*       This routine sets the port's special Ether Type. This Ether Type is used 
*       for Policy and FrameMode of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       eType - Ethernet type
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
MSD_STATUS msdPortEtherTypeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  eType
);

/*******************************************************************************
* msdPortAllowVidZeroGet
*
* DESCRIPTION:
*       This routine gets allow VID of Zero mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: allow VID of zero, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortAllowVidZeroGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortAllowVidZeroSet
*
* DESCRIPTION:
*       This routine enable or disable allow VID of Zero of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE: allow VID of zero, MSD_FALSE: otherwise
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
MSD_STATUS msdPortAllowVidZeroSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortEgressRateGet
*
* DESCRIPTION:
*       This routine gets the port's egress rate and count mode.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       mode - MSD_ELIMIT_MODE
*       rate  - egress rate
*                    rate in kbps that should used with the MSD_ELIMIT_MODE of
*                     MSD_ELIMIT_LAYER1, MSD_ELIMIT_LAYER2, or MSD_ELIMIT_LAYER3
*                     64kbps ~ 1Mbps    : increments of 64kbps,
*                     1Mbps ~ 100Mbps   : increments of 1Mbps, 
*                     100Mbps ~ 1000Mbps: increments of 10Mbps
*                     1Gbps ~ 5Gbps: increments of 100Mbps
*                     Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                    rate in frame per second that should used with MSD_ELIMIT_MODE
*                     of MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortEgressRateGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_ELIMIT_MODE  *mode,
    OUT MSD_U32  *rate
);

/*******************************************************************************
* msdPortEgressRateSet
*
* DESCRIPTION:
*       This routine sets the port's egress rate.
*
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       mode - MSD_ELIMIT_MODE
*       rate  - egress rate
*                    rate in kbps that should used with the MSD_ELIMIT_MODE of
*                     MSD_ELIMIT_LAYER1, MSD_ELIMIT_LAYER2, or MSD_ELIMIT_LAYER3
*                     64kbps ~ 1Mbps    : increments of 64kbps,
*                     1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                     100Mbps ~ 1000Mbps: increments of 10Mbps
*                     1Gbps ~ 5Gbps: increments of 100Mbps
*                     Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                    rate in frame per second that should used with MSD_ELIMIT_MODE
*                     of MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
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
*		None
*
*******************************************************************************/
MSD_STATUS msdPortEgressRateSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_ELIMIT_MODE  mode,
    IN  MSD_U32  rate
);

/*******************************************************************************
* msdPortFlowControlEnableGet
*
* DESCRIPTION:
*       This routine gets switch port flow control enable/disable status and return
*       flow control mode
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - enable/disable the flow control
*       mode - flow control mode
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
MSD_STATUS msdPortFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en,
    OUT MSD_PORT_FC_MODE  *mode
);

/*******************************************************************************
* msdPortFlowControlEnableSet
*
* DESCRIPTION:
*       This routine enable/disable port flow control and set flow control mode
*          mode - PORT_FC_TX_RX_ENABLED,
*                 PORT_RX_ONLY,
*                 PORT_TX_ONLY,
*                 PORT_PFC_ENABLED
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*		en - enable/disable the flow control
*       mode - flow control mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS msdPortFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en,
    IN  MSD_PORT_FC_MODE  mode
);

/*******************************************************************************
* msdPortEgressMonitorSourceEnableGet
*
* DESCRIPTION:
*       This routine gets EgressMonitorSource bit for the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE if EgressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortEgressMonitorSourceEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortEgressMonitorSourceEnableSet
*
* DESCRIPTION:
*       This routine sets EgressMonitorSource bit for the given port. When this bit
*       is cleared to a zero, normal network switching occurs. When this bit is set
*       to a one, any frame that egresses out this port will also be sent to the
*		EgressMonitorDest Port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*       en - MSD_TRUE to set EgressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS msdPortEgressMonitorSourceEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortIngressMonitorSourceEnableGet
*
* DESCRIPTION:
*       This routine gets IngressMonitorSource bit for the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en  - MSD_TRUE if IngressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortIngressMonitorSourceEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortIngressMonitorSourceEnableSet
*
* DESCRIPTION:
*       This routine Sets IngressMonitorSource bit for the given port. When this
*       be is cleared to a zero, normal network switching occurs. When this bit
*       is set to a one, any frame that ingresses in this port will also be sent
*		to the IngressMonitorDest Port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*       en - MSD_TRUE to set IngressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS msdPortIngressMonitorSourceEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortForceDuplexModeGet
*
* DESCRIPTION:
*       This routine retrives the port force duplex mode.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCE_DUPLEX_MODE
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
MSD_STATUS msdPortForceDuplexModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCE_DUPLEX_MODE  *mode
);

/*******************************************************************************
* msdPortForceDuplexModeSet
*
* DESCRIPTION:
*       This routine sets the port force duplex mode.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*       mode - MSD_PORT_FORCE_DUPLEX_MODE
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortForceDuplexModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCE_DUPLEX_MODE  mode
);

/*******************************************************************************
* msdPortDuplexStatusGet
*
* DESCRIPTION:
*       This routine retrives the port duplex status.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for Full-duplex  or MSD_FALSE for Half-duplex
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
MSD_STATUS msdPortDuplexStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *mode
);

/*******************************************************************************
* msdPortForceLinkModeGet
*
* DESCRIPTION:
*       This routine retrives the force link state.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCE_LINK_MODE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortForceLinkModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCE_LINK_MODE  *mode
);

/*******************************************************************************
* msdPortForceLinkModeSet
*
* DESCRIPTION:
*       This routine sets the force link state.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*       mode - MSD_PORT_FORCE_LINK_MODE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortForceLinkModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCE_LINK_MODE  mode
);

/*******************************************************************************
* msdPortLinkStatusGet
*
* DESCRIPTION:
*       This routine retrives the link state.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - the logical port number.
*
* OUTPUTS:
*       state - MSD_TRUE for Up  or MSD_FALSE for Down
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdPortLinkStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *state
);

/*******************************************************************************
* msdPortForceSpeedModeGet
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       mode - PERIDOT_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS msdPortForceSpeedModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCED_SPEED_MODE  *mode
);

/*******************************************************************************
* msdPortForceSpeedModeSet
*
* DESCRIPTION:
*       This routine sets switch MAC force speed.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*       mode - PERIDOT_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortForceSpeedModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCED_SPEED_MODE  mode
);

/*******************************************************************************
* msdPortSpeedStatusGet
*
* DESCRIPTION:
*       This routine retrives the port MAC speed.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number to set.
*
* OUTPUTS:
*       speed - MSD_PORT_SPEED type.
*                (PORT_SPEED_10M,PORT_SPEED_100M, PORT_SPEED_1000M, PORT_SPEED_10G
*                etc.)
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
MSD_STATUS msdPortSpeedStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_SPEED  *speed
);

/*******************************************************************************
* msdPortCutThroughEnableGet
*
* DESCRIPTION:
*       This routine gets port Cut Through configuration of specified port on
*       specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       cutThru - MSD_CUT_THROUGH
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
MSD_STATUS msdPortCutThroughEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_CUT_THROUGH  *cutThru
);

/*******************************************************************************
* msdPortCutThroughEnableSet
*
* DESCRIPTION:
*       This routine sets port Cut Through configuration of specified port on
*       specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       cutThru - MSD_CUT_THROUGH
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
MSD_STATUS msdPortCutThroughEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_CUT_THROUGH  *cutThru
);

/*******************************************************************************
* msdPortIgnoreFcsGet
*
* DESCRIPTION:
*       This routine gets force good FCS enable status. When force good FCS
*       bit is set to a one, the last four bytes of frames received on this port
*       are overwritten with a good CRC and the frames will be accepted by switch.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to set force good fcs, MSD_FALSE otherwise
*
* OUTPUTS:
*       en - MSD_TRUE if force good fcs is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortIgnoreFcsGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortIgnoreFcsSet
*
* DESCRIPTION:
*       This routine enable or disable force good FCS. When force good FCS
*       bit is set to a one, the last four bytes of frames received on this port
*       are overwritten with a good CRC and the frames will be accepted by switch.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to set force good fcs, MSD_FALSE otherwise
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
MSD_STATUS msdPortIgnoreFcsSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortAllowBadFcsGet
*
* DESCRIPTION:
*       This routine gets the configuration for Allow receiving frames on this
*       port with a bad FCS of specified port on specified deviceand the frames
*       will be accepted by switch.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if allow bad fcs is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortAllowBadFcsGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortAllowBadFcsSet
*
* DESCRIPTION:
*       This routine sets allow bad fcs to Allow receiving frames on this
*       port with a bad FCS of specified port on specified deviceand the frames
*       will be accepted by switch.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to set allow bad fcs, MSD_FALSE otherwise
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
MSD_STATUS msdPortAllowBadFcsSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortPauseLimitOutGet
*
* DESCRIPTION:
*       This routine gets pause refresh frames transmit limit for specific port
*       on a specific device.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitOutGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *limit
);

/*******************************************************************************
* msdPortPauseLimitOutSet
*
* DESCRIPTION:
*       This routine sets pause refresh frames transmit limit for specific port
*       on a specific device.
*       Set 0 for transmitting continuous pause frame refreshes.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitOutSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  limit
);

/*******************************************************************************
* msdPortPauseLimitInGet
*
* DESCRIPTION:
*       This routine gets pause refresh frames receive limit for specific
*       port on a specific device.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitInGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *limit
);

/*******************************************************************************
* msdPortPauseLimitInSet
*
* DESCRIPTION:
*       This routine sets pause refresh frames receive limit for specific port
*       on a specific device.
*       Set 0 forreceiving continuous pause frame refreshes
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitInSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  limit
);

/*******************************************************************************
* msdPortQueueToPauseGet
*
* DESCRIPTION:
*       This routine gets QueueToPause value that is used to determine which
*       queues are paused when receiving a pause frame.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       queues - output queues that are paused off
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
MSD_STATUS msdPortQueueToPauseGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U8  *queues
);

/*******************************************************************************
* msdPortQueueToPauseSet
*
* DESCRIPTION:
*       This routine sets QueueToPause value that is used to determine which queues
*       are paused when receiving a pause frame. When IEEE 802.3x flow control is
*       enabled on this port and a properly formed Pause frame is received on this
*		port, this QtoPause value is used to determine which output queues on this
*		port are Paused off or not.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       queues - output queues that are paused off
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
MSD_STATUS msdPortQueueToPauseSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  queues
);

/*******************************************************************************
* msdPortRxPriorityFlowControlEnableGet
*
* DESCRIPTION:
*       This routine gets the status if swich port pay attention for the
*		specified PFC priority information or not when receiving a PFC pause frame.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       en - MSD_TRUE when enable, MSD_FALSE when disable
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
MSD_STATUS msdPortRxPriorityFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortRxPriorityFlowControlEnableSet
*
* DESCRIPTION:
*       This routine decides if swich port pay attention for the specified PFC
*		priority information when receiving a PFC pause frame.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       en - MSD_TRUE to enable, MSD_FALSE to disable
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
MSD_STATUS msdPortRxPriorityFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortRxPriorityFlowControlToQueueGet
*
* DESCRIPTION:
*       This routine Get receive priority flow control frame's Priority X(0~7)
*       to Egress Queue mapping of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       queue - priority queue
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
MSD_STATUS msdPortRxPriorityFlowControlToQueueGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_U8  *queue
);

/*******************************************************************************
* msdPortRxPriorityFlowControlToQueueSet
*
* DESCRIPTION:
*       This routine Set receive priority flow control frame's Priority X(0~7)
*       to Egress Queue mapping of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       queue - priority queue
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
MSD_STATUS msdPortRxPriorityFlowControlToQueueSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_U8  queue
);

/*******************************************************************************
* msdPortTxPriorityFlowControlEnableGet
*
* DESCRIPTION:
*       This routine Get transmit priority flow control enable of specified
*       port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       en - MSD_TRUE when enable, MSD_FALSE when disable
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
MSD_STATUS msdPortTxPriorityFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortTxPriorityFlowControlEnableSet
*
* DESCRIPTION:
*       This routine Set transmit priority flow control enable of specified
*       port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       en - MSD_TRUE to enable, MSD_FALSE to disable
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
MSD_STATUS msdPortTxPriorityFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortOutQueueSizeGet
*
* DESCRIPTION:
*       This routine gets egress queue size counter value.This counter reflects
*       the current number of Egress buffers switched to this port and each
*       queue. The que=0xf is the total number of buffers across all four
*       priority queues.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       queueNum - the queue number
*
* OUTPUTS:
*       count - queue size counter value
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
MSD_STATUS msdPortOutQueueSizeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  queueNum,
    OUT MSD_U16  *count
);

/*******************************************************************************
* msdPortMapDAGet
*
* DESCRIPTION:
*       This routine gets Map DA mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if Map DA is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortMapDAGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortMapDASet
*
* DESCRIPTION:
*       This routine sets map DA mode of specified port on specified device. When
*       set to zero, the frame will be sent out the ports defined by Egress Floods
*       subsequently modified by any enabled filetering. Otherwise, normal switch
*       operation will occur
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       en - MSD_TRUE to set Map DA bit, MSD_FALSE otherwise
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
MSD_STATUS msdPortMapDASet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortARPMirrorGet
*
* DESCRIPTION:
*       this routine gets Arp Mirror Enable bit of specified port on specified
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
MSD_STATUS msdPortARPMirrorGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
);

/*******************************************************************************
* msdPortARPMirrorSet
*
* DESCRIPTION:
*       this routine sets Arp Mirror Enable bit of specified port on specified
*       device. when set to one non-filtered Tagged or Untagged Frames that
*       ingress this port that have the Broadcast Destination Address an Ethertype
*       of 0x0806 are mirrored to the CPUDest port.
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
MSD_STATUS msdPortARPMirrorSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
);

/*******************************************************************************
* msdPortVlanPortsGet
*
* DESCRIPTION:
*       this routine gets port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       memPorts - vlan ports
*       memPortsLen - number of vlan ports
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
MSD_STATUS msdPortVlanPortsGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_LPORT  *memPorts,
    OUT MSD_U8  *memPortsLen
);

/*******************************************************************************
* msdPortVlanPortsSet
*
* DESCRIPTION:
*       this routine sets port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       memPorts - vlan ports to set
*       memPortsLen - number of vlan ports
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
MSD_STATUS msdPortVlanPortsSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_LPORT  *memPorts,
    IN  MSD_U8  memPortsLen
);

/*******************************************************************************
* msdPortFrameModeGet
*
* DESCRIPTION:
*       this routine gets frame mode of a specified port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_FRAME_MODE
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
MSD_STATUS msdPortFrameModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_FRAME_MODE  *mode
);

/*******************************************************************************
* msdPortFrameModeSet
*
* DESCRIPTION:
*       This routine sets frame mode of a specified port.Frmae Mode is used to
*       define the expected Ingress and the generated Egress tagging frame
*		format for this port as follows:
*            MSD_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            MSD_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for
*                Chip-to-Chip and Chip-to-CPU connections.
*            MSD_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port
*                (see gprtSetPortEType/gprtGetPortEType API).
*            MSD_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to
*                be used on ports that connect to a CPU.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       mode - MSD_FRAME_MODE
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
MSD_STATUS msdPortFrameModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_FRAME_MODE  mode
);

/*******************************************************************************
* msdPortStateGet
*
* DESCRIPTION:
*       This routine gets the port state.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       mode - MSD_PORT_STP_STATE
*               MSD_PORT_DISABLE    - port is disabled.
*               MSD_PORT_BLOCKING   - port is in blocking/listening state.
*               MSD_PORT_LEARNING   - port is in learning state.
*               MSD_PORT_FORWARDING - port is in forwarding state.
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
MSD_STATUS msdPortStateGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_STP_STATE  *mode
);

/*******************************************************************************
* msdPortStateSet
*
* DESCRIPTION:
*       This routine sets the port state.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       mode - MSD_PORT_STP_STATE
*               MSD_PORT_DISABLE    - port is disabled.
*               MSD_PORT_BLOCKING   - port is in blocking/listening state.
*               MSD_PORT_LEARNING   - port is in learning state.
*               MSD_PORT_FORWARDING - port is in forwarding state.
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
MSD_STATUS msdPortStateSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_STP_STATE  mode
);

/*******************************************************************************
* msdPortRegDump
*
* DESCRIPTION:
*       This routine sets read 32 port registers for a specified port and print
*		them out
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
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
MSD_STATUS msdPortRegDump
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum
);
#ifdef __cplusplus
}
#endif

#endif /* __msdPortCtrl_h */
