#include <msdCopyright.h>

/********************************************************************************
* msdSysConfig.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <msdSysConfig.h>
#include <msdDevObj.h>
#include <msdSem.h>
#include <msdUtils.h>
#include <msdHwAccess.h>
#ifdef TOPAZ_BUILD_IN
#include <topaz/include/Topaz_msdApi.h>
#endif
#ifdef PERIDOT_BUILD_IN
#include <peridot/include/Peridot_msdApi.h>
#endif
#ifdef PEARL_BUILD_IN
#include <pearl/include/Pearl_msdApi.h>
#endif
#ifdef AGATE_BUILD_IN
#include <agate/include/Agate_msdApi.h>
#endif

static MSD_STATUS InitDevObj_Peridot(MSD_QD_DEV *dev)
{
#ifdef PERIDOT_BUILD_IN
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = Peridot_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = Peridot_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = Peridot_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = Peridot_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = Peridot_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = Peridot_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = Peridot_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = Peridot_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = Peridot_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = Peridot_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = Peridot_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = Peridot_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = Peridot_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = Peridot_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = Peridot_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = Peridot_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = Peridot_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = Peridot_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = Peridot_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = Peridot_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = Peridot_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = Peridot_gfdbGetNonStaticEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = Peridot_gfdbDump;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = Peridot_gprtSetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = Peridot_gprtGetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = Peridot_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = Peridot_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = Peridot_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = Peridot_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = Peridot_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = Peridot_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = Peridot_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = Peridot_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = Peridot_gprtSetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = Peridot_gprtGetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = Peridot_gprtSetRxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = Peridot_gprtGetRxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = Peridot_gprtSetRxPriorityFlowControlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = Peridot_gprtGetRxPriorityFlowControlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = Peridot_gprtSetTxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = Peridot_gprtGetTxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = Peridot_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = Peridot_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = Peridot_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = Peridot_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = Peridot_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = Peridot_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = Peridot_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = Peridot_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = Peridot_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = Peridot_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = Peridot_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = Peridot_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = Peridot_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = Peridot_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = Peridot_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = Peridot_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = Peridot_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = Peridot_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = Peridot_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = Peridot_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = Peridot_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = Peridot_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = Peridot_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = Peridot_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = Peridot_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = Peridot_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = Peridot_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = Peridot_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = Peridot_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = Peridot_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = Peridot_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = Peridot_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = Peridot_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = Peridot_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = Peridot_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = Peridot_gvtuDump;
	
    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = Peridot_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = Peridot_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = Peridot_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = Peridot_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = Peridot_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = Peridot_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = Peridot_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = Peridot_gsysGetTrunkRouting;
	
	dev->SwitchDevObj.IMPObj.impRun = Peridot_impRunIntf;
	dev->SwitchDevObj.IMPObj.impReset = Peridot_impResetIntf;
	dev->SwitchDevObj.IMPObj.impWriteEEPROM = Peridot_impWriteEEPROMIntf;
	dev->SwitchDevObj.IMPObj.impLoadToEEPROM = Peridot_impLoadToEEPROMIntf;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = Peridot_impLoadToRAMIntf;
	dev->SwitchDevObj.IMPObj.impStop = Peridot_impStopIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = Peridot_gphyReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = Peridot_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = Peridot_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = Peridot_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = Peridot_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = Peridot_gphySetPortAutoModeIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = Peridot_gphySetEEE;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = Peridot_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource=Peridot_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup=Peridot_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource=Peridot_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource=Peridot_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg=Peridot_gpirlGetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = Peridot_gpirlSetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = Peridot_gpirlAdvConfigIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = Peridot_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = Peridot_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = Peridot_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode=Peridot_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode=Peridot_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged=Peridot_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged=Peridot_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged=Peridot_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged=Peridot_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock=Peridot_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock=Peridot_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock=Peridot_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock=Peridot_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode=Peridot_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode=Peridot_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = Peridot_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = Peridot_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = Peridot_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = Peridot_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = Peridot_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = Peridot_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode=Peridot_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode=Peridot_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable=Peridot_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = Peridot_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = Peridot_gprtSetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = Peridot_gprtGetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = Peridot_gprtSetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = Peridot_gprtGetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = Peridot_gprtSetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = Peridot_gprtGetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = Peridot_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = Peridot_gprtGetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = Peridot_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = Peridot_gprtGetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = Peridot_gprtSetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = Peridot_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl=Peridot_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl=Peridot_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource=Peridot_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource=Peridot_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource=Peridot_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource=Peridot_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate=Peridot_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate=Peridot_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = Peridot_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = Peridot_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = Peridot_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = Peridot_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = Peridot_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = Peridot_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = Peridot_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = Peridot_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = Peridot_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = Peridot_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = Peridot_gprtGetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = Peridot_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = Peridot_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = Peridot_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = Peridot_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = Peridot_gprtRegDump;

    dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = Peridot_gprtGetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = Peridot_gprtSetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = Peridot_gqosGetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = Peridot_gprtGetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = Peridot_gprtSetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = Peridot_gprtGetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = Peridot_gprtSetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = Peridot_gprtGetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = Peridot_gprtSetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = Peridot_gqosSetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = Peridot_gprtRewriteDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = Peridot_gprtSetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = Peridot_gprtGetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = Peridot_gprtSetCosToDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = Peridot_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = Peridot_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = Peridot_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter=Peridot_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters=Peridot_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode=Peridot_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode=Peridot_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = Peridot_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = Peridot_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl=Peridot_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched=Peridot_gprtSetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = Peridot_gprtGetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = Peridot_gsysSetQoSWeightIntf;
    dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = Peridot_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = Peridot_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = Peridot_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = Peridot_gsysSetMonitorMgmtCtrl;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = Peridot_gsysGetMonitorMgmtCtrl;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = Peridot_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = Peridot_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = Peridot_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = Peridot_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = Peridot_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = Peridot_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = Peridot_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = Peridot_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = Peridot_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = Peridot_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = Peridot_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = Peridot_gsysGlobal2RegDump;

    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = Peridot_gqavGetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = Peridot_gqavSetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = Peridot_gqavGetPortQpriXRate;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = Peridot_gqavSetPortQpriXRate;

    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = Peridot_gptpSetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = Peridot_gptpGetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = Peridot_gptpSetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = Peridot_gptpGetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpSetPortConfig = Peridot_gptpSetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPortConfig = Peridot_gptpGetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = Peridot_gptpGetTimeStampIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = Peridot_gptpGetTSValidSt;
    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = Peridot_gptpResetTimeStamp;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = Peridot_gptpSetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = Peridot_gptpGetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = Peridot_gptpGetIntStatusIntf;
    dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = Peridot_gptpSetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = Peridot_gptpGetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = Peridot_gptpSetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = Peridot_gptpGetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = Peridot_gptpSetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = Peridot_gptpGetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpSetMode = Peridot_gptpSetModeIntf;
    dev->SwitchDevObj.PTPObj.gptpGetMode = Peridot_gptpGetModeIntf;
    dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = Peridot_gptpSetMasterSlaveIntf;
    dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = Peridot_gptpGetMasterSlaveIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = Peridot_gptpGetTODBusyBitStatus;
    dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = Peridot_gptpTODStoreCompensationIntf;
    dev->SwitchDevObj.PTPObj.gptpTODStoreAll = Peridot_gptpTODStoreAllIntf;
    dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = Peridot_gptpTODCaptureAllIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPulse = Peridot_gptpSetPulseIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPulse = Peridot_gptpGetPulseIntf;
    dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = Peridot_gtaiGetPTPGlobalTime;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = Peridot_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = Peridot_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = Peridot_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = Peridot_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = Peridot_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = Peridot_gtcamFindEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = Peridot_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = Peridot_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = Peridot_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = Peridot_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = Peridot_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = Peridot_gtcamEgrFlushEntryAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = Peridot_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = Peridot_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = Peridot_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = Peridot_gtcamDump;
    dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = Peridot_gtcamAdvConfigIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = Peridot_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = Peridot_msdRmuAtuDumpIntf;	
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = Peridot_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = Peridot_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = Peridot_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = Peridot_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = Peridot_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = Peridot_msdRmuRegDump;

    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = Peridot_msdGetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = Peridot_msdSetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = Peridot_msdGetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = Peridot_msdSetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = Peridot_msdSetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = Peridot_msdGetSMIPhyXMDIOReg;

    dev->SwitchDevObj.LEDObj.gledForceOn = Peridot_gledForceOn;
    dev->SwitchDevObj.LEDObj.gledForceOff = Peridot_gledForceOff;
    dev->SwitchDevObj.LEDObj.gledLinkAct = Peridot_gledLinkAct;
#endif
    return MSD_OK;
}

static MSD_STATUS InitDevObj_Topaz(MSD_QD_DEV *dev)
{
#ifdef TOPAZ_BUILD_IN
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = Topaz_gfdbAddMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = Topaz_gfdbGetAtuEntryNextIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlush = Topaz_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = Topaz_gfdbFlushInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbMove = Topaz_gfdbMoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbMoveInDB = Topaz_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = Topaz_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = Topaz_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = Topaz_gfdbDelAtuEntry;
    dev->SwitchDevObj.ATUObj.gfdbGetViolation = Topaz_gfdbGetViolationIntf;
    dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = Topaz_gfdbFindAtuMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = Topaz_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = Topaz_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = Topaz_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = Topaz_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = Topaz_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = Topaz_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = Topaz_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = Topaz_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = Topaz_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = Topaz_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = Topaz_gfdbGetNonStaticEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = Topaz_gfdbDump;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = Topaz_gprtSetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = Topaz_gprtGetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = Topaz_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = Topaz_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = Topaz_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = Topaz_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = Topaz_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = Topaz_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = Topaz_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = Topaz_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = Topaz_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = Topaz_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = Topaz_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = Topaz_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = Topaz_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = Topaz_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = Topaz_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = Topaz_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = Topaz_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = Topaz_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = Topaz_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = Topaz_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = Topaz_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = Topaz_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = Topaz_gprtGetARPMirror;

    dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = Topaz_gvctGetAdvCableDiagIntf;
    dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = Topaz_gvctGetAdvExtendedStatusIntf;

    dev->SwitchDevObj.STUObj.gstuAddEntry = Topaz_gstuAddEntryIntf;
    dev->SwitchDevObj.STUObj.gstuDelEntry = Topaz_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = Topaz_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = Topaz_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = Topaz_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = Topaz_gstuDump;

    dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = Topaz_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = Topaz_gvtuExistVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuFlush = Topaz_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = Topaz_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = Topaz_gvtuGetEntryNextIntf;
    dev->SwitchDevObj.VTUObj.gvtuDelEntry = Topaz_gvtuDelEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetViolation = Topaz_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = Topaz_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = Topaz_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = Topaz_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = Topaz_gvtuDump;

    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = Topaz_gprtGetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = Topaz_gprtGetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = Topaz_gprtSetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = Topaz_gprtSetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = Topaz_gprtGetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = Topaz_gqosGetPrioMapRule;
    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = Topaz_gprtSetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = Topaz_gqosSetPrioMapRuleIntf;

    dev->SwitchDevObj.IMPObj.impRun = Topaz_impRunIntf;
    dev->SwitchDevObj.IMPObj.impReset = Topaz_impResetIntf;
    dev->SwitchDevObj.IMPObj.impWriteEEPROM = Topaz_impWriteEEPROMIntf;
    dev->SwitchDevObj.IMPObj.impLoadToEEPROM = Topaz_impLoadToEEPROMIntf;
    dev->SwitchDevObj.IMPObj.impLoadToRAM = Topaz_impLoadToRAMIntf;
    dev->SwitchDevObj.IMPObj.impStop = Topaz_impStopIntf;

    dev->SwitchDevObj.PHYCTRLObj.gphyReset = Topaz_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = Topaz_gphySetPortLoopback;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = Topaz_gphySetPortSpeedIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = Topaz_gphyPortPowerDown;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = Topaz_gphySetPortDuplexMode;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = Topaz_gphySetPortAutoModeIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = Topaz_gphySetEEE;

    dev->SwitchDevObj.PIRLObj.gpirlInitialize = Topaz_gpirlInitializeIntf;
    dev->SwitchDevObj.PIRLObj.gpirlInitResource = Topaz_gpirlInitResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = Topaz_gpirlCustomSetupIntf;
    dev->SwitchDevObj.PIRLObj.gpirlReadResource = Topaz_gpirlReadResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlWriteResource = Topaz_gpirlWriteResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlGetResReg = Topaz_gpirlGetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = Topaz_gpirlSetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = Topaz_gpirlAdvConfigIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = Topaz_gpirlAdvConfigDisableIntf;

    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = Topaz_gvlnSetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = Topaz_gvlnGetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = Topaz_gvlnSetPortVlanDot1qModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = Topaz_gvlnGetPortVlanDot1qModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = Topaz_gprtSetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = Topaz_gprtGetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = Topaz_gprtSetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = Topaz_gprtGetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = Topaz_gprtSetUnicastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = Topaz_gprtGetUnicastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = Topaz_gprtSetMulticastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = Topaz_gprtGetMulticastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = Topaz_gprtSetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = Topaz_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = Topaz_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = Topaz_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = Topaz_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = Topaz_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = Topaz_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = Topaz_gprtGetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = Topaz_gprtSetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = Topaz_gprtGetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = Topaz_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = Topaz_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = Topaz_gprtSetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = Topaz_gprtGetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = Topaz_gprtSetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = Topaz_gprtGetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = Topaz_gprtSetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = Topaz_gprtGetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = Topaz_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = Topaz_gprtGetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = Topaz_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = Topaz_gprtGetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = Topaz_gprtSetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = Topaz_gprtGetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = Topaz_gprtSetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = Topaz_gprtGetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = Topaz_gprtSetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = Topaz_gprtGetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = Topaz_grcSetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = Topaz_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = Topaz_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = Topaz_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = Topaz_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = Topaz_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = Topaz_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = Topaz_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = Topaz_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = Topaz_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = Topaz_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = Topaz_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = Topaz_gprtGetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = Topaz_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = Topaz_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = Topaz_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = Topaz_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = Topaz_gprtRegDump;

    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = Topaz_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = Topaz_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = Topaz_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = Topaz_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = Topaz_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = Topaz_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = Topaz_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = Topaz_gsysGetTrunkRouting;

    dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = Topaz_gstatsFlushAll;
    dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = Topaz_gstatsFlushPort;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = Topaz_gstatsGetPortCounterIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = Topaz_gstatsGetPortAllCountersIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = Topaz_gstatsGetHistogramModeIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = Topaz_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = Topaz_gmibDump;

    dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = Topaz_gprtSetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = Topaz_gprtGetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = Topaz_gprtSetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = Topaz_gprtGetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = Topaz_gsysSetQoSWeightIntf;
    dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = Topaz_gsysGetQoSWeightIntf;

    dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = Topaz_gsysSetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = Topaz_gsysGetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = Topaz_gsysSetMonitorMgmtCtrl;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = Topaz_gsysGetMonitorMgmtCtrl;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = Topaz_gsysSetCPUDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = Topaz_gsysGetCPUDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = Topaz_gsysSetIngressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = Topaz_gsysGetIngressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = Topaz_gsysSetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = Topaz_gsysGetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = Topaz_gsysSetRsvd2CpuEnables;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = Topaz_gsysGetRsvd2CpuEnables;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = Topaz_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = Topaz_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = Topaz_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = Topaz_gsysGlobal2RegDump;

    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = Topaz_gqavGetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = Topaz_gqavSetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = Topaz_gqavGetPortQpriXRate;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = Topaz_gqavSetPortQpriXRate;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = Topaz_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = Topaz_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = Topaz_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = Topaz_gtcamRemoveEntryPort;

    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = Topaz_gptpSetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = Topaz_gptpGetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = Topaz_gptpSetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = Topaz_gptpGetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpSetPortConfig = Topaz_gptpSetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPortConfig = Topaz_gptpGetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = Topaz_gptpGetTimeStampIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = Topaz_gptpGetTSValidSt;
    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = Topaz_gptpResetTimeStamp;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = Topaz_gptpSetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = Topaz_gptpGetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = Topaz_gptpGetIntStatusIntf;

	if (dev->deviceId != MSD_88E6141)
    {
        dev->SwitchDevObj.TCAMObj.gtcamFindEntry = Topaz_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = Topaz_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = Topaz_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = Topaz_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = Topaz_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = Topaz_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = Topaz_gtcamDump;
        dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = Topaz_gtcamAdvConfigIntf;
	}
	/* Topaz deviceId MSD_88E6141 doesn't support TCAM and AVB */

	dev->SwitchDevObj.RMUObj.grmuGetID = Topaz_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = Topaz_msdRmuAtuDumpIntf;	
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = Topaz_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = Topaz_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = Topaz_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = Topaz_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = Topaz_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = Topaz_msdRmuRegDump;

    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = Topaz_msdGetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = Topaz_msdSetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = Topaz_msdGetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = Topaz_msdSetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = Topaz_msdSetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = Topaz_msdGetSMIPhyXMDIOReg;

    dev->SwitchDevObj.LEDObj.gledForceOn = Topaz_gledForceOn;
    dev->SwitchDevObj.LEDObj.gledForceOff = Topaz_gledForceOff;
    dev->SwitchDevObj.LEDObj.gledLinkAct = Topaz_gledLinkAct;
#endif
    return MSD_OK;
}

static MSD_STATUS InitDevObj_Pearl(MSD_QD_DEV *dev)
{
#ifdef PEARL_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = Pearl_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = Pearl_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = Pearl_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = Pearl_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = Pearl_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = Pearl_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = Pearl_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = Pearl_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = Pearl_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = Pearl_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = Pearl_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = Pearl_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = Pearl_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = Pearl_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = Pearl_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = Pearl_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = Pearl_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = Pearl_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = Pearl_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = Pearl_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = Pearl_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = Pearl_gfdbGetNonStaticEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = Pearl_gfdbDump;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = Pearl_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = Pearl_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = Pearl_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = Pearl_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = Pearl_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = Pearl_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = Pearl_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = Pearl_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = Pearl_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = Pearl_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = Pearl_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = Pearl_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = Pearl_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = Pearl_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = Pearl_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = Pearl_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = Pearl_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = Pearl_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = Pearl_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = Pearl_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = Pearl_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = Pearl_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = Pearl_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = Pearl_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = Pearl_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = Pearl_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = Pearl_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = Pearl_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = Pearl_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = Pearl_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = Pearl_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = Pearl_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = Pearl_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = Pearl_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = Pearl_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = Pearl_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = Pearl_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = Pearl_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = Pearl_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = Pearl_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = Pearl_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = Pearl_gvtuDump;

	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = Pearl_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = Pearl_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = Pearl_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = Pearl_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = Pearl_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = Pearl_gqosGetPrioMapRule;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = Pearl_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = Pearl_gqosSetPrioMapRuleIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = Pearl_gphyReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = Pearl_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = Pearl_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = Pearl_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = Pearl_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = Pearl_gphySetPortAutoModeIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = Pearl_gphySetEEE;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = Pearl_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = Pearl_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = Pearl_gpirlCustomSetupIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = Pearl_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = Pearl_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = Pearl_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = Pearl_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = Pearl_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = Pearl_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = Pearl_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = Pearl_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = Pearl_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = Pearl_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = Pearl_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = Pearl_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = Pearl_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = Pearl_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = Pearl_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = Pearl_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = Pearl_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = Pearl_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = Pearl_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = Pearl_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = Pearl_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = Pearl_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = Pearl_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = Pearl_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = Pearl_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = Pearl_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = Pearl_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = Pearl_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = Pearl_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = Pearl_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = Pearl_gprtSetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = Pearl_gprtGetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = Pearl_gprtSetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = Pearl_gprtGetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = Pearl_gprtSetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = Pearl_gprtGetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = Pearl_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = Pearl_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = Pearl_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = Pearl_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = Pearl_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = Pearl_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = Pearl_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = Pearl_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = Pearl_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = Pearl_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = Pearl_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = Pearl_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = Pearl_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = Pearl_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = Pearl_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = Pearl_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = Pearl_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = Pearl_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = Pearl_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = Pearl_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = Pearl_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = Pearl_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = Pearl_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = Pearl_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = Pearl_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = Pearl_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = Pearl_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = Pearl_gprtRegDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = Pearl_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = Pearl_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = Pearl_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = Pearl_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = Pearl_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = Pearl_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = Pearl_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = Pearl_gsysGetTrunkRouting;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = Pearl_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = Pearl_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = Pearl_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = Pearl_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = Pearl_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = Pearl_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = Pearl_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = Pearl_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = Pearl_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = Pearl_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = Pearl_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = Pearl_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = Pearl_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = Pearl_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = Pearl_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = Pearl_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = Pearl_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = Pearl_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = Pearl_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = Pearl_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = Pearl_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = Pearl_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = Pearl_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = Pearl_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = Pearl_gsysGlobal2RegDump;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = Pearl_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = Pearl_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = Pearl_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = Pearl_gqavSetPortQpriXRate;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = Pearl_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = Pearl_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = Pearl_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = Pearl_gtcamRemoveEntryPort;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = Pearl_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = Pearl_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = Pearl_gptpSetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = Pearl_gptpGetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = Pearl_gptpSetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = Pearl_gptpGetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = Pearl_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = Pearl_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = Pearl_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = Pearl_gptpSetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = Pearl_gptpGetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = Pearl_gptpGetIntStatusIntf;

	if (dev->deviceId != MSD_88E6320)
	{
		dev->SwitchDevObj.TCAMObj.gtcamFindEntry = Pearl_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = Pearl_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = Pearl_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = Pearl_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = Pearl_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = Pearl_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = Pearl_gtcamDump;
		dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = Pearl_gtcamAdvConfigIntf;
		dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = Pearl_gprtSetCutThrouthEnableIntf;
		dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = Pearl_gprtGetCutThrouthEnableIntf;
	}
	/* Pearl deviceId MSD_88E6320 doesn't support TCAM and cut thru */

	dev->SwitchDevObj.RMUObj.grmuGetID = Pearl_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = Pearl_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = Pearl_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = Pearl_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = Pearl_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = Pearl_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = Pearl_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = Pearl_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = Pearl_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = Pearl_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = Pearl_msdGetSMIPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = Pearl_gledForceOn;
	dev->SwitchDevObj.LEDObj.gledForceOff = Pearl_gledForceOff;
#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_Agate(MSD_QD_DEV *dev)
{
#ifdef AGATE_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = Agate_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = Agate_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = Agate_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = Agate_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = Agate_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = Agate_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = Agate_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = Agate_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = Agate_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = Agate_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = Agate_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = Agate_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = Agate_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = Agate_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = Agate_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = Agate_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = Agate_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = Agate_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = Agate_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = Agate_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = Agate_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = Agate_gfdbGetNonStaticEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = Agate_gfdbDump;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = Agate_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = Agate_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = Agate_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = Agate_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = Agate_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = Agate_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = Agate_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = Agate_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = Agate_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = Agate_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = Agate_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = Agate_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = Agate_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = Agate_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = Agate_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = Agate_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = Agate_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = Agate_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = Agate_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = Agate_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = Agate_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = Agate_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = Agate_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = Agate_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = Agate_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = Agate_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = Agate_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = Agate_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = Agate_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = Agate_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = Agate_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = Agate_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = Agate_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = Agate_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = Agate_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = Agate_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = Agate_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = Agate_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = Agate_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = Agate_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = Agate_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = Agate_gvtuDump;

	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = Agate_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = Agate_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = Agate_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = Agate_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = Agate_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = Agate_gqosGetPrioMapRule;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = Agate_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = Agate_gqosSetPrioMapRuleIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = Agate_gphyReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = Agate_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = Agate_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = Agate_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = Agate_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = Agate_gphySetPortAutoModeIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = Agate_gphySetEEE;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = Agate_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = Agate_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = Agate_gpirlCustomSetupIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = Agate_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = Agate_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = Agate_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = Agate_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = Agate_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = Agate_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = Agate_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = Agate_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = Agate_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = Agate_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = Agate_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = Agate_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = Agate_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = Agate_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = Agate_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = Agate_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = Agate_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = Agate_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = Agate_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = Agate_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = Agate_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = Agate_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = Agate_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = Agate_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = Agate_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = Agate_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = Agate_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = Agate_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = Agate_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = Agate_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = Agate_gprtSetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = Agate_gprtGetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = Agate_gprtSetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = Agate_gprtGetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = Agate_gprtSetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = Agate_gprtGetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = Agate_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = Agate_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = Agate_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = Agate_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = Agate_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = Agate_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = Agate_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = Agate_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = Agate_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = Agate_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = Agate_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = Agate_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = Agate_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = Agate_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = Agate_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = Agate_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = Agate_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = Agate_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = Agate_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = Agate_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = Agate_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = Agate_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = Agate_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = Agate_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = Agate_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = Agate_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = Agate_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = Agate_gprtRegDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = Agate_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = Agate_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = Agate_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = Agate_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = Agate_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = Agate_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = Agate_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = Agate_gsysGetTrunkRouting;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = Agate_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = Agate_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = Agate_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = Agate_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = Agate_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = Agate_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = Agate_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = Agate_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = Agate_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = Agate_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = Agate_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = Agate_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = Agate_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = Agate_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = Agate_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = Agate_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = Agate_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = Agate_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = Agate_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = Agate_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = Agate_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = Agate_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = Agate_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = Agate_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = Agate_gsysGlobal2RegDump;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = Agate_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = Agate_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = Agate_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = Agate_gqavSetPortQpriXRate;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = Agate_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = Agate_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = Agate_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = Agate_gtcamRemoveEntryPort;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = Agate_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = Agate_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = Agate_gptpSetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = Agate_gptpGetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = Agate_gptpSetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = Agate_gptpGetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = Agate_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = Agate_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = Agate_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = Agate_gptpSetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = Agate_gptpGetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = Agate_gptpGetIntStatusIntf;

		dev->SwitchDevObj.TCAMObj.gtcamFindEntry = Agate_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = Agate_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = Agate_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = Agate_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = Agate_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = Agate_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = Agate_gtcamDump;
		dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = Agate_gtcamAdvConfigIntf;
	

	dev->SwitchDevObj.RMUObj.grmuGetID = Agate_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = Agate_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMibDump = Agate_msdRmuMibDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = Agate_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = Agate_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = Agate_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = Agate_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = Agate_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = Agate_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = Agate_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = Agate_msdGetSMIPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = Agate_gledForceOn;
	dev->SwitchDevObj.LEDObj.gledForceOff = Agate_gledForceOff;
#endif
	return MSD_OK;
}

static MSD_STATUS InitObj(MSD_QD_DEV *dev)
{
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFlush = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDump = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = NULL;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = NULL;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = NULL;

	dev->SwitchDevObj.STUObj.gstuAddEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = NULL;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = NULL;
	dev->SwitchDevObj.STUObj.gstuDump = NULL;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuFlush = NULL;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDump = NULL;

	dev->SwitchDevObj.IMPObj.impRun = NULL;
	dev->SwitchDevObj.IMPObj.impReset = NULL;
	dev->SwitchDevObj.IMPObj.impWriteEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = NULL;
	dev->SwitchDevObj.IMPObj.impStop = NULL;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = NULL;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = NULL;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = NULL;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = NULL;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = NULL;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = NULL;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = NULL;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = NULL;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = NULL;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = NULL;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamDump = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = NULL;

	dev->SwitchDevObj.RMUObj.grmuGetID = NULL;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMibDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = NULL;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = NULL;
    dev->SwitchDevObj.RMUObj.grmuRegDump = NULL;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = NULL;

	dev->SwitchDevObj.LEDObj.gledForceOn = NULL;
	dev->SwitchDevObj.LEDObj.gledForceOff = NULL;
	dev->SwitchDevObj.LEDObj.gledLinkAct = NULL;

	return MSD_OK;
}

MSD_STATUS InitDevObj(MSD_QD_DEV *dev)
{
	InitObj(dev);
    if (dev->devName == MSD_TOPAZ)
        InitDevObj_Topaz(dev);
    else if (dev->devName == MSD_PERIDOT)
        InitDevObj_Peridot(dev);
	if (dev->devName == MSD_PEARL)
		InitDevObj_Pearl(dev);
	else if (dev->devName == MSD_AGATE)
		InitDevObj_Agate(dev);
    return MSD_OK;
}