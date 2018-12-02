#include <msdCopyright.h>
/********************************************************************************
* sample_msdPortRmon.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Get all counters of the given port.
*    2. Flush all counters for a port.
*    3. Get a specific counter of the given port.
*    4. Flush all counters for all ports.
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

void displayCounter(MSD_STATS_COUNTER_SET *statsCounter);

MSD_STATUS sample_msdPortRmon(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_LPORT port;
	MSD_STATS_COUNTER_SET statsCounterSet;

	MSD_STATS_COUNTERS counter;
	MSD_U32 statsData;

	/*
	 *    Get all counters of the given port(port0).
	 */
	port = 0;
	if ((status = msdStatsPortAllCountersGet(devNum, port, &statsCounterSet)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsPortAllCountersGet returned fail.\n"));
		return status;
	}
	/* display the counter */
	displayCounter(&statsCounterSet);

	/*
	 *    Flush all counters of this port(port0).
	 */
	if ((status = msdStatsFlushPort(devNum, port)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsFlushPort returned fail.\n"));
		return status;
	}

	/*
	 *    Get a specific counter(e.g. STATS_MaxOctets) of this given port(port0)
	 *    statsDate should be 0 after msdStatsFlushPort above
	 */
    counter = MSD_STATS_MaxOctets;
	if ((status = msdStatsPortCounterGet(devNum, port, counter, &statsData)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsPortCounterGet returned fail.\n"));
		return status;
	}

	/*
	 *    Flush all counters for all ports.
	 */
	if ((status = msdStatsFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsFlushAll returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPortRmon sample code success.\n"));
	return MSD_OK;
}

void displayCounter(MSD_STATS_COUNTER_SET *statsCounter)
{
	/* Bank 0 */
	MSG_PRINT(("InGoodOctetsLo  %08lu   \n", statsCounter->InGoodOctetsLo));
	MSG_PRINT(("InGoodOctetsHi  %08lu   \n", statsCounter->InGoodOctetsHi));
	MSG_PRINT(("InBadOctets     %08lu   \n", statsCounter->InBadOctets));
	MSG_PRINT(("OutFCSErr       %08lu   \n", statsCounter->OutFCSErr));
	MSG_PRINT(("InUnicasts      %08lu   \n", statsCounter->InUnicasts));
	MSG_PRINT(("Deferred        %08lu   \n", statsCounter->Deferred));
	MSG_PRINT(("InBroadcasts    %08lu   \n", statsCounter->InBroadcasts));
	MSG_PRINT(("InMulticasts    %08lu   \n", statsCounter->InMulticasts));
    /*
        Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
        (refer to Histogram Mode)
    */
	MSG_PRINT(("Octets64        %08lu   \n", statsCounter->Octets64));
	MSG_PRINT(("Octets127       %08lu   \n", statsCounter->Octets127));
	MSG_PRINT(("Octets255       %08lu   \n", statsCounter->Octets255));
	MSG_PRINT(("Octets511       %08lu   \n", statsCounter->Octets511));
	MSG_PRINT(("Octets1023      %08lu   \n", statsCounter->Octets1023));
	MSG_PRINT(("OctetsMax       %08lu   \n", statsCounter->OctetsMax));
	MSG_PRINT(("OutOctetsLo     %08lu   \n", statsCounter->OutOctetsLo));
	MSG_PRINT(("OutOctetsHi     %08lu   \n", statsCounter->OutOctetsHi));
	MSG_PRINT(("OutUnicasts     %08lu   \n", statsCounter->OutUnicasts));
	MSG_PRINT(("Excessive       %08lu   \n", statsCounter->Excessive));
	MSG_PRINT(("OutMulticasts   %08lu   \n", statsCounter->OutMulticasts));
	MSG_PRINT(("OutBroadcasts   %08lu   \n", statsCounter->OutBroadcasts));
	MSG_PRINT(("Single          %08lu   \n", statsCounter->Single));

	MSG_PRINT(("OutPause        %08lu   \n", statsCounter->OutPause));
	MSG_PRINT(("InPause         %08lu   \n", statsCounter->InPause));
	MSG_PRINT(("Multiple        %08lu   \n", statsCounter->Multiple));
	MSG_PRINT(("InUndersize     %08lu   \n", statsCounter->InUndersize));
	MSG_PRINT(("InFragments     %08lu   \n", statsCounter->InFragments));
	MSG_PRINT(("InOversize      %08lu   \n", statsCounter->InOversize));
	MSG_PRINT(("InJabber        %08lu   \n", statsCounter->InJabber));
	MSG_PRINT(("InRxErr         %08lu   \n", statsCounter->InRxErr));
	MSG_PRINT(("InFCSErr        %08lu   \n", statsCounter->InFCSErr));
	MSG_PRINT(("Collisions      %08lu   \n", statsCounter->Collisions));
	MSG_PRINT(("Late            %08lu   \n", statsCounter->Late));

	/* Bank 1 */
	MSG_PRINT(("InDiscards      %08lu   \n", statsCounter->InDiscards));
	MSG_PRINT(("InFiltered      %08lu   \n", statsCounter->InFiltered));
	MSG_PRINT(("InAccepted      %08lu   \n", statsCounter->InAccepted));
	MSG_PRINT(("InBadAccepted   %08lu   \n", statsCounter->InBadAccepted));
	MSG_PRINT(("InGoodAvbClassA %08lu   \n", statsCounter->InGoodAvbClassA));
	MSG_PRINT(("InGoodAvbClassB %08lu   \n", statsCounter->InGoodAvbClassB));
	MSG_PRINT(("InBadAvbClassA  %08lu   \n", statsCounter->InBadAvbClassA));
	MSG_PRINT(("InBadAvbClassB  %08lu   \n", statsCounter->InBadAvbClassB));
	MSG_PRINT(("TCAMCounter0    %08lu   \n", statsCounter->TCAMCounter0));
	MSG_PRINT(("TCAMCounter1    %08lu   \n", statsCounter->TCAMCounter1));
	MSG_PRINT(("TCAMCounter2    %08lu   \n", statsCounter->TCAMCounter2));
	MSG_PRINT(("TCAMCounter3    %08lu   \n", statsCounter->TCAMCounter3));
	MSG_PRINT(("InDroppedAvbA   %08lu   \n", statsCounter->InDroppedAvbA));
	MSG_PRINT(("InDroppedAvbB   %08lu   \n", statsCounter->InDroppedAvbB));
	MSG_PRINT(("InDaUnknown     %08lu   \n", statsCounter->InDaUnknown));
	MSG_PRINT(("InMGMT          %08lu   \n", statsCounter->InMGMT));
	MSG_PRINT(("OutQueue0       %08lu   \n", statsCounter->OutQueue0));
	MSG_PRINT(("OutQueue1       %08lu   \n", statsCounter->OutQueue1));
	MSG_PRINT(("OutQueue2       %08lu   \n", statsCounter->OutQueue2));
	MSG_PRINT(("OutQueue3       %08lu   \n", statsCounter->OutQueue3));
	MSG_PRINT(("OutQueue4       %08lu   \n", statsCounter->OutQueue4));
	MSG_PRINT(("OutQueue5       %08lu   \n", statsCounter->OutQueue5));
	MSG_PRINT(("OutQueue6       %08lu   \n", statsCounter->OutQueue6));
	MSG_PRINT(("OutQueue7       %08lu   \n", statsCounter->OutQueue7));
	MSG_PRINT(("OutCutThrough   %08lu   \n", statsCounter->OutCutThrough));
	MSG_PRINT(("reserved_19     %08lu   \n", statsCounter->reserved_19));
	MSG_PRINT(("OutOctetsA      %08lu   \n", statsCounter->OutOctetsA));
	MSG_PRINT(("OutOctetsB      %08lu   \n", statsCounter->OutOctetsB));
	MSG_PRINT(("reserved_1C     %08lu   \n", statsCounter->OutYel));
	MSG_PRINT(("OutDroppedYel   %08lu   \n", statsCounter->OutDroppedYel));
	MSG_PRINT(("OutDiscards     %08lu   \n", statsCounter->OutDiscards));
	MSG_PRINT(("OutMGMT         %08lu   \n", statsCounter->OutMGMT));
}
