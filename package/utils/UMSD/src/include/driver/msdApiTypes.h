#include <msdCopyright.h>
/*
*				Copyright 2017, MARVELL SEMICONDUCTOR, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED
* OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
*/

/*
* FILENAME:    $Workfile: mv_types.h $
* REVISION:    $Revision: 3 $
* LAST UPDATE: $Modtime: 12/24/02 5:37p $
*
* DESCRIPTION:
*     This file defines common data types used on Host and NetGX sides.
*/

#ifndef __msdApiTypes_h
#define __msdApiTypes_h
#ifdef __cplusplus
extern "C" {
#endif


/* general */

#undef IN
#define IN
#undef OUT
#define OUT
#undef INOUT
#define INOUT


#ifndef NULL
#define NULL ((void*)0)
#endif

typedef void  MSD_VOID;
typedef char  MSD_8;
typedef short MSD_16;
typedef long  MSD_32;

typedef unsigned char  MSD_U8;
typedef unsigned short MSD_U16;
typedef unsigned long  MSD_U32;
typedef unsigned int   MSD_UINT;

typedef double MSD_DOUBLE;

typedef enum {
	MSD_FALSE = 0,
	MSD_TRUE = 1
} MSD_BOOL;

#define		MSD_ETHERNET_HEADER_SIZE        (6)
#define		MSD_MAX_SWITCH_PORTS				11
typedef struct
{
	MSD_U8       arEther[MSD_ETHERNET_HEADER_SIZE];
}MSD_ETHERADDR;


/* status / error codes */
typedef int MSD_STATUS;

#define MSD_OK              (0x00)   /* Operation succeeded                   */
#define MSD_FAIL            (0x01)   /* Operation failed                    */
#define MSD_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#define MSD_NO_SUCH         (0x0D)   /* No such item                    */
#define MSD_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#define MSD_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#define MSD_BAD_CPU_PORT    (0x20)   /* Input CPU Port is not valid physical port number */

typedef MSD_U32 MSD_LPORT;
typedef MSD_U32 MSD_ETYPE;
typedef MSD_U32 MSD_SEM;

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
#define MSD_BIT(n)                  ( 1<<(n) )
#define MSD_BIT_SET(y, mask)        ( y |=  (mask) )
#define MSD_BIT_CLEAR(y, mask)      ( y &= ~(mask) )
#define MSD_BIT_FLIP(y, mask)       ( y ^=  (mask) )

	/* ! Create a bitmask of length \a len. */
#define MSD_BIT_MASK(len)           ( MSD_BIT(len)-1 )
	/* ! Create a bitfield mask of length \a starting at bit \a start. */
#define MSD_BF_MASK(start, len)     ( MSD_BIT_MASK(len)<<(start) )
	/* ! Prepare a bitmask for insertion or combining. */
#define MSD_BF_PREP(x, start, len)  ( ((x)&MSD_BIT_MASK(len)) << (start) )
	/* ! Extract a bitfield of length \a len starting at bit \a start from \a y. */
#define MSD_BF_GET(y, start, len)   ( ((y)>>(start)) & MSD_BIT_MASK(len) )
	/* ! Insert a new bitfield value \a x into \a y. */
#define MSD_BF_SET(y, x, start, len)    \
	(y = ((y)&~MSD_BF_MASK(start, len)) | MSD_BF_PREP(x, start, len))

	/****************************************************************************/
	/* Exported ATU Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: struct MSD_ATU_EXT_PRI
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
	*      macFPri    - MAC Queue priority data (0 ~ 7).
	*/
	typedef struct
	{
		MSD_U8             macFPri;
		MSD_U8             macQPri;
	} MSD_ATU_EXT_PRI;
	/*
	*  typedef: struct MSD_ATU_ENTRY
	*
	*  Description: address translation unit Entry
	*
	*  Fields:
	*      macAddr          - mac address
	*      trunkMemberOrLAG - MSD_TRUE if entry belongs to a Trunk/LAG. This field will be
	*                         ignored if device does not support Trunk.
	*      portVec          - port Vector.
	*                         If trunkMember field is MSD_TRUE, this value represents trunk ID.
	*      entryState       - the entry state.
	*      fid              - ATU MAC Address Database number. If multiple address
	*                         databases are not being used, fid should be zero.
	*                         If multiple address databases are being used, this value
	*                         should be set to the desired address database number.
	*      exPrio           - extended priority information.
	*
	*  Comment:
	*      None.
	*/

	typedef struct
	{
		MSD_ETHERADDR      macAddr;
		MSD_BOOL           trunkMemberOrLAG;
		MSD_U32            portVec;
		MSD_U8             reserved;
		MSD_U16            fid;
		MSD_U8			  entryState;
		MSD_ATU_EXT_PRI    exPrio;
	} MSD_ATU_ENTRY;

	/*
	* typedef: enum MSD_FLUSH_CMD
	*
	* Description: Enumeration of the address translation unit flush operation.
	*
	* Enumerations:
	*   MSD_FLUSH_ALL			- flush all entries.
	*   MSD_FLUSH_ALL_NONSTATIC	- flush all non-static entries.
	*/
	typedef enum
	{
		MSD_FLUSH_ALL = 1,
		MSD_FLUSH_ALL_NONSTATIC = 2,
	} MSD_FLUSH_CMD;
	/*
	* typedef: enum MSD_MOVE_CMD
	*
	* Description: Enumeration of the address translation unit move or remove operation.
	*     When destination port is set to 0x1F, Remove operation occurs.
	*
	* Enumerations:
	*   MSD_MOVE_ALL				- move all entries.
	*   MSD_MOVE_ALL_NONSTATIC	- move all non-static entries.
	*/
	typedef enum
	{
		MSD_MOVE_ALL = 1,
		MSD_MOVE_ALL_NONSTATIC = 2,
	} MSD_MOVE_CMD;


	/*
	*  typedef: struct MSD_ATU_INT_CAUSE
	*
	*  Description: ATU Interrupt Cause
	*
	*  Fields:
	*      ageOutVio  -  Age Out Violation
	*      memberVio  -  Member Violation
	*      missVio    -  Miss Violation
	*      fullVio    -  Full Violation
	*
	*/
	typedef struct
	{
		MSD_BOOL ageOutVio;
		MSD_BOOL memberVio;
		MSD_BOOL missVio;
		MSD_BOOL fullVio;
	} MSD_ATU_INT_CAUSE;

	/*
	*  typedef: struct MSD_ATU_INT_STATUS
	*
	*  Description: VLAN translation unit interrupt status
	*
	*  Fields:
	*      intCause  - ATU Interrupt Cause
	*                    MSD_AGE_OUT_VIOLATION,MSD_MEMEBER_VIOLATION,
	*                    MSD_MISS_VIOLATION, MSD_FULL_VIOLATION
	*
	*      SPID      - source port number. If intCause is full violation, it means nothing
	*
	*      fid       - Forwarding Information Database Number. If intCause is full violation, it means nothing
	*
	*      macAddr   - MAC Address
	*/
	typedef struct
	{
		MSD_ATU_INT_CAUSE   atuIntCause;
		MSD_U8    spid;
		MSD_U16   fid;
		MSD_ETHERADDR  macAddr;
	} MSD_ATU_INT_STATUS;
	/****************************************************************************/
	/* Exported ATU Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported VCT Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: enum MSD_ADV_VCT_MOD
	*
	*  Description: Enumeration of Advanced VCT Mode
	*
	*  Fields:
	*      MSD_ADV_VCT_MAX_PEAK		- Maximum Peak
	*      MSD_ADV_VCT_FIRST_PEAK	- First Peak
	*      MSD_ADV_VCT_OFFSET		- Offset
	*	   MSD_ADV_VCT_SAMPLE_POINT	- Sample Point
	*/
	typedef enum
	{
		/* Advanced VCT Mode */
		MSD_ADV_VCT_MAX_PEAK = 0x00,
		MSD_ADV_VCT_FIRST_PEAK = 0x01,
		MSD_ADV_VCT_OFFSET = 0x02,
		MSD_ADV_VCT_SAMPLE_POINT = 0x03
#if 0
		/* Advanced VCT Transmiter Channel Select */
		MSD_ADV_VCT_NO_CROSSPAIR = 0x04,  /* 23_5 bit 13:11 = 000 */
		MSD_ADV_VCT_CROSSPAIR = 0x08   /* 23_5 bit 13:11 = 100, 101, 110, 111 */
#endif
	}  MSD_ADV_VCT_MOD;

	/*
	*  typedef: enum MSD_ADV_VCT_TRANS_CHAN_SEL
	*
	*  Description: Enumeration of Advanced VCT Transmiter Channel Select
	*
	*  Fields:
	*      MSD_ADV_VCT_TCS_NO_CROSSPAIR	- Tx 0 => Rx 0, Tx 1 => Rx 1, Tx 2 => Rx 2, Tx 3 => Rx 3
	*      MSD_ADV_VCT_TCS_CROSSPAIR_0	- Tx 0 => Rx 0, Tx 0 => Rx 1, Tx 0 => Rx 2, Tx 0 => Rx 3
	*      MSD_ADV_VCT_TCS_CROSSPAIR_1   - Tx 1 => Rx 0, Tx 1 => Rx 1, Tx 1 => Rx 2, Tx 1 => Rx 3
	*	   MSD_ADV_VCT_TCS_CROSSPAIR_2	- Tx 2 => Rx 0, Tx 2 => Rx 1, Tx 2 => Rx 2, Tx 2 => Rx 3
	*      MSD_ADV_VCT_TCS_CROSSPAIR_3   - Tx 3 => Rx 0, Tx 3 => Rx 1, Tx 3 => Rx 2, Tx 3 => Rx 3
	*/
	typedef enum
	{
		/* Advanced VCT Mode */
		MSD_ADV_VCT_TCS_NO_CROSSPAIR = 0,
		MSD_ADV_VCT_TCS_CROSSPAIR_0 = 0x4,
		MSD_ADV_VCT_TCS_CROSSPAIR_1 = 0x5,
		MSD_ADV_VCT_TCS_CROSSPAIR_2 = 0x6,
		MSD_ADV_VCT_TCS_CROSSPAIR_3 = 0x7
	} MSD_ADV_VCT_TRANS_CHAN_SEL;

	/*
	*  typedef: enum MSD_ADV_VCT_SAMPLE_AVG
	*
	*  Description: Enumeration of Number of Sample Averaged
	*
	*  Fields:
	*      MSD_ADV_VCT_SAVG_2	- 2 samples
	*      MSD_ADV_VCT_SAVG_4	- 4 samples
	*      MSD_ADV_VCT_SAVG_8	- 8 samples
	*	   MSD_ADV_VCT_SAVG_16	- 16 smaples
	*      MSD_ADV_VCT_SAVG_32   - 32 samples
	*      MSD_ADV_VCT_SAVG_64   - 64 samples
	*      MSD_ADV_VCT_SAVG_128  - 128 samples
	*      MSD_ADV_VCT_SAVG_256  - 256 samples
	*/
	typedef enum
	{
		/* Advanced VCT Mode */
		MSD_ADV_VCT_SAVG_2 = 0,
		MSD_ADV_VCT_SAVG_4 = 1,
		MSD_ADV_VCT_SAVG_8 = 2,
		MSD_ADV_VCT_SAVG_16 = 3,
		MSD_ADV_VCT_SAVG_32 = 4,
		MSD_ADV_VCT_SAVG_64 = 5,
		MSD_ADV_VCT_SAVG_128 = 6,
		MSD_ADV_VCT_SAVG_256 = 7
	} MSD_ADV_VCT_SAMPLE_AVG;

	/*
	*  typedef: struct MSD_ADV_VCT_MODE
	*
	*  Description: strucuture for Advanced VCT Mode
	*
	*  Fields:
	*      mode				- Advanced VCT Mode
	*      transChanSel		- Advanced VCT Transmiter Channel Select
	*      sampleAvg		- Numble of Sample Averaged
	*	   peakDetHyst		- Peak Detection Hysteresis
	*      samplePointDist	- Sample Point Distance
	*/
	typedef unsigned int  MSD_ADV_VCT_PEAKDET_HYST;

	typedef struct
	{
		MSD_ADV_VCT_MOD                 mode;
		MSD_ADV_VCT_TRANS_CHAN_SEL      transChanSel;
		MSD_ADV_VCT_SAMPLE_AVG          sampleAvg;
		MSD_ADV_VCT_PEAKDET_HYST        peakDetHyst;
		MSD_16                          samplePointDist;
	}  MSD_ADV_VCT_MODE;

	/*
	* typedef: enum MSD_ADV_VCT_STATUS
	*
	* Description: Enumeration of Advanced VCT status
	*
	* Enumerations:
	*      MSD_ADV_VCT_FAIL     - advanced virtual cable test failed.
	*                             cable lengh cannot be determined.
	*      MSD_ADV_VCT_NORMAL   - normal cable.
	*                             cable lengh may not be determined.
	*      MSD_ADV_VCT_IMP_GREATER_THAN_115 - impedance mismatch > 115 ohms
	*                             cable lengh is valid.
	*      MSD_ADV_VCT_IMP_LESS_THAN_85 - impedance mismatch < 85 ohms
	*                             cable lengh is valid.
	*      MSD_ADV_VCT_OPEN      - cable open
	*                             cable lengh is valid.
	*      MSD_ADV_VCT_SHORT     - cable shorted
	*                             cable lengh is valid.
	*      MSD_ADV_VCT_CROSS_PAIR_SHORT - cross pair short.
	*                             cable lengh for each channel is valid.
	*/
	typedef enum
	{
		MSD_ADV_VCT_FAIL,
		MSD_ADV_VCT_NORMAL,
		MSD_ADV_VCT_IMP_GREATER_THAN_115,
		MSD_ADV_VCT_IMP_LESS_THAN_85,
		MSD_ADV_VCT_OPEN,
		MSD_ADV_VCT_SHORT,
		MSD_ADV_VCT_CROSS_PAIR_SHORT
	}  MSD_ADV_VCT_STATUS;

#define MSD_MDI_PAIR_NUM         4    /* (1,2),(3,6),(4,5),(7,8) */
#define MSD_CHANNEL_PAIR_NUM     2    /* (channel A,B),(channel C,D) */

	/*
	* typedef: struct MSD_CROSS_PAIR_LIST
	*
	* Description: strucuture for cross pair short channels.
	*
	* Fields:
	*      channel - cross pair short channel list
	*                channel[i] is MSD_TRUE if the channel[i] is cross pair short
	*                with the current channel under test.
	*      dist2fault - estimated distance to the shorted location.
	*                   valid only if related channel (above) is MSD_TRUE.
	*/
	typedef struct _MSD_CROSS_SHORT_LIST
	{
		MSD_BOOL    channel[MSD_MDI_PAIR_NUM];
		MSD_16      dist2fault[MSD_MDI_PAIR_NUM];
	} MSD_CROSS_SHORT_LIST;

	/*
	* typedef: struct MSD_ADV_CABLE_STATUS
	*
	* Description: strucuture for advanced cable status.
	*
	* Fields:
	*      cableStatus - VCT cable status for each channel.
	*      crossShort  - cross pair short list for each channel.
	*                    Valid only if relative cableStatus is MSD_ADV_VCT_CROSS_PAIR_SHORT.
	*      dist2fault  - estimated distance to fault for each channel.
	*                    Valid if relative cableStatus is one of the followings:
	*                      MSD_ADV_VCT_NORMAL
	*                      MSD_ADV_VCT_IMP_GREATER_THAN_115
	*                      MSD_ADV_VCT_IMP_LESS_THAN_85,
	*                      MSD_ADV_VCT_OPEN, or
	*                        MSD_ADV_VCT_SHORT
	*      offset	  - The offset seen at the receiver.
	*      sampleAmp  - returned amplitude for sample point.
	*/
	typedef struct
	{
		MSD_ADV_VCT_STATUS   cableStatus[MSD_MDI_PAIR_NUM];
		union {
			MSD_CROSS_SHORT_LIST crossShort;
			MSD_16     dist2fault;
			MSD_16     offset;
			MSD_16     sampleAmp;
		}u[MSD_MDI_PAIR_NUM];
	}  MSD_ADV_CABLE_STATUS;

	/* Definition for Advance Virtual Cable Test */
	/*
	* typedef: enum MSD_RX_CHANNEL
	*
	* Description: Enumeration of Receiver Channel Assignment
	*
	* Enumerations:
	*        MSD_CHANNEL_A   - Channel A
	*        MSD_CHANNEL_B   - Channel B
	*        MSD_CHANNEL_C   - Channel C
	*        MSD_CHANNEL_D   - Channel D
	*/
	typedef enum
	{
		MSD_CHANNEL_A,
		MSD_CHANNEL_B,
		MSD_CHANNEL_C,
		MSD_CHANNEL_D
	} MSD_RX_CHANNEL;

	/*
	* typedef: enum MSD_POLARITY_STATUS
	*
	* Description: Enumeration of polarity status
	*
	* Enumerations:
	*      MSD_POSITIVE    - positive polarity
	*      MSD_NEGATIVE    - negative polarity
	*/
	typedef enum
	{
		MSD_POSITIVE,
		MSD_NEGATIVE

	} MSD_POLARITY_STATUS;

	/*
	* typedef: struct MSD_ADV_EXTENDED_STATUS
	*
	* Description: Currently the 1000Base-T PCS can determine the cable polarity
	*         on pairs A,B,C,D; crossover on pairs A,B and C,D; and skew among
	*        the pares. These status enhance the capability of the virtual cable tester
	*
	* Fields:
	*      isValid     - MSD_TRUE if this structure have valid information,
	*                      MSD_FALSE otherwise.
	*                      It is valid only if 1000BASE-T Link is up.
	*      pairSwap    - Receive channel assignement
	*      pairPolarity- MSD_POSITIVE, if polarity is positive,
	*                    MSD_NEGATIVE, otherwise
	*      pairSkew    - pair skew in units of ns
	*      cableLen    - cable length based on DSP
	*/
	typedef struct
	{
		MSD_BOOL            isValid;
		MSD_RX_CHANNEL      pairSwap[MSD_MDI_PAIR_NUM];
		MSD_POLARITY_STATUS pairPolarity[MSD_MDI_PAIR_NUM];
		MSD_U32             pairSkew[MSD_MDI_PAIR_NUM];
		MSD_U32             cableLen[MSD_MDI_PAIR_NUM];
	} MSD_ADV_EXTENDED_STATUS;
	/****************************************************************************/
	/* Exported VCT Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported STU Types			 			                                */
	/****************************************************************************/
	/*
	* typedef: enum MSD_PORT_STP_STATE
	*
	* Description: Enumeration of the port spanning tree state.
	*
	* Fields:
	*   MSD_PORT_DISABLE    - port is disabled.
	*   MSD_PORT_BLOCKING   - port is in blocking/listening state.
	*   MSD_PORT_LEARNING   - port is in learning state.
	*   MSD_PORT_FORWARDING - port is in forwarding state.
	*/
	typedef enum
	{
		MSD_PORT_DISABLE = 0,
		MSD_PORT_BLOCKING,
		MSD_PORT_LEARNING,
		MSD_PORT_FORWARDING
	} MSD_PORT_STP_STATE;

	/*
	*  typedef: struct MSD_STU_ENTRY
	*
	*  Description: 802.1s Port State Information Database (STU) Entry
	*
	*  Fields:
	*      sid       - STU ID
	*      portState - Per VLAN Port States for each port.
	*/
	typedef struct
	{
		MSD_U16               sid;
		MSD_PORT_STP_STATE    portState[MSD_MAX_SWITCH_PORTS];
	} MSD_STU_ENTRY;
	/****************************************************************************/
	/* Exported STU Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported VTU Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: struct MSD_VTU_EXT_INFO
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
	*       useVIDQPri - Use VID queue priority override. When this is MSD_TRUE and
	*                     VIDQPriOverride of the ingress port of the frame is enabled,
	*                     vidQPri data is used to override the frame priority on
	*                     any frame associated with this VID.
	*       vidQPri    - VID queue priority data (0 ~ 7).
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
		MSD_BOOL           useVIDQPri;
		MSD_U8             vidQPri;
		MSD_BOOL           dontLearn;
		MSD_BOOL           filterUC;
		MSD_BOOL           filterBC;
		MSD_BOOL           filterMC;
	} MSD_VTU_EXT_INFO;

	/*
	*  typedef: struct MSD_STU_ENTRY
	*
	*  Description: Enumeration for MSD_VLAN_MEMBER_TAG
	*
	*  Fields:
	*      MSD_MEMBER_EGRESS_UNMODIFIED	- Port is a member of this VLAN & frame Egress unmodified
	*      MSD_MEMBER_EGRESS_UNTAGGED	- Port is a member of this VLAN & frame Egress Untagged
	*      MSD_MEMBER_EGRESS_TAGGED		- Port is a member of this VLAN & frame Egress Tagged
	*      MSD_NOT_A_MEMBER				- Port is a member of this VLAN. Any frames with this VID are discarded
	*									  at Ingress and are not allowed to Egress this port
	*/
	typedef enum
	{
		MSD_MEMBER_EGRESS_UNMODIFIED = 0,
		MSD_MEMBER_EGRESS_UNTAGGED,
		MSD_MEMBER_EGRESS_TAGGED,
		MSD_NOT_A_MEMBER
	}MSD_PORT_MEMBER_TAG;

	/*
	*  typedef: struct MSD_VTU_ENTRY
	*
	*  Description: VLAN tarnslaton unit Entry
	*        Each field in the structure is device specific, i.e., some fields may not
	*        be supported by the used switch device. In such case, those fields are
	*        ignored by the MSD driver. Please refer to the datasheet for the list of
	*        supported fields.
	*
	*  Fields:
	*      vid        - VLAN ID
	*      fid        - database number or FID (forwarding information database)
	*      sid        - 802.1s Port State Database ID
	*	   memberTagP - Membership and Egress Tagging
	*                   memberTagP[0] is for Port 0, MemberTagP[1] is for port 1, and so on
	*      vidPolicy  - indicate that the frame with this VID uses VID Policy
	*      vidExInfo  - extanded information for Priority Override and Non Rate Limit
	*/
	typedef struct
	{
		MSD_U16        vid;
		MSD_U16        fid;
		MSD_U8         sid;
		MSD_PORT_MEMBER_TAG    memberTagP[MSD_MAX_SWITCH_PORTS];
		MSD_BOOL           vidPolicy;
		MSD_VTU_EXT_INFO   vidExInfo;
	} MSD_VTU_ENTRY;
	/*
	*  typedef: struct MSD_VTU_INT_CAUSE
	*
	*  Description: VTU Interrupt Cause
	*
	*  Fields:
	*      fullVio    - Full violation occurs or not
	*      memberVio  - Member violation occurs or not
	*      missVio    - Miss violation occurs or not
	*/
	typedef struct
	{
		MSD_BOOL fullVio;
		MSD_BOOL memberVio;
		MSD_BOOL missVio;
	} MSD_VTU_INT_CAUSE;
	/*
	*  typedef: struct MSD_VTU_INT_STATUS
	*
	*  Description: VLAN tarnslaton unit interrupt status
	*
	*  Fields:
	*      intCause  - VTU Interrupt Cause
	*      SPID      - source port number
	*                     if intCause is MSD_VTU_FULL_VIOLATION, it means nothing
	*      vid       - VLAN ID
	*                     if intCause is MSD_VTU_FULL_VIOLATION, it means nothing
	*					   vid[12] is the vtuPage, vid[11:0] is the original vid
	*/
	typedef struct
	{
		MSD_VTU_INT_CAUSE   vtuIntCause;
		MSD_U8    spid;
		MSD_U16   vid;
	} MSD_VTU_INT_STATUS;

	/*
	*  typedef: enum MSD_VTU_MODE
	*
	*  Description: VLAN tarnslaton unit mode
	*
	*  Fields:
	*      MSD_VTU_MODE_0   - 4K vtu, page should be 0 unless overridden by TCAM
	*      MSD_VTU_MODE_1,  - 8K vtu with vtu  page being  0 if a frame is a  unicast frame
	*      MSD_VTU_MODE_2,  - use 8K vtu with vtu page being = to the source
	*                        port¡¯s VTU page bit(port offset 0x05)
	*      MSD_VTU_MODE_3,  - use 8K vtu with vtu page being a one if frame is not
	*	                     properly Provider tagged entering a Provider port
	*/
	typedef enum
	{
		MSD_VTU_MODE_0 = 0,
		MSD_VTU_MODE_1,
		MSD_VTU_MODE_2,
		MSD_VTU_MODE_3,
	}MSD_VTU_MODE;

	/*
	* Bit Vector Definition for VTU Violation
	*/
#define MSD_VTU_MEMBER_VIOLATION            	0x4
#define MSD_VTU_MISS_VIOLATION              	0x2
#define MSD_VTU_FULL_VIOLATION       	 	0x1    
	/****************************************************************************/
	/* Exported VTU Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported Phy Control Types			 			                        */
	/****************************************************************************/
	/*
	* typedef: enum MSD_PHY_AUTO_MODE
	*
	* Description: Enumeration of Autonegotiation mode.
	*    Auto for both speed and duplex.
	*    Auto for speed only and Full duplex.
	*    Auto for speed only and Half duplex. (1000Mbps is not supported)
	*    Auto for duplex only and speed 1000Mbps.
	*    Auto for duplex only and speed 100Mbps.
	*    Auto for duplex only and speed 10Mbps.
	*    1000Mbps Full duplex.
	*    100Mbps Full duplex.
	*    100Mbps Half duplex.
	*    10Mbps Full duplex.
	*    10Mbps Half duplex.
	*/
	typedef enum
	{
		MSD_SPEED_AUTO_DUPLEX_AUTO,
		MSD_SPEED_1000_DUPLEX_AUTO,
		MSD_SPEED_100_DUPLEX_AUTO,
		MSD_SPEED_10_DUPLEX_AUTO,
		MSD_SPEED_AUTO_DUPLEX_FULL,
		MSD_SPEED_AUTO_DUPLEX_HALF,
		MSD_SPEED_1000_DUPLEX_FULL,
		MSD_SPEED_1000_DUPLEX_HALF,
		MSD_SPEED_100_DUPLEX_FULL,
		MSD_SPEED_100_DUPLEX_HALF,
		MSD_SPEED_10_DUPLEX_FULL,
		MSD_SPEED_10_DUPLEX_HALF
	}MSD_PHY_AUTO_MODE;

	/*
	* typedef: enum MSD_PHY_SPEED
	*
	* Description: Enumeration of Phy Speed
	*
	* Fields:
	*    PHY_SPEED_10_MBPS   - 10Mbps
	*    PHY_SPEED_100_MBPS  - 100Mbps
	*    PHY_SPEED_1000_MBPS - 1000Mbps
	*/
	typedef enum
	{
		MSD_PHY_SPEED_10_MBPS,
		MSD_PHY_SPEED_100_MBPS,
		MSD_PHY_SPEED_1000_MBPS
	} MSD_PHY_SPEED;
	/****************************************************************************/
	/* Exported Phy Control Types			 			                        */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported PIRL Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: enum MSD_PIRL_COUNT_MODE
	*
	*  Description: Enumeration of the port egress rate limit counting mode.
	*
	*  Fields:
	*      MSD_PIRL_COUNT_FRAME -
	*                Count the number of frames
	*      MSD_PIRL_COUNT_ALL_LAYER1 -
	*                Count all Layer 1 bytes:
	*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
	*      MSD_PIRL_COUNT_ALL_LAYER2 -
	*                Count all Layer 2 bytes: Frame's DA to CRC
	*      MSD_PIRL_COUNT_ALL_LAYER3 -
	*                Count all Layer 3 bytes:
	*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
	*/
	typedef enum
	{
		MSD_PIRL_COUNT_FRAME = 0,
		MSD_PIRL_COUNT_ALL_LAYER1,
		MSD_PIRL_COUNT_ALL_LAYER2,
		MSD_PIRL_COUNT_ALL_LAYER3
	} MSD_PIRL_COUNT_MODE;

	/*
	*  typedef: struct MSD_PIRL_CUSTOM_RATE_LIMIT
	*
	*  Description:  This structure includes all the necessary parameters used to set port
	*                ingress rate limit. It's the output of API 'gpirlCustomSetup' based on users'
	*                target rate and target burst allocation. And also, this structure can be used
	*                in API 'gpirlWriteResource' to configure IRL port resource. You can check IRL
	*                sample code for detail usage. If the real Ingress Rate Limit is too much off
	*                from the target rate, please contact FAE and gets the correct parameters.
	*
	*  Fields:
	*        isValid         - Reserved for future use
	*        ebsLimit        - Excess Burst Size limit on the Yellow bucket ( 0 ~ 0xFFFFFF)
	*                          In Single Rate Two Color Mode, EBSLimit must be cleared to zero.
	*        cbsLimit        - Committed Burst Size limit on the Green bucket
	*        bktIncrement    - the amount of tokens that need to be added for each
	*                          byte of packet information.
	*        bktRateFactorGrn   - the amount of tokens that need to be decremented for each rate
	*                             resource decrement on the Green Bucket.
	*                             The rate for Green is:
	*                                  Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * BRFGrn)/BktInc
	*                                  Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * BRFGrn)/BktInc
	*        bktRateFactorYel   - the amount of tokens that need to be decremented for each rate
	*                             resource decrement on the Yellow Bucket.
	*                             In Single Rate Two Color Mode, BRFYel must be cleared to zero.
	*                             In Single Rate Three Color Mode, BRFYel must be equal to BRFGrn. Both must be non-zero.
	*                                  The rate for (Green + Yellow) is:
	*                                      Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * BRFYel)/BktInc
	*                                      Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * BRFTel)/BktInc
	*							   In Dual Rate Three Color Mode, BRFYel must be greater than BRFGrn
	*                                  The Yellow rate limit is:
	*                                      Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * (BRFYel-BRFGrn)/BktInc
	*                                      Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * (BRFYel-BRFGrn)/BktInc
	*        IRLConstant      - internal coefficient
	*        countMode	      - bytes to be counted for accounting
	*                          MSD_PIRL2_COUNT_FRAME,
	*                          MSD_PIRL2_COUNT_ALL_LAYER1,
	*                          MSD_PIRL2_COUNT_ALL_LAYER2, or
	*                          MSD_PIRL2_COUNT_ALL_LAYER3
	*/
	typedef struct
	{
		MSD_BOOL       isValid;
		MSD_U32        ebsLimit;
		MSD_U32        cbsLimit;
		MSD_U32        bktIncrement;
		MSD_U32        bktRateFactorGrn;
		MSD_U32        bktRateFactorYel;
		/*topaz*/
		MSD_U32		  IRLConstant;

		/*Count Mode*/
		MSD_PIRL_COUNT_MODE       countMode;

	} MSD_PIRL_CUSTOM_RATE_LIMIT;

	/*
	*  typedef: enum MSD_PIRL_FC_DEASSERT
	*
	*  Description: Enumeration of the port flow control de-assertion mode on PIRL.
	*
	*  Fields:
	*      MSD_PIRL_FC_DEASSERT_EMPTY -
	*                De-assert when the ingress rate resource has become empty
	*      MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
	*                De-assert when the ingress rate resource has enough room as
	*                specified by Limit.
	*/
	typedef enum
	{
		MSD_PIRL_FC_DEASSERT_EMPTY = 0,
		MSD_PIRL_FC_DEASSERT_LIMIT
	} MSD_PIRL_FC_DEASSERT;

	/*
	* Typedef: enum MSD_PIRL_ACTION
	*
	* Description: Defines the Action that should be taken when
	*        there there are not enough tokens to accept the entire incoming frame
	*
	* Fields:
	*        MSD_PIRL_ACTION_DROP - drop packets
	*        MSD_PIRL_ACTION_FC - end flow control packet
	*/
	typedef enum
	{
		MSD_PIRL_ACTION_DROP = 0x0,
		MSD_PIRL_ACTION_FC = 0x1
	} MSD_PIRL_ACTION;

	
	/*
	*  typedef: struct MSD_PIRL_DATA
	*
	*  Description: data structure for PIRL Bucket programing that is resource based.
	*
	*  Fields:
	*      bktTypeMask	- various traffic type to be counted
	*      tcamFlows	- indicate to account for tcam flows 0 to 3, but not TCP/UDP data
	*      priAndPt    - determine the incoming frames that get rate limited using
	*                    this ingress rate resource.
	*                         MSD_TRUE - typeMask And priMask
	*                         MSD_FALSE - typeMask Or priMask
	*      useFPri     - used to determine what priority type this resource is to account for
	*                    via the Pri Select bits below as follows:
	*                          MSD_TRUE  - use the FPri assigned to the frame
	*                          MSD_FALSE - use the QPri assigned to the frame
	*      priSelect   - priority bit mask that each bit indicates one of the eight
	*                    priorities. Setting each one of these bits indicates
	*                    that this particular rate resource is slated to account for
	*                    incoming frames with the enabled bits' priority.
	*      colorAware  - set to color aware mode or color blind mode
	*      accountGrnOverflow	- Green&Yellow buckets are connected when in Coloar Aware mode.
	*      accountQConf    - account discarded frames due to queue congestion
	*      accountFiltered - account filtered frames. Account for All
	*      samplingMode    - sample one out of so many frames/bytes for a stream of frames
	*      actionMode      - action should be taken when there are not enough tokens
	*                        to accept the entire incoming frame
	*                                MSD_PIRL_ACTION_DROP - drop frame
	*                                MSD_PIRL_ACTION_FC - send flow control mode.
	*      fcMode         - flow control de-assertion mode when actionMode is
	*                       set to MSD_PIRL_ACTION_FC.
	*                                MSD_PIRL_FC_DEASSERT_EMPTY -
	*                                    De-assert when the ingress rate resource has
	*                                    become empty.
	*                                MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
	*                                    De-assert when the ingress rate resource has
	*                                    enough room as specified by the EBSLimit.
	*      fcPriority     - When the ingress port associated with this resource is enabled
	*                          for Priority Flow Control and if this resource is configured to
	*                          use FC Action, then this register is used to indicate the PFC
	*                          priority that is to be used on the Priority flow Control frames
	*                          generated due to this resource.
	*
	*/
	typedef struct
	{
		/* frame types to be accounted*/
		MSD_U16 		bktTypeMask;
		MSD_BOOL		tcamFlows;
		MSD_BOOL		priAndPt;
		MSD_BOOL		useFPri;
		MSD_U8		priSelect;

		/*color aware or color blind mode*/
		MSD_BOOL		colorAware;

		/*condition for account*/
		MSD_BOOL		accountGrnOverflow;
		MSD_BOOL		accountQConf;
		MSD_BOOL		accountFiltered;

		/*condition for account*/
		MSD_BOOL		samplingMode;

		/*flow control*/
		MSD_PIRL_ACTION    actionMode;
		MSD_PIRL_FC_DEASSERT		fcMode;

		MSD_U8	    fcPriority;	/* used when port PFC mode enabled */

		/*customer setup*/
		MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
	} MSD_PIRL_DATA;

	/*
	*  typedef: struct MSD_PIRL_PKTTYPE_OR_PRI
	*
	*  Description: data structure for PIRL Rate limit based on packet type of priority.
	*
	*  Fields:
	*      bktTypeMask	- various traffic type to be counted
	*      tcamFlows	- indicate to account for tcam flows 0 to 3, but not TCP/UDP data
	*      priAndPt    - determine the incoming frames that get rate limited using
	*                    this ingress rate resource.
	*                         MSD_TRUE - typeMask And priMask
	*                         MSD_FALSE - typeMask Or priMask
	*      useFPri     - used to determine what priority type this resource is to account for
	*                    via the Pri Select bits below as follows:
	*                          MSD_TRUE  - use the FPri assigned to the frame
	*                          MSD_FALSE - use the QPri assigned to the frame
	*      priSelect   - priority bit mask that each bit indicates one of the eight
	*                    priorities. Setting each one of these bits indicates
	*                    that this particular rate resource is slated to account for
	*                    incoming frames with the enabled bits' priority.
	*      colorAware  - set to color aware mode or color blind mode
	*      accountGrnOverflow	- Green&Yellow buckets are connected when in Coloar Aware mode.
	*      accountQConf    - account discarded frames due to queue congestion
	*      accountFiltered - account filtered frames. Account for All
	*      samplingMode    - sample one out of so many frames/bytes for a stream of frames
	*      actionMode      - action should be taken when there are not enough tokens
	*                        to accept the entire incoming frame
	*                                MSD_PIRL_ACTION_DROP - drop frame
	*                                MSD_PIRL_ACTION_FC - send flow control mode.
	*      fcMode         - flow control de-assertion mode when actionMode is
	*                       set to MSD_PIRL_ACTION_FC.
	*                                MSD_PIRL_FC_DEASSERT_EMPTY -
	*                                    De-assert when the ingress rate resource has
	*                                    become empty.
	*                                MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
	*                                    De-assert when the ingress rate resource has
	*                                    enough room as specified by the EBSLimit.
	*      fcPriority     - When the ingress port associated with this resource is enabled
	*                          for Priority Flow Control and if this resource is configured to
	*                          use FC Action, then this register is used to indicate the PFC
	*                          priority that is to be used on the Priority flow Control frames
	*                          generated due to this resource.
	*
	*/
	typedef struct
	{
		MSD_U16   bktTypeMask;
		MSD_BOOL  tcamFlows;
		MSD_BOOL  priAndPt;
		MSD_BOOL  useFPri;
		MSD_U8	 priSelect;
	} MSD_PIRL_PKTTYPE_OR_PRI;
	/****************************************************************************/
	/* Exported PIRL Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported Port Control Types			 			                        */
	/****************************************************************************/
	/*  typedef: enum MSD_8021Q_MODE
	*
	*   Description: Enumeration of 802.1Q mode
	*
	*  Fields:
	*      MSD_8021Q_DISABLE		- Use Port Based VLANs only.
	*      MSD_8021Q_FALLBACK	- Enable 802.1Q for this Ingress port.
	*							  Do not discard Ingress Membership violations and use the
	*							  VLANTable bits if the frame¡¯s VID is not contained in the VTU.
	*      MSD_8021Q_CHECK		- Enable 802.1Q for this Ingress port.
	*							  Do not discard Ingress Membership violations but discard
	*							  frames if its VID is not contained in the VTU.
	*      MSD_8021Q_SECURE		- Enable 802.1Q for this Ingress port.
	*							  Discard Ingress Membership violations and discard frames
	*							  whose VID is not contained in the VTU
	*/
	typedef enum
	{
		MSD_8021Q_DISABLE = 0,
		MSD_8021Q_FALLBACK,
		MSD_8021Q_CHECK,
		MSD_8021Q_SECURE
	} MSD_8021Q_MODE;

	/*
	* Typedef: enum MSD_MTU_SIZE
	*
	* Description: Defines Jumbo Frame Size allowed to be tx and rx
	*
	* Fields:
	*      MSD_MTU_1522	- Rx and Tx frames with max byte of 1522.
	*      MSD_MTU_2048	- Rx and Tx frames with max byte of 2048.
	*      MSD_MTU_10240 - Rx and Tx frames with max byte of 10240.
	*
	*/
	typedef enum
	{
		MSD_MTU_1522 = 0,
		MSD_MTU_2048,
		MSD_MTU_10240
	} MSD_MTU_SIZE;

	/*
	* Typedef: enum MSD_PORT_FC_MODE
	*
	* Description: Definition for full duplex flow control mode
	*
	* Fields:
	*      PORT_FC_TX_RX_ENABLED	- Enable IEEE 802.3x Pause for both transmit and receive.
	*      PORT_FC_RX_ONLY			- Process received IEEE 802.3x frames, don¡¯t transmit Pause.
	*      PORT_FC_TX_ONLY			- Transmit IEEE 802.3x frames, ignore received Pause.
	*      PORT_PFC_ENABLED			- Enable IEEE 802.1Qbb Priority Flow Control (PFC).
	*/
	typedef enum
	{
		MSD_PORT_FC_TX_RX_ENABLED,
		MSD_PORT_FC_RX_ONLY,
		MSD_PORT_FC_TX_ONLY,
		MSD_PORT_PFC_ENABLED
	} MSD_PORT_FC_MODE;

	/*
	*  typedef: enum MSD_ELIMIT_MODE
	*
	*  Description: Enumeration of the port egress rate limit counting mode.
	*
	*  Fields:
	*      MSD_ELIMIT_FRAME -
	*                Count the number of frames
	*      MSD_ELIMIT_LAYER1 -
	*                Count all Layer 1 bytes:
	*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
	*      MSD_ELIMIT_LAYER2 -
	*                Count all Layer 2 bytes: Frame's DA to CRC
	*      MSD_ELIMIT_LAYER3 -
	*                Count all Layer 3 bytes:
	*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
	*/
	typedef enum
	{
		MSD_ELIMIT_FRAME = 0,
		MSD_ELIMIT_LAYER1,
		MSD_ELIMIT_LAYER2,
		MSD_ELIMIT_LAYER3
	} MSD_ELIMIT_MODE;

	/*
	* Typedef: union MSD_ERATE_TYPE
	*
	* Description: Egress Rate
	*
	* Fields:
	*      kbRate      - rate in kbps that should used with the MSD_ELIMIT_MODE of
	*                                MSD_ELIMIT_LAYER1,
	*                                MSD_ELIMIT_LAYER2, or
	*                                MSD_ELIMIT_LAYER3 (see grcSetELimitMode)
	*                            64kbps ~ 1Mbps    : increments of 64kbps,
	*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
	*                            100Mbps ~ 1000Mbps: increments of 10Mbps
	*                            1Gbps ~ 5Gbps: increments of 100Mbps
	*                            Therefore, the valid values are:
	*                                64, 128, 192, 256, 320, 384,..., 960,
	*                                1000, 2000, 3000, 4000, ..., 100000,
	*                                110000, 120000, 130000, ..., 1000000
	*                                1100000, 1200000, 1300000, ..., 5000000.
	*      fRate         - frame per second that should used with MSD_ELIMIT_MODE of
	*                                MSD_PIRL_ELIMIT_FRAME
	*                            Valid values are between 3815 and 14880000
	*/
	typedef union
	{
		MSD_U32            kbRate;
		MSD_U32            fRate;
	} MSD_ERATE_TYPE;

	/*
	* Typedef: enum MSD_PORT_FORCE_DUPLEX_MODE
	*
	* Description: Definition for the Port Duplex Mode
	*
	* Fields:
	*      MSD_DUPLEX_HALF	- Half duplex
	*	   MSD_DUPLEX_FULL	- Full duplex
	*      MSD_DUPLEX_AUTO	- Auto for duplex
	*/
	typedef enum
	{
		MSD_DUPLEX_HALF,
		MSD_DUPLEX_FULL,
		MSD_DUPLEX_AUTO
	} MSD_PORT_FORCE_DUPLEX_MODE;

	/*
	* Typedef: enum MSD_PORT_SPEED
	*
	* Description: Definition for the Port Speed
	*
	* Fields:
	*      MSD_PORT_SPEED_10M	  - Set speed value 10Mbps
	*	   MSD_PORT_SPEED_100M	  - Set speed value 100Mbps
	*	   MSD_PORT_SPEED_200M	  - Set speed value 200Mbps
	*      MSD_PORT_SPEED_1000M	  - Set speed value 1000Mbps
	*	   MSD_PORT_SPEED_2_5G	  - Set speed value 2.5Gbps
	*      MSD_PORT_SPEED_10G	  - Set speed value 10Gbps
	*      MSD_PORT_SPEED_UNKNOWN  - Set speed unknown value
	*/
	typedef enum
	{
		MSD_PORT_SPEED_10M = 0,
		MSD_PORT_SPEED_100M = 1,
		MSD_PORT_SPEED_200M = 2,
		MSD_PORT_SPEED_1000M = 3,
		MSD_PORT_SPEED_2_5G = 4,
		MSD_PORT_SPEED_10G = 5,
		MSD_PORT_SPEED_UNKNOWN = 6
	} MSD_PORT_SPEED;

	/*
	* Typedef: enum MSD_PORT_FORCED_SPEED_MODE
	*
	* Description: Definition for the Port Force Speed
	*
	* Fields:
	*      MSD_PORT_FORCE_SPEED_10M		- Force speed value 10Mbps
	*	   MSD_PORT_FORCE_SPEED_100M		- Force speed value 100Mbps
	*      MSD_PORT_FORCE_SPEED_200M		- Force speed value 200Mbps
	*      MSD_PORT_FORCE_SPEED_1000M	- Force speed value 1000Mbps
	*	   MSD_PORT_FORCE_SPEED_2_5G		- Force speed value 2.5Gbps
	*      MSD_PORT_FORCE_SPEED_10G		- Force speed value 10Gbps
	*      MSD_PORT_DO_NOT_FORCE_SPEED	- Do not Force speed value
	*/
	typedef enum
	{
		MSD_PORT_FORCE_SPEED_10M = 0x0,
		MSD_PORT_FORCE_SPEED_100M,
		MSD_PORT_FORCE_SPEED_200M,
		MSD_PORT_FORCE_SPEED_1000M,
		MSD_PORT_FORCE_SPEED_2_5G,
		MSD_PORT_FORCE_SPEED_10G,
		MSD_PORT_DO_NOT_FORCE_SPEED
	} MSD_PORT_FORCED_SPEED_MODE;

	/*
	* Typedef: enum MSD_PORT_FORCE_LINK_MODE
	*
	* Description: Definition for the Port Force Link Mode
	*
	* Fields:
	*      MSD_PORT_FORCE_LINK_UP		- Force link up
	*	   MSD_PORT_FORCE_LINK_DOWN		- Force link down
	*      MSD_PORT_DO_NOT_FORCE_LINK	- Do not force link
	*/
	typedef enum
	{
		MSD_PORT_FORCE_LINK_UP,
		MSD_PORT_FORCE_LINK_DOWN,
		MSD_PORT_DO_NOT_FORCE_LINK
	} MSD_PORT_FORCE_LINK_MODE;

	/*
	* Typedef: enum MSD_PRI_OVERRIDE
	*
	* Description: Enumeration of priority override
	*
	* Fields:
	*      MSD_PRI_OVERRIDE_NONE			- Normal frame priority processing occurs
	*	   MSD_PRI_OVERRIDE_FRAME		- VTU frame priority overrides can occur on this port
	*      MSD_PRI_OVERRIDE_QUEUE		- VTU queue priority overrides can occur on this port
	*      MSD_PRI_OVERRIDE_FRAME_QUEUE	- VTU frame and queue priority overrides can occur on this port
	*/
	typedef enum
	{
		MSD_PRI_OVERRIDE_NONE = 0,
		MSD_PRI_OVERRIDE_FRAME,
		MSD_PRI_OVERRIDE_QUEUE,
		MSD_PRI_OVERRIDE_FRAME_QUEUE,
	} MSD_PRI_OVERRIDE;

	/*
	*  typedef: struct MSD_CUT_THROUGH
	*
	*  Description: structure for Cut Through.
	*
	*  Fields:
	*      enable		- Enable the cut through queue
	*      cutThruQueue	- Set the value of cut through queue
	*/
	typedef struct
	{
		MSD_BOOL enable;
		MSD_U8	cutThruQueue;
	} MSD_CUT_THROUGH;

	/*
	* Typedef: enum MSD_FRAME_MODE
	*
	* Description: Defines Frame Mode
	*
	* Fields:
	*      MSD_FRAME_MODE_NORMAL			- Normal mode.
	*      MSD_FRAME_MODE_DSA			- DSA mode.
	*      MSD_FRAME_MODE_PROVIDER		- Provider mode.
	*      MSD_FRAME_MODE_ETHER_TYPE_DSA	- Ether Type DSA mode.
	*/
	typedef enum
	{
		MSD_FRAME_MODE_NORMAL,
		MSD_FRAME_MODE_DSA,
		MSD_FRAME_MODE_PROVIDER,
		MSD_FRAME_MODE_ETHER_TYPE_DSA
	}  MSD_FRAME_MODE;
	/****************************************************************************/
	/* Exported Port Control Types			 			                        */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported Qos Map Types			 			                            */
	/****************************************************************************/
	/*
	* Typedef: enum MSD_INITIAL_PRI
	*
	* Description: Defines Initial Priority
	*
	* Fields:
	*      MSD_INITIAL_PRI_DEFAULT	- Use Port defauts for Fpri, Qpri&Color.
	*      MSD_INITIAL_PRI_TAG		- Use Tag Priority.
	*      MSD_INITIAL_PRI_IP		- Use IP Priority.
	*	   MSD_INITIAL_PRI_TAG_IP	- Use Tag&IP Priority.
	*/
	typedef enum
	{
		MSD_INITIAL_PRI_DEFAULT = 0,
		MSD_INITIAL_PRI_TAG,
		MSD_INITIAL_PRI_IP,
		MSD_INITIAL_PRI_TAG_IP
	} MSD_INITIAL_PRI;

	/*
	* Typedef: enum MSD_IEEE_PRIORITY_TABLE_SET
	*
	* Description: Supported Frame Priority Table set For IEEE priority mapping
	*
	* Fields:
	*      MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI	-
	*									Ingress PCP to IEEE_FPRI & IEEE_QPRI mapping
	*      MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP -
	*									Egress Green Frame FPRI to PCP mapping (non-AVB)
	*      MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP -
	*									Egress Yellow Frame FPRI to PCP mapping (non-AVB)
	*	   MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP	-
	*									Egress AVB Frame FPRI to PCP mapping
	*      MSD_IEEE_PRIORITY_TABLE_RESERVED -
	*									Reserved for future use
	*      MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP -
	*									Egress Green Frame FPRI to DSCP mapping (non-AVB)
	*      MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP -
	*									Egress Yellow Frame FPRI to DSCP mapping (non-AVB)
	*	   MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP -
	*									Egress AVB Frame FPRI to DSCP mapping
	*/
	typedef enum
	{
		MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI,   /* 0 */
		MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP, /* 0x1 */
		MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP,   /* 0x2 */
		MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP,   /* 0x3 */
		MSD_IEEE_PRIORITY_TABLE_RESERVED,			/* 0x4 */
		MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP,/* 0x5 */
		MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP,  /* 0x6 */
		MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP,  /* 0x7 */
	} MSD_IEEE_PRIORITY_TABLE_SET;

	/*
	* Typedef: struct each table
	*
	* Description: Supported the format of each Table set For IEEE priority mapping
	*
	* Fields:
	*      ieeeYellow		- IEEE Yellow. When this bit is set to one any
	*						 IEEE Tagged frame that accesses this entry will be given an initial IEEE color of Yellow
	*
	*      disIeeeQpri		- Disable IEEE QPRI Mapping
	*      ieeeQpri			- IEEE Queue Priority Mapping
	*      disIeeeFpri		- Disable IEEE FPRI Mapping
	*      ieeeFpri			- IEEE Frame Priority Mapping
	*
	*      disVIDMarking	- Disable VID Marking for this FPRI
	*      disPCPMarking	- Disable FPRI to PCP Marking for this FPRI
	*      framePCP			- Frame Priority Code Point
	*      enDSCPMarking	- Enable FPRI to DSCP Marking for this FPRI
	*/
	typedef struct
	{
		MSD_U8     ieeeYellow;
		MSD_U8     disIeeeQpri;
		MSD_U8     ieeeQpri;
		MSD_U8     disIeeeFpri;
		MSD_U8     ieeeFpri;
	} MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;

	typedef struct
	{
		MSD_U8     disVIDMarking;
		MSD_U8     disPCPMarking;
		MSD_U8     framePCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;

	typedef struct
	{
		MSD_U8     disVIDMarking;
		MSD_U8     disPCPMarking;
		MSD_U8     framePCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;

	typedef struct
	{
		MSD_U8     disVIDMarking;
		MSD_U8     disPCPMarking;
		MSD_U8     framePCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;

	typedef struct
	{
		MSD_U8     enDSCPMarking;
		MSD_U8     frameDSCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;

	typedef struct
	{
		MSD_U8     enDSCPMarking;
		MSD_U8     frameDSCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;

	typedef struct
	{
		MSD_U8     enDSCPMarking;
		MSD_U8     frameDSCP;
	} MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;

	/*
	* Typedef: union MSD_IEEE_PRIORITY_TABLE_STRUCT
	*
	* Description: IEEE Priority Table
	*
	* Fields:
	*      ingTbl			- Ingress PCP to IEEE_FPRI & IEEE_QPRI
	*      egGreeTblPcp		- Egress Green Frame FPRI to PCP
	*      egYelTblPcp      - Egress Yellow Frame FPRI to PCP
	*      egAvbTblPcp      - Egress AVB Frame FPRI to PCP
	*      egGreeTblDscp	- Egress Green Frame FPRI to DSCP
	*      egYelTblDscp		- Egress Yellow Frame FPRI to DSCP
	*      egAvbTbldscp		- Egress AVB Frame FPRI to DSCP
	*/
	typedef union
	{
		MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT     ingTbl;
		MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT   egGreeTblPcp;
		MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT     egYelTblPcp;
		MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT     egAvbTblPcp;
		MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT  egGreeTblDscp;
		MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT    egYelTblDscp;
		MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT    egAvbTbldscp;
	} MSD_IEEE_PRIORITY_TABLE_STRUCT;
	/****************************************************************************/
	/* Exported Qos Map Types			 			                            */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported MIBS Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: struct MSD_STATS_COUNTERS
	*
	*  Description: MIB counter
	*
	*  Fields:
	*      MSD_STATS_InGoodOctetsLo -
	*						The lower 32-bits of the 64-bit InGoodOctets counter
	*	   MSD_STATS_InGoodOctetsHi -
	*                       The upper 32-bits of the 64-bit InGoodOctets counter
	*      MSD_STATS_InBadOctets -
	*                       The sum of lengths of all bad Ethrnet frames received
	*      MSD_STATS_OutFCSErr -
	*                       The number of frames transmitted with an invalid FCS
	*      MSD_STATS_InUnicasts -
	*                       The number of good frames received that have a Unicast destination MAC address
	*      MSD_STATS_Deferred -
	*                       The total number of successfully transmitted frames that experienced no collisions but are delayed
	*      MSD_STATS_InBroadcasts -
	*                       The number of good frames received that have a Broadcast destination MAC address
	*      MSD_STATS_InMulticasts -
	*                       The number of good frames received that have a Multicast destination MAC address
	*      MSD_STATS_64Octets -
	*						Total frames received with a length of exactly 64 octets, including those with errors
	*      MSD_STATS_127Octets -
	*						Total frames received with a length of exactly 65 and 127 octets, including those with errors
	*      MSD_STATS_255Octets -
	*						Total frames received with a length of exactly 128 and 255 octets, including those with errors
	*      MSD_STATS_511Octets -
	*						Total frames received with a length of exactly 256 and 511 octets, including those with errors
	*      MSD_STATS_1023Octets -
	*						Total frames received with a length of exactly 1024 and MaxSize octets, including those with errors
	*      MSD_STATS_MaxOctets -
	*						The number of good frames received that have a Unicast destination MAC address
	*      MSD_STATS_OutOctetsLo -
	*						The lower 32-bits of the 64-bit OutOctets counter
	*      MSD_STATS_OutOctetsHi -
	*						The upper 32-bits of the 64-bit OutOctets counter
	*      MSD_STATS_OutUnicasts -
	*						The number of frames sent that have a Unicast destination MAC address
	*      MSD_STATS_Excessive -
	*						The number frames dropped in the transmit MAC.
	*      MSD_STATS_OutMulticasts -
	*						The number of good frames sent that have a Multicast destination MAC address
	*      MSD_STATS_OutBroadcasts -
	*						The number of good frames sent that have a Broadcast destination MAC address
	*      MSD_STATS_Single -
	*						The total number of successfully transmitted framnes that experienced exactly one collision
	*      MSD_STATS_OutPause -
	*						The number of Flow Control frames sent
	*      MSD_STATS_InPause -
	*						The number of Good frames received that have a Pause destination MAC address
	*      MSD_STATS_Multiple -
	*						The total number of successfully transmitted frames that experienced more than one collision.
	*      MSD_STATS_InUndersize -
	*						Total frames received with a length of less than 64 octets but with a valid FCS
	*      MSD_STATS_InFragments -
	*						Total frames received with a length of less than 64 octets and an invalid FCS
	*      MSD_STATS_InOversize -
	*						Total frames received with a length of less than MaxSize octets but with a valid FCS
	*      MSD_STATS_InJabber -
	*						Total frames received with a length of less than MaxSize octets but with an invalid FCS
	*      MSD_STATS_InRxErr -
	*						Total frames received with an RxErr signal from the PHY
	*      MSD_STATS_InFCSErr -
	*						Total frames received with a CRC error not counted in InFragments, InJabber or InRxErr.
	*      MSD_STATS_Collisions -
	*						The number of collision events seen by the MAC not including those counted in
	*						Single, Multiple, Excessive, or Late
	*      MSD_STATS_Late -
	*						The number of times a collision is detected later than 52 bits-times into the
	*						transmission of a frame
	*      MSD_STATS_InDiscards -
	*						The number of good, non-filtered, frames that are received but can¡¯t be forwarded
	*						due to a lack of buffer memory
	*      MSD_STATS_InFiltered -
	*						The number of good frames that were not forwarded due to policy filtering rules
	*						such as but not limited to: 802.1Q Mode, Tagging mode, SA fitering e.t.c
	*      MSD_STATS_InAccepted -
	*						The number of good frames that are not policy filtered nor discarded due to an error
	*						and made it throuth the Ingress amd is presented to the Queue Controller
	*	   MSD_STATS_InBadAccepted -
	*						The number of frames with a CRC error that is not filtered nor discarded
	*      MSD_STATS_InGoodAvbClassA -
	*						The number of good AVB frames received that have a Priority Code Point for
	*						Class A that are not Undersize nor Oversize and are not discarded or fitered
	*      MSD_STATS_InGoodAvbClassB -
	*						The number of good AVB frames received that have a Priority Code Point for
	*						Class B that are not Undersize nor Oversize and are not discarded or fitered
	*      MSD_STATS_InBadAvbClassA -
	*						The number of bad AVB frames received that have a Priority Code Point for
	*						Class A that are not Undersize nor Oversize
	*	   MSD_STATS_InBadAvbClassB -
	*						The number of bad AVB frames received that have a Priority Code Point for
	*						Class B that are not Undersize nor Oversize
	*      MSD_STATS_TCAMCounter0 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=0 and are not discarded or filtered
	*      MSD_STATS_TCAMCounter1 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=1 and are not discarded or filtered
	*      MSD_STATS_TCAMCounter2 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=2 and are not discarded or filtered
	*	   MSD_STATS_TCAMCounter3 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=3 and are not discarded or filtered
	*      MSD_STATS_InDroppedAvbA -
	*						The number of good AVB frames received that have a Priority Code Point for Class A that are not
	*						Undersize nor Oversize and are not discarded or filtered but were not kept by the switch due to a lack of AVB buffers
	*      MSD_STATS_InDroppedAvbB -
	*						The number of good AVB frames received that have a Priority Code Point for Class B that are not
	*						Undersize nor Oversize and are not discarded or filtered but were not kept by the switch due to a lack of AVB buffers
	*      MSD_STATS_InDaUnknown -
	*						The number of good frames received that did not have a Destination Address ¡®hit¡¯ from the ATU
	*						and are not discarded or filtered
	*      MSD_STATS_InMGMT -
	*						The number of good frames received that are considered to be Management frames and are not discared
	*						size is legal and its CRC is good or it was firced good by register
	*	   MSD_STATS_OutQueue0 -
	*						The number of frames that egress this port from Queue0
	*      MSD_STATS_OutQueue1 -
	*						The number of frames that egress this port from Queue1
	*      MSD_STATS_OutQueue2 -
	*						The number of frames that egress this port from Queue2
	*      MSD_STATS_OutQueue3 -
	*						The number of frames that egress this port from Queue3
	*      MSD_STATS_OutQueue4 -
	*						The number of frames that egress this port from Queue4
	*      MSD_STATS_OutQueue5 -
	*						The number of frames that egress this port from Queue5
	*      MSD_STATS_OutQueue6 -
	*						The number of frames that egress this port from Queue6
	*      MSD_STATS_OutQueue7 -
	*						The number of frames that egress this port from Queue7
	*      MSD_STATS_OutCutThrough -
	*						The number of frames that egress this port from the Cut Through path
	*      MSD_STATS_OutOctetsA -
	*						The sum of lengths of all Ethernet frames sent from the AVB Class A Queue not including frames
	*						that are considered Management by ingress
	*      MSD_STATS_OutOctetsB -
	*						The sum of lengths of all Ethernet frames sent from the AVB Class B Queue not including frames
	*						that are considered Management by ingress
	*      MSD_STATS_OutYel -
	*						The number of Yellow frames that egressed this port
	*      MSD_STATS_OutDroppedYel -
	*						The number of Yellow frames not counted in InDiscards that are ¡®head dropped¡¯ from an egress port¡¯s
	*						queues and the number of Yellow frames¡¯s ¡®tail dropped¡¯ from an egress port¡¯s queues due to Queue
	*						Controller¡¯s queue limits
	*      MSD_STATS_OutDiscards -
	*						The number of Green frames not counted in Indiscards that are ¡®head dropped¡¯ from an egress port¡¯s
	*						queues and the number of Green frames¡¯s ¡®tail dropped¡¯ from an egress port¡¯s queues due to Queue
	*						Controller¡¯s queue limits
	*      MSD_STATS_OutMGMT -
	*						The number of frames transmitted that were considered to be Management frames
	*/
#define MSD_TYPE_BANK 0x400
	typedef enum
	{
		/* Bank 0 */
		MSD_STATS_InGoodOctetsLo = 0,
		MSD_STATS_InGoodOctetsHi,
		MSD_STATS_InBadOctets,

		MSD_STATS_OutFCSErr,
		MSD_STATS_InUnicasts,
		MSD_STATS_Deferred,             /* offset 5 */
		MSD_STATS_InBroadcasts,
		MSD_STATS_InMulticasts,
		MSD_STATS_64Octets,
		MSD_STATS_127Octets,
		MSD_STATS_255Octets,            /* offset 10 */
		MSD_STATS_511Octets,
		MSD_STATS_1023Octets,
		MSD_STATS_MaxOctets,
		MSD_STATS_OutOctetsLo,
		MSD_STATS_OutOctetsHi,
		MSD_STATS_OutUnicasts,          /* offset 16 */
		MSD_STATS_Excessive,
		MSD_STATS_OutMulticasts,
		MSD_STATS_OutBroadcasts,
		MSD_STATS_Single,
		MSD_STATS_OutPause,
		MSD_STATS_InPause,
		MSD_STATS_Multiple,
		MSD_STATS_InUndersize,          /* offset 24 */
		MSD_STATS_InFragments,
		MSD_STATS_InOversize,
		MSD_STATS_InJabber,
		MSD_STATS_InRxErr,
		MSD_STATS_InFCSErr,
		MSD_STATS_Collisions,
		MSD_STATS_Late,                 /* offset 31 */
		/* Bank 1 */
		MSD_STATS_InDiscards = MSD_TYPE_BANK + 0x00,
		MSD_STATS_InFiltered = MSD_TYPE_BANK + 0x01,
		MSD_STATS_InAccepted = MSD_TYPE_BANK + 0x02,
		MSD_STATS_InBadAccepted = MSD_TYPE_BANK + 0x03,
		MSD_STATS_InGoodAvbClassA = MSD_TYPE_BANK + 0x04,
		MSD_STATS_InGoodAvbClassB = MSD_TYPE_BANK + 0x05,
		MSD_STATS_InBadAvbClassA = MSD_TYPE_BANK + 0x06,
		MSD_STATS_InBadAvbClassB = MSD_TYPE_BANK + 0x07,
		MSD_STATS_TCAMCounter0 = MSD_TYPE_BANK + 0x08,
		MSD_STATS_TCAMCounter1 = MSD_TYPE_BANK + 0x09,
		MSD_STATS_TCAMCounter2 = MSD_TYPE_BANK + 0x0a,
		MSD_STATS_TCAMCounter3 = MSD_TYPE_BANK + 0x0b,
		MSD_STATS_InDroppedAvbA = MSD_TYPE_BANK + 0x0c,
		MSD_STATS_InDroppedAvbB = MSD_TYPE_BANK + 0x0d,
		MSD_STATS_InDaUnknown = MSD_TYPE_BANK + 0x0e,
		MSD_STATS_InMGMT = MSD_TYPE_BANK + 0x0f,
		MSD_STATS_OutQueue0 = MSD_TYPE_BANK + 0x10,
		MSD_STATS_OutQueue1 = MSD_TYPE_BANK + 0x11,
		MSD_STATS_OutQueue2 = MSD_TYPE_BANK + 0x12,
		MSD_STATS_OutQueue3 = MSD_TYPE_BANK + 0x13,
		MSD_STATS_OutQueue4 = MSD_TYPE_BANK + 0x14,
		MSD_STATS_OutQueue5 = MSD_TYPE_BANK + 0x15,
		MSD_STATS_OutQueue6 = MSD_TYPE_BANK + 0x16,
		MSD_STATS_OutQueue7 = MSD_TYPE_BANK + 0x17,
		MSD_STATS_OutCutThrough = MSD_TYPE_BANK + 0x18,
		MSD_STATS_OutOctetsA = MSD_TYPE_BANK + 0x1a,
		MSD_STATS_OutOctetsB = MSD_TYPE_BANK + 0x1b,
		MSD_STATS_OutYel = MSD_TYPE_BANK + 0x1c,
		MSD_STATS_OutDroppedYel = MSD_TYPE_BANK + 0x1d,
		MSD_STATS_OutDiscards = MSD_TYPE_BANK + 0x1e,
		MSD_STATS_OutMGMT = MSD_TYPE_BANK + 0x1f

	} MSD_STATS_COUNTERS;

	/*
	*  typedef: struct MSD_STATS_COUNTER_SET
	*
	*  Description: MIB counter
	*
	*  Fields:
	*      InGoodOctetsLo -
	*						The lower 32-bits of the 64-bit InGoodOctets counter
	*	   InGoodOctetsHi -
	*                       The upper 32-bits of the 64-bit InGoodOctets counter
	*      InBadOctets -
	*                       The sum of lengths of all bad Ethrnet frames received
	*      OutFCSErr -
	*                       The number of frames transmitted with an invalid FCS
	*      InUnicasts -
	*                       The number of good frames received that have a Unicast destination MAC address
	*      Deferred -
	*                       The total number of successfully transmitted frames that experienced no collisions but are delayed
	*      InBroadcasts -
	*                       The number of good frames received that have a Broadcast destination MAC address
	*      InMulticasts -
	*                       The number of good frames received that have a Multicast destination MAC address
	*      Octets64 -
	*						Total frames received with a length of exactly 64 octets, including those with errors
	*      Octets127 -
	*						Total frames received with a length of exactly 65 and 127 octets, including those with errors
	*      Octets255 -
	*						Total frames received with a length of exactly 128 and 255 octets, including those with errors
	*      Octets511 -
	*						Total frames received with a length of exactly 256 and 511 octets, including those with errors
	*      Octets1023 -
	*						Total frames received with a length of exactly 1024 and MaxSize octets, including those with errors
	*      OctetsMax -
	*						The number of good frames received that have a Unicast destination MAC address
	*      OutOctetsLo -
	*						The lower 32-bits of the 64-bit OutOctets counter
	*      OutOctetsHi -
	*						The upper 32-bits of the 64-bit OutOctets counter
	*      OutUnicasts -
	*						The number of frames sent that have a Unicast destination MAC address
	*      Excessive -
	*						The number frames dropped in the transmit MAC.
	*      OutMulticasts -
	*						The number of good frames sent that have a Multicast destination MAC address
	*      OutBroadcasts -
	*						The number of good frames sent that have a Broadcast destination MAC address
	*      Single -
	*						The total number of successfully transmitted framnes that experienced exactly one collision
	*      OutPause -
	*						The number of Flow Control frames sent
	*      InPause -
	*						The number of Good frames received that have a Pause destination MAC address
	*      Multiple -
	*						The total number of successfully transmitted frames that experienced more than one collision.
	*      InUndersize -
	*						Total frames received with a length of less than 64 octets but with a valid FCS
	*      InFragments -
	*						Total frames received with a length of less than 64 octets and an invalid FCS
	*      InOversize -
	*						Total frames received with a length of less than MaxSize octets but with a valid FCS
	*      InJabber -
	*						Total frames received with a length of less than MaxSize octets but with an invalid FCS
	*      InRxErr -
	*						Total frames received with an RxErr signal from the PHY
	*      InFCSErr -
	*						Total frames received with a CRC error not counted in InFragments, InJabber or InRxErr.
	*      Collisions -
	*						The number of collision events seen by the MAC not including those counted in
	*						Single, Multiple, Excessive, or Late
	*      Late -
	*						The number of times a collision is detected later than 52 bits-times into the
	*						transmission of a frame
	*      InDiscards -
	*						The number of good, non-filtered, frames that are received but can¡¯t be forwarded
	*						due to a lack of buffer memory
	*      InFiltered -
	*						The number of good frames that were not forwarded due to policy filtering rules
	*						such as but not limited to: 802.1Q Mode, Tagging mode, SA fitering e.t.c
	*      InAccepted -
	*						The number of good frames that are not policy filtered nor discarded due to an error
	*						and made it throuth the Ingress amd is presented to the Queue Controller
	*	   InBadAccepted -
	*						The number of frames with a CRC error that is not filtered nor discarded
	*      InGoodAvbClassA -
	*						The number of good AVB frames received that have a Priority Code Point for
	*						Class A that are not Undersize nor Oversize and are not discarded or fitered
	*      InGoodAvbClassB -
	*						The number of good AVB frames received that have a Priority Code Point for
	*						Class B that are not Undersize nor Oversize and are not discarded or fitered
	*      InBadAvbClassA -
	*						The number of bad AVB frames received that have a Priority Code Point for
	*						Class A that are not Undersize nor Oversize
	*	   InBadAvbClassB -
	*						The number of bad AVB frames received that have a Priority Code Point for
	*						Class B that are not Undersize nor Oversize
	*      TCAMCounter0 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=0 and are not discarded or filtered
	*      TCAMCounter1 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=1 and are not discarded or filtered
	*      TCAMCounter2 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=2 and are not discarded or filtered
	*	   TCAMCounter3 -
	*						The number of good frames received that have a TCAM Hit on a TCAM Entry that
	*						has its IncTcamCtr bit set to a one and its FlowID[7:6]=3 and are not discarded or filtered
	*      InDroppedAvbA -
	*						The number of good AVB frames received that have a Priority Code Point for Class A that are not
	*						Undersize nor Oversize and are not discarded or filtered but were not kept by the switch due to a lack of AVB buffers
	*      InDroppedAvbB -
	*						The number of good AVB frames received that have a Priority Code Point for Class B that are not
	*						Undersize nor Oversize and are not discarded or filtered but were not kept by the switch due to a lack of AVB buffers
	*      InDaUnknown -
	*						The number of good frames received that did not have a Destination Address ¡®hit¡¯ from the ATU
	*						and are not discarded or filtered
	*      InMGMT -
	*						The number of good frames received that are considered to be Management frames and are not discared
	*						size is legal and its CRC is good or it was firced good by register
	*	   OutQueue0 -
	*						The number of frames that egress this port from Queue0
	*      OutQueue1 -
	*						The number of frames that egress this port from Queue1
	*      OutQueue2 -
	*						The number of frames that egress this port from Queue2
	*      OutQueue3 -
	*						The number of frames that egress this port from Queue3
	*      OutQueue4 -
	*						The number of frames that egress this port from Queue4
	*      OutQueue5 -
	*						The number of frames that egress this port from Queue5
	*      OutQueue6 -
	*						The number of frames that egress this port from Queue6
	*      OutQueue7 -
	*						The number of frames that egress this port from Queue7
	*      OutCutThrough -
	*						The number of frames that egress this port from the Cut Through path
	*      reserved_19 -
	*						Reserved for future use
	*      OutOctetsA -
	*						The sum of lengths of all Ethernet frames sent from the AVB Class A Queue not including frames
	*						that are considered Management by ingress
	*      OutOctetsB -
	*						The sum of lengths of all Ethernet frames sent from the AVB Class B Queue not including frames
	*						that are considered Management by ingress
	*      OutYel -
	*						The number of Yellow frames that egressed this port
	*      OutDroppedYel -
	*						The number of Yellow frames not counted in InDiscards that are ¡®head dropped¡¯ from an egress port¡¯s
	*						queues and the number of Yellow frames¡¯s ¡®tail dropped¡¯ from an egress port¡¯s queues due to Queue
	*						Controller¡¯s queue limits
	*      OutDiscards -
	*						The number of Green frames not counted in Indiscards that are ¡®head dropped¡¯ from an egress port¡¯s
	*						queues and the number of Green frames¡¯s ¡®tail dropped¡¯ from an egress port¡¯s queues due to Queue
	*						Controller¡¯s queue limits
	*      OutMGMT -
	*						The number of frames transmitted that were considered to be Management frames
	*/
	typedef struct _MSD_STATS_COUNTER_SET
	{
		/* Bank 0 */
		MSD_U32    InGoodOctetsLo;     /* offset 0 */
		MSD_U32    InGoodOctetsHi;     /* offset 1 */
		MSD_U32    InBadOctets;        /* offset 2 */
		MSD_U32    OutFCSErr;          /* offset 3 */
		MSD_U32    InUnicasts;         /* offset 4 */
		MSD_U32    Deferred;           /* offset 5 */
		MSD_U32    InBroadcasts;       /* offset 6 */
		MSD_U32    InMulticasts;       /* offset 7 */
		/*
		Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
		(refer to Histogram Mode)
		*/
		MSD_U32    Octets64;         /* 64 Octets, offset 8 */
		MSD_U32    Octets127;        /* 65 to 127 Octets, offset 9 */
		MSD_U32    Octets255;        /* 128 to 255 Octets, offset 10 */
		MSD_U32    Octets511;        /* 256 to 511 Octets, offset 11 */
		MSD_U32    Octets1023;       /* 512 to 1023 Octets, offset 12 */
		MSD_U32    OctetsMax;        /* 1024 to Max Octets, offset 13 */
		MSD_U32    OutOctetsLo;      /* offset 14 */
		MSD_U32    OutOctetsHi;      /* offset 15 */
		MSD_U32    OutUnicasts;      /* offset 16 */
		MSD_U32    Excessive;        /* offset 17 */
		MSD_U32    OutMulticasts;    /* offset 18 */
		MSD_U32    OutBroadcasts;    /* offset 19 */
		MSD_U32    Single;           /* offset 20 */

		MSD_U32    OutPause;         /* offset 21 */
		MSD_U32    InPause;          /* offset 22 */
		MSD_U32    Multiple;         /* offset 23 */
		MSD_U32    InUndersize;        /* offset 24 */
		MSD_U32    InFragments;        /* offset 25 */
		MSD_U32    InOversize;         /* offset 26 */
		MSD_U32    InJabber;           /* offset 27 */
		MSD_U32    InRxErr;          /* offset 28 */
		MSD_U32    InFCSErr;         /* offset 29 */
		MSD_U32    Collisions;       /* offset 30 */
		MSD_U32    Late;             /* offset 31 */
		/* Bank 1 */
		MSD_U32    InDiscards;       /* offset 0x00 */
		MSD_U32    InFiltered;       /* offset 0x01 */
		MSD_U32    InAccepted;       /* offset 0x02 */
		MSD_U32    InBadAccepted;    /* offset 0x03 */
		MSD_U32    InGoodAvbClassA;  /* offset 0x04 */
		MSD_U32    InGoodAvbClassB;  /* offset 0x05 */
		MSD_U32    InBadAvbClassA;  /* offset 0x06 */
		MSD_U32    InBadAvbClassB;  /* offset 0x07 */
		MSD_U32    TCAMCounter0;     /* offset 0x08 */
		MSD_U32    TCAMCounter1;     /* offset 0x09 */
		MSD_U32    TCAMCounter2;     /* offset 0x0a */
		MSD_U32    TCAMCounter3;     /* offset 0x0b */
		MSD_U32    InDroppedAvbA;    /* offset 0x0c */
		MSD_U32    InDroppedAvbB;    /* offset 0x0d */
		MSD_U32    InDaUnknown;     /* offset 0x0e */
		MSD_U32    InMGMT;           /* offset 0x0f */
		MSD_U32    OutQueue0;        /* offset 0x10 */
		MSD_U32    OutQueue1;        /* offset 0x11 */
		MSD_U32    OutQueue2;        /* offset 0x12 */
		MSD_U32    OutQueue3;        /* offset 0x13 */
		MSD_U32    OutQueue4;        /* offset 0x14 */
		MSD_U32    OutQueue5;        /* offset 0x15 */
		MSD_U32    OutQueue6;        /* offset 0x16 */
		MSD_U32    OutQueue7;        /* offset 0x17 */
		MSD_U32    OutCutThrough;    /* offset 0x18 */
		MSD_U32    reserved_19;     /* offset 0x19 */
		MSD_U32    OutOctetsA;       /* offset 0x1a */
		MSD_U32    OutOctetsB;       /* offset 0x1b */
		MSD_U32    OutYel;           /* offset 0x1c */
		MSD_U32    OutDroppedYel;    /* offset 0x1d */
		MSD_U32    OutDiscards;      /* offset 0x1e */
		MSD_U32    OutMGMT;          /* offset 0x1f */

	}MSD_STATS_COUNTER_SET;

	typedef struct _MSD_STATS_COUNTER_SET_LEGACY
	{
		/* Bank 0 */
		MSD_U32    InGoodOctetsLo;     /* offset 0 */
		MSD_U32    InGoodOctetsHi;     /* offset 1 */
		MSD_U32    InBadOctets;        /* offset 2 */
		MSD_U32    OutFCSErr;          /* offset 3 */
		MSD_U32    InUnicasts;         /* offset 4 */
		MSD_U32    Deferred;           /* offset 5 */
		MSD_U32    InBroadcasts;       /* offset 6 */
		MSD_U32    InMulticasts;       /* offset 7 */
		/*
		Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
		(refer to Histogram Mode)
		*/
		MSD_U32    Octets64;         /* 64 Octets, offset 8 */
		MSD_U32    Octets127;        /* 65 to 127 Octets, offset 9 */
		MSD_U32    Octets255;        /* 128 to 255 Octets, offset 10 */
		MSD_U32    Octets511;        /* 256 to 511 Octets, offset 11 */
		MSD_U32    Octets1023;       /* 512 to 1023 Octets, offset 12 */
		MSD_U32    OctetsMax;        /* 1024 to Max Octets, offset 13 */
		MSD_U32    OutOctetsLo;      /* offset 14 */
		MSD_U32    OutOctetsHi;      /* offset 15 */
		MSD_U32    OutUnicasts;      /* offset 16 */
		MSD_U32    Excessive;        /* offset 17 */
		MSD_U32    OutMulticasts;    /* offset 18 */
		MSD_U32    OutBroadcasts;    /* offset 19 */
		MSD_U32    Single;           /* offset 20 */

		MSD_U32    OutPause;         /* offset 21 */
		MSD_U32    InPause;          /* offset 22 */
		MSD_U32    Multiple;         /* offset 23 */
		MSD_U32    InUndersize;      /* offset 24 */
		MSD_U32    InFragments;      /* offset 25 */
		MSD_U32    InOversize;       /* offset 26 */
		MSD_U32    InJabber;         /* offset 27 */
		MSD_U32    InRxErr;          /* offset 28 */
		MSD_U32    InFCSErr;         /* offset 29 */
		MSD_U32    Collisions;       /* offset 30 */
		MSD_U32    Late;             /* offset 31 */
	}MSD_STATS_COUNTER_SET_LEGACY;

	/*
	* typedef: enum MSD_HISTOGRAM_MODE
	*
	* Description: Enumeration of the histogram counters mode.
	*
	* Enumerations:
	*   MSD_COUNT_RX_ONLY - In this mode, Rx Histogram Counters are counted.
	*   MSD_COUNT_TX_ONLY - In this mode, Tx Histogram Counters are counted.
	*   MSD_COUNT_RX_TX   - In this mode, Rx and Tx Histogram Counters are counted.
	*/
	typedef enum
	{
		MSD_COUNT_RX_ONLY = 1,
		MSD_COUNT_TX_ONLY,
		MSD_COUNT_RX_TX
	} MSD_HISTOGRAM_MODE;
	/****************************************************************************/
	/* Exported MIBS Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported Queue Control Types			 			                        */
	/****************************************************************************/
#define MSD_MAX_QOS_WEIGHTS        128
	/*
	*  typedef: enum MSD_PORT_SCHED_MODE
	*
	*  Description: Enumeration of port scheduling mode
	*
	*  Fields:
	*
	*		   MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0 - use 33,25,17,12,6,3,2,1 weighted fair
	*										scheduling
	*	       MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0 - use a strict for priority 7 and weighted
	*										round robin for priorities 6,5,4,3,2,1 and 0
	*		   MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0 - use a strict for priority 7,6 and weighted
	*										round robin for priorities 5,4,3,2,1 and 0
	*		   MSD_PORT_SCHED_WRR_PRI4_3_2_1_0 - use a strict for priority 7,6,5 and weighted
	*										round robin for priorities 4,3,2,1 and 0
	*		   MSD_PORT_SCHED_WRR_PRI3_2_1_0 - use a strict for priority 7,6,5,4 and
	*										weighted round robin for priorities 3,2,1 and 0
	*		   MSD_PORT_SCHED_WRR_PRI2_1_0 - use a strict for priority 7,6,5,4,3 and
	*										weighted round robin for priorities 2,1 and 0
	*		   MSD_PORT_SCHED_WRR_PRI1_0 - use a strict for priority 7,6,5,4,3,2
	*										and weighted round robin for priorities 1 and 0
	*		   MSD_PORT_SCHED_STRICT_PRI_ALL - use a strict priority scheme for all priorities
	*/
	typedef enum
	{
		MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0 = 0x0,
		MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0,
		MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0,
		MSD_PORT_SCHED_WRR_PRI4_3_2_1_0,
		MSD_PORT_SCHED_WRR_PRI3_2_1_0,
		MSD_PORT_SCHED_WRR_PRI2_1_0,
		MSD_PORT_SCHED_WRR_PRI1_0,
		MSD_PORT_SCHED_STRICT_PRI_ALL
	} MSD_PORT_SCHED_MODE;

	/*
	* Typedef: struct MSD_QoS_WEIGHT
	*
	* Description: This structure is used for Programmable Round Robin Weights.
	*
	* Fields:
	*      len    - length of the valid queue data
	*      queue  - upto 128 queue data
	*/
	typedef struct
	{
		MSD_U32        len;
		MSD_U8         queue[MSD_MAX_QOS_WEIGHTS];
	} MSD_QoS_WEIGHT;
	/****************************************************************************/
	/* Exported Queue Control Types			 			                        */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported TCAM Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: enum MSD_TCAM_MODE
	*
	*  Description: Enumeration of TCAM mode
	*
	*  Fields:
	*      MSD_TCAM_MODE_DISABLE		- TCAM Disable (default)
	*      MSD_TCAM_MODE_ENABLE_48	- TCAM Enabled for 48 byte searches only
	*      MSD_TCAM_MODE_ENABLE_96	- TCAM Enabled for 48 byte and/or 96 bytes searches
	*/
	typedef enum
	{
		MSD_TCAM_MODE_DISABLE,
		MSD_TCAM_MODE_ENABLE_48,
		MSD_TCAM_MODE_ENABLE_96,
	} MSD_TCAM_MODE;

	/*
	*  typedef: enum MSD_TCAM_DPV_MODE
	*
	*  Description: Enumeration of TCAM mode
	*
	*  Fields:
	*      MSD_TCAM_DPV_MODE_DO_NOTHING		- The frame's DPV is not modified
	*      MSD_TCAM_DPV_MODE_AND_DPV		- Bit wise AND the frame's DPV with the TCAM entry's DPV data
	*      MSD_TCAM_DPV_MODE_OR_DPV			- Bit wise OR the frame's DPV with the TCAM entry's DPV data
	*	   MSD_TCAM_DPV_MODE_OVERRIDE_DPV   - Replace the frame's DPV with the TCAM entry's DPV data
	*/
	typedef enum
	{
		MSD_TCAM_DPV_MODE_DO_NOTHING = 0,
		MSD_TCAM_DPV_MODE_AND_DPV = 1,
		MSD_TCAM_DPV_MODE_OR_DPV = 2,
		MSD_TCAM_DPV_MODE_OVERRIDE_DPV = 3
	} MSD_TCAM_DPV_MODE;

	/*
	* Typedef: struct MSD_TCAM_DATA
	*
	* Description: TCAM data.
	*
	* Fields:
	*      frameType		- Frame type
	*      frameTypeMask	- Mask for frame type
	*      spv				- Source Port Vector
	*      spvMask			- Mask for spv (Source Port Vector)
	*      ppri				- Provider pri
	*      ppriMask			- Mask for ppri (Provider pri)
	*      pvid				- Provider VID
	*      pvidMask			- Mask for pvid (Provider VID)
	*      frameOctet		- Frame octet
	*      frameOctetMask	- Mask for frame octet
	*	   continu			- Continue this TCAM entry
	*      interrupt		- Interrupt on a TCAM hit
	*      IncTcamCtr		- Increment the port¡¯s TCAM Counter
	*      vidOverride		- VID Override Enable
	*      vidData			- VID Override Data
	*      nextId			- Next Index
	*      qpriOverride		- QPRI Override Enable
	*      qpriData			- QPRI Override Data
	*      fpriOverride		- FPRI Override Enable
	*      fpriData			- FPRI Override Data
	*      dpvSF			- Source Port Filter
	*      dpvData			- DPV Data
	*      dpvMode			- DPV Mode
	*      colorMode		- Color Mode
	*      vtuPageOverride	- VTU Page Override
	*      vtuPage			- VTU Page
	*      unKnownFilter	- Unknown Filtering
	*      egActPoint		- Egress Action Pointer
	*      ldBalanceOverride - Load Balance Override Enable
	*      ldBalanceData	- Load Balance Override Data
	*      DSCPOverride		- DSCP Override Enable
	*      DSCP				- DSCP Data
	*      factionOverride	- Frame Action Override Enable
	*      factionData		- Frame Action Data
	*/
	typedef struct {
		/* Key & Mask */
		MSD_U8        frameType;
		MSD_U8        frameTypeMask;
		MSD_U16       spv;
		MSD_U16       spvMask;
		MSD_U8        ppri;
		MSD_U8        ppriMask;
		MSD_U16       pvid;
		MSD_U16       pvidMask;

		/* Ethernet Frame Content*/
		MSD_U8        frameOctet[48];
		MSD_U8        frameOctetMask[48];

		/* Ingress Action */
		MSD_U8        continu;
		MSD_U8        interrupt;
		MSD_U8        IncTcamCtr;
		MSD_BOOL      vidOverride;
		MSD_U16       vidData;

		MSD_U8        nextId;
		MSD_BOOL      qpriOverride;
		MSD_U8        qpriData;
		MSD_BOOL      fpriOverride;
		MSD_U8        fpriData;

		MSD_U8        dpvSF;
		MSD_U32       dpvData;

		MSD_TCAM_DPV_MODE   dpvMode;
		MSD_U8        colorMode;
		MSD_BOOL      vtuPageOverride;
		MSD_U8        vtuPage;
		MSD_U8        unKnownFilter;
		MSD_U8        egActPoint;

		MSD_BOOL      ldBalanceOverride;
		MSD_U8        ldBalanceData;
		MSD_BOOL      DSCPOverride;
		MSD_U8        DSCP;

		MSD_BOOL      factionOverride;
		MSD_U16       factionData;
	}  MSD_TCAM_DATA;

	/*
	* Typedef: struct MSD_TCAM_EGR_DATA
	*
	* Description: Egress TCAM data.
	*
	* Fields:
	*	   frameModeOverride	- Egress Frame Mode Override Enable
	*      frameMode			- Egress Frame Mode Data
	*      tagModeOverride		- Egress tag Mode Override Enabled
	*      tagMode				- Egress tag Mode Data
	*      daMode				- DA Mode
	*      saMode				- SA Mode
	*      egVidModeOverride	- Egress VID Override Enable
	*      egVidMode			- Egress VID Mode
	*      egVidData			- Egress VID Data
	*      egDSCPMode			- DSCP Mode
	*      egDSCP				- Egress DSCP
	*      egfpriModeOverride	- Egress FPRI Override Enable
	*      egfpriMode			- Egress FPRI Mode
	*      egEC					- Egress CFI/DE Override Data
	*      egFPRI				- Egress FPRI Override Data
	*/
	typedef struct
	{
		MSD_BOOL      frameModeOverride;
		MSD_U8        frameMode;
		MSD_BOOL      tagModeOverride;
		MSD_U8        tagMode;
		MSD_U8        daMode;
		MSD_U8        saMode;

		MSD_BOOL      egVidModeOverride;
		MSD_U8        egVidMode;
		MSD_U16       egVidData;

		MSD_U8        egDSCPMode;
		MSD_U8        egDSCP;
		MSD_BOOL      egfpriModeOverride;
		MSD_U8        egfpriMode;
		MSD_U8        egEC;
		MSD_U8        egFPRI;

	}  MSD_TCAM_EGR_DATA;

    /*
    *  typedef: enum MSD_TCAM_PKT_TYPE
    *
    *  Description: Enumeration of TCAM mapping packet type for advance setting
    *
    *  Fields:
    *      TCAM_TYPE_IPV4_TCP   - TCP over IPv4 packet
    *      TCAM_TYPE_IPV4_UDP   - UDP over IPv4 packet
    *      TCAM_TYPE_IPV6_TCP   - TCP over IPv6 packet
    *      TCAM_TYPE_IPV6_UDP   - UDP over IPv6 packet
    */
    typedef enum
    {
        MSD_TCAM_TYPE_IPV4_TCP,
        MSD_TCAM_TYPE_IPV4_UDP,
        MSD_TCAM_TYPE_IPV6_TCP,
        MSD_TCAM_TYPE_IPV6_UDP
    } MSD_TCAM_PKT_TYPE;

    /*Typedef: struct MSD_TCAM_ENT_KEY
    *
    * Description : TCAM entry data for key/mask.
    *
    * Fields :
    *frameType - Frame type
    *      frameType - Frame type
    *      spv  - Source Port Vector
    *      ppri - Provider pri
    *      pvid - Provider VID
    *
    */
    typedef struct
    {
        MSD_U8       frameType;
        MSD_U16      spv;
        MSD_U8       ppri;
        MSD_U16      pvid;
    } MSD_TCAM_ENT_KEY;

    /*Typedef: struct MSD_TCAM_ENT_IPV4_UNT
    *
    * Description : IPv4 header format
    *
    * Fields :
    *      version  - version number
    *      ihl      - header length
    *      tos      - type of service
    *      length       - total length
    *      id           - identification
    *      flag         - fragment flag
    *      fragOffset   - fragmentation offset
    *      ttl          - time to live
    *      protocol     - up level protocol type
    *      hdrChkSum    - header checksum
    *      sip      - source ip address
    *      dip      - destination ip address
    */
    typedef struct
    {
        MSD_U8       version;
        MSD_U8       ihl;
        MSD_U8       tos;
        MSD_U16      length;
        MSD_U16      id;
        MSD_U8       flag;
        MSD_U16      fragOffset;
        MSD_U8       ttl;
        MSD_U8       protocol;
        MSD_U16      hdrChkSum;
        MSD_U8       sip[4];
        MSD_U8       dip[4];
    } MSD_TCAM_ENT_IPV4_UNT;

    /*Typedef: struct MSD_TCAM_ENT_IPV6_UNT
    *
    * Description : IPv6 header format
    *
    * Fields :
    *      version  - version number
    *      tc       - traffic class
    *      flowLbl  - flow label
    *      payloadLen - payload length
    *      nextHdr    - next header
    *      hopLimit   - hop limit
    *      sip      - source ip address
    *      dip      - destination ip address
    */
    typedef struct
    {
        MSD_U8 version;
        MSD_U8 tc;
        MSD_U32 flowLbl;
        MSD_U16 payloadLen;
        MSD_U8 nextHdr;
        MSD_U8 hopLimit;
        MSD_U16 sip[8];
        MSD_U16 dip[8];
    } MSD_TCAM_ENT_IPV6_UNT;

    /*Typedef: struct TCAM_ENT_TCP_UNT
    *
    * Description : TCP header format
    *
    * Fields :
    *    srcPort    - source port
    *    destPort   - destonation port
    *    seqNum     - sequence number
    *    ackNum     - acknowledgment number
    *    dataOffset - data offset/header length
    *    flags      - contral flag
    *    windowSize - window size
    *    chkSum     - checksum
    *    urgPtr     - urgent pointer
    */
    typedef struct
    {
        MSD_U16 srcPort;
        MSD_U16 destPort;
        MSD_U32 seqNum;
        MSD_U32 ackNum;
        MSD_U8  offset;
        MSD_U16 flags;
        MSD_U16 windowSize;
        MSD_U16 chkSum;
        MSD_U16 urgPtr;
    } MSD_TCAM_ENT_TCP_UNT;

    /*Typedef: struct TCAM_ENT_UDP_UNT
    *
    * Description : UDP header format
    *
    * Fields :
    *      srcPort   - source port
    *      destPort  - destonation port
    *      length    - length
    *      chkSum    - checksum
    */
    typedef struct
    {
        MSD_U16 srcPort;
        MSD_U16 destPort;
        MSD_U16 length;
        MSD_U16 chkSum;
    } MSD_TCAM_ENT_UDP_UNT;

    /*Typedef: struct TCAM_ENT_IPV4_UDP_UNT
    *
    * Description : UDP over IPv4 packet header format
    *
    * Fields :
    *      ipv4 - IPv4 header format
    *      udp  - UDP header format
    */
    typedef struct
    {
        MSD_TCAM_ENT_IPV4_UNT  ipv4;
        MSD_TCAM_ENT_UDP_UNT   udp;
    } MSD_TCAM_ENT_IPV4_UDP_UNT;

    /*Typedef: struct TCAM_ENT_IPV4_TCP_UNT
    *
    * Description : TCP over IPv4 packet header format
    *
    * Fields :
    *      ipv4 - IPv4 header format
    *      tcp  - TCP header format
    */
    typedef struct
    {
        MSD_TCAM_ENT_IPV4_UNT  ipv4;
        MSD_TCAM_ENT_TCP_UNT  tcp;
    } MSD_TCAM_ENT_IPV4_TCP_UNT;

    /*Typedef: struct TCAM_ENT_IPV6_UDP_UNT
    *
    * Description : UDP over IPv6 packet header format
    *
    * Fields :
    *      ipv6 - IPv6 header format
    *      udp  - UDP header format
    */
    typedef struct
    {
        MSD_TCAM_ENT_IPV6_UNT  ipv6;
        MSD_TCAM_ENT_UDP_UNT  udp;
    } MSD_TCAM_ENT_IPV6_UDP_UNT;

    /*Typedef: struct TCAM_ENT_IPV6_TCP_UNT
    *
    * Description : TCP over IPv6 packet header format
    *
    * Fields :
    *      ipv6 - IPv6 header format
    *      tcp  - TCP header format
    */
    typedef struct
    {
        MSD_TCAM_ENT_IPV6_UNT  ipv6;
        MSD_TCAM_ENT_TCP_UNT  tcp;
    } MSD_TCAM_ENT_IPV6_TCP_UNT;

    /*Typedef: union MSD_TCAM_ENT_PKT_FORMAT_UNT
    *
    * Description : union struct for different packet format defination.
    *
    * Fields :
    *frameType - Frame type
    *      ipv4Tcp   - TCP over IPv4 packet header format
    *      ipv4Udp   - UDP over IPv4 packet header format
    *      ipv6Tcp   - TCP over IPv6 packet header format
    *      ipv6Udp   - UDP over IPv6 packet header format
    */
    typedef union
    {
        MSD_TCAM_ENT_IPV4_TCP_UNT ipv4Tcp;
        MSD_TCAM_ENT_IPV4_UDP_UNT ipv4Udp;
        MSD_TCAM_ENT_IPV6_TCP_UNT ipv6Tcp;
        MSD_TCAM_ENT_IPV6_UDP_UNT ipv6Udp;
    } MSD_TCAM_ENT_PKT_FORMAT_UNT;

    /*Typedef: struct MSD_TCAM_ENT_PKT_UNT
    *
    * Description : TCAM entry data for packet data/mask.
    *
    * Fields :
    *frameType - Frame type
    *      da   - mac address DA
    *      sa   - mac address SA
    *      vlanTag   - vlan tag
    *      etherType - ether type
    *      pktUnit   - union struct for different packet type
    */
    typedef struct
    {
        MSD_ETHERADDR da;
        MSD_ETHERADDR sa;
        MSD_U32 vlanTag;
        MSD_U16 etherType;
        MSD_TCAM_ENT_PKT_FORMAT_UNT pktUnit;
    } MSD_TCAM_ENT_PKT_UNT;

    /*Typedef: struct MSD_TCAM_ENT_PKT_UNT
    *
    * Description : TCAM action data for advance setting
    *
    * Fields :
    *      interrupt - Interrupt on a TCAM hit
    *      IncTcamCtr - Increment the port¡¯s TCAM Counter
    *      vidOverride - VID Override Enable
    *      vidData - VID Override Data
    *      nextId - Next Index
    *      qpriOverride - QPRI Override Enable
    *      qpriData - QPRI Override Data
    *      fpriOverride - FPRI Override Enable
    *      fpriData - FPRI Override Data
    *      dpvSF - Source Port Filter
    *      dpvData - DPV Data
    *      dpvMode - DPV Mode
    *      colorMode - Color Mode
    *      vtuPageOverride - VTU Page Override
    *      vtuPage - VTU Page
    *      unKnownFilter - Unknown Filtering
    *      egActPoint - Egress Action Pointer
    *      ldBalanceOverride - Load Balance Override Enable
    *      ldBalanceData - Load Balance Override Data
    *      DSCPOverride - DSCP Override Enable
    *      DSCP - DSCP Data
    *      factionOverride - Frame Action Override Enable
    *      factionData - Frame Action Data
    */
    typedef struct
    {
        MSD_U8        interrupt;
        MSD_U8        IncTcamCtr;
        MSD_BOOL      vidOverride;
        MSD_U16       vidData;

        MSD_BOOL      qpriOverride;
        MSD_U8        qpriData;
        MSD_BOOL      fpriOverride;
        MSD_U8        fpriData;

        MSD_U8        dpvSF;
        MSD_U32       dpvData;

		MSD_TCAM_DPV_MODE   dpvMode;
        MSD_U8        colorMode;
        MSD_BOOL      vtuPageOverride;
        MSD_U8        vtuPage;
        MSD_U8        unKnownFilter;
        MSD_U8        egActPoint;

        MSD_BOOL      ldBalanceOverride;
        MSD_U8        ldBalanceData;
        MSD_BOOL      DSCPOverride;
        MSD_U8        DSCP;

        MSD_BOOL      factionOverride;
        MSD_U16       factionData;
    } MSD_TCAM_ENT_ACT;
	/****************************************************************************/
	/* Exported TCAM Types			 			                                */
	/****************************************************************************/

	/****************************************************************************/
	/* Exported RMU Types			 			                                */
	/****************************************************************************/
	/*isWaitOnBit or direct register read/write*/
#define MSD_RMU_WAIT_ON_BIT_FALSE 0
#define MSD_RMU_WAIT_ON_BIT_TRUE 1
	/*Op Code definition*/
#define MSD_RMU_REQ_OPCODE_READ  2
#define MSD_RMU_REQ_OPCODE_WRITE 1
#define MSD_RMU_WAIT_ON_BIT_VAL0 0
#define MSD_RMU_WAIT_ON_BIT_VAL1 3

	typedef struct {
		MSD_U8 isWaitOnBit;
		MSD_U8 opCode;
		MSD_U8 devAddr;
		MSD_U8 regAddr;
		MSD_U16 data;
	} MSD_RegCmd;
	/****************************************************************************/
	/* Exported RMU Types			 			                                */
	/****************************************************************************/
    typedef struct {
        MSD_U16 data[32];
    }MSD_RegList;
	/****************************************************************************/
	/* Exported PTP Types			 			                                */
	/****************************************************************************/
	/*
	*  typedef: struct PTP_PORT_INTERRUPT_EN
	*
	*  Description: Port PTP interrupt Enable/Disable
	*
	*  Fields:
	*      ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
	*      ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
	*/
	typedef struct
	{
		MSD_BOOL		ptpArrIntEn;
		MSD_BOOL		ptpDepIntEn;
	} MSD_PTP_PORT_INTERRUPT_EN;

	/*
	* Typedef: enum MSD_PTP_SPEC
	*
	* Description: Defines the PTP (Precise Time Protocol) SPEC type
	*
	* Fields:
	*      MSD_PTP_IEEE_1588       - IEEE 1588
	*      MSD_PTP_IEEE_802_1AS    - IEEE 802.1as
	*/
	typedef enum
	{
		MSD_PTP_IEEE_1588 = 0x0,
		MSD_PTP_IEEE_802_1AS = 0x1
	} MSD_PTP_SPEC;

	/*
	*  typedef: struct PTP_PORT_CONFIG
	*
	*  Description: PTP configuration parameters for each port
	*
	*  Fields:
	*      transSpec      - This is to differentiate between various timing protocols, 802.1as or IEEE1588.
	*      disTSpec		  - Disable Transport specific check
	*      etJump         - offset to Ether type start address in bytes
	*      ipJump         - offset to IP header start address counting from Ether type offset
	*      ptpArrIntEn    - PTP port arrival interrupt enable
	*      ptpDepIntEn    - PTP port departure interrupt enable
	*      disTSOverwrite - disable time stamp counter overwriting until the corresponding
	*                       timer valid bit is cleared
	*      filterAct	  - Filter 802.1 Act from LEDs
	*      hwAccPtp		  - Hardware Accelerated PTP
	*      kpFramwSa	  - Keep Frame SA
	*      ExthwAcc		  - External Hardware Acceleration enable
	*      arrTSMode	  - Arrival TS Mode
	*      arrLedCtrl	  - LED control for packets entering the device
	*      depLedCtrl	  - LED control for packets departing the device
	*/
	typedef struct
	{
		MSD_PTP_SPEC		transSpec;
		MSD_BOOL			disTSpec;
		MSD_U32			etJump;
		MSD_U32			ipJump;
		MSD_BOOL			ptpArrIntEn;
		MSD_BOOL			ptpDepIntEn;
		MSD_BOOL			disTSOverwrite;
		MSD_BOOL			filterAct;    /* Filter 802.1 Act from LEDs */
		MSD_BOOL			hwAccPtp;     /* Hardware Accelerated PTP */
		MSD_BOOL			kpFramwSa;    /* KS = Keep Frame SA */
		MSD_BOOL			ExthwAcc;    /*Exterbak Hardware Acceleration enable*/
		MSD_U16			arrTSMode;
		MSD_U8			arrLedCtrl; /*LED control for packets entering the device. */
		MSD_U8			depLedCtrl; /*LED control for packets departing the device. */
	}MSD_PTP_PORT_CONFIG;

	/*
	* Typedef: enum PTP_TIME
	*
	* Description: Defines the PTP Time to be read
	*
	* Fields:
	*      PTP_ARR0_TIME		- PTP Arrival Time 0
	*      PTP_ARR1_TIME		- PTP Arrival Time 1
	*      PTP_DEP_TIME			- PTP Departure Time
	*/
	typedef enum
	{
		MSD_PTP_ARR0_TIME = 0x0,
		MSD_PTP_ARR1_TIME = 0x1,
		MSD_PTP_DEP_TIME = 0x2
	} MSD_PTP_TIME;

	/*
	* Typedef: enum PTP_INT_STATUS
	*
	* Description: Defines the PTP Port interrupt status for time stamp
	*
	* Fields:
	*      PTP_INT_NORMAL        - No error condition occurred
	*      PTP_INT_OVERWRITE     - PTP logic has to process more than one PTP frame
	*                              that needs time stamping before the current read.
	*                              Only the latest one is saved.
	*      PTP_INT_DROP          - PTP logic has to process more than one PTP frame
	*                              that needs time stamping before the current read.
	*                              Only the oldest one is saved.
	*
	*/
	typedef enum
	{
		MSD_PTP_INT_NORMAL = 0x0,
		MSD_PTP_INT_OVERWRITE = 0x1,
		MSD_PTP_INT_DROP = 0x2
	} MSD_PTP_INT_STATUS;

	/*
	*  typedef: struct PTP_TS_STATUS
	*
	*  Description: PTP port status of time stamp
	*
	*  Fields:
	*      isValid        - time stamp is valid
	*      status         - time stamp error status
	*      timeStamped    - time stamp value of a PTP frame that needs to be time stamped
	*      ptpSeqId		  - sequence ID of the frame whose time stamp information has been captured
	*/
	typedef struct
	{
		MSD_BOOL    isValid;
		MSD_U32    timeStamped;
		MSD_U32    ptpSeqId;
		MSD_PTP_INT_STATUS    status;
	} MSD_PTP_TS_STATUS;

	/*
	*  typedef: struct PTP_GLOBAL_CONFIG
	*
	*  Description: PTP global configuration parameters
	*
	*  Fields:
	*      ptpEType    - PTP Ether Type
	*      msgIdTSEn   - Message IDs that needs time stamp
	*      tsArrPtr    - Time stamp arrival time counter pointer (either Arr0Time or Arr1Time)
	*/
	typedef struct
	{
		MSD_U32    ptpEType;
		MSD_U32    msgIdTSEn;
		MSD_U32    tsArrPtr;
	} MSD_PTP_GLOBAL_CONFIG;

	/*
	*  typedef: struct PTP_INTERRUPT_STATUS
	*
	*  Description: PTP global interrupt status structure after Opus
	*
	*  Fields:
	*     ptpTrigGenInt;                Trigger generate mode Interrupt
	*     ptpEventInt;                  Event Capture Interrupt
	*     ptpIntStPortVect;             Precise Time Protocol Interrupt for Ports.
	*/
	typedef struct
	{
		MSD_U8     ptpTrigGenInt;
		MSD_U8     ptpEventInt;
		MSD_U32    ptpIntStPortVect;
	} MSD_PTP_INTERRUPT_STATUS;

    /*
    *  typedef: struct PTP_MODE_CONFIG
    *
    *  Description: PTP mode configuration parameters
    *
    *  Fields:
    *      altScheme    - Alternate Scheme
    *      grandMstr    - Grand Master Enable.
    *      oneStep    - OneStep Enable.
    *      PTPMode    - PTP Mode as follows:
    *                   00 = Boundary Clock
    *                   01 = Peer to Peer Transparent Clock
    *                   10 = End to End Transparent Clock
    */
    typedef struct
    {
        MSD_U8    altScheme;
        MSD_U8    grandMstr;
        MSD_U8    oneStep;
        MSD_U8    PTPMode;
    } MSD_PTP_MODE_CONFIG;

    /*
    0: Set device as slave
    1: Set device as master
    */
    typedef enum
    {
        MSD_PTP_ROLE_SLAVE = 0,
        MSD_PTP_ROLE_MASTER = 1,
    }MSD_PTP_MASTER_SLAVE;

    /*
    *  typedef: struct PTP_TIME_ARRAY
    *
    *  Description: PTP Time Array consists of:
    *                       10 byte ToD time,
    *                       8 byte 1722 time,
    *                       4 byte Compensation,
    *                       1 byte Domain Number,
    *                       And a 1 bit Clock Valid.
    *
    *  Fields:
    *     todLoadPoint:        Time of Day Load Point
    *     clkValid:            Clock Valid bit
    *     domainNumber:        Domain Number, to be associated with the selected Time Array
    *     todNanoseconds:      Time Array Time of Day, Nano second 32 bits
    *     todSecondsLow:       Time Array Time of Day, second portion 0-31 bits
    *     todSecondsHigh:      Time Array Time of Day, second portion 32-47 bits
    *     Nanoseconds1722Low:  Time Array 1722 Time of Day in Nano seconds 0-31 bits
    *     Nanoseconds1722High: Time Array 1722 Time of Day in Nano seconds 32-63 bits
    *     todCompensation:     Time Array Time of Day Compensation 32 bits
    */
    typedef struct
    {
        MSD_U32    todLoadPoint;
        MSD_BOOL   clkValid;
        MSD_U8     domainNumber;
        MSD_U32    todNanoseconds;
        MSD_U32    todSecondsLow;
        MSD_U16    todSecondsHigh;
        MSD_U32    Nanoseconds1722Low;
        MSD_U32    Nanoseconds1722High;
        MSD_U32    todCompensation;
    }MSD_PTP_TIME_ARRAY;

    /*
    *  typedef: struct PTP_PULSE_STRUCT
    *
    *  Description: PTP 1pps Pulse parameters
    *
    *  Fields:
    *     ptpPulseWidth; Pulse Width for the 1 Pulse Per Second on the Second signal
    *     ptp1ppsWidthRange; Pulse Width Range for the 1 Pulse Per Second on the Second signal
    *     ptp1ppsPhase; Phase of the 1 Pulse Per Second on the Second signal
    *     ptp1ppsSelect; Select for the 1 Pulse Per Second on the Second signal.
    */
    typedef struct
    {
        MSD_U8     ptpPulseWidth;
        MSD_U8     ptp1ppsWidthRange;
        MSD_U8     ptp1ppsPhase;
        MSD_U8     ptp1ppsSelect;
    } MSD_PTP_PULSE_STRUCT;
	/****************************************************************************/
	/* Exported PTP Types			 			                                */
	/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __msApiTypes_h */
