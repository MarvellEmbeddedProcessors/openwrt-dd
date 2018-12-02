#include <msdCopyright.h>

/******************************************************************************
* Peridot_msdPTP.h
*
* DESCRIPTION:
*       API definitions for PTP
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef __Peridot_msdPTP_h
#define __Peridot_msdPTP_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>
#include <msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef USE_SINGLE_READ
#define MAX_PTP_CONSECUTIVE_READ    4
#define PERIDOTMSD_PTP_BUILD_TIME(_time1, _time2)       (((_time1) << 16) | (_time2))

/*
*  typedef: struct PERIDOT_PTP_PORT_INTERRUPT_EN
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
}PERIDOT_PTP_PORT_INTERRUPT_EN;

/*
* Typedef: enum PERIDOT_MSD_PTP_SPEC
*
* Description: Defines the PTP (Precise Time Protocol) SPEC type
*
* Fields:
*      MSD_PTP_IEEE_1588         - IEEE 1588
*      MSD_PTP_IEEE_802_1AS    - IEEE 802.1as
*/
typedef enum
{
    PERIDOT_MSD_PTP_IEEE_1588 = 0x0,
    PERIDOT_MSD_PTP_IEEE_802_1AS = 0x1
} PERIDOT_MSD_PTP_SPEC;

/*
*  typedef: struct PERIDOT_PTP_PORT_CONFIG
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
    MSD_BOOL        hwAccPtp;     /* Hardware Accelerated PTP */
    MSD_BOOL        kpFramwSa;    /* KS = Keep Frame SA */
    MSD_BOOL        ExthwAcc;    /*Exterbak Hardware Acceleration enable*/
    MSD_U16         arrTSMode;
    MSD_U8			arrLedCtrl; /*LED control for packets entering the device. */
    MSD_U8			depLedCtrl; /*LED control for packets departing the device. */
}PERIDOT_PTP_PORT_CONFIG;

/*
* Typedef: enum PERIDOT_PTP_TIME
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
    PERIDOT_PTP_ARR0_TIME = 0x0,
    PERIDOT_PTP_ARR1_TIME = 0x1,
    PERIDOT_PTP_DEP_TIME = 0x2
} PERIDOT_PTP_TIME;

/*
* Typedef: enum PERIDOT_PTP_INT_STATUS
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
    PERIDOT_PTP_INT_NORMAL = 0x0,
    PERIDOT_PTP_INT_OVERWRITE = 0x1,
    PERIDOT_PTP_INT_DROP = 0x2
} PERIDOT_PTP_INT_STATUS;

/*
*  typedef: struct PERIDOT_PTP_TS_STATUS
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
} PERIDOT_PTP_TS_STATUS;

/*
*  typedef: struct PERIDOT_PTP_GLOBAL_CONFIG
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
} PERIDOT_PTP_GLOBAL_CONFIG;

/*
*  typedef: struct PERIDOT_PTP_INTERRUPT_STATUS
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
} PERIDOT_PTP_INTERRUPT_STATUS;

/*
*  typedef: struct PERIDOT_PTP_MODE_CONFIG
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
} PERIDOT_PTP_MODE_CONFIG;

/*
0: Set device as slave
1: Set device as master
*/
typedef enum
{
    PERIDOT_PTP_ROLE_SLAVE = 0,
    PERIDOT_PTP_ROLE_MASTER = 1,
}PERIDOT_PTP_MASTER_SLAVE;

/*
*  typedef: struct PERIDOT_PTP_TIME_ARRAY
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
}PERIDOT_PTP_TIME_ARRAY;

/*
* Typedef: enum PERIDOT_PTP_TOD_STORE_OPERATION
*
* Description: Defines the PTP time array Store Operation type
*
* Fields:
*      PTP_TOD_STORE_COMP           - Store Comp register only to selected TimeArray
*      PTP_TOD_STORE_ALL            - Store All Registers to selected TimeArray
*/
typedef enum
{
    PERIDOT_PTP_TOD_STORE_COMP = 0x2,
    PERIDOT_PTP_TOD_STORE_ALL = 0x3,
} PERIDOT_PTP_TOD_STORE_OPERATION;

/*
*  typedef: struct PERIDOT_PTP_PULSE_STRUCT
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
} PERIDOT_PTP_PULSE_STRUCT;
/****************************************************************************/
/* Exported PTP Types			 			                                */
/****************************************************************************/



MSD_STATUS Peridot_gptpSetIntEnable
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN PERIDOT_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Peridot_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Peridot_gptpGetIntEnable
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT PERIDOT_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Peridot_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Peridot_gptpSetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_BOOL  en
);
MSD_STATUS Peridot_gptpGetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_BOOL  *en
);

MSD_STATUS Peridot_gptpSetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  PERIDOT_PTP_PORT_CONFIG *ptpData
);
MSD_STATUS Peridot_gptpSetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_PTP_PORT_CONFIG *ptpData
);

MSD_STATUS Peridot_gptpGetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT PERIDOT_PTP_PORT_CONFIG *ptpData
);
MSD_STATUS Peridot_gptpGetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_CONFIG *ptpData
);

MSD_STATUS Peridot_gptpGetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  PERIDOT_PTP_TIME    timeToRead,
    OUT PERIDOT_PTP_TS_STATUS    *ptpStatus
);
MSD_STATUS Peridot_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
);

MSD_STATUS Peridot_gptpGetTSValidSt
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
);

MSD_STATUS Peridot_gptpResetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToReset
);

MSD_STATUS Peridot_gptpSetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    IN  PERIDOT_PTP_GLOBAL_CONFIG    *ptpData
);
MSD_STATUS Peridot_gptpSetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
);

MSD_STATUS Peridot_gptpGetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    OUT PERIDOT_PTP_GLOBAL_CONFIG    *ptpData
);
MSD_STATUS Peridot_gptpGetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
);

MSD_STATUS Peridot_gptpGetIntStatus
(
    IN  MSD_QD_DEV* dev,
    OUT PERIDOT_PTP_INTERRUPT_STATUS *ptpIntSt
);
MSD_STATUS Peridot_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
);

MSD_STATUS Peridot_gptpSetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	delay
);

MSD_STATUS Peridot_gptpGetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*delay
);

MSD_STATUS Peridot_gptpSetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
);

MSD_STATUS Peridot_gptpGetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
);

MSD_STATUS Peridot_gptpSetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
);

MSD_STATUS Peridot_gptpGetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
);

MSD_STATUS Peridot_gptpSetMode
(
    IN  MSD_QD_DEV* dev,
    IN  PERIDOT_PTP_MODE_CONFIG *mode
);
MSD_STATUS Peridot_gptpSetModeIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MODE_CONFIG *mode
);

MSD_STATUS Peridot_gptpGetMode
(
    IN  MSD_QD_DEV* dev,
    OUT PERIDOT_PTP_MODE_CONFIG *mode
);
MSD_STATUS Peridot_gptpGetModeIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_MODE_CONFIG *mode
);

MSD_STATUS Peridot_gptpSetMasterSlave
(
    IN  MSD_QD_DEV* dev,
    IN  PERIDOT_PTP_MASTER_SLAVE role
);
MSD_STATUS Peridot_gptpSetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE role
);

MSD_STATUS Peridot_gptpGetMasterSlave
(
    IN  MSD_QD_DEV* dev,
    IN  PERIDOT_PTP_MASTER_SLAVE *role
);
MSD_STATUS Peridot_gptpGetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE *role
);

MSD_STATUS Peridot_gptpGetTODBusyBitStatus
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_BOOL	*busyStatus
);

MSD_STATUS Peridot_gptpTODStoreCompensation
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  PERIDOT_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Peridot_gptpTODStoreCompensationIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Peridot_gptpTODStoreAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  PERIDOT_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Peridot_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Peridot_gptpTODCaptureAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT PERIDOT_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Peridot_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Peridot_gptpSetPulse
(
    IN  MSD_QD_DEV* dev,
    IN  PERIDOT_PTP_PULSE_STRUCT *pulsePara
);
MSD_STATUS Peridot_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
);

MSD_STATUS Peridot_gptpGetPulse
(
    IN  MSD_QD_DEV* dev,
    OUT PERIDOT_PTP_PULSE_STRUCT *pulsePara
);
MSD_STATUS Peridot_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
);

MSD_STATUS Peridot_gtaiGetPTPGlobalTime
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_U32 *ptpTime
);
#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdPTP_h */
