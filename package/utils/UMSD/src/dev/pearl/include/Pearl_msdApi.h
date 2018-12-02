#include <msdCopyright.h>

/********************************************************************************
* Pearl_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __Pearl_msdApi_h
#define __Pearl_msdApi_h
#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <pearl/include/api/Pearl_msdBrgFdb.h>
#include <pearl/include/api/Pearl_msdVct.h>
#include <pearl/include/api/Pearl_msdBrgVtu.h>
#include <pearl/include/api/Pearl_msdIMP.h>
#include <pearl/include/api/Pearl_msdBrgStu.h>
#include <pearl/include/api/Pearl_msdPIRL.h>
#include <pearl/include/api/Pearl_msdPortRmon.h>
#include <pearl/include/api/Pearl_msdTCAM.h>
#include <pearl/include/api/Pearl_msdRMU.h>
#include <pearl/include/api/Pearl_msdPhyCtrl.h>
#include <pearl/include/api/Pearl_msdPortPrioMap.h>
#include <pearl/include/api/Pearl_msdPortCtrl.h>
#include <pearl/include/api/Pearl_msdSysCtrl.h>
#include <pearl/include/api/Pearl_msdQav.h>
#include <pearl/include/api/Pearl_msdPTP.h>
#include <pearl/include/api/Pearl_msdQueueCtrl.h>
#include <pearl/include/api/Pearl_msdLed.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdApi_h */
