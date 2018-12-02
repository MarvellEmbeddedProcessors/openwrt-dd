#include <msdCopyright.h>

/******************************************************************************
* Topaz_msdPTP.h
*
* DESCRIPTION:
*       API definitions for PTP
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef __Topaz_msdPTP_h
#define __Topaz_msdPTP_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef USE_SINGLE_READ
#define MAX_PTP_CONSECUTIVE_READ    4
#define TOPAZMSD_PTP_BUILD_TIME(_time1, _time2)       (((_time1) << 16) | (_time2))

/*
*  typedef: struct TOPAZ_PTP_PORT_INTERRUPT_EN
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
}TOPAZ_PTP_PORT_INTERRUPT_EN;

/*
* Typedef: enum TOPAZ_MSD_PTP_SPEC
*
* Description: Defines the PTP (Precise Time Protocol) SPEC type
*
* Fields:
*      MSD_PTP_IEEE_1588         - IEEE 1588
*      MSD_PTP_IEEE_802_1AS    - IEEE 802.1as
*/
typedef enum
{
    TOPAZ_MSD_PTP_IEEE_1588 = 0x0,
    TOPAZ_MSD_PTP_IEEE_802_1AS = 0x1
} TOPAZ_MSD_PTP_SPEC;

/*
*  typedef: struct TOPAZ_PTP_PORT_CONFIG
*
*  Description: PTP configuration parameters for each port
*
*  Fields:
*      transSpec    - This is to differentiate between various timing protocols.
*      disTSpec     - Disable Transport specific check
*      etJump         - offset to Ether type start address in bytes
*      ipJump         - offset to IP header start address counting from Ether type offset
*      ptpArrIntEn    - PTP port arrival interrupt enable
*      ptpDepIntEn    - PTP port departure interrupt enable
*      disTSOverwrite     - disable time stamp counter overwriting until the corresponding
*                          timer valid bit is cleared.
*/
typedef struct
{
    MSD_PTP_SPEC	transSpec;
    MSD_BOOL		disTSpec;
    MSD_U32			etJump;
    MSD_U32			ipJump;
    MSD_BOOL		ptpArrIntEn;
    MSD_BOOL		ptpDepIntEn;
    MSD_BOOL		disTSOverwrite;
    MSD_BOOL        filterAct;    /* Filter 802.1 Act from LEDs */
    MSD_U16         arrTSMode;
    MSD_U8			arrLedCtrl; /*LED control for packets entering the device. */
    MSD_U8			depLedCtrl; /*LED control for packets departing the device. */
}TOPAZ_PTP_PORT_CONFIG;

/*
* Typedef: enum TOPAZ_PTP_TIME
*
* Description: Defines the PTP Time to be read
*
* Fields:
*      PTP_ARR0_TIME			- PTP Arrival Time 0
*      PTP_ARR1_TIME			- PTP Arrival Time 1
*      PTP_DEP_TIME			- PTP Departure Time
*/
typedef enum
{
    TOPAZ_PTP_ARR0_TIME = 0x0,
    TOPAZ_PTP_ARR1_TIME = 0x1,
    TOPAZ_PTP_DEP_TIME = 0x2
} TOPAZ_PTP_TIME;

/*
* Typedef: enum TOPAZ_PTP_INT_STATUS
*
* Description: Defines the PTP Port interrupt status for time stamp
*
* Fields:
*      PTP_INT_NORMAL        - No error condition occurred
*      PTP_INT_OVERWRITE     - PTP logic has to process more than one PTP frame
*                                  that needs time stamping before the current read.
*                                Only the latest one is saved.
*      PTP_INT_DROP          - PTP logic has to process more than one PTP frame
*                                  that needs time stamping before the current read.
*                                Only the oldest one is saved.
*
*/
typedef enum
{
    TOPAZ_PTP_INT_NORMAL = 0x0,
    TOPAZ_PTP_INT_OVERWRITE = 0x1,
    TOPAZ_PTP_INT_DROP = 0x2
} TOPAZ_PTP_INT_STATUS;

/*
*  typedef: struct TOPAZ_PTP_TS_STATUS
*
*  Description: PTP port status of time stamp
*
*  Fields:
*      isValid        - time stamp is valid
*      status        - time stamp error status
*      timeStamped    - time stamp value of a PTP frame that needs to be time stamped
*      ptpSeqId    - sequence ID of the frame whose time stamp information has been captured
*/
typedef struct
{
    MSD_BOOL    isValid;
    MSD_U32    timeStamped;
    MSD_U32    ptpSeqId;
    MSD_PTP_INT_STATUS    status;
} TOPAZ_PTP_TS_STATUS;

/*
*  typedef: struct TOPAZ_PTP_GLOBAL_CONFIG
*
*  Description: PTP global configuration parameters
*
*  Fields:
*      ptpEType    - PTP Ether Type
*      msgIdTSEn     - Message IDs that needs time stamp
*      tsArrPtr     - Time stamp arrival time counter pointer (either Arr0Time or Arr1Time)
*/
typedef struct
{
    MSD_U32    ptpEType;
    MSD_U32    msgIdTSEn;
    MSD_U32    tsArrPtr;
} TOPAZ_PTP_GLOBAL_CONFIG;

/*
*  typedef: struct TOPAZ_PTP_INTERRUPT_STATUS
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
} TOPAZ_PTP_INTERRUPT_STATUS;

/****************************************************************************/
/* Exported PTP Types			 			                                */
/****************************************************************************/



MSD_STATUS Topaz_gptpSetIntEnable
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN TOPAZ_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Topaz_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Topaz_gptpGetIntEnable
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT TOPAZ_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Topaz_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Topaz_gptpSetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_BOOL  en
);
MSD_STATUS Topaz_gptpGetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_BOOL  *en
);

MSD_STATUS Topaz_gptpSetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  TOPAZ_PTP_PORT_CONFIG *ptpData
);
MSD_STATUS Topaz_gptpSetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_PTP_PORT_CONFIG *ptpData
);

MSD_STATUS Topaz_gptpGetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT TOPAZ_PTP_PORT_CONFIG *ptpData
);
MSD_STATUS Topaz_gptpGetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_CONFIG *ptpData
);

MSD_STATUS Topaz_gptpGetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  TOPAZ_PTP_TIME    timeToRead,
    OUT TOPAZ_PTP_TS_STATUS    *ptpStatus
);
MSD_STATUS Topaz_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
);

MSD_STATUS Topaz_gptpGetTSValidSt
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
);

MSD_STATUS Topaz_gptpResetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToReset
);

MSD_STATUS Topaz_gptpSetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    IN  TOPAZ_PTP_GLOBAL_CONFIG    *ptpData
);
MSD_STATUS Topaz_gptpSetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
);

MSD_STATUS Topaz_gptpGetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    OUT TOPAZ_PTP_GLOBAL_CONFIG    *ptpData
);
MSD_STATUS Topaz_gptpGetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
);

MSD_STATUS Topaz_gptpGetIntStatus
(
    IN  MSD_QD_DEV* dev,
    OUT TOPAZ_PTP_INTERRUPT_STATUS *ptpIntSt
);
MSD_STATUS Topaz_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
);
#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdPTP_h */
