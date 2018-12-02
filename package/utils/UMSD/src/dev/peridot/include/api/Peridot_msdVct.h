#include <msdCopyright.h>
/*******************************************************************************
* Peridot_msdPhy.h
*
* DESCRIPTION:
*       API definitions for Marvell Phy functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_msdPhy_h
#define __Peridot_msdPhy_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Definition for Advance Virtual Cable Test */

/*
 * typedef: enum MSD_RX_CHANNEL
 *
 * Description: Enumeration of Receiver Channel Assignment
 *
 * Enumerations:
 *        PERIDOT_MSD_CHANNEL_A   - Channel A
 *        PERIDOT_MSD_CHANNEL_B   - Channel B
 *        PERIDOT_MSD_CHANNEL_C   - Channel C
 *        PERIDOT_MSD_CHANNEL_D   - Channel D
 */
typedef enum
{
    PERIDOT_MSD_CHANNEL_A,
    PERIDOT_MSD_CHANNEL_B,
    PERIDOT_MSD_CHANNEL_C,
    PERIDOT_MSD_CHANNEL_D
} PERIDOT_MSD_RX_CHANNEL;

/*
 * typedef: enum MSD_POLARITY_STATUS
 *
 * Description: Enumeration of polarity status
 *
 * Enumerations:
 *      PERIDOT_MSD_POSITIVE    - positive polarity
 *      PERIDOT_MSD_NEGATIVE    - negative polarity
 */
typedef enum
{
    PERIDOT_MSD_POSITIVE,
    PERIDOT_MSD_NEGATIVE

} PERIDOT_MSD_POLARITY_STATUS;

#define PERIDOT_MSD_MDI_PAIR_NUM         4    /* (1,2),(3,6),(4,5),(7,8) */
#define PERIDOT_MSD_CHANNEL_PAIR_NUM     2    /* (channel A,B),(channel C,D) */

/*
 * typedef: enum  PERIDOT_MSD_ADV_VCT_TRANS_CHAN_SEL
 *
 * Description: Enumeration of Advanced VCT Transmitter channel select
 *
 * Enumerations:
 *         PERIDOT_MSD_ADV_VCT_NO_CROSSPAIR - Transmitter channel select is 000
 *         PERIDOT_MSD_ADV_VCT_CROSSPAIR    - Transmitter channelselect is 100/101/110/111
 */
typedef enum
{
    /* Advanced VCT Mode */
     PERIDOT_MSD_ADV_VCT_TCS_NO_CROSSPAIR        = 0,
     PERIDOT_MSD_ADV_VCT_TCS_CROSSPAIR_0            = 0x4,
     PERIDOT_MSD_ADV_VCT_TCS_CROSSPAIR_1            = 0x5,
     PERIDOT_MSD_ADV_VCT_TCS_CROSSPAIR_2            = 0x6,
     PERIDOT_MSD_ADV_VCT_TCS_CROSSPAIR_3            = 0x7
} PERIDOT_MSD_ADV_VCT_TRANS_CHAN_SEL;


typedef enum
{
    /* Advanced VCT Mode */
     PERIDOT_MSD_ADV_VCT_SAVG_2        = 0,
     PERIDOT_MSD_ADV_VCT_SAVG_4        = 1,
     PERIDOT_MSD_ADV_VCT_SAVG_8        = 2,
     PERIDOT_MSD_ADV_VCT_SAVG_16        = 3,
     PERIDOT_MSD_ADV_VCT_SAVG_32        = 4,
     PERIDOT_MSD_ADV_VCT_SAVG_64        = 5,
     PERIDOT_MSD_ADV_VCT_SAVG_128    = 6,
     PERIDOT_MSD_ADV_VCT_SAVG_256    = 7
}  PERIDOT_MSD_ADV_VCT_SAMPLE_AVG;

typedef enum
{
    /* Advanced VCT Mode */
     PERIDOT_MSD_ADV_VCT_MAX_PEAK        =0x00,
     PERIDOT_MSD_ADV_VCT_FIRST_PEAK        =0x01,
     PERIDOT_MSD_ADV_VCT_OFFSET            =0x02,
     PERIDOT_MSD_ADV_VCT_SAMPLE_POINT    =0x03
#if 0
    /* Advanced VCT Transmiter Channel Select */
     PERIDOT_MSD_ADV_VCT_NO_CROSSPAIR    =0x04,  /* 23_5 bit 13:11 = 000 */
     PERIDOT_MSD_ADV_VCT_CROSSPAIR        =0x08   /* 23_5 bit 13:11 = 100, 101, 110, 111 */
#endif
}  PERIDOT_MSD_ADV_VCT_MOD;

typedef unsigned int  PERIDOT_MSD_ADV_VCT_PEAKDET_HYST;

/*
 * typedef: enum  PERIDOT_MSD_ADV_VCT_MODE
 *
 * Description: Enumeration of Advanced VCT Mode and Transmitter channel select
 *
 * Enumerations:
 *       PERIDOT_MSD_ADV_VCT_FIRST_PEAK   - first peak above a certain threshold is reported.
 *       PERIDOT_MSD_ADV_VCT_MAX_PEAK     - maximum peak above a certain threshold is reported.
 *       PERIDOT_MSD_ADV_VCT_OFFSE         - offset
 *       PERIDOT_MSD_ADV_VCT_SAMPLE_POINT - sample point
 */
typedef struct
{
     PERIDOT_MSD_ADV_VCT_MOD                    mode;
     PERIDOT_MSD_ADV_VCT_TRANS_CHAN_SEL      transChanSel;
     PERIDOT_MSD_ADV_VCT_SAMPLE_AVG            sampleAvg;
     PERIDOT_MSD_ADV_VCT_PEAKDET_HYST        peakDetHyst;
    MSD_16                            samplePointDist; 
}  PERIDOT_MSD_ADV_VCT_MODE;

/*
 * typedef: enum  PERIDOT_MSD_ADV_VCT_STATUS
 *
 * Description: Enumeration of Advanced VCT status
 *
 * Enumerations:
 *       PERIDOT_MSD_ADV_VCT_FAIL     - advanced virtual cable test failed.
 *                             cable lengh cannot be determined.
 *       PERIDOT_MSD_ADV_VCT_NORMAL   - normal cable.
 *                             cable lengh may not be determined.
 *       PERIDOT_MSD_ADV_VCT_IMP_GREATER_THAN_115 - impedance mismatch > 115 ohms
 *                             cable lengh is valid.
 *       PERIDOT_MSD_ADV_VCT_IMP_LESS_THAN_85 - impedance mismatch < 85 ohms
 *                             cable lengh is valid.
 *       PERIDOT_MSD_ADV_VCT_OPEN      - cable open
 *                             cable lengh is valid.
 *       PERIDOT_MSD_ADV_VCT_SHORT      - cable shorted
 *                             cable lengh is valid.
 *       PERIDOT_MSD_ADV_VCT_CROSS_PAIR_SHORT - cross pair short.
 *                             cable lengh for each channel is valid.
 */
typedef enum
{
     PERIDOT_MSD_ADV_VCT_FAIL,
     PERIDOT_MSD_ADV_VCT_NORMAL,
     PERIDOT_MSD_ADV_VCT_IMP_GREATER_THAN_115,
     PERIDOT_MSD_ADV_VCT_IMP_LESS_THAN_85,
     PERIDOT_MSD_ADV_VCT_OPEN,
     PERIDOT_MSD_ADV_VCT_SHORT,
     PERIDOT_MSD_ADV_VCT_CROSS_PAIR_SHORT
}  PERIDOT_MSD_ADV_VCT_STATUS;


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
typedef struct _PERIDOT_MSD_CROSS_SHORT_LIST
{
    MSD_BOOL    channel[PERIDOT_MSD_MDI_PAIR_NUM];
    MSD_16     dist2fault[PERIDOT_MSD_MDI_PAIR_NUM];
} PERIDOT_MSD_CROSS_SHORT_LIST;

/*
 * typedef: struct  PERIDOT_MSD_ADV_CABLE_STATUS
 *
 * Description: strucuture for advanced cable status.
 *
 * Fields:
 *      cableStatus - VCT cable status for each channel.
 *      crossShort  - cross pair short list for each channel. 
 *                    Valid only if relative cableStatus is  PERIDOT_MSD_ADV_VCT_CROSS_PAIR_SHORT.
 *      dist2fault  - estimated distance to fault for each channel.
 *                    Valid if relative cableStatus is one of the followings:
 *                       PERIDOT_MSD_ADV_VCT_NORMAL
 *                       PERIDOT_MSD_ADV_VCT_IMP_GREATER_THAN_115
 *                       PERIDOT_MSD_ADV_VCT_IMP_LESS_THAN_85,
 *                       PERIDOT_MSD_ADV_VCT_OPEN, or
 *                         PERIDOT_MSD_ADV_VCT_SHORT
 *      offset  - The offset seen at the receiver.
 *      sampleAmp  - returned amplitude for sample point.
 */
typedef struct
{
	PERIDOT_MSD_ADV_VCT_STATUS   cableStatus[PERIDOT_MSD_MDI_PAIR_NUM];
    union {
		PERIDOT_MSD_CROSS_SHORT_LIST crossShort;
        MSD_16     dist2fault;
        MSD_16     offset;
        MSD_16     sampleAmp;
	}u[PERIDOT_MSD_MDI_PAIR_NUM];
}  PERIDOT_MSD_ADV_CABLE_STATUS;

/*
 * typedef: struct  PERIDOT_MSD_ADV_EXTENDED_STATUS
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
 *      pairPolarity- PERIDOT_MSD_POSITIVE, if polarity is positive,
 *                    PERIDOT_MSD_NEGATIVE, otherwise
 *      pairSkew    - pair skew in units of ns
 *      cableLen    - cable length based on DSP
 */
typedef struct
{
    MSD_BOOL						isValid;
	PERIDOT_MSD_RX_CHANNEL		pairSwap[PERIDOT_MSD_MDI_PAIR_NUM];
	PERIDOT_MSD_POLARITY_STATUS	pairPolarity[PERIDOT_MSD_MDI_PAIR_NUM];
	MSD_U32						pairSkew[PERIDOT_MSD_MDI_PAIR_NUM];
	MSD_U32						cableLen[PERIDOT_MSD_MDI_PAIR_NUM];
}  PERIDOT_MSD_ADV_EXTENDED_STATUS;

typedef struct _Peridot_SW_VCT_REGISTER
{
    MSD_U8    page;
    MSD_U8    regOffset;
} Peridot_SW_VCT_REGISTER;


/*******************************************************************************
*  Peridot_gvctGetAdvCableDiag
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
MSD_STATUS  Peridot_gvctGetAdvCableDiag
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN   PERIDOT_MSD_ADV_VCT_MODE mode,
    OUT  PERIDOT_MSD_ADV_CABLE_STATUS *cableStatus
);

MSD_STATUS  Peridot_gvctGetAdvCableDiagIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT        port,
	IN   MSD_ADV_VCT_MODE mode,
	OUT  MSD_ADV_CABLE_STATUS *cableStatus
);
/*******************************************************************************
*  Peridot_gvctGetAdvExtendedStatus
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
MSD_STATUS  Peridot_gvctGetAdvExtendedStatus
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT  PERIDOT_MSD_ADV_EXTENDED_STATUS *extendedStatus
);
MSD_STATUS Peridot_gvctGetAdvExtendedStatusIntf
(
	IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT   port,
	OUT MSD_ADV_EXTENDED_STATUS *extendedStatus
);
#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdVct_h */
