#include <msdCopyright.h>
/********************************************************************************
* sample_msdBrgVtu.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Flush All Entries in the STU.
*    2. Load a entry in STU table.
*    3. Find STU entry from the specified SID.
*    4. Delete a STU entry.
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

MSD_STATUS sample_msdBrgStu(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_STU_ENTRY stuEntry;
    MSD_U16 sid;
    MSD_BOOL found;
	MSD_QD_DEV *dev;
	int i;

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	 *    Flush all Entries in VTU and STU
	 */
    if ((status = msdVlanAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdVlanAllDelete returned fail.\n"));
		return status;
	}

	/*
	 *    Initialize the STU entry
	 */
	stuEntry.sid = 1;

	for(i=0; i<dev->numOfPorts; i++)
	{
		/*
		 *    port0, port4, port8 : MSD_PORT_DISABLE
		 *    port1, port5, port9 : MSD_PORT_BLOCKING
		 *    port2, port6, port10: MSD_PORT_LEARNING
		 *    port3, port7        : MSD_PORT_FORWARDING
		 */
		stuEntry.portState[i] = i%4;
	}

	/*
	 *    Add this STU Entry.
	 */
    if ((status = msdStuEntryAdd(devNum, &stuEntry)) != MSD_OK)
    {
        MSG_PRINT(("msdStuEntryAdd returned fail.\n"));
        return status;
    }

    /*
     *    Find STU entry for the specific SID.
     *    Start to search valid STU entry from the lowest sid
     */
    sid = stuEntry.sid;
    if ((status = msdStuEntryFind(devNum, sid, &stuEntry, &found)) != MSD_OK)
    {
    	MSG_PRINT(("msdStuEntryFind returned fail.\n"));
    	return status;
    }

	/*
	 *    delete this entry.
	 */
    if ((status = msdStuEntryDelete(devNum, sid)) != MSD_OK)
	{
		MSG_PRINT(("msdStuEntryDelete returned fail.\n"));
		return status;
	}


	MSG_PRINT(("sample code success.\n"));
	return MSD_OK;
}
