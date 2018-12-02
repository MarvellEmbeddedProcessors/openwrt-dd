#include <msdCopyright.h>
/********************************************************************************
* sample_msdBrgVtu.c
*
* DESCRIPTION:
*    The sample sample_msdBrgVtu will demonstrate how to
*    1. Flush All Entries in the VTU.
*    2. Load a VTU entry.
*    3. Find VTU entry for a specific VID.
*    4. Del a VTU entry.
*    5. Get/Clear Violation Data.
*
*    The sample sample_msdBrgVtu_Stu will demonstrate how to
*    support 802.1s Per VLAN Spanning Tree (check the datasheet for details)
*    1. Set some ports as a vlan group
*    2. Configure vlan ports' PortState: blocking/learning/forwarding
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Basic VTU operation.
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdBrgVtu(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_VTU_ENTRY vtuEntry;
	MSD_VTU_INT_STATUS vtuIntStatus;
    MSD_U16 vlanId;
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
	 *    Flush all Entries in the VTU
	 */
    if ((status = msdVlanAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdVlanAllDelete returned fail.\n"));
		return status;
	}

	/*
	 *    Initialize the VTU entry
	 */

	vtuEntry.fid = 0;

	vtuEntry.vid = 1;

	vtuEntry.sid = 1;

	for(i=0; i<dev->numOfPorts; i++)
	{
		/*
		 * p0, p4, p8...  : MEMBER_EGRESS_UNMODIFIED,
		 * p1, p5, p9...  : MEMBER_EGRESS_UNTAGGED,
		 * p2, p6, p10... : MEMBER_EGRESS_TAGGED,
		 * p3, p7         : NOT_A_MEMBER
		 * p0, p1, p2, p4, p5, p6, ... is the member of this vlan(vid=1)
		 */
		vtuEntry.memberTagP[i] = i%4;
	}

	vtuEntry.vidPolicy = MSD_FALSE;  /* VID Policy bit.*/

	vtuEntry.vidExInfo.useVIDFPri = MSD_FALSE; /* If device doesnot support VTU Frame Priority override,
		                                         this field is ignored. */
	vtuEntry.vidExInfo.vidFPri = 0;           /* If device doesnot support VTU Frame Priority override,
		                                         this field is ignored. */
	vtuEntry.vidExInfo.useVIDQPri = MSD_FALSE; /* If device doesnot support VTU Queue Priority override,
		                                         this field is ignored. */
	vtuEntry.vidExInfo.vidQPri = 0;           /* If device doesnot support VTU Queue Priority override,
		                                         this field is ignored. */

	vtuEntry.vidExInfo.dontLearn = MSD_FALSE;  /* Do not learn the MAC address */
	vtuEntry.vidExInfo.filterUC =  MSD_FALSE;  /* Determine whether or not to filter unicast frame */
	vtuEntry.vidExInfo.filterBC = MSD_FALSE;   /* Determine whether or not to filter broadcast frame */
	vtuEntry.vidExInfo.filterMC = MSD_FALSE;   /* Determine whether or not to filter multicast frame */

	/*
	 *    Add this VTU Entry.
	 */
    if ((status = msdVlanEntryAdd(devNum, &vtuEntry)) != MSD_OK)
    {
        MSG_PRINT(("msdVlanEntryAdd returned fail.\n"));
        return status;
    }

    /*
     *    Find VTU entry for a specific VID
     */

    vlanId = 1;
    if ((status = msdVlanEntryFind(devNum, vlanId, &vtuEntry, &found)) != MSD_OK)
    {
    	MSG_PRINT(("msdVlanEntryFind returned fail.\n"));
    	return status;
    }

	/*
	 *    delete this entry.
	 */
    if ((status = msdVlanEntryDelete(devNum, vlanId)) != MSD_OK)
	{
		MSG_PRINT(("msdVlanEntryDelete returned fail.\n"));
		return status;
	}

	/*
	 *    Get Violation.
	 */
    if ((status = msdVlanViolationGet(devNum, &vtuIntStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdVlanViolationGet returned fail.\n"));
		return status;
	}

    MSG_PRINT(("%s: %d, %s,  VTU_MEMBER_VIOLATION: %d\n", __FILE__, __LINE__, __FUNCTION__,
        vtuIntStatus.vtuIntCause.memberVio));
    MSG_PRINT(("%s: %d, %s,  VTU_MISS_VIOLATION: %d\n", __FILE__, __LINE__, __FUNCTION__,
        vtuIntStatus.vtuIntCause.missVio));
    MSG_PRINT(("%s: %d, %s,  VTU_FULL_VIOLATION: %d\n", __FILE__, __LINE__, __FUNCTION__,
        vtuIntStatus.vtuIntCause.fullVio));


	MSG_PRINT(("sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2:
*       Load one VTU entry to configure the DUT's port 0, port 1, port 2 as one 
*       vlan group, then load one STU entry to set port2' PortState as blocking
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdBrgVtu_Stu(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_VTU_ENTRY vtuEntry;
	MSD_STU_ENTRY stuEntry;
	MSD_QD_DEV *dev;
	MSD_8021Q_MODE mode;
	int i;

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	* Enable 802.1Q mode for all the ports
	*/
	mode = MSD_8021Q_FALLBACK;
	for (i = 0; i < dev->numOfPorts; i++)
	{
		if ((status = msdPort8021qModeSet(devNum, i, mode)) != MSD_OK)
		{
			MSG_PRINT(("msdPort8021qModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	* Load one VTU entry to set port 0,1,2 as one VLAN group
	*/
	vtuEntry.fid = 0;
	vtuEntry.vid = 1;
	vtuEntry.sid = 1;
	for (i = 0; i<dev->numOfPorts; i++)
	{
		if (i < 3)
		{
			vtuEntry.memberTagP[i] = MSD_MEMBER_EGRESS_UNMODIFIED;
		}
		else
		{
			vtuEntry.memberTagP[i] = MSD_NOT_A_MEMBER;
		}
	}

	vtuEntry.vidPolicy = MSD_FALSE;  /* VID Policy bit.*/
	vtuEntry.vidExInfo.useVIDFPri = MSD_FALSE; /* If device doesnot support VTU Frame Priority override,
											  this field is ignored. */
	vtuEntry.vidExInfo.vidFPri = 0;           /* If device doesnot support VTU Frame Priority override,
											  this field is ignored. */
	vtuEntry.vidExInfo.useVIDQPri = MSD_FALSE; /* If device doesnot support VTU Queue Priority override,
											  this field is ignored. */
	vtuEntry.vidExInfo.vidQPri = 0;           /* If device doesnot support VTU Queue Priority override,
											  this field is ignored. */
	vtuEntry.vidExInfo.dontLearn = MSD_FALSE;  /* Do not learn the MAC address */
	vtuEntry.vidExInfo.filterUC = MSD_FALSE;  /* Determine whether or not to filter unicast frame */
	vtuEntry.vidExInfo.filterBC = MSD_FALSE;   /* Determine whether or not to filter broadcast frame */
	vtuEntry.vidExInfo.filterMC = MSD_FALSE;   /* Determine whether or not to filter multicast frame */
	if ((status = msdVlanEntryAdd(devNum, &vtuEntry)) != MSD_OK)
	{
		MSG_PRINT(("msdVlanEntryAdd returned fail.\n"));
		return status;
	}

	/*
	* Load one STU entry with the same sid to set the port2' PortState as blocking
	*/
	stuEntry.sid = vtuEntry.sid;
	for (i = 0; i<dev->numOfPorts; i++)
	{
		if (i == 2)
		{
			stuEntry.portState[i] = MSD_PORT_BLOCKING;
		}
		else
		{
			stuEntry.portState[i] = MSD_PORT_FORWARDING;
		}
	}
	if ((status = msdStuEntryAdd(devNum, &stuEntry)) != MSD_OK)
	{
		MSG_PRINT(("msdStuEntryAdd returned fail.\n"));
		return status;
	}

	/**************************************************************************************
	*   After this setup, the frames (non MGMT frame) entering port0  that match this     *
	*   vlan will be only forwarded to port 1.                                                 *
    *   Port 2 (which been blocking) or other ports (which not belong this vlan group)    *
    *   will not forward frames                                                           *
	**************************************************************************************/

	MSG_PRINT(("sample_msdBrgVtu_Stu success.\n"));
	return MSD_OK;
}
