#include <msdCopyright.h>

/********************************************************************************
* msdApiInternal.h
*
* DESCRIPTION:
*       API Prototypes for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __PEARL_msdApiInternal_h
#define __PEARL_msdApiInternal_h

#ifdef __cplusplus
extern "C" {
#endif
#define PEARL_IS_BROADCAST_MAC(mac) (((mac).arEther[0] == 0xFF) && ((mac).arEther[1] == 0xFF) && ((mac).arEther[2] == 0xFF) && ((mac).arEther[3] == 0xFF) && ((mac).arEther[4] == 0xFF) && ((mac).arEther[5] == 0xFF))

/* definition for Trunking */
#define PEARL_IS_TRUNK_ID_VALID(_dev, _id)    (((_id) < 16) ? 1 : 0)

/* Convert port number to smi device address */
#define PEARL_MSD_CALC_SMI_DEV_ADDR(_dev, _portNum)  \
	((_dev)->baseRegAddr + (_portNum))
/***************************************************************************/

/*
 * Typedef: enum MSD_ATU_OPERARION
 *
 * Description: Defines the different ATU and VTU operations
 *
 * Fields:
 *      FLUSH_ALL           - Flush all entries.
 *      FLUSH_UNLOCKED      - Flush all unlocked entries in ATU.
 *      LOAD_PURGE_ENTRY    - Load / Purge entry.
 *      GET_NEXT_ENTRY      - Get next ATU or VTU entry.
 *      FLUSH_ALL_IN_DB     - Flush all entries in a particular DBNum.
 *      FLUSH_UNLOCKED_IN_DB - Flush all unlocked entries in a particular DBNum.
 *      SERVICE_VIOLATONS   - sevice violations of VTU
 *
 */
typedef enum
{
    PEARL_FLUSH_ALL = 1,        /* for both atu and vtu */
    PEARL_FLUSH_NONSTATIC,        /* for atu only */
    PEARL_LOAD_PURGE_ENTRY,    /* for both atu and vtu */
    PEARL_GET_NEXT_ENTRY,        /* for both atu and vtu */
    PEARL_FLUSH_ALL_IN_DB,    /* for atu only */
    PEARL_FLUSH_NONSTATIC_IN_DB,    /* for atu only */
    PEARL_SERVICE_VIOLATIONS     /* for vtu only */
} PEARL_MSD_ATU_OPERATION, PEARL_MSD_VTU_OPERATION;
	
/*
* Define max num of queues
*/
#define PEARL_MAX_NUM_OF_QUEUES 4

#ifdef __cplusplus
}
#endif

#endif /* __PEARL_msApiInternal_h */
