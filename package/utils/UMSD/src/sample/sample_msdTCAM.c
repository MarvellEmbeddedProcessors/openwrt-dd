#include <msdCopyright.h>
/********************************************************************************
* sample_msdTCAM.c
*
* DESCRIPTION:
*    the sample sample_msdTCAM will demonstrate how to
*    1. Flush All Entries for both ingress and egress TCAM table.
*    2. Load a TCAM entry to match Ethernet frame with specified DA and vid.
*    3. Flush a single TCAM entry.
*    4. Load the TCAM entry again.
*    5. Get next higher valid TCAM entry starting from a specific entry number.
*    6. Read a TCAM entry.
*
*    the sample sample_msdTCAM_cascade will demonstrate how to
*    1. Cascade two ingress TCAM entries to match Ethernet frame with specified ipv6 header (ipv6 header is too long, we should cascade two TCAM entries to match the whole header)
*
*    the sample sample_msdTCAM_ingress_egress_cascade will demonstrate how to
*    1. Cascade ingress and egress entries
*
*    the sample sample_msdTcam_Adv will demonstrate how to
*    1. Match the TCP over Ipv6 frame with specified DA, ipv6 and Tcp header using advance TCAM APIs
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

static void displayTcamData(MSD_TCAM_DATA *tcamData);
static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData);
#define QD_REG_PRI_OVERRIDE         0xD		/* Override Register */

static void displayTcamData(MSD_TCAM_DATA *tcamData)
{
	int i;

	MSG_PRINT(("----- Key & Mask -----\n"));

	MSG_PRINT(("frameType         : %04x\n", tcamData->frameType));
	MSG_PRINT(("frameTypeMask     : %04x\n", tcamData->frameTypeMask));
	MSG_PRINT(("spv               : %04x\n", tcamData->spv));
	MSG_PRINT(("spvMask           : %04x\n", tcamData->spvMask));
	MSG_PRINT(("ppri              : %04x\n", tcamData->ppri));
	MSG_PRINT(("ppriMask          : %04x\n", tcamData->ppriMask));
	MSG_PRINT(("pvid              : %04x\n", tcamData->pvid));
	MSG_PRINT(("pvidMask          : %04x\n", tcamData->pvidMask));

	MSG_PRINT(("----- Ethernet Frame Content -----\n"));

	for (i = 0; i<48; i++)
	{
		MSG_PRINT(("frame data Octet %2d         : %4x\n", i, tcamData->frameOctet[i]));
		MSG_PRINT(("frame data OctetMask %2d     : %4x\n", i, tcamData->frameOctetMask[i]));
	}

	MSG_PRINT(("----- Ingress Action -----\n"));

	MSG_PRINT(("continu           : %04x\n", tcamData->continu));
	MSG_PRINT(("interrupt         : %04x\n", tcamData->interrupt));
	MSG_PRINT(("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr));
	MSG_PRINT(("vidOverride       : %04x\n", tcamData->vidOverride));
	MSG_PRINT(("vidData           : %04x\n", tcamData->vidData));

	MSG_PRINT(("nextId            : %04x\n", tcamData->nextId));
	MSG_PRINT(("qpriOverride      : %04x\n", tcamData->qpriOverride));
	MSG_PRINT(("qpriData          : %04x\n", tcamData->qpriData));
	MSG_PRINT(("fpriOverride      : %04x\n", tcamData->fpriOverride));
	MSG_PRINT(("fpriData          : %04x\n", tcamData->fpriData));

	MSG_PRINT(("dpvSF             : %04x\n", tcamData->dpvSF));
	MSG_PRINT(("dpvData           : %04x\n", (unsigned int)tcamData->dpvData));
	MSG_PRINT(("dpvMode           : %04x\n", tcamData->dpvMode));

	MSG_PRINT(("unKnownFilter     : %04x\n", tcamData->unKnownFilter));

	MSG_PRINT(("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride));
	MSG_PRINT(("ldBalanceData     : %04x\n", tcamData->ldBalanceData));

	MSG_PRINT(("factionOverride   : %04x\n", tcamData->factionOverride));
	MSG_PRINT(("factionData       : %04x\n", tcamData->factionData));
#ifdef PERIDOT_BUILD_IN
	MSG_PRINT(("colorMode         : %04x\n", tcamData->colorMode));
	MSG_PRINT(("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride));
	MSG_PRINT(("vtuPage           : %04x\n", tcamData->vtuPage));
	MSG_PRINT(("egActPoint        : %04x\n", tcamData->egActPoint));
	MSG_PRINT(("DSCPOverride      : %04x\n", tcamData->DSCPOverride));
	MSG_PRINT(("DSCP              : %04x\n", tcamData->DSCP));
#endif
}

static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData)
{
	MSG_PRINT(("----- MSD_TCAM_EGR_DATA -----\n"));

	MSG_PRINT(("frameModeOverride : %04x\n", tcamEgrData->frameModeOverride));
	MSG_PRINT(("frameMode         : %04x\n", tcamEgrData->frameMode));
	MSG_PRINT(("tagModeOverride   : %04x\n", tcamEgrData->tagModeOverride));
	MSG_PRINT(("tagMode           : %04x\n", tcamEgrData->tagMode));
	MSG_PRINT(("daMode            : %04x\n", tcamEgrData->daMode));
	MSG_PRINT(("saMode            : %04x\n", tcamEgrData->saMode));

	MSG_PRINT(("egVidModeOverride : %04x\n", tcamEgrData->egVidModeOverride));
	MSG_PRINT(("egVidMode         : %04x\n", tcamEgrData->egVidMode));
	MSG_PRINT(("egVidData         : %04x\n", tcamEgrData->egVidData));

	MSG_PRINT(("egDSCPMode        : %04x\n", tcamEgrData->egDSCPMode));
	MSG_PRINT(("egDSCP            : %04x\n", tcamEgrData->egDSCP));
	MSG_PRINT(("egfpriModeOverride: %04x\n", tcamEgrData->egfpriModeOverride));
	MSG_PRINT(("egfpriMode        : %04x\n", tcamEgrData->egfpriMode));
	MSG_PRINT(("egEC              : %04x\n", tcamEgrData->egEC));
	MSG_PRINT(("egFPRI            : %04x\n", tcamEgrData->egFPRI));
}


MSD_STATUS sample_msdTCAM(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    First, set TCAM Mode for all ports
	*    In the real operation, customize this part of code according to your hardware setup
	*    Set the TCAM Mode for the ports which want to use tcam
	*/
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	 *    Flush all entries for both ingress and egress TCAM table
	 */
    if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	 *    Initialize a TCAM entry
	 */
	msdMemSet(&tcamData,0,sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
	tcamData.frameType = 0x0;   /* Normal Port */
	tcamData.frameTypeMask = 0x3;     /* Mask Type */

	/*
	 *   use frameOctet and framOctetMask to match Ethernet frame format (unit: byte), with the DA = 01:00:5E:00:00:00 and vid = 0x123
	 *                            DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	 *                           6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	 *                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	 *                 |                       |                       |         | vid |       |       ......     |               |
	 *                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	 *   frameOctet     0 1 0 0 5 E 0 0 0 0 0 0                                 0 1 2 3
	 *   frameOctetMask F F F F F F F F F F F F                                 0 F F F
	 */

	/* Match DA = 01:00:5E:00:00:00 */
	tcamData.frameOctet[0] = 0x01;
	tcamData.frameOctetMask[0] = 0xff;
	tcamData.frameOctet[1] = 0x00;
	tcamData.frameOctetMask[1] = 0xff;
	tcamData.frameOctet[2] = 0x5E;
	tcamData.frameOctetMask[2] = 0xff;
	tcamData.frameOctet[3] = 0x00;
	tcamData.frameOctetMask[3] = 0xff;
	tcamData.frameOctet[4] = 0x00;
	tcamData.frameOctetMask[4] = 0xff;
	tcamData.frameOctet[5] = 0x00;
	tcamData.frameOctetMask[5] = 0xff;

	/* Match vid = 0x123 */
	tcamData.frameOctet[14] = 0x01;
	tcamData.frameOctetMask[14] = 0x0f;
	tcamData.frameOctet[15] = 0x23;
	tcamData.frameOctetMask[15] = 0xff;

	tcamData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
	tcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	tcamData.dpvMode = 0x3; /*  Replace the frame¡¯s DPV with the TCAM Entry¡¯s DPV Data */
	tcamData.dpvData = 0x006; /*B0000110, frame hit this entry will egress from port1 and port2*/


	/*
	 *    Load this TCAM entry.
	 */
	tcamPointer = 1;  /* 0 ~ 0xff */
    if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

	/*
	 *    Flush this TCAM entry above.
	 */
    if ((status = msdTcamEntryDelete(devNum, tcamPointer)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryDelete returned fail.\n"));
        return status;
    }

	/*
	 *    Load this TCAM entry again.
	 */
	tcamPointer = 1;  /* 0 ~ 0xff */
    if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

    /*
     *    Get next higher valid TCAM entry starting from a specific entry number.(0xff).
     *    Start to search valid TCAM entry from the lowest tcamPointer
     *    Display this found valid TCAM entry
     */
	tcamPointer = 0xff; /*0xff for Peridot, 0x7f for Topaz*/
    if ((status = msdTcamEntryGetNext(devNum, &tcamPointer, &tcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryGetNext returned fail.\n"));
        return status;
    }
    MSG_PRINT(("find the next valid TCAM entry number is %d\n", (int)tcamPointer));/* result tcamPointer==1*/
    displayTcamData(&tcamData);

    /*
     *    Read this TCAM entry.
     *    Display this TCAM
     */
    if ((status = msdTcamEntryRead(devNum, tcamPointer, &tcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryRead returned fail.\n"));
        return status;
    }
    displayTcamData(&tcamData);


	MSG_PRINT(("sample_msdTCAM sample code success.\n"));
	return MSD_OK;
}

MSD_STATUS sample_msdTCAM_cascade(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer1 = 1;
	MSD_TCAM_DATA  tcamData1;
	MSD_U32        tcamPointer2 = 0xf;
	MSD_TCAM_DATA  tcamData2;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	 *    First, set TCAM Mode for all ports
	 *    In the real operation, customize this part of code according to your hardware setup
	 *    Set the TCAM Mode for the ports which want to use tcam
	 */
	tcamMode = MSD_TCAM_MODE_ENABLE_96; /* enable 96 bytes match tcam mode, 2 entries cascade match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	 *    Flush all entries for both ingress and egress TCAM table
	 */
    if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	 *     Initialize and load the 2 cascading ingress TCAM entries
	 *
     *
	 *     Use frameOctet and framOctetMask to match:
	 *
	 *     1. Ethernet frame format as below (unit: byte), with specified DA = 01:00:5E:00:00:00, Type = 0x86DD (ipv6)
	 *                              DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	 *                             6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	 *                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	 *                   |                       |                       |               |       |       ......     |               |
	 *                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	 *     frameOctet     0 1 0 0 5 E 0 0 0 0 0 0                                         8 6 D D
	 *     frameOctetMask F F F F F F F F F F F F                                         F F F F
     *
     *     2. IPv6 packet header format is in the "Data and Pad" filed of Ethernet frame (unit: bit) as below, with specified ipv6 Destination Address = 2015:1123:4567:89AB:CDEF:0550:A00A:5AA5
     *       0           3                              11              15                              23                              31
     *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     *     |Version-4 bits |    Traffic Class - 8 bits     |                                Flow Label                                     |  tcamData1.frameOctet[18] - tcamData1.frameOctet[21]
     *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     *     |                   Payload Length - 16 bits                    |     Next Header - 8 bits      |        Hop Limit - 8 bits     |  tcamData1.frameOctet[22] - tcamData1.frameOctet[25]
     *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     *     |                                                                                                                               |  tcamData1.frameOctet[26] - tcamData1.frameOctet[29]
     *     +                                                                                                                               +
     *     |                                                                                                                               |  tcamData1.frameOctet[30] - tcamData1.frameOctet[33]
     *     +                                            Source Address - 128 bits                                                          +
     *     |                                                                                                                               |  tcamData1.frameOctet[34] - tcamData1.frameOctet[37]
     *     +                                                                                                                               +
     *     |                                                                                                                               |  tcamData1.frameOctet[38] - tcamData1.frameOctet[41]
     *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     *     |                                                                                                                               |  tcamData1.frameOctet[42] - tcamData1.frameOctet[45]
     *     +                                                                                                                               +
     *     |                                                                                                                               |  tcamData1.frameOctet[46] - tcamData1.frameOctet[47], tcamData2.frameOctet[0] - tcamData2.frameOctet[1]
     *     +                                          Destination Address - 128 bits                                                       +
     *     |                                     2015:1123:4567:89AB:CDEF:0550:A00A:5AA5                                                   |  tcamData2.frameOctet[ 2] - tcamData2.frameOctet[ 5]
     *     +                                                                                                                               +
     *     |                                                                                                                               |  tcamData2.frameOctet[ 6] - tcamData2.frameOctet[ 9]
     *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     *
     *
     *     Ethernet Frame DA:  tcamData1.frameOctet[0] - tcamData1.frameOctet[5]
     *     Ethernet Frame Type: tcamData1.frameOctet[16], tcamData1.frameOctet[17]
     *     IPv6 Destination Address:  tcamData1.frameOctet[42] - tcamData1.frameOctet[47], tcamData2.frameOctet[0] - tcamData2.frameOctet[9]
     *
     *
     */

	msdMemSet(&tcamData1,0,sizeof(MSD_TCAM_DATA)); /* Set tcamData1 all bit to 0 by default */
	msdMemSet(&tcamData2,0,sizeof(MSD_TCAM_DATA)); /* Set tcamData2 all bit to 0 by default */

	tcamData1.frameType = 0x0;   /* Normal Port */
	tcamData1.frameTypeMask = 0x3;     /* Mask Type */

	/* Match Ethernet Frame DA = 01:00:5E:00:00:00 */
	tcamData1.frameOctet[0] = 0x01;
	tcamData1.frameOctetMask[0] = 0xff;
	tcamData1.frameOctet[1] = 0x00;
	tcamData1.frameOctetMask[1] = 0xff;
	tcamData1.frameOctet[2] = 0x5E;
	tcamData1.frameOctetMask[2] = 0xff;
	tcamData1.frameOctet[3] = 0x00;
	tcamData1.frameOctetMask[3] = 0xff;
	tcamData1.frameOctet[4] = 0x00;
	tcamData1.frameOctetMask[4] = 0xff;
	tcamData1.frameOctet[5] = 0x00;
	tcamData1.frameOctetMask[5] = 0xff;

	/* Match Ethernet Frame Type = 0x86DD(ipv6) */
	tcamData1.frameOctet[16] = 0x86;
	tcamData1.frameOctetMask[16] = 0xff;
	tcamData1.frameOctet[17] = 0xDD;
	tcamData1.frameOctetMask[17] = 0xff;

	tcamData1.continu = 0x1;  /* cascade, continue bit should be set to 1, has next entry*/
	tcamData1.nextId = (MSD_U8)tcamPointer2;  /* nextId should be equal to the second TCAM entry number (0xf) */

	/* Match IPv6 Destination Address = 2015:1123:4567:89AB:CDEF:0550:A00A:5AA5, across tcamData1 and tcamData2 */
	tcamData1.frameOctet[42] = 0x20;
	tcamData1.frameOctetMask[42] = 0xff;
	tcamData1.frameOctet[43] = 0x15;
	tcamData1.frameOctetMask[43] = 0xff;
	tcamData1.frameOctet[44] = 0x11;
	tcamData1.frameOctetMask[44] = 0xff;
	tcamData1.frameOctet[45] = 0x23;
	tcamData1.frameOctetMask[45] = 0xff;
	tcamData1.frameOctet[46] = 0x45;
	tcamData1.frameOctetMask[46] = 0xff;
	tcamData1.frameOctet[47] = 0x67;
	tcamData1.frameOctetMask[47] = 0xff;

	tcamData2.frameOctet[0]  = 0x89;
	tcamData2.frameOctetMask[0]  = 0xff;
	tcamData2.frameOctet[1]  = 0xAB;
	tcamData2.frameOctetMask[1]  = 0xff;
	tcamData2.frameOctet[2]  = 0xCD;
	tcamData2.frameOctetMask[2]  = 0xff;
	tcamData2.frameOctet[3]  = 0xEF;
	tcamData2.frameOctetMask[3]  = 0xff;
	tcamData2.frameOctet[4]  = 0x05;
	tcamData2.frameOctetMask[4]  = 0xff;
	tcamData2.frameOctet[5]  = 0x50;
	tcamData2.frameOctetMask[5]  = 0xff;
	tcamData2.frameOctet[6]  = 0xA0;
	tcamData2.frameOctetMask[6]  = 0xff;
	tcamData2.frameOctet[7]  = 0x0A;
	tcamData2.frameOctetMask[7]  = 0xff;
	tcamData2.frameOctet[8]  = 0x5A;
	tcamData2.frameOctetMask[8]  = 0xff;
	tcamData2.frameOctet[9]  = 0xA5;
	tcamData2.frameOctetMask[9]  = 0xff;

    tcamData2.pvid = (MSD_U16)tcamPointer2; /* PVID[7:0] or Index, set index value as tcamPointer2 to support cascade*/
    tcamData2.pvidMask = 0xff;

	tcamData2.continu = 0x0;  /*  continue bit should be set to 0, no next entry */
	tcamData2.interrupt = 0x1; /* if the frame data matched the 2 cascade entry, there will be a tcam interrupt */
	tcamData2.dpvMode = 0x3; /*  Replace the frame¡¯s DPV with the TCAM Entry¡¯s DPV Data */
	tcamData2.dpvData = 0x06; /* B0000110, frame hit this entry will egress from port1 and port2*/

    if ((status = msdTcamEntryAdd(devNum, tcamPointer1, &tcamData1)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

    if ((status = msdTcamEntryAdd(devNum, tcamPointer2, &tcamData2)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

	MSG_PRINT(("sample_msdTCAM_cascade sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
*     This sample demonstrate
*     1. how to use one Ingress Tcam Entry to match the Ethernet frame with vid=0x123 and the ipv6 packet header (partial) with specified DSCP (DSCP=0x20). Although IPv6 header is long, but in this case we only care the "DSCP" field, so only one Ingress Tcam Entry is needed
*     2. how to use 2 Egress Tcam Entries to deal with the matched ipv6 packet, let the packet egress from 2 different ports with 2 different DSCP overwrite (port1 with DSCP=0x30; port2 with DSCP=0x10)
*     3. how to cascade Ingress and Egress Tcam entries
*
*     Entry data mapping is like below:
*
*     Ethernet frame format with Type of IPv6(0x86DD), and vid=0x123, starting from DA (unit: byte)
*                              DA                      SA                   Tag     Length/Type   Data and Pad         FCS
*                             6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
*                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
*                   |                       |                       |         | vid |       |       ......     |               |
*                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
*     frameOctet                                                             0 1 2 3 8 6 D D
*     frameOctetMask                                                         F F F F F F F F
*
*     Ipv6 packet header(with DSCP=0x20) format is in the "Data and Pad" filed of Ethernet frame (unit: bit)
*       0           3                              11              15                              23                              31
*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*     |Version-4 bits |    Traffic Class - 8 bits     |                                Flow Label                                     |  tcamData1.frameOctet[18] - tcamData1.frameOctet[21]
*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*     |                   Payload Length - 16 bits                    |     Next Header - 8 bits      |        Hop Limit - 8 bits     |  tcamData1.frameOctet[22] - tcamData1.frameOctet[25]
*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*     |                                                                                                                               |  tcamData1.frameOctet[26] - tcamData1.frameOctet[29]
*     +                                                                                                                               +
*     |                                                                                                                               |  tcamData1.frameOctet[30] - tcamData1.frameOctet[33]
*     +                                            Source Address - 128 bits                                                          +
*     |                                                                                                                               |  tcamData1.frameOctet[34] - tcamData1.frameOctet[37]
*     +                                                                                                                               +
*     |                                                                                                                               |  tcamData1.frameOctet[38] - tcamData1.frameOctet[41]
*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*     |                                                                                                                               |  tcamData1.frameOctet[42] - tcamData1.frameOctet[45]
*     +                                                                                                                               +
*     |                                                                                                                               |  tcamData1.frameOctet[46] - tcamData1.frameOctet[47]
*     +                                          Destination Address - 128 bits                                                       +
*     |                                                                                                                               |  do not match
*     +                                                                                                                               +
*     |                                                                                                                               |  do not match
*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*
*     The DSCP field structure is presented in the "Traffic Class" filed of ipv6 packet header as below (unit: bit):
*     DSCP: differentiated services codepoint
*     CU:   currently unused
*                   +---+---+---+---+---+---+---+---+
*                   |      DSCP - 6 bits    |  CU   |  tcamData1.frameOctet[18] - tcamData1.frameOctet[19]
*                   +---+---+---+---+---+---+---+---+
*     frameOctet      1   0   0   0   0   0
*     frameOctetMask  1   1   1   1   1   1
*
*     Vid in Ethernet Frame Tag: tcamData1.frameOctet[14], tcamData1.frameOctet[15]
*     Ethernet Frame Type: tcamData1.frameOctet[16], tcamData1.frameOctet[17]
*     DSCP in IPv6 header: tcamData1.frameOctet[18], tcamData1.frameOctet[19]
*
*
*******************************************************************************/
MSD_STATUS sample_msdTCAM_ingress_egress_cascade(MSD_U8 devNum)
{
	MSD_STATUS     status;

	MSD_U32        IgrTcamPointer = 1;
	MSD_TCAM_DATA  IgrTcamData;

	MSD_U32 EgrTcamPointer = 0xf;
	MSD_TCAM_EGR_DATA EgrTcamData1;
	MSD_TCAM_EGR_DATA EgrTcamData2;

	MSD_LPORT port1 = 1;
	MSD_LPORT port2 = 2;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	if (MSD_TOPAZ == dev->devName)
		return MSD_NOT_SUPPORTED;
	/*
	*    First, set TCAM Mode for all ports
	*    In the real operation, customize this part of code according to your hardware setup
	*    Set the TCAM Mode for the ports which want to use tcam
	*/
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	 *    Flush all entries for both ingress and egress TCAM table
	 */
    if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	 *    Initialize and load the Ingress TCAM Entry
	 */
	msdMemSet(&IgrTcamData,0,sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
	/* Match vid = 0x123 */
	IgrTcamData.frameOctetMask[14] = 0x0f;
	IgrTcamData.frameOctet[14] = 0x01;
	IgrTcamData.frameOctetMask[15] = 0xff;
	IgrTcamData.frameOctet[15] = 0x23;
	/* Match Ethernet Frame Type = 0x86DD(ipv6) */
	IgrTcamData.frameOctet[16] = 0x86;
	IgrTcamData.frameOctetMask[16] = 0xff;
	IgrTcamData.frameOctet[17] = 0xDD;
	IgrTcamData.frameOctetMask[17] = 0xff;
	/*
	 *    Match the ipv6 packet, Traffic Class(including DSCP and CU) field in this Ingress TCAM entry, unit: bit
	 *
	 *           ...frameOctet[17]     |          frameOctet[18]       |          frameOctet[19]       |      frameOctet[20] ...
	 *       ...frameOctetMask[17]     |      frameOctetMask[18]       |      frameOctetMask[19]       |  frameOctetMask[20] ...
	 *                                 |                               |                               |
     *     +---+-------+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---------+---+
     *           Length/Type           |    version    |      DSCP = 0x20      |   CU  |             Flow Label
     *     +---+-------+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---------+---+
     *               frameOctet        | 0   0   0   0   1   0   0   0 | 0   0   0   0   0   0   0   0 |
	 *               frameOctetMask    | 0   0   0   0   1   1   1   1 | 1   1   0   0   0   0   0   0 |
	 *
	 *     mask 1 means matching needed,  mask 0 means matching not needed
	 *     frameOctet[18] low 4 bits and frameOctet[19] high 2 bits is for DSCP, so their frameOctet 6 bits must be 0b100000 = 0x20, and their mask 6 bits must be 0b111111
	 */
	IgrTcamData.frameOctet[18] = 0x08; /* 0b00001000 */
	IgrTcamData.frameOctetMask[18] = 0x0f; /* 0b00001111 */
	IgrTcamData.frameOctet[19] = 0x00; /* 0b00000000 */
	IgrTcamData.frameOctetMask[19] = 0xC0; /* 0b11000000 */

	IgrTcamData.continu = 0x0;  /* only match partial IPv6 header, one ingress tcam entry is needed, no cascade, continue should be set to 0, no next entry*/
	IgrTcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	IgrTcamData.dpvMode = 0x3; /*  Replace the frame¡¯s DPV with the TCAM Entry¡¯s DPV Data */
	IgrTcamData.dpvData = 0x006; /* B00000000110, frame hit this entry will egress from port1 and port2*/
	IgrTcamData.egActPoint = (MSD_U8)EgrTcamPointer;  /* in order to cascade ingress and egress tcam entry, EgrTcamPointer should NOT be 0*/
    if ((status = msdTcamEntryAdd(devNum, IgrTcamPointer, &IgrTcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

	/*
	 *    Initialize and load the first egress TCAM entry
	 */
	msdMemSet(&EgrTcamData1,0,sizeof(MSD_TCAM_EGR_DATA)); /* Set all bit to 0 by default */
	EgrTcamData1.egDSCPMode = 0x1; /* modify the DSCP value using the DSCP value below */
	EgrTcamData1.egDSCP = 0x30; /* egress from port1=1 with DSCP=0x30 */
    if ((status = msdEgrTcamEntryAdd(devNum, port1, EgrTcamPointer, &EgrTcamData1)) != MSD_OK)
    {
        MSG_PRINT(("msdEgrTcamEntryAdd returned fail.\n"));
        return status;
    }
    /*
     *    Read the first egress TCAM entry.
     *    Display this egress TCAM entry
     */
    if ((status = msdEgrTcamEntryRead(devNum, port1, EgrTcamPointer, &EgrTcamData1)) != MSD_OK)
    {
        MSG_PRINT(("msdEgrTcamEntryRead returned fail.\n"));
        return status;
    }
    displayTcamEgrData(&EgrTcamData1);

	/*
	 *    Initialize and load the second egress TCAM entry
	 */
	msdMemSet(&EgrTcamData2,0,sizeof(MSD_TCAM_EGR_DATA)); /* Set all bit to 0 by default */
	EgrTcamData2.egDSCPMode = 0x1; /* modify the DSCP value using the DSCP value below */
	EgrTcamData2.egDSCP = 0x10; /* egress from port2=2 with DSCP=0x10 */
    if ((status = msdEgrTcamEntryAdd(devNum, port2, EgrTcamPointer, &EgrTcamData2)) != MSD_OK)
    {
        MSG_PRINT(("msdEgrTcamEntryAdd returned fail.\n"));
        return status;
    }
    /*
     *    Read the second egress TCAM entry.
     *    Display this egress TCAM entry
     */
    if ((status = msdEgrTcamEntryRead(devNum, port2, EgrTcamPointer, &EgrTcamData2)) != MSD_OK)
    {
        MSG_PRINT(("msdEgrTcamEntryRead returned fail.\n"));
        return status;
    }
    displayTcamEgrData(&EgrTcamData2);

	MSG_PRINT(("sample_msdTCAM_ingress_egress_cascade sample code success.\n"));

	return MSD_OK;
}

/********************************************************************************
*   This sample demonstrate
*   1. Remove one port for a valid entry, that means the matching frame from this port will not been hit 
*   2. Add one port for a valid entry, that means the matching fram from this port will be hit 
*  
********************************************************************************/
MSD_STATUS sample_msdTCAM_add_remove_port(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer = 1;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    First, set TCAM Mode for all ports
	*    In the real operation, customize this part of code according to your hardware setup
	*    Set the TCAM Mode for the ports which want to use tcam
	*/
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	* Load one tcam entry that match the DA = aa:bb:cc:dd:ee:00, enable the TCAM counter increment
	* This entry can be used by all the ports
	*/
	msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA));
	tcamData.IncTcamCtr = 1;
	tcamData.frameOctet[0] = 0xaa;
	tcamData.frameOctetMask[0] = 0xff;
	tcamData.frameOctet[1] = 0xbb;
	tcamData.frameOctetMask[1] = 0xff;
	tcamData.frameOctet[2] = 0xcc;
	tcamData.frameOctetMask[2] = 0xff;
	tcamData.frameOctet[3] = 0xdd;
	tcamData.frameOctetMask[3] = 0xff;
	tcamData.frameOctet[4] = 0xee;
	tcamData.frameOctetMask[4] = 0xff;
	tcamData.frameOctet[5] = 0x00;
	tcamData.frameOctetMask[5] = 0xff;
	if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}

	/******************************************************************************************
	*   After this setup, if one frame with the DA = aa:bb:cc:dd:ee:00 entered to any port    * 
	*   the frame will hit the TCAM and the TCAM counter in MIBs for this port will increment *
	*******************************************************************************************/

	/*Remove the Port 0 for this entry*/
	if ((status = msdTcamEntryPortRemove(devNum, tcamPointer, 0)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryPortRemove returned fail.\n"));
		return status;
	}
	/*****************************************************************************************
	*   After this remove operation, the frame with the DA = aa:bb:cc:dd:ee:00 entered to    *
	*   port0 can not hit the tcam and the TCAM counter in MIBs for port0 will not increment *
    *   all other ports can hit this tcam                                                    *
	******************************************************************************************/

    /*Remove the Port 1 for this entry*/
    if ((status = msdTcamEntryPortRemove(devNum, tcamPointer, 1)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryPortRemove returned fail.\n"));
        return status;
    }
    /*****************************************************************************************
    *   After this remove operation, the frame with the DA = aa:bb:cc:dd:ee:00 entered to    *
    *   port0 and port1 can not hit the tcam and the TCAM counter in MIBs for port0 and port1*
    *   will not increment, all other ports can hit                                          *                                                                  *
    ******************************************************************************************/

	/*Add the Port 0 for this entry*/
	if ((status = msdTcamEntryPortAdd(devNum, tcamPointer, 0)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryPortAdd returned fail.\n"));
		return status;
	}
	/**************************************************************************************
	*   After this add operaiton, the frame with the DA = aa:bb:cc:dd:ee:00 entered to    *
	*   port0 can hit and the TCAM counter in MIBs for port0 will increment               *
    *   all other ports except port1 can hit this entry                                   *
	**************************************************************************************/

	MSG_PRINT(("sample_msdTCAM_add_remove_port success.\n"));
	return MSD_OK;
}

/*
*   This sample demonstrate
*   Match the TCP over Ipv6 frame with specified DA, ipv6 and Tcp header, details as follows:
*   1. Match Ethernet Frame DA = AA:BB:CC:DD:EE:00
*   2. Match the Vlan Id = 0x1
*   3. Match partial of the Ipv6 Destination Address: xxxx:xxxx:x122:xxxx:3456:xxxx:xxxx:5x5A
*   4. Match the Tcp source port = 0x4
*   5. Frame meets above conditions will egress from port1 and port2
*/
MSD_STATUS sample_msdTcam_Adv(MSD_U8 devNum)
{
    MSD_STATUS retVal;
    MSD_TCAM_PKT_TYPE pktType;
    MSD_U32 tcamPointer;
    MSD_U32 tcamPointer2;
    MSD_TCAM_ENT_KEY keyPtr;
    MSD_TCAM_ENT_KEY keyMaskPtr;
    MSD_TCAM_ENT_PKT_UNT patternPtr;
    MSD_TCAM_ENT_PKT_UNT maskPtr;
    MSD_TCAM_ENT_ACT actionPtr;
    MSD_BOOL entry2Used;

    /*Initialize */
    tcamPointer = 0;
    tcamPointer2 = 1;
    msdMemSet(&keyPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    msdMemSet(&keyMaskPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    msdMemSet(&patternPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    msdMemSet(&maskPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    msdMemSet(&actionPtr, 0, sizeof(MSD_TCAM_ENT_ACT));

    /*Setting the packet type*/
    pktType = MSD_TCAM_TYPE_IPV6_TCP;

    /*key mask setting*/
    keyPtr.frameType = 0; /*Normal Port*/
    keyMaskPtr.frameType = 0x3; /*Mask type*/

    /*Match the frame DA = AA:BB:CC:DD:EE:00*/
    patternPtr.da.arEther[0] = 0xAA;
    patternPtr.da.arEther[1] = 0xBB;
    patternPtr.da.arEther[2] = 0xCC;
    patternPtr.da.arEther[3] = 0xDD;
    patternPtr.da.arEther[4] = 0xEE;
    patternPtr.da.arEther[5] = 0x00;
    maskPtr.da.arEther[0] = 0xff;
    maskPtr.da.arEther[1] = 0xff;
    maskPtr.da.arEther[2] = 0xff;
    maskPtr.da.arEther[3] = 0xff;
    maskPtr.da.arEther[4] = 0xff;
    maskPtr.da.arEther[5] = 0xff;

    /*Match the vlan Id = 0x1*/
    patternPtr.vlanTag = 0x1;
    maskPtr.vlanTag = 0xfff; /*vlan id take 12bits*/

    /* Match Ethernet Frame Type = 0x86DD(ipv6) */
    patternPtr.etherType = 0x86DD;
    maskPtr.etherType = 0xffff;

    /* Match IPv6 partial of Destination Address: xxxx:xxxx:x122:xxxx:3456:xxxx:xxxx:5x5A*/
    patternPtr.pktUnit.ipv6Tcp.ipv6.dip[2] = 0x0122;
    patternPtr.pktUnit.ipv6Tcp.ipv6.dip[4] = 0x3456;
    patternPtr.pktUnit.ipv6Tcp.ipv6.dip[7] = 0x505A;
    maskPtr.pktUnit.ipv6Tcp.ipv6.dip[2] = 0x0fff;
    maskPtr.pktUnit.ipv6Tcp.ipv6.dip[4] = 0xffff;
    maskPtr.pktUnit.ipv6Tcp.ipv6.dip[7] = 0xf0ff;

    /*Match Tcp source port*/
    patternPtr.pktUnit.ipv6Tcp.tcp.srcPort = 0x4;
    maskPtr.pktUnit.ipv6Tcp.tcp.srcPort = 0xffff;

    /*Action: frame meets above conditions will egress from port1 and port2*/
    actionPtr.dpvMode = 0x03; /* Replace the frame¡¯s DPV with the TCAM Entry¡¯s DPV Data */
    actionPtr.dpvData = 0x06; /* B0000110, frame hit this entry will egress from port1 and port2*/

    /*operation*/
    retVal = msdTcamAdvConfig(devNum, pktType, tcamPointer, tcamPointer2, &keyMaskPtr, &keyPtr, &maskPtr, &patternPtr, &actionPtr, &entry2Used);

    if (retVal != MSD_OK)
    {
        MSG_PRINT(("msdTcamAdvConfig returned fail.\n"));
        return retVal;
    }

	/*setting correct TCAM mode for the ingress port (here take port 0 as example) based on the entry used*/
    if (entry2Used == MSD_FALSE)
    {
		msdPortTcamModeSet(devNum, 0, MSD_TCAM_MODE_ENABLE_48);
        MSG_PRINT(("Tcam entry %d is not used for this setting.\n", tcamPointer2));
	}
	else
	{
		msdPortTcamModeSet(devNum, 0, MSD_TCAM_MODE_ENABLE_96);
		MSG_PRINT(("Tcam entry %d and %d is cascaded for this setting.\n", tcamPointer, tcamPointer2));
	}

    return retVal;
}