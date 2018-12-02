/*******************************************************************************
*                 Copyright 2015, MARVELL TECHNOLOGY GROUP LTD                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
*
* apiCLI.h
*
* DESCRIPTION:
*       Command Line Interface
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __apiCLI_h
#define __apiCLI_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CLI_COMMAND_STRUCT
{
    char *cmd;
    int(*callback)();
    char *help;
}CLI_COMMAND_STRUCT;

typedef struct _EXTERN_CLI_STRUCT
{
	char *cmd;
	int (*callback)();
	CLI_COMMAND_STRUCT *cmdList;
	char *desc;
	char *help;
}EXTERN_CLI_STRUCT;

typedef MSD_STATUS(*API_SET_PORT_U32) (MSD_U8, MSD_LPORT, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_PU32) (MSD_U8, MSD_LPORT, MSD_U32*);

typedef MSD_STATUS(*API_SET_PORT_BOOL_U32) (MSD_U8, MSD_LPORT, MSD_BOOL, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_PBOOL_PU32) (MSD_U8, MSD_LPORT, MSD_BOOL*, MSD_U32*);

typedef MSD_STATUS(*API_SET_PORT_U32_U32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_U32_PU32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32*);
typedef MSD_STATUS(*API_GET_PORT_PU32_PU32) (MSD_U8, MSD_LPORT, MSD_U32*, MSD_U32*);
typedef MSD_STATUS(*API_SET_PORT_U32_U32_U32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_U32__U32_PU32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32,MSD_U32*);

typedef MSD_STATUS(*API_SET_U32) (MSD_U8, MSD_U32);
typedef MSD_STATUS(*API_GET_PU32) (MSD_U8, MSD_U32*);

typedef MSD_STATUS(*API_SET_DEV) (MSD_U8);

typedef void(*PRINT_CALLBACK)(const char *string);
void setPrintCallback(PRINT_CALLBACK callback);

typedef struct _CLI_PAIR_API_STRUCT_
{
    char* setCmd;
    void* setFunc;

    char* getCmd;
    void* getFunc;
}CLI_PAIR_API_STRUCT;

extern EXTERN_CLI_STRUCT externCLICMDList[];

int fileOperation();

int atuAddEntry();
int atuGetNextEntry();
int atuFlush();
int atuFlushInDB();
int atuMove();
int atuMoveInDB();
int atuDelEntry();
int atuGetVio();
int atuFindEntry();
int atuPortRemove();
int portRemoveINDB();
int atuHelp();
int atuOperation();

int stuAddEntry();
int stuGetNext();
int stuFindEntry();
int stuDelEntry();
int stuGetEntryCount();
int stuHelp();
int stuOperation();

int vtuAddEntry();
int vtuFlush();
int vtuFindEntry();
int vtuDelEntry();
int vtuSetMode();
int vtuGetMode();
int vtuGetVio();
int vutIsExist();
int vtuGetNext();
int vtuGetCount();
int vtuSetMemberTag();
int vtuGetMemberTag();
int vtuHelp();
int vtuOperation();

int IMPRun();
int IMPReset();
int IMPStop();
int IMPWriteEEPROM();
int IMPLoadToEEPROM();
int IMPLoadToRAM();
int impHelp();
int impOperation();

int phyReset();
int phyHelp();
int phyOperation();
int serdesHelp();
int serdesOperation();

int irlInit();
int irlInitRes();
int irlCustomSetup();
int irlReadRes();
int irlWriteRes();
int irlRReg();
int irlWReg();
int irlAdvConfig();
int irlAdvConfigDisable();
int irlHelp();
int irlOperation();

int vctCableDiag();
int vctExtenedStatus();
int vctHelp();
int vctOperation();

int qavSetXRate();
int qavGetXRate();
int qavSetXHiLimit();
int qavGetXHiLimit();
int qavHelp();
int qavOperation();

int ptpSetIntEn();
int ptpGetIntEn();
int ptpSetPortConfig();
int ptpGetPortConfig();
int ptpGetTimeStamp();
int ptpSetGlobalConfig();
int ptpGetGlobalConfig();
int ptpGetIntStatus();
int ptpSetPtpMode();
int ptpGetPtpMode();
int ptpStoreCompensation();
int ptpStoreAll();
int ptpCaptureAll();
int ptpSetPtpPulse();
int ptpGetPtpPulse();
int ptpHelp();
int ptpOperation();

int qosSetPrioMap();
int qosGetPrioMap();
int qosHelp();
int qosOperation();

int sysCtrlHelp();
int sysCtrlOperation();

int tcamFlushAll();
int tcamFlushEntry();
int tcamAddEntry();
int tcamReadEntry();
int tcamGetNextEntry();
int tcamFindEntry();
int tcamSetMode();
int tcamGetMode();
int tcamAddPort();
int tcamRemovePort();
int tcamEgrDelAllPorts();
int tcamEgrDelEntry();
int tcamEgrAddEntry();
int tcamEgrReadEntry();
int tcamEgrGetNextEntry();
int tcamAdvConfig();
int tcamHelp();
int tcamOperation();

int rmuGetID();
int rmuDumpAtu();
int rmuDumpMib2();
int rmuDumpTcam();
int rmuDumpMib();
int rmuTcamRead();
int rmuTcamWrite();
int rmuDumpReg();
int rmuHelp();
int rmuOperation();

int portGetVlanPorts();
int portSetVlanPorts();
int portSetCutThrough();
int portGetCutThrough();
int portCtrlOperation();
int portCtrlHelp();

int mibOperation();
int mibHelp();
int mibFlushAll();
int mibFlushPort();
int mibGetPortCounter();
int mibGetPortAllCounters();

int qcSetQosWeight();
int qcGetQosWeight();
int queueCtrlOperation();
int queueCtrlHelp();

int trunkHelp();
int trunkOperation();

int ledHelp();
int ledOperation();

int ReadRegister();
int WriteRegister();
/*
int ReadRegisterField();
int WriteRegisterField();

int setBusInterface();
*/
int getBusInterface();

int apiCLIHelp();
int execSystemCmd();
int exitProgram();
int parseCmd(char *cmd);
void apiCLI_alloc_buf();
void apiCLI_free_buf();
int parseCmd_cli(const char *command, char *argv[], int argc);

static int setPortU32U32();
static int getPortU32PU32();
static int setPortU32U32U32();
static int getPortU32U32PU32();
static int setPortU32();
static int getPortPU32();
static int setU32();
static int getPU32();
static int setDev();
static int getPortPU32PU32();

#ifdef __cplusplus
}
#endif

#endif