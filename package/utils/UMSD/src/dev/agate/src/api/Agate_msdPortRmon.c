#include <msdCopyright.h>

/*******************************************************************************
* Agate_msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/


#include <agate/include/api/Agate_msdPortRmon.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


/* Definitions for MIB Counter */
/*
#define AGATE_MSD_STATS_NO_OP               0x0
#define AGATE_MSD_STATS_FLUSH_ALL           0x1
#define AGATE_MSD_STATS_FLUSH_PORT          0x2
#define AGATE_MSD_STATS_READ_COUNTER        0x4
#define AGATE_MSD_STATS_CAPTURE_PORT        0x5
*/

/*
 *    Type definition for MIB counter operation
*/
typedef enum 
{
	Agate_STATS_FLUSH_ALL = 1,            /* Flush all counters for all ports */
	Agate_STATS_FLUSH_PORT = 2,           /* Flush all counters for a port */
	Agate_STATS_READ_COUNTER = 4,         /* Read a specific counter from a port */
	Agate_STATS_READ_ALL = 5,              /* Read all counters from a port */
} AGATE_MSD_STATS_OPERATION;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Agate_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   AGATE_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
	IN   AGATE_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
);

static MSD_STATUS Agate_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
);

/*******************************************************************************
* Agate_gstatsFlushAll
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
MSD_STATUS Agate_gstatsFlushAll
(
        IN MSD_QD_DEV  *dev
)
{
    MSD_STATUS           retVal;

    MSD_DBG_INFO(("Agate_gstatsFlushAll Called.\n"));

    retVal = Agate_statsOperationPerform(dev,Agate_STATS_FLUSH_ALL,0,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_statsOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gstatsFlushAll Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Agate_gstatsFlushPort
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
MSD_STATUS Agate_gstatsFlushPort
(
    IN MSD_QD_DEV  *dev,
    IN MSD_LPORT      port
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Agate_gstatsFlushPort Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Agate_statsOperationPerform(dev,Agate_STATS_FLUSH_PORT,hwPort,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_statsOperationPerform FLUSH_PORT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gstatsFlushPort Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Agate_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine Agate_gets a specific counter of the given port
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
MSD_STATUS Agate_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  AGATE_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Agate_gstatsGetPortCounter Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    if (counter > 0x1F || counter < 0 )
    {
        MSD_DBG_ERROR(("Bad counter: %d.\n", (int)counter));
        return MSD_BAD_PARAM;
    }

	retVal = Agate_statsOperationPerform(dev, Agate_STATS_READ_COUNTER, hwPort, counter, (MSD_VOID*)statsData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_statsOperationPerform READ_COUNTER returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gstatsGetPortCounter Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Agate_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine Agate_gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to AGATE_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Agate_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT AGATE_MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Agate_gstatsGetPortAllCounters Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Agate_statsOperationPerform(dev,Agate_STATS_READ_ALL,hwPort,0,(MSD_VOID*)statsCounterSet);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_statsOperationPerform READ_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gstatsGetPortAllCounters Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Agate_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine Agate_gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (AGATE_MSD_COUNT_RX_ONLY, AGATE_MSD_COUNT_TX_ONLY, 
*                    and AGATE_MSD_COUNT_RX_TX)
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
MSD_STATUS Agate_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
    OUT AGATE_MSD_HISTOGRAM_MODE    *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Agate_gstatsGetHistogramMode Called.\n"));
	
    if(mode == NULL)
    {
		MSD_DBG_ERROR(("Mode is NULL.\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the Histogram mode bit.                */
	retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_STATS_OPERATION, 10, 2, &data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Software definition starts from 0 ~ 3, while hardware supports the values from 1 to 3 */
	switch(data)
	{
	    case 0x1:
			*mode = AGATE_MSD_COUNT_RX_ONLY;
			break;
		case 0x2:
			*mode = AGATE_MSD_COUNT_TX_ONLY;
			break;
		case 0x3:
			*mode = AGATE_MSD_COUNT_RX_TX;
			break;
        default:
			MSD_DBG_ERROR(("Read back Bad Mode: %u.\n", (unsigned int)data));
            return MSD_BAD_PARAM;
	}

    MSD_DBG_INFO(("Agate_gstatsGetHistogramMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (AGATE_MSD_COUNT_RX_ONLY, AGATE_MSD_COUNT_TX_ONLY, 
*                    and AGATE_MSD_COUNT_RX_TX)
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
MSD_STATUS Agate_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
    IN AGATE_MSD_HISTOGRAM_MODE        mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Agate_gstatsSetHistogramMode Called.\n"));

    switch (mode)
    {
        case AGATE_MSD_COUNT_RX_ONLY:
			data = 1;
			break;
        case AGATE_MSD_COUNT_TX_ONLY:
			data = 2;
			break;
        case AGATE_MSD_COUNT_RX_TX:
			data = 3;
            break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)mode));
            return MSD_BAD_PARAM;
    }

    /*data = (MSD_U16)mode;*/

    /* Set the Histogram mode bit.                */
	retVal = msdSetAnyRegField(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_STATS_OPERATION, 10, 2, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gstatsSetHistogramMode Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Agate_statsOperationPerform
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
*		None.
*
*******************************************************************************/

static MSD_STATUS Agate_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   AGATE_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   AGATE_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;			/* Data to be set into the      */
    MSD_U32	tmpCounter;
    MSD_U32	startCounter;
    MSD_U32	lastCounter;
    MSD_U16	portNum;
	

    msdSemTake(dev->devNum, dev->statsRegsSem,OS_WAIT_FOREVER);

	portNum = (port + 1) << 5;

    /* Wait until the stats in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

	retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,&data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->statsRegsSem);
		return retVal;
	}
	data &= 0xfff;
    /* Set the STAT Operation register */
    switch (statsOp)
    {
        case Agate_STATS_FLUSH_ALL:
            data |= (1 << 15) | (Agate_STATS_FLUSH_ALL << 12);
            retVal = msdSetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,data);

            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

        case Agate_STATS_FLUSH_PORT:
			data &= 0xc1f;
            data |= (1 << 15) | (Agate_STATS_FLUSH_PORT << 12) | portNum ;
            retVal = msdSetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,data);
			
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

		case Agate_STATS_READ_COUNTER:
            retVal = Agate_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->statsRegsSem);
                return retVal;
            }
            break;

        case Agate_STATS_READ_ALL:
			{
				
				
				
				lastCounter = (MSD_U32)Agate_STATS_Late;
				startCounter = (MSD_U32)Agate_STATS_InGoodOctetsLo;
					
				for(tmpCounter=startCounter; tmpCounter<=lastCounter; tmpCounter++)
				{
					retVal = Agate_statsReadCounter(dev, portNum, tmpCounter,((MSD_U32*)statsData+tmpCounter-startCounter));
					if(retVal != MSD_OK)
					{
						msdSemGive(dev->devNum, dev->statsRegsSem);
						return retVal;
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
* Agate_statsReadCounter
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
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*
* COMMENTS:
*        If Semaphore is used, Semaphore should be acquired before this function call.
*******************************************************************************/
static MSD_STATUS Agate_statsReadCounter
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
        retVal = msdGetAnyRegField(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}
	
    data = (MSD_U16)((1 << 15) | (Agate_STATS_READ_COUNTER << 12) | port | (counter&0x1f) );
	if (counter)
	{
		data |= (1<<10);
	}

    retVal = msdSetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,data);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR,AGATE_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}

	retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_STATS_COUNTER3_2, &counter3_2);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_STATS_COUNTER1_0, &counter1_0);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    *statsData = (counter3_2 << 16) | counter1_0;

    return MSD_OK;

}
/*******************************************************************************
* Agate_gmibDump
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
MSD_STATUS Agate_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
)
{
	MSD_STATUS status;
	AGATE_MSD_STATS_COUNTER_SET statsCounter; 
	msdMemSet(&statsCounter, 0, sizeof(AGATE_MSD_STATS_COUNTER_SET));
	if ((status = Agate_gstatsGetPortAllCounters(dev, portNum, &statsCounter)) != MSD_OK)
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
	
	MSG(("\n"));
	return MSD_OK;

}

