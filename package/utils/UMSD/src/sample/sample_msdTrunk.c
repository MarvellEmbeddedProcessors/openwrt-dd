#include <msdCopyright.h>
/********************************************************************************
* sample_msdTrunk.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*		Sample #1: enable trunk port for port 1 and port 2 with trunkId 0. source Mac Address of 
*		            ingress frames for port 1 and port 2 will be learned by ATU with Trunk bit
*                   enable and port vector equal to TrunkId.
*
*		Sample #2: Load balance based on trunk routing and trunk mask
*		1. load static atu entry with trunk enable
*		2. Configure Trunk Mapping Table in Global 2,offset 0x08
*		3. Configure Trunk Mask Table
*		Note: Load balancing is used to ensure frames only egress one link (or port)
*			member of a trunk. Here, in order to simplify the question, set
*			HashTrunk bit to 0, which means that calculating the final MaskNum
*			using the XOR value of lower 3 bits of actual MAC address.
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1: Atu learn for trunk port, enable trunk port for port 1 and port 2 
* with trunkId 0. source Mac Address of ingress frames entering port 1 and port 2 will
* be learned by ATU with Trunk bit enable and port vector equal to TrunkId.
********************************************************************************/
MSD_STATUS sample_msdTrunk_PortTrunkingLearn(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_LPORT trunkPort1 = 1;
	MSD_LPORT trunkPort2 = 2;
	MSD_BOOL en = MSD_TRUE;
	MSD_U16 trunkId = 0;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    enable and set trunkId for port 1
	*/
	if ((status = msdPortLAGPortSet(devNum, trunkPort1, en, trunkId)) != MSD_OK)
	{
		MSG_PRINT(("msdPortLAGPortSet returned fail.\n"));
		return status;
	}

	/*
	*    enable and set trunkId for port 2
	*/
	if ((status = msdPortLAGPortSet(devNum, trunkPort2, en, trunkId)) != MSD_OK)
	{
		MSG_PRINT(("msdPortLAGPortSet returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdTrunk_PortTrunkingLearn success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2: Load balance based on trunk routing and trunk mask
*		1. Configure Trunk Mapping Table
*		2. Configure Trunk Mask Table
*		Note: Load balancing is used to ensure frames only egress one link (or port)
*			member of a trunk. Here, in order to simplify the question, set 
*			HashTrunk bit to 0, which means that calculating the final MaskNum 
*			using the XOR value of lower 3 bits of actual MAC address.
#
********************************************************************************/
MSD_STATUS sample_msdTrunk_TrunkRouting_TrunkMask(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U32 len = 8;
	MSD_U32 idx = 0;

	/* Configure Trunk Mapping Table
	TrunkId(14:12)				TrunkMap(10:0)
		1                           2'b00000000110 - means port 1/2 are from the same TrunkID 1
	Note: for Topaz, Agate, Pearl TrunkMap valid bit are bit 6:0 */
	MSD_U32 trunkId = 1;
    MSD_U32 trunkRoute = (1 << 1) | (1 << 2);		/* egress trunk ports are port 1 and port 2*/

	/* Configure Trunk Mask Table
		MaskNum(14:12)				TrunkMask(10:0) - means port 1 / 2 are load balance
			0                           2'b11111111101
			1                           2'b11111111011
			2                           2'b11111111101
			3                           2'b11111111011
			4                           2'b11111111101
			5                           2'b11111111011
			6                           2'b11111111101
			7                           2'b11111111011
	Note: for Topaz, TrunkMask valid bit are bit 6:0 */
	
	MSD_U32 trunkMaskNum[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	MSD_U32 trunkMask[8] = { 0x7D, 0x7B, 0x7D, 0x7B, 0x7D, 0x7B, 0x7D, 0x7B };

	MSD_BOOL hashEn = MSD_FALSE;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

    /*
    *    Set Trunk Routing
    */
    if ((status = msdTrunkRoutingSet(devNum, trunkId, trunkRoute)) != MSD_OK)
    {
        MSG_PRINT(("msdTrunkRoutingSet returned fail.\n"));
        return status;
    }

    /*
    *    Disable Hash Trunk
    */
    if ((status = msdTrunkHashTrunkSet(devNum, hashEn)) != MSD_OK)
    {
        MSG_PRINT(("msdTrunkMaskTableSet returned fail.\n"));
        return status;
    }

    /*
    *    Set Trunk Mask Table
    */
    for (idx = 0; idx < 8; idx++)
    {
        if ((status = msdTrunkMaskTableSet(devNum, trunkMaskNum[idx], trunkMask[idx])) != MSD_OK)
        {
            MSG_PRINT(("msdTrunkMaskTableSet returned fail.\n"));
            return status;
        }
    }

	MSG_PRINT(("sample_msdTrunk_TrunkRouting_TrunkMask success.\n"));
	return MSD_OK;
}