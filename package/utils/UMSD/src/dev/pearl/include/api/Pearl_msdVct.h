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
* Pearl_msdPhy.h
*
* DESCRIPTION:
*       API definitions for Marvell Phy functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Pearl_msdPhy_H
#define __Pearl_msdPhy_H

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Definition for Advance Virtual Cable Test */

/*
 * typedef: enum PEARL_MSD_RX_CHANNEL
 *
 * Description: Enumeration of Receiver Channel Assignment
 *
 * Enumerations:
 *        PEARL_MSD_CHANNEL_A   - Channel A
 *        PEARL_MSD_CHANNEL_B   - Channel B
 *        PEARL_MSD_CHANNEL_C   - Channel C
 *        PEARL_MSD_CHANNEL_D   - Channel D
 */
typedef enum
{
    PEARL_MSD_CHANNEL_A,
    PEARL_MSD_CHANNEL_B,
    PEARL_MSD_CHANNEL_C,
    PEARL_MSD_CHANNEL_D
} PEARL_MSD_RX_CHANNEL;

/*
 * typedef: enum PEARL_MSD_POLARITY_STATUS
 *
 * Description: Enumeration of polarity status
 *
 * Enumerations:
 *      PEARL_MSD_POSITIVE    - positive polarity
 *      PEARL_MSD_NEGATIVE    - negative polarity
 */
typedef enum
{
    PEARL_MSD_POSITIVE,
    PEARL_MSD_NEGATIVE

} PEARL_MSD_POLARITY_STATUS;

#define PEARL_MSD_MDI_PAIR_NUM         4    /* (1,2),(3,6),(4,5),(7,8) */
#define PEARL_MSD_CHANNEL_PAIR_NUM     2    /* (channel A,B),(channel C,D) */

/*
 * typedef: enum PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL
 *
 * Description: Enumeration of Advanced VCT Transmitter channel select
 *
 * Enumerations:
 *        PEARL_MSD_ADV_VCT_NO_CROSSPAIR - Transmitter channel select is 000
 *        PEARL_MSD_ADV_VCT_CROSSPAIR    - Transmitter channelselect is 100/101/110/111
 */
typedef enum
{
    /* Advanced VCT Mode */
    PEARL_MSD_ADV_VCT_TCS_NO_CROSSPAIR        = 0,
    PEARL_MSD_ADV_VCT_TCS_CROSSPAIR_0            = 0x4,
    PEARL_MSD_ADV_VCT_TCS_CROSSPAIR_1            = 0x5,
    PEARL_MSD_ADV_VCT_TCS_CROSSPAIR_2            = 0x6,
    PEARL_MSD_ADV_VCT_TCS_CROSSPAIR_3            = 0x7
} PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL;


typedef enum
{
    /* Advanced VCT Mode */
    PEARL_MSD_ADV_VCT_SAVG_2        = 0,
    PEARL_MSD_ADV_VCT_SAVG_4        = 1,
    PEARL_MSD_ADV_VCT_SAVG_8        = 2,
    PEARL_MSD_ADV_VCT_SAVG_16        = 3,
    PEARL_MSD_ADV_VCT_SAVG_32        = 4,
    PEARL_MSD_ADV_VCT_SAVG_64        = 5,
    PEARL_MSD_ADV_VCT_SAVG_128    = 6,
    PEARL_MSD_ADV_VCT_SAVG_256    = 7
} PEARL_MSD_ADV_VCT_SAMPLE_AVG;

typedef enum
{
    /* Advanced VCT Mode */
    PEARL_MSD_ADV_VCT_MAX_PEAK        =0x00,
    PEARL_MSD_ADV_VCT_FIRST_PEAK        =0x01,
    PEARL_MSD_ADV_VCT_OFFSET            =0x02,
    PEARL_MSD_ADV_VCT_SAMPLE_POINT    =0x03
#if 0
    /* Advanced VCT Transmiter Channel Select */
    PEARL_MSD_ADV_VCT_NO_CROSSPAIR    =0x04,  /* 23_5 bit 13:11 = 000 */
    PEARL_MSD_ADV_VCT_CROSSPAIR        =0x08   /* 23_5 bit 13:11 = 100, 101, 110, 111 */
#endif
} PEARL_MSD_ADV_VCT_MOD;

typedef unsigned int PEARL_MSD_ADV_VCT_PEAKDET_HYST;

/*
 * typedef: enum PEARL_MSD_ADV_VCT_MODE
 *
 * Description: Enumeration of Advanced VCT Mode and Transmitter channel select
 *
 * Enumerations:
 *      PEARL_MSD_ADV_VCT_MOD   
 *      PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL     
 *      PEARL_MSD_ADV_VCT_SAMPLE_AVG       
 *      PEARL_MSD_ADV_VCT_PEAKDET_HYST 
 */
typedef struct
{
    PEARL_MSD_ADV_VCT_MOD                    mode;
    PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL      transChanSel;
    PEARL_MSD_ADV_VCT_SAMPLE_AVG            sampleAvg;
    PEARL_MSD_ADV_VCT_PEAKDET_HYST        peakDetHyst;
    MSD_16                            samplePointDist; 
} PEARL_MSD_ADV_VCT_MODE;

/*
 * typedef: enum PEARL_MSD_ADV_VCT_STATUS
 *
 * Description: Enumeration of Advanced VCT status
 *
 * Enumerations:
 *      PEARL_MSD_ADV_VCT_FAIL     - advanced virtual cable test failed.
 *                             cable lengh cannot be determined.
 *      PEARL_MSD_ADV_VCT_NORMAL   - normal cable.
 *                             cable lengh may not be determined.
 *      PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115 - impedance mismatch > 115 ohms
 *                             cable lengh is valid.
 *      PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85 - impedance mismatch < 85 ohms
 *                             cable lengh is valid.
 *      PEARL_MSD_ADV_VCT_OPEN      - cable open
 *                             cable lengh is valid.
 *      PEARL_MSD_ADV_VCT_SHORT      - cable shorted
 *                             cable lengh is valid.
 *      PEARL_MSD_ADV_VCT_CROSS_PAIR_SHORT - cross pair short.
 *                             cable lengh for each channel is valid.
 */
typedef enum
{
    PEARL_MSD_ADV_VCT_FAIL,
    PEARL_MSD_ADV_VCT_NORMAL,
    PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115,
    PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85,
    PEARL_MSD_ADV_VCT_OPEN,
    PEARL_MSD_ADV_VCT_SHORT,
    PEARL_MSD_ADV_VCT_CROSS_PAIR_SHORT
} PEARL_MSD_ADV_VCT_STATUS;


/*
 * typedef: struct PEARL_MSD_CROSS_PAIR_LIST
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
typedef struct PEARL_MSD_CROSS_SHORT_LIST
{
    MSD_BOOL    channel[PEARL_MSD_MDI_PAIR_NUM];
    MSD_16     dist2fault[PEARL_MSD_MDI_PAIR_NUM];
} PEARL_MSD_CROSS_SHORT_LIST;

/*
 * typedef: struct PEARL_MSD_ADV_CABLE_STATUS
 *
 * Description: strucuture for advanced cable status.
 *
 * Fields:
 *      cableStatus - VCT cable status for each channel.
 *      crossShort  - cross pair short list for each channel. 
 *                    Valid only if relative cableStatus is MSD_ADV_VCT_CROSS_PAIR_SHORT.
 *      dist2fault  - estimated distance to fault for each channel.
 *                    Valid if relative cableStatus is one of the followings:
 *                      PEARL_MSD_ADV_VCT_NORMAL
 *                      PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115
 *                      PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85,
 *                      PEARL_MSD_ADV_VCT_OPEN, or
 *                      PEARL_MSD_ADV_VCT_SHORT
 *      offset  - The offset seen at the receiver.
 *      sampleAmp  - returned amplitude for sample point.
 */
typedef struct 
{
    PEARL_MSD_ADV_VCT_STATUS   cableStatus[PEARL_MSD_MDI_PAIR_NUM];
    union {
        PEARL_MSD_CROSS_SHORT_LIST crossShort;
        MSD_16     dist2fault;
        MSD_16     offset;
        MSD_16     sampleAmp;
    }u[PEARL_MSD_MDI_PAIR_NUM];
} PEARL_MSD_ADV_CABLE_STATUS;

/*
 * typedef: struct PEARL_MSD_ADV_EXTENDED_STATUS
 *
 * Description: Currently the 1000Base-T PCS can determine the cable polarity
 *         on pairs A,B,C,D; crossover on pairs A,B and C,D; and skew among 
 *        the pares. These status enhance the capability of the virtual cable tester
 *
 * Fields:
 *      isValid        - MSD_TRUE if this structure have valid information, 
 *                       MSD_FALSE otherwise.
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
    PEARL_MSD_RX_CHANNEL      pairSwap[PEARL_MSD_MDI_PAIR_NUM];
    PEARL_MSD_POLARITY_STATUS pairPolarity[PEARL_MSD_MDI_PAIR_NUM];
    MSD_U32             pairSkew[PEARL_MSD_MDI_PAIR_NUM];
    MSD_U32                cableLen[PEARL_MSD_MDI_PAIR_NUM];
} PEARL_MSD_ADV_EXTENDED_STATUS;

typedef struct Pearl_SW_VCT_REGISTER
{
    MSD_U8    page;
    MSD_U8    regOffset;
} Pearl_SW_VCT_REGISTER;


/*******************************************************************************
* Pearl_gvctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       port - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gvctGetAdvCableDiag
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN  PEARL_MSD_ADV_VCT_MODE mode,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus
);
MSD_STATUS Pearl_gvctGetAdvCableDiagIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT        port,
IN  MSD_ADV_VCT_MODE mode,
OUT MSD_ADV_CABLE_STATUS *cableStatus
);

/*******************************************************************************
* gvctGetAdvExtendedStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable length is also provided.
*
* INPUTS:
*       dev  - pointer to GT driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Pearl_gvctGetAdvExtendedStatus
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT PEARL_MSD_ADV_EXTENDED_STATUS *extendedStatus
);
MSD_STATUS Pearl_gvctGetAdvExtendedStatusIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT   port,
OUT MSD_ADV_EXTENDED_STATUS *extendedStatus
);
#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdVct_h */
