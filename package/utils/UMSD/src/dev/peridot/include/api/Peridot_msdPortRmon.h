#include <msdCopyright.h>
/*******************************************************************************
* Peridot_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_msdPortRmon_h
#define __Peridot_msdPortRmon_h

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
 *  typedef: struct MSD_STATS_COUNTERS
 *
 *  Description: MIB counter
 *     
 */
#define PERIDOT_MSD_TYPE_BANK 0x400
typedef enum
{
	/* Bank 0 */
    Peridot_STATS_InGoodOctetsLo = 0,
    Peridot_STATS_InGoodOctetsHi,
    Peridot_STATS_InBadOctets,
    
    Peridot_STATS_OutFCSErr,
    Peridot_STATS_InUnicasts,
    Peridot_STATS_Deferred,             /* offset 5 */
    Peridot_STATS_InBroadcasts,
    Peridot_STATS_InMulticasts,
    Peridot_STATS_64Octets,
    Peridot_STATS_127Octets,
    Peridot_STATS_255Octets,            /* offset 10 */
    Peridot_STATS_511Octets,
    Peridot_STATS_1023Octets,
    Peridot_STATS_MaxOctets,
    Peridot_STATS_OutOctetsLo,
    Peridot_STATS_OutOctetsHi,
    Peridot_STATS_OutUnicasts,          /* offset 16 */
    Peridot_STATS_Excessive,
    Peridot_STATS_OutMulticasts,
    Peridot_STATS_OutBroadcasts,
    Peridot_STATS_Single,
    Peridot_STATS_OutPause,
    Peridot_STATS_InPause,
    Peridot_STATS_Multiple,
    Peridot_STATS_InUndersize,          /* offset 24 */
    Peridot_STATS_InFragments,
    Peridot_STATS_InOversize,
    Peridot_STATS_InJabber,
    Peridot_STATS_InRxErr,
    Peridot_STATS_InFCSErr,
    Peridot_STATS_Collisions,
    Peridot_STATS_Late,                 /* offset 31 */
	/* Bank 1 */
    Peridot_STATS_InDiscards      = PERIDOT_MSD_TYPE_BANK+0x00,
    Peridot_STATS_InFiltered      = PERIDOT_MSD_TYPE_BANK+0x01,
    Peridot_STATS_InAccepted      = PERIDOT_MSD_TYPE_BANK+0x02,
    Peridot_STATS_InBadAccepted   = PERIDOT_MSD_TYPE_BANK+0x03,
    Peridot_STATS_InGoodAvbClassA = PERIDOT_MSD_TYPE_BANK+0x04,
    Peridot_STATS_InGoodAvbClassB = PERIDOT_MSD_TYPE_BANK+0x05,
    Peridot_STATS_InBadAvbClassA  = PERIDOT_MSD_TYPE_BANK+0x06,
    Peridot_STATS_InBadAvbClassB  = PERIDOT_MSD_TYPE_BANK+0x07,
    Peridot_STATS_TCAMCounter0    = PERIDOT_MSD_TYPE_BANK+0x08,
    Peridot_STATS_TCAMCounter1    = PERIDOT_MSD_TYPE_BANK+0x09,
    Peridot_STATS_TCAMCounter2    = PERIDOT_MSD_TYPE_BANK+0x0a,
    Peridot_STATS_TCAMCounter3    = PERIDOT_MSD_TYPE_BANK+0x0b,
    Peridot_STATS_InDroppedAvbA   = PERIDOT_MSD_TYPE_BANK+0x0c,
    Peridot_STATS_InDroppedAvbB   = PERIDOT_MSD_TYPE_BANK+0x0d, 
    Peridot_STATS_InDaUnknown     = PERIDOT_MSD_TYPE_BANK+0x0e,
    Peridot_STATS_InMGMT          = PERIDOT_MSD_TYPE_BANK+0x0f,
    Peridot_STATS_OutQueue0       = PERIDOT_MSD_TYPE_BANK+0x10,
    Peridot_STATS_OutQueue1       = PERIDOT_MSD_TYPE_BANK+0x11,
    Peridot_STATS_OutQueue2       = PERIDOT_MSD_TYPE_BANK+0x12,
    Peridot_STATS_OutQueue3       = PERIDOT_MSD_TYPE_BANK+0x13,
    Peridot_STATS_OutQueue4       = PERIDOT_MSD_TYPE_BANK+0x14,
    Peridot_STATS_OutQueue5       = PERIDOT_MSD_TYPE_BANK+0x15,
    Peridot_STATS_OutQueue6       = PERIDOT_MSD_TYPE_BANK+0x16,
    Peridot_STATS_OutQueue7       = PERIDOT_MSD_TYPE_BANK+0x17,
    Peridot_STATS_OutCutThrough   = PERIDOT_MSD_TYPE_BANK+0x18,
    Peridot_STATS_OutOctetsA      = PERIDOT_MSD_TYPE_BANK+0x1a,
    Peridot_STATS_OutOctetsB      = PERIDOT_MSD_TYPE_BANK+0x1b,
    Peridot_STATS_OutYel          = PERIDOT_MSD_TYPE_BANK+0x1c,
    Peridot_STATS_OutDroppedYel   = PERIDOT_MSD_TYPE_BANK+0x1d, 
    Peridot_STATS_OutDiscards     = PERIDOT_MSD_TYPE_BANK+0x1e, 
    Peridot_STATS_OutMGMT         = PERIDOT_MSD_TYPE_BANK+0x1f

} PERIDOT_MSD_STATS_COUNTERS;

/*
 *  typedef: struct PERIDOT_MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct _PERIDOT_MSD_STATS_COUNTER_SET
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
    MSD_U32    InBadAvbClassA ;  /* offset 0x06 */
    MSD_U32    InBadAvbClassB ;  /* offset 0x07 */
    MSD_U32    TCAMCounter0;     /* offset 0x08 */
    MSD_U32    TCAMCounter1;     /* offset 0x09 */
    MSD_U32    TCAMCounter2;     /* offset 0x0a */
    MSD_U32    TCAMCounter3;     /* offset 0x0b */
    MSD_U32    InDroppedAvbA;    /* offset 0x0c */
    MSD_U32    InDroppedAvbB;    /* offset 0x0d */
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
    MSD_U32    OutYel;           /* offset 0x1c */
    MSD_U32    OutDroppedYel;    /* offset 0x1d */
    MSD_U32    OutDiscards;      /* offset 0x1e */
    MSD_U32    OutMGMT;          /* offset 0x1f */

}PERIDOT_MSD_STATS_COUNTER_SET;

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
	PERIDOT_MSD_COUNT_RX_ONLY = 1,
	PERIDOT_MSD_COUNT_TX_ONLY,
	PERIDOT_MSD_COUNT_RX_TX
} PERIDOT_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Peridot_gstatsFlushAll
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
MSD_STATUS Peridot_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* Peridot_gstatsFlushPort
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
MSD_STATUS Peridot_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* Peridot_gstatsGetPortCounter
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
MSD_STATUS Peridot_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT         port,
	IN  PERIDOT_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS Peridot_gstatsGetPortCounterIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
IN  MSD_STATS_COUNTERS    counter,
OUT MSD_U32            *statsData
);
/*******************************************************************************
* Peridot_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to PERIDOT_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Peridot_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
	OUT PERIDOT_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS Peridot_gstatsGetPortAllCountersIntf
(
IN  MSD_QD_DEV               *dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* Peridot_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
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
MSD_STATUS Peridot_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
	OUT PERIDOT_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS Peridot_gstatsGetHistogramModeIntf
(
IN  MSD_QD_DEV   *dev,
OUT MSD_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* Peridot_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
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
MSD_STATUS Peridot_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
	IN PERIDOT_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS Peridot_gstatsSetHistogramModeIntf
(
IN MSD_QD_DEV   *dev,
IN MSD_HISTOGRAM_MODE        mode
);

/*******************************************************************************
* Peridot_gmibDump
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
MSD_STATUS Peridot_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);

#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdPortRmon_h */
