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
* Topaz_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz_msdPortRmon_h
#define __Topaz_msdPortRmon_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported MIBS Types			 			                                   */
/****************************************************************************/
/*
 *  typedef: struct TOPAZ_MSD_STATS_COUNTERS
 *
 *  Description: MIB counter
 *     
 */
#define TOPAZ_MSD_TYPE_BANK 0x400
typedef enum
{
	/* Bank 0 */
    Topaz_STATS_InGoodOctetsLo = 0,
    Topaz_STATS_InGoodOctetsHi,
    Topaz_STATS_InBadOctets,
    
    Topaz_STATS_OutFCSErr,
    Topaz_STATS_InUnicasts,
    Topaz_STATS_Deferred,            /* offset 5 */
    Topaz_STATS_InBroadcasts,
    Topaz_STATS_InMulticasts,
    Topaz_STATS_64Octets,
    Topaz_STATS_127Octets,
    Topaz_STATS_255Octets,            /* offset 10 */
    Topaz_STATS_511Octets,
    Topaz_STATS_1023Octets,
    Topaz_STATS_MaxOctets,
    Topaz_STATS_OutOctetsLo,
    Topaz_STATS_OutOctetsHi,
    Topaz_STATS_OutUnicasts,        /* offset 16 */
    Topaz_STATS_Excessive,
    Topaz_STATS_OutMulticasts,
    Topaz_STATS_OutBroadcasts,
    Topaz_STATS_Single,
    Topaz_STATS_OutPause,
    Topaz_STATS_InPause,
    Topaz_STATS_Multiple,
    Topaz_STATS_InUndersize,            /* offset 24 */
    Topaz_STATS_InFragments,
    Topaz_STATS_InOversize,
    Topaz_STATS_InJabber,
    Topaz_STATS_InRxErr,
    Topaz_STATS_InFCSErr,
    Topaz_STATS_Collisions,
    Topaz_STATS_Late,                    /* offset 31 */
	/* Bank 1 */
    Topaz_STATS_InDiscards      = TOPAZ_MSD_TYPE_BANK+0x00,
    Topaz_STATS_InFiltered      = TOPAZ_MSD_TYPE_BANK+0x01,
    Topaz_STATS_InAccepted      = TOPAZ_MSD_TYPE_BANK+0x02,
    Topaz_STATS_InBadAccepted   = TOPAZ_MSD_TYPE_BANK+0x03,
    Topaz_STATS_InGoodAvbClassA = TOPAZ_MSD_TYPE_BANK+0x04,
    Topaz_STATS_InGoodAvbClassB = TOPAZ_MSD_TYPE_BANK+0x05,
    Topaz_STATS_InBadAvbClassA  = TOPAZ_MSD_TYPE_BANK+0x06,
    Topaz_STATS_InBadAvbClassB  = TOPAZ_MSD_TYPE_BANK+0x07,
    Topaz_STATS_TCAMCounter0    = TOPAZ_MSD_TYPE_BANK+0x08,
    Topaz_STATS_TCAMCounter1    = TOPAZ_MSD_TYPE_BANK+0x09,
    Topaz_STATS_TCAMCounter2    = TOPAZ_MSD_TYPE_BANK+0x0a,
    Topaz_STATS_TCAMCounter3    = TOPAZ_MSD_TYPE_BANK+0x0b,
    Topaz_STATS_InDroppedAvbA   = TOPAZ_MSD_TYPE_BANK+0x0c, /* For Opus */
    Topaz_STATS_InDroppedAvbB   = TOPAZ_MSD_TYPE_BANK+0x0d, /* For Opus */
    Topaz_STATS_InDaUnknown     = TOPAZ_MSD_TYPE_BANK+0x0e,
    Topaz_STATS_InMGMT          = TOPAZ_MSD_TYPE_BANK+0x0f,
    Topaz_STATS_OutQueue0       = TOPAZ_MSD_TYPE_BANK+0x10,
    Topaz_STATS_OutQueue1       = TOPAZ_MSD_TYPE_BANK+0x11,
    Topaz_STATS_OutQueue2       = TOPAZ_MSD_TYPE_BANK+0x12,
    Topaz_STATS_OutQueue3       = TOPAZ_MSD_TYPE_BANK+0x13,
    Topaz_STATS_OutQueue4       = TOPAZ_MSD_TYPE_BANK+0x14,
    Topaz_STATS_OutQueue5       = TOPAZ_MSD_TYPE_BANK+0x15,
    Topaz_STATS_OutQueue6       = TOPAZ_MSD_TYPE_BANK+0x16,
    Topaz_STATS_OutQueue7       = TOPAZ_MSD_TYPE_BANK+0x17,
    Topaz_STATS_OutCutThrough   = TOPAZ_MSD_TYPE_BANK+0x18,
    Topaz_STATS_OutOctetsA      = TOPAZ_MSD_TYPE_BANK+0x1a,
    Topaz_STATS_OutOctetsB      = TOPAZ_MSD_TYPE_BANK+0x1b,
    Topaz_STATS_OutDroppedYel   = TOPAZ_MSD_TYPE_BANK+0x1d, /* For Opus */
    Topaz_STATS_OutDiscards     = TOPAZ_MSD_TYPE_BANK+0x1e, /* For Opus */
    Topaz_STATS_OutMGMT         = TOPAZ_MSD_TYPE_BANK+0x1f

} TOPAZ_MSD_STATS_COUNTERS;

/*
 *  typedef: struct MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct TOPAZ_MSD_STATS_COUNTER_SET
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
    MSD_U32    InRxErr;      /* offset 28 */
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
    MSD_U32    InBadAvbClassA ;  /* offset 0x06 */
    MSD_U32    InBadAvbClassB ;  /* offset 0x07 */
    MSD_U32    TCAMCounter0;     /* offset 0x08 */
    MSD_U32    TCAMCounter1;     /* offset 0x09 */
    MSD_U32    TCAMCounter2;     /* offset 0x0a */
    MSD_U32    TCAMCounter3;     /* offset 0x0b */
    MSD_U32    InDroppedAvbA;    /* For Opus, reserved_c for Pearl; offset 0x0c */
    MSD_U32    InDroppedAvbB;     /* For Opus, reserved_d for Pearl; offset 0x0d */
    MSD_U32    InDaUnknown ;     /* offset 0x0e */
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
    MSD_U32    reserved_19 ;     /* offset 0x19 */
    MSD_U32    OutOctetsA;       /* offset 0x1a */
    MSD_U32    OutOctetsB;       /* offset 0x1b */
    MSD_U32    reserved_1C;      /* For Opus, reserved_1c for Pearl; offset 0x1c */
    MSD_U32    OutDroppedYel;    /* For Opus, reserved_1d for Pearl; offset 0x1d */
    MSD_U32    OutDiscards;      /* For Opus, reserved_1e for Pearl; offset 0x1e */
    MSD_U32    OutMGMT;          /* offset 0x1f */

} TOPAZ_MSD_STATS_COUNTER_SET;

/*
 * typedef: enum TOPAZ_MSD_HISTOGRAM_MODE
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
    TOPAZ_MSD_COUNT_RX_ONLY = 1,
    TOPAZ_MSD_COUNT_TX_ONLY,
    TOPAZ_MSD_COUNT_RX_TX
} TOPAZ_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Topaz_gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* Topaz_gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
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
MSD_STATUS Topaz_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* Topaz_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Topaz_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  TOPAZ_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS Topaz_gstatsGetPortCounterIntf
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
/*******************************************************************************
* Topaz_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to TOPAZ_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Topaz_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
    OUT TOPAZ_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS Topaz_gstatsGetPortAllCountersIntf
(
IN  MSD_QD_DEV* dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* Topaz_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (TOPAZ_MSD_COUNT_RX_ONLY, TOPAZ_MSD_COUNT_TX_ONLY, 
*                    and TOPAZ_MSD_COUNT_RX_TX)
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
MSD_STATUS Topaz_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
    OUT TOPAZ_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS Topaz_gstatsGetHistogramModeIntf
(
    IN  MSD_QD_DEV   *dev,
    OUT MSD_HISTOGRAM_MODE    *mode
);
/*******************************************************************************
* gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (TOPAZ_MSD_COUNT_RX_ONLY, TOPAZ_MSD_COUNT_TX_ONLY, 
*                    and TOPAZ_MSD_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Topaz_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
    IN TOPAZ_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS Topaz_gstatsSetHistogramModeIntf
(
    IN MSD_QD_DEV   *dev,
    IN MSD_HISTOGRAM_MODE        mode
);
/*******************************************************************************
* Topaz_gmibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
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
MSD_STATUS Topaz_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);
#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdPortRmon_h */
