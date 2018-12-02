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
* Topaz_msdPhy.h
*
* DESCRIPTION:
*       API definitions for Marvell Phy functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz_msdPhy_H
#define __Topaz_msdPhy_H

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Definition for Advance Virtual Cable Test */

/*
 * typedef: enum TOPAZ_MSD_RX_CHANNEL
 *
 * Description: Enumeration of Receiver Channel Assignment
 *
 * Enumerations:
 *        TOPAZ_MSD_CHANNEL_A   - Channel A
 *        TOPAZ_MSD_CHANNEL_B   - Channel B
 *        TOPAZ_MSD_CHANNEL_C   - Channel C
 *        TOPAZ_MSD_CHANNEL_D   - Channel D
 */
typedef enum
{
    TOPAZ_MSD_CHANNEL_A,
    TOPAZ_MSD_CHANNEL_B,
    TOPAZ_MSD_CHANNEL_C,
    TOPAZ_MSD_CHANNEL_D
} TOPAZ_MSD_RX_CHANNEL;

/*
 * typedef: enum TOPAZ_MSD_POLARITY_STATUS
 *
 * Description: Enumeration of polarity status
 *
 * Enumerations:
 *      TOPAZ_MSD_POSITIVE    - positive polarity
 *      TOPAZ_MSD_NEGATIVE    - negative polarity
 */
typedef enum
{
    TOPAZ_MSD_POSITIVE,
    TOPAZ_MSD_NEGATIVE

} TOPAZ_MSD_POLARITY_STATUS;

#define TOPAZ_MSD_MDI_PAIR_NUM         4    /* (1,2),(3,6),(4,5),(7,8) */
#define TOPAZ_MSD_CHANNEL_PAIR_NUM     2    /* (channel A,B),(channel C,D) */

/*
 * typedef: enum TOPAZ_MSD_ADV_VCT_TRANS_CHAN_SEL
 *
 * Description: Enumeration of Advanced VCT Transmitter channel select
 *
 * Enumerations:
 *        TOPAZ_MSD_ADV_VCT_NO_CROSSPAIR - Transmitter channel select is 000
 *        TOPAZ_MSD_ADV_VCT_CROSSPAIR    - Transmitter channelselect is 100/101/110/111
 */
typedef enum
{
    /* Advanced VCT Mode */
    TOPAZ_MSD_ADV_VCT_TCS_NO_CROSSPAIR        = 0,
    TOPAZ_MSD_ADV_VCT_TCS_CROSSPAIR_0            = 0x4,
    TOPAZ_MSD_ADV_VCT_TCS_CROSSPAIR_1            = 0x5,
    TOPAZ_MSD_ADV_VCT_TCS_CROSSPAIR_2            = 0x6,
    TOPAZ_MSD_ADV_VCT_TCS_CROSSPAIR_3            = 0x7
} TOPAZ_MSD_ADV_VCT_TRANS_CHAN_SEL;


typedef enum
{
    /* Advanced VCT Mode */
    TOPAZ_MSD_ADV_VCT_SAVG_2        = 0,
    TOPAZ_MSD_ADV_VCT_SAVG_4        = 1,
    TOPAZ_MSD_ADV_VCT_SAVG_8        = 2,
    TOPAZ_MSD_ADV_VCT_SAVG_16        = 3,
    TOPAZ_MSD_ADV_VCT_SAVG_32        = 4,
    TOPAZ_MSD_ADV_VCT_SAVG_64        = 5,
    TOPAZ_MSD_ADV_VCT_SAVG_128    = 6,
    TOPAZ_MSD_ADV_VCT_SAVG_256    = 7
} TOPAZ_MSD_ADV_VCT_SAMPLE_AVG;

typedef enum
{
    /* Advanced VCT Mode */
    TOPAZ_MSD_ADV_VCT_MAX_PEAK        =0x00,
    TOPAZ_MSD_ADV_VCT_FIRST_PEAK        =0x01,
    TOPAZ_MSD_ADV_VCT_OFFSET            =0x02,
    TOPAZ_MSD_ADV_VCT_SAMPLE_POINT    =0x03
#if 0
    /* Advanced VCT Transmiter Channel Select */
    TOPAZ_MSD_ADV_VCT_NO_CROSSPAIR    =0x04,  /* 23_5 bit 13:11 = 000 */
    TOPAZ_MSD_ADV_VCT_CROSSPAIR        =0x08   /* 23_5 bit 13:11 = 100, 101, 110, 111 */
#endif
} TOPAZ_MSD_ADV_VCT_MOD;

typedef unsigned int TOPAZ_MSD_ADV_VCT_PEAKDET_HYST;

/*
 * typedef: enum TOPAZ_MSD_ADV_VCT_MODE
 *
 * Description: Enumeration of Advanced VCT Mode and Transmitter channel select
 *
 * Enumerations:
 *      TOPAZ_MSD_ADV_VCT_MOD   
 *      TOPAZ_MSD_ADV_VCT_TRANS_CHAN_SEL     
 *      TOPAZ_MSD_ADV_VCT_SAMPLE_AVG       
 *      TOPAZ_MSD_ADV_VCT_PEAKDET_HYST 
 */
typedef struct
{
    TOPAZ_MSD_ADV_VCT_MOD                    mode;
    TOPAZ_MSD_ADV_VCT_TRANS_CHAN_SEL      transChanSel;
    TOPAZ_MSD_ADV_VCT_SAMPLE_AVG            sampleAvg;
    TOPAZ_MSD_ADV_VCT_PEAKDET_HYST        peakDetHyst;
    MSD_16                            samplePointDist; 
} TOPAZ_MSD_ADV_VCT_MODE;

/*
 * typedef: enum TOPAZ_MSD_ADV_VCT_STATUS
 *
 * Description: Enumeration of Advanced VCT status
 *
 * Enumerations:
 *      TOPAZ_MSD_ADV_VCT_FAIL     - advanced virtual cable test failed.
 *                             cable lengh cannot be determined.
 *      TOPAZ_MSD_ADV_VCT_NORMAL   - normal cable.
 *                             cable lengh may not be determined.
 *      TOPAZ_MSD_ADV_VCT_IMP_GREATER_THAN_115 - impedance mismatch > 115 ohms
 *                             cable lengh is valid.
 *      TOPAZ_MSD_ADV_VCT_IMP_LESS_THAN_85 - impedance mismatch < 85 ohms
 *                             cable lengh is valid.
 *      TOPAZ_MSD_ADV_VCT_OPEN      - cable open
 *                             cable lengh is valid.
 *      TOPAZ_MSD_ADV_VCT_SHORT      - cable shorted
 *                             cable lengh is valid.
 *      TOPAZ_MSD_ADV_VCT_CROSS_PAIR_SHORT - cross pair short.
 *                             cable lengh for each channel is valid.
 */
typedef enum
{
    TOPAZ_MSD_ADV_VCT_FAIL,
    TOPAZ_MSD_ADV_VCT_NORMAL,
    TOPAZ_MSD_ADV_VCT_IMP_GREATER_THAN_115,
    TOPAZ_MSD_ADV_VCT_IMP_LESS_THAN_85,
    TOPAZ_MSD_ADV_VCT_OPEN,
    TOPAZ_MSD_ADV_VCT_SHORT,
    TOPAZ_MSD_ADV_VCT_CROSS_PAIR_SHORT
} TOPAZ_MSD_ADV_VCT_STATUS;


/*
 * typedef: struct TOPAZ_MSD_CROSS_PAIR_LIST
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
typedef struct TOPAZ_MSD_CROSS_SHORT_LIST
{
    MSD_BOOL    channel[TOPAZ_MSD_MDI_PAIR_NUM];
    MSD_16     dist2fault[TOPAZ_MSD_MDI_PAIR_NUM];
} TOPAZ_MSD_CROSS_SHORT_LIST;

/*
 * typedef: struct TOPAZ_MSD_ADV_CABLE_STATUS
 *
 * Description: strucuture for advanced cable status.
 *
 * Fields:
 *      cableStatus - VCT cable status for each channel.
 *      crossShort  - cross pair short list for each channel. 
 *                    Valid only if relative cableStatus is MSD_ADV_VCT_CROSS_PAIR_SHORT.
 *      dist2fault  - estimated distance to fault for each channel.
 *                    Valid if relative cableStatus is one of the followings:
 *                      TOPAZ_MSD_ADV_VCT_NORMAL
 *                      TOPAZ_MSD_ADV_VCT_IMP_GREATER_THAN_115
 *                      TOPAZ_MSD_ADV_VCT_IMP_LESS_THAN_85,
 *                      TOPAZ_MSD_ADV_VCT_OPEN, or
 *                      TOPAZ_MSD_ADV_VCT_SHORT
 *      offset  - The offset seen at the receiver.
 *      sampleAmp  - returned amplitude for sample point.
 */
typedef struct 
{
    TOPAZ_MSD_ADV_VCT_STATUS   cableStatus[TOPAZ_MSD_MDI_PAIR_NUM];
    union {
        TOPAZ_MSD_CROSS_SHORT_LIST crossShort;
        MSD_16     dist2fault;
        MSD_16     offset;
        MSD_16     sampleAmp;
    }u[TOPAZ_MSD_MDI_PAIR_NUM];
} TOPAZ_MSD_ADV_CABLE_STATUS;

/*
 * typedef: struct TOPAZ_MSD_ADV_EXTENDED_STATUS
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
    TOPAZ_MSD_RX_CHANNEL      pairSwap[TOPAZ_MSD_MDI_PAIR_NUM];
    TOPAZ_MSD_POLARITY_STATUS pairPolarity[TOPAZ_MSD_MDI_PAIR_NUM];
    MSD_U32             pairSkew[TOPAZ_MSD_MDI_PAIR_NUM];
    MSD_U32                cableLen[TOPAZ_MSD_MDI_PAIR_NUM];
} TOPAZ_MSD_ADV_EXTENDED_STATUS;

typedef struct Topaz_SW_VCT_REGISTER
{
    MSD_U8    page;
    MSD_U8    regOffset;
} Topaz_SW_VCT_REGISTER;


/*******************************************************************************
* Topaz_gvctGetAdvCableDiag
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
MSD_STATUS Topaz_gvctGetAdvCableDiag
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN  TOPAZ_MSD_ADV_VCT_MODE mode,
    OUT TOPAZ_MSD_ADV_CABLE_STATUS *cableStatus
);
MSD_STATUS Topaz_gvctGetAdvCableDiagIntf
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
MSD_STATUS Topaz_gvctGetAdvExtendedStatus
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT TOPAZ_MSD_ADV_EXTENDED_STATUS *extendedStatus
);
MSD_STATUS Topaz_gvctGetAdvExtendedStatusIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT   port,
OUT MSD_ADV_EXTENDED_STATUS *extendedStatus
);
#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdVct_h */
