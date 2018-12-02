#include <msdCopyright.h>

/*******************************************************************************
* Topaz_msdPIRL.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdPIRL.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>


/*
 * Typedef: enum TOPAZ_MSD_PIRL_OPERATION
 *
 * Description: Defines the PIRL (Port Ingress Rate Limit) Operation type
 *
 * Fields:
 *      Topaz_PIRL_INIT_ALL_RESOURCE - Initialize all resources to the inital state
 *      Topaz_PIRL_INIT_RESOURCE     - Initialize selected resources to the inital state
 *      Topaz_PIRL_WRITE_RESOURCE    - Write to the selected resource/register
 *      Topaz_PIRL_READ_RESOURCE     - Read from the selected resource/register
 */
typedef enum
{
    Topaz_PIRL_INIT_ALL_RESOURCE      = 0x1,
    Topaz_PIRL_INIT_RESOURCE          = 0x2,
    Topaz_PIRL_WRITE_RESOURCE         = 0x3,
    Topaz_PIRL_READ_RESOURCE          = 0x4
} TOPAZ_MSD_PIRL_OPERATION;


/*
 *  typedef: struct TOPAZ_MSD_PIRL_OP_DATA
 *
 *  Description: data required by PIRL Operation
 *
 *  Fields:
 *      irlUnit   - Ingress Rate Limit Unit that defines one of IRL resources.
 *      irlReg    - Ingress Rate Limit Register.
 *      irlData   - Ingress Rate Limit Data.
 */
typedef struct
{
    MSD_U32    irlUnit;
    MSD_U32    irlReg;
    MSD_U32    irlData;
} TOPAZ_MSD_PIRL_OP_DATA;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Topaz_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    TOPAZ_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT TOPAZ_MSD_PIRL_OP_DATA     *opData
);
static MSD_STATUS Topaz_UpIntToConstant
(
	IN   MSD_U32 TimeUpInt,
	OUT  TOPAZ_MSD_PIRL_DATA    *pirlData
 );

static MSD_STATUS Topaz_ConstantToUpInt
(
 
	IN    TOPAZ_MSD_PIRL_DATA    *pirlData,
	OUT   MSD_U32 *TimeUpInt
 );

/*******************************************************************************
* Topaz_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes PIRL Resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlInitialize
(
    IN  MSD_QD_DEV              *dev
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    TOPAZ_MSD_PIRL_OPERATION    op;

    MSD_DBG_INFO(("Topaz_gpirlInitialize Called.\n"));

    op = Topaz_PIRL_INIT_ALL_RESOURCE;

    retVal = Topaz_pirlOperationPerform(dev, op, 0, 0, 0, NULL);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_INIT_ALL_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gpirlInitialize Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource's related counters for a particular port.
*
* INPUTS:
*       irlPort	- target port
*		irlRes - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlInitResource
(
    IN    MSD_QD_DEV            *dev,
	IN    MSD_LPORT             irlPort,
    IN    MSD_U32               irlRes
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    TOPAZ_MSD_PIRL_OPERATION    op;
    TOPAZ_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Topaz_gpirlInitResource Called.\n"));

    op = Topaz_PIRL_INIT_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
		return MSD_BAD_PARAM;
	}


	if(irlRes > 4)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	
    retVal = Topaz_pirlOperationPerform(dev, op, port, irlRes, 0, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_INIT_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gpirlInitResource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gpirlCustomSetup
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRF/Constant/Delta according to user 
*       specific target rate, target burst size and countMode.

*
* INPUTS:
*       tgtRate	- target rate(units is fps when countMode = MSD_PIRL_COUNT_FRAME,
*				  otherwise units is bps)
*       tgtBstSize	- target burst size(units is Byte)
*       customSetup - Constant and countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRF/Constant/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		As the input for structure 'MSD_PIRL_CUSTOM_RATE_LIMIT', Constant can be 0 or any  
*		value from {7812500, 125000000, 62500000, 31250000, 15625000, 3906250, 1953750, 976250}.
*		If Constant is 0, this API will automatically calculate the best IRL Constant 
*		which means Delta is small enough or 0. However, since Constant is Global for all
*       Resource of all ports, user can specify fixed Constant to gurantee that the Constant is 
*		consistent across all existing resources.
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlCustomSetup
(
    IN  MSD_U32	   	tgtRate,
    IN  MSD_U32	   	tgtBstSize,
	INOUT TOPAZ_MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
#if defined(__KERNEL__) && defined(LINUX)
    return MSD_NOT_SUPPORTED;
#else
	MSD_U32 IRL_constant[8] = {7812500, 125000000, 62500000 , 31250000,
							15625000, 3906250, 1953750, 976250};
	MSD_U16 bktIncrement=0xFFF;
	MSD_U16 bktRateFactor = 0;
	MSD_U32 ebsLimit = 0xFFFFFF;
	TOPAZ_MSD_PIRL_CUSTOM_RATE_LIMIT Setup;
	MSD_DOUBLE	   	Rate = 0.0;
	MSD_DOUBLE delta = 1.0,deltaMin = 1.0, mode = 1.0;
	MSD_32 i, j;
	MSD_32 lp_start, lp_end;
	MSD_DOUBLE tgRate = (MSD_DOUBLE)(tgtRate * 1000.0);

    MSD_DBG_INFO(("Topaz_gpirlCustomSetup Called.\n"));

	msdMemSet(&Setup,0,sizeof(TOPAZ_MSD_PIRL_CUSTOM_RATE_LIMIT));
    /* customSetup->IRLConstant = 0; */

	if(customSetup->IRLConstant == 0)
	{
		lp_start = 0;
		lp_end = 8;
	}
	else
	{
		switch(customSetup->IRLConstant)
		{
			case 7812500:
				lp_start = 0;
				lp_end = 1;
				break;
				
			case 125000000:
				lp_start = 1;
				lp_end = 2;
				break;
				
			case 62500000:
				lp_start = 2;
				lp_end = 3;
				break;
				
			case 31250000:
				lp_start = 3;
				lp_end = 4;
				break;
				
			case 15625000:
				lp_start = 4;
				lp_end = 5;
				break;
				
			case 3906250:
				lp_start = 5;
				lp_end = 6;
				break;
				
			case 1953750:
				lp_start = 6;
				lp_end = 7;
				break;
				
			case 976250:
				lp_start = 7;
				lp_end = 8;
				break;
				
            default:
                MSD_DBG_ERROR(("bad irl constant: %u.\n", (unsigned int)customSetup->IRLConstant));
				return MSD_BAD_PARAM;

		}
	}
	
	if(tgtBstSize < 1600 || tgtRate == 0)
    {
        MSD_DBG_ERROR(("Bad tgtBstSize Or tgtRate, tgtBstSize: %u, tgtRate: %u.\n", tgtBstSize, tgtRate));
        return MSD_BAD_PARAM;
	}
	if(customSetup->countMode == TOPAZ_MSD_PIRL_COUNT_FRAME)
		mode = 8.0;
	
	for(i=lp_start;i<lp_end;i++)
	{
		for(j=1;j<bktIncrement;j++)
		{	
			if(((MSD_U32)((MSD_DOUBLE)ebsLimit/j + 0.5))<tgtBstSize+1)
				continue;
			if((MSD_U32)(tgRate/IRL_constant[i]*j*mode + 0.5)<0xFFFF)
				bktRateFactor = (MSD_U16)(tgRate / IRL_constant[i] * j*mode + 0.5);
			else 
				continue;   /* IRL_constant too small */
			
			Rate = (MSD_DOUBLE)(IRL_constant[i]/mode*((MSD_DOUBLE)bktRateFactor/j));
			delta = (MSD_DOUBLE)(tgRate - Rate) / tgRate;
			if(delta < 0)
				delta = 0 - delta;
				
			if(deltaMin > delta)
			{
				deltaMin = delta;
				Setup.bktIncrement = j;
				Setup.bktRateFactor = bktRateFactor;
				Setup.ebsLimit = ebsLimit;
				Setup.cbsLimit = ebsLimit - j * tgtBstSize;
				Setup.IRLConstant = IRL_constant[i];

				if(delta==0)
				{
					i = lp_end;
					break;
				}
			}			
		}
	}
    if (deltaMin == 1.0)
    {
        MSD_DBG_ERROR(("Failed (Topaz_gpirlCustomSetup tune failed).\n"));
        return MSD_FAIL;
    }

    MSD_DBG_INFO(("delta((tgRate - rate)/tgRate) = %f", deltaMin));
	customSetup->isValid = MSD_TRUE;
	customSetup->ebsLimit = Setup.ebsLimit;
	customSetup->cbsLimit = Setup.cbsLimit;
	customSetup->bktIncrement = Setup.bktIncrement;
	customSetup->bktRateFactor = Setup.bktRateFactor;
	customSetup->IRLConstant = Setup.IRLConstant;
	customSetup->Delta = deltaMin;

    MSD_DBG_INFO(("Topaz_gpirlCustomSetup Exit.\n"));
    return MSD_OK;	
#endif
}

/*******************************************************************************
* Topaz_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - target logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlReadResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    OUT TOPAZ_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal;            /* Functions return value */
    MSD_U16          data[8];     /* temporary Data storage */
    TOPAZ_MSD_PIRL_OPERATION    op;
    TOPAZ_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;
    MSD_32               i;

    MSD_DBG_INFO(("Topaz_gpirlReadResource Called.\n"));

    op = Topaz_PIRL_READ_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
        return MSD_BAD_PARAM;
	}

	if(irlRes > 4)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}


    opData.irlData = 0;

    retVal = Topaz_pirlOperationPerform(dev, op, 0xf, irlRes, 1, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	msdMemSet((void*)(pirlData), 0, sizeof(TOPAZ_MSD_PIRL_DATA));

	retVal = Topaz_UpIntToConstant((opData.irlData & 0x7), pirlData);
	if(retVal != MSD_OK)
	{
        MSD_DBG_ERROR(("Topaz_UpIntToConstantreturned: %s.\n", msdDisplayStatus(retVal)));
        return MSD_BAD_PARAM;
	}

    for(i=0; i<8; i++)
    {
        opData.irlData = 0;

        retVal = Topaz_pirlOperationPerform(dev, op, port, irlRes, i, &opData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        data[i] = (MSD_U16)opData.irlData;
    }

    /* reg0 data */
    pirlData->bktTypeMask = (data[0]) & 0x7FFF;

    /* reg1 data */
    pirlData->customSetup.bktIncrement = data[1] & 0xFFF;

    /* reg2 data */
    pirlData->customSetup.bktRateFactor = data[2] & 0xFFFF;

    /* reg3,4 data */
    pirlData->customSetup.countMode = (data[3]>>2) & 3;
    pirlData->customSetup.cbsLimit = ((data[3] >> 4) & 0xFFF) | ((data[4] & 0xFFF) << 12);

    /* reg5,6 data */
    pirlData->customSetup.ebsLimit = data[5] | ((data[6] & 0xFF) << 16);
	pirlData->customSetup.isValid = MSD_TRUE;
	
    /* reg6 data */
    pirlData->fcMode = ((data[6] >> 14) & 0x1)?TOPAZ_MSD_PIRL_FC_DEASSERT_CBS_LIMIT:TOPAZ_MSD_PIRL_FC_DEASSERT_EMPTY;
    pirlData->actionMode = ((data[6] >> 13) & 0x1)?Topaz_PIRL_ACTION_ACCEPT:Topaz_PIRL_ACTION_USE_LIMIT_ACTION;
    pirlData->ebsLimitAction = ((data[6] >> 12) & 0x1)?Topaz_EBS_LIMIT_ACTION_FC:Topaz_EBS_LIMIT_ACTION_DROP;
    pirlData->samplingMode = ((data[6] >> 11) & 0x1)?MSD_TRUE:MSD_FALSE;
	
	/* reg7 data */
    pirlData->accountFiltered = ((data[7] >> 14) & 0x1)?MSD_TRUE:MSD_FALSE;
    pirlData->accountQConf = ((data[7] >> 15) & 0x1)?MSD_TRUE:MSD_FALSE;
	pirlData->priORpt = ((data[7]>>12) & 0x1)?MSD_TRUE:MSD_FALSE;
	pirlData->priSelect = (MSD_U8)((data[7]>>8) & 0xf);
	pirlData->tcamFlows = ((data[7]>>3) & 0x1)?MSD_TRUE:MSD_FALSE;

    MSD_DBG_INFO(("Topaz_gpirlReadResource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- target port
*		irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
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
MSD_STATUS Topaz_gpirlWriteResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    IN  TOPAZ_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal;            /* Functions return value */
    MSD_U16          data[8];     /* temporary Data storage */
    TOPAZ_MSD_PIRL_OPERATION    op;
    TOPAZ_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;
    int                i;

    MSD_DBG_INFO(("Topaz_gpirlWriteResource Called.\n"));

    op = Topaz_PIRL_WRITE_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
        return MSD_BAD_PARAM;
	}

	if(irlRes > 4)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}

    /* reg0 data */
    data[0] = (MSD_U16)pirlData->bktTypeMask;        /* Bit[14:0] : Traffic Type   */

    /* reg1 data */
    data[1] = (MSD_U16)pirlData->customSetup.bktIncrement;    /* Bit[11:0] : Bucket Increment */

    /* reg2 data */
    data[2] = (MSD_U16)pirlData->customSetup.bktRateFactor;    /* Bit[15:0] : Bucket Rate Factor */

    /* reg3 data */
    data[3] = (MSD_U16)(((pirlData->customSetup.cbsLimit & 0xFFF) << 4) | 
					(((MSD_U16)pirlData->customSetup.countMode)<<2));    /* Bit[15:4] : CBS Limit[11:0]  Bit[3:2] : count mode*/

    /* reg4 data */
    data[4] = (MSD_U16)(pirlData->customSetup.cbsLimit >> 12);        /* Bit[11:0] : CBS Limit[23:12] */

    /* reg5 data */
    data[5] = (MSD_U16)(pirlData->customSetup.ebsLimit & 0xFFFF);        /* Bit[15:0] : EBS Limit[15:0] */
	
    /* reg6 data */
    data[6] = (MSD_U16)((pirlData->customSetup.ebsLimit >> 16)    |    /* Bit[7:0] : EBS Limit[23:16] */
                    (((MSD_U16)pirlData->fcMode) << 14)    |    /* Bit[14] : PIRL Flow Control Mode */
                    (((MSD_U16)pirlData->actionMode) << 13)    |    /* Bit[13] : PIRL Action Mode */
                    (((MSD_U16)pirlData->ebsLimitAction) << 12)    |    /* Bit[12] : EBS Limit Action */
                    (((MSD_U16)pirlData->samplingMode?1:0) << 11));   /* Bit[12] : sampling Mode */

	/* reg7 data */
    data[7] = (MSD_U16)((pirlData->accountQConf?1:0) << 15)    |    /* Bit[15] : account discarded frames due to queue congestion */
                    (((MSD_U16)pirlData->accountFiltered) << 14)    |    /* Bit[14] : account filtered frames. Account for All  */
                    (((MSD_U16)(pirlData->priORpt?0x1:0)) << 12)    |    /* Bit[13] : typeMask OR priMask */
                    (((MSD_U16)pirlData->priSelect) << 8)    |    /* Bit[11:8] : priority bit mask */
                    (((MSD_U16)(pirlData->tcamFlows?1:0) << 3));   /* Bit[3] : tcam flows account enable bit*/


	retVal = Topaz_ConstantToUpInt(pirlData, &opData.irlData);
	if(retVal != MSD_OK)
	{
        MSD_DBG_ERROR(("bad irl constant: %u.\n", (unsigned int)(pirlData->customSetup.IRLConstant)));
        return MSD_BAD_PARAM;
	}
    retVal = Topaz_pirlOperationPerform(dev, op, 0xf, irlRes, 1, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    for(i=0; i<8; i++)
    {
        opData.irlData = data[i];

        retVal = Topaz_pirlOperationPerform(dev, op, port, irlRes, i, &opData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

    MSD_DBG_INFO(("Topaz_gpirlWriteResource Exit.\n"));
    return MSD_OK;  
}

/*******************************************************************************
* Topaz_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*
* OUTPUTS:
*        data	- register value
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
MSD_STATUS Topaz_gpirlGetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 		 regOffset,
    OUT MSD_U16		 *data
)
{
	MSD_STATUS       retVal;            /* Functions return value */
    TOPAZ_MSD_PIRL_OPERATION    op;
    TOPAZ_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Topaz_gpirlGetResReg Called.\n"));

	if (NULL == data)
	{
		MSD_DBG_ERROR(("Input param MSD_U16 data is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	
    op = Topaz_PIRL_READ_RESOURCE;
	if(irlPort != 0xf)
	{
		port = MSD_LPORT_2_PORT(irlPort);
		if(port == MSD_INVALID_PORT)
		{
			MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
		return MSD_BAD_PARAM;
		}
	}
	else
		port = (MSD_U8)irlPort;
	if(irlRes > 4)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	opData.irlData = 0;

	retVal = Topaz_pirlOperationPerform(dev, op, port, irlRes, regOffset, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	*data = (MSD_U16)opData.irlData;

    MSD_DBG_INFO(("Topaz_gpirlGetResReg Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
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
MSD_STATUS Topaz_gpirlSetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 		 regOffset,
    IN  MSD_U16		 data
)
{
    MSD_STATUS       retVal;            /* Functions return value */
    TOPAZ_MSD_PIRL_OPERATION    op;
    TOPAZ_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Topaz_gpirlSetResReg Called.\n"));

    op = Topaz_PIRL_WRITE_RESOURCE;
	if(irlPort != 0xf)
	{
		port = MSD_LPORT_2_PORT(irlPort);
		if(port == MSD_INVALID_PORT)
		{
			MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
		    return MSD_BAD_PARAM;
		}
	}
	else
		port = (MSD_U8)irlPort;
	if(irlRes > 4)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	opData.irlData = data;

	retVal = Topaz_pirlOperationPerform(dev, op, port, irlRes, regOffset, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_pirlOperationPerform Topaz_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gpirlSetResReg Exit.\n"));
    return MSD_OK;  
}

/*******************************************************************************
* Topaz_statsOperationPerform
*
* DESCRIPTION:
*       This function accesses Ingress Rate Command Register and Data Register.
*
* INPUTS:
*       pirlOp       - The stats operation bits to be written into the stats
*                     operation register.
*       irlPort      - port number
*       irlRes       - resource unit to be accessed
*       irlReg       - IRL register address to be accessed
*       counter      - counter to be read if it's read operation
*
* OUTPUTS:
*       pirlData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Topaz_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
	IN    TOPAZ_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
	INOUT TOPAZ_MSD_PIRL_OP_DATA     *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U16          data;     /* temporary Data storage */

    msdSemTake(dev->devNum, dev->pirlRegsSem,OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
        case Topaz_PIRL_INIT_ALL_RESOURCE:
            data = (1 << 15) | (Topaz_PIRL_INIT_ALL_RESOURCE << 12);
            retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;
        case Topaz_PIRL_INIT_RESOURCE:
            data = (MSD_U16)((1 << 15) | (Topaz_PIRL_INIT_RESOURCE << 12) | (irlPort&0xf)<<8 | 
                    ((irlRes&0x7)<< 5));
            retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

        case Topaz_PIRL_WRITE_RESOURCE:
            data = (MSD_U16)opData->irlData;
            retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_DATA,data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

            data = (MSD_U16)((1 << 15) | (Topaz_PIRL_WRITE_RESOURCE << 12) | (irlPort&0xf)<<8 | 
                    ((irlRes&0x7) << 5) | (irlReg & 0xF));
            retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

		case Topaz_PIRL_READ_RESOURCE:
			data = (MSD_U16)((1 << 15) | (Topaz_PIRL_READ_RESOURCE << 12) | (irlPort & 0xf) << 8 |
                    ((irlRes&0x7) << 5) | (irlReg & 0xF));
            retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,15,1,&data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->pirlRegsSem);
                    return retVal;
                }
            }

            retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_DATA,&data);
            opData->irlData = (MSD_U32)data;
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;

        default:
            
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return MSD_FAIL;
    }

    /* Wait until the pirl in ready. */

    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL2_DEV_ADDR,TOPAZ_QD_REG_IGR_RATE_COMMAND,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    msdSemGive(dev->devNum, dev->pirlRegsSem);
    return retVal;
}

static MSD_STATUS Topaz_UpIntToConstant
(
	IN   MSD_U32 TimeUpInt,
	OUT  TOPAZ_MSD_PIRL_DATA    *pirlData
 )
{
	MSD_U32 IRL_constant[8] = {125000000, 62500000 , 31250000,
							15625000, 7812500, 3906250, 1953750, 976250};

	if(TimeUpInt > 7)
		return MSD_BAD_PARAM;
	else
		pirlData->customSetup.IRLConstant = IRL_constant[TimeUpInt];
	return MSD_OK;
}

static MSD_STATUS Topaz_ConstantToUpInt
(
 
IN    TOPAZ_MSD_PIRL_DATA    *pirlData,
	OUT   MSD_U32 *TimeUpInt
 )
{
	switch(pirlData->customSetup.IRLConstant)
	{
		case 125000000:
			*TimeUpInt = 0;
			break;

		case 62500000:
			*TimeUpInt = 1;
			break;

		case 31250000:
			*TimeUpInt = 2;
			break;

		case 15625000:
			*TimeUpInt = 3;
			break;

		case 7812500:
			*TimeUpInt = 4;
			break;

		case 3906250:
			*TimeUpInt = 5;
			break;

		case 1953750:
			*TimeUpInt = 6;
			break;

		case 976250:
			*TimeUpInt = 7;
			break;
		default:
			return MSD_BAD_PARAM;

	}
	return MSD_OK;
}
