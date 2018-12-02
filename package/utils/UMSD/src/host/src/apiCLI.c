/*******************************************************************************
*                 Copyright 2016, MARVELL TECHNOLOGY GROUP LTD                 *
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
* apiCLI.c
*
* DESCRIPTION:
*       Command Line Interface for API debug
*
* DEPENDENCIES:   
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "msdApi.h"
#include "apiCLI.h"

#define MAX_ARGS        200
#define MAX_ARGS_LENGTH 200
#define MAX_CMD_LENGTH  2000
#define PROMPT_STRING "MCLI> "
static int logOnOffFlag = 0;
static char* g_filename = NULL;
static PRINT_CALLBACK printCallback = NULL;

void setPrintCallback(PRINT_CALLBACK callback)
{
    printCallback = callback;
}

static void CLIPrint(int level, char* prompt, char* format, ...)
{
	va_list argP;
    char dbgStr[1000] = {0};
	FILE *fp;

	va_start(argP, format);

    strncpy(dbgStr, prompt, strlen(prompt));
    vsprintf(dbgStr + strlen(prompt), format, argP);

    if (level > 0)
    {
        if (printCallback != NULL)
            printCallback(dbgStr);
        else
            printf("%s", dbgStr);
    }

    if (logOnOffFlag == 1)
    {
        fp = fopen("CLI.log", "a");
        if (fp == NULL)
        {
            return;
        }
        fprintf(fp, "%s%s", prompt, dbgStr);

        fclose(fp);
    }
}
#define CLI_ERROR(format, ...) CLIPrint(1, "(CLI_ERROR: ) ", format, ##__VA_ARGS__)
#define CLI_INFO(format, ...) CLIPrint(1, "", format, ##__VA_ARGS__)
#define CLI_CMD_TRACE(format, ...) CLIPrint(0, "(Running CMD: )", format, ##__VA_ARGS__)
#define CLI_WARNING(format, ...) CLIPrint(1, "(CLI_WARNING: ) ", format, ##__VA_ARGS__)

static char *CMD_ARGS[MAX_ARGS] = { 0 };
static char TempString[MAX_CMD_LENGTH] = { 0 };
static int nargs;
static int prompt_cnt = 0;
static int exit_flag;
static int file_flag;
static int strIcmp(const char* p1, const char* p2)
{
	while (*p1 && *p2)
	{
		char c1 = *p1;
		char c2 = *p2;
		if (c1 >= 'A' && c1 <= 'Z')
		{
			c1 += 'a' - 'A';
		}
		if (c2 >= 'A' && c2 <= 'Z')
		{
			c2 += 'a' - 'A';
		}
		if (c1 != c2)
		{
			return c1 - c2;
		}
		p1++;
		p2++;
	}
	return *p1 - *p2;
}
static char* syntaxCmd(const char* cmd, CLI_COMMAND_STRUCT *cmdList)
{
    int index = 0;

    memset(TempString, 0, MAX_CMD_LENGTH);
    while (1)
    {
        if ((cmdList+index)->callback == NULL)
            break;
        if (strIcmp(cmd, (cmdList + index)->cmd) == 0)
        {
            const char* temp = (cmdList + index)->help;
            char *temp1 = strchr(temp, ':');
            int len = strlen(temp) - strlen(temp1);
            memcpy(TempString, temp, len);
            break;
        }
        index++;
    }

    return TempString;
}

extern MSD_QD_DEV       *qddev;
extern MSD_U8	sohoDevNum;

static CLI_COMMAND_STRUCT *gCurrentCmd = NULL;

#ifdef UNITTEST
extern void runUnitTestCode(MSD_U8 devNum);
extern void runSampleCode(MSD_U8 devNum);
int UnitTestCases()
{
	runUnitTestCode(sohoDevNum);
	return 0;
}
int SampleCases()
{
	runSampleCode(sohoDevNum);
	return 0;
}
#endif
extern void runCustomizeCode(MSD_U8 devNum);
int CustomizedCases()
{
	runCustomizeCode(sohoDevNum);
	return 0;
}

CLI_PAIR_API_STRUCT pairCmdList[] =
{
    /*Qos methods*/
    { "setCosQueue",    (void*)msdQosPortCosToQueueSet,         "getCosQueue",      (void*)msdQosPortCosToQueueGet },
    { "setCosDscp",     (void*)msdQosPortCosToDscpMappingSet,   "getCosDscp",       (void*)msdQosPortCosToDscpMappingGet },
    { "setPriMapRule",  (void*)msdPortPrioRuleSet,              "getPriMapRule",    (void*)msdPortPrioRuleGet },
    { "setInitPri",     (void*)msdPortInitialPriSet,            "getInitPri",       (void*)msdPortInitialPriGet },
    { "setDefQPri",     (void*)msdPortDefQPriSet,               "getDefQPri",       (void*)msdPortDefQPriGet },
    { "setDefFPri",     (void*)msdPortDefFPriSet,               "getDefFPri",       (void*)msdPortDefFPriGet },
    { "enRewriteDscp",  (void*)msdQosPortRewriteDscp, "", NULL },

    /*Sys methods*/
    { "setFloodBC",     (void*)msdSysFloodBCSet,                "getFloodBC",       (void*)msdSysFloodBCGet },
    { "setCPUDest",     (void*)msdSysCPUDestSet,                "getCPUDest",       (void*)msdSysCPUDestGet },
    { "setIMD",         (void*)msdSysIngressMonitorDestSet,     "getIMD",           (void*)msdSysIngressMonitorDestGet },
    { "setEMD",         (void*)msdSysEgressMonitorDestSet,      "getEMD",           (void*)msdSysEgressMonitorDestGet },
    { "setMMC",         (void*)msdSysMonitorMgmtCtrlSet,        "getMMC",           (void*)msdSysMonitorMgmtCtrlGet },
    { "setRsvd2CpuEn",  (void*)msdSysRsvd2CpuEnablesSet,        "getRsvd2CpuEn",    (void*)msdSysRsvd2CpuEnablesGet },
    { "setMGMTPri",     (void*)msdSysMGMTPriSet,                "getMGMTPri",       (void*)msdSysMGMTPriGet },
    { "dumpGlobal1",    (void*)msdGlobal1RegDump,  "", NULL},
    { "dumpGlobal2",    (void*)msdGlobal2RegDump,  "", NULL },

    /*Port methods*/
    { "setDefVid",              (void*)msdPortDefaultVlanIdSet,         "getDefVid",            (void*)msdPortDefaultVlanIdGet },
    { "setQMode",               (void*)msdPort8021qModeSet,             "getQMode",             (void*)msdPort8021qModeGet },
    { "setDiscardTagged",       (void*)msdPortDiscardTaggedSet,         "getDiscardTagged",     (void*)msdPortDiscardTaggedGet },
    { "setDiscardUTagged",      (void*)msdPortDiscardUntaggedSet,       "getDiscardUTagged",    (void*)msdPortDiscardUntaggedGet},
    { "setUCFloodBlock",        (void*)msdPortUnicastFloodBlockSet,     "getUCFloodBlock",      (void*)msdPortUnicastFloodBlockGet},
    { "setMCFloodBlock",        (void*)msdPortMulticastFloodBlockSet,   "getMCFloodBlock",      (void*)msdPortMulticastFloodBlockGet },
    { "setBCMode",              (void*)msdPortDiscardBcastModeSet,      "getBCMode",            (void*)msdPortDiscardBcastModeGet },
    { "setIGMPSnoopEnable",     (void*)msdPortIGMPSnoopEnableSet,       "getIGMPSnoopEnable",   (void*)msdPortIGMPSnoopEnableGet },
    { "setHeaderMode",          (void*)msdPortHeaderModeSet,            "getHeaderMode",        (void*)msdPortHeaderModeGet },
    { "setDropOnLock",          (void*)msdPortDropOnLockSet,            "getDropOnLock",        (void*)msdPortDropOnLockGet },
    { "setMtu",                 (void*)msdPortMtuSet,                   "getMtu",               (void*)msdPortMtuGet },
    { "setLearnEnable",         (void*)msdPortLearnEnableSet,           "getLearnEnable",       (void*)msdPortLearnEnableGet },
    { "setVTUPriOverride",      (void*)msdPortVTUPriorityOverrideSet,   "getVTUPriOverride",    (void*)msdPortVTUPriorityOverrideGet },
    { "setSAPriOverride",       (void*)msdPortSAPriorityOverrideSet,    "getSAPriOverride",     (void*)msdPortSAPriorityOverrideGet },
    { "setDAPriOverride",       (void*)msdPortDAPriorityOverrideSet,    "getDAPriOverride",     (void*)msdPortDAPriorityOverrideGet },
    { "setMsgPort",             (void*)msdPortMessagePortSet,           "getMsgPort",           (void*)msdPortMessagePortGet },
    { "setEtherType",           (void*)msdPortEtherTypeSet,             "getEtherType",         (void*)msdPortEtherTypeGet },
    { "setAllowVidZero",        (void*)msdPortAllowVidZeroSet,          "getAllowVidZero",      (void*)msdPortAllowVidZeroGet },
    { "setEgrRate",             (void*)msdPortEgressRateSet,            "getEgrRate",           (void*)msdPortEgressRateGet },
    { "setFlowCtrl",            (void*)msdPortFlowControlEnableSet,     "getFlowCtrl",          (void*)msdPortFlowControlEnableGet },
    { "setEMS",                 (void*)msdPortEgressMonitorSourceEnableSet,     "getEMS",       (void*)msdPortEgressMonitorSourceEnableGet },
    { "setIMS",                 (void*)msdPortIngressMonitorSourceEnableSet,    "getIMS",       (void*)msdPortIngressMonitorSourceEnableGet },
    { "setForceDuplexMode",     (void*)msdPortForceDuplexModeSet,       "getForceDuplexMode",   (void*)msdPortForceDuplexModeGet },
    { "setForceLinkMode",       (void*)msdPortForceLinkModeSet,         "getForceLinkMode",     (void*)msdPortForceLinkModeGet },    
    { "setForceSpeed",          (void*)msdPortForceSpeedModeSet,        "getForceSpeed",        (void*)msdPortForceSpeedModeGet },
    { "setIgnoreFcs",           (void*)msdPortIgnoreFcsSet,             "getIgnoreFcs",         (void*)msdPortIgnoreFcsGet },
    { "setAllowBadFcs",         (void*)msdPortAllowBadFcsSet,           "getAllowBadFcs", (void*)msdPortAllowBadFcsGet },
    { "setPauseLimitOut",       (void*)msdPortPauseLimitOutSet,         "getPauseLimitOut", (void*)msdPortPauseLimitOutGet },
    { "setPauseLimitIn",        (void*)msdPortPauseLimitInSet,          "getPauseLimitIn", (void*)msdPortPauseLimitInGet },
    { "setQueueToPause",        (void*)msdPortQueueToPauseSet,          "getQueueToPause", (void*)msdPortQueueToPauseGet },
    { "setRxPriFCEnable",       (void*)msdPortRxPriorityFlowControlEnableSet,   "getRxPriFCEnable", (void*)msdPortRxPriorityFlowControlEnableGet },
    { "setRxPriFCToQueue",      (void*)msdPortRxPriorityFlowControlToQueueSet,  "getRxPriFCToQueue", (void*)msdPortRxPriorityFlowControlToQueueGet },
    { "setTxPriFCEnable",       (void*)msdPortTxPriorityFlowControlEnableSet,   "getTxPriFCEnable",  (void*)msdPortTxPriorityFlowControlEnableGet },
    { "setMapDA",               (void*)msdPortMapDASet,                 "getMapDA", (void*)msdPortMapDAGet },
    { "setARPMirror",           (void*)msdPortARPMirrorSet,             "getARPMirror", (void*)msdPortARPMirrorGet },
    { "setLocked",              (void*)msdPortLockedSet,                "getLocked", (void*)msdPortLockedGet },
    { "setHoldAt1",             (void*)msdPortHoldAt1Set,               "getHoldAt1", (void*)msdPortHoldAt1Get },
    { "setIntOnAgeOut",         (void*)msdPortIntOnAgeOutSet,           "getIntOnAgeOut", (void*)msdPortIntOnAgeOutGet },
    { "setRefreshLocked",       (void*)msdPortRefreshLockedSet,         "getRefreshLocked", (void*)msdPortRefreshLockedGet },
    { "setIgnoreWrongData",     (void*)msdPortIgnoreWrongDataSet,       "getIgnoreWrongData", (void*)msdPortIgnoreWrongDataGet },

    { "", NULL, "getOutQueueSize", (void*)msdPortOutQueueSizeGet },
    { "", NULL, "getLinkState", (void*)msdPortLinkStatusGet },
    { "", NULL, "getSpeed",     (void*)msdPortSpeedStatusGet },
    { "", NULL, "getDuplex",    (void*)msdPortDuplexStatusGet },
    { "setFrameMode", (void*)msdPortFrameModeSet,   "getFrameMode", (void*)msdPortFrameModeGet },
    { "setPortState", (void*)msdPortStateSet,       "getPortState", (void*)msdPortStateGet },
    { "portdump", (void*)msdPortRegDump, "", NULL },

    /*QC*/
    { "setQc",          (void*)msdPortQueueCtrlSet, "getQc",        (void*)msdPortQueueCtrlGet },
    { "setPortSched",   (void*)msdPortSchedSet,     "getPortSched", (void*)msdPortSchedGet },

    /*Trunk*/
    { "setTrunkMask",   (void*)msdTrunkMaskTableSet,    "getTrunkMask",     (void*)msdTrunkMaskTableGet },
    { "setTrunkHash",   (void*)msdTrunkHashTrunkSet,    "getTrunkHash",     (void*)msdTrunkHashTrunkGet },
    { "setTrunkRoute",  (void*)msdTrunkRoutingSet,      "getTrunkRoute",    (void*)msdTrunkRoutingGet },
    { "setLAGPort",     (void*)msdPortLAGPortSet,       "getLAGPort",       (void*)msdPortLAGPortGet },

    /*ATU methods*/
    { "setAging",           (void*)msdFdbAgingTimeoutSet,           "getAging",             (void*)msdFdbAgingTimeoutGet },
    { "setLearn2All",       (void*)msdFdbLearnOnAllDevicesSet,      "getLearn2All",         (void*)msdFdbLearnOnAllDevicesGet },
    { "setPortLearnLimit",  (void*)msdFdbPortLearnLimitSet,         "getPortLearnLimit",    (void*)msdFdbPortLearnLimitGet },
    { "", NULL,			"getLearnCount",            (void*)msdFdbPortLearnCountGet  },
    { "", NULL,			"getCount",                 (void*)msdFdbEntryCountGet  },
	{ "", NULL,			"getCountPerFid",           (void*)msdFdbEntryCountPerFidGet  },
    { "", NULL ,		"getNonStaticCount",        (void*)msdFdbEntryCountNonStaticGet},
	{ "", NULL,			"getNonStaticCountPerFid", (void*)msdFdbEntryCountNonStaticPerFidGet },
	{ "atudump",	(void*)msdFdbDump,	 "", NULL, },

    /*vtu dump*/
    { "vlandump", (void*)msdVtuDump, "", NULL},
    
    /*stu dump*/
    { "studump", (void*)msdStuDump, "", NULL},

    /*tcam dump*/
    { "tcamdump", (void*)msdTcamDump, "", NULL},

    /*Phy methods*/
    { "setLoopback",    (void*)msdPhyPortLoopbackSet,   "", NULL },
    { "setSpeed",       (void*)msdPhyPortSpeedSet,      "", NULL },
    { "powerDown",      (void*)msdPhyPortPowerDown,     "", NULL },
    { "setDuplex",      (void*)msdPhyPortDuplexModeSet, "", NULL },
    { "setAutoMode",    (void*)msdPhyPortAutoModeSet,   "", NULL },
    { "setEEE",         (void*)msdPhyEEESet,            "", NULL },
    { "rw",             (void*)msdSetSMIPhyReg,         "rr", (void*)msdGetSMIPhyReg },

    /*Serdes methods*/
    { "xrw",            (void*)msdSetSMIC45PhyReg,      "xrr", (void*)msdGetSMIC45PhyReg },

    /*MIBs*/
    { "setHistogramMode", (void*)msdStatsHistogramModeSet, "getHistogramMode", (void*)msdStatsHistogramModeGet },
    { "mibdump", (void*)msdPortMibDump, "", NULL },

    /*Qav*/
    { "setXRate", (void*)msdQavPortQpriXRateSet, "getXRate", (void*)msdQavPortQpriXRateGet },
    { "setXHiLimit", (void*)msdQavPortQpriXHiLimitSet, "getXHiLimit", (void*)msdQavPortQpriXHiLimitGet },

    /*PTP*/
    { "setPortPTPEn",           (void*)msdPtpPortPTPEnSet,          "getPortPTPEn",             (void*)msdPtpPortPTPEnGet },
    { "setMeanPathDelay",       (void*)msdPtpMeanPathDelaySet,      "getMeanPathDelay",         (void*)msdPtpMeanPathDelayGet },
    { "setIgrPathDelayAsym",    (void*)msdPtpIgrPathDelayAsymSet,   "getIgrPathDelayAsym",      (void*)msdPtpIgrPathDelayAsymGet },
    { "setEgrPathDelayAsym",    (void*)msdPtpEgrPathDelayAsymSet,   "getEgrPathDelayAsym",      (void*)msdPtpEgrPathDelayAsymGet },
    { "setMasterSlave",         (void*)msdPtpMasterSlaveSet,        "getMasterSlave",           (void*)msdPtpMasterSlaveGet },
    { "", NULL,         "getTODBusyBit",    (void*)msdPtpTODBusyBitStatusGet },
    { "", NULL,         "getPTPGlobalTime", (void*)msdGetPTPGlobalTime },
    { "", NULL,         "getTSValid",       (void*)msdPtpTSValidStGet },
    { "resetTS",        (void*)msdPtpTimeStampReset, "", NULL },

    /*LED*/
    { "forceOn",        (void*)msdLedForceOn,   "", NULL },
    { "forceOff",       (void*)msdLedForceOff,  "", NULL },
    { "setLinkAct",     (void*)msdLedLinkAct,   "", NULL },

    {"", NULL, "", NULL}
};

CLI_COMMAND_STRUCT cliCMDList[] =
{
	{ "file",	fileOperation,		"file <filename> : Load the command from the file\n"},
	{ "atu",	atuOperation,		"atu  <subcmd> : ATU operation\n"},
	{ "stu",	stuOperation,		"stu  <subcmd> : STU operation\n" },
	{ "vlan",	vtuOperation,		"vlan <subcmd> : VTU operation\n" },
	{ "imp",	impOperation,		"imp  <subcmd> : IMP operation\n" },
	{ "phy",	phyOperation,		"phy  <subcmd> : Phy operation\n" },
    { "serdes", serdesOperation,    "serdes <subcmd> : Serdes operation\n"},
	{ "irl",	irlOperation,		"irl  <subcmd> : IRL operation\n" },
	{ "vct",	vctOperation,		"vct  <subcmd> : VCT operation\n" },
    { "qav",    qavOperation,       "qav  <subcmd> : QAV operation\n" },
    { "ptp",    ptpOperation,       "ptp  <subcmd> : PTP operation\n" },
    { "qos",    qosOperation,       "qos  <subcmd> : Qos operation\n" },
	{ "tcam",   tcamOperation,		"tcam <subcmd> : TCAM operation\n" },
	{ "rmu",	rmuOperation,		"rmu  <subcmd> : RMU operation\n" },

    { "trunk",  trunkOperation,     "trunk <subcmd> : Trunk operation\n"},
	{ "port",   portCtrlOperation,  "port  <subcmd> : Port operation\n" },
	{ "mib",    mibOperation,       "mib   <subcmd> : MIBs operation\n" },
	{ "qc",     queueCtrlOperation, "qc    <subcmd> : Queue control operation\n" },
	{ "sys",    sysCtrlOperation,   "sys   <subcmd> : Sys operation\n" },
    { "led",    ledOperation,       "led   <subcmd> : Led operation\n"},

	{ "rr",		ReadRegister,		"rr  <devAddr> <regAddr> : Read register\n" },
	{ "rw",		WriteRegister,		"rw  <devAddr> <regAddr> <data> : Write register\n" },
	/*{ "rrf",	ReadRegisterField,	"rrf <devAddr> <regAddr> <offset> <size> : Read register field\n"},
	{ "rwf",	WriteRegisterField, "rwf <devAddr> <regAddr> <offset> <size> <data> : Write register field\n"},

	{ "setBus",	setBusInterface,	"setBus <busInterface> : Set the bus interface, SMI, RMU or SMI multichip\n"},*/
	{ "getBus",	getBusInterface,	"getBus : Get the bus interface, SMI, RMU or SMI multichip\n" },

#ifdef UNITTEST
	{ "unittest", UnitTestCases,     "unittest   : Run the Unit test cases\n" },
	{ "sample", SampleCases,         "sample     : Run the sample cases\n" },
#endif	
	{ "customized", CustomizedCases, "customized : Run customized cases\n" },

	{ "exec",   execSystemCmd,		"exec <systemCmd> : exec the system command\n"},
	{ "exit",	exitProgram,		"exit : Exit command prompt\n"},
	{ "help",	apiCLIHelp,			"help : Print list of commands\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT fdbCMDList[] =
{
	{ "addEntry", atuAddEntry, "atu addEntry [options] : Creates the new entry in MAC address table.\n"},
	{ "getNext", atuGetNextEntry, "atu getNext <macAddr> <fid>: get next atu entry\n" },	
	{ "delAll", atuFlush, "atu delAll <deleteCMD> : delete all or all non-static atu entry\n"},
	{ "delAllInDB", atuFlushInDB, "atu delAllInDB <deleteCMD> <fid>: delete all atu entry based on the DB number\n" },
	{ "move", atuMove, "atu move <moveCMD> <moveFrom> <moveTo>: move all or all non-static atu entry\n" },
	{ "moveInDB", atuMoveInDB, "atu moveInDB <moveCMD> <fid> <moveFrom> <moveTo>: move the atu entry based on fid\n" },
	{ "delEntry", atuDelEntry, "atu delEntry <macAddr> <fid> : delete the atu entry\n" },
	{ "getVio", atuGetVio, "atu getVio : get the atu Violation data\n" },
	{ "findEntry", atuFindEntry, "atu findEntry <macAddr> <fid> : find atu entry\n" },
	{ "setAging", setU32, "atu setAging <timeout> : set the atu entry aging time out in millisecond\n" },
    { "getAging", getPU32, "atu getAging : get the atu entry aging time out in millisecond\n"},

    { "portRemove", atuPortRemove , "atu portRemove <RemoveCMD> <port> : remove all or all non-static addresses from a port\n"},
    { "portRemoveInDB", portRemoveINDB, "atu portRemoveInDB <RemoveCMD> <fid> <port> : remove all or all non-static addresses from a port in the specified ATU fid\n"},
    { "getLearn2All", getPU32, "atu getLearn2All : get the learn on all devices status\n"},
    { "setLearn2All", setU32, "atu setLearn2All <en> : set the learn on all devices\n"},
    { "setPortLearnLimit", setPortU32, "atu setPortLearnLimit <port> <limit> : Set auto learning limit for specified port of a specified device.\n"},
    { "getPortLearnLimit", getPortPU32, "atu getPortLearnLimit <port> : Get auto learning limit for specified port of a specified device.\n"},
    { "getLearnCount", getPortPU32, "atu getLearnCount <port>: Read the current number of active unicast MAC addresses associated with the given port\n"},
    { "getCount", getPU32, "atu getCount : Counts all entries in the Address Translation Unit.\n"},
    { "getCountPerFid", getPortPU32, "atu getCountPerFid <fid> : Counts all non-static entries in the specified fid in the Address Translation Unit\n"},
    { "getNonStaticCount", getPU32, "atu getNonStaticCount : Counts all non-static entries in the Address Translation Unit.\n"},
    { "getNonStaticCountPerFid", getPortPU32, "atu getNonStaticCountPerFid <fid> : Counts all non-static entries in the specified fid in the Address Translation Unit.\n"},
	{ "dump", setU32, "atu dump <fid> : Find all MAC address in the specified fid and print them out.\n" },/*The dump command will print out the info, no out para*/

	{ "help", atuHelp, "atu help [subcmd]: Display atu command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT stuCMDList[] = 
{
	{ "addEntry", stuAddEntry, "stu addEntry [options] : add stu entry\n" },
    { "getNext", stuGetNext, "stu getNext <sid> : get next valid stu entry\n"},
	{ "findEntry", stuFindEntry, "stu findEntry <sid> : find stu entry with input sid\n" },
	{ "delEntry", stuDelEntry, "stu delEntry <sid>: delete the stu entry\n" },
    { "getCount", stuGetEntryCount, "stu getCount : get number of valid entries in the STU table\n"},
    { "dump", setDev, "stu dump : Find all the stu entries and print them out\n"},

	{ "help", stuHelp, "stu help [subcmd] : Display stu command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT vtuCMDList[] =
{
	{ "addEntry", vtuAddEntry, "vlan addEntry [options] : add vlan entry\n" },
	{ "delAll", vtuFlush, "vlan delAll : flush the vlan entry\n" },
	{ "findEntry", vtuFindEntry, "vlan findEntry <vlanId> : find vtu entry\n" },
    { "isExist", vutIsExist, "vlan isExist <vlanId> : check if the vlan entry exist\n"},
    { "getNext", vtuGetNext, "vlan getNext <vlanId> : Get next vlan entry\n"},
	{ "delEntry", vtuDelEntry, "vlan delEntry <vlanId>: delete the vlan entry\n" },
	{ "setMode", vtuSetMode, "vlan setMode <modeValue>: set the vlan mode\n" },
	{ "getMode", vtuGetMode, "vlan getMode : get the vlan mode\n"},
	{ "getVio", vtuGetVio, "vlan getVio : get the vlan Violation data\n" },
    { "getCount", vtuGetCount, "vlan getCount : get the valid entry count in the VTU table\n"},
    { "setMemberTag", vtuSetMemberTag, "vlan setMemberTag <vlanId> <portNum> <memberTag> : update the vlan entry member tag\n"},
    { "getMemberTag", vtuGetMemberTag, "vlan getMemberTag <vlanId> <portNum> : get the memberTag for special port if it exist\n"},
    { "dump", setDev, "vlan dump : Find all the vlan entries and print them out\n"},

	{ "help", vtuHelp, "vlan help [subcmd] : Display vlan command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT impCMDList[] = 
{
	{ "run", IMPRun, "imp run <addr> : run the imp with specified address\n"},
	{ "reset", IMPReset, "imp reset : reset imp module\n"},
	{ "stop", IMPStop, "imp stop : stop the imp\n"},
	{ "writeEEPROM", IMPWriteEEPROM, "imp writeEEPROM <addr> <data>: write data to EEPROM\n"},
	{ "loadToEEPROM", IMPLoadToEEPROM, "imp loadToEEPROM  <filename>: load ihx file to eeprom\n"},
	{ "loadToRAM", IMPLoadToRAM, "imp loadToRAM <filename>: load ihx file data to ram\n"},
	{ "help", impHelp, "imp help [subcmd]: Display imp command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT phyCMDList[] = 
{
	{ "reset", phyReset, "phy reset <port>: phy reset per port\n"},
    { "setLoopback", setPortU32, "phy setLoopback <port> <enable> : Enable/Disable Internal Port Loopback.\n" },
    { "setSpeed", setPortU32, "phy setSpeed <port> <speed> : set speed per port.\n" },
    { "powerDown", setPortU32, "phy powerDown <port> <state> : Enable/disable (power down).\n" },
    { "setDuplex", setPortU32, "phy setDuplex <port> <mode> : Sets duplex mode per port.\n" },
    { "setAutoMode", setPortU32, "phy setAutoMode <port> <mode> : Sets auto mode per port.\n" },
    { "setEEE", setPortU32, "phy setEEE <port> <en>: Enable or disable the energy efficient ethernet feature per port. \n"},
    
    { "rr", getPortU32PU32, "phy rr <devAddr> <regAddr> : Indirectly read internal PHY register through SMI PHY command.\n" },
    { "rw", setPortU32U32, "phy rw <devAddr> <regAddr> <data> : Indirectly write internal PHY register through SMI PHY command.\n"},    

	{ "help", phyHelp, "phy help [subcmd]: Display phy command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT serdesCMDList[] =
{
    {"xrr", getPortU32U32PU32, "serdes xrr <devAddr> <phyAddr> <regAddr> : Indirectly read internal Serdes register through SMI C45 command.\n"},
    {"xrw", setPortU32U32U32, "serdes xrw <devAddr> <phyAddr> <regAddr> <data> : Indirectly write internal Serdes register through SMI C45 command.\n"},

    { "help", serdesHelp, "serdes help [subcmd]: Display serdes command options\n" },
    {"", NULL, ""}
};

CLI_COMMAND_STRUCT irlCMDList[] = 
{
	{ "reset", irlInit, "irl reset : initializes all PIRL Resources for all ports\n"},
	{ "initResCount", irlInitRes, "irl initResCount <port> <res>: initializes the selected PIRL Resource counter per port\n"},
	{ "customSetup", irlCustomSetup, "irl customSetup <tgtRate> <tgtBstSize> <countMode> : Calculate CBS/EBS/BI/BRFGrn/BRFYel\n"},
	{ "readRes", irlReadRes, "irl readRes <port> <res> : read Resource bucket parameter\n"},
	{ "writeRes", irlWriteRes, "irl writeRes <port> <res> [options]: writes Resource bucket parameter\n"},
	{ "rr", irlRReg, "irl rr <port> <res> <offset>: read general IRL register value\n"},
	{ "rw", irlWReg, "irl rw <port> <res> <offset> <data>: write general IRL register value\n"},
    { "advConfig", irlAdvConfig, "irl advConfig <port> <res> <tgtRate> <tgtBstSize> <countMode> [options] : config ingress rate limit based on packet type or priority the given resource of the port\n"},
    { "disableAdvConfig", irlAdvConfigDisable, "irl disableAdvConfig <port> <res> : clear config for ingress rate limit to the given resource of the port\n"},
	{ "help", irlHelp, "irl help [help]: Display irl command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT vctCMDList[] =
{
	{ "cableDiag", vctCableDiag, "vct cableDiag <port> <modeOptions> : get the status per MDI pair\n"},
	{ "extenedStatus", vctExtenedStatus, "vct extenedStatus <port> : retrieves extended cable status\n"},
	{ "help", vctHelp, "vct help [subcmd]: Display vct command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT qavCMDList[] = 
{
    { "setXRate", setPortU32U32, "qav setXRate <port> <queue> <rate> : set priority queue rate on a port\n"},
    { "getXRate", getPortU32PU32, "qav getXRate <port> <queue> : get priority queue rate per prot\n"},
    { "setXHiLimit", setPortU32U32, "qav setXHiLimit <port> <queue> <hiLimit> : set priority queue HiLimit on a port\n" },
    { "getXHiLimit", getPortU32PU32, "qav getXHiLimit <port> <queue> : get priority queue HiLimit on a port\n" },
    { "help", qavHelp, "qav help [subcmd] : Display qav command options\n"},
    { "", NULL, ""}
};

CLI_COMMAND_STRUCT ptpCMDList[] = 
{
    { "setPortPTPEn", setPortU32, "ptp setPortPTPEn <port> <en> : enables or disables PTP on a port\n" },
    { "getPortPTPEn", getPortPU32, "ptp getPortPTPEn <port> : get the PTP enable status on a port\n"},
    { "setIntEn", ptpSetIntEn, "ptp setIntEn <port> [options] : set port arrival/departure interrupt enable/disable\n"},
    { "getIntEn", ptpGetIntEn, "ptp getIntEn <port> : get port arrival/departure interrupt enable/disable status\n"},
    { "setPortConfig", ptpSetPortConfig, "ptp setPortConfig <port> [options] : set PTP port configuration parameters\n"},
    { "getPortConfig", ptpGetPortConfig, "ptp getPortConfig <port> : get PTP port configuration parameters\n"},
    { "getTimeStamp", ptpGetTimeStamp, "ptp getTimeStamp <port> <timeToRead> : get PTP port time stamp value\n"},
    { "getTSValid", getPortU32PU32, "ptp getTSValid <port> <timeToRead> : get PTP time valid bit\n"},
    { "resetTS", setPortU32, "ptp resetTS <port> <timeToReset> : reset PTP time valid bit\n"},
    { "setGlobalConfig", ptpSetGlobalConfig, "ptp setGlobalConfig [options] : set PTP global configuration parameters\n"},
    { "getGlobalConfig", ptpGetGlobalConfig, "ptp getGlobalConfig : get PTP global configuration parameters\n"},
    { "getIntStatus", ptpGetIntStatus, "ptp getIntStatus : get PTP interrupt status\n"},
    { "setMeanPathDelay", setPortU32, "ptp setMeanPathDelay <port> <delay>: Set ingress mean path delay on a port\n"},
    { "getMeanPathDelay", getPortPU32, "ptp getMeanPathDelay <port>: Get ingress mean path delay on a port\n"},
    { "setIgrPathDelayAsym", setPortU32, "ptp setIgrPathDelayAsym <port> <asym>: Set ingress path delay asymmetry on a port\n"},
    { "getIgrPathDelayAsym", getPortPU32, "ptp getIgrPathDelayAsym <port> : Get ingress path delay asymmetry on a port\n"},
    { "setEgrPathDelayAsym", setPortU32, "ptp setEgrPathDelayAsym <port> <asym>: Set egress path delay asymmetry on a port\n" },
    { "getEgrPathDelayAsym", getPortPU32, "ptp getEgrPathDelayAsym <port> : Get egress path delay asymmetry on a port\n" },
    { "setPtpMode", ptpSetPtpMode, "ptp setPtpMode [options] : Set PTP mode configuration\n" },
    { "getPtpMode", ptpGetPtpMode, "ptp getPtpMode : Get PTP mode configuration\n"},
    { "setMasterSlave", setU32, "ptp setMasterSlave <role> : Set PTP device as Master or Slave\n"},
    { "getMasterSlave", getPU32, "ptp getMasterSlave : Get PTP device role, 0 for slave, 1 for master\n"},
    { "getTODBusyBit", getPU32, "ptp getTODBusyBit : Get the busy bit status to see if the Hardware is busy\n"},
    { "storeCompensation", ptpStoreCompensation, "ptp storeCompensation <Index> [options] : Store Compensation register only to selected TimeArray\n"},
    { "storeAll", ptpStoreAll, "ptp storeAll <Index> [options] : Store all register to selected TimeArray\n" },
    { "captureAll", ptpCaptureAll, "ptp captureAll <Index> : Capture selected TimeArray\n"},
    { "setPtpPulse", ptpSetPtpPulse, "ptp setPtpPulse [options] : Set PTP Pulse parameters\n"},
    { "getPtpPulse", ptpGetPtpPulse, "ptp getPtpPulse : Get PTP Pulse parameters.\n"},
    { "getPTPGlobalTime", getPU32, "ptp getPTPGlobalTime : Get the global timer value that is running of the free running clock\n"},


    { "help", ptpHelp, "ptp help [subcmd] : Display ptp command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT qosCMDList[] = 
{
    { "setPrioMap", qosSetPrioMap, "qos setPrioMap <port> <table> <entryPtr> [option] : Set port IEEE Priority Mapping\n"},
    { "getPrioMap", qosGetPrioMap, "qos getPrioMap <port> <table> <entryPtr> : Get port IEEE Priority Mapping\n"},
    { "setCosQueue", setPortU32U32, "qos setCosQueue <port> <cos> <queue> : Update the COS to egress queue mapping\n"},
    { "getCosQueue", getPortU32PU32, "qos getCosQueue <port> <cos> : Get the physical queue to be mapped for trusted or assigned cos\n" },
    { "setCosDscp", setPortU32U32, "qos setCosDscp <port> <cos> <dscp> : Update the COS to DSCP mapping. This will be used for rewriting DSCP for IPv4/IPv6 packets based on the assigned cos value.\n"},
    { "getCosDscp", getPortU32PU32, "qos getCosDscp <port> <cos> :  Get the DSCP for IPv4/IPv6 packets based on the assigned cos value.\n"},
    { "setPriMapRule", setPortU32, "qos setPriMapRule <port> <mode> : Sets priority mapping rule\n" },
    { "getPriMapRule", getPortPU32, "qos getPriMapRule <port> : Gets priority mapping rule\n" },
    { "setInitPri", setPortU32, "qos setInitPri <port> <mode> : Sets initial Priority assignment\n" },
    { "getInitPri", getPortPU32, "qos getInitPri <port> : Gets initial Priority assignment\n" },
    { "setDefQPri", setPortU32, "qos setDefQPri <port> <mode> : Set Default queue priority\n" },
    { "getDefQPri", getPortPU32, "qos getDefQPri <port> : Get Default queue priority\n" },
    { "setDefFPri", setPortU32, "qos setDefFPri <port> <mode> : Set Default frame priority\n" },
    { "getDefFPri", getPortPU32, "qos getDefFPri <port> : Get Default frame priority\n" },
    { "enRewriteDscp", setPortU32, "qos enRewriteDscp <port> <en> : enable/disable dscp re-write at egress on the packet\n"},
    { "help", qosHelp, "qos help [subcmd] : Display qos command options\n" },

    { "", NULL, "" }
};

CLI_COMMAND_STRUCT tcamCMDList[] = 
{
	{ "delAll", tcamFlushAll, "tcam delAll : flush all entries\n"},
	{ "delEntry", tcamFlushEntry, "tcam delEntry <entryNum> : flush one single entry\n"},
	{ "addEntry", tcamAddEntry, "tcam addEntry <entryNum> [options] : load one entry\n"},
	{ "readEntry", tcamReadEntry, "tcam readEntry <entryNum> : read one entry data\n"},
	{ "getNextEntry", tcamGetNextEntry, "tcam getNextEntry <entryNum> : get next valied entry\n"},
	{ "findEntry", tcamFindEntry, "tcam findEntry <entryNum> : find one entry\n"},
    { "setMode", tcamSetMode, "tcam setMode <port> <mode> : set tcam mode per port\n"},
    { "getMode", tcamGetMode, "tcam getMode <port> : get tcam mode per port\n"},
    { "addPort", tcamAddPort, "tcam addPort <entryNum> <port> : Add a port to an existing tcam entry\n"},
    { "removePort", tcamRemovePort, "tcam removePort <entryNum> <port> : Remove a port to an existing tcam entry\n"},
	{ "egrDelAllPorts", tcamEgrDelAllPorts, "tcam egrDelAllPorts <entryNum>: flush one egress entries for all the ports\n" },
	{ "egrDelEntry", tcamEgrDelEntry, "tcam egrDelEntry <port> <entryNum> : flush one single egress entry per port\n" },
	{ "egrAddEntry", tcamEgrAddEntry, "tcam egrAddEntry <port> <entryNum> [options] : load one egress entry\n" },
	{ "egrReadEntry", tcamEgrReadEntry, "tcam egrReadEntry <port> <entryNum> : read one egress entry data\n" },
	{ "egrGetNextEntry", tcamEgrGetNextEntry, "tcam egrGetNextEntry <port> <entryNum> : get next valied egress entry\n" },
    { "advanceConfig", tcamAdvConfig, "tcam advanceConfig <pktType> <entryNum1> <entrynum2> [options] : Set tcam entry to match standerd IPv4/IPv6, TCP/UDP packets with given header and mask\n"},
    { "dump", setDev, "tcam dump : Find all the tcam entries and print them out\n"},

	{ "help", tcamHelp, "tcam help [subcmd] : Display tcam command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT rmuCMDList[] = 
{
	{ "getID", rmuGetID, "rmu getID : Get the current Device ID\n"},
	{ "dumpAtu", rmuDumpAtu, "rmu dumpAtu <continueCode> : Dump ATU entry from the specified starting address\n"},	
	{ "dumpMib2", rmuDumpMib2, "rmu dumpMib2 <enClearOnRead> <port> : Dump MIB2 from the specified port\n" },
	{ "dumpTcam", rmuDumpTcam, "rmu dumpTcam <continueCode> : Dump valid TCAM entries\n" },
	{ "dumpMib", rmuDumpMib, "rmu dumpMib <enClearOnRead> <port> : Dump MIB from the specified port, not include Bank1\n" },
	{ "tcamRead", rmuTcamRead, "rmu tcamRead <entryNum> : single TCAM entry read-back\n"},
	{ "tcamWrite", rmuTcamWrite, "rmu tcamWrite <entryNum> [options] : single TCAM entry load\n" },
    { "dumpReg", rmuDumpReg, "rmu dumpReg <devAddr> : Dump 32 registers for the specified device address\n"},
	
	{ "help", rmuHelp, "rmu help [subcmd]: Display rmu command options\n" },
	{ "", NULL, "" }
};

CLI_COMMAND_STRUCT sysCtrlCMDList[] =
{
    { "setFloodBC",     setU32, "sys setFloodBC <en> : Sets Flood Broadcast\n" },
    { "getFloodBC",     getPU32, "sys getFloodBC : Gets Flood Broadcast state \n" },
    { "setCPUDest",     setU32, "sys setCPUDest <port> : Sets CPU Destination Port\n" },
    { "getCPUDest",     getPU32, "sys getCPUDest : Gets CPU Destination Port \n" },
    { "setIMD",         setU32, "sys setIMD <port> : Sets Ingress Monitor Destination Port\n" },
    { "getIMD",         getPU32, "sys getIMD : Gets Ingress Monitor Destination Port\n" },
    { "setEMD",         setU32, "sys setEMD <port> : Sets Egress Monitor Destination Port\n" },
    { "getEMD",         getPU32, "sys getEMD : Gets Egress Monitor Destination Port\n" },
    { "setMMC",         setPortU32, "sys setMMC <point> <data> :  Set Monitor and mgmt control data\n" },
    { "getMMC",         getPortPU32, "sys getMMC <point> : Get Monitor and mgmt control data\n" },
    { "setRsvd2CpuEn",  setU32, "sys setRsvd2CpuEn <enBits> : Reserved DA Enables\n" },
    { "getRsvd2CpuEn",  getPU32, "sys getRsvd2CpuEn : Get Reserved DA Enables state\n" },
    { "setMGMTPri",     setU32, "sys setMGMTPri <pri> : pri - PRI[2:0] bits (should be less than 8)\n" },
    { "getMGMTPri",     getPU32, "sys getMGMTPri : Get pri\n" },
    { "dumpGlobal1",    setDev, "sys dumpGlobal1 : Reads all the Global1 registers and print them out\n"},
    { "dumpGlobal2",    setDev, "sys dumpGlobal2 : Reads all the Global2 registers and print them out\n"},

    { "help",           sysCtrlHelp, "sys help [subcmd]: Display sys command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT portCtrlCMDList[] =
{
    { "setDefVid", setPortU32, "port setDefVid <port> <vid> : Set the port default vlan id\n" },
    { "getDefVid", getPortPU32, "port getDefVid <port> : Get the port default vlan id\n" },
    { "setQMode", setPortU32, "port setQMode <port> <mode> : Sets the IEEE 802.1q mode for this port\n" },
    { "getQMode", getPortPU32, "port getQMode <port> : Gets the IEEE 802.1q mode for this port\n" },
    { "setDiscardTagged", setPortU32, "port setDiscardTagged <port> <en> : Set discard tagged frame,1 to discard tagged frame, 0 otherwise\n" },
    { "getDiscardTagged", getPortPU32, "port getDiscardTagged <port> : Get discard tagged frame,1 to discard tagged frame, 0 otherwise\n" },
    { "setDiscardUTagged", setPortU32, "port setDiscardUTagged <port> <en> : Set discard untagged frame,1 to discard untagged frame, 0 otherwise\n" },
    { "getDiscardUTagged", getPortPU32, "port getDiscardUTagged <port> : Get discard tagged untagged,1 to discard untagged frame, 0 otherwise\n" },
    { "setUCFloodBlock", setPortU32, "port setUCFloodBlock <port> <en> : Set Egress Flooding mode for unitcaset of a switch port.\n" },
    { "getUCFloodBlock", getPortPU32, "port getUCFloodBlock <port> : Get Egress Flooding mode for unitcaset of a switch port\n" },
    { "setMCFloodBlock", setPortU32, "port setMCFloodBlock <port> <en> : Set Egress Flooding mode for multicast of a switch port.\n" },
    { "getMCFloodBlock", getPortPU32, "port getMCFloodBlock <port> : Get Egress Flooding mode for multicast of a switch port.\n" },
    { "setBCMode", setPortU32, "port setBCMode <port> <en> : Sets the Discard Broadcast mode\n" },
    { "getBCMode", getPortPU32, "port getBCMode <port> : Gets the Discard Broadcast mode\n" },
    { "setIGMPSnoopEnable", setPortU32, "port setIGMPSnoopEnable <port> <en> : Sets the Enable/disable trapping all IPV4 IGMP packets to the CPU,based on their ingress port.\n"},
    { "getIGMPSnoopEnable", getPortPU32, "port getIGMPSnoopEnable <port>  : Gets the Enable/disable trapping all IPV4 IGMP packets to the CPU,based on their ingress port.\n" },
    { "setHeaderMode", setPortU32, "port setHeaderMode <port> <en> : Sets the ingress and egress header mode of a switch port\n"},
    { "getHeaderMode", getPortPU32, "port getHeaderMode <port> : Gets the ingress and egress header mode of a switch port\n"},
    { "setDropOnLock", setPortU32, "port setDropOnLock <port> <en> : Sets the DropOnLock mode of specified port on specified device\n"},
    { "getDropOnLock", getPortPU32, "port getDropOnLock <port> : Gets the DropOnLock mode of specified port on specified device\n"},
    { "setMtu", setPortU32, "port setMtu <port> <mode> : Set the max frame size allowed to be received and transmitted from or to a given port\n" },
    { "getMtu", getPortPU32, "port getMtu <port> : Get the max frame size allowed to be received and transmitted from or to a given port\n" },
    { "setLearnEnable", setPortU32, "port setLearnEnable <port> <en> : Enables/disables automatic learning of new source MAC\n" },
    { "getLearnEnable", getPortPU32, "port getLearnEnable <port> : Gets LearnEnable setup status\n" },
    { "setVTUPriOverride", setPortU32, "port setVTUPriOverride <port> <mode> : Set VTU Priority Override of specified port on specified device\n"},
    { "getVTUPriOverride", getPortPU32, "port getVTUPriOverride <port> : Get VTU Priority Override of specified port on specified device\n"},
    { "setSAPriOverride", setPortU32, "port setSAPriOverride <port> <mode> : Set SA Priority Override of specified port on specified device\n"},
    { "getSAPriOverride", getPortPU32, "port getSAPriOverride <port> : Get SA Priority Override of specified port on specified device\n"},
    { "setDAPriOverride", setPortU32, "port setDAPriOverride <port> <mode> : Set DA Priority Override of specified port on specified device\n" },
    { "getDAPriOverride", getPortPU32, "port getDAPriOverride <port> : Get DA Priority Override of specified port on specified device\n" },
    { "setMsgPort", setPortU32, "port setMsgPort <port> <en> : Set message port for specific port on a specific device\n"},
    { "getMsgPort", getPortPU32, "port getMsgPort <port> : Get if one specified port is message port\n"},
    { "setEtherType", setPortU32, "port setEtherType <port> <eType> : Sets the port's special Ether Type\n"},
    { "getEtherType", getPortPU32, "port getEtherType <port> : Gets the port's special Ether Type\n"},
    { "setAllowVidZero", setPortU32, "port setAllowVidZero <port> <en> : Set allow VID of Zero of specified port on specified device\n"},
    { "getAllowVidZero", getPortPU32, "port getAllowVidZero <port> :  Get allow VID of Zero of specified port on specified device\n"},
    { "setEgrRate", setPortU32U32, "port setEgrRate <port> <mode> <rate> : Sets the port's egress data limit\n" },
    { "getEgrRate", getPortPU32PU32, "port getEgrRate <port> : Gets the port's egress data limit mode and limit value\n" },
    { "setFlowCtrl", setPortU32U32, "port setFlowCtrl <port> <isEnable> <mode> : Enable/disable port flow control and set flow control mode\n" },
    { "getFlowCtrl", getPortPU32PU32, "port getFlowCtrl <port> : Get switch port flow control enable/disable status and return flow control mode\n" },
    { "setEMS", setPortU32, "port setEMS <port> <mode> : 1 to set EgressMonitorSource, 0 otherwise\n" },
    { "getEMS", getPortPU32, "port getEMS <port> : Gets EgressMonitorSource bit for the given port\n" },
    { "setIMS", setPortU32, "port setIMS <port> <mode> : 1 to set IngressMonitorSource, 0 otherwise\n" },
    { "getIMS", getPortPU32, "port getIMS <port> : Gets IngressMonitorSource bit for the given port\n" },
    { "setForceDuplexMode", setPortU32, "port setForceDuplexMode <port> <mode> : Set the port duplex mode\n"},
    { "getForceDuplexMode", getPortPU32, "port getForceDuplexMode <port> : Get the port duplex mode\n"},
    { "getDuplex", getPortPU32, "port getDuplex <port> : Retrives the port duplex mode\n" },
    { "setForceLinkMode", setPortU32, "port setForceLinkMode <port> <mode> : Set the force link mode per port\n"},
    { "getForceLinkMode", getPortPU32, "port getForceLinkMode <port> : Get the force link mode per port\n"},
    { "getLinkState", getPortPU32, "port getLinkState <port> : Retrives the link state\n" },
    { "setForceSpeed", setPortU32, "port setForceSpeed <port> <mode> : Forces switch MAC speed\n" },
    { "getForceSpeed", getPortPU32, "port getForceSpeed <port> : Retrieves switch MAC Force Speed value\n" },
    { "getSpeed", getPortPU32, "port getSpeed <port> : Retrives the port MAC speed\n" },
    { "setVlanPorts", portSetVlanPorts, "port setVlanPorts <port> <memPortsArray> : Set port based vlan table of a specified port\n"},
    { "getVlanPorts", portGetVlanPorts, "port getVlanPorts <port> : Get frame mode of a specified port\n"},
    { "setFrameMode", setPortU32, "port setFrameMode <port> <mode> : Set the expected Ingress and the generated Egress tagging frame format\n"},
    { "getFrameMode", getPortPU32, "port getFrameMode <port> : Get frame mode of a specified port\n"},
    { "setPortState", setPortU32, "port setPortState <port> <mode> : Set the port state\n"},
    { "getPortState", getPortPU32, "port getPortState <port> : Get the port state\n"},
    { "setIgnoreFcs", setPortU32, "port setIgnoreFcs <port> <en> : Enable or disable force good FCS\n"},
    { "getIgnoreFcs", getPortPU32, "port getIgnoreFcs <port> : Get the force good FCS status\n"},
    { "setAllowBadFcs", setPortU32, "port setAllowBadFcs <port> <en> : Set allow bad fcs to receiving frames or not\n"},
    { "getAllowBadFcs", getPortPU32, "port getAllowBadFcs <port> : Get the allow bad fcs status\n"},
    { "setPauseLimitOut", setPortU32, "port setPauseLimitOut <port> <limit> : Set pause refresh frames transmit limit\n"},
    { "getPauseLimitOut", getPortPU32, "port getPauseLimitOut <port> : Get pause refresh frames transmit limit\n"},
    { "setPauseLimitIn", setPortU32, "port setPauseLimitIn <port> <limit> : Set pause refresh frames receive limit\n" },
    { "getPauseLimitIn", getPortPU32, "port getPauseLimitIn <port> : Get pause refresh frames receive limit\n" },
    { "setQueueToPause", setPortU32, "port setQueueToPause <port> <queue> : Set which queue paused when receiving a pause frame\n"},
    { "getQueueToPause", getPortPU32, "port getQueueToPause <port> : Get queue value that is used to determine which queue pasued\n"},
    { "setRxPriFCEnable", setPortU32U32, "port setRxPriFCEnable <port> <pfcPri> <en> : Decides which port pay attention for specified PFC pri when recevie pause frame\n"},
    { "getRxPriFCEnable", getPortU32PU32, "port getRxPriFCEnable <port> <pfcPri> : Get the status for one port with one pri pay attention for pause frame or not\n"},
    { "setRxPriFCToQueue", setPortU32U32, "port setRxPriFCToQueue <port> <pfcPri> <queue> : Set receive priority flow control frame's priority\n"},
    { "getRxPriFCToQueue", getPortU32PU32, "port getRxPriFCToQueue <port> <pfcPri> : Get receive priority flow control frame's priority\n"},
    { "setTxPriFCEnable", setPortU32U32, "port setTxPriFCEnable <port> <pfcPri> <en> : Set transmit priority flow control enable\n" },
    { "getTxPriFCEnable", getPortU32PU32, "port getTxPriFCEnable <port> <pfcPri> : Get transmit priority flow control enable\n" },
    { "getOutQueueSize", getPortU32PU32, "port getOutQueueSize <port> <queue> : Get egress queue size counter value\n"},
    { "setMapDA", setPortU32, "port setMapDA <port> <en> : Set map DA mode\n"},
    { "getMapDA", getPortPU32, "port getMapDA <port> : Get map DA mode\n"},
    { "setARPMirror", setPortU32, "port setARPMirror <port> <en> : Set ARP mirror enable status\n"},
    { "getARPMirror", getPortPU32, "port getARPMirror <port> : Get ARP mirror enable status\n"},
    { "setLocked", setPortU32, "port setLocked <port> <en> : Enable or disable port locked mode\n"},
    { "getLocked", getPortPU32, "port getLocked <port> : Get port locked mode status\n"},
    { "setHoldAt1", setPortU32, "port setHoldAt1 <port> <en> : Enable or disable port HoldAt1 mode\n" },
    { "getHoldAt1", getPortPU32, "port getHoldAt1 <port> : Get port HoldAt1 mode status\n" },
    { "setIntOnAgeOut", setPortU32, "port setIntOnAgeOut <port> <en> : Enable or disable interrupt on age out\n" },
    { "getIntOnAgeOut", getPortPU32, "port getIntOnAgeOut <port> : Get interrupt enable status on age out\n" },
    { "setRefreshLocked", setPortU32, "port setRefreshLocked <port> <en> : Enable or disable auto refresh known addresses when port is locked\n" },
    { "getRefreshLocked", getPortPU32, "port getRefreshLocked <port> : Get auto refresh mode status when port is locked\n" },
    { "setIgnoreWrongData", setPortU32, "port setIgnoreWrongData <port> <en> : Enable or disable ignore wrong data mode of specified port\n" },
    { "getIgnoreWrongData", getPortPU32, "port getIgnoreWrongData <port> : Get ignore wrong data mode\n" },
    { "setCutThroughEnable", portSetCutThrough, "port setCutThroughEnable <port> [options] : Set port Cut Through configuration\n"},
    { "getCutThroughEnable", portGetCutThrough, "port getCutThroughEnable <port> : Get port Cut Through configuration\n"},

    { "dump", setU32, "port dump <port>: Reads all the registers for specified port and print them out\n"},

    { "help", portCtrlHelp, "port help [subcmd]: Display portCtrl command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT mibCMDList[] =
{
    { "flush", mibFlushAll, "mib flush : Flush All MIBs counters for all ports\n" },
    { "flushport", mibFlushPort, "mib flushport <port> : Flush All MIBs counters for a given port\n" },
    { "getPortCounter", mibGetPortCounter, "mib getPortCounter <port> <counter> : Gets a specific counter of the given port\n" },
    { "getPortAllCounter", mibGetPortAllCounters, "mib getPortAllCounter <port> : Gets all MIBs counters of the given port\n" },
    { "getHistogramMode", getPU32, "mib getHistogramMode : Gets the Histogram Counters Mode\n" },
    { "setHistogramMode", setU32, "mib setHistogramMode <mode>: Sets the Histogram Counters Mode\n" },
    { "dump", setU32, "mib dump <port>: dump all the mibs information for specified port\n"},

    { "help", mibHelp, "mib help [subcmd]: Display MIBs command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT queueCtrlCMDList[] =
{
    
    { "setQc", setPortU32U32,	"qc setQc <port> <point> <data> : Set Queue control data to the Queue Control register\n" },
    { "getQc", getPortU32PU32,	"qc getQc <port> <point> : Get Queue control data to the Queue Control register\n" },
    { "setPortSched", setPortU32,			"qc setPortSched <port> <mode> : Sets Port Scheduling Mode\n" },
    { "getPortSched", getPortPU32,			"qc getPortSched <port> : Gets Port Scheduling Mode\n" },
    { "setQosWeight", qcSetQosWeight, "qc setQosWeight [options] : Programmable Round Robin Weights.\n"},
    { "getQosWeight", qcGetQosWeight, "qc getQosWeight : get the round robin weights\n"},

    { "help", queueCtrlHelp, "qc help [subcmd] : Display qc command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT trunkCMDList[] =
{
    { "setTrunkMask", setPortU32, "trunk setTrunkMask <maskNum> <trunkMask> : Sets Trunk Mask for the given Mask Number.\n" },
    { "getTrunkMask", getPortPU32, "trunk getTrunkMask <maskNum> : Gets Trunk Mask for the given Trunk Number.\n" },
    { "setTrunkHash", setU32, "trunk setTrunkHash <en> : Sets Hash DA & SA for TrunkMask selection enable or disable.\n" },
    { "getTrunkHash", getPU32, "trunk getTrunkHash  : Gets Trunk Mask for the given Trunk Number enable or disable.\n" },
    { "setTrunkRoute", setPortU32, "trunk setTrunkRoute <trunkId> <trunkRoute> : Sets routing information for the given Trunk ID\n" },
    { "getTrunkRoute", getPortPU32, "trunk getTrunkRoute <trunkId> : Gets routing information for the given Trunk ID\n" },
    { "setLAGPort", setPortU32U32, "trunk setLAGPort <port> <en> <LAGId> : Enables/disables and sets the LAG ID.\n" },
    { "getLAGPort", getPortPU32PU32, "trunk getLAGPort <port> : Gets the trunk state of the port. \n" },

    { "help", trunkHelp, "trunk help [subcmd] : Display trunk command options\n" },
    { "", NULL, "" }
};

CLI_COMMAND_STRUCT ledCMDList[] = 
{
    {"forceOn", setPortU32, "led forceOn <port> <ledNum> : Force the LED On\n"},
    {"forceOff", setPortU32, "led forceOff <port> <ledNum> : Force the LED Off\n"},
    {"setLinkAct", setPortU32, "led setLinkAct <port> <ledNum> : Set the Port LED mode to link/act. That is: off-no link, on-link, blink-activity\n"},

    {"help", ledHelp, "led help [subcmd] : Display led command options\n"},
    {"", NULL, ""}
};


EXTERN_CLI_STRUCT externCLICMDList[] =
{
	{ "atu",	atuOperation,	fdbCMDList,		"Show atu commands", "atu help [subcmd]: Display atu command options\n"},
	{ "stu",	stuOperation,	stuCMDList,		"Show stu commands", "stu help [subcmd]: Display stu command options\n"},
	{ "vlan",	vtuOperation,	vtuCMDList,		"Show vlan commands", "vlan help [subcmd]: Display vlan command options\n"},
	{ "imp",	impOperation,	impCMDList,		"Show imp commands", "imp help [subcmd]: Display imp command options\n"},
	{ "phy",	phyOperation,	phyCMDList,		"Show phy commands", "phy help [subcmd]: Display phy command options\n"},
    { "serdes", serdesOperation, serdesCMDList, "Show serdes commands", "serdes help [subcmd]: Display serdes command oprions\n"},
	{ "irl",	irlOperation,	irlCMDList,		"Show irl commands", "irl help [subcmd]: Display irl command options\n"},
	{ "vct",	vctOperation,	vctCMDList,		"Show vct commands", "vct help [subcmd]: Display vct command options\n"},
    { "qav",    qavOperation,   qavCMDList,    	"Show qav commands", "qav help [subcmd]: Display qav command options\n"},
    { "ptp",    ptpOperation,   ptpCMDList,    	"Show ptp commands", "ptp help [subcmd]: Display ptp command options\n"},
    { "qos",    qosOperation,   qosCMDList,    	"Show qos commands", "qos help [subcmd]: Display qos command options\n"},
	{ "tcam",   tcamOperation,	tcamCMDList,	"Show tcam commands", "tcam help [subcmd]: Display tcam command options\n"},
	{ "rmu",	rmuOperation,	rmuCMDList, 	"Show rmu commands", "rmu help [subcmd]: Display rmu command options\n"},

    { "trunk",  trunkOperation,   	trunkCMDList,  		"Show trunk commands", "trunk help [subcmd]: Display trunk command options\n"},
	{ "port",   portCtrlOperation,  portCtrlCMDList, 	"Show port commands", "port help [subcmd]: Display port command options\n"},
	{ "mib",    mibOperation,      	mibCMDList,  		"Show mib commands", "mib help [subcmd]: Display mib command options\n"},
	{ "qc",     queueCtrlOperation, queueCtrlCMDList, 	"Show qc commands", "qc help [subcmd]: Display qc command options\n"},
	{ "sys",    sysCtrlOperation,   sysCtrlCMDList, 	"Show sys commands", "sys help [subcmd]: Display sys command options\n"},
    { "led",    ledOperation,      	ledCMDList,  		"Show led commands", "led help [subcmd]: Display led command options\n"},

	{ "rr",		ReadRegister,		NULL, "rr <devAddr> <regAddr> : Read register", "rr <devAddr> <regAddr> : Read register\n" },
	{ "rw",		WriteRegister,		NULL, "rw <devAddr> <regAddr> <data> : Write register", "rw <devAddr> <regAddr> <data> : Write register\n" },
    { "file",   fileOperation,      NULL, "file <filename> : Load the command from the file", "file <filename> : Load the command from the file\n" },
	{ "getBus",	getBusInterface,	NULL, "getBus : Get the bus interface, SMI, RMU or SMI multichip", "getBus : Get the bus interface, SMI, RMU or SMI multichip\n" },

#ifdef UNITTEST
	{ "unittest", UnitTestCases,   NULL,  "unittest   : Run the Unit test cases", "unittest   : Run the Unit test cases\n" },
	{ "sample", SampleCases,       NULL,  "sample     : Run the sample cases", "sample     : Run the sample cases\n" },
#endif	
	{ "customized", CustomizedCases, NULL,  "customized : Run customized cases", "customized : Run customized cases\n" },

	//{ "exec",   execSystemCmd,		"exec <systemCmd> : exec the system command\n"},
	{ NULL, NULL, NULL, NULL, NULL}
};

static char* reflectMSD_STATUS(MSD_STATUS status)
{
	char * str;
	switch (status)
	{
	case MSD_OK:
		str = "Operation succeeded";
		break;
	case MSD_FAIL:
		str = "Operation failed";
		break;
	case MSD_BAD_PARAM:
		str = "Invalid parameter";
		break;
	case MSD_NO_SUCH:
		str = "Can not find such item";
		break;
	case MSD_NOT_SUPPORTED:
		str = "The request is not support";
		break;
	case MSD_ALREADY_EXIST:
		str = "Tried to create existing item";
		break;
	case MSD_BAD_CPU_PORT:
		str = "Input CPU Port is not valid physical port number";
		break;
	default:
		str = "Unknow status";
		break;
	}
	return str;
}
static int setPortU32U32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U32 para2;

    if (nargs != 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    while (1)
    {
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getPortU32PU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    while (1)
    {
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, para1, &para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", gCurrentCmd->cmd, para2);
    return retVal;
}
static int setPortU32U32U32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U32 para2;
    MSD_U32 para3;

    if (nargs != 6)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    para3 = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);

    while (1)
    {
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2, para3);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getPortU32U32PU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;
    MSD_U32 para3 = 0;

    if (nargs != 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    while (1)
    {
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_U32__U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2, &para3);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", gCurrentCmd->cmd, para3);
    return retVal;
}
static int getPortPU32PU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_PU32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, &para1, &para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value1: 0x%x  value2: 0x%x\n", gCurrentCmd->cmd, para1, para2);
    return retVal;
}
static int setPortU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    while (1)
    {
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getPortPU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para = 0;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, &para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", gCurrentCmd->cmd, para);
    return retVal;
}
static int setU32()
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_U32 para;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    para = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
        char temp[50] = { 0 };
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], "dump") == 0)
        {
            strcat(temp, CMD_ARGS[0]);
            strcat(temp, CMD_ARGS[1]);
        }
        else
        {
            strcat(temp, CMD_ARGS[1]);
        }
        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getPU32()
{
    MSD_STATUS retVal;
    int index = 0;
    MSD_U32 para = 0;

    if (nargs != 2)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }


    while (1)
    {
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}
        if (strIcmp(CMD_ARGS[1], pairCmdList[index].getCmd) == 0)
        {
            API_GET_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, &para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", gCurrentCmd->cmd, para);
    return retVal;
}
static int setDev()
{
    MSD_STATUS retVal;
    int index = 0;

    if (nargs != 2)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    while (1)
    {
        char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
        if (strIcmp(CMD_ARGS[1], "dump") == 0)
        {
            strcat(temp, CMD_ARGS[0]);
            strcat(temp, CMD_ARGS[1]);
        }
        else
        {
            strcat(temp, CMD_ARGS[1]);
        }
        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_DEV tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}

int runCommand(FILE* fp, char* prompt);
int fileOperation()
{
	int retVal = 0;
	char *filename = NULL;
	FILE* fp = NULL;

	if (nargs != 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: file <filename>: load command from file\n");
		return 1;
	}

    if (strIcmp(CMD_ARGS[1], "logOn") == 0)
    {
        logOnOffFlag = 1;
    }
    else if (strIcmp(CMD_ARGS[1], "logOff") == 0)
    {
        logOnOffFlag = 0;
    }
    else
    {
        filename = CMD_ARGS[1];
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            CLI_ERROR("Error: Cannot open the cli command file: %s\n", filename);
            return 2;
        }

        retVal = runCommand(fp, "");

        fclose(fp);
    }
	return retVal;
}

static void printAtuOptions()
{
	CLI_INFO("	[options](default value for option item is 0):\n");
	CLI_INFO("	options for atu entry, format as: -macAddr aabbccddee00 -state 7\n");
	CLI_INFO("	-macAddr    : mac address, format as aabbccddee00\n");
	CLI_INFO("	-lag        : trunk member or LAG\n");
	CLI_INFO("	-portVec    : port vector\n");
	CLI_INFO("	-fid        : ATU MAC Address Database number\n");
	CLI_INFO("	-state      : atu entry state\n");
	CLI_INFO("	-FPri       : MAC frame priority data\n");
	CLI_INFO("	-QPri       : MAC Queue priority data\n");
}
static void atuParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, fdbCMDList));
	if (strIcmp(cmd, "addEntry") == 0)
	{
		printAtuOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        atu addentry -macaddr 112233445566 -state 0xe -portvec 3 -fid 1\n");
	}
	else if (strIcmp(cmd, "getNext") == 0)
	{
        CLI_INFO("	macAddr: mac address strart to search, format as aabbccddee00\n");
        CLI_INFO("	fid : ATU MAC Address Database Number\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        atu getNext aabbccddee00 0\n");
	}
	else if (strIcmp(cmd, "delAll") == 0)
	{
		CLI_INFO("	deleteCMD:\n");
		CLI_INFO("		1: delete All\n");
        CLI_INFO("		2: delete All non static\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu delAll 1\n");
	}
	else if (strIcmp(cmd, "delAllInDB") == 0)
	{
		CLI_INFO("	deleteCMD:\n");
		CLI_INFO("		1: delete All\n");
		CLI_INFO("		2: delete All non static\n");
		CLI_INFO("	fid : ATU MAC Address Database Number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu delAllInDB 1 1\n");
	}
	else if (strIcmp(cmd, "move") == 0)
	{
		CLI_INFO("	moveCMD:\n");
		CLI_INFO("		1: Move All\n");
		CLI_INFO("		2: Move All non static\n");
		CLI_INFO("	moveFrom : port where moving from\n");
		CLI_INFO("	moveTo : port where moving to\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu move 1 2 3\n");
	}
	else if (strIcmp(cmd, "moveInDB") == 0)
	{
		CLI_INFO("	moveCMD:\n");
		CLI_INFO("		1: Move All\n");
		CLI_INFO("		2: Move All non static\n");
		CLI_INFO("	fid : ATU MAC Address Database Number\n");
		CLI_INFO("	moveFrom: port where moving from\n");
		CLI_INFO("	moveTo  : port where moving to\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu moveInDB 1 1 2 3\n");
	}
	else if (strIcmp(cmd, "delEntry") == 0)
	{
        CLI_INFO("	macAddr : mac address, format as aabbccddee00\n");
        CLI_INFO("	fid : ATU MAC Address Database number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu delEntry 112233445566 1\n");
	}
	else if (strIcmp(cmd, "getVio") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getVio\n");
	}
	else if (strIcmp(cmd, "findEntry") == 0)
	{
        CLI_INFO("	macAddr : mac address, format as aabbccddee00\n");
        CLI_INFO("	fid : ATU MAC Address Database number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu findEntry 112233445566 1\n");
	}
	else if (strIcmp(cmd, "setAging") == 0)
	{
		CLI_INFO("	timeout : aging time in millisecond\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu setAging 120000\n");
	}
    else if (strIcmp(cmd, "getAging") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getAging\n");
    }
    else if (strIcmp(cmd, "portRemove") == 0)
    {
        CLI_INFO("	RemoveCMD:\n");
        CLI_INFO("		1: Remove All\n");
        CLI_INFO("		2: Remove All non static\n");
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu portRemove 1 0\n");
    }
    else if (strIcmp(cmd, "portRemoveInDB") == 0)
    {
        CLI_INFO("	RemoveCMD:\n");
        CLI_INFO("		1: Remove All\n");
        CLI_INFO("		2: Remove All non static\n");
        CLI_INFO("	fid  : ATU MAC Address Database Number\n");
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu portRemoveInDB 1 1 0\n");
    }
    else if (strIcmp(cmd, "getLearn2All") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getLearn2All\n");
    }
    else if (strIcmp(cmd, "setLearn2All") == 0)
    {
        CLI_INFO("  en : 1 if Learn2All is enabled, 0 otherwise\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu setLearn2All 1 \n");
    }
    else if (strIcmp(cmd, "setPortLearnLimit") == 0)
    {
        CLI_INFO("  port : logical port number. \n");
        CLI_INFO("  limit : auto learning limit. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu setPortLearnLimit 1 1 \n");
    }
	else if (strIcmp(cmd, "getPortLearnLimit") == 0)
	{
		CLI_INFO("  port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getPortLearnLimit 1 \n");
	}
    else if (strIcmp(cmd, "getLearnCount") == 0)
    {
        CLI_INFO("  port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getLearnCount 0 \n");
    }
    else if (strIcmp(cmd, "getCount") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getCount\n");
    }
    else if (strIcmp(cmd, "getCountPerFid") == 0)
    {
        CLI_INFO("  fid : ATU MAC Address Database Number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getCountPerFid 1\n");
    }
    else if (strIcmp(cmd, "getNonStaticCount") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getNonStaticCount\n");
    }
    else if (strIcmp(cmd, "getNonStaticCountPerFid") == 0)
    {
        CLI_INFO("  fid : ATU MAC Address Database Number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu getNonStaticCountPerFid 1\n");
    }
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("	fid : ATU MAC Address Database Number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        atu dump 1\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
static void macStr2macArr(char* inp, MSD_U8* out)
{
	int i, val;
	val = 0;
	for (i = 0; i<12; i++)
	{
		if ((inp[i] >= 48) && (inp[i] <= 57))
			val = (val << 4) + inp[i] - 48;
		if ((inp[i] >= 65) && (inp[i] <= 70))
			val = (val << 4) + (inp[i] - 65 + 10);
		if ((inp[i] >= 97) && (inp[i] <= 102))
			val = (val << 4) + (inp[i] - 97 + 10);
		if (i & 0x1)
		{
			out[i / 2] = val;
			val = 0;
		}
	}
}
static int updateFdbEntry(MSD_ATU_ENTRY * entry, int startIndex)
{
	int index;
	char* cmd, *value;
    for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;		

		if (*cmd != '-')
		{
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
		}
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

		if (strIcmp(cmd, "-macAddr") == 0)
		{
			macStr2macArr(value, entry->macAddr.arEther);
		}
		else if (strIcmp(cmd, "-lag") == 0)
		{
			entry->trunkMemberOrLAG = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-portVec") == 0)
		{
			entry->portVec = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-fid") == 0)
		{
            entry->fid = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-state") == 0)
		{
			entry->entryState = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-FPri") == 0)
		{
			entry->exPrio.macFPri = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-QPri") == 0)
		{
			entry->exPrio.macQPri = (MSD_U8)strtoul(value, NULL, 0);
		}
		else
		{
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
		}
	}
	
    return 0;
}
static void printOutFdbEntry(MSD_ATU_ENTRY * entry)
{
	int i;
    char tempStr[200] = { 0 };
	CLI_INFO("ATU entry info:\n");
    sprintf(tempStr, "	macAddr:	");
	//CLI_INFO("	macAddr:	");
	for (i = 0; i < 6; i++)
	{
		//CLI_INFO("%02X", entry->macAddr.arEther[i]);
        sprintf(tempStr + strlen(tempStr), "%02X", entry->macAddr.arEther[i]);
	}
	//CLI_INFO("\n");
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
	CLI_INFO("	LAG:		0x%X\n", entry->trunkMemberOrLAG);
	CLI_INFO("	portVec:	0x%lX\n", entry->portVec);
    CLI_INFO("	fid:		0x%X\n", entry->fid);
	CLI_INFO("	State:		0x%X\n", entry->entryState);
	CLI_INFO("	FPri:		0x%X\n", entry->exPrio.macFPri);
	CLI_INFO("	QPri:		0x%X\n", entry->exPrio.macQPri);
}
static void printOutFdbViolation(MSD_ATU_INT_STATUS *status)
{
	int i;
    char tempStr[200] = { 0 };

	CLI_INFO("ATU violation info:\n");
	CLI_INFO("	atuIntCause:	\n", status->atuIntCause);
	CLI_INFO("		Age out Vio status:	%d\n", status->atuIntCause.ageOutVio);
	CLI_INFO("		Full Vio status:	%d\n", status->atuIntCause.fullVio);
	CLI_INFO("		Member Vio status:	%d\n", status->atuIntCause.memberVio);
	CLI_INFO("		Miss Vio status:	%d\n", status->atuIntCause.missVio);
	CLI_INFO("	spid:			%X\n", status->spid);
	CLI_INFO("	fid:			%X\n", status->fid);
    sprintf(tempStr, "	macAddr:	");
    //CLI_INFO("	macAddr:	");
    for (i = 0; i < 6; i++)
    {
        //CLI_INFO("%02X", status->macAddr.arEther[i]);
        sprintf(tempStr + strlen(tempStr), "%02X", status->macAddr.arEther[i]);
    }
    //CLI_INFO("\n");
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
}
int atuGetNextEntry()
{
	MSD_STATUS retVal;
    MSD_ETHERADDR macAddr;
    MSD_U32  fid;
    MSD_ATU_ENTRY entry;

    memset(&macAddr, 0, sizeof(MSD_ETHERADDR));
    memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbEntryNextGet(sohoDevNum, &macAddr, fid, &entry);

	if (retVal == MSD_NO_SUCH)
	{
        CLI_INFO("Cannot find any valid atu entry start from macAddr: %s for fid %d:\n", CMD_ARGS[2], fid);
		return retVal;
	}

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get next mac entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	printOutFdbEntry(&entry);

	return 0;
}
int atuAddEntry()
{
	MSD_STATUS retVal;
	MSD_ATU_ENTRY entry;

	memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

    if (updateFdbEntry(&entry, 2) != 0)
        return MSD_FAIL;
	
	retVal = msdFdbMacEntryAdd(sohoDevNum, &entry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
            if (gCurrentCmd != NULL)
                CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error add mac entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int atuFlush()
{
	MSD_STATUS retVal;
	MSD_FLUSH_CMD flushCMD;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
		return 1;
	}

	flushCMD = (MSD_FLUSH_CMD)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdFdbAllDelete(sohoDevNum, flushCMD);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush ATU  flushCMD=%X ret[%d: %s]"
			"\n", flushCMD, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int atuFlushInDB()
{
	MSD_STATUS retVal;
	MSD_FLUSH_CMD flushCMD;
	MSD_U32 fid;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
		return 1;
	}

	flushCMD = (MSD_FLUSH_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbAllDeleteInDB(sohoDevNum, flushCMD, fid);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush ATU based on fid flushCMD=%X fid=%lX ret[%d: %s]"
            "\n", flushCMD, fid, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int atuMove()
{
	MSD_STATUS retVal;
	MSD_MOVE_CMD moveCMD;
	MSD_LPORT moveFrom;
	MSD_LPORT moveTo;

	if (nargs != 5)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
		return 1;
	}
	moveCMD = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
	moveFrom = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
	moveTo = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);

	retVal = msdFdbMove(sohoDevNum, moveCMD, moveFrom, moveTo);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error move ATU moveCMD=%X moveFrom=%d moveTo=%d ret[%d: %s]"
			"\n", moveCMD, (int)moveFrom, (int)moveTo, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int atuMoveInDB()
{
	MSD_STATUS retVal;
	MSD_MOVE_CMD moveCMD;
    MSD_U32 fid;
	MSD_LPORT moveFrom;
	MSD_LPORT moveTo;

	if (nargs != 6)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
		return 1;
	}
	moveCMD = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	moveFrom = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);
	moveTo = (MSD_LPORT)strtoul(CMD_ARGS[5], NULL, 0);

    retVal = msdFdbMoveInDB(sohoDevNum, moveCMD, fid, moveFrom, moveTo);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error move ATU moveCMD=%X moveFrom=%d moveTo=%d ret[%d: %s]"
			"\n", moveCMD, (int)moveFrom, (int)moveTo, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int atuDelEntry()
{
	MSD_STATUS retVal;
    MSD_ETHERADDR macAddr;
    MSD_U32 fid;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }
    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

	retVal = msdFdbMacEntryDelete(sohoDevNum, &macAddr, fid);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error delete mac entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int atuGetVio()
{
	MSD_STATUS retVal;
	MSD_ATU_INT_STATUS status;

	memset(&status, 0, sizeof(MSD_ATU_INT_STATUS));

	retVal = msdFdbViolationGet(sohoDevNum, &status);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get atu violation ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	printOutFdbViolation(&status);
	return 0;
}
int atuFindEntry()
{
	MSD_STATUS retVal;
	MSD_ATU_ENTRY entry;
    MSD_ETHERADDR macAddr;
    MSD_U32 fid;
	MSD_BOOL found;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }
    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

	memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

	retVal = msdFdbMacEntryFind(sohoDevNum, &macAddr, fid, &entry, &found);

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error find mac entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

    if (found == MSD_TRUE)
    {
        CLI_INFO("Find the atu entry with macAddr %s in fid %d\n", CMD_ARGS[2], (int)fid);
        printOutFdbEntry(&entry);
    }
    else
    {
        CLI_INFO("Can not find the atu entry with macAddr %s in fid %d\n", CMD_ARGS[2], (int)fid);
    }

	return 0;
}
int atuPortRemove()
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD  moveCmd;
    MSD_LPORT     portNum;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    moveCmd = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbPortRemove(sohoDevNum, moveCmd,portNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int portRemoveINDB()
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD  moveCmd;
    MSD_U32        fid;
    MSD_LPORT     portNum;

    if (nargs != 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    moveCmd = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdFdbPortRemoveInDB(sohoDevNum, moveCmd, fid, portNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int atuHelp()
{
	int index = 0;

	if (nargs >= 3)
	{
		atuParaHelp(CMD_ARGS[2]);
		return 0;
	}

	while (1)
	{
		if (fdbCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", fdbCMDList[index].help);
		index++;
	}

	return 0;
}
int atuOperation()
{
	int index = 0;
	while (1)
	{
		if (fdbCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], fdbCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(fdbCMDList[index]);
			fdbCMDList[index].callback();
			break;
		}
		index++;
	}
	if (fdbCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"atu help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static int updateArray(int* index, char* value, int maxLen, unsigned int list[])
{
    int stateIndex;
    char* temp;

    if (*value != '{')
    {
        CLI_ERROR("Invalid option input: %s\n", value);
        return -1;
    }

    temp = strchr(value, '}');
    if ((temp != NULL) && *(temp + 1) != '\0')
    {
        CLI_ERROR("Invalid option input: %s\n", value);
        return -1;
    }

    /*{} means all the members set to 0*/
    if (strIcmp(value, "{}") == 0)
        return 0;

    if (strlen(value) == 1)
    {
        value = CMD_ARGS[(*index)];
        *index = *index + 1;
    }
    else
        value = value + 1;
    list[0] = strtoul(value, NULL, 0);

    for (stateIndex = 1; (*index < nargs) && (stateIndex < maxLen); stateIndex++)
    {
        if (strchr(value, '}') != NULL)
            break;

        value = CMD_ARGS[(*index)];
        *index = *index + 1;
        if (*value == '}')
        {
            if (strlen(value) != 1)
            {
                CLI_ERROR("Invalid option input: %s\n", value);
                return -1;
            }

            break;
        }

        list[stateIndex] = strtoul(value, NULL, 0);
        //if (strchr(value, '}') != NULL)
        //    break;
    }

    return 0;
}
static int updateStuEntry(MSD_STU_ENTRY *entry, int startIndex)
{
	int index;
	char* cmd, *value;
    for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }        
        
        if (strIcmp(cmd, "-sid") == 0)
		{
			entry->sid = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-portState") == 0)
		{
            unsigned int tempList[MSD_MAX_SWITCH_PORTS] = { 0 };
            int i;
			if (updateArray(&index, value, MSD_MAX_SWITCH_PORTS, tempList) != 0)
            {
                return -1;
            }
			for (i = 0; i < MSD_MAX_SWITCH_PORTS; i++)
            {
                entry->portState[i] = (MSD_PORT_STP_STATE)tempList[i];
            }
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static printOutStuEntry(MSD_STU_ENTRY *entry)
{
	int i;
    char tempStr[200] = { 0 };

	CLI_INFO("Stu Entry Info:\n");
	CLI_INFO("	sid:		%X\n", entry->sid);
	//CLI_INFO("	portState:	");
    sprintf(tempStr, "	portState:	");
	for (i = 0; i < qddev->numOfPorts; i++)
	{
		//CLI_INFO("%X ", entry->portState[i]);
        sprintf(tempStr + strlen(tempStr), "%X ", entry->portState[i]);
	}
	//CLI_INFO("\n");
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
}
int stuAddEntry()
{
	MSD_STATUS retVal;
	MSD_STU_ENTRY entry;

	memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    if (updateStuEntry(&entry, 2) != 0)
        return -1;

    retVal = msdStuEntryAdd(sohoDevNum, &entry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error add stu entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int stuGetNext()
{
    MSD_STATUS retVal;
    MSD_U16 sid;
    MSD_STU_ENTRY entry;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    retVal = msdStuEntryNextGet(sohoDevNum, sid, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid stu entry start from sid %d:\n", sid);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find stu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    printOutStuEntry(&entry);
    return retVal;
}
int stuFindEntry()
{
	MSD_STATUS retVal;
	MSD_STU_ENTRY entry;
    MSD_U16 sid;
    MSD_BOOL found;
	
    if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    retVal = msdStuEntryFind(sohoDevNum, sid, &entry, &found);

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error find stu entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

    if (found == MSD_FALSE)
    {
        CLI_INFO("Cannot find the stu entry with the stu Id: %d\n", sid);
    }
    else
    {
        CLI_INFO("Find the stu entry with the stu Id: %d\n", sid);
	    printOutStuEntry(&entry);
    }

	return 0;
}
int stuDelEntry()
{
	MSD_STATUS retVal;
    MSD_U16 sid;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdStuEntryDelete(sohoDevNum, sid);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error delete stu entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int stuGetEntryCount()
{
    MSD_STATUS retVal;
    MSD_U16 count;

    if (nargs != 2)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    retVal = msdStuEntryCountGet(sohoDevNum, &count);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get stu entry count ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("The valied stu entry count is:%d\n", count);

    return 0;
}
void printStuOptions()
{
	CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for stu entry, format as: -sid 1 -portState {1 2 3}\n");
	CLI_INFO("	-sid		: STU ID\n");
	CLI_INFO("	-portState	: array of Per VLAN Port States for each port\n");
	CLI_INFO("		0	: port disable\n");
	CLI_INFO("		1	: port blocking\n");
	CLI_INFO("		2	: port learning\n");
	CLI_INFO("		3	: port forwarding\n");
}
void stuParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, stuCMDList));
	if (strIcmp(cmd, "addEntry") == 0)
	{
		printStuOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        stu addentry -sid 1 -portstate {0 1}\n");
	}
	else if (strIcmp(cmd, "findEntry") == 0)
	{
        CLI_INFO("  sid : SID ID, valid value is 0 ~ 63\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        stu findEntry 1 \n");
	}
    else if (strIcmp(cmd, "getNext") == 0)
    {
        CLI_INFO("  sid : SID ID, valid value is 0 ~ 63\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        stu getNext 1 \n");
    }
	else if (strIcmp(cmd, "delEntry") == 0)
	{
        CLI_INFO("  sid : SID ID, valid value is 0 ~ 63\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        stu delentry 1 \n");
	}
    else if (strIcmp(cmd, "getCount") == 0)
    {
        CLI_INFO("Example:\n");
	    CLI_INFO("        stu getCount\n");
    }
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        stu dump\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int stuHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		stuParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (stuCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", stuCMDList[index].help);
		index++;
	}

	return 0;
}
int stuOperation()
{
	int index = 0;
	while (1)
	{
		if (stuCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], stuCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(stuCMDList[index]);
			stuCMDList[index].callback();
			break;
		}
		index++;
	}
	if (stuCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"stu help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static int updateVtuEntry(MSD_VTU_ENTRY *entry, int startIndex)
{
	int index;
	char* cmd, *value;
    for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

		if (strIcmp(cmd, "-fid") == 0)
		{
			entry->fid = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vid") == 0)
		{
			entry->vid = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-sid") == 0)
		{
			entry->sid = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-TagP") == 0)
		{
            int i;
            unsigned int tempList[MSD_MAX_SWITCH_PORTS] = { 0 };
            if (updateArray(&index, value, MSD_MAX_SWITCH_PORTS, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < MSD_MAX_SWITCH_PORTS; i++)
            {
                entry->memberTagP[i] = (MSD_PORT_MEMBER_TAG)tempList[i];
            }
		}
		else if (strIcmp(cmd, "-vidPolicy") == 0)
		{
			entry->vidPolicy = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-useVIDFPri") == 0)
		{
			entry->vidExInfo.useVIDFPri = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vidFPri") == 0)
		{
			entry->vidExInfo.vidFPri = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-useVIDQPri") == 0)
		{
			entry->vidExInfo.useVIDQPri = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vidQPri") == 0)
		{
			entry->vidExInfo.vidQPri = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-dontLearn") == 0)
		{
			entry->vidExInfo.dontLearn = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-filterUC") == 0)
		{
			entry->vidExInfo.filterUC = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-filterBC") == 0)
		{
			entry->vidExInfo.filterBC = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-filterMC") == 0)
		{
			entry->vidExInfo.filterMC = (MSD_BOOL)strtoul(value, NULL, 0);
		}
        else 
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static void printOutVtuEntry(MSD_VTU_ENTRY *entry)
{
	int i;
    char tempStr[200] = { 0 };

	CLI_INFO("Vtu Entry Info:\n");
	CLI_INFO("	vid:			%X\n", entry->vid);
    CLI_INFO("	fid:			%X\n", entry->fid);
	CLI_INFO("	sid:			%X\n", entry->sid);
	//CLI_INFO("	TagP:			");
    sprintf(tempStr, "	TagP:			");
	for (i = 0; i < qddev->numOfPorts; i++)
	{
		//CLI_INFO("%X ", entry->memberTagP[i]);
        sprintf(tempStr + strlen(tempStr), "%X ", entry->memberTagP[i]);
	}
	//CLI_INFO("\n");
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
	CLI_INFO("	vidPolicy:		%X\n", entry->vidPolicy);
	CLI_INFO("	vidExInfo:		\n");
	CLI_INFO("		useVIDFPri:		%X\n", entry->vidExInfo.useVIDFPri);
	CLI_INFO("		vidFPri:		%X\n", entry->vidExInfo.vidFPri);
	CLI_INFO("		useVIDQPri:		%X\n", entry->vidExInfo.useVIDQPri);
	CLI_INFO("		vidQPri:		%X\n", entry->vidExInfo.vidQPri);
	CLI_INFO("		dontLearn:		%X\n", entry->vidExInfo.dontLearn);
	CLI_INFO("		filterUC:		%X\n", entry->vidExInfo.filterUC);
	CLI_INFO("		filterBC:		%X\n", entry->vidExInfo.filterBC);
	CLI_INFO("		filterMC:		%X\n", entry->vidExInfo.filterMC);
}
static void printOutVtuViolation(MSD_VTU_INT_STATUS *status)
{
	CLI_INFO("VTU violation info:\n");

	CLI_INFO("	spid:			%X\n", status->spid);
	CLI_INFO("	vid:			%X\n", status->vid);
    CLI_INFO("	vtuIntCause:	\n");
    CLI_INFO("		fullVio:        %X\n", status->vtuIntCause.fullVio);
    CLI_INFO("		memberVio:      %X\n", status->vtuIntCause.memberVio);
    CLI_INFO("		missVio:        %X\n", status->vtuIntCause.missVio);
}
int vtuAddEntry()
{
	MSD_STATUS retVal;
	MSD_VTU_ENTRY entry;

	memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    if (updateVtuEntry(&entry, 2) != 0)
        return -1;

    retVal = msdVlanEntryAdd(sohoDevNum, &entry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error add vlan entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int vtuFlush()
{
	MSD_STATUS retVal;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    retVal = msdVlanAllDelete(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush vtu entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int vtuFindEntry()
{
	MSD_STATUS retVal;
	MSD_VTU_ENTRY entry;
	MSD_BOOL found;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    retVal = msdVlanEntryFind(sohoDevNum, vlanId, &entry, &found);

    if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error find vlan entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

    if (found == MSD_FALSE)
	{
        CLI_INFO("Cannot find the vlan entry with the vlan Id: %d\n", vlanId);
    }
    else
    {
        CLI_INFO("Find the vlan entry with the vlan Id: %d\n", vlanId);
		printOutVtuEntry(&entry);
	}

	return 0;
}
int vtuDelEntry()
{
	MSD_STATUS retVal;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanEntryDelete(sohoDevNum, vlanId);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error delete vlan entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int vtuSetMode()
{
	MSD_STATUS retVal;
	MSD_VTU_MODE    mode;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	mode = (MSD_VTU_MODE)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanModeSet(sohoDevNum, mode);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error set vtu mode ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int vtuGetMode()
	{
	MSD_STATUS retVal;
	MSD_VTU_MODE    mode;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    retVal = msdVlanModeGet(sohoDevNum, &mode);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error set vlan mode ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("Vtu get back mode: %x\n", mode);
	return 0;
}
int vtuGetVio()
{
	MSD_STATUS retVal;
	MSD_VTU_INT_STATUS status;

	memset(&status, 0, sizeof(MSD_VTU_INT_STATUS));

    retVal = msdVlanViolationGet(sohoDevNum, &status);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get atu violation ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	printOutVtuViolation(&status);
	return 0;
}
int vutIsExist()
{
	MSD_STATUS retVal;
    MSD_BOOL exist;
    MSD_U16 vlanId;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanExists(sohoDevNum, vlanId, &exist);

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error check vlan entry exist ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

    if (exist == MSD_FALSE)
    {
        CLI_INFO("the vlan entry with vlanId %d is not exist\n", vlanId);
    }
    else
    {
        CLI_INFO("the vlan entry with vlanId %d is exist\n", vlanId);
    }

	return 0;
}
int vtuGetNext()
{
    MSD_STATUS retVal;
    MSD_VTU_ENTRY entry;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    retVal = msdVlanEntryNextGet(sohoDevNum, vlanId, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid vlan entry start from vlan Id %d:\n", vlanId);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find vlan entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutVtuEntry(&entry);

    return 0;
}
int vtuGetCount()
{
	MSD_STATUS retVal;
    MSD_U16 count;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    retVal = msdVlanEntryCountGet(sohoDevNum, &count);

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error get vtu entry count ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

    CLI_INFO("The valid vlan entry count is:%d\n", count);

	return 0;
}
int vtuSetMemberTag()
{
	MSD_STATUS retVal;
    MSD_U16 vlanId;
    MSD_LPORT  portNum;
    MSD_PORT_MEMBER_TAG memberTag;

    if (nargs != 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    memberTag = (MSD_PORT_MEMBER_TAG)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdVlanMemberTagSet(sohoDevNum, vlanId, portNum, memberTag);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error set the vlan memberTag ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int vtuGetMemberTag()
{
    MSD_STATUS retVal;
    MSD_U16 vlanId;
    MSD_LPORT  portNum;
    MSD_PORT_MEMBER_TAG memberTag;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    
    retVal = msdVlanMemberTagGet(sohoDevNum, vlanId, portNum, &memberTag);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get the vlan memberTag ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Read back the memberTag for port %d int vlan %d is %d\n", portNum, vlanId, memberTag);
    return 0;
}
void printVtuOptions()
{
	CLI_INFO("	[options](default value for option item is 0):\n");
	CLI_INFO("	options for vtu entry, format as: -vid 1 -sid 1\n");
	CLI_INFO("	-fid		: database number or FID (forwarding information database)\n");
	CLI_INFO("	-vid		: VLAN ID\n");
	CLI_INFO("	-sid		: 802.1s Port State Database ID\n");
	CLI_INFO("	-TagP		: Membership and Egress Tagging array, format as: -TagP {1 2 3}\n");
    CLI_INFO("	    		  -TagP {1 2 3} means port0 is MEMBER_EGRESS_UNTAGGED, port1 is MEMBER_EGRESS_TAGGED\n");
    CLI_INFO("	    		  port2 is NOT_A_MEMBER, and all other ports are MEMBER_EGRESS_UNMODIFIED\n");
	CLI_INFO("		0	: MEMBER_EGRESS_UNMODIFIED\n");
	CLI_INFO("		1	: MEMBER_EGRESS_UNTAGGED\n");
	CLI_INFO("		2	: MEMBER_EGRESS_TAGGED\n");
	CLI_INFO("		3	: NOT_A_MEMBER\n");
	CLI_INFO("	-vidPolicy	: indicate that the frame with this VID uses VID Policy.\n");
	CLI_INFO("	-useVIDFPri	: Use VID queue priority override.  \n");
	CLI_INFO("	-vidFPri	: VID frame priority data (0 ~ 7). \n");
	CLI_INFO("	-useVIDQPri	: Use VID frame priority override. \n");
	CLI_INFO("	-vidQPri	: VID queue priority data. \n");
	CLI_INFO("	-dontLearn	: Don't learn. \n");
	CLI_INFO("	-filterUC	: Filter Unicast frames. \n");
	CLI_INFO("	-filterBC	: Filter Broadcast frames. \n");
	CLI_INFO("	-filterMC	: Filter Multicast frames. \n");
}
void printVtumode()
{
	CLI_INFO("<modeValue>:");
	CLI_INFO("vtu mode\n");
	CLI_INFO("	0 : 4K vtu, page should be 0 unless overridden by TCAM \n");
	CLI_INFO("	1 : 8K vtu with vtu  page being  0 if a frame is a  unicast frame \n");
	CLI_INFO("	2 : use 8K vtu with vtu page being = to the source \n");
	CLI_INFO("	    port VTU page bit(port offset 0x05)\n");
	CLI_INFO("	3 : use 8K vtu with vtu page being a one if frame is not properly \n");
	CLI_INFO("	    Provider tagged entering a Provider port \n");
}
void vtuParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, vtuCMDList));
	if (strIcmp(cmd, "addEntry") == 0)
	{
		printVtuOptions();
		CLI_INFO("Example:\n");
        CLI_INFO("        vlan addentry -sid 1 -vid 1 -fid 1 -TagP {1 2 3} -vidPolicy 1 \n");
	}
	else if (strIcmp(cmd, "delAll") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan delAll\n");
	}
	else if (strIcmp(cmd, "delEntry") == 0)
	{
        CLI_INFO("  vlanId : vlan entry id\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan delEntry 1\n");
	}
	else if (strIcmp(cmd, "getVio") == 0)
	{
        CLI_INFO("Example:\n");
		CLI_INFO("        vlan getVio\n");
	}
	else if (strIcmp(cmd, "findEntry") == 0)
	{
        CLI_INFO("  vlanId : vlan entry id\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan findEntry 2\n");
	}
    else if (strIcmp(cmd, "getNext") == 0)
    {
        CLI_INFO("  vlanId : vlan entry id\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        vlan getNext 1\n");
    }
	else if (strIcmp(cmd, "setMode") == 0)
	{
		printVtumode();
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan setMode 0\n");
	}
	else if (strIcmp(cmd, "getMode") == 0)
	{
		
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan getMode\n");
	}
    else if (strIcmp(cmd, "isExist") == 0)
    {
        CLI_INFO("  vlanId    : vlan entry Id\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan isExist 1\n");
    }
    else if (strIcmp(cmd, "getCount") == 0)
    {
       CLI_INFO("Example:\n");
		CLI_INFO("        vlan getCount\n");
    }
    else if (strIcmp(cmd, "setMemberTag") == 0)
    {
        CLI_INFO("  vlanId    : vlan entry Id\n");
        CLI_INFO("  portNum   : port number\n");
        CLI_INFO("  memberTag : enum element of MSD_VLAN_MEMBER_TAG\n");
        CLI_INFO("      0 : MEMBER_EGRESS_UNMODIFIED\n");
        CLI_INFO("      1 : MEMBER_EGRESS_UNTAGGED\n");
        CLI_INFO("      2 : MEMBER_EGRESS_TAGGED\n");
        CLI_INFO("      3 : NOT_A_MEMBER\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan setMemberTag 2 0 1\n");
    }
    else if (strIcmp(cmd, "getMemberTag") == 0)
    {
        CLI_INFO("  vlanId    : vlan entry Id\n");
        CLI_INFO("  portNum   : port number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vlan getMemberTag 2 0\n");
    }
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        vlan dump\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int vtuHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		vtuParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (vtuCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", vtuCMDList[index].help);
		index++;
	}

	return 0;
}
int vtuOperation()
{
	int index = 0;

	while (1)
	{
		if (vtuCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], vtuCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(vtuCMDList[index]);
			vtuCMDList[index].callback();
			break;
		}
		index++;
	}
	if (vtuCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"vlan help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

typedef enum {
    S200k = 0,
    S400k = 1,
    S500k = 2,
    S600k = 3,
    S800k = 4,
    S1000k = 5,
    S1200k = 6,
    S1500k = 7
} I2C_SPEED;
static MSD_U16 ReadInt(MSD_U8 *data, MSD_U8 cnt)
{
    MSD_U8 Idx = 0;
    MSD_U16 val = 0;
    cnt = cnt * 2;
    while (cnt > 0)
    {
        if (data[Idx] >= '0' && data[Idx] <= '9')
        {
            val = (val << 4) | (data[Idx] - '0');
            cnt--;
            Idx++;
        }
        else if (data[Idx] >= 'a' && data[Idx] <= 'f')
        {
            val = (val << 4) | (data[Idx] - 'a' + 10);
            cnt--;
            Idx++;
        }
        else if (data[Idx] >= 'A' && data[Idx] <= 'F' + 10)
        {
            val = (val << 4) | (data[Idx] - 'A' + 10);
            cnt--;
            Idx++;
        }
        else
        {
            val = (val << 4) | 0xF;
            cnt--;
            Idx++;
        }

    }
    return val;
}
typedef struct NODE
{
    MSD_U16 addr;
    MSD_U16 len;
    MSD_U8* data;
    struct NODE* next;
}Node;
static void freeNode(struct NODE *head)
{
    while (head != NULL)
    {
        Node* tmp = head->next;
        free(head->data);
        free(head);
        head = tmp;
    }
}
static MSD_U8* Ihx_Parse_EEPROM(MSD_U16 *data_len, MSD_U8* filename, I2C_SPEED speed, MSD_U16 eeprom_addr_src, MSD_U16 ram_dest_addr, MSD_U16 ram_exec_addr)
{
    MSD_U8* ihx_data;
    MSD_U16 ihx_data_len;
    MSD_U16 Idx;
    MSD_U8 Dnum;
    MSD_U16 type;
    MSD_U8 chk;
    MSD_U16 code_start = 0xffff;
    MSD_U16 code_end = 0;
    MSD_U16 addr;
    MSD_U16 code_size;
    MSD_U8 data;
    MSD_U16 i;
    MSD_U8* eeprom_data;

    FILE *pfile;
    if ((pfile = fopen(filename, "rb")) == NULL)
    {
        CLI_ERROR("Can not open the file: %s\n", filename);
        return NULL;
    }

    fseek(pfile, 0, SEEK_END);
    ihx_data_len = (MSD_U16)ftell(pfile);

    ihx_data = (MSD_U8*)malloc(ihx_data_len * sizeof(MSD_U8) + 1);
    rewind(pfile);
    fread(ihx_data, sizeof(MSD_U8), ihx_data_len, pfile);

    /* get code size */
    for (Idx = 0; Idx < ihx_data_len;)
    {
        if (ihx_data[Idx] == ':')
        {
            Idx++;
            Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
            Idx += 2;
            addr = ReadInt(ihx_data + Idx, 2);
            Idx += 4;

            type = ReadInt(ihx_data + Idx, 1);
            if (type == 0)
            {
                if (code_start > addr)
                    code_start = addr;
                if (code_end < (addr + Dnum))
                    code_end = addr + Dnum;
            }
            else if (type == 1)
                break;
        }
        else
        {
            Idx += 1;
        }
    }

    code_size = code_end - code_start;

    eeprom_data = (unsigned char*)malloc((code_size + eeprom_addr_src)*sizeof(char) + 1);

    for (Idx = 0; Idx < code_size + eeprom_addr_src; Idx++)
    {
        eeprom_data[Idx] = 0xff;
    }

    /* fast boot function */
    eeprom_data[0] = 0xed;

    /* eeprom speed configuration */
    data = (MSD_U8)speed;
    eeprom_data[1] = data;

    /* code size */
    eeprom_data[2] = (MSD_U8)(code_size & 0xff);
    eeprom_data[3] = (MSD_U8)((code_size >> 8) & 0xff);

    /* ram address */
    eeprom_data[4] = (MSD_U8)(ram_dest_addr & 0xff);
    eeprom_data[5] = (MSD_U8)((ram_dest_addr >> 8) & 0xff);

    /* eeprom src address */
    eeprom_data[6] = (MSD_U8)(eeprom_addr_src & 0xff);
    eeprom_data[7] = (MSD_U8)((eeprom_addr_src >> 8) & 0xff);

    eeprom_data[8] = 0xc3;

    /* ram executing address */
    eeprom_data[9] = (MSD_U8)(ram_exec_addr & 0xff);
    eeprom_data[0xa] = (MSD_U8)((ram_exec_addr >> 8) & 0xff);

    type = 0;
    Idx = 0;
    while (type != 1)
    {
        while ((ihx_data[Idx] != ':') && ihx_data[Idx] > 0)
            Idx++;
        if (ihx_data[Idx] != ':')
        {
            /* ihx file format error */
            free(eeprom_data);
            eeprom_data = NULL;
            return eeprom_data;
        }
        Idx++;

        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        chk = Dnum;
        Idx += 2;

        addr = ReadInt(ihx_data + Idx, 2);
        chk += (addr & 0xff);
        chk += ((addr >> 8) & 0xff);
        Idx += 4;

        type = ReadInt(ihx_data + Idx, 1);
        chk += type;
        Idx += 2;

        for (i = 0; i < Dnum; i++)
        {
            chk += ReadInt(ihx_data + Idx, 1);
            if (type == 0)
                eeprom_data[addr + eeprom_addr_src + i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);

            Idx += 2;
        }

        if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
        {
            free(eeprom_data);
            eeprom_data = NULL;
            return eeprom_data;
        }
        Idx += 2;

    }
    free(ihx_data);

    *data_len = code_size + eeprom_addr_src;
    return eeprom_data;
}
static Node* Ihx_Parse_Memory(MSD_U8* filename)
{
    MSD_U8* ihx_data;
    MSD_U16 ihx_data_len;
    MSD_U16 Idx;
    MSD_U8 Dnum;
    MSD_U16 type;
    MSD_U8 chk;
    MSD_U16 addr;
    MSD_U16 i;

    Node *head = NULL, *cur = NULL, *ne = NULL;

    FILE *pfile;
    if ((pfile = fopen(filename, "rb")) == NULL)
    {
        CLI_ERROR("Can not open the file: %s\n", filename);
        return NULL;
    }

    fseek(pfile, 0, SEEK_END);
    ihx_data_len = (MSD_U16)ftell(pfile);

    ihx_data = (unsigned char*)malloc(ihx_data_len*sizeof(char) + 1);
    rewind(pfile);
    fread(ihx_data, sizeof(unsigned char), ihx_data_len, pfile);

    type = 0;
    Idx = 0;
    while (type != 1)
    {
        while ((ihx_data[Idx] != ':') && ihx_data[Idx] > 0)
            Idx++;
        if (ihx_data[Idx] != ':')
        {
            /* ihx file format error */
            freeNode(head);
            head = NULL;
            free(ihx_data);
            return NULL;
        }
        Idx++;

        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        chk = Dnum;
        Idx += 2;

        addr = ReadInt(ihx_data + Idx, 2);
        chk += (addr & 0xff);
        chk += ((addr >> 8) & 0xff);
        Idx += 4;

        type = ReadInt(ihx_data + Idx, 1);
        chk += type;
        Idx += 2;

        if (type == 0)
        {
            ne = (Node*)malloc(sizeof(Node));
            ne->len = Dnum;
            ne->addr = addr;
            ne->data = (MSD_U8*)malloc(Dnum * sizeof(MSD_U8));
            ne->next = NULL;

            if (head == NULL)
                head = ne;

            if (cur == NULL)
                cur = ne;
            else
            {
                cur->next = ne;
                cur = ne;
            }
        }

        for (i = 0; i < Dnum; i++)
        {
            chk += ReadInt(ihx_data + Idx, 1);
            if (type == 0)
                cur->data[i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);

            Idx += 2;
        }

        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
        {
            freeNode(head);
            head = NULL;
            free(ihx_data);
            return NULL;
        }

        Idx += 2;
    }
    free(ihx_data);

    return head;
}
static int UpdateFilename(char *tempName)
{
    char *pTemp = tempName;
    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }
    else if (nargs == 3)
    {
        char *p = CMD_ARGS[2];
        while (*p != 0)
        {
            if (*p != '\"')
            {
                *pTemp = *p;
                pTemp++;
            }
            p++;
        }
        *pTemp = 0;
    }
    else
    {
        char *p = tempName;
        if (g_filename == NULL)
        {
            if (gCurrentCmd != NULL)
                CLI_ERROR("Syntax Error, invlid file name: %s\n", CMD_ARGS[2]);
            return 1;
        }

        strcat(tempName, g_filename);
        while (*p != 0 && *p != '\n')
        {
            if (*p != '\"')
            {
                *pTemp = *p;
                pTemp++;
            }
            p++;
        }
        *pTemp = 0;
    }

    return 0;
}
int IMPRun()
{
	MSD_STATUS retVal;
	MSD_U16	addr;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	addr = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdImpRun(sohoDevNum, addr);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error start run the IMP ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int IMPReset()
{
	MSD_STATUS retVal;
	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	retVal = msdImpReset(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error reset the IMP ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int IMPStop()
{
	MSD_STATUS retVal;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	retVal = msdImpStop(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error write EEPROM ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int IMPWriteEEPROM()
{
	MSD_STATUS retVal;
	MSD_U16	addr;
	MSD_U8	data;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	addr = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	data = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);

	retVal = msdImpWriteEEPROM(sohoDevNum, addr, data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error write EEPROM ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int IMPLoadToEEPROM()
{
	MSD_STATUS retVal;
	MSD_U8	*data = NULL;
    MSD_U16 data_len = 0;
    I2C_SPEED speed = S200k;
    char filename[MAX_ARGS_LENGTH] = { 0 };

    UpdateFilename(filename);

    MSD_U16 ram_dest_addr = 0;
    MSD_U16 ram_exec_addr = 0;
    MSD_U16 eeprom_addr_src = 0x10;

    data = Ihx_Parse_EEPROM(&data_len, filename, speed, eeprom_addr_src, ram_dest_addr, ram_exec_addr);

    if (data == NULL)
    {
        CLI_ERROR("Parsing the %s error, make sure the file is correct\n", filename);
        return MSD_FAIL;
    }

    /* load data to eeprom */
    retVal = msdImpLoadToEEPROM(sohoDevNum, data_len, data);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            free(data);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error write EEPROM ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        free(data);
        return MSD_FAIL;
    }

    free(data);
	return 0;
}
int IMPLoadToRAM()
{
	MSD_STATUS retVal;
    char filename[MAX_ARGS_LENGTH] = { 0 };
    MSD_U16 addr = 0xffff;
    Node* head;
    Node* node;

    UpdateFilename(filename);

    head = Ihx_Parse_Memory(filename);
    node = head;

    if (head == NULL)
    {
        CLI_ERROR("Parsing the %s error, make sure the file is correct\n", filename);
        return MSD_FAIL;
    }

    /* load data to memory */
    while (head != NULL)
    {
        retVal = msdImpLoadToRAM(sohoDevNum, head->addr, head->len, head->data);
        if (retVal != MSD_OK) {
            if (retVal == MSD_NOT_SUPPORTED)
            {
                CLI_WARNING("The device maybe not support this feature, please double checked\n");
                freeNode(node);
                return MSD_NOT_SUPPORTED;
            }
            CLI_ERROR("Error write EEPROM ret[%d: %s]"
                "\n", retVal, reflectMSD_STATUS(retVal));
            freeNode(node);
            return MSD_FAIL;
        }
        if (addr > head->addr)
            addr = head->addr;
        head = head->next;
    }

    freeNode(node);
    node = NULL;

	return 0;
}
void impParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, impCMDList));
	if (strIcmp(cmd, "run") == 0)
	{
		CLI_INFO("	addr : specified start address\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        imp run 0\n");
	}
	else if (strIcmp(cmd, "reset") == 0)
	{

		CLI_INFO("Example:\n");
		CLI_INFO("        imp reset\n");
	}
	else if (strIcmp(cmd, "stop") == 0)
	{

		CLI_INFO("Example:\n");
		CLI_INFO("        imp stop\n");
	}
	else if (strIcmp(cmd, "writeEEPROM") == 0)
	{
		CLI_INFO("	addr : EEPROM address\n");
		CLI_INFO("	data : to be written\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        imp writeEEPROM 20 00\n"); 
	}
	else if (strIcmp(cmd, "loadToEEPROM") == 0)
	{
		CLI_INFO("	filename : ihx file name to load\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        imp loadToEEPROM test.ihx\n");
	}
	else if (strIcmp(cmd, "loadToRAM") == 0)
	{
        CLI_INFO("	filename : ihx file name to load\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        imp loadToRAM test.ihx\n");
	}
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int impHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		impParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (impCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", impCMDList[index].help);
		index++;
	}

	return 0;
}
int impOperation()
{
	int index = 0;
	while (1)
	{
		if (impCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], impCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(impCMDList[index]);
			impCMDList[index].callback();
			break;
		}
		index++;
	}
	if (impCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"imp help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

int phyReset()
{
	MSD_STATUS retVal;
	MSD_LPORT port;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdPhyReset(sohoDevNum, port);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error phy reset ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
void phyParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, phyCMDList));
	if (strIcmp(cmd, "reset") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("       	  The physical address, if SERDES device is accessed\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy reset 1\n");
	}
	else if (strIcmp(cmd, "setLoopback") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("       	  The physical address, if SERDES device is accessed\n");
		CLI_INFO("	enable : enable/disable loopback mode\n");
		CLI_INFO("		    1 : enable loopback mode\n");
		CLI_INFO("		    0 : disable loopback mode\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy setLoopback 1 1\n");
	}
	else if (strIcmp(cmd, "setSpeed") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("	        The physical address, if SERDES device is accessed\n");
		CLI_INFO("	setSpeed : port speed.\n");
		CLI_INFO("		    0 : PHY_SPEED_10_MBPS for 10Mbps\n");
		CLI_INFO("		    1 : PHY_SPEED_100_MBPS for 100Mbps\n");
		CLI_INFO("		    2 : PHY_SPEED_1000_MBPS for 1000Mbps\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy setSpeed 1 1\n");
	}
	else if (strIcmp(cmd, "powerDown") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("	         The physical address, if SERDES device is accessed\n");
		CLI_INFO("	enable : enable/disable <power down>\n");
		CLI_INFO("		    1 : power down\n");
		CLI_INFO("		    0 : normal operation\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy powerDown 1 1\n");
	}
	else if (strIcmp(cmd, "setDuplex") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("	        The physical address, if SERDES device is accessed\n");
		CLI_INFO("	mode : dulpex mode\n");
        CLI_INFO("		    1 : Full-duplex\n");
        CLI_INFO("		    0 : Half-duplex\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy setDuplex 1 1\n");
		
	}
	else if (strIcmp(cmd, "setAutoMode") == 0)
	{
		CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
		CLI_INFO("	        The physical address, if SERDES device is accessed\n");
		CLI_INFO("	mode : Auto Mode to be written\n");
		CLI_INFO("		    0 : SPEED_AUTO_DUPLEX_AUTO\n");
		CLI_INFO("		    1 : SPEED_1000_DUPLEX_AUTO\n");
		CLI_INFO("		    2 : SPEED_100_DUPLEX_AUTO\n");
		CLI_INFO("		    3 : SPEED_10_DUPLEX_AUTO\n");
		CLI_INFO("		    4 : SPEED_AUTO_DUPLEX_FULL\n");
		CLI_INFO("		    5 : SPEED_AUTO_DUPLEX_HALF\n");
		CLI_INFO("		    6 : SPEED_1000_DUPLEX_FULL\n");
		CLI_INFO("		    7 : SPEED_1000_DUPLEX_HALF\n");
		CLI_INFO("		    8 : SPEED_100_DUPLEX_FULL\n");
		CLI_INFO("		    9 : SPEED_100_DUPLEX_HALF\n");
		CLI_INFO("		    10 : SPEED_10_DUPLEX_FULL\n");
		CLI_INFO("		    11 : SPEED_10_DUPLEX_HALF\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy setAutoMode 1 1\n");
	}
    else if (strIcmp(cmd, "setEEE") == 0)
    {
        CLI_INFO("	port : The logical port number, unless SERDES device is accessed\n");
        CLI_INFO("	en   : 1 is enable EEE, 0 is disable EEE\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy setEEE 1 1\n");
    }
    else if (strIcmp(cmd, "rr") == 0)
    {
        CLI_INFO("  devAddr : physical phy address, the base address for Peridot is 0, and for Topaz is 0x10\n");
        CLI_INFO("  regAddr : register address\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy rr 0x11 1 (for Topaz)\n");
		CLI_INFO("        phy rr 1 1 (for Peridot)\n");
    }
    else if (strIcmp(cmd, "rw") == 0)
    {
        CLI_INFO("  devAddr : physical phy address, the base address for Peridot is 0, and for Topaz is 0x10\n");
        CLI_INFO("  regAddr : register address\n");
        CLI_INFO("  data    : data to be written\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        phy rw 0x11 1 0x1940 (for Topaz)\n");
		CLI_INFO("        phy rw 1 1 0x1940 (for Peridot)\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int phyHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		phyParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (phyCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", phyCMDList[index].help);
		index++;
	}

	return 0;
}
int phyOperation()
{
	int index = 0;
	while (1)
	{
		if (phyCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], phyCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(phyCMDList[index]);
			phyCMDList[index].callback();
			break;
		}
		index++;
	}
	if (phyCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"phy help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

void serdesParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, serdesCMDList));
    if (strIcmp(cmd, "xrr") == 0)
    {
        CLI_INFO("  devAddr : serdes device address\n");
        CLI_INFO("  phyAddr : serdes physical phy address\n");
        CLI_INFO("  regAddr : serdes register address\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        serdes xrr 4 0x9 0x1000\n");
    }
    else if (strIcmp(cmd, "xrw") == 0)
    {
        CLI_INFO("  devAddr : serdes device address\n");
        CLI_INFO("  phyAddr : serdes physical phy address\n");
        CLI_INFO("  regAddr : serdes register address\n");
        CLI_INFO("  data    : data to be written\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        serdes xrw 4 0x9 0x1000 0x6040\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
int serdesHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        serdesParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (serdesCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", serdesCMDList[index].help);
        index++;
    }

    return 0;
}
int serdesOperation()
{
    int index = 0;
    while (1)
    {
        if (serdesCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], serdesCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(serdesCMDList[index]);
            serdesCMDList[index].callback();
            break;
        }
        index++;
    }
    if (serdesCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"serdes help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static int updateCustomStruct(MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup, int startIndex)
{
	int index;
	char* cmd, *value;
    for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];		
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-isValid") == 0)
		{
			customSetup->isValid = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ebsLimit") == 0)
		{
			customSetup->ebsLimit = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-cbsLimit") == 0)
		{
			customSetup->cbsLimit = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-bktIncrement") == 0)
		{
			customSetup->bktIncrement = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-bktRateFactorGrn") == 0)
		{
			customSetup->bktRateFactorGrn = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-bktRateFactorYel") == 0)
		{
			customSetup->bktRateFactorYel = (MSD_U32)strtoul(value, NULL, 0);
		}
		//else if (strIcmp(cmd, "-Delta") == 0)
		//{
		//	customSetup->Delta = (MSD_DOUBLE)strtod(value, NULL);
		//}
		else if (strIcmp(cmd, "-IRLConstant") == 0)
		{
			customSetup->IRLConstant = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-countMode") == 0)
		{
			customSetup->countMode = (MSD_U32)strtoul(value, NULL, 0);
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static int  updatePkttypeOrPri(MSD_PIRL_PKTTYPE_OR_PRI *pktTypeOrPri, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; )
    {
        cmd = CMD_ARGS[index];        
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-bktTypeMask") == 0)
        {
            pktTypeOrPri->bktTypeMask = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamFlows") == 0)
        {
            pktTypeOrPri->tcamFlows = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priAndPt") == 0)
        {
            pktTypeOrPri->priAndPt = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-useFPri") == 0)
        {
            pktTypeOrPri->useFPri = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priSelect") == 0)
        {
            pktTypeOrPri->priSelect = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutCustomSetup(MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup)
{
	CLI_INFO("Custom Setup info:\n");
	CLI_INFO("		isValid:            %X\n", customSetup->isValid);
	CLI_INFO("		ebsLimit:           %lX\n", customSetup->ebsLimit);
	CLI_INFO("		cbsLimit:           %lX\n", customSetup->cbsLimit);
	CLI_INFO("		bktIncrement:       %lX\n", customSetup->bktIncrement);
	CLI_INFO("		bktRateFactorGrn:   %lX\n", customSetup->bktRateFactorGrn);
	CLI_INFO("		bktRateFactorYel:   %lX\n", customSetup->bktRateFactorYel);
	//CLI_INFO("		Delta:              %lf\n", customSetup->Delta);
	CLI_INFO("		IRLConstant:        %lX\n", customSetup->IRLConstant);
	CLI_INFO("		countMode:          %X\n", customSetup->countMode);
}
static int updateIrlRes(MSD_PIRL_DATA *pirlData, int startIndex)
{
	int index;
	char* cmd, *value;
    MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup = &(pirlData->customSetup);
	for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-bktTypeMask") == 0)
		{
			pirlData->bktTypeMask = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-tcamFlows") == 0)
		{
			pirlData->tcamFlows = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-priAndPt") == 0)
		{
			pirlData->priAndPt = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-useFPri") == 0)
		{
			pirlData->useFPri = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-priSelect") == 0)
		{
			pirlData->priSelect = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-colorAware") == 0)
		{
			pirlData->colorAware = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-accountGrnOverflow") == 0)
		{
			pirlData->accountGrnOverflow = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-accountQConf") == 0)
		{
			pirlData->accountQConf = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-accountFiltered") == 0)
		{
			pirlData->accountFiltered = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-samplingMode") == 0)
		{
			pirlData->samplingMode = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-actionMode") == 0)
		{
			pirlData->actionMode = (MSD_PIRL_ACTION)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-fcMode") == 0)
		{
			pirlData->fcMode = (MSD_PIRL_FC_DEASSERT)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-fcPriority") == 0)
		{
			pirlData->fcPriority = (MSD_U8)strtoul(value, NULL, 0);
		}
        else if (strIcmp(cmd, "-isValid") == 0)
        {
            customSetup->isValid = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ebsLimit") == 0)
        {
            customSetup->ebsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-cbsLimit") == 0)
        {
            customSetup->cbsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktIncrement") == 0)
        {
            customSetup->bktIncrement = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorGrn") == 0)
        {
            customSetup->bktRateFactorGrn = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorYel") == 0)
        {
            customSetup->bktRateFactorYel = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IRLConstant") == 0)
        {
            customSetup->IRLConstant = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-countMode") == 0)
        {
            customSetup->countMode = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static void printOutIrlRes(MSD_PIRL_DATA *pirlData)
{
	CLI_INFO("IRL Resource info:\n");

	CLI_INFO("	bktTypeMask:		%X\n", pirlData->bktTypeMask);
	CLI_INFO("	tcamFlows:			%X\n", pirlData->tcamFlows);
	CLI_INFO("	priAndPt:			%X\n", pirlData->priAndPt);
	CLI_INFO("	useFPri:			%X\n", pirlData->useFPri);
	CLI_INFO("	priSelect:			%X\n", pirlData->priSelect);
	CLI_INFO("	colorAware:			%X\n", pirlData->colorAware);
	CLI_INFO("	accountGrnOverflow:	%X\n", pirlData->accountGrnOverflow);
	CLI_INFO("	accountQConf:		%X\n", pirlData->accountQConf);
	CLI_INFO("	accountFiltered:	%X\n", pirlData->accountFiltered);
	CLI_INFO("	samplingMode:		%X\n", pirlData->samplingMode);
	CLI_INFO("	actionMode:			%X\n", pirlData->actionMode);
	CLI_INFO("	fcMode:				%X\n", pirlData->fcMode);
	CLI_INFO("	fcPriority:			%X\n", pirlData->fcPriority);
	printOutCustomSetup(&(pirlData->customSetup));
}
int irlInit()
{
	MSD_STATUS retVal;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    retVal = msdIRLReset(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Initialize IRL ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int irlInitRes()
{
	MSD_STATUS retVal;
	MSD_U32	irlPort;
	MSD_U32  irlRes;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdIRLInitResourceCounter(sohoDevNum, irlPort, irlRes);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Init IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
			"\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int irlCustomSetup()
{
	MSD_STATUS retVal;
	MSD_U32	tgtRate;
	MSD_U32  tgtBstSize;
	MSD_PIRL_CUSTOM_RATE_LIMIT  customSetup;
    MSD_PIRL_COUNT_MODE  countMode;

	if (nargs < 5)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	tgtRate = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	tgtBstSize = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    countMode = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

	memset(&customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT));
    if (updateCustomStruct(&customSetup, 5) != 0)
        return -1;

    retVal = msdIRLCustomSetup(sohoDevNum, tgtRate, tgtBstSize, countMode, &customSetup);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error custom setup tgtRate=%d tgtBstSize=%d ret[%d: %s]"
			"\n", (int)tgtRate, (int)tgtBstSize, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	printOutCustomSetup(&customSetup);

	return 0;

}
int irlReadRes()
{
	MSD_STATUS retVal;
	MSD_U32	irlPort;
	MSD_U32  irlRes;
	MSD_PIRL_DATA    pirlData;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	memset(&pirlData, 0, sizeof(MSD_PIRL_DATA));

    retVal = msdIRLResourceRead(sohoDevNum, irlPort, irlRes, &pirlData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
			"\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	printOutIrlRes(&pirlData);

	return 0;
}
int irlWriteRes()
{
	MSD_STATUS retVal;
	MSD_U32	irlPort;
	MSD_U32  irlRes;
	MSD_PIRL_DATA    pirlData;

	if (nargs < 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    memset(&pirlData, 0, sizeof(MSD_PIRL_DATA));
    if (updateIrlRes(&pirlData, 4) != 0)
        return -1;

    retVal = msdIRLResourceWrite(sohoDevNum, irlPort, irlRes, &pirlData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Write IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
			"\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int irlRReg()
{
	MSD_STATUS retVal;
	MSD_LPORT	irlPort;
	MSD_U32  irlRes;
	MSD_U32  regOffset;
	MSD_U16 data;

	if (nargs != 5)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	regOffset = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdIRLResRegGet(sohoDevNum, irlPort, irlRes, regOffset, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read IRL register irlPort=%d irlRes=%d offset=%d ret[%d: %s]"
			"\n", (int)irlPort, (int)irlRes, (int)regOffset, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("Read IRL Reg - port: %d, res: %d, offset: %d, data = %X \n", (int)irlPort, (int)irlRes, (int)regOffset, data);
	return 0;
}
int irlWReg()
{
	MSD_STATUS retVal;
	MSD_LPORT	irlPort;
	MSD_U32  irlRes;
	MSD_U32  regOffset;
	MSD_U16  data;

	if (nargs != 6)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	regOffset = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
	data = (MSD_U16)strtoul(CMD_ARGS[5], NULL, 0);

    retVal = msdIRLResRegSet(sohoDevNum, irlPort, irlRes, regOffset, data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read IRL register irlPort=%d irlRes=%d offset=%d ret[%d: %s]"
			"\n", (int)irlPort, (int)irlRes, (int)regOffset, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int irlAdvConfig()
{
    MSD_STATUS retVal;
    MSD_LPORT  irlPort;
    MSD_U32  irlRes;
    MSD_U32  tgtRate;
    MSD_U32  tgtBstSize;
    MSD_PIRL_COUNT_MODE  countMode;
    MSD_PIRL_PKTTYPE_OR_PRI pktTypeOrPri;

    if (nargs < 7)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    tgtRate = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    tgtBstSize = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);
    countMode = (MSD_U32)strtoul(CMD_ARGS[6], NULL, 0);
    if (updatePkttypeOrPri(&pktTypeOrPri, 7) != 0)
        return -1;

    retVal = msdIRLAdvConfig(sohoDevNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &pktTypeOrPri);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int irlAdvConfigDisable()
{
    MSD_STATUS retVal;
    MSD_LPORT  irlPort;
    MSD_U32  irlRes;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdIRLAdvConfigDisable(sohoDevNum, irlPort, irlRes);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
void irlParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, irlCMDList));
	if (strIcmp(cmd, "reset") == 0)
	{	
		CLI_INFO("Example:\n");
		CLI_INFO("        irl reset\n");
	}
	else if (strIcmp(cmd, "initResCount") == 0)
	{
		CLI_INFO("	port : target logical port\n");
		CLI_INFO("	res  : resource unit to be accessed\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl initResCount 0 0\n");
			
	}
	else if (strIcmp(cmd, "customSetup") == 0)
	{
		CLI_INFO("	tgtRate    : target rate\n");
		CLI_INFO("	tgtBstSize : target burst size\n");
		CLI_INFO("	countMode  :\n");
		CLI_INFO("	        0 : MSD_PIRL_COUNT_FRAME\n");
		CLI_INFO("	        1 : MSD_PIRL_COUNT_ALL_LAYER1\n");
		CLI_INFO("	        2 : MSD_PIRL_COUNT_ALL_LAYER2\n");
		CLI_INFO("	        3 : MSD_PIRL_COUNT_ALL_LAYER3\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl customSetup 1000000 3000 2\n");
	}
	else if (strIcmp(cmd, "readRes") == 0)
	{
		CLI_INFO("	port : target logical port\n");
		CLI_INFO("	res  : resource unit to be accessed\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl readRes 1 1\n");
	}
	else if (strIcmp(cmd, "writeRes") == 0)
	{
		CLI_INFO("	port : target logical port\n");
		CLI_INFO("	res  : resource unit to be accessed\n");
		CLI_INFO("	[options](default value for option item is 0):\n");
        CLI_INFO("	options to set IRL data\n");
		CLI_INFO("	-bktTypeMask : various traffic type to be counted\n");
		CLI_INFO("	-tcamFlows : indicate to account for tcam flows 0 to 3, but not TCP/UDP data\n");
		CLI_INFO("	-priAndPt : determine the incoming frames\n");
		CLI_INFO("	        0 : typeMask Or priMask\n");
		CLI_INFO("	        1 : typeMask And priMask\n");
		CLI_INFO("	-useFPri : determine what priority type this resource is to account\n");
		CLI_INFO("	        0 : use the QPri assigned to the frame\n");
		CLI_INFO("	        1 : use the FPri assigned to the frame\n");
		CLI_INFO("	-priSelect : priority bit mask that each bit indicates one of the eight priorities\n");
		CLI_INFO("	-colorAware : set to color aware mode or color blind mode\n");
		CLI_INFO("	-accountGrnOverflow : Green&Yellow buckets are connected when in Coloar Aware mode\n");
		CLI_INFO("	-accountQConf : account discarded frames due to queue congestion\n");
		CLI_INFO("	-accountFiltered : account filtered frames. Account for All\n");
		CLI_INFO("	-samplingMode : sample one out of so many frames/bytes for a stream of frames\n");
		CLI_INFO("	-actionMode : action should be taken when there are not enough tokens\n");
		CLI_INFO("	        0 : PIRL_ACTION_DROP - drop frame\n");
		CLI_INFO("	        1 : PIRL_ACTION_FC - send flow control mode\n");
		CLI_INFO("	-fcMode : flow control de-assertion mode\n");
		CLI_INFO("	        0 : MSD_PIRL_FC_DEASSERT_EMPTY\n");
		CLI_INFO("	        1 : MSD_PIRL_FC_DEASSERT_CBS_LIMIT\n");
		CLI_INFO("	-fcPriority : used when port PFC mode enabled\n");
		
		CLI_INFO("	-isValid      : Reserved for future use\n");
		CLI_INFO("	-ebsLimit     : Excess Burst Size limit on the Yellow bucket ( 0 ~ 0xFFFFFF)\n");
		CLI_INFO("	-cbsLimit     : Committed Burst Size limit on the Green bucket\n");
		CLI_INFO("	-bktIncrement : the amount of tokens that need to be added for each byte of packet information.\n");
		CLI_INFO("	-bktRateFactorGrn    : the amount of tokens that need to be decremented for each rate\n");
		CLI_INFO("	-bktRateFactorYel    : the amount of tokens that need to be decremented for each rate\n");
		/*CLI_INFO("	-Delta        : the error ratio between Calculated Rate and Target Rate((Rate - TgtRate)/TgtRate)\n");*/
		CLI_INFO("	-IRLConstant  : Constant can be any value from {7812500, 125000000, \n");
		CLI_INFO("	                62500000, 31250000, 15625000, 3906250, 1953750, 976250} \n");
		CLI_INFO("	-countMode    : Frame bytes or Frames to be accounted for in the rate resource's rate limiting calculations\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl writeRes 1 1 -bktTypeMask 5 -tcamFlows 0 -priAndPt 1 -priSelect 4 -colorAware 0 -accountGrnOverflow 0 -accountQConf 1  -samplingMode 1 -actionMode 1 -fcPriority 2 -isValid 1 -ebsLimit 0xff00 -cbsLimit 1 -bktIncrement 1 -bktRateFactorGrn 1 -IRLConstant 7812500 -countMode 2\n");
	}
	else if (strIcmp(cmd, "rr") == 0)
	{
		CLI_INFO("	port   : logical port number\n");
		CLI_INFO("	res    :  target resource\n");
		CLI_INFO("	offset :  register address\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl rr 0 0 0\n");
	}
	else if (strIcmp(cmd, "rw") == 0)
	{
		CLI_INFO("	port	: logical port number\n");
		CLI_INFO("	res     : target resource\n");
		CLI_INFO("	offset  : register address\n");
		CLI_INFO("	data	: register value\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl rw 0 0 0 1\n");
	}
    else if (strIcmp(cmd, "advConfig") == 0)
    {
        CLI_INFO("	port : target logical port\n");
        CLI_INFO("	res  : resource unit to be accessed\n");
        CLI_INFO("	tgtRate    : target rate\n");
        CLI_INFO("	tgtBstSize : target burst size\n");
        CLI_INFO("	countMode  :\n");
        CLI_INFO("	        0 : MSD_PIRL_COUNT_FRAME\n");
        CLI_INFO("	        1 : MSD_PIRL_COUNT_ALL_LAYER1\n");
        CLI_INFO("	        2 : MSD_PIRL_COUNT_ALL_LAYER2\n");
        CLI_INFO("	        3 : MSD_PIRL_COUNT_ALL_LAYER3\n");
        CLI_INFO("	[options](default value for option item is 0):\n");
        CLI_INFO("	options to set packet type or priority\n");
        CLI_INFO("	-bktTypeMask : various traffic type to be counted\n");
        CLI_INFO("	-tcamFlows   : indicate to account for tcam flows 0 to 3, but not TCP/UDP data\n");
        CLI_INFO("	-priAndPt    : determine the incoming frames that get rate limited using this ingress rate resource\n");
        CLI_INFO("	-useFPri     : used to determine what priority type this resource is to account.\n");
        CLI_INFO("	-priSelect   : priority bit mask that each bit indicates one of the eight priorities\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl advConfig  1 2 1000000 3000 2 -bktTypeMask 5 -tcamFlows 0 -priAndPt 1\n");
    }
    else if (strIcmp(cmd, "disableAdvConfig") == 0)
    {
        CLI_INFO("	port : target logical port\n");
        CLI_INFO("	res  : resource unit to be accessed\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        irl disableAdvConfig 1 2\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int irlHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		irlParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (irlCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", irlCMDList[index].help);
		index++;
	}

	return 0;
}
int irlOperation()
{
	int index = 0;
	while (1)
	{
		if (irlCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], irlCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(irlCMDList[index]);
			irlCMDList[index].callback();
			break;
		}
		index++;
	}
	if (irlCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"irl help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static int updateVctMode(MSD_ADV_VCT_MODE *mode, int startIndex)
{
	int index;
	char* cmd, *value;
	for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-mode") == 0)
		{
			mode->mode = (MSD_ADV_VCT_MOD)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-transChanSel") == 0)
		{
			mode->transChanSel = (MSD_ADV_VCT_TRANS_CHAN_SEL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-sampleAvg") == 0)
		{
			mode->sampleAvg = (MSD_ADV_VCT_SAMPLE_AVG)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-peakDetHyst") == 0)
		{
			mode->peakDetHyst = (MSD_ADV_VCT_PEAKDET_HYST)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-samplePointDist") == 0)
		{
			mode->samplePointDist = (MSD_16)strtoul(value, NULL, 0);
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static void displayAdvVCTResult(MSD_ADV_CABLE_STATUS *cableStatus, int channel, MSD_ADV_VCT_MODE mode)
{
	int i, j = channel;

	if (mode.mode == MSD_ADV_VCT_OFFSET)
	{
		if (cableStatus->u[j].offset == 0)
			CLI_INFO("Advanced VCT offset pair %x test failed\n", j);
		else if (cableStatus->u[j].offset == 0x80)
			CLI_INFO("Advanced VCT offset pair %x is 0 \n", j);
		else
			CLI_INFO("Advanced VCT offset pair %x is %d \n", j, cableStatus->u[j].offset);

		return;
	}
	if (mode.mode == MSD_ADV_VCT_SAMPLE_POINT)
	{
		if (cableStatus->u[j].sampleAmp & 0x80)
			CLI_INFO("Advanced VCT sample point return pair %x is -%d \n", j, cableStatus->u[j].sampleAmp & 0x7f);
		else
			CLI_INFO("Advanced VCT sample point return pair %x is +%d \n", j, cableStatus->u[j].sampleAmp & 0x7f);

		return;
	}

	switch (cableStatus->cableStatus[channel])
	{
	case MSD_ADV_VCT_FAIL:
		CLI_INFO("Advanced Cable Test Failed\n");
		break;
	case MSD_ADV_VCT_NORMAL:
		CLI_INFO("No problem found.\n");
		break;
	case MSD_ADV_VCT_IMP_GREATER_THAN_115:
		CLI_INFO("Impedance is greater than 115 Ohms.\n");
		CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
		break;
	case MSD_ADV_VCT_IMP_LESS_THAN_85:
		CLI_INFO("Impedance is less than 85 Ohms.\n");
		CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
		break;
	case MSD_ADV_VCT_OPEN:
		CLI_INFO("Open Cable.\n");
		CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
		break;
	case MSD_ADV_VCT_SHORT:
		CLI_INFO("Shorted Cable.\n");
		CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
		break;
	case MSD_ADV_VCT_CROSS_PAIR_SHORT:
		/*CLI_INFO("Cable Test Passed.\n");*/
		for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
		{
			if (cableStatus->u[channel].crossShort.channel[i] == MSD_TRUE)
			{
				CLI_INFO("\tCross pair short in channel %i.\n", i);
				CLI_INFO("\tApproximately %i meters from the tested port.\n",
					cableStatus->u[channel].crossShort.dist2fault[i]);
			}
		}
		break;
	default:
		CLI_INFO("Unknown Test Result.\n");
		break;
	}
}
static void vctParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, vctCMDList));
    if (strIcmp(cmd, "cableDiag") == 0)
    {
        CLI_INFO("	port : target internal phy port\n");
        CLI_INFO("	<modeOptions>:\n");
        CLI_INFO("	 -mode :\n");
        CLI_INFO("		0 :  MSD ADV VCT MAX PEAK \n");
        CLI_INFO("		1 :  MSD ADV VCT FIRST PEAK \n");
        CLI_INFO("		2 :  MSD ADV VCT OFFSET \n");
        CLI_INFO("		3 :  MSD ADV VCT SAMPLE POINT \n");
        CLI_INFO("	 -transChanSel:\n");
        CLI_INFO("		0 :  MSD ADV VCT TCS NO CROSSPAIR\n");
        CLI_INFO("		4 :  MSD ADV VCT TCS CROSSPAIR 0\n");
        CLI_INFO("		5 :  MSD ADV VCT TCS CROSSPAIR 1\n");
        CLI_INFO("		6 :  MSD ADV VCT TCS CROSSPAIR 2\n");
        CLI_INFO("		7 :  MSD ADV VCT TCS CROSSPAIR 3\n");
        CLI_INFO("	 -sampleAvg:\n");
        CLI_INFO("		0 :  MSD ADV VCT SAVG 2\n");
        CLI_INFO("		1 :  MSD ADV VCT SAVG 4\n");
        CLI_INFO("		2 :  MSD ADV VCT SAVG 8\n");
        CLI_INFO("		3 :  MSD ADV VCT SAVG 16\n");
        CLI_INFO("		4 :  MSD ADV VCT SAVG 32\n");
        CLI_INFO("		5 :  MSD ADV VCT SAVG 64\n");
        CLI_INFO("		6 :  MSD ADV VCT SAVG 128\n");
        CLI_INFO("		7 :  MSD ADV VCT SAVG 256\n");
        CLI_INFO("	 -peakDetHyst    : MSD ADV VCT PEAKDET HYST \n");
        CLI_INFO("	 -samplePointDist: sample Point Dist\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vct cableDiag 1 -mode 0 -transChanSel 4\n");
    }
    else if (strIcmp(cmd, "extenedStatus") == 0)
    {
        CLI_INFO("	port : target internal phy port\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        vct extenedStatus 1\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
int vctCableDiag()
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_ADV_VCT_MODE mode;
	MSD_ADV_CABLE_STATUS cableStatus;

	if (nargs < 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}
	memset(&mode, 0, sizeof(MSD_ADV_VCT_MODE));
	memset(&cableStatus, 0, sizeof(MSD_ADV_CABLE_STATUS));

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if(updateVctMode(&mode, 3) != 0)
        return -1;

	retVal = msdVctGetAdvCableDiag(sohoDevNum, port, mode, &cableStatus);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Get Cable Diag port=%d ret[%d: %s]"
			"\n", (int)port, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	if (mode.transChanSel <MSD_ADV_VCT_TCS_CROSSPAIR_0 || mode.transChanSel > MSD_ADV_VCT_TCS_CROSSPAIR_3)
	{
		CLI_ERROR("Invalid transChanSel: %d\n", mode.transChanSel);
	}
	else
	{
		CLI_INFO("MDI PAIR %i:\n", mode.transChanSel - 4);
		displayAdvVCTResult(&cableStatus, mode.transChanSel - 4, mode);
	}

	return 0;
}
int vctExtenedStatus()
{
	MSD_STATUS status;
	MSD_LPORT port;
	MSD_ADV_EXTENDED_STATUS extendedStatus;
	int i;
	char ch;

	if (nargs < 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&extendedStatus, 0, sizeof(MSD_ADV_EXTENDED_STATUS));
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

	/*
	*     Start getting Extended Information.
	*/
	if ((status = msdVctGetAdvExtendStatus(sohoDevNum, port, &extendedStatus)) != MSD_OK)
	{
		CLI_INFO("msdVctGetAdvExtendStatus return Failed\n");
		return status;
	}

	if (!extendedStatus.isValid)
	{
		CLI_INFO("Not able to get Extended Status.\n");
		CLI_INFO("Please check if 1000B-T Link is established on Port %i.\n", (int)port);
		return status;
	}

	/* Pair Polarity */
	CLI_INFO("Pair Polarity:\n");
	for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
	{
		CLI_INFO("MDI PAIR %i: %s\n", i,
			(extendedStatus.pairPolarity[i] == MSD_POSITIVE) ? "Positive" : "Negative");
	}

	/* Pair Swap */
	CLI_INFO("Pair Swap:\n");
	for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
	{
		switch (extendedStatus.pairSwap[i])
		{
		case MSD_CHANNEL_A:
			ch = 'A';
			break;
		case MSD_CHANNEL_B:
			ch = 'B';
			break;
		case MSD_CHANNEL_C:
			ch = 'C';
			break;
		case MSD_CHANNEL_D:
			ch = 'D';
			break;
		default:
			CLI_ERROR("Error: reported unknown Pair Swap %i\n", extendedStatus.pairSwap[i]);
			ch = 'U';
			break;
		}

		CLI_INFO("MDI PAIR %i: Channel %c\n", i, ch);
	}

	/* Pair Skew */
	CLI_INFO("Pair Skew:\n");
	for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
	{
		CLI_INFO("MDI PAIR %i: %ins\n", i, (int)extendedStatus.pairSkew[i]);
	}

	/* Cable Len */
	CLI_INFO("Cable Len:\n");
	for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
	{
		CLI_INFO("MDI PAIR %i: approximately %im\n", i, (int)extendedStatus.cableLen[i]);
	}

	return 0;
}
int vctHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		vctParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (vctCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", vctCMDList[index].help);
		index++;
	}

	return 0;
}
int vctOperation()
{
	int index = 0;
	while (1)
	{
		if (vctCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], vctCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(vctCMDList[index]);
			vctCMDList[index].callback();
			break;
		}
		index++;
	}
	if (vctCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"vct help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static void qavParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, qavCMDList));
    if (strIcmp(cmd, "setXRate") == 0)
    {
		CLI_INFO("	port : target logical port\n");
        CLI_INFO("	queue : queue priority\n");
        CLI_INFO("	rate : rate in kbps and increments of 64kbps\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qav setXRate 1 1 64\n");

    }
    else if (strIcmp(cmd, "getXRate") == 0)
    {
        CLI_INFO("	port : target logical port\n");
        CLI_INFO("	queue : queue priority\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qav getXRate 1 1\n");
    }
    else if (strIcmp(cmd, "setXHiLimit") == 0)
    {
        CLI_INFO("	port : target logical port\n");
        CLI_INFO("	queue : queue priority\n");
        CLI_INFO("	hiLimit : number of credits in bytes\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qav setXHiLimit 1 1 1\n");
    }
    else if (strIcmp(cmd, "getXHiLimit") == 0)
    {
        CLI_INFO("	port : target logical port\n");
        CLI_INFO("	queue : queue priority\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qav getXHiLimit 1 1\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
int qavHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        qavParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (qavCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", qavCMDList[index].help);
        index++;
    }

    return 0;
}
int qavOperation()
{
    int index = 0;
    while (1)
    {
        if (qavCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], qavCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(qavCMDList[index]);
            qavCMDList[index].callback();
            break;
        }
        index++;
    }
    if (qavCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"qav help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static int updateIntEnable(MSD_PTP_PORT_INTERRUPT_EN *interruptEn, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; )
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpArrIntEn") == 0)
        {
            interruptEn->ptpArrIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpDepIntEn") == 0)
        {
            interruptEn->ptpDepIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updatePortConfig(MSD_PTP_PORT_CONFIG* ptpData, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; )
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        
        if (strIcmp(cmd, "-transSpec") == 0)
        {
            ptpData->transSpec = (MSD_PTP_SPEC)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disTSpec") == 0)
        {
            ptpData->disTSpec = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etJump") == 0)
        {
            ptpData->etJump = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ipJump") == 0)
        {
            ptpData->ipJump = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpArrIntEn") == 0)
        {
            ptpData->ptpArrIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpDepIntEn") == 0)
        {
            ptpData->ptpDepIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disTSOverwrite") == 0)
        {
            ptpData->disTSOverwrite = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-filterAct") == 0)
        {
            ptpData->filterAct = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-hwAccPtp") == 0)
        {
            ptpData->hwAccPtp = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-kpFramwSa") == 0)
        {
            ptpData->kpFramwSa = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ExthwAcc") == 0)
        {
            ptpData->ExthwAcc = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrTSMode") == 0)
        {
            ptpData->arrTSMode = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrLedCtrl") == 0)
        {
            ptpData->arrLedCtrl = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-depLedCtrl") == 0)
        {
            ptpData->depLedCtrl = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpPortConfig(MSD_PTP_PORT_CONFIG *ptpData)
{
    CLI_INFO("PTP Port Configuration:\n");
    CLI_INFO("  transSpec:      %d\n", ptpData->transSpec);
    CLI_INFO("  disTSpec:       %d\n", ptpData->disTSpec);
    CLI_INFO("  etJump:         0x%x\n", ptpData->etJump);
    CLI_INFO("  ipJump:         0x%x\n", ptpData->ipJump);
    CLI_INFO("  ptpArrIntEn:    %d\n", ptpData->ptpArrIntEn);
    CLI_INFO("  ptpDepIntEn:    %d\n", ptpData->ptpDepIntEn);
    CLI_INFO("  disTSOverwrite: %d\n", ptpData->disTSOverwrite);
    CLI_INFO("  filterAct:      %d\n", ptpData->filterAct);
    CLI_INFO("  hwAccPtp:       %d\n", ptpData->hwAccPtp);
    CLI_INFO("  kpFramwSa:      %d\n", ptpData->kpFramwSa);
    CLI_INFO("  ExthwAcc:       %d\n", ptpData->ExthwAcc);
    CLI_INFO("  arrTSMode:      0x%x\n", ptpData->arrTSMode);
    CLI_INFO("  arrLedCtrl:     0x%x\n", ptpData->arrLedCtrl);
    CLI_INFO("  depLedCtrl:     0x%x\n", ptpData->depLedCtrl);
}
static void printOutTimeStamp(MSD_PTP_TS_STATUS *ptpStatus)
{
    CLI_INFO("PTP Time Stamp status:\n");
    CLI_INFO("  isValid:        %d\n", ptpStatus->isValid);
    CLI_INFO("  timeStamped:    0x%x\n", ptpStatus->timeStamped);
    CLI_INFO("  ptpSeqId:       0x%x\n", ptpStatus->ptpSeqId);
    CLI_INFO("  status:         %d\n", ptpStatus->status);
}
static int updatePtpGlobalConfig(MSD_PTP_GLOBAL_CONFIG *ptpData, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; )
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpEType") == 0)
        {
            ptpData->ptpEType = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-msgIdTSEn") == 0)
        {
            ptpData->msgIdTSEn = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsArrPtr") == 0)
        {
            ptpData->tsArrPtr = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpGlobalConfig(MSD_PTP_GLOBAL_CONFIG *ptpData)
{
    CLI_INFO("PTP Global Configuration:\n");
    CLI_INFO("  ptpEType:       0x%x\n", ptpData->ptpEType);
    CLI_INFO("  msgIdTSEn:      0x%x\n", ptpData->msgIdTSEn);
    CLI_INFO("  tsArrPtr:       0x%x\n", ptpData->tsArrPtr);
}
static void printOutPtpIntStatus(MSD_PTP_INTERRUPT_STATUS *ptpIntSt)
{
    CLI_INFO("PTP Interrupt status:\n");
    CLI_INFO("  ptpTrigGenInt:      0x%x\n", ptpIntSt->ptpTrigGenInt);
    CLI_INFO("  ptpEventInt:        0x%x\n", ptpIntSt->ptpEventInt);
    CLI_INFO("  ptpIntStPortVect:   0x%x\n", ptpIntSt->ptpIntStPortVect);
}
static int updatePtpMode(MSD_PTP_MODE_CONFIG *mode, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-altScheme") == 0)
        {
            mode->altScheme = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-grandMstr") == 0)
        {
            mode->grandMstr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-oneStep") == 0)
        {
            mode->oneStep = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-PTPMode") == 0)
        {
            mode->PTPMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpMode(MSD_PTP_MODE_CONFIG *mode)
{
    CLI_INFO("PTP Mode Configuration:\n");
    CLI_INFO("  altScheme:       0x%x\n", mode->altScheme);
    CLI_INFO("  grandMstr:       0x%x\n", mode->grandMstr);
    CLI_INFO("  oneStep:         0x%x\n", mode->oneStep);
    CLI_INFO("  PTPMode:         0x%x\n", mode->PTPMode);
}
static int updateTimeArray(MSD_PTP_TIME_ARRAY *timeArray, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-todLoadPoint") == 0)
        {
            timeArray->todLoadPoint = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-clkValid") == 0)
        {
            timeArray->clkValid = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-domainNumber") == 0)
        {
            timeArray->domainNumber = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todNanoseconds") == 0)
        {
            timeArray->todNanoseconds = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todSecondsLow") == 0)
        {
            timeArray->todSecondsLow = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todSecondsHigh") == 0)
        {
            timeArray->todSecondsHigh = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Nanoseconds1722Low") == 0)
        {
            timeArray->Nanoseconds1722Low = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Nanoseconds1722High") == 0)
        {
            timeArray->Nanoseconds1722High = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todCompensation") == 0)
        {
            timeArray->todCompensation = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutTimeArray(MSD_U8 timeArrayIndex, MSD_PTP_TIME_ARRAY *timeArray)
{
    CLI_INFO("PTP timeArray %d Configuration:\n", timeArrayIndex);
    CLI_INFO("  todLoadPoint:       0x%x\n", timeArray->todLoadPoint);
    CLI_INFO("  clkValid:           0x%x\n", timeArray->clkValid);
    CLI_INFO("  domainNumber:       0x%x\n", timeArray->domainNumber);
    CLI_INFO("  todNanoseconds:     0x%x\n", timeArray->todNanoseconds);
    CLI_INFO("  todSecondsLow:      0x%x\n", timeArray->todSecondsLow);
    CLI_INFO("  todSecondsHigh:     0x%x\n", timeArray->todSecondsHigh);
    CLI_INFO("  Nanoseconds1722Low: 0x%x\n", timeArray->Nanoseconds1722Low);
    CLI_INFO("  Nanoseconds1722High:0x%x\n", timeArray->Nanoseconds1722High);
    CLI_INFO("  todCompensation:    0x%x\n", timeArray->todCompensation);
}
static int updatePtpPulse(MSD_PTP_PULSE_STRUCT *pulsePara, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpPulseWidth") == 0)
        {
            pulsePara->ptpPulseWidth = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsWidthRange") == 0)
        {
            pulsePara->ptp1ppsWidthRange = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsPhase") == 0)
        {
            pulsePara->ptp1ppsPhase = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsSelect") == 0)
        {
            pulsePara->ptp1ppsSelect = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPulse(MSD_PTP_PULSE_STRUCT *pulsePara)
{
    CLI_INFO("PTP Pulse Configuration:\n");
    CLI_INFO("  ptpPulseWidth:          0x%x\n", pulsePara->ptpPulseWidth);
    CLI_INFO("  ptp1ppsWidthRange:      0x%x\n", pulsePara->ptp1ppsWidthRange);
    CLI_INFO("  ptp1ppsPhase:           0x%x\n", pulsePara->ptp1ppsPhase);
    CLI_INFO("  ptp1ppsSelect:          0x%x\n", pulsePara->ptp1ppsSelect);
}
static void printIntEnOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for Port PTP interrupt Enable/Disable, format as: -ptpArrIntEn 1 -ptpDepIntEn 1\n");
    CLI_INFO("	-ptpArrIntEn    : Port Arrival Interrupt enable\n");
    CLI_INFO("          0 : Disable PTP Arrival capture interrupts\n");
    CLI_INFO("          1 : Enable PTP Arrival capture interrupts\n");
    CLI_INFO("	-ptpDepIntEn    : Port Departure Interrupt enable\n");
    CLI_INFO("          0 : Disable PTP Departure capture interrupts\n");
    CLI_INFO("          1 : Enable PTP Departure capture interrupts\n");
}
static void printOutGlobalOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for Port PTP Global configuration\n");
    CLI_INFO("	-ptpEType : PTP Ether Type\n");
    CLI_INFO("	-msgIdTSEn : Message IDs that needs time stamp\n");
    CLI_INFO("	-tsArrPtr : Time stamp arrival time counter pointer (either Arr0Time or Arr1Time)\n");
}
static void printPortConfigOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for  PTP configuration parameters for each port\n");
    CLI_INFO("  -transSpec : PTP (Precise Time Protocol) SPEC type\n");
    CLI_INFO("          0 : IEEE 1588\n");
    CLI_INFO("          1 : IEEE 802.1as\n");
    CLI_INFO("	-disTSpec : Disable Transport specific check\n");
    CLI_INFO("          0 : Enable checking for Transport Spec\n");
    CLI_INFO("          1 : Disnable checking for Transport Spec\n");
    CLI_INFO("	-etJump : offset to Ether type start address in bytes\n");
    CLI_INFO("	-ipJump : offset to IP header start address counting from Ether type offset\n");
    CLI_INFO("	-ptpArrIntEn : Port Arrival Interrupt enable\n");
    CLI_INFO("          0 : Disable PTP Arrival capture interrupts\n");
    CLI_INFO("          1 : Enable PTP Arrival capture interrupts\n");
    CLI_INFO("	-ptpDepIntEn : Port Departure Interrupt enable\n");
    CLI_INFO("          0 : Disable PTP Departure capture interrupts\n");
    CLI_INFO("          1 : Enable PTP Departure capture interrupts\n");
    CLI_INFO("	-disTSOverwrite : time stamp counter overwriting\n");
    CLI_INFO("          0 : Overwrite unread Time Stamps in the registers with new data\n");
    CLI_INFO("          1 : Keep unread Time Stamps in the registers until read\n");
    CLI_INFO("	-filterAct : Filter LED Activity\n");
    CLI_INFO("          0 : LED Activity is activated for all frames\n");
    CLI_INFO("          1 : LED Activity is not activated for most IEEE 802.1 frames\n");
    CLI_INFO("	-hwAccPtp : Port PTP Hardware Acceleration enable\n");
    CLI_INFO("          0 : No, or only Ingress PTP hardware acceleration\n");
    CLI_INFO("          1 : Ingress and Egress PTP hardware acceleration is enabled\n");
    CLI_INFO("	-kpFramwSa : Keep Frame's SA\n");
    CLI_INFO("          0 : Place Port's SA into modified egressing PTP frames\n");
    CLI_INFO("          1 : Keep the frame's SA even for modified egressing PTP frames\n");
    CLI_INFO("	-ExthwAcc : External Hardware Acceleration enable\n");
    CLI_INFO("          0 : Port has no External PHY or its PTP hardware acceleration is off\n");
    CLI_INFO("          1 : Port's Extermal PHY is enabled with PTP hardware acceleration\n");
    CLI_INFO("	-arrTSMode : Arrival Time Stamp Mode\n");
    CLI_INFO("	-arrLedCtrl : LED control for packets entering the device.\n");
    CLI_INFO("	-depLedCtrl : LED control for packets departing the device.\n");
}
static void printOutPtpModeOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for PTP mode parameters\n");
    CLI_INFO("	-altScheme : define the values that are returned in the requestReceiptTimestamp filed in IEEE 1588\n");
    CLI_INFO("          0 : RequestReceiptTimestamp=t2, responseOriginTimestamp=t3 & the correctionFiled = 0\n");
    CLI_INFO("          1 : RequestReceiptTimestamp=0 , responseOriginTimestamp=0  & the correctionFiled = turn around time\n");
    CLI_INFO("	-grandMstr : Grand Master enable\n");
    CLI_INFO("          0 : Hardware accelerate with this device NOT being the Grand Master\n");
    CLI_INFO("          1 : Hardware accelerate with this device being the Grand Master\n");
    CLI_INFO("	-oneStep  : One step enable\n");
    CLI_INFO("          0 : Hardware accelerate using AutoFollowUp Two Step frame formats\n");
    CLI_INFO("          1 : Hardware accelerate using One Step frame formats\n");
    CLI_INFO("	-PTPMode  : PTP mode\n");
    CLI_INFO("          0 : Boundary Clock\n");
    CLI_INFO("          1 : Peer to Peer Transparent Clock\n");
    CLI_INFO("          2 : End to End Transparent Clock\n");
}
static void printOutTimeArrayOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for PTP Time Array parameters\n");
    CLI_INFO("	-todLoadPoint : Time of Day Load Point.\n");
    CLI_INFO("	-clkValid : Clock Valid bit.\n");
    CLI_INFO("	-domainNumber: Domain Number, to be associated with the selected Time Array.\n");
    CLI_INFO("	-todNanoseconds: Time Array Time of Day, Nano second 32 bits.\n");
    CLI_INFO("	-todSecondsLow:       Time Array Time of Day, second portion 0-31 bits\n");
    CLI_INFO("	-todSecondsHigh:      Time Array Time of Day, second portion 32-47 bits\n");
    CLI_INFO("	-Nanoseconds1722Low:  Time Array 1722 Time of Day in Nano seconds 0-31 bits\n");
    CLI_INFO("	-Nanoseconds1722High: Time Array 1722 Time of Day in Nano seconds 32-63 bits\n");
    CLI_INFO("	-todCompensation:     Time Array Time of Day Compensation 32 bits\n");
}
static void printOutPtpPulseOption()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for PTP Pulse parameters\n");
    CLI_INFO("	-ptpPulseWidth :     Pulse Width for the 1 Pulse Per Second on the Second signal (0~0xf).\n");
    CLI_INFO("	-ptp1ppsWidthRange : Pulse Width Range for the 1 Pulse Per Second on the Second signal.\n");
    CLI_INFO("          0 : 8 nSec units for a 125MHz PTP clock\n");
    CLI_INFO("          1 : 64 nSec units for a 125MHz PTP clock\n");
    CLI_INFO("          2 : 512 nSec units for a 125MHz PTP clock\n");
    CLI_INFO("          3 : 4096 nSec units for a 125MHz PTP clock\n");
    CLI_INFO("          4 : 32.768 uSec units for a 125MHz PTP clock\n");
    CLI_INFO("          5 : 262.144 uSec units for a 125MHz PTP clock\n");
    CLI_INFO("          6 : 2.097 mSec units for a 125MHz PTP clock\n");
    CLI_INFO("          7 : 16.777 mSec units for a 125MHz PTP clock\n");
    CLI_INFO("	-ptp1ppsPhase: Phase of the 1 Pulse Per Second on the Second signal.\n");
    CLI_INFO("          0 : leading edge of the 1 PPS signal is the rising edge if the signal\n");
    CLI_INFO("          1 : leading edge of the 1 PPS signal is the falling edge if the signal\n");
    CLI_INFO("	-ptp1ppsSelect: selected time array used to generate the 1 PPS signal.\n");
}
static void ptpParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, ptpCMDList));

    if (strIcmp(cmd, "setPortPTPEn") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	en   : 1 to enable PTP, 0 to disable PTP.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp setPortPTPEn 1 1\n");
    }
    else if (strIcmp(cmd, "getPortPTPEn") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getPortPTPEn 1\n");
    }
    else if (strIcmp(cmd, "setIntEn") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        printIntEnOption();
		CLI_INFO("Example:\n");
        CLI_INFO("  To update PTP Interrupt configure, using getIntEn to get origin value and update the field you concern\n");
        CLI_INFO("  Note that the not zero origin value should be set back explicitly\n");
        CLI_INFO("        ptp getIntEn 1\n");
		CLI_INFO("        ptp setIntEn 1 -ptpArrIntEn 1 -ptpDepIntEn 0\n");
    }
    else if (strIcmp(cmd, "getIntEn") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getIntEn 1\n");
    }
    else if (strIcmp(cmd, "setPortConfig") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        printPortConfigOption();
		CLI_INFO("Example:\n");
        CLI_INFO("  To update PTP port configure, using getPortConfig to get origin value and update the field you concern\n");
        CLI_INFO("  Note that the not zero origin value should be set back explicitly\n");
        CLI_INFO("        ptp getPortConfig 1\n");
		CLI_INFO("        ptp setPortConfig 1 -transSpec 1 -disTSpec 0 -etJump 0xc -depLedCtrl 0x80\n");
    }
    else if (strIcmp(cmd, "getPortConfig") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getPortConfig 1\n");
    }
    else if (strIcmp(cmd, "getTimeStamp") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	timeToRead : MSD_PTP_TIME enum type.\n");
        CLI_INFO("      0 : PTP Arrival Time 0\n");
        CLI_INFO("      1 : PTP Arrival Time 1\n");
        CLI_INFO("      2 : PTP Departure Time\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getTimeStamp 1 2\n");
    }
    else if (strIcmp(cmd, "getTSValid") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	timeToRead : MSD_PTP_TIME enum type.\n");
        CLI_INFO("      0 : PTP Arrival Time 0\n");
        CLI_INFO("      1 : PTP Arrival Time 1\n");
        CLI_INFO("      2 : PTP Departure Time\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getTSValid 1 2\n");
    }
    else if (strIcmp(cmd, "resetTS") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	timeToRead : MSD_PTP_TIME enum type.\n");
        CLI_INFO("      0 : PTP Arrival Time 0\n");
        CLI_INFO("      1 : PTP Arrival Time 1\n");
        CLI_INFO("      2 : PTP Departure Time\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp resetTS 1 2\n");
    }
    else if (strIcmp(cmd, "setGlobalConfig") == 0)
    {
        printOutGlobalOption();
		CLI_INFO("Example:\n");
        CLI_INFO("  To update PTP Global configure, using getGlobalConfig to get origin value and update the field you concern\n");
        CLI_INFO("  Note that the not zero origin value should be set back explicitly\n");
        CLI_INFO("        ptp getGlobalConfig\n");
		CLI_INFO("        ptp setGlobalConfig -ptpEType 0x88F7 -msgIdTSEn 0xFFFF -tsArrPtr 0\n");
    }
    else if (strIcmp(cmd, "getGlobalConfig") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getGlobalConfig\n");
    }
    else if (strIcmp(cmd, "getIntStatus") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        ptp getIntStatus\n");
    }
    else if (strIcmp(cmd, "setMeanPathDelay") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	delay: cable delay between this port and its link partner in unsiged nano seconds\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp setMeanPathDelay 0 10\n");
    }
    else if (strIcmp(cmd, "getMeanPathDelay") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getMeanPathDelay 0\n");
    }
    else if (strIcmp(cmd, "setIgrPathDelayAsym") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	asym : bit 15 is path delay asymmetry sign(0 added, 1 subtracted), bit 14:0 indicates the asymmetry value\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp setIgrPathDelayAsym 0 0x10\n");
    }
    else if (strIcmp(cmd, "getIgrPathDelayAsym") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getIgrPathDelayAsym 0\n");
    }
    else if (strIcmp(cmd, "setEgrPathDelayAsym") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	asym : bit 15 is path delay asymmetry sign(0 added, 1 subtracted), bit 14:0 indicates the asymmetry value\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp setEgrPathDelayAsym 0 0x10\n");
    }
    else if (strIcmp(cmd, "getEgrPathDelayAsym") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getEgrPathDelayAsym 0\n");
    }
    else if (strIcmp(cmd, "setMasterSlave") == 0)
    {
        CLI_INFO("	role : MSD_PTP_MASTER_SLAVE enum type.\n");
        CLI_INFO("		0 : device as slave\n");
        CLI_INFO("		1 : device as master\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp setMasterSlave 0\n");
    }
    else if (strIcmp(cmd, "getMasterSlave") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getMasterSlave\n");
    }
    else if (strIcmp(cmd, "getTODBusyBit") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getTODBusyBit\n");
    }
    else if (strIcmp(cmd, "getPTPGlobalTime") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getPTPGlobalTime\n");
    }
    else if (strIcmp(cmd, "getPtpMode") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getPtpMode\n");
    }
    else if (strIcmp(cmd, "getPtpPulse") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp getPtpPulse\n");
    }
    else if (strIcmp(cmd, "captureAll") == 0)
    {
        CLI_INFO("	Index : time array index.\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp captureAll 0\n");
    }
    else if (strIcmp(cmd, "setPtpMode") == 0)
    {
        printOutPtpModeOption();
        CLI_INFO("Example:\n");
        CLI_INFO("  To update PTP mode configure, using getPtpMode to get origin value and update the field you concern\n");
        CLI_INFO("  Note that the not zero origin value should be set back explicitly\n");
        CLI_INFO("        ptp getPtpMode\n");
        CLI_INFO("        ptp setPtpMode -altScheme 0 -grandMstr 1 -oneStep 1 -PTPMode 0\n");
    }
    else if (strIcmp(cmd, "storeCompensation") == 0)
    {
        CLI_INFO("	Index : time array index.\n");
        printOutTimeArrayOption();
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp storeCompensation 0 -todLoadPoint 0 -clkValid 1 -domainNumber 0 -todNanoseconds 0x100 -todCompensation 0x200\n");
    }
    else if (strIcmp(cmd, "storeAll") == 0)
    {
        CLI_INFO("	Index : time array index.\n");
        printOutTimeArrayOption();
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp storeAll 0 -todLoadPoint 0 -clkValid 1 -domainNumber 0 -todNanoseconds 0x100 -todCompensation 0x200\n");
    }
    else if (strIcmp(cmd, "setPtpPulse") == 0)
    {
        printOutPtpPulseOption();
        CLI_INFO("Example:\n");
        CLI_INFO("        ptp setPtpPulse -ptpPulseWidth 0xf -ptp1ppsWidthRange 3 -ptp1ppsPhase 1 -ptp1ppsSelect 0\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
int ptpSetIntEn()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_INTERRUPT_EN interruptEn;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&interruptEn, 0, sizeof(MSD_PTP_PORT_INTERRUPT_EN));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateIntEnable(&interruptEn, 3) != 0)
        return -1;

    retVal = msdPtpIntEnableSet(sohoDevNum, port, &interruptEn);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetIntEn()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_INTERRUPT_EN interruptEn;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&interruptEn, 0, sizeof(MSD_PTP_PORT_INTERRUPT_EN));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPtpIntEnableGet(sohoDevNum, port, &interruptEn);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("PTP interrupt info:\n");
    CLI_INFO("  Arrival interrupt enable status: %d\n", interruptEn.ptpArrIntEn);
    CLI_INFO("  Departure interrupt enable status: %d\n", interruptEn.ptpDepIntEn);

    return retVal;
}
int ptpSetPortConfig()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_CONFIG ptpData;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&ptpData, 0, sizeof(MSD_PTP_PORT_CONFIG));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if (updatePortConfig(&ptpData, 3) != 0)
        return -1;

    retVal = msdPtpPortConfigSet(sohoDevNum, port, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPortConfig()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_CONFIG ptpData;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&ptpData, 0, sizeof(MSD_PTP_PORT_CONFIG));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPtpPortConfigGet(sohoDevNum, port, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpPortConfig(&ptpData);
    return retVal;
}
int ptpGetTimeStamp()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_TIME timeToRead;
    MSD_PTP_TS_STATUS ptpStatus;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&ptpStatus, 0, sizeof(MSD_PTP_TS_STATUS));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    timeToRead = (MSD_PTP_TIME)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdPtpTimeStampGet(sohoDevNum, port, timeToRead, &ptpStatus);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutTimeStamp(&ptpStatus);
    return retVal;
}
int ptpSetGlobalConfig()
{
    MSD_STATUS retVal;
    MSD_PTP_GLOBAL_CONFIG  ptpData;

    memset(&ptpData, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));
    if (updatePtpGlobalConfig(&ptpData, 2) != 0)
        return -1;

    retVal = msdPtpGlobalConfigSet(sohoDevNum, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetGlobalConfig()
{
    MSD_STATUS retVal;
    MSD_PTP_GLOBAL_CONFIG  ptpData;

    memset(&ptpData, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));

    retVal = msdPtpGlobalConfigGet(sohoDevNum, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpGlobalConfig(&ptpData);
    return retVal;
}
int ptpGetIntStatus()
{
    MSD_STATUS retVal;
    MSD_PTP_INTERRUPT_STATUS  ptpIntSt;

    memset(&ptpIntSt, 0, sizeof(MSD_PTP_INTERRUPT_STATUS));

    retVal = msdPtpIntStatusGet(sohoDevNum, &ptpIntSt);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpIntStatus(&ptpIntSt);
    return retVal;
}
int ptpSetPtpMode()
{
    MSD_STATUS retVal;
    MSD_PTP_MODE_CONFIG mode;

    memset(&mode, 0, sizeof(MSD_PTP_MODE_CONFIG));
    if (updatePtpMode(&mode, 2) != 0)
        return -1;

    retVal = msdPtpModeSet(sohoDevNum, &mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPtpMode()
{
    MSD_STATUS retVal;
    MSD_PTP_MODE_CONFIG mode;

    memset(&mode, 0, sizeof(MSD_PTP_MODE_CONFIG));
    retVal = msdPtpModeGet(sohoDevNum, &mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpMode(&mode);
    return retVal;
}
int ptpStoreCompensation()
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));
    if (updateTimeArray(&timeArray, 3) != 0)
        return -1;

    retVal = msdPtpTODStoreCompensation(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpStoreAll()
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));
    if (updateTimeArray(&timeArray, 3) != 0)
        return -1;

    retVal = msdPtpTODStoreAll(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpCaptureAll()
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));

    retVal = msdPtpTODCaptureAll(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutTimeArray(timeArrayIndex, &timeArray);
    return retVal;
}
int ptpSetPtpPulse()
{
    MSD_STATUS retVal;
    MSD_PTP_PULSE_STRUCT pulsePara;

    memset(&pulsePara, 0, sizeof(MSD_PTP_PULSE_STRUCT));
    if (updatePtpPulse(&pulsePara, 2) != 0)
        return -1;

    retVal = msdPtpPulseSet(sohoDevNum, &pulsePara);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPtpPulse()
{
    MSD_STATUS retVal;
    MSD_PTP_PULSE_STRUCT pulsePara;

    memset(&pulsePara, 0, sizeof(MSD_PTP_PULSE_STRUCT));

    retVal = msdPtpPulseGet(sohoDevNum, &pulsePara);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPulse(&pulsePara);
    return retVal;
}
int ptpHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        ptpParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (ptpCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", ptpCMDList[index].help);
        index++;
    }

    return 0;
}
int ptpOperation()
{
    int index = 0;
    while (1)
    {
        if (ptpCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], ptpCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(ptpCMDList[index]);
            ptpCMDList[index].callback();
            break;
        }
        index++;
    }
    if (ptpCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"ptp help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static int updateIEEEPriorityTableStruct(MSD_IEEE_PRIORITY_TABLE_STRUCT *ieeePriorityTableStruct, int startIndex)
{
    int index;
    char *cmd, *value;
    for (index = startIndex; index < nargs; index = index + 2)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ieeeYellow") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeYellow = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disIeeeQpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.disIeeeQpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ieeeQpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeQpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disIeeeFpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.disIeeeFpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ieeeFpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeFpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTbldscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTbldscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutIEEEPriorityTableStruct(MSD_IEEE_PRIORITY_TABLE_SET table, MSD_IEEE_PRIORITY_TABLE_STRUCT *ieeePriorityTableStruct)
{
	CLI_INFO("IEEE Priority table struct for ");
	if (table == MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI)
	{
		CLI_INFO("IingTbl: \n");
    CLI_INFO("					IeeeYellow:     %x\n", ieeePriorityTableStruct->ingTbl.ieeeYellow);
    CLI_INFO("					DisIeeeQpri:    %x\n", ieeePriorityTableStruct->ingTbl.disIeeeQpri);
    CLI_INFO("					IeeeQpri:       %x\n", ieeePriorityTableStruct->ingTbl.ieeeQpri);
    CLI_INFO("					DisIeeeFpri:    %x\n", ieeePriorityTableStruct->ingTbl.disIeeeFpri);
    CLI_INFO("					IeeeFpri:       %x\n", ieeePriorityTableStruct->ingTbl.ieeeFpri);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP)
    {
        CLI_INFO("EgGreeTblPcp: \n");
    CLI_INFO("					DisVIDMarking:  %x\n", ieeePriorityTableStruct->egGreeTblPcp.disVIDMarking);
    CLI_INFO("					DisPCPMarking:  %x\n", ieeePriorityTableStruct->egGreeTblPcp.disPCPMarking);
    CLI_INFO("					FramePCP:       %x\n", ieeePriorityTableStruct->egGreeTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP)
    {

        CLI_INFO("EgYelTblPcp: \n");
    CLI_INFO("					DisVIDMarking:  %x\n", ieeePriorityTableStruct->egYelTblPcp.disVIDMarking);
    CLI_INFO("					DisPCPMarking:  %x\n", ieeePriorityTableStruct->egYelTblPcp.disPCPMarking);
    CLI_INFO("					FramePCP:       %x\n", ieeePriorityTableStruct->egYelTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP)
    {
        CLI_INFO("EgAvbTblPcp: \n");
    CLI_INFO("					DisVIDMarking:  %x\n", ieeePriorityTableStruct->egAvbTblPcp.disVIDMarking);
    CLI_INFO("					DisPCPMarking:  %x\n", ieeePriorityTableStruct->egAvbTblPcp.disPCPMarking);
    CLI_INFO("					FramePCP:       %x\n", ieeePriorityTableStruct->egAvbTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP)
    {
        CLI_INFO("EgGreeTblDscp: \n");
    CLI_INFO("					EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egGreeTblDscp.enDSCPMarking);
    CLI_INFO("					FrameDSCP:  %x\n", ieeePriorityTableStruct->egGreeTblDscp.frameDSCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP)
    {
        CLI_INFO("EgYelTblDscp: \n");
    CLI_INFO("					EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egYelTblDscp.enDSCPMarking);
    CLI_INFO("					FrameDSCP:  %x\n", ieeePriorityTableStruct->egYelTblDscp.frameDSCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP)
    {
        CLI_INFO("EgAvbTbldscp: \n");
    CLI_INFO("					EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egAvbTbldscp.enDSCPMarking);
    CLI_INFO("					FrameDSCP:  %x\n", ieeePriorityTableStruct->egAvbTbldscp.frameDSCP);
}
    else
    {
        CLI_INFO("Invalid IEEE table input\n");
    }
}
static void qosParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, qosCMDList));
    if (strIcmp(cmd, "setPriMapRule") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : 1 for user prio rule 0 for otherwise.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setPriMapRule 1 1\n");
    }
    else if (strIcmp(cmd, "getPriMapRule") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getPriMapRule 1\n");
    }
    else if (strIcmp(cmd, "setInitPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("		0 : MSD_INITIAL_PRI_DEFAULT\n");
        CLI_INFO("		1 : MSD_INITIAL_PRI_TAG\n");
        CLI_INFO("		2 : MSD_INITIAL_PRI_IP\n");
        CLI_INFO("		3 : MSD_INITIAL_PRI_TAG_IP\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setInitPri 1 1\n");
    }
    else if (strIcmp(cmd, "getInitPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getInitPri 1\n");
    }
    else if (strIcmp(cmd, "setDefQPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : default queue priority (0~7)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setDefQPri 1 1\n");
    }
    else if (strIcmp(cmd, "getDefQPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getDefQPri 1\n");
    }
    else if (strIcmp(cmd, "setDefFPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : default queue priority (0~7)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setDefFPri 1 1\n");
    }
    else if (strIcmp(cmd, "getDefFPri") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getDefFPri 1\n");
    }
    else if (strIcmp(cmd, "setCosQueue") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	cos  : Trusted or assigned cos for the packet\n");
        CLI_INFO("	queue: Physical queue to be mapped for trusted or assigned cos\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setCosQueue 1 1 1\n");
    }
    else if (strIcmp(cmd, "getCosQueue") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	cos  : Trusted or assigned cos for the packet\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getCosQueue 1 1\n");
    }
    else if (strIcmp(cmd, "setCosDscp") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	cos  : Trusted or assigned cos for the packet\n");
        CLI_INFO("	dscp : frame differentiated services code point\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setCosDscp 1 1 1\n");
    }
    else if (strIcmp(cmd, "getCosDscp") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	cos  : Trusted or assigned cos for the packet\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getCosDscp 1 1\n");
    }
    else if (strIcmp(cmd, "enRewriteDscp") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : enable/disable dscp re-write at egress on the packet\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos enRewriteDscp 1 1\n");
    }
    else if (strIcmp(cmd, "setPrioMap") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	table:\n");
        CLI_INFO("		0 : MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI   \n");
        CLI_INFO("		1 : MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP \n");
        CLI_INFO("		2 : MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP   \n");
        CLI_INFO("		3 : MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP   \n");
        CLI_INFO("		4 : MSD_IEEE_PRIORITY_TABLE_RESERVED   \n");
        CLI_INFO("		5 : MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP\n");
        CLI_INFO("		6 : MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP  \n");
        CLI_INFO("		7 : MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP  \n");
        CLI_INFO("	entryPtr:Pointer to the desired entry of the IEEE Priority Mapping table selected \n");
        CLI_INFO("[option]");
        CLI_INFO("IEEE priority map options, union struct for different table\n");
        CLI_INFO("	Ingress PCP to IEEE_FPRI & IEEE_QPRI mapping:\n");
        CLI_INFO("	-ieeeYellow      : IEEE Yellow\n");
        CLI_INFO("	-disIeeeQpri     : disable IEEE Qpri Mapping\n");
        CLI_INFO("	-ieeeQpri        : IEEE Qpri Mapping\n");
        CLI_INFO("	-disIeeeFpri     : disable IEEE Fpri Mapping\n");
        CLI_INFO("	-ieeeFpri        : IEEE Fpri Mapping\n");
        CLI_INFO("	Egress Green Frame FPRI to PCP mapping (non-AVB):\n");
        CLI_INFO("	-disVIDMarkingGreen : disable VID Marking Green\n");
        CLI_INFO("	-disPCPMarkingGreen : disable PCP Marking Green\n");
        CLI_INFO("	-framePCPGreen   : frame PCP Green\n");
        CLI_INFO("	Egress Yellow Frame FPRI to PCP mapping (non-AVB):\n");
        CLI_INFO("	-disVIDMarkingYel: disable VID Marking Yellow\n");
        CLI_INFO("	-disPCPMarkingYel: disable PCP Marking Yellow\n");
        CLI_INFO("	-framePCPYel     : frame PCP Yellow\n");
        CLI_INFO("	Egress AVB Frame FPRI to PCP mapping:\n");
        CLI_INFO("	-disVIDMarkingAVB: disable VID Marking AVB\n");
        CLI_INFO("	-disPCPMarkingAVB: disable PCP Marking AVB\n");
        CLI_INFO("	-framePCPAVB     : frame PCP AVB\n");
        CLI_INFO("	Egress Green Frame FPRI to DSCP mapping (non-AVB):\n");
        CLI_INFO("	-enDSCPMarkingGreen : enDSCP Marking Green\n");
        CLI_INFO("	-frameDSCPGreen  : frame DSCP Green\n");
        CLI_INFO("	Egress Yellow Frame FPRI to DSCP mapping (non-AVB):\n");
        CLI_INFO("	-enDSCPMarkingYel: enDSCP Marking Yellow\n");
        CLI_INFO("	-frameDSCPYel    : frame DSCP Yellow\n");
        CLI_INFO("	Egress AVB Frame FPRI to DSCP mapping:\n");
        CLI_INFO("	-enDSCPMarkingAVB: enDSCP Marking AVB\n");
        CLI_INFO("	-frameDSCPAVB    : frame DSCP AVB\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos setPrioMap 1 0 1 -ieeeYellow 1 -ieeeQpri 1\n");
		CLI_INFO("        qos setPrioMap 1 1 1 -disVIDMarkingGreen 1 -disPCPMarkingGreen 1\n");
		CLI_INFO("        qos setPrioMap 1 2 1 -disVIDMarkingYel 1 -disPCPMarkingYel 1\n");
		CLI_INFO("        qos setPrioMap 1 3 1 -disVIDMarkingAVB 1 -disPCPMarkingAVB 1\n");
		CLI_INFO("        qos setPrioMap 1 5 1 -enDSCPMarkingGreen 1 -frameDSCPGreen 1\n");
		CLI_INFO("        qos setPrioMap 1 6 1 -enDSCPMarkingYel 1 -frameDSCPYel 1\n");
		CLI_INFO("        qos setPrioMap 1 6 1 -enDSCPMarkingAVB 1 -frameDSCPAVB 1\n");
		
    }
    else if (strIcmp(cmd, "getPrioMap") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	table:\n");
        CLI_INFO("		0 : Ingress PCP to IEEE_FPRI & IEEE_QPRI mapping   \n");
        CLI_INFO("		1 : Egress Green Frame FPRI to PCP mapping (non-AVB) \n");
        CLI_INFO("		2 : Egress Yellow Frame FPRI to PCP mapping (non-AVB) \n");
        CLI_INFO("		3 : Egress AVB Frame FPRI to PCP mapping   \n");
        CLI_INFO("		4 : Egress Green Frame FPRI to DSCP mapping (non-AVB) \n");
        CLI_INFO("		5 : Egress Green Frame FPRI to DSCP mapping (non-AVB)\n");
        CLI_INFO("		6 : Egress Yellow Frame FPRI to DSCP mapping (non-AVB)  \n");
        CLI_INFO("		7 : Egress AVB Frame FPRI to DSCP mapping  \n");
        CLI_INFO("	entryPtr:Pointer to the desired entry of the IEEE Priority Mapping table selected \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qos getPrioMap 1 1 1\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
int qosSetPrioMap()
{
    MSD_LPORT port;
    MSD_IEEE_PRIORITY_TABLE_SET table;
    MSD_U8 entryPtr;
    MSD_STATUS retVal;
    int index = 0;
    MSD_IEEE_PRIORITY_TABLE_STRUCT ieeePriorityTableStruct;

    if (nargs < 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&ieeePriorityTableStruct, 0, sizeof(MSD_IEEE_PRIORITY_TABLE_STRUCT));
    if (updateIEEEPriorityTableStruct(&ieeePriorityTableStruct, 5) != 0)
        return -1;
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    table = (MSD_IEEE_PRIORITY_TABLE_SET)strtoul(CMD_ARGS[3], NULL, 0);
    entryPtr = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);
        
    retVal = msdQosPortPrioMapSet(sohoDevNum, port, table, entryPtr, &ieeePriorityTableStruct);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set Priority Map ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int qosGetPrioMap()
{
    MSD_LPORT port;
    MSD_IEEE_PRIORITY_TABLE_SET table;
    MSD_U8 entryPtr;
    MSD_STATUS retVal;
    int index = 0;
    MSD_IEEE_PRIORITY_TABLE_STRUCT ieeePriorityTableStruct;

    if (nargs != 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&ieeePriorityTableStruct, 0, sizeof(ieeePriorityTableStruct));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    table = (MSD_IEEE_PRIORITY_TABLE_SET)strtoul(CMD_ARGS[3], NULL, 0);
    entryPtr = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdQosPortPrioMapGet(sohoDevNum, port, table, entryPtr, &ieeePriorityTableStruct);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get Priority map ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutIEEEPriorityTableStruct(table, &ieeePriorityTableStruct);

    return retVal;
}
int qosHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        qosParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (qosCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", qosCMDList[index].help);
        index++;
    }

    return 0;
}
int qosOperation()
{
    int index = 0;
    while (1)
    {
        if (qosCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], qosCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(qosCMDList[index]);
            qosCMDList[index].callback();
            break;
        }
        index++;
    }
    if (qosCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"qos help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static void sysCtrlParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, sysCtrlCMDList));
    if (strIcmp(cmd, "setFloodBC") == 0)
    {
        CLI_INFO("	en : 1 to enable Flood Broadcast, 0 otherwise.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setFloodBC 1\n");
    }
    else if (strIcmp(cmd, "getFloodBC") == 0)
    {
		
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getFloodBC\n");
    }
    else if (strIcmp(cmd, "setCPUDest") == 0)
    {
        CLI_INFO("	port : the logical port number.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setCPUDest 1\n");
    }
    else if (strIcmp(cmd, "getCPUDest") == 0)
    {
		
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getCPUDest\n");
    }
    else if (strIcmp(cmd, "setIMD") == 0)
    {
        CLI_INFO("	port : the logical port number.when port = 0x1F,IMD is disabled.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setIMD 1\n");
    }
    else if (strIcmp(cmd, "getIMD") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getIMD\n");
    }
    else if (strIcmp(cmd, "setEMD") == 0)
    {
        CLI_INFO("	port :the logical port number.If port = 0x1F, EMD is disabled\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setEMD 1\n");
    }
    else if (strIcmp(cmd, "getEMD") == 0)
    {
		
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getEMD\n");
    }
    else if (strIcmp(cmd, "setMMC") == 0)
    {
        CLI_INFO("	point : Pointer to the Monitor and mgmt control register\n");
        CLI_INFO("			0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x\n");
        CLI_INFO("			0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x\n");
        CLI_INFO("			0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x\n");
        CLI_INFO("			0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x\n");
        CLI_INFO("			0x20 to 0x2F: Mirror Destination Port Settings\n");
        CLI_INFO("			0x30 to 0x3F: Trap Destination Port Settings\n");
        CLI_INFO("	data : Monitor and mgmt Control data\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setMMC 1 1\n");
    }
    else if (strIcmp(cmd, "getMMC") == 0)
    {
        CLI_INFO("	point : Pointer to the Monitor and mgmt control register\n");
        CLI_INFO("			0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x\n");
        CLI_INFO("			0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x\n");
        CLI_INFO("			0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x\n");
        CLI_INFO("			0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x\n");
        CLI_INFO("			0x20 to 0x2F: Mirror Destination Port Settings\n");
        CLI_INFO("			0x30 to 0x3F: Trap Destination Port Settings\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getMMC 1\n");
    }
    else if (strIcmp(cmd, "setRsvd2CpuEn") == 0)
    {
        CLI_INFO("	enBits : bit vector of enabled Reserved Multicast.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setRsvd2CpuEn 1\n");
    }
    else if (strIcmp(cmd, "getRsvd2CpuEn") == 0)
    {
	
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getRsvd2CpuEn\n");
    }
    else if (strIcmp(cmd, "setMGMTPri") == 0)
    {
        CLI_INFO("	pri : PRI[2:0] bits\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        sys setMGMTPri 2\n");
	}
    else if (strIcmp(cmd, "getMGMTPri") == 0)
	{
		
		CLI_INFO("Example:\n");
		CLI_INFO("        sys getMGMTPri\n");
	}
    else if (strIcmp(cmd, "dumpGlobal1") == 0)
    {

        CLI_INFO("Example:\n");
        CLI_INFO("        sys dumpGlobal1\n");
    }
    else if (strIcmp(cmd, "dumpGlobal2") == 0)
    {

        CLI_INFO("Example:\n");
        CLI_INFO("        sys dumpGlobal2\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int sysCtrlHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
        sysCtrlParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
        if (sysCtrlCMDList[index].callback == NULL)
			break;
        CLI_INFO("%s", sysCtrlCMDList[index].help);
		index++;
	}

	return 0;
}
int sysCtrlOperation()
{
	int index = 0;
	while (1)
	{
        if (sysCtrlCMDList[index].callback == NULL)
			break;
        if (strIcmp(CMD_ARGS[1], sysCtrlCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(sysCtrlCMDList[index]);
            sysCtrlCMDList[index].callback();
			break;
		}
		index++;
	}
    if (sysCtrlCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"sys help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static int updateTcamData(MSD_TCAM_DATA *entry, int startIndex)
{
	int index;
	char* cmd, *value;
	for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameType") == 0)
		{
			entry->frameType = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-frameTypeMask") == 0)
		{
			entry->frameTypeMask = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-spv") == 0)
		{
			entry->spv = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-spvMask") == 0)
		{
			entry->spvMask = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ppri") == 0)
		{
			entry->ppri = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ppriMask") == 0)
		{
			entry->ppriMask = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-pvid") == 0)
		{
			entry->pvid = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-pvidMask") == 0)
		{
			entry->pvidMask = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-continu") == 0)
		{
			entry->continu = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-interrupt") == 0)
		{
			entry->interrupt = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-IncTcamCtr") == 0)
		{
			entry->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vidOverride") == 0)
		{
			entry->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vidData") == 0)
		{
			entry->vidData = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-nextId") == 0)
		{
			entry->nextId = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-qpriOverride") == 0)
		{
			entry->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-qpriData") == 0)
		{
			entry->qpriData = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-fpriOverride") == 0)
		{
			entry->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-fpriData") == 0)
		{
			entry->fpriData = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-dpvSF") == 0)
		{
			entry->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-dpvData") == 0)
		{
			entry->dpvData = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-dpvMode") == 0)
		{
			entry->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-colorMode") == 0)
		{
			entry->colorMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vtuPageOverride") == 0)
		{
			entry->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-vtuPage") == 0)
		{
			entry->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-unKnownFilter") == 0)
		{
			entry->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egActPoint") == 0)
		{
			entry->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
		{
			entry->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ldBalanceData") == 0)
		{
			entry->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-DSCPOverride") == 0)
		{
			entry->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-DSCP") == 0)
		{
			entry->DSCP = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-factionOverride") == 0)
		{
			entry->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-factionData") == 0)
		{
			entry->factionData = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-frameOctet") == 0)
		{
            int i;
            unsigned int tempList[48] = { 0 };
            if (updateArray(&index, value, 48, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 48; i++)
            {
                entry->frameOctet[i] = tempList[i];
            }
		}
		else if (strIcmp(cmd, "-frameOctetMask") == 0)
		{
            int i;
            unsigned int tempList[48] = { 0 };
            if (updateArray(&index, value, 48, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 48; i++)
            {
                entry->frameOctetMask[i] = tempList[i];
            }
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
static int updateEgrTcamData(MSD_TCAM_EGR_DATA *entry, int startIndex)
{
	int index;
	char* cmd, *value;
	for (index = startIndex; index < nargs; )
	{
		cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        
        if (strIcmp(cmd, "-frameModeOverride") == 0)
		{
			entry->frameModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-frameMode") == 0)
		{
			entry->frameMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-tagModeOverride") == 0)
		{
			entry->tagModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-tagMode") == 0)
		{
			entry->tagMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-daMode") == 0)
		{
			entry->daMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-saMode") == 0)
		{
			entry->saMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egVidModeOverride") == 0)
		{
			entry->egVidModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egVidMode") == 0)
		{
			entry->egVidMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egVidData") == 0)
		{
			entry->egVidData = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egDSCPMode") == 0)
		{
			entry->egDSCPMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egDSCP") == 0)
		{
			entry->egDSCP = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egfpriModeOverride") == 0)
		{
			entry->egfpriModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egfpriMode") == 0)
		{
			entry->egfpriMode = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egEC") == 0)
		{
			entry->egEC = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-egFPRI") == 0)
		{
			entry->egFPRI = (MSD_U8)strtoul(value, NULL, 0);
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
	}

    return 0;
}
/*
static int updateKeyAndMask(MSD_TCAM_ENT_KEY *keyMaskPtr, MSD_TCAM_ENT_KEY *keyPtr, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameType") == 0)
        {
            keyPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameTypeMask") == 0)
        {
            keyMaskPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spv") == 0)
        {
            keyPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spvMask") == 0)
        {
            keyMaskPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppri") == 0)
        {
            keyPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppriMask") == 0)
        {
            keyMaskPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvid") == 0)
        {
            keyPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvidMask") == 0)
        {
            keyMaskPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int updateAction(MSD_TCAM_ENT_ACT *action, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        
        if (strIcmp(cmd, "-interrupt") == 0)
        {
            action->interrupt = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IncTcamCtr") == 0)
        {
            action->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidOverride") == 0)
        {
            action->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidData") == 0)
        {
            action->vidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriOverride") == 0)
        {
            action->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriData") == 0)
        {
            action->qpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriOverride") == 0)
        {
            action->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriData") == 0)
        {
            action->fpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvSF") == 0)
        {
            action->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvData") == 0)
        {
            action->dpvData = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvMode") == 0)
        {
            action->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorMode") == 0)
        {
            action->colorMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPageOverride") == 0)
        {
            action->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPage") == 0)
        {
            action->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-unKnownFilter") == 0)
        {
            action->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egActPoint") == 0)
        {
            action->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
        {
            action->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceData") == 0)
        {
            action->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCPOverride") == 0)
        {
            action->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCP") == 0)
        {
            action->DSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionOverride") == 0)
        {
            action->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionData") == 0)
        {
            action->factionData = (MSD_U16)strtoul(value, NULL, 0);
        }        
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
*/
static void v4ipStr2ipAddr(char* str, MSD_U8* addr)
{
    char* token = NULL;
    token = strtok(str, ".");
    addr[0] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
    addr[1] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
    addr[2] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
    addr[3] = (MSD_U8)strtoul(token, NULL, 0);
}
static void v4ipStr2ipAddrMask(char* str, MSD_U8* addr)
{
    char* token = NULL;
    token = strtok(str, ".");
    addr[0] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
    addr[1] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
    addr[2] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
    addr[3] = (MSD_U8)strtoul(token, NULL, 16);
}
static int updateIPV4Header(MSD_TCAM_ENT_IPV4_UNT *ipv4, MSD_TCAM_ENT_IPV4_UNT *ipv4Mask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-version") == 0)
    {
        ipv4->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ihl") == 0)
    {
        ipv4->ihl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tos") == 0)
    {
        ipv4->tos = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ipLength") == 0)
    {
        ipv4->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-id") == 0)
    {
        ipv4->id = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flag") == 0)
    {
        ipv4->flag = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-fragOffset") == 0)
    {
        ipv4->fragOffset = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ttl") == 0)
    {
        ipv4->ttl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-protocol") == 0)
    {
        ipv4->protocol = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hdrChkSum") == 0)
    {
        ipv4->hdrChkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV4Ip") == 0)
    {
        v4ipStr2ipAddr(value, ipv4->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV4Ip") == 0)
    {
        v4ipStr2ipAddr(value, ipv4->dip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-versionMask") == 0)
    {
        ipv4Mask->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ihlMask") == 0)
    {
        ipv4Mask->ihl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tosMask") == 0)
    {
        ipv4Mask->tos = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ipLengthMask") == 0)
    {
        ipv4Mask->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-idMask") == 0)
    {
        ipv4Mask->id = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flagMask") == 0)
    {
        ipv4Mask->flag = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-fragOffsetMask") == 0)
    {
        ipv4Mask->fragOffset = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ttlMask") == 0)
    {
        ipv4Mask->ttl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-protocolMask") == 0)
    {
        ipv4Mask->protocol = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hdrChkSumMask") == 0)
    {
        ipv4Mask->hdrChkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV4IpMask") == 0)
    {
        v4ipStr2ipAddrMask(value, ipv4Mask->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV4IpMask") == 0)
    {
        v4ipStr2ipAddrMask(value, ipv4Mask->dip);
        ret = 1;
    }

    return ret;
}
static void v6ipStr2ipAddr(char* str, MSD_U16* addr)
{
    char* token = NULL;
    token = strtok(str, ":");
    addr[0] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[1] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[2] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[3] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[4] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[5] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[6] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
    addr[7] = (MSD_U16)strtoul(token, NULL, 16);
}
static int updateIPV6Header(MSD_TCAM_ENT_IPV6_UNT *ipv6, MSD_TCAM_ENT_IPV6_UNT *ipv6Mask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-version") == 0)
    {
        ipv6->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tc") == 0)
    {
        ipv6->tc = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flowLbl") == 0)
    {
        ipv6->flowLbl = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-payloadLen") == 0)
    {
        ipv6->payloadLen = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-nextHdr") == 0)
    {
        ipv6->nextHdr = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hopLimit") == 0)
    {
        ipv6->hopLimit = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV6Ip") == 0)
    {
        v6ipStr2ipAddr(value, ipv6->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV6Ip") == 0)
    {
        v6ipStr2ipAddr(value, ipv6->dip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-versionMask") == 0)
    {
        ipv6Mask->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcMask") == 0)
    {
        ipv6Mask->tc = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flowLblMask") == 0)
    {
        ipv6Mask->flowLbl = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-payloadLenMask") == 0)
    {
        ipv6Mask->payloadLen = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-nextHdrMask") == 0)
    {
        ipv6Mask->nextHdr = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hopLimitMask") == 0)
    {
        ipv6Mask->hopLimit = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV6IpMask") == 0)
    {
        v6ipStr2ipAddr(value, ipv6Mask->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV6IpMask") == 0)
    {
        v6ipStr2ipAddr(value, ipv6Mask->dip);
        ret = 1;
    }

    return ret;
}
static int updateTCPHeader(MSD_TCAM_ENT_TCP_UNT *tcp, MSD_TCAM_ENT_TCP_UNT *tcpMask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-srcPort") == 0)
    {
        tcp->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPort") == 0)
    {
        tcp->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-seqNum") == 0)
    {
        tcp->seqNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ackNum") == 0)
    {
        tcp->ackNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpOffset") == 0)
    {
        tcp->offset = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpFlags") == 0)
    {
        tcp->flags = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-windowSize") == 0)
    {
        tcp->windowSize = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSum") == 0)
    {
        tcp->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-urgPtr") == 0)
    {
        tcp->urgPtr = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-srcPortMask") == 0)
    {
        tcpMask->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPortMask") == 0)
    {
        tcpMask->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-seqNumMask") == 0)
    {
        tcpMask->seqNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ackNumMask") == 0)
    {
        tcpMask->ackNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpOffsetMask") == 0)
    {
        tcpMask->offset = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpFlagsMask") == 0)
    {
        tcpMask->flags = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-windowSizeMask") == 0)
    {
        tcpMask->windowSize = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSumMask") == 0)
    {
        tcpMask->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-urgPtrMask") == 0)
    {
        tcpMask->urgPtr = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }

    return ret;
}
static int updateUDPHeader(MSD_TCAM_ENT_UDP_UNT *udp, MSD_TCAM_ENT_UDP_UNT *udpMask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-srcPort") == 0)
    {
        udp->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPort") == 0)
    {
        udp->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-udpLength") == 0)
    {
        udp->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSum") == 0)
    {
        udp->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-srcPortMask") == 0)
    {
        udpMask->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPortMask") == 0)
    {
        udpMask->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-udpLengthMask") == 0)
    {
        udpMask->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSumMask") == 0)
    {
        udpMask->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }

    return ret;
}
static int updateAdvConfigParameter(MSD_TCAM_ENT_KEY *keyMaskPtr, MSD_TCAM_ENT_KEY *keyPtr, MSD_TCAM_ENT_ACT *action, MSD_TCAM_PKT_TYPE pktType, MSD_TCAM_ENT_PKT_UNT *patternPtr, MSD_TCAM_ENT_PKT_UNT *maskPtr, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameType") == 0)
        {
            keyPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameTypeMask") == 0)
        {
            keyMaskPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spv") == 0)
        {
            keyPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spvMask") == 0)
        {
            keyMaskPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppri") == 0)
        {
            keyPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppriMask") == 0)
        {
            keyMaskPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvid") == 0)
        {
            keyPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvidMask") == 0)
        {
            keyMaskPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-interrupt") == 0)
        {
            action->interrupt = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IncTcamCtr") == 0)
        {
            action->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidOverride") == 0)
        {
            action->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidData") == 0)
        {
            action->vidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriOverride") == 0)
        {
            action->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriData") == 0)
        {
            action->qpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriOverride") == 0)
        {
            action->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriData") == 0)
        {
            action->fpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvSF") == 0)
        {
            action->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvData") == 0)
        {
            action->dpvData = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvMode") == 0)
        {
            action->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorMode") == 0)
        {
            action->colorMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPageOverride") == 0)
        {
            action->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPage") == 0)
        {
            action->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-unKnownFilter") == 0)
        {
            action->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egActPoint") == 0)
        {
            action->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
        {
            action->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceData") == 0)
        {
            action->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCPOverride") == 0)
        {
            action->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCP") == 0)
        {
            action->DSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionOverride") == 0)
        {
            action->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionData") == 0)
        {
            action->factionData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-da") == 0)
        {
            macStr2macArr(value, patternPtr->da.arEther);
        }
        else if (strIcmp(cmd, "-sa") == 0)
        {
            macStr2macArr(value, patternPtr->sa.arEther);
        }
        else if (strIcmp(cmd, "-vlanTag") == 0)
        {
            patternPtr->vlanTag = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etherType") == 0)
        {
            patternPtr->etherType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-daMask") == 0)
        {
            macStr2macArr(value, maskPtr->da.arEther);
        }
        else if (strIcmp(cmd, "-saMask") == 0)
        {
            macStr2macArr(value, maskPtr->sa.arEther);
        }
        else if (strIcmp(cmd, "-vlanTagMask") == 0)
        {
            maskPtr->vlanTag = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etherTypeMask") == 0)
        {
            maskPtr->etherType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else
        {
            int ret = 0;
            switch (pktType)
            {
            case MSD_TCAM_TYPE_IPV4_TCP:
                ret |= updateIPV4Header(&(patternPtr->pktUnit.ipv4Tcp.ipv4), &(maskPtr->pktUnit.ipv4Tcp.ipv4), cmd, value);
                ret |= updateTCPHeader(&(patternPtr->pktUnit.ipv4Tcp.tcp), &(maskPtr->pktUnit.ipv4Tcp.tcp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV4_UDP:
                ret |= updateIPV4Header(&(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4), cmd, value);
                ret |= updateUDPHeader(&(patternPtr->pktUnit.ipv4Udp.udp), &(maskPtr->pktUnit.ipv4Udp.udp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV6_TCP:
                ret |= updateIPV6Header(&(patternPtr->pktUnit.ipv6Tcp.ipv6), &(maskPtr->pktUnit.ipv6Tcp.ipv6), cmd, value);
                ret |= updateTCPHeader(&(patternPtr->pktUnit.ipv6Tcp.tcp), &(maskPtr->pktUnit.ipv6Tcp.tcp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV6_UDP:
                ret |= updateIPV6Header(&(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6), cmd, value);
                ret |= updateUDPHeader(&(patternPtr->pktUnit.ipv6Udp.udp), &(maskPtr->pktUnit.ipv6Udp.udp), cmd, value);
                break;
            default:
                break;
            }
            if (ret == 0)
            {
                CLI_ERROR("Invalid option input: %s\n", cmd);
                return -1;
            }
        }
    }
    return 0;
}
static void displayTcamData(MSD_TCAM_DATA *tcamData)
{
	int i;

	CLI_INFO("----- Key & Mask -----\n");

	CLI_INFO("frameType         : %04x\n", tcamData->frameType);
	CLI_INFO("frameTypeMask     : %04x\n", tcamData->frameTypeMask);
	CLI_INFO("spv               : %04x\n", tcamData->spv);
	CLI_INFO("spvMask           : %04x\n", tcamData->spvMask);
	CLI_INFO("ppri              : %04x\n", tcamData->ppri);
	CLI_INFO("ppriMask          : %04x\n", tcamData->ppriMask);
	CLI_INFO("pvid              : %04x\n", tcamData->pvid);
	CLI_INFO("pvidMask          : %04x\n", tcamData->pvidMask);

	CLI_INFO("----- Ethernet Frame Content -----\n");

	for (i = 0; i<48; i++)
	{
		CLI_INFO("frame data Octet %2d         : %4x\n", i, tcamData->frameOctet[i]);
		CLI_INFO("frame data OctetMask %2d     : %4x\n", i, tcamData->frameOctetMask[i]);
	}

	CLI_INFO("----- Ingress Action -----\n");

	CLI_INFO("continu           : %04x\n", tcamData->continu);
	CLI_INFO("interrupt         : %04x\n", tcamData->interrupt);
	CLI_INFO("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr);
	CLI_INFO("vidOverride       : %04x\n", tcamData->vidOverride);
	CLI_INFO("vidData           : %04x\n", tcamData->vidData);

	CLI_INFO("nextId            : %04x\n", tcamData->nextId);
	CLI_INFO("qpriOverride      : %04x\n", tcamData->qpriOverride);
	CLI_INFO("qpriData          : %04x\n", tcamData->qpriData);
	CLI_INFO("fpriOverride      : %04x\n", tcamData->fpriOverride);
	CLI_INFO("fpriData          : %04x\n", tcamData->fpriData);

	CLI_INFO("dpvSF             : %04x\n", tcamData->dpvSF);
	CLI_INFO("dpvData           : %04x\n", (unsigned int)tcamData->dpvData);
	CLI_INFO("dpvMode           : %04x\n", tcamData->dpvMode);

	CLI_INFO("unKnownFilter     : %04x\n", tcamData->unKnownFilter);

	CLI_INFO("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride);
	CLI_INFO("ldBalanceData     : %04x\n", tcamData->ldBalanceData);

	CLI_INFO("factionOverride   : %04x\n", tcamData->factionOverride);
	CLI_INFO("factionData       : %04x\n", tcamData->factionData);
#ifdef PERIDOT_BUILD_IN
	CLI_INFO("colorMode         : %04x\n", tcamData->colorMode);
	CLI_INFO("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride);
	CLI_INFO("vtuPage           : %04x\n", tcamData->vtuPage);
	CLI_INFO("egActPoint        : %04x\n", tcamData->egActPoint);
	CLI_INFO("DSCPOverride      : %04x\n", tcamData->DSCPOverride);
	CLI_INFO("DSCP              : %04x\n", tcamData->DSCP);
#endif
}
static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData)
{
	CLI_INFO("----- MSD_TCAM_EGR_DATA -----\n");

	CLI_INFO("frameModeOverride : %04x\n", tcamEgrData->frameModeOverride);
	CLI_INFO("frameMode         : %04x\n", tcamEgrData->frameMode);
	CLI_INFO("tagModeOverride   : %04x\n", tcamEgrData->tagModeOverride);
	CLI_INFO("tagMode           : %04x\n", tcamEgrData->tagMode);
	CLI_INFO("daMode            : %04x\n", tcamEgrData->daMode);
	CLI_INFO("saMode            : %04x\n", tcamEgrData->saMode);

	CLI_INFO("egVidModeOverride : %04x\n", tcamEgrData->egVidModeOverride);
	CLI_INFO("egVidMode         : %04x\n", tcamEgrData->egVidMode);
	CLI_INFO("egVidData         : %04x\n", tcamEgrData->egVidData);

	CLI_INFO("egDSCPMode        : %04x\n", tcamEgrData->egDSCPMode);
	CLI_INFO("egDSCP            : %04x\n", tcamEgrData->egDSCP);
	CLI_INFO("egfpriModeOverride: %04x\n", tcamEgrData->egfpriModeOverride);
	CLI_INFO("egfpriMode        : %04x\n", tcamEgrData->egfpriMode);
	CLI_INFO("egEC              : %04x\n", tcamEgrData->egEC);
	CLI_INFO("egFPRI            : %04x\n", tcamEgrData->egFPRI);
}
int tcamFlushAll()
{
	MSD_STATUS retVal;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    retVal = msdTcamAllDelete(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush tcam all entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int tcamFlushEntry()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryDelete(sohoDevNum, tcamPointer);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int tcamAddEntry()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;

	if (nargs < 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateTcamData(&tcamData, 3) != 0)
        return -1;

    retVal = msdTcamEntryAdd(sohoDevNum, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int tcamReadEntry()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryRead(sohoDevNum, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	displayTcamData(&tcamData);
	return 0;
}
int tcamGetNextEntry()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryGetNext(sohoDevNum, &tcamPointer, &tcamData);

	if (retVal == MSD_NO_SUCH)
	{
		CLI_INFO("Cannot find any valid tcam entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[2], NULL, 0));
		return retVal;
	}

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get next single entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	CLI_INFO("Get back next entry: %d\n", (int)tcamPointer);
	displayTcamData(&tcamData);
	return 0;
}
int tcamFindEntry()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;
	MSD_BOOL	found;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryFind(sohoDevNum, tcamPointer, &tcamData, &found);

	if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error find single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
    if (found == MSD_FALSE)
    {
        CLI_INFO("Can not find the tcam entry entryNum=%d\n", (int)tcamPointer);
    } 
    else
    {
        CLI_INFO("Find the entry entryNum=%d and detail info:\n", (int)tcamPointer);
        displayTcamData(&tcamData);
    }

	return 0;
}
int tcamSetMode()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_TCAM_MODE mode;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    mode = (MSD_TCAM_MODE)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdPortTcamModeSet(sohoDevNum, port, mode);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set tcam mode port=%d mode=%d ret[%d: %s]"
            "\n", (int)port, mode, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamGetMode()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_TCAM_MODE mode;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPortTcamModeGet(sohoDevNum, port, &mode);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get tcam mode port=%d  ret[%d: %s]"
            "\n", (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    switch (mode)
    {
    case MSD_TCAM_MODE_DISABLE:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_DISABLE\n", port);
        break;
    case MSD_TCAM_MODE_ENABLE_48:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_ENABLE_48\n", port);
        break;
    case MSD_TCAM_MODE_ENABLE_96:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_ENABLE_96\n", port);
        break;
    default:
        CLI_INFO("Unknown Tcam mode for Port %d \n");
        break;
    }

    return retVal;
}
int tcamAddPort()
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_LPORT port;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdTcamEntryPortAdd(sohoDevNum, tcamPointer, port);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add tcam port entryNum=%d port=%d  ret[%d: %s]"
            "\n", (int)tcamPointer, (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int tcamRemovePort()
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_LPORT port;

    if (nargs != 4)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdTcamEntryPortRemove(sohoDevNum, tcamPointer, port);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add tcam port entryNum=%d port=%d  ret[%d: %s]"
            "\n", (int)tcamPointer, (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int tcamEgrDelAllPorts()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdEgrTcamEntryAllPortsDelete(sohoDevNum, tcamPointer);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush egr entry for all ports entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int tcamEgrDelEntry()
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_U32 tcamPointer;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryPerPortDelete(sohoDevNum, port, tcamPointer);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush egr entry per port port=%d entryNum=%d ret[%d: %s]"
			"\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int tcamEgrAddEntry()
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_U32 tcamPointer;
	MSD_TCAM_EGR_DATA tcamData;

	if (nargs < 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    if (updateEgrTcamData(&tcamData, 4) != 0)
        return -1;

    retVal = msdEgrTcamEntryAdd(sohoDevNum, port, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error load single egress entry port=%d entryNum=%d ret[%d: %s]"
			"\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int tcamEgrReadEntry()
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_U32 tcamPointer;
	MSD_TCAM_EGR_DATA tcamData;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryRead(sohoDevNum, port, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error read single egress entry port=%d entryNum=%d ret[%d: %s]"
			"\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	displayTcamEgrData(&tcamData);
	return 0;
}
int tcamEgrGetNextEntry()
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_U32 tcamPointer;
	MSD_TCAM_EGR_DATA tcamData;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

	memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryGetNext(sohoDevNum, port, &tcamPointer, &tcamData);

	if (retVal == MSD_NO_SUCH)
	{
		CLI_INFO("Cannot find any valid tcam egress entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[3], NULL, 0));
		return retVal;
	}

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get next egress single entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	CLI_INFO("Get back next entry: %d\n", (int)tcamPointer);
	displayTcamEgrData(&tcamData);
	return 0;
}
int tcamAdvConfig()
{
    MSD_STATUS retVal;
    MSD_TCAM_PKT_TYPE pktType;
    MSD_U32 tcamPointer;
    MSD_U32 tcamPointer2;
    MSD_BOOL entry2Used;

    MSD_TCAM_ENT_KEY keyMaskPtr;
    MSD_TCAM_ENT_KEY keyPtr;
    MSD_TCAM_ENT_PKT_UNT maskPtr;
    MSD_TCAM_ENT_PKT_UNT patternPtr;
    MSD_TCAM_ENT_ACT actionPtr;


    if (nargs <= 5)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    memset(&keyMaskPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    memset(&keyPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    memset(&maskPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    memset(&patternPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    memset(&actionPtr, 0, sizeof(MSD_TCAM_ENT_ACT));
    pktType = (MSD_TCAM_PKT_TYPE)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    tcamPointer2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    if (updateAdvConfigParameter(&keyMaskPtr, &keyPtr, &actionPtr, pktType, &patternPtr, &maskPtr, 5) != 0)
        return -1;


    retVal = msdTcamAdvConfig(sohoDevNum, pktType, tcamPointer, tcamPointer2, &keyMaskPtr, &keyPtr, &maskPtr, &patternPtr, &actionPtr, &entry2Used);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (entry2Used == MSD_TRUE)
    {
        CLI_INFO("Tcam advance configuration success, and the second entry(entryNum=%d) has been used\n", tcamPointer2);
    }
    else
    {
        CLI_INFO("Tcam advance configuration success, and the second entry(entryNum=%d) not been used\n", tcamPointer2);
    }

    return 0;
}
static void printTcamOptions()
{
	CLI_INFO("	[options](default value for option item is 0):\n");
	CLI_INFO("	options for tcam entry, format as: -continu 0 -dpvMode 1\n");
	CLI_INFO("	-frameType        : frame type\n");
	CLI_INFO("	-frameTypeMask    : mask for frame type\n");
	CLI_INFO("	-spv              : source port vector\n");
	CLI_INFO("	-spvMask          : mask for source port vector\n");
	CLI_INFO("	-ppri             : provider PRI\n");
	CLI_INFO("	-ppriMask         : mask for provider PRI\n");
	CLI_INFO("	-pvid             : provider VID\n");
	CLI_INFO("	-pvidMask         : mask for provider VID\n");
	CLI_INFO("	-frameOctet       : array of Ethernet Frame Content,format as -frameOctet {0x11 0x22}\n");
	CLI_INFO("	-frameOctetMask   : array of Ethernet Frame Content,format as -frameOctetMask {0xff 0xff}\n");
	CLI_INFO("	-continu          : Continue this TCAM entry\n");
	CLI_INFO("	-interrupt        : Interrupt on a TCAM hit\n");
	CLI_INFO("	-IncTcamCtr       : Increment the port's TCAM Counter\n");
	CLI_INFO("	-vidOverride      : VID Override Enable\n");
	CLI_INFO("	-vidData          : VID Override Data\n");
	CLI_INFO("	-nextId           : Next Index\n");
	CLI_INFO("	-qpriOverride     : QPRI Override Enable\n");
	CLI_INFO("	-qpriData         : QPRI Override Data\n");
	CLI_INFO("	-fpriOverride     : FPRI Override Enable\n");
	CLI_INFO("	-fpriData         : FPRI Override Data\n");
	CLI_INFO("	-dpvSF            : DPV Source Port Filter\n");
	CLI_INFO("	-dpvData          : DPV Data\n");

	CLI_INFO("	-dpvMode          : DPV Mode\n");
	CLI_INFO("	-colorMode        : Color Mode\n");
	CLI_INFO("	-vtuPageOverride  : VTU Page Override\n");
	CLI_INFO("	-vtuPage          : VTU Page Data\n");
	CLI_INFO("	-unKnownFilter    : UnKnown Filter\n");
	CLI_INFO("	-egActPoint       : Egress Action Pointer\n");

	CLI_INFO("	-ldBalanceOverride: Load Balance Override Enable\n");
	CLI_INFO("	-ldBalanceData    : Load Balance Override Data\n");
	CLI_INFO("	-DSCPOverride     : DSCP Override Enable\n");
	CLI_INFO("	-DSCP             : DSCP Override Data\n");

	CLI_INFO("	-factionOverride  : Frame Action Override Enable\n");
	CLI_INFO("	-factionData      : Frame Action Override Data\n");
}
static void printTcamEgrOptions()
{
	CLI_INFO("	[options](default value for option item is 0):\n");
	CLI_INFO("	options for tcam entry, format as: -frameModeOverride 0 -frameMode 1\n");
	CLI_INFO("	-frameModeOverride : Egress Frame Mode Override Enable\n");
	CLI_INFO("	-frameMode         : Egress Frame Mode Data\n");
	CLI_INFO("	-tagModeOverride   : Egress tag Mode Override Enable\n");
	CLI_INFO("	-tagMode           : Egress tag Mode Data\n");
	CLI_INFO("	-daMode            : da Mode\n");
	CLI_INFO("	-saMode            : sa Mode\n");

	CLI_INFO("	-egVidModeOverride : Egress Vid Mode Override Enable\n");
	CLI_INFO("	-egVidMode         : Egress Vid Mode \n");
	CLI_INFO("	-egVidData         : Egress Vid Mode Data\n");

	CLI_INFO("	-egDSCPMode        : Egress DSCP Mode \n");
	CLI_INFO("	-egDSCP            : Egress DSCP \n");
	CLI_INFO("	-egfpriModeOverride: Egress FPRI Mode Override Enable\n");
	CLI_INFO("	-egfpriMode        : Egress FPRI Mode \n");
	CLI_INFO("	-egEC              : Egress EC \n");
	CLI_INFO("	-egFPRI            : Egress FPRI  \n");
}
static void printOutAdvConfigOptions()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for tcam key/mask, format as: -frameType 0 -frameTypeMask 0x3\n");
    CLI_INFO("	-frameType        : frame type\n");
    CLI_INFO("	-frameTypeMask    : mask for frame type\n");
    CLI_INFO("	-spv              : source port vector\n");
    CLI_INFO("	-spvMask          : mask for source port vector\n");
    CLI_INFO("	-ppri             : provider PRI\n");
    CLI_INFO("	-ppriMask         : mask for provider PRI\n");
    CLI_INFO("	-pvid             : provider VID\n");
    CLI_INFO("	-pvidMask         : mask for provider VID\n");
    CLI_INFO("	options for standard layer2 header data/mask, format as: -da aabbccddee00 -daMask ffffffffffff\n");
    CLI_INFO("	-da             : destination mac address, format as aabbccddee00\n");
    CLI_INFO("	-sa             : source  mac address, format as aabbccddee00\n");
    CLI_INFO("	-vlanTag        : vlan tag\n");
    CLI_INFO("	-etherType      : ether type\n");
    CLI_INFO("	-daMask             : Mask for destination mac address, format as aabbccddee00\n");
    CLI_INFO("	-saMask             : Mask for source  mac address, format as aabbccddee00\n");
    CLI_INFO("	-vlanTagMask        : Mask for vlan tag\n");
    CLI_INFO("	-etherTypeMask      : Mask for ether type\n");
    CLI_INFO("	options for standard ipv4 header data/mask, format as: -version 4 -versionMask 0xf\n");
    CLI_INFO("	-version        : version number(4bits)\n");
    CLI_INFO("	-ihl            : header length(4bits)\n");
    CLI_INFO("	-tos            : type of service(8bits)\n");
    CLI_INFO("	-ipLength       : total length(16bits)\n");
    CLI_INFO("	-id             : identification(16bits)\n");
    CLI_INFO("	-flag           : fragment flag(3bits)\n");
    CLI_INFO("	-fragOffset     : fragmentation offset(13bits)\n");
    CLI_INFO("	-ttl            : time to live(8bits)\n");
    CLI_INFO("	-protocol       : up level protocol type(8bits)\n");
    CLI_INFO("	-hdrChkSum      : header checksum(16bits)\n");
    CLI_INFO("	-sV4Ip          : source ip address, format as 192.168.0.1\n");
    CLI_INFO("	-dV4Ip          : destination ip address, format as 192.168.0.1\n");
    CLI_INFO("	-versionMask        : Mask for version number(4bits)\n");
    CLI_INFO("	-ihlMask            : Mask for header length(4bits)\n");
    CLI_INFO("	-tosMask            : Mask for type of service(8bits)\n");
    CLI_INFO("	-ipLengthMask       : Mask for total length(16bits)\n");
    CLI_INFO("	-idMask             : Mask for identification(16bits)\n");
    CLI_INFO("	-flagMask           : Mask for fragment flag(3bits)\n");
    CLI_INFO("	-fragOffsetMask     : Mask for fragmentation offset(13bits)\n");
    CLI_INFO("	-ttlMask            : Mask for time to live(8bits)\n");
    CLI_INFO("	-protocolMask       : Mask for up level protocol type(8bits)\n");
    CLI_INFO("	-hdrChkSumMask      : Mask for header checksum(16bits)\n");
    CLI_INFO("	-sV4IpMask          : Mask for source ip address, format as ff.ff.ff.ff\n");
    CLI_INFO("	-dV4IpMask          : Mask for destination ip address, format as ff.ff.ff.ff\n");
    CLI_INFO("	options for standard ipv6 header data/mask, format as: -version 6 -versionMask 0xf\n");
    CLI_INFO("	-version        : version number(4bits)\n");
    CLI_INFO("	-tc             : traffic class(8bits)\n");
    CLI_INFO("	-flowLbl        : flow label(20bits)\n");
    CLI_INFO("	-payloadLen     : payload length(16bits)\n");
    CLI_INFO("	-nextHdr        : next header(8bits)\n");
    CLI_INFO("	-hopLimit       : hop limit(8bits)\n");
    CLI_INFO("	-sV6Ip          : source ip address, format as 2015:1123:4567:89AB:CDEF:0550:A00A:5AA5\n");
    CLI_INFO("	-dV6Ip          : destination ip address, format as 2015:1123:4567:89AB:CDEF:0550:A00A:5AA5\n");
    CLI_INFO("	-versionMask        : Mask for version number(4bits)\n");
    CLI_INFO("	-tcMask             : Mask for traffic class(8bits)\n");
    CLI_INFO("	-flowLblMask        : Mask for flow label(20bits)\n");
    CLI_INFO("	-payloadLenMask     : Mask for payload length(16bits)\n");
    CLI_INFO("	-nextHdrMask        : Mask for next header(8bits)\n");
    CLI_INFO("	-hopLimitMask       : Mask for hop limit(8bits)\n");
    CLI_INFO("	-sV6IpMask          : Mask for source ip address, format as ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff\n");
    CLI_INFO("	-dV6IpMask          : Mask for destination ip address, format as ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff\n");
    CLI_INFO("	options for standard tcp header data/mask, format as: -srcPort 0x123 -srcPortMask 0xffff\n");
    CLI_INFO("	-srcPort        : source port(16bits)\n");
    CLI_INFO("	-destPort       : destonation port(16bits)\n");
    CLI_INFO("	-seqNum         : sequence number(32bits)\n");
    CLI_INFO("	-ackNum         : acknowledgment number(32bits)\n");
    CLI_INFO("	-tcpOffset      : data offset/header length(4bits)\n");
    CLI_INFO("	-tcpFlags       : contral flag(6bits)\n");
    CLI_INFO("	-windowSize     : window size(16bits)\n");
    CLI_INFO("	-chkSum         : checksum(16bits)\n");
    CLI_INFO("	-urgPtr         : urgent pointer(16bits)\n");
    CLI_INFO("	-srcPortMask        : Mask for source port(16bits)\n");
    CLI_INFO("	-destPortMask       : Mask for destonation port(16bits)\n");
    CLI_INFO("	-seqNumMask         : Mask for sequence number(32bits)\n");
    CLI_INFO("	-ackNumMask         : Mask for acknowledgment number(32bits)\n");
    CLI_INFO("	-tcpOffsetMask      : Mask for data offset/header length(4bits)\n");
    CLI_INFO("	-tcpFlagsMask       : Mask for contral flag(6bits)\n");
    CLI_INFO("	-windowSizeMask     : Mask for window size(16bits)\n");
    CLI_INFO("	-chkSumMask         : Mask for checksum(16bits)\n");
    CLI_INFO("	-urgPtrMask         : Mask for urgent pointer(16bits)\n");
    CLI_INFO("	options for standard udp header data/mask, format as: -srcPort 0x123 -srcPortMask 0xffff\n");
    CLI_INFO("	-srcPort        : source port(16bits)\n");
    CLI_INFO("	-destPort       : destonation port(16bits)\n");    
    CLI_INFO("	-udpLength      : length(16bits)\n");
    CLI_INFO("	-chkSum         : checksum(16bits)\n");
    CLI_INFO("	-srcPortMask        : Mask for source port(16bits)\n");
    CLI_INFO("	-destPortMask       : Mask for destonation port(16bits)\n");
    CLI_INFO("	-udpLengthMask      : Mask for length(16bits)\n");
    CLI_INFO("	-chkSumMask         : Mask for checksum(16bits)\n");
    CLI_INFO("	options for tcam action, format as: -interrupt 1 -dpvMode 0x3\n");
    CLI_INFO("	-interrupt        : Interrupt on a TCAM hit\n");
    CLI_INFO("	-IncTcamCtr       : Increment the port's TCAM Counter\n");
    CLI_INFO("	-vidOverride      : VID Override Enable\n");
    CLI_INFO("	-vidData          : VID Override Data\n");
    CLI_INFO("	-qpriOverride     : QPRI Override Enable\n");
    CLI_INFO("	-qpriData         : QPRI Override Data\n");
    CLI_INFO("	-fpriOverride     : FPRI Override Enable\n");
    CLI_INFO("	-fpriData         : FPRI Override Data\n");
    CLI_INFO("	-dpvSF            : DPV Source Port Filter\n");
    CLI_INFO("	-dpvData          : DPV Data\n");
    CLI_INFO("	-dpvMode          : DPV Mode\n");
    CLI_INFO("	-colorMode        : Color Mode\n");
    CLI_INFO("	-vtuPageOverride  : VTU Page Override\n");
    CLI_INFO("	-vtuPage          : VTU Page Data\n");
    CLI_INFO("	-unKnownFilter    : UnKnown Filter\n");
    CLI_INFO("	-egActPoint       : Egress Action Pointer\n");
    CLI_INFO("	-ldBalanceOverride: Load Balance Override Enable\n");
    CLI_INFO("	-ldBalanceData    : Load Balance Override Data\n");
    CLI_INFO("	-DSCPOverride     : DSCP Override Enable\n");
    CLI_INFO("	-DSCP             : DSCP Override Data\n");
    CLI_INFO("	-factionOverride  : Frame Action Override Enable\n");
    CLI_INFO("	-factionData      : Frame Action Override Data\n");

}
static void tcamParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, tcamCMDList));
	if (strIcmp(cmd, "delAll") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam delAll\n");
		
	}
	else if (strIcmp(cmd, "delEntry") == 0)
	{
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam delEntry 0\n");
	}
	else if (strIcmp(cmd, "addEntry") == 0)
	{
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
		printTcamOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam addEntry 0 -frameType 0 -frameTypeMask 3 -spv 3 -spvMask 4 -ppri 5\n");
	}
	else if (strIcmp(cmd, "readEntry") == 0)
	{
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam readEntry 0\n");
	}
	else if (strIcmp(cmd, "getNextEntry") == 0)
	{
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam getNextEntry 0\n");
	}
	else if (strIcmp(cmd, "findEntry") == 0)
	{
		CLI_INFO("	entryNum : the tcam entry index to search.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam findEntry 0\n");
	}
    else if (strIcmp(cmd, "setMode") == 0)
    {
        CLI_INFO("	port : switch port\n");
        CLI_INFO("	mode : tcam mode\n");
        CLI_INFO("      0 : MSD_TCAM_MODE_DISABLE\n");
        CLI_INFO("      1 : MSD_TCAM_MODE_ENABLE_48\n");
        CLI_INFO("      2 : MSD_TCAM_MODE_ENABLE_96\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam setMode 0 0\n");
    }
    else if (strIcmp(cmd, "getMode") == 0)
    {
        CLI_INFO("	port : switch port\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam getMode 0\n");
    }
    else if (strIcmp(cmd, "addPort") == 0)
    {
        CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
        CLI_INFO("	port : switch port\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam addPort 0 0\n");
    }
    else if (strIcmp(cmd, "removePort") == 0)
    {
        CLI_INFO("	entryNum : pointer to the desired entry of TCAM (Peridot: 0 ~ 255  Topaz: 0 ~ 127)\n");
        CLI_INFO("	port : switch port\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam removePort 0 0\n");
    }
	else if (strIcmp(cmd, "egrDelAllPorts") == 0)
	{
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (1 ~ 63)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam egrDelAllPorts 1\n");
	}
	else if (strIcmp(cmd, "egrDelEntry") == 0)
	{
		CLI_INFO("	port : switch port\n");
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (1 ~ 63)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam egrDelEntry 1 1\n");
	}
	else if (strIcmp(cmd, "egrAddEntry") == 0)
	{
		CLI_INFO("	port : switch port\n");
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (1 ~ 63)\n");
		printTcamEgrOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam egrAddEntry 1 1 -frameModeOverride 1 -frameMode 1 -tagModeOverride 1\n");
	}
	else if (strIcmp(cmd, "egrReadEntry") == 0)
	{
		CLI_INFO("	port : switch port\n");
		CLI_INFO("	entryNum : pointer to the desired entry of TCAM (1 ~ 63)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam egrReadEntry 1 1\n");
		
	}
	else if (strIcmp(cmd, "egrGetNextEntry") == 0)
	{
		CLI_INFO("	port : switch port\n");
		CLI_INFO("	entryNum : pointer to start search TCAM\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        tcam egrGetNextEntry 1 0\n");
	}
    else if (strIcmp(cmd, "advanceConfig") == 0)
    {
        CLI_INFO("	pktType : Enumeration of TCAM mapping packet type\n");
        CLI_INFO("	    0 : TCP over IPv4 packet\n");
        CLI_INFO("	    1 : UDP over IPv4 packet\n");
        CLI_INFO("	    2 : TCP over IPv6 packet\n");
        CLI_INFO("	    3 : UDP over IPv6 packet\n");
        CLI_INFO("	entryNum1 : the first tcam entry number\n");
        CLI_INFO("	entryNum2 : the second tcam entry number\n");
        printOutAdvConfigOptions();
        CLI_INFO("Example:\n");
        CLI_INFO("  To match destionation macAddr and destionation IPV6 ipAddr and replace the frame's DPV\n");
        CLI_INFO("        tcam advanceConfig 2 1 15 -frameType 0 -frameTypeMask 0x3 -da 01005e000000 -daMask ffffffffffff -etherType 0x86dd -etherTypeMask 0xffff -dV6Ip 2015:1123:4567:89AB:CDEF:0550:A00A:5AA5 -dV6IpMask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dpvData 6 -dpvMode 3\n");
    }
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("Example:\n");
        CLI_INFO("        tcam dump\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int tcamHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		tcamParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (tcamCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", tcamCMDList[index].help);
		index++;
	}

	return 0;
}
int tcamOperation()
{
	int index = 0;
	while (1)
	{
		if (tcamCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], tcamCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(tcamCMDList[index]);
			tcamCMDList[index].callback();
			break;
		}
		index++;
	}
	if (tcamCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"tcam help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

int ReadRegister()
{
	MSD_STATUS retVal;
	MSD_U8 devAddr, regAddr;
	MSD_U16 data = 0;

	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: rr  <devAddr> <regAddr> : Read register\n");
		return 1;
	}

	devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
	regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdGetAnyReg(sohoDevNum, devAddr, regAddr, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read  dev=%X reg=%X val=%X ret[%d: %s]"
			"\n", devAddr, regAddr, data, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("Read - dev: %X, reg: %X, data = %X \n", devAddr, regAddr, data);
	return 0;
}
int WriteRegister()
{
	MSD_STATUS retVal;
	MSD_U8 devAddr, regAddr;
	MSD_U16 data;

	if (nargs != 4)
	{
		CLI_ERROR("Syntax Error, Using command as follows: rw <devAddr> <regAddr> <data> : Write register\n");
		return 1;
	}

	devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
	regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	data = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);

	retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Write  dev=%X reg=%X val=%X ret[%d: %s]"
			"\n", devAddr, regAddr, data, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
/*
int ReadRegisterField()
{
	MSD_STATUS retVal;
	MSD_U8 devAddr, regAddr, offset, size;
	MSD_U16 data = 0;

	if (nargs != 5)
	{
		CLI_ERROR("Syntax Error, Using command as follows: rrf <devAddr> <regAddr> <offset> <size> : Read register field\n");
		return 1;
	}

	devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
	regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	offset = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
	size = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);

	retVal = msdGetAnyRegField(sohoDevNum, devAddr, regAddr, offset, size, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read Filed dev=%X reg=%X offset=%X, size=%X, val=%X ret[%d: %s]"
			"\n", devAddr, regAddr, offset, size, data, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("ReadField - dev: %X, reg: %X, offset: %X, size: %X, data = %X \n", devAddr, regAddr, offset, size, data);
	return 0;
}
int WriteRegisterField()
{
	MSD_STATUS retVal;
	MSD_U8 devAddr, regAddr, offset, size;
	MSD_U16 data = 0;

	if (nargs != 6)
	{
		CLI_ERROR("Syntax Error, Using command as follows: rwf <devAddr> <regAddr> <offset> <size> <data>: Write register Field\n");
		return 1;
	}

	devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
	regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	offset = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
	size = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);
	data = (MSD_U16)strtoul(CMD_ARGS[5], NULL, 0);

	retVal = msdSetAnyRegField(sohoDevNum, devAddr, regAddr, offset, size, data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error Read Filed dev=%X reg=%X offset=%X, size=%X, val=%X ret[%d: %s]"
			"\n", devAddr, regAddr, offset, size, data, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("ReadField - dev: %X, reg: %X, offset: %X, size: %X, data = %X \n", devAddr, regAddr, offset, size, data);
	return 0;
}


int setBusInterface()
{
	MSD_STATUS retVal = MSD_OK;
	MSD_INTERFACE channel;

	if (nargs != 2)
	{
		CLI_INFO("Syntax Error, Using command as follows: setBus <busInterface> : Set the bus interface, SMI, RMU or SMI multichip\n");
		return 1;
	}

	if (strIcmp(CMD_ARGS[1], "help") == 0)
	{
		CLI_INFO("setBus <busInterface>\n");
		CLI_INFO("	busInterface:\n");
		CLI_INFO("		0: MSD_INTERFACE_SMI\n");
		CLI_INFO("		1: MSD_INTERFACE_SMI_MULTICHIP\n");
		CLI_INFO("		2: MSD_INTERFACE_RMU\n");
	}
	else
	{
		channel = (MSD_INTERFACE)strtoul(CMD_ARGS[1], NULL, 0);
		retVal = gtSetDriverInterface(sohoDevNum, channel);
		if (retVal != MSD_OK) {
			CLI_ERROR("Error set dirver bus interface ret[%d: %s]"
				"\n", retVal, reflectMSD_STATUS(retVal));
			return MSD_FAIL;
		}
	}

	return retVal;
}
*/
int getBusInterface()
{
	MSD_STATUS retVal = MSD_OK;
	MSD_INTERFACE channel;
	char *str;

	retVal = msdGetDriverInterface(sohoDevNum, &channel);
	if (retVal != MSD_OK) {
		CLI_ERROR("Error get dirver bus interface ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	switch (channel)
	{
	case MSD_INTERFACE_SMI:
		str = "SMI";
		break;
	case MSD_INTERFACE_SMI_MULTICHIP:
		str = "SMI MultiChip";
		break;
	case MSD_INTERFACE_RMU:
		str = "RMU";
		break;
	default:
		str = "Unkonwn";
		break;
	}

	CLI_INFO("Current Bus interface is: %s\n", str);
	return retVal;
}

static void portctrlParaHelp(char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, portCtrlCMDList));

    if (strIcmp(cmd, "setDefVid") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
        CLI_INFO("	vid : the port vlan id.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setDefVid 0 1\n");
    }
    else if (strIcmp(cmd, "getDefVid") == 0)
    {
        CLI_INFO("	port : logical port number to set.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDefVid 0\n");
    }
    else if (strIcmp(cmd, "setQMode") == 0)
    {
        CLI_INFO("	port : logical port number.\n");
        CLI_INFO("	mode :802.1q mode for this port.\n");
        CLI_INFO("      0 : Disable\n");
        CLI_INFO("      1 : Fallback\n");
        CLI_INFO("      2 : Check\n");
        CLI_INFO("      3 : Secure\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setQMode 0 1\n");
    }
    else if (strIcmp(cmd, "getQMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getQMode 0\n");
    }
    else if (strIcmp(cmd, "setDiscardTagged") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to discard tagged frame 0 otherwise.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setDiscardTagged 0 1\n");
    }
    else if (strIcmp(cmd, "getDiscardTagged") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDiscardTagged 0\n");
    }
    else if (strIcmp(cmd, "setDiscardUTagged") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to discard tagged frame 0 otherwise.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setDiscardUTagged 0 1\n");
    }
    else if (strIcmp(cmd, "getDiscardUTagged") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDiscardUTagged 0\n");
    }
    else if (strIcmp(cmd, "setUCFloodBlock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to do not egress out the unknown DA unicast frames, 0 for normal operation.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setUCFloodBlock 0 1\n");
    }
    else if (strIcmp(cmd, "getUCFloodBlock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getUCFloodBlock 0\n");
    }
    else if (strIcmp(cmd, "setMCFloodBlock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to do not egress out the unknown DA multicast frames, 0 for normal operation.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setMCFloodBlock 0 1\n");
    }
    else if (strIcmp(cmd, "getMCFloodBlock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getMCFloodBlock 0\n");
    }
    else if (strIcmp(cmd, "setBCMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to enable the discard broadcast mode, 0 for normal operation.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setBCMode 0 1\n");
    }
    else if (strIcmp(cmd, "getBCMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getBCMode 0\n");
    }
    else if (strIcmp(cmd, "setIGMPSnoopEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to enable the mode that trapping all the IPV4 IGMP packets to CPU, 0 to disable the mode.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setIGMPSnoopEnable 1 1\n");
    }
    else if (strIcmp(cmd, "getIGMPSnoopEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getIGMPSnoopEnable 1\n");
    }
    else if (strIcmp(cmd, "setHeaderMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to enable the ingress and egress header mode, 0 to disable the mode.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setHeaderMode 0 1\n");
    }
    else if (strIcmp(cmd, "getHeaderMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getHeaderMode 0\n");
    }
    else if (strIcmp(cmd, "setDropOnLock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to enable the drop on lock mode, 0 to disable the mode.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setDropOnLock 0 1\n");
    }
    else if (strIcmp(cmd, "getDropOnLock") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDropOnLock 0\n");
    }
    else if (strIcmp(cmd, "setMtu") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode: frame max byte size\n");
        CLI_INFO("	    0: Rx and Tx frames with max byte of 1522.\n");
        CLI_INFO("	    1: Rx and Tx frames with max byte of 2048.\n");
        CLI_INFO("	    2: Rx and Tx frames with max byte of 10240. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setMtu 1 1\n");
    }
    else if (strIcmp(cmd, "getMtu") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getMtu 1\n");
    }
    else if (strIcmp(cmd, "setLearnEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : 1 for learning enable or 0 otherwise\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setLearnEnable 0 1\n");
    }
    else if (strIcmp(cmd, "getLearnEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getLearnEnable 0\n");
    }
    else if (strIcmp(cmd, "setVTUPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("	    0: MSD_PRI_OVERRIDE_NONE \n");
        CLI_INFO("	    1: MSD_PRI_OVERRIDE_FRAME \n");
        CLI_INFO("	    2: MSD_PRI_OVERRIDE_QUEUE \n");
        CLI_INFO("	    3: MSD_PRI_OVERRIDE_FRAME_QUEUE \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setVTUPriOverride 0 1\n");
    }
    else if (strIcmp(cmd, "getVTUPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getVTUPriOverride 0\n");
    }
    else if (strIcmp(cmd, "setSAPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("	    0: MSD_PRI_OVERRIDE_NONE \n");
        CLI_INFO("	    1: MSD_PRI_OVERRIDE_FRAME \n");
        CLI_INFO("	    2: MSD_PRI_OVERRIDE_QUEUE \n");
        CLI_INFO("	    3: MSD_PRI_OVERRIDE_FRAME_QUEUE \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setSAPriOverride 0 1\n");
    }
    else if (strIcmp(cmd, "getSAPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getSAPriOverride 0\n");
    }
    else if (strIcmp(cmd, "setDAPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("	    0: MSD_PRI_OVERRIDE_NONE \n");
        CLI_INFO("	    1: MSD_PRI_OVERRIDE_FRAME \n");
        CLI_INFO("	    2: MSD_PRI_OVERRIDE_QUEUE \n");
        CLI_INFO("	    3: MSD_PRI_OVERRIDE_FRAME_QUEUE \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setDAPriOverride 0 1\n");
    }
    else if (strIcmp(cmd, "getDAPriOverride") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDAPriOverride 0\n");
    }
    else if (strIcmp(cmd, "setMsgPort") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to enable this port as message port, 0 to disable the mode.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setMsgPort 0 1\n");
    }
    else if (strIcmp(cmd, "getMsgPort") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getMsgPort 0\n");
    }
    else if (strIcmp(cmd, "setEtherType") == 0)
    {
        CLI_INFO("	port  : logical port number. \n");
        CLI_INFO("	eType : ethernet type.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setEtherType 0 1\n");
    }
    else if (strIcmp(cmd, "getEtherType") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getEtherType 0\n");
    }
    else if (strIcmp(cmd, "setAllowVidZero") == 0)
    {
        CLI_INFO("	port  : logical port number. \n");
        CLI_INFO("	en   : 1 to allow the vid of zero, 0 to disable the mode.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setAllowVidZero 0 1\n");
    }
    else if (strIcmp(cmd, "getAllowVidZero") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getAllowVidZero 0 \n");
    }
    else if (strIcmp(cmd, "setForceLinkMode") == 0)
    {
        CLI_INFO("	port  : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("	    0: PORT_FORCE_LINK_UP \n");
        CLI_INFO("	    1: PORT_FORCE_LINK_DOWN \n");
        CLI_INFO("	    2: PORT_DO_NOT_FORCE_LINK \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setForceLinkMode 1 1\n");
    }
    else if (strIcmp(cmd, "getForceLinkMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getForceLinkMode 1\n");
    }
    else if (strIcmp(cmd, "setForceDuplexMode") == 0)
    {
        CLI_INFO("	port  : logical port number. \n");
        CLI_INFO("	mode : \n");
        CLI_INFO("	    0: MSD_DUPLEX_HALF \n");
        CLI_INFO("	    1: MSD_DUPLEX_FULL \n");
        CLI_INFO("	    2: MSD_DUPLEX_AUTO \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setForceDuplexMode 1 1\n");
    }
    else if (strIcmp(cmd, "getForceDuplexMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getForceDuplexMode 1\n");
    }
    else if (strIcmp(cmd, "setForceSpeed") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode:MSD_PORT_FORCED_SPEED_MODE\n");
        CLI_INFO("	    0 : MSD_PORT_FORCE_SPEED_10M\n");
        CLI_INFO("	    1 : MSD_PORT_FORCE_SPEED_100M\n");
        CLI_INFO("	    2 : MSD_PORT_FORCE_SPEED_200M\n");
        CLI_INFO("	    3 : MSD_PORT_FORCE_SPEED_1000M\n");
        CLI_INFO("	    4 : MSD_PORT_FORCE_SPEED_2_5G\n");
        CLI_INFO("	    5 : MSD_PORT_FORCE_SPEED_10G\n");
        CLI_INFO("	    6 : MSD_PORT_DO_NOT_FORCE_SPEED\n");
        CLI_INFO("Example:\n");
	CLI_INFO("        port setForceSpeed 0 1\n");
    }
    else if (strIcmp(cmd, "getForceSpeed") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getForceSpeed 0\n");
    }
    else if (strIcmp(cmd, "setEMS") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : 1 to set EgressMonitorSource or 0 otherwise\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setEMS 0 1\n");
    }
    else if (strIcmp(cmd, "getEMS") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getEMS 0\n");
    }
    else if (strIcmp(cmd, "setIMS") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : 1 to set IngressMonitorSource or 0 otherwise\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setIMS 0 1\n");
    }
    else if (strIcmp(cmd, "getIMS") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getIMS 0\n");
    }
    else if (strIcmp(cmd, "setEgrRate") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : Enumeration of the port egress rate limit counting mode\n");
        CLI_INFO("		0:Count the number of frames\n");
        CLI_INFO("		1:Count all Layer 1 bytes\n");
        CLI_INFO("		2:Count all Layer 2 bytes\n");
        CLI_INFO("		3:Count all Layer 3 bytes\n");
        CLI_INFO("	rate : union data, kbRate or frame Rate based on ELIMIT_MODE\n");
        CLI_INFO("		kbRate valid value:\n");
        CLI_INFO("			64, 128, 192, 256, 320, 384,..., 960,\n");
        CLI_INFO("			1000, 2000, 3000, 4000, ..., 100000,\n");
        CLI_INFO("			110000, 120000, 130000, ..., 1000000\n");
        CLI_INFO("			1100000, 1200000, 1300000, ..., 5000000.(Peridot only)\n");
        CLI_INFO("		fRate valid value:\n");
        CLI_INFO("			3815 and 14880000 (Peridot)\n");
        CLI_INFO("			7600 and 1488000 (Topaz)\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setEgrRate 1 1 100000\n");
    }
    else if (strIcmp(cmd, "getEgrRate") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getEgrRate 1\n");
    }   
    else if (strIcmp(cmd, "setFlowCtrl") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	isEnable :enable/disable the flow control.\n");
        CLI_INFO("	mode : flow control mode\n");
        CLI_INFO("		0:PORT_FC_TX_RX_ENABLED.\n");
        CLI_INFO("		1:PORT_RX_ONLY.\n");
        CLI_INFO("		2:PORT_TX_ONLY.\n");
        CLI_INFO("		3:PORT_PFC_ENABLED.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setFlowCtrl 1 1 1\n");
    }
    else if (strIcmp(cmd, "getFlowCtrl") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getFlowCtrl 1\n");
    }
    else if (strIcmp(cmd, "getDuplex") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getDuplex 1\n");
    }
    else if (strIcmp(cmd, "getLinkState") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getLinkState 1\n");
    }
    else if (strIcmp(cmd, "getSpeed") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getSpeed 1\n");
    }
    else if (strIcmp(cmd, "setFrameMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : frame mode\n");
        CLI_INFO("		0 : Normal Network mode\n");
        CLI_INFO("		1 : DSA mode\n");
        CLI_INFO("		2 : Provider mode\n");
        CLI_INFO("		3 : Ether Type DSA mode\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setFrameMode 0 1\n");
    }
    else if (strIcmp(cmd, "getFrameMode") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getFrameMode 0\n");
    }
    else if (strIcmp(cmd, "setVlanPorts") == 0)
    {
        CLI_INFO("Format as : port setVlanPorts 0 {1 2 3 4}\n");
        CLI_INFO("	port : logical port number. \n");
	CLI_INFO("	memPortsArray	: array of vlan prots for this port\n");
	CLI_INFO("Example:\n");
	CLI_INFO("        port setVlanPorts 0 {1 2 3}\n");
    }
    else if (strIcmp(cmd, "getVlanPorts") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getVlanPorts 0\n");
    }
    else if (strIcmp(cmd, "setPortState") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	mode : Port state mode\n");
        CLI_INFO("		0 : disabled\n");
        CLI_INFO("		1 : blocking/listening\n");
        CLI_INFO("		2 : learning\n");
        CLI_INFO("		3 : forwarding\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port setPortState 0 1\n");
    }
    else if (strIcmp(cmd, "getPortState") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        port getPortState 0\n");
    }
    else if (strIcmp(cmd, "setIgnoreFcs") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to force good fcs or 0 otherwise\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setIgnoreFcs 0 1\n");
    }
    else if (strIcmp(cmd, "getIgnoreFcs") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getIgnoreFcs 0\n");
    }
    else if (strIcmp(cmd, "setAllowBadFcs") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en   : 1 to allow bad fcs or 0 otherwise\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setAllowBadFcs 0 1\n");
    }
    else if (strIcmp(cmd, "getAllowBadFcs") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getAllowBadFcs 0\n");
    }
    else if (strIcmp(cmd, "setPauseLimitOut") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	limit: the max number of Pause refresh frames for each congestion situation(0 ~ 0xff)\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setPauseLimitOut 0 10\n");
    }
    else if (strIcmp(cmd, "getPauseLimitOut") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getPauseLimitOut 0\n");
    }
    else if (strIcmp(cmd, "setPauseLimitIn") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	limit: the max number of Pause refresh frames for each congestion situation(0 ~ 0xff)\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setPauseLimitIn 0 10\n");
    }
    else if (strIcmp(cmd, "getPauseLimitIn") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getPauseLimitIn 0\n");
    }
    else if (strIcmp(cmd, "setQueueToPause") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	queue: output queues that are paused off\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setQueueToPause 0 0\n");
    }
    else if (strIcmp(cmd, "getQueueToPause") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getQueueToPause 0\n");
    }
    else if (strIcmp(cmd, "setRxPriFCEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setRxPriFCEnable 0 0 1\n");
    }
    else if (strIcmp(cmd, "getRxPriFCEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getRxPriFCEnable 0 0\n");
    }
    else if (strIcmp(cmd, "setRxPriFCToQueue") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("	queue : priority queue \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setRxPriFCToQueue 0 0 1\n");
    }
    else if (strIcmp(cmd, "getRxPriFCToQueue") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getRxPriFCToQueue 0 0\n");
    }
    else if (strIcmp(cmd, "setTxPriFCEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setTxPriFCEnable 0 0 1\n");
    }
    else if (strIcmp(cmd, "getTxPriFCEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	pfcPri: priority flow control frame's priority\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getTxPriFCEnable 0 0\n");
    }
    else if (strIcmp(cmd, "getOutQueueSize") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	queue : priority queue number\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getOutQueueSize 0 0\n");
    }
    else if (strIcmp(cmd, "setMapDA") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable map DA, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setMapDA 0 1\n");
    }
    else if (strIcmp(cmd, "getMapDA") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getMapDA 0\n");
    }
    else if (strIcmp(cmd, "setARPMirror") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setARPMirror 0 1\n");
    }
    else if (strIcmp(cmd, "getARPMirror") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getARPMirror 0\n");
    }
    else if (strIcmp(cmd, "setLocked") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setLocked 0 1\n");
    }
    else if (strIcmp(cmd, "getLocked") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getLocked 0\n");
    }
    else if (strIcmp(cmd, "setHoldAt1") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setHoldAt1 0 1\n");
    }
    else if (strIcmp(cmd, "getHoldAt1") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getHoldAt1 0\n");
    }
    else if (strIcmp(cmd, "setIntOnAgeOut") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setIntOnAgeOut 0 1\n");
    }
    else if (strIcmp(cmd, "getIntOnAgeOut") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getIntOnAgeOut 0\n");
    }
    else if (strIcmp(cmd, "setRefreshLocked") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setRefreshLocked 0 1\n");
    }
    else if (strIcmp(cmd, "getRefreshLocked") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getRefreshLocked 0\n");
    }
    else if (strIcmp(cmd, "setIgnoreWrongData") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	en : 1 to enable, 0 to disable. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setIgnoreWrongData 0 1\n");
    }
    else if (strIcmp(cmd, "getIgnoreWrongData") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getIgnoreWrongData 0\n");
    }
    else if (strIcmp(cmd, "setCutThroughEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("	[options](default value for option item is 0):\n");
        CLI_INFO("	options for Cut Through, format as: -enable 1 -queue 0\n");
        CLI_INFO("	-enable		: 1 to enable cut through, 0 to disable\n");
        CLI_INFO("	-queue   	: the value of the cut through queue\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port setCutThroughEnable 0 -enable 1 -queue 0\n");
    }
    else if (strIcmp(cmd, "getCutThroughEnable") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port getCutThroughEnable 0\n");
    }
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        port dump 0\n");
    }
    else
    {
        CLI_ERROR("Can not find the command: %s\n", cmd);
    }
}
static int updateMemPortsArray(MSD_LPORT* memPorts, int len, int startIndex)
{
    int index = startIndex;
    char *value;
    int i;
    unsigned int tempPortsList[MSD_MAX_SWITCH_PORTS] = { 0 };

    value = CMD_ARGS[index];
    index++;
    
    if (updateArray(&index, value, len, tempPortsList) != 0)
    {
        return -1;
    }

    for (i = 0; i < len; i++)
    {
        memPorts[i] = tempPortsList[i];
    }
    return 0;
}
int portSetVlanPorts()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_LPORT *memPorts;
    MSD_U8  memPortsLen;
    int i;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memPortsLen = nargs - 3;
    for (i = 3; i < nargs; i++)
    {
        if (strcmp(CMD_ARGS[i], "{") == 0 || strcmp(CMD_ARGS[i], "}") == 0)
        {
            memPortsLen--;
        }
    }
    memPorts = (MSD_LPORT*)malloc(memPortsLen*sizeof(MSD_LPORT));
    if (memPorts == NULL)
    {
        CLI_ERROR("malloc memery for memPorts error\n");
        return 1;
    }

    if (updateMemPortsArray(memPorts, memPortsLen, 3) != 0)
        return -1;

    retVal = msdPortVlanPortsSet(sohoDevNum, port, memPorts, memPortsLen);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            free(memPorts);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error setPortVlanPorts ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        free(memPorts);
        return MSD_FAIL;
    }

    free(memPorts);
    return retVal;
}
int portGetVlanPorts()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_LPORT memPorts[MSD_MAX_SWITCH_PORTS] = {0};
    char tempStr[200] = { 0 };
    MSD_U8  memPortsLen = 0;
    int i;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdPortVlanPortsGet(sohoDevNum, port, memPorts, &memPortsLen);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error setPortVlanPorts ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    sprintf(tempStr, "Array of Vlan Prots for port%d: ", port);
    //CLI_INFO("Array of Vlan Prots for port%d: \n", port);
    for (i = 0; i < memPortsLen; i++)
    {
        //CLI_INFO("%d ", memPorts[i]);
        sprintf(tempStr + strlen(tempStr), "%d ", memPorts[i]);
    }
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
    //CLI_INFO("\n");

    return retVal;
}
static int updateCutThrough(MSD_CUT_THROUGH* cutThru, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-enable") == 0)
        {
            cutThru->enable = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-queue") == 0)
        {
            cutThru->cutThruQueue = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutCutThrough( MSD_LPORT port, MSD_CUT_THROUGH* cutThru)
{
    CLI_INFO("Port %d Cut Through status:\n", port);
    CLI_INFO("  enable status:        %d\n", cutThru->enable);
    CLI_INFO("  Cut through queue:    0x%x\n", cutThru->cutThruQueue);
}
int portSetCutThrough()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_CUT_THROUGH cutThru;

    if (nargs < 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&cutThru, 0, sizeof(MSD_CUT_THROUGH));
    if (updateCutThrough(&cutThru, 3) != 0)
        return -1;

    retVal = msdPortCutThroughEnableSet(sohoDevNum, port, &cutThru);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int portGetCutThrough()
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_CUT_THROUGH cutThru;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&cutThru, 0, sizeof(MSD_CUT_THROUGH));

    retVal = msdPortCutThroughEnableGet(sohoDevNum, port, &cutThru);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutCutThrough(port, &cutThru);

    return 0;
}
int portCtrlHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        portctrlParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (portCtrlCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", portCtrlCMDList[index].help);
        index++;
    }

    return 0;
}
int portCtrlOperation()
{
	int index = 0;
	while (1)
	{
		if (portCtrlCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], portCtrlCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(portCtrlCMDList[index]);
			portCtrlCMDList[index].callback();
			break;
		}
		index++;
	}
	if (portCtrlCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"port help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

static void displayCounter(MSD_STATS_COUNTER_SET *statsCounter, MSD_BOOL fullbank)
{
	/* Bank 0 */
	CLI_INFO("InGoodOctetsLo  %08lu   \n", statsCounter->InGoodOctetsLo);
	CLI_INFO("InGoodOctetsHi  %08lu   \n", statsCounter->InGoodOctetsHi);
	CLI_INFO("InBadOctets     %08lu   \n", statsCounter->InBadOctets);
	CLI_INFO("OutFCSErr       %08lu   \n", statsCounter->OutFCSErr);
	CLI_INFO("InUnicasts      %08lu   \n", statsCounter->InUnicasts);
	CLI_INFO("Deferred        %08lu   \n", statsCounter->Deferred);
	CLI_INFO("InBroadcasts    %08lu   \n", statsCounter->InBroadcasts);
	CLI_INFO("InMulticasts    %08lu   \n", statsCounter->InMulticasts);
	/*
	Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
	(refer to Histogram Mode)
	*/
	CLI_INFO("Octets64        %08lu   \n", statsCounter->Octets64);
	CLI_INFO("Octets127       %08lu   \n", statsCounter->Octets127);
	CLI_INFO("Octets255       %08lu   \n", statsCounter->Octets255);
	CLI_INFO("Octets511       %08lu   \n", statsCounter->Octets511);
	CLI_INFO("Octets1023      %08lu   \n", statsCounter->Octets1023);
	CLI_INFO("OctetsMax       %08lu   \n", statsCounter->OctetsMax);
	CLI_INFO("OutOctetsLo     %08lu   \n", statsCounter->OutOctetsLo);
	CLI_INFO("OutOctetsHi     %08lu   \n", statsCounter->OutOctetsHi);
	CLI_INFO("OutUnicasts     %08lu   \n", statsCounter->OutUnicasts);
	CLI_INFO("Excessive       %08lu   \n", statsCounter->Excessive);
	CLI_INFO("OutMulticasts   %08lu   \n", statsCounter->OutMulticasts);
	CLI_INFO("OutBroadcasts   %08lu   \n", statsCounter->OutBroadcasts);
	CLI_INFO("Single          %08lu   \n", statsCounter->Single);

	CLI_INFO("OutPause        %08lu   \n", statsCounter->OutPause);
	CLI_INFO("InPause         %08lu   \n", statsCounter->InPause);
	CLI_INFO("Multiple        %08lu   \n", statsCounter->Multiple);
	CLI_INFO("InUndersize     %08lu   \n", statsCounter->InUndersize);
	CLI_INFO("InFragments     %08lu   \n", statsCounter->InFragments);
	CLI_INFO("InOversize      %08lu   \n", statsCounter->InOversize);
	CLI_INFO("InJabber        %08lu   \n", statsCounter->InJabber);
	CLI_INFO("InRxErr         %08lu   \n", statsCounter->InRxErr);
	CLI_INFO("InFCSErr        %08lu   \n", statsCounter->InFCSErr);
	CLI_INFO("Collisions      %08lu   \n", statsCounter->Collisions);
	CLI_INFO("Late            %08lu   \n", statsCounter->Late);

	/* Bank 1 */
    if (fullbank == 1)
    {
        CLI_INFO("InDiscards      %08lu   \n", statsCounter->InDiscards);
        CLI_INFO("InFiltered      %08lu   \n", statsCounter->InFiltered);
        CLI_INFO("InAccepted      %08lu   \n", statsCounter->InAccepted);
        CLI_INFO("InBadAccepted   %08lu   \n", statsCounter->InBadAccepted);
        CLI_INFO("InGoodAvbClassA %08lu   \n", statsCounter->InGoodAvbClassA);
        CLI_INFO("InGoodAvbClassB %08lu   \n", statsCounter->InGoodAvbClassB);
        CLI_INFO("InBadAvbClassA  %08lu   \n", statsCounter->InBadAvbClassA);
        CLI_INFO("InBadAvbClassB  %08lu   \n", statsCounter->InBadAvbClassB);
        CLI_INFO("TCAMCounter0    %08lu   \n", statsCounter->TCAMCounter0);
        CLI_INFO("TCAMCounter1    %08lu   \n", statsCounter->TCAMCounter1);
        CLI_INFO("TCAMCounter2    %08lu   \n", statsCounter->TCAMCounter2);
        CLI_INFO("TCAMCounter3    %08lu   \n", statsCounter->TCAMCounter3);
        CLI_INFO("InDroppedAvbA   %08lu   \n", statsCounter->InDroppedAvbA);
        CLI_INFO("InDroppedAvbB   %08lu   \n", statsCounter->InDroppedAvbB);
        CLI_INFO("InDaUnknown     %08lu   \n", statsCounter->InDaUnknown);
        CLI_INFO("InMGMT          %08lu   \n", statsCounter->InMGMT);
        CLI_INFO("OutQueue0       %08lu   \n", statsCounter->OutQueue0);
        CLI_INFO("OutQueue1       %08lu   \n", statsCounter->OutQueue1);
        CLI_INFO("OutQueue2       %08lu   \n", statsCounter->OutQueue2);
        CLI_INFO("OutQueue3       %08lu   \n", statsCounter->OutQueue3);
        CLI_INFO("OutQueue4       %08lu   \n", statsCounter->OutQueue4);
        CLI_INFO("OutQueue5       %08lu   \n", statsCounter->OutQueue5);
        CLI_INFO("OutQueue6       %08lu   \n", statsCounter->OutQueue6);
        CLI_INFO("OutQueue7       %08lu   \n", statsCounter->OutQueue7);
        CLI_INFO("OutCutThrough   %08lu   \n", statsCounter->OutCutThrough);
        CLI_INFO("reserved_19     %08lu   \n", statsCounter->reserved_19);
        CLI_INFO("OutOctetsA      %08lu   \n", statsCounter->OutOctetsA);
        CLI_INFO("OutOctetsB      %08lu   \n", statsCounter->OutOctetsB);
        CLI_INFO("reserved_1C     %08lu   \n", statsCounter->OutYel);
        CLI_INFO("OutDroppedYel   %08lu   \n", statsCounter->OutDroppedYel);
        CLI_INFO("OutDiscards     %08lu   \n", statsCounter->OutDiscards);
        CLI_INFO("OutMGMT         %08lu   \n", statsCounter->OutMGMT);
    }
}
static void mibParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, mibCMDList));
	if (strIcmp(cmd, "flush") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        mib flush\n");
	}
	else if (strIcmp(cmd, "flushport") == 0)
	{
		CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        mib flushport 0\n");
		
	}
	else if (strIcmp(cmd, "getPortCounter") == 0)
	{
		CLI_INFO("	port : logical port number. \n");
		CLI_INFO("	counter : the counter which will be read.\n");
		CLI_INFO("		0 : STATS_InGoodOctetsLo .\n");
		CLI_INFO("		1 : STATS_InGoodOctetsHi.\n");
		CLI_INFO("		2 : STATS_InBadOctets.\n");
		CLI_INFO("		3 : STATS_OutFCSErr.\n");
		CLI_INFO("		4 : STATS_InUnicasts.\n");
		CLI_INFO("		5 : STATS_Deferred.\n");
		CLI_INFO("		6 : STATS_InBroadcasts.\n");
		CLI_INFO("		7 : STATS_InMulticasts.\n");
		CLI_INFO("		8 : STATS_64Octets.\n");
		CLI_INFO("		9 : STATS_127Octets.\n");
		CLI_INFO("		10 : STATS_255Octets.\n");
		CLI_INFO("		11 : STATS_511Octets.\n");
		CLI_INFO("		12 : STATS_1023Octets.\n");
		CLI_INFO("		13 : STATS_MaxOctets.\n");
		CLI_INFO("		14 : STATS_OutOctetsLo.\n");
		CLI_INFO("		15 : STATS_OutOctetsHi.\n");
		CLI_INFO("		16 : STATS_OutUnicasts.\n");
		CLI_INFO("		17 : STATS_Excessive.\n");
		CLI_INFO("		18 : STATS_OutMulticasts.\n");
		CLI_INFO("		19 : STATS_OutBroadcasts.\n");
		CLI_INFO("		20 : STATS_Single.\n");
		CLI_INFO("		21 : STATS_OutPause.\n");
		CLI_INFO("		22 : STATS_InPause.\n");
		CLI_INFO("		23 : STATS_Multiple.\n");
		CLI_INFO("		24 : STATS_InUndersize.\n");
		CLI_INFO("		25 : STATS_InFragments.\n");
		CLI_INFO("		26 : STATS_InOversize.\n");
		CLI_INFO("		27 : STATS_InJabber.\n");
		CLI_INFO("		28 : STATS_InRxErr.\n");
		CLI_INFO("		29 : STATS_InFCSErr.\n");
		CLI_INFO("		30 : STATS_Collisions.\n");
		CLI_INFO("		31 : STATS_Late.\n");
		CLI_INFO("\n");
		CLI_INFO("		0x400 : STATS_InDiscards .\n");
		CLI_INFO("		0x401 : STATS_InFiltered .\n");
		CLI_INFO("		0x402 : STATS_InAccepted .\n");
		CLI_INFO("		0x403 : STATS_InBadAccepted .\n");
		CLI_INFO("		0x404 : STATS_InGoodAvbClassA .\n");
		CLI_INFO("		0x405 : STATS_InGoodAvbClassB .\n");
		CLI_INFO("		0x406 : STATS_InBadAvbClassA .\n");
		CLI_INFO("		0x407 : STATS_InBadAvbClassB .\n");
		CLI_INFO("		0x408 : STATS_TCAMCounter0 .\n");
		CLI_INFO("		0x409 : STATS_TCAMCounter1 .\n");
		CLI_INFO("		0x40a : STATS_TCAMCounter2 .\n");
		CLI_INFO("		0x40b : STATS_TCAMCounter3 .\n");
		CLI_INFO("		0x40c : STATS_InDroppedAvbA .\n");
		CLI_INFO("		0x40d : STATS_InDroppedAvbB .\n");
		CLI_INFO("		0x40e : STATS_InDaUnknown .\n");
		CLI_INFO("		0x40f : STATS_InMGMT .\n");
		CLI_INFO("		0x410 : STATS_OutQueue0 .\n");
		CLI_INFO("		0x411 : STATS_OutQueue1 .\n");
		CLI_INFO("		0x412 : STATS_OutQueue2 .\n");
		CLI_INFO("		0x413 : STATS_OutQueue3 .\n");
		CLI_INFO("		0x414 : STATS_OutQueue4 .\n");
		CLI_INFO("		0x415 : STATS_OutQueue5 .\n");
		CLI_INFO("		0x416 : STATS_OutQueue6 .\n");
		CLI_INFO("		0x417 : STATS_OutQueue7 .\n");
		CLI_INFO("		0x418 : STATS_OutCutThrough .\n");
		CLI_INFO("		0x419 : STATS_OutOctetsA .\n");
		CLI_INFO("		0x41a : STATS_OutOctetsB .\n");
		CLI_INFO("		0x41b : STATS_OutYel .\n");
		CLI_INFO("		0x41c : STATS_OutDroppedYel .\n");
		CLI_INFO("		0x41d : STATS_OutDiscards .\n");
		CLI_INFO("		0x41e : STATS_OutMGMT .\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        mib getPortCounter 0 0\n");

	}
	else if (strIcmp(cmd, "getPortAllCounter") == 0)
	{
		CLI_INFO("	port : logical port number. \n");		
		CLI_INFO("Example:\n");
		CLI_INFO("        mib getPortAllCounter 0\n");

	}
	else if (strIcmp(cmd, "getHistogramMode") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        mib getHistogramMode\n");
	}
	else if (strIcmp(cmd, "setHistogramMode") == 0)
	{
		CLI_INFO("	mode: Histogram Mode\n");
		CLI_INFO("		1: MSD_COUNT_RX_ONLY\n");
		CLI_INFO("		2: MSD_COUNT_TX_ONLY\n");
		CLI_INFO("		3: MSD_COUNT_RX_TX\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        mib setHistogramMode 1\n");
	}
    else if (strIcmp(cmd, "dump") == 0)
    {
        CLI_INFO("	port : logical port number. \n");
        CLI_INFO("Example:\n");
        CLI_INFO("        mib dump 0\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int mibHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		mibParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (mibCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", mibCMDList[index].help);
		index++;
	}

	return 0;
}
int mibFlushAll()
{
	int index = 0;
	MSD_STATUS retVal;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}
    retVal = msdStatsFlushAll(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int mibFlushPort()
{
	int index = 0;
	MSD_STATUS retVal;
	MSD_LPORT port;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdStatsFlushPort(sohoDevNum, port);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
	}
	return 0;
}
int mibGetPortCounter()
{
	int index = 0;
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_STATS_COUNTERS counter;
	MSD_U32 statsData;
	
	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	counter = (MSD_STATS_COUNTERS)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdStatsPortCounterGet(sohoDevNum, port, counter, &statsData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
	}
	CLI_INFO("StatsData : 0x%lX\n", statsData);
	return 0;
}
int mibGetPortAllCounters()
{
	int index = 0;
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_STATS_COUNTER_SET    statsCounterSet;
	memset(&statsCounterSet, 0, sizeof(statsCounterSet));

    if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}
	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdStatsPortAllCountersGet(sohoDevNum, port, &statsCounterSet);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
	}
	displayCounter(&statsCounterSet, MSD_TRUE);
	return 0;
}
int mibOperation()
{
    int index = 0;
    while (1)
    {
        if (mibCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], mibCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(mibCMDList[index]);
            mibCMDList[index].callback();
            break;
        }
        index++;
    }
    if (mibCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"mib help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static void printQosWeightOptions()
{
    CLI_INFO("	[options](default value for option item is 0):\n");
    CLI_INFO("	options for Qos weight, format as: -len 1 -queue {4 3 2 1}\n");
    CLI_INFO("	-len		: length of the valid queue data\n");
    CLI_INFO("	-queue   	: array of queue\n");
}
static void  queueCtrlParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, queueCtrlCMDList));

	if (strIcmp(cmd, "setQc") == 0)
	{
		CLI_INFO("	port  : logical port number. \n");
		CLI_INFO("	point : Pointer to the Queue control register. \n");
		CLI_INFO("	data  : Queue Control data written to the register pointed to by the point above. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qc setQc 0 0 1\n");
	}
	else if (strIcmp(cmd, "getQc") == 0)
	{
		CLI_INFO("	port  : logical port number. \n");
		CLI_INFO("	point : Pointer to the Queue control register. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qc getQc 0 0\n");

	}
	else if (strIcmp(cmd, "setPortSched") == 0)
	{
		CLI_INFO("	port  : logical port number. \n");
		CLI_INFO("	mode  : \n");
		CLI_INFO("		0 : MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0.\n");
		CLI_INFO("		1 : MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0.\n");
		CLI_INFO("		2 : MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0.\n");
		CLI_INFO("		3 : MSD_PORT_SCHED_WRR_PRI4_3_2_1_0.\n");
		CLI_INFO("		4 : MSD_PORT_SCHED_WRR_PRI3_2_1_0.\n");
		CLI_INFO("		5 : MSD_PORT_SCHED_WRR_PRI2_1_0.\n");
		CLI_INFO("		6 : MSD_PORT_SCHED_WRR_PRI1_0.\n");
		CLI_INFO("		7 : MSD_PORT_SCHED_STRICT_PRI_ALL.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qc setPortSched 0 1\n");
	}
	else if (strIcmp(cmd, "getPortSched") == 0)
	{
		CLI_INFO("	port : logical port number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qc getPortSched 0\n");

	}
    else if (strIcmp(cmd, "setQosWeight") == 0)
    {
        printQosWeightOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        qc setQosWeight -len 1 -queue {4 3 2 1}\n");
    }
    else if (strIcmp(cmd, "getQosWeight") == 0)
    {
        CLI_INFO("qc getQosWeight : get the round robin weights\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        qc getQosWeight\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
static int updateQosWeight(MSD_QoS_WEIGHT* weight, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; )
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        
        if (strIcmp(cmd, "-len") == 0)
        {
            weight->len = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-queue") == 0)
        {
            int i;
            unsigned int tempList[MSD_MAX_QOS_WEIGHTS] = { 0 };
            if (updateArray(&index, value, MSD_MAX_QOS_WEIGHTS, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < MSD_MAX_QOS_WEIGHTS; i++)
            {
                weight->queue[i] = (MSD_U8)tempList[i];
            }
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutWeight(MSD_QoS_WEIGHT* weight)
{
    int i;
    char tempStr[500] = { 0 };

    CLI_INFO("Qos Weight Info:\n");
    CLI_INFO("	len: %X\n", weight->len);
    //CLI_INFO("	queue:	");
    sprintf(tempStr, "	queue:	");
    for (i = 0; i < (int)weight->len; i++)
    {
        //CLI_INFO("%X ", weight->queue[i]);
        sprintf(tempStr + strlen(tempStr), "%X ", weight->queue[i]);
    }
    //CLI_INFO("\n");
    sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
}
int qcSetQosWeight()
{
    MSD_STATUS retVal;
    MSD_QoS_WEIGHT weight;

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));
    if (updateQosWeight(&weight, 2) != 0)
        return-1;

    retVal = msdSysQoSWeightSet(sohoDevNum, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int qcGetQosWeight()
{
    MSD_STATUS retVal;
    MSD_QoS_WEIGHT weight;

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));

    retVal = msdSysQoSWeightGet(sohoDevNum, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutWeight(&weight);

    return retVal;
}
int queueCtrlHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		queueCtrlParaHelp(CMD_ARGS[2]);
		return 0;
	}
	while (1)
	{
		if (queueCtrlCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", queueCtrlCMDList[index].help);
		index++;
	}

	return 0;
}
int queueCtrlOperation()
{
    int index = 0;
    while (1)
    {
        if (queueCtrlCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], queueCtrlCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(queueCtrlCMDList[index]);
            queueCtrlCMDList[index].callback();
            break;
        }
        index++;
    }
    if (queueCtrlCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"qc help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static void trunkParaHelp(char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, trunkCMDList));

    if (strIcmp(cmd, "setTrunkMask") == 0)
    {
        CLI_INFO("	maskNum   : Mask Number. \n");
        CLI_INFO("	trunkMask : Trunk mask bits. Bit 0 controls trunk masking for port 0, bit 1 for port 1 , etc.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk setTrunkMask 1 1\n");
    }
    else if (strIcmp(cmd, "getTrunkMask") == 0)
    {
        CLI_INFO("	maskNum   : Mask Number. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk getTrunkMask 1\n");
    }
    else if (strIcmp(cmd, "setTrunkHash") == 0)
    {
        CLI_INFO("	en : MSD_TRUE to use lookup table, MSD_FALSE to use XOR. \n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk setTrunkHash 1\n");
    }
    else if (strIcmp(cmd, "getTrunkHash") == 0)
    {
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk getTrunkHash\n");

    }
    else if (strIcmp(cmd, "setTrunkRoute") == 0)
    {
        CLI_INFO("	trunkId : Trunk ID.\n");
        CLI_INFO("	trunkRoute   : Trunk route bits. Bit 0 controls trunk routing for port 0, bit 1 for port 1 , etc.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk setTrunkRoute 1 1\n");
    }
    else if (strIcmp(cmd, "getTrunkRoute") == 0)
    {
        CLI_INFO("	trunkId : Trunk ID.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk getTrunkRoute 1\n");
    }
    else if (strIcmp(cmd, "setLAGPort") == 0)
    {
        CLI_INFO("	portNum : the logical port number.\n");
        CLI_INFO("	en      : 1 for the LAG port, 0 otherwise\n");
        CLI_INFO("	LAGId   : 0 ~ 31, valid only if en is MSD_TRUE\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk setLAGPort 1 1 1\n");
    }
    else if (strIcmp(cmd, "getLAGPort") == 0)
    {
        CLI_INFO("	portNum : the logical port number.\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        trunk getLAGPort 1\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int trunkHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        trunkParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (trunkCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", trunkCMDList[index].help);
        index++;
    }

    return 0;
}
int trunkOperation()
{
    int index = 0;
    while (1)
    {
        if (trunkCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], trunkCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(trunkCMDList[index]);
            trunkCMDList[index].callback();
            break;
        }
        index++;
    }
    if (trunkCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"trunk help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static void  ledParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, ledCMDList));
    if (strIcmp(cmd, "forceOn") == 0)
    {
        CLI_INFO("	port   : the logical port number.\n");
        CLI_INFO("	ledNum : LED index\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        LED forceOn 1 1\n");
    }
    else if (strIcmp(cmd, "forceOff") == 0)
    {
        CLI_INFO("	port   : the logical port number.\n");
        CLI_INFO("	ledNum : LED index\n");
		CLI_INFO("Example:\n");
		CLI_INFO("         LED forceOff 1 1\n");
    }
    else if (strIcmp(cmd, "setLinkAct") == 0)
    {
        CLI_INFO("	port   : the logical port number.\n");
        CLI_INFO("	ledNum : LED index\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        LED setLinkAct 1 1\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
int ledHelp()
{
    int index = 0;
    if (nargs >= 3)
    {
        ledParaHelp(CMD_ARGS[2]);
        return 0;
    }
    while (1)
    {
        if (ledCMDList[index].callback == NULL)
            break;
        CLI_INFO("%s", ledCMDList[index].help);
        index++;
    }

    return 0;
}
int ledOperation()
{
    int index = 0;
    while (1)
    {
        if (ledCMDList[index].callback == NULL)
            break;
        if (strIcmp(CMD_ARGS[1], ledCMDList[index].cmd) == 0)
        {
            gCurrentCmd = &(ledCMDList[index]);
            ledCMDList[index].callback();
            break;
        }
        index++;
    }
    if (ledCMDList[index].callback == NULL)
        CLI_ERROR("Invalid command %s %s. Use \"trunk help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
    return 0;
}

static void rmuParaHelp(const char* cmd)
{
    CLI_INFO("%s\n", syntaxCmd(cmd, rmuCMDList));
	if (strIcmp(cmd, "getID") == 0)
	{
		CLI_INFO("Example:\n");
		CLI_INFO("        rmu getID\n");
	}
	else if (strIcmp(cmd, "dumpAtu") == 0)
	{
		CLI_INFO("	continueCode : seaching pointer for ATU dump, using 0 to start at the beginning of the ATU's memory\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        rmu dumpAtu 0\n");
	}
    else if (strIcmp(cmd, "dumpMib") == 0)
    {
        CLI_INFO("	enClearOnRead : indicate dump mib clear or not\n");
        CLI_INFO("	port : request port\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        rmu dumpMib 0 0\n");
    }
	else if (strIcmp(cmd, "dumpMib2") == 0)
	{
		CLI_INFO("	enClearOnRead : indicate dump mib clear or not\n");
		CLI_INFO("	port : request port\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        rmu dumpMib2 0 0\n");
	}
	else if (strIcmp(cmd, "dumpTcam") == 0)
	{
		CLI_INFO("	continueCode : seaching pointer for TCAM dump, logic like the GetNex operation.\n");
        CLI_INFO("	                using 0xff(Peridot) or 0x3f(Topaz) to start at the beginning of the memery\n");
        CLI_INFO("Example:\n");
		CLI_INFO("        rmu dumpTcam 0xff (Peridot)\n");
	}
	else if (strIcmp(cmd, "tcamRead") == 0)
	{
		CLI_INFO("	entryNum : tcam entry number\n");
		CLI_INFO("Example:\n");
		CLI_INFO("        rmu tcamRead 0\n");
	}
	else if (strIcmp(cmd, "tcamWrite") == 0)
	{
		CLI_INFO("	entryNum : tcam entry number\n");
		printTcamOptions();
		CLI_INFO("Example:\n");
		CLI_INFO("        rmu tcamWrite 0 -frameType 0 -frameTypeMask 3 -spv 3 -spvMask 4 \n");
	}
    else if (strIcmp(cmd, "dumpReg") == 0)
    {
        CLI_INFO("	devAddr : device address\n");
        CLI_INFO("Example:\n");
        CLI_INFO("        rmu dumpReg 0x10\n");
    }
	else
	{
		CLI_ERROR("Can not find the command: %s\n", cmd);
	}
}
static MSD_STATUS checkRMUInterface()
{
    MSD_STATUS retVal;
    MSD_INTERFACE channel;

    retVal = msdGetDriverInterface(sohoDevNum, &channel);
    if (retVal != MSD_OK) {
        CLI_ERROR("Error get dirver bus interface ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (channel != MSD_INTERFACE_RMU)
        return MSD_FAIL;

    return retVal;
}
int rmuGetID()
{
	MSD_STATUS retVal;
	MSD_U16 id;

	if (nargs != 2)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	retVal = msdRMUGetID(sohoDevNum, &id);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error get the devcie ID ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("Get out Device ID: %X\n", id);
	return 0;
}
int rmuDumpAtu()
{
	MSD_STATUS retVal;
	MSD_U32 startAddr;
	int i;
	MSD_U32 numOfEntry = 48;
	MSD_ATU_ENTRY atuEntry[48];
	MSD_ATU_ENTRY *pAtuEntry = &(atuEntry[0]);

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	memset(pAtuEntry, 0, sizeof(MSD_ATU_ENTRY)*numOfEntry);
	startAddr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdRMUAtuDump(sohoDevNum, &startAddr, &numOfEntry, &pAtuEntry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error dump ATU ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	for (i = 0; i < (int)numOfEntry; i++)
	{
		printOutFdbEntry(pAtuEntry + i);
	}
    CLI_INFO("There are %d entries found in this searching\n", numOfEntry);
	CLI_INFO("ATU searching continue code: 0x%lX\n", startAddr);
	return 0;
}
int rmuDumpMib()
{
	MSD_STATUS retVal;
	MSD_BOOL enClearOnRead;
	MSD_LPORT port;
	MSD_STATS_COUNTER_SET_LEGACY statsCounterSet;

	if (nargs != 4)
	{
		CLI_ERROR("Syntax Error: rmu dumpMib <enClearOnRead> <port> : Dump MIB from the specified port\n");
		return 1;
	}

	memset(&statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET_LEGACY));
	enClearOnRead = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdRMUMibDump(sohoDevNum, enClearOnRead, port, &statsCounterSet);
	if (retVal != MSD_OK) {
	    if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error dump MIB ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	displayCounter(&statsCounterSet, MSD_FALSE);

	return 0;
}
int rmuDumpMib2()
{
	MSD_STATUS retVal;
	MSD_BOOL enClearOnRead;
	MSD_LPORT port;
	MSD_STATS_COUNTER_SET statsCounterSet;

	if (nargs != 4)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	memset(&statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));
	enClearOnRead = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

	retVal = msdRMUMib2Dump(sohoDevNum, enClearOnRead, port, &statsCounterSet);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error dump MIB ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	displayCounter(&statsCounterSet, MSD_TRUE);

	return 0;
}
int rmuDumpReg()
{
    MSD_STATUS retVal;
    MSD_U8 devAddr;
    MSD_RegList dataList;
    int i;

    if (nargs != 3)
    {
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(&dataList, 0, sizeof(MSD_RegList));
    devAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMURegisterDump(sohoDevNum, devAddr, &dataList);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump Tcam ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("registers value for devAddr%d:\n", devAddr);
    for (i = 0; i < 32; i++)
    {
        CLI_INFO("reg%02d = 0x%04x\n", i, dataList.data[i]);
    }

    return 0;
}
int rmuDumpTcam()
{
	MSD_STATUS retVal;
	MSD_U32 startAddr;
	int i;
	MSD_U32 numOfEntry = 3;
	MSD_TCAM_DATA tcamEntry[3];
	MSD_TCAM_DATA *pTcamEntry = &(tcamEntry[0]);

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	memset(pTcamEntry, 0, sizeof(MSD_TCAM_DATA)*numOfEntry);
	startAddr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdRMUTcamDump(sohoDevNum, &startAddr, &numOfEntry, &pTcamEntry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error dump Tcam ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	for (i = 0; i < (int)numOfEntry; i++)
	{
		displayTcamData(pTcamEntry + i);
	}
    CLI_INFO("There are %d entries found in this searching\n", numOfEntry);
	CLI_INFO("TCAM searching continue code: 0x%lX\n", startAddr);
	return 0;
}
int rmuTcamRead()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;

	if (nargs != 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdRMUTcamEntryRead(sohoDevNum, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	displayTcamData(&tcamData);
	return 0;
}
int rmuTcamWrite()
{
	MSD_STATUS retVal;
	MSD_U32 tcamPointer;
	MSD_TCAM_DATA tcamData;

	if (nargs < 3)
	{
        if (gCurrentCmd != NULL)
            CLI_ERROR("Syntax Error, Using command as follows: %s", gCurrentCmd->help);
        return 1;
	}

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

	memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateTcamData(&tcamData, 3) != 0)
        return -1;

	retVal = msdRMUTcamEntryAdd(sohoDevNum, tcamPointer, &tcamData);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int rmuHelp()
{
	int index = 0;
	if (nargs >= 3)
	{
		rmuParaHelp(CMD_ARGS[2]);
		return 0;
	}

	while (1)
	{
		if (rmuCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", rmuCMDList[index].help);
		index++;
	}

	return 0;
}
int rmuOperation()
{
	int index = 0;
	while (1)
	{
		if (rmuCMDList[index].callback == NULL)
			break;
		if (strIcmp(CMD_ARGS[1], rmuCMDList[index].cmd) == 0)
		{
            gCurrentCmd = &(rmuCMDList[index]);
			rmuCMDList[index].callback();
			break;
		}
		index++;
	}
	if (rmuCMDList[index].callback == NULL)
		CLI_ERROR("Invalid command %s %s. Use \"rmu help [subcmd]\" to see the option\n", CMD_ARGS[0], CMD_ARGS[1]);
	return 0;
}

int apiCLIHelp()
{
	int index = 0;

	while (1)
	{
		if (cliCMDList[index].callback == NULL)
			break;
		CLI_INFO("%s", cliCMDList[index].help);
		index++;
	}

	return 0;
}
int execSystemCmd()
{
	char sys_cmd[MAX_CMD_LENGTH] = "";
	int rval;
	int i;

	for (i = 1; i<nargs; i++){
		strcat(sys_cmd, CMD_ARGS[i]);
		strcat(sys_cmd, " ");
	}

	rval = system(sys_cmd);

	if (rval == -1){
		CLI_INFO("Failed to execute \"%s\"\n", sys_cmd);
	}
	return 0;
}
int exitProgram()
{
	exit_flag = 1;
	return 0;
}

int parseCmd(char *cmd){
	char line[5000] = {0};
	char *token;
	int count = 0;
	int i;
	
	for (i = 0; i<MAX_ARGS; i++)
		memset(CMD_ARGS[i], 0, sizeof(CMD_ARGS[i]));
	strcpy(line, cmd);
	token = strtok(line, " \t\n");
	count = 0;
	while (token != NULL){
		strcpy(CMD_ARGS[count], token);
		token = strtok(NULL, " \t\n");
		count++;
	}

	nargs = count;
	return count;
}
int parseCmd_cli(const char *command, char *argv[], int argc)
{
    char line[50] = { 0 };
    char *token;
    int count = 0;
    int i; 

    for (i = 0; i<MAX_ARGS; i++)
        memset(CMD_ARGS[i], 0, sizeof(CMD_ARGS[i]));

    strcpy(line, command);
    token = strtok(line, " \t\n");
    count = 0;
    while (token != NULL){
        strcpy(CMD_ARGS[count], token);
        token = strtok(NULL, " \t\n");
        count++;
    }
    for (i = 0; i < argc; i++)
    {
        strcpy(CMD_ARGS[count], argv[i]);
        count++;
    }

    nargs = count;
    return count;
}
int runCommand(FILE* fp, char* prompt)
{
	int i, rval = 0;
	char *buf = NULL;
	int len = 0;

	buf = (char *)malloc(MAX_CMD_LENGTH*sizeof(char));
	if (buf == NULL){
		CLI_ERROR("Error: Cannot allocate memory\n");
		return -1;
	}
	len = (int)MAX_CMD_LENGTH;
	memset(buf, 0, MAX_CMD_LENGTH*sizeof(char));

	while (fgets(buf, MAX_CMD_LENGTH, fp) != NULL)
	{
		exit_flag = 0;
		nargs = parseCmd(buf);
        g_filename = strstr(buf, "\"");

		if (nargs == 0 && prompt_cnt == 0){
			printf("%s", prompt);
			continue;
		}

		if (*(CMD_ARGS[0]) != '#')
		{
			int index = 0;
			while (1)
			{
				if (cliCMDList[index].callback == NULL)
				{
					CLI_CMD_TRACE(buf);
					break;
				}
				if (strIcmp(CMD_ARGS[0], cliCMDList[index].cmd) == 0)
				{
					CLI_CMD_TRACE(buf);
					cliCMDList[index].callback();
					break;
				}
				index++;
			}
			if (cliCMDList[index].callback == NULL)
				CLI_ERROR("Invalid command %s. Use \"help\" to see the option\n", CMD_ARGS[0]);
		}

		if (exit_flag == 1)
			break;

		memset(buf, 0, MAX_CMD_LENGTH);

		for (i = 0; i<MAX_ARGS; i++)
			memset(CMD_ARGS[i], 0, sizeof(CMD_ARGS[i]));

		if (prompt_cnt == 0)
			printf("%s", prompt);
	}

	if (buf)
		free(buf);
	return rval;
}
void apiCLI()
{
	int len = 0;
	int i, rval = 0;
	FILE *fp = NULL;
	char *prompt = PROMPT_STRING;

	for (i = 0; i<MAX_ARGS; i++){
		CMD_ARGS[i] = (char *)malloc(MAX_ARGS_LENGTH*sizeof(char));
		memset(CMD_ARGS[i], 0, MAX_ARGS_LENGTH*sizeof(char));
	}

	fp = stdin;

	if (prompt_cnt == 0)
		printf("%s", prompt);

	rval = runCommand(fp, prompt);

	for (i = 0; i<MAX_ARGS; i++){
		free(CMD_ARGS[i]);
	}
}

void apiCLI_alloc_buf()
{
	int i;
	for (i = 0; i<MAX_ARGS; i++){
		CMD_ARGS[i] = (char *)malloc(MAX_ARGS_LENGTH*sizeof(char));
		memset(CMD_ARGS[i], 0, MAX_ARGS_LENGTH*sizeof(char));
	}
}
void apiCLI_free_buf()
{
	int i;
	for (i = 0; i<MAX_ARGS; i++){
		free(CMD_ARGS[i]);
	}
}