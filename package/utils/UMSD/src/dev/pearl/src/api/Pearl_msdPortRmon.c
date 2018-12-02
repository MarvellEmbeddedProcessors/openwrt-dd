#include <msdCopyright.h>

/*******************************************************************************
* Pearl_msdPortCounter.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdPortRmon.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>


/* Definitions for MIB Counter */
#define PEARL_MSD_STATS_NO_OP               0x0
#define PEARL_MSD_STATS_FLUSH_ALL           0x1
#define PEARL_MSD_STATS_FLUSH_PORT          0x2
#define PEARL_MSD_STATS_READ_COUNTER        0x4
#define PEARL_MSD_STATS_CAPTURE_PORT        0x5
#define PEARL_MSD_STATS_CAPTURE_PORT_CLEAR  0x6

/*
 *    Type definition for MIB counter operation
*/
typedef enum 
{
    Pearl_STATS_FLUSH_ALL,            /* Flush all counters for all ports */
    Pearl_STATS_FLUSH_PORT,           /* Flush all counters for a port */
    Pearl_STATS_READ_COUNTER,         /* Read a specific counter from a port */
    Pearl_STATS_READ_REALTIME_COUNTER,  /* Read a realtime counter from a port */
    Pearl_STATS_READ_ALL,              /* Read all counters from a port */
    Pearl_STATS_READ_COUNTER_CLEAR,   /* For RMU page2,Read a specific counter from a port and clear*/
    Pearl_STATS_READ_ALL_CLEAR        /* For RMU page2,Read all counters from a port and clear*/
} PEARL_MSD_STATS_OPERATION;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Pearl_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   PEARL_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   PEARL_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
);

static MSD_STATUS Pearl_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
);

/*******************************************************************************
* Pearl_gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsFlushAll
(
        IN MSD_QD_DEV  *dev
)
{
    MSD_STATUS           retVal;

    MSD_DBG_INFO(("Pearl_gstatsFlushAll Called.\n"));

    retVal = Pearl_statsOperationPerform(dev,Pearl_STATS_FLUSH_ALL,0,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_statsOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gstatsFlushAll Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Pearl_gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsFlushPort
(
    IN MSD_QD_DEV  *dev,
    IN MSD_LPORT      port
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Pearl_gstatsFlushPort Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Pearl_statsOperationPerform(dev,Pearl_STATS_FLUSH_PORT,hwPort,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_statsOperationPerform FLUSH_PORT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gstatsFlushPort Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Pearl_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  PEARL_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Pearl_gstatsGetPortCounter Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    if (counter > PEARL_MSD_TYPE_BANK + 0x1F || counter < 0 || (counter > 0x1f && counter < PEARL_MSD_TYPE_BANK))
    {
        MSD_DBG_ERROR(("Bad counter: %d.\n", (int)counter));
        return MSD_BAD_PARAM;
    }

    retVal = Pearl_statsOperationPerform(dev,Pearl_STATS_READ_COUNTER,hwPort,counter,(MSD_VOID*)statsData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_statsOperationPerform READ_COUNTER returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gstatsGetPortCounter Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Pearl_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to MSD_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT PEARL_MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Pearl_gstatsGetPortAllCounters Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Pearl_statsOperationPerform(dev,Pearl_STATS_READ_ALL,hwPort,0,(MSD_VOID*)statsCounterSet);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_statsOperationPerform READ_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gstatsGetPortAllCounters Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Pearl_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
    OUT PEARL_MSD_HISTOGRAM_MODE    *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Pearl_gstatsGetHistogramMode Called.\n"));
	
    if(mode == NULL)
    {
		MSD_DBG_ERROR(("Mode is NULL.\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the Histogram mode bit.                */
	retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL1_DEV_ADDR, PEARL_QD_REG_STATS_OPERATION, 10, 2, &data);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Software definition starts from 0 ~ 2, while hardware supports the values from 1 to 3 */
	switch (data)
	{
	case 0x1:
		*mode = PEARL_MSD_COUNT_RX_ONLY;
		break;
	case 0x2:
		*mode = PEARL_MSD_COUNT_TX_ONLY;
		break;
	case 0x3:
		*mode = PEARL_MSD_COUNT_RX_TX;
		break;
	default:
		MSD_DBG_ERROR(("Read back Bad Mode: %u.\n", (unsigned int)data));
		return MSD_BAD_PARAM;
	}
	
    MSD_DBG_INFO(("Pearl_gstatsGetHistogramMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Pearl_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
    IN PEARL_MSD_HISTOGRAM_MODE        mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Pearl_gstatsSetHistogramMode Called.\n"));

    switch (mode)
    {
	    case PEARL_MSD_COUNT_RX_ONLY:
	    case PEARL_MSD_COUNT_TX_ONLY:
	    case PEARL_MSD_COUNT_RX_TX:
            break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)mode));
            return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    /* Set the Histogram mode bit.                */
	retVal = msdSetAnyRegField(dev->devNum, PEARL_GLOBAL1_DEV_ADDR, PEARL_QD_REG_STATS_OPERATION, 10, 2, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gstatsSetHistogramMode Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Pearl_statsOperationPerform
*
* DESCRIPTION:
*       This function is used by all stats control functions, and is responsible
*       to write the required operation into the stats registers.
*
* INPUTS:
*       statsOp       - The stats operation bits to be written into the stats
*                     operation register.
*       port        - port number
*       counter     - counter to be read if it's read operation
*
* OUTPUTS:
*       statsData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/

static MSD_STATUS Pearl_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   PEARL_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   PEARL_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;			/* Data to be set into the      */
    MSD_U32	tmpCounter;
    MSD_U32	startCounter;
    MSD_U32	lastCounter;
    MSD_U16	portNum;
	MSD_U8 	bank;

    msdSemTake(dev->devNum, dev->statsRegsSem,OS_WAIT_FOREVER);

	portNum = (port + 1) << 5;

    /* Wait until the stats in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

    /* Set the STAT Operation register */
    switch (statsOp)
    {
        case Pearl_STATS_FLUSH_ALL:
            data |= (1 << 15) | (PEARL_MSD_STATS_FLUSH_ALL << 12);
            retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,data);
			
			data |= (1 << 15) | (PEARL_MSD_STATS_FLUSH_ALL << 12) | (1<<10);
			retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,data);

            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

        case Pearl_STATS_FLUSH_PORT:
            data |= (1 << 15) | (PEARL_MSD_STATS_FLUSH_PORT << 12) | portNum ;
            retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,data);
			data |= (1 << 15) | (PEARL_MSD_STATS_FLUSH_PORT << 12) | portNum | (1<<10);
			retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,data);
			
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

        case Pearl_STATS_READ_COUNTER:
            retVal = Pearl_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->statsRegsSem);
                return retVal;
            }
            break;

        case Pearl_STATS_READ_ALL:
			{
				int bankSize = 2;
				for(bank=0; bank<bankSize; bank++)
				{
					lastCounter = (bank==0)?(MSD_U32)Pearl_STATS_Late : (MSD_U32)Pearl_STATS_OutMGMT;
					startCounter = (bank==0)?(MSD_U32)Pearl_STATS_InGoodOctetsLo : (MSD_U32)Pearl_STATS_InDiscards;

					if(bank==1)
						statsData = (unsigned int *)statsData + Pearl_STATS_Late +1;
					for(tmpCounter=startCounter; tmpCounter<=lastCounter; tmpCounter++)
					{
						retVal = Pearl_statsReadCounter(dev, portNum, tmpCounter,((MSD_U32*)statsData+tmpCounter-startCounter));
						if(retVal != MSD_OK)
						{
							msdSemGive(dev->devNum, dev->statsRegsSem);
							return retVal;
						}
					}
				}
			}
			break;
			
        default:
            
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return MSD_FAIL;
    }

    msdSemGive(dev->devNum, dev->statsRegsSem);
    return MSD_OK;
}

/*******************************************************************************
* Pearl_statsReadCounter
*
* DESCRIPTION:
*       This function is used to read a captured counter.
*
* INPUTS:
*       port        - port number
*       counter     - counter to be read if it's read operation
*
* OUTPUTS:
*       statsData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*        If Semaphore is used, Semaphore should be acquired before this function call.
*******************************************************************************/
static MSD_STATUS Pearl_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
)
{
    MSD_STATUS   retVal;         /* Functions return value.            */
    MSD_U16      data;/* Data to be set into the  register. */ 
    MSD_U16    counter3_2;     /* Counter Register Bytes 3 & 2       */
    MSD_U16    counter1_0;     /* Counter Register Bytes 1 & 0       */
	
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}
	
    data = (MSD_U16)((1 << 15) | (PEARL_MSD_STATS_READ_COUNTER << 12) | port | (counter&0x1f) );
	if (counter & PEARL_MSD_TYPE_BANK)
	{
		data |= (1<<9);
	}

    retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,data);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}

    retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_COUNTER3_2,&counter3_2);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_STATS_COUNTER1_0,&counter1_0);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    *statsData = (counter3_2 << 16) | counter1_0;

    return MSD_OK;

}
/*******************************************************************************
* Pearl_gmibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Pearl_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
)
{
	MSD_STATUS status;
	PEARL_MSD_STATS_COUNTER_SET statsCounter;
	msdMemSet(&statsCounter, 0, sizeof(PEARL_MSD_STATS_COUNTER_SET));

	if ((status = Pearl_gstatsGetPortAllCounters(dev, portNum, &statsCounter)) != MSD_OK)
	{
		return status;
	}
	/* Bank 0 */
	MSG(("InGoodOctetsLo  %08lu   \n", statsCounter.InGoodOctetsLo));
	MSG(("InGoodOctetsHi  %08lu   \n", statsCounter.InGoodOctetsHi));
	MSG(("InBadOctets     %08lu   \n", statsCounter.InBadOctets));
	MSG(("OutFCSErr       %08lu   \n", statsCounter.OutFCSErr));
	MSG(("InUnicasts      %08lu   \n", statsCounter.InUnicasts));
	MSG(("Deferred        %08lu   \n", statsCounter.Deferred));
	MSG(("InBroadcasts    %08lu   \n", statsCounter.InBroadcasts));
	MSG(("InMulticasts    %08lu   \n", statsCounter.InMulticasts));
	/*
	Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
	(refer to Histogram Mode)
	*/
	MSG(("Octets64        %08lu   \n", statsCounter.Octets64));
	MSG(("Octets127       %08lu   \n", statsCounter.Octets127));
	MSG(("Octets255       %08lu   \n", statsCounter.Octets255));
	MSG(("Octets511       %08lu   \n", statsCounter.Octets511));
	MSG(("Octets1023      %08lu   \n", statsCounter.Octets1023));
	MSG(("OctetsMax       %08lu   \n", statsCounter.OctetsMax));
	MSG(("OutOctetsLo     %08lu   \n", statsCounter.OutOctetsLo));
	MSG(("OutOctetsHi     %08lu   \n", statsCounter.OutOctetsHi));
	MSG(("OutUnicasts     %08lu   \n", statsCounter.OutUnicasts));
	MSG(("Excessive       %08lu   \n", statsCounter.Excessive));
	MSG(("OutMulticasts   %08lu   \n", statsCounter.OutMulticasts));
	MSG(("OutBroadcasts   %08lu   \n", statsCounter.OutBroadcasts));
	MSG(("Single          %08lu   \n", statsCounter.Single));

	MSG(("OutPause        %08lu   \n", statsCounter.OutPause));
	MSG(("InPause         %08lu   \n", statsCounter.InPause));
	MSG(("Multiple        %08lu   \n", statsCounter.Multiple));
	MSG(("InUndersize     %08lu   \n", statsCounter.InUndersize));
	MSG(("InFragments     %08lu   \n", statsCounter.InFragments));
	MSG(("InOversize      %08lu   \n", statsCounter.InOversize));
	MSG(("InJabber        %08lu   \n", statsCounter.InJabber));
	MSG(("InRxErr         %08lu   \n", statsCounter.InRxErr));
	MSG(("InFCSErr        %08lu   \n", statsCounter.InFCSErr));
	MSG(("Collisions      %08lu   \n", statsCounter.Collisions));
	MSG(("Late            %08lu   \n", statsCounter.Late));

	/* Bank 1 */
	MSG(("InDiscards      %08lu   \n", statsCounter.InDiscards));
	MSG(("InFiltered      %08lu   \n", statsCounter.InFiltered));
	MSG(("InAccepted      %08lu   \n", statsCounter.InAccepted));
	MSG(("InBadAccepted   %08lu   \n", statsCounter.InBadAccepted));
	MSG(("InGoodAvbClassA %08lu   \n", statsCounter.InGoodAvbClassA));
	MSG(("InGoodAvbClassB %08lu   \n", statsCounter.InGoodAvbClassB));
	MSG(("InBadAvbClassA  %08lu   \n", statsCounter.InBadAvbClassA));
	MSG(("InBadAvbClassB  %08lu   \n", statsCounter.InBadAvbClassB));
	MSG(("TCAMCounter0    %08lu   \n", statsCounter.TCAMCounter0));
	MSG(("TCAMCounter1    %08lu   \n", statsCounter.TCAMCounter1));
	MSG(("TCAMCounter2    %08lu   \n", statsCounter.TCAMCounter2));
	MSG(("TCAMCounter3    %08lu   \n", statsCounter.TCAMCounter3));
	MSG(("InDroppedAvbA   %08lu   \n", statsCounter.InDroppedAvbA));
	MSG(("InDroppedAvbB   %08lu   \n", statsCounter.InDroppedAvbB));
	MSG(("InDaUnknown     %08lu   \n", statsCounter.InDaUnknown));
	MSG(("InMGMT          %08lu   \n", statsCounter.InMGMT));
	MSG(("OutQueue0       %08lu   \n", statsCounter.OutQueue0));
	MSG(("OutQueue1       %08lu   \n", statsCounter.OutQueue1));
	MSG(("OutQueue2       %08lu   \n", statsCounter.OutQueue2));
	MSG(("OutQueue3       %08lu   \n", statsCounter.OutQueue3));
	MSG(("OutQueue4       %08lu   \n", statsCounter.OutQueue4));
	MSG(("OutQueue5       %08lu   \n", statsCounter.OutQueue5));
	MSG(("OutQueue6       %08lu   \n", statsCounter.OutQueue6));
	MSG(("OutQueue7       %08lu   \n", statsCounter.OutQueue7));
	MSG(("OutCutThrough   %08lu   \n", statsCounter.OutCutThrough));
	MSG(("reserved_19     %08lu   \n", statsCounter.reserved_19));
	MSG(("OutOctetsA      %08lu   \n", statsCounter.OutOctetsA));
	MSG(("OutOctetsB      %08lu   \n", statsCounter.OutOctetsB));
	MSG(("reserved_1C     %08lu   \n", statsCounter.reserved_1C));
	MSG(("OutDroppedYel   %08lu   \n", statsCounter.OutDroppedYel));
	MSG(("OutDiscards     %08lu   \n", statsCounter.OutDiscards));
	MSG(("OutMGMT         %08lu   \n", statsCounter.OutMGMT));

	MSG(("\n"));
	return MSD_OK;
}