#include <msdCopyright.h>

/********************************************************************************
* msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __msdApi_h
#define __msdApi_h
#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/
#include <msdApiTypes.h>
#include <driver/msdHwAccess.h>
#include <platform/msdSem.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

/* add modules whatever you needed */
#include <api/msdBrgFdb.h>
#include <api/msdAdvVct.h>
#include <api/msdBrgStu.h>
#include <api/msdBrgVtu.h>
#include <api/msdIMP.h>
#include <api/msdPhyCtrl.h>
#include <api/msdPIRL.h>
#include <api/msdPortCtrl.h>
#include <api/msdQosMap.h>
#include <api/msdPortRmon.h>
#include <api/msdQueueCtrl.h>
#include <api/msdSysCtrl.h>
#include <api/msdTCAM.h>
#include <api/msdRMU.h>
#include <api/msdQav.h>
#include <api/msdPTP.h>
#include <api/msdTrunk.h>
#include <api/msdLed.h>
#ifdef __cplusplus
}
#endif

#endif /* __msdApi_h */
