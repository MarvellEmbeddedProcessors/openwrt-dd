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
* Pearl_msdPIRL.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PIRL functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Pearl_msdPIRL_h
#define __Pearl_msdPIRL_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported PIRL Types			 			                                   */
/****************************************************************************/

/*
 *  typedef: enum PEARL_MSD_PIRL_COUNT_MODE
 *
 *  Description: Enumeration of the port egress rate limit counting mode.
 *
 *  Enumerations:
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
    PEARL_MSD_PIRL_COUNT_FRAME = 0,
    PEARL_MSD_PIRL_COUNT_ALL_LAYER1,
    PEARL_MSD_PIRL_COUNT_ALL_LAYER2,
    PEARL_MSD_PIRL_COUNT_ALL_LAYER3
} PEARL_MSD_PIRL_COUNT_MODE;

/*
 *  typedef: struct PEARL_MSD_PIRL_CUSTOM_RATE_LIMIT
 *
 *  Description:  This structure includes all the necessary parameters used to set port 
 *				  ingress rate limit. It's the output of API 'gpirlCustomSetup' based on users' 
 *			 	  target rate and target burst allocation. And also, this structure can be used 
 *				  in API 'gpirlWriteResource' to configure IRL port resource. You can check IRL 
 *				  sample code for detail usage. If the real Ingress Rate Limit is too much off 
 *				  from the target rate, please contact FAE and gets the correct parameters.
 *
 *        isValid         - Reserved for future use
 *        ebsLimit        - Excess Burst Size limit ( 0 ~ 0xFFFFFF)
 *        cbsLimit        - Committed Burst Size limit 
 *        bktIncrement    - the amount of tokens that need to be added for each 
 *                          byte of packet information.
 *        bktRateFactor   - the amount of tokens that need to be decremented for each rate 
 *							resource decrement.
 *        IRLConstant     -	Constant can be any value from {7812500, 125000000, 
 *							62500000, 31250000,15625000, 3906250, 1953750, 976250} 
 *						    bit-based rate Limit:   Calculated Igress Rate(bps) = (IRLConstant * BRF)/BI
 *							frame-based rate Limit: Calculated Igress Rate(fps) = (IRLConstant * BRF)/(BI*8)
 *        Delta           - the error ratio between Calculated Rate and Target Rate((Rate - TgtRate)/TgtRate)
 *		  countMode	      - bytes to be counted for accounting
 *      				    PEARL_MSD_PIRL2_COUNT_FRAME,
 *                 		    PEARL_MSD_PIRL2_COUNT_ALL_LAYER1,
 *          			    PEARL_MSD_PIRL2_COUNT_ALL_LAYER2, or
 *            			    PEARL_MSD_PIRL2_COUNT_ALL_LAYER3
*/
typedef struct
{
    MSD_BOOL       isValid;
    MSD_U32        ebsLimit;
    MSD_U32        cbsLimit;
    MSD_U32        bktIncrement;
    MSD_U32        bktRateFactor;
    MSD_U32		  IRLConstant;
	MSD_DOUBLE	  Delta;

    /*Count Mode*/
    PEARL_MSD_PIRL_COUNT_MODE       countMode;

} PEARL_MSD_PIRL_CUSTOM_RATE_LIMIT;

/*
 *  typedef: enum PEARL_MSD_PIRL_FC_DEASSERT
 *
 *  Description: Enumeration of the port flow control de-assertion mode on PIRL.
 *
 *  Enumerations:
 *      PEARL_MSD_PIRL_FC_DEASSERT_EMPTY -
 *                De-assert when the ingress rate resource has become empty
 *        PEARL_MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
 *                De-assert when the ingress rate resource has enough room as
 *                specified by the CBSLimit.
 */
typedef enum
{
    PEARL_MSD_PIRL_FC_DEASSERT_EMPTY = 0,
    PEARL_MSD_PIRL_FC_DEASSERT_CBS_LIMIT
} PEARL_MSD_PIRL_FC_DEASSERT;

/*
 * Typedef: enum PEARL_MSD_PIRL_ACTION
 *
 * Description: Defines the Action that should be taken when
 *        there there are not enough tokens to accept the entire incoming frame
 *
 * Fields:
 *        Pearl_PIRL_ACTION_ACCEPT - accept the frame
 *        Pearl_PIRL_ACTION_USE_LIMIT_ACTION - use EBS Limit Action
 */
typedef enum
{
    Pearl_PIRL_ACTION_USE_LIMIT_ACTION = 0x0,
    Pearl_PIRL_ACTION_ACCEPT     = 0x1
} PEARL_MSD_PIRL_ACTION;


/*
 * Typedef: enum PEARL_MSD_EBS_LIMIT_ACTION
 *
 * Description: Defines the EBS Limit Action that should be taken when
 *        the incoming port information rate exceeds the EBS_Limit.
 *
 * Fields:
 *        Pearl_EBS_LIMIT_ACTION_DROP - drop packets
 *        Pearl_EBS_LIMIT_ACTION_FC   - send flow control packet
 */
typedef enum
{
    Pearl_EBS_LIMIT_ACTION_DROP     = 0x0,
    Pearl_EBS_LIMIT_ACTION_FC       = 0x1
} PEARL_MSD_EBS_LIMIT_ACTION;

/*
 *  typedef: struct PEARL_MSD_PIRL_DATA
 *
 *  Description: data structure for PIRL Bucket programing that is resource based.
 *
 *  Fields:
 *      bktTypeMask	- various traffic type to be counted
 *      tcamFlows	- indicate to account for tcam flows 0 to 3, but not TCP/UDP data                 
 *		priORpt		- determine the incoming frames that get rate limited using
 *                          this ingress rate resource.
 *                                  MSD_TRUE - typeMask OR priMask
 *                                  MSD_FALSE - typeMask AND priMask                                
 *		priSelect	- priority bit mask that each bit indicates one of the four
 *						queue priorities. Setting each one of these bits indicates
 *                 		that this particular rate resource is slated to account for
 *             			incoming frames with the enabled bits' priority.
 *      accountQConf    - account discarded frames due to queue congestion
 *      accountFiltered - account filtered frames. Account for All 
 *  	samplingMode    - sample one out of so many frames/bytes for a stream of frames
 *		fcMode			- flow control de-assertion mode when limitAction is
 *                          set to EBS_LIMIT_ACTION_FC.
 *                                PEARL_MSD_PIRL_FC_DEASSERT_EMPTY -
 *                                    De-assert when the ingress rate resource has 
 *                                    become empty.
 *                                PEARL_MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
 *                                    De-assert when the ingress rate resource has 
 *                                    enough room as specified by the CBSLimit.     
 *		actionMode		- action should be taken when there are not enough tokens
 *                          to accept the entire incoming frame
 *                                Pearl_PIRL_ACTION_ACCEPT - accept the frame
 *                                Pearl_PIRL_ACTION_USE_LIMIT_ACTION - use limitAction
 *		ebsLimitAction  - action should be taken when the incoming rate exceeds
 *                          the ebsLimit.
 *                                Pearl_EBS_LIMIT_ACTION_DROP - drop packets
 *                                Pearl_EBS_LIMIT_ACTION_FC   - send flow control packet 
 *                                
 *                                
 */
typedef struct
{
    /* frame types to be accounted*/
	MSD_U16 		bktTypeMask;
	MSD_BOOL		tcamFlows;
    MSD_BOOL		priORpt;
	MSD_U8		priSelect;

    /*condition for account*/
    MSD_BOOL		accountQConf;
	MSD_BOOL		accountFiltered;

	/*condition for account*/
	MSD_BOOL		samplingMode;
	   
	/*flow control*/
    PEARL_MSD_PIRL_FC_DEASSERT		fcMode;
    PEARL_MSD_PIRL_ACTION    actionMode;
    PEARL_MSD_EBS_LIMIT_ACTION ebsLimitAction;
	
	/*customer setup*/
	PEARL_MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;

} PEARL_MSD_PIRL_DATA;
/****************************************************************************/
/* Exported PIRL Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Pearl_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes PIRL Resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlInitialize
(
    IN  MSD_QD_DEV              *dev
);
MSD_STATUS Pearl_gpirlInitializeIntf
(
IN  MSD_QD_DEV              *dev
);

/*******************************************************************************
* Pearl_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- target logical port
*		irlRes - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlInitResource
(
    IN  MSD_QD_DEV              *dev,
	IN    MSD_U32               irlPort,
    IN    MSD_U32               irlRes
);
MSD_STATUS Pearl_gpirlInitResourceIntf
(
IN  MSD_QD_DEV              *dev,
IN    MSD_U32               irlPort,
IN    MSD_U32               irlRes
);
/*******************************************************************************
* Pearl_gpirlCustomSetup
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRF/Constant/Delta according to user 
*       specific target rate, target burst size and countMode.

*
* INPUTS:
*       tgtRate	- target rate(units is fps when countMode = MSD_PIRL_COUNT_FRAME,
*				  otherwise units is bps)
*       tgtBstSize	- target burst size(units is Byte)
*       customSetup - Constant and countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRF/Constant/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		As the input for structure 'MSD_PIRL_CUSTOM_RATE_LIMIT', Constant can be 0 or any  
*		value from {7812500, 125000000, 62500000, 31250000, 15625000, 3906250, 1953750, 976250}.
*		If Constant is 0, this API will automatically calculate the best IRL Constant 
*		which means Delta is small enough or 0. However, since Constant is Global for all
*       Resource of all ports, user can specify fixed Constant to gurantee that the Constant is 
*		consistent across all existing resources.
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlCustomSetup
(
    IN    MSD_U32	   	tgtRate,
    IN    MSD_U32	   	tgtBstSize,
	INOUT PEARL_MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
);
MSD_STATUS Pearl_gpirlCustomSetupIntf
(
IN    MSD_U32	   	tgtRate,
IN    MSD_U32	   	tgtBstSize,
IN    MSD_PIRL_COUNT_MODE    countMode,
OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
);
/*******************************************************************************
* Pearl_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - target logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlReadResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    OUT PEARL_MSD_PIRL_DATA    *pirlData
);

/*******************************************************************************
* Pearl_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- target port
*		irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlWriteResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    IN  PEARL_MSD_PIRL_DATA    *pirlData
);

/*******************************************************************************
* Pearl_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*
* OUTPUTS:
*        data	- register value
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlGetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 		 regOffset,
    OUT MSD_U16		 *data
);

/*******************************************************************************
* Pearl_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
*        
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlSetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 		 regOffset,
    IN  MSD_U16		 data
);
/*******************************************/


MSD_STATUS Pearl_gpirlReadResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
OUT MSD_PIRL_DATA    *pirlData
);
MSD_STATUS Pearl_gpirlWriteResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
IN  MSD_PIRL_DATA    *pirlData
);
MSD_STATUS Pearl_gpirlGetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
OUT MSD_U16		 *data
);
MSD_STATUS Pearl_gpirlSetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
IN  MSD_U16		 data
);

/*******************************************************************************
* Pearl_gpirlAdvConfigIntf
*
* DESCRIPTION:
*       This routine config ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlAdvConfigIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes,
IN  MSD_U32  tgtRate,
IN  MSD_U32  tgtBstSize,
IN  MSD_PIRL_COUNT_MODE  countMode,
IN  MSD_PIRL_PKTTYPE_OR_PRI  *pktTypeOrPri
);

/*******************************************************************************
* Pearl_gpirlAdvConfigDisableIntf
*
* DESCRIPTION:
*       This routine clear config for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gpirlAdvConfigDisableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes
);

#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdPIRL_h */
