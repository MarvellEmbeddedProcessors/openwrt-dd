#include <msdCopyright.h>
/********************************************************************************
* sample_msdPIRL.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Initialize all IRL Resources for all ports.
*    2. Sample of ingress layer 2 bit rate limited for ARP frames
*    3. Sample of ingress layer 2 bit rate limited for ARP frames using advanced APIs
*	 4. Sample of ingress layer 2 bit rate limited for Frame Priority 2 frames 
*	 5. Sample of ingress layer 2 bit rate limited for Frame Priority 2 frames using advanced APIs
*	 6. Sample of ingress layer 2 bit rate limited for Queue Priority 2 frames
*	 7. Sample of ingress layer 2 bit rate limited for Queue Priority 2 frames using advanced APIs
*	 8. Sample of ingress layer 2 bit rate limited for unknown unicast frames with queue priority 2
*	 9. Sample of ingress layer 2 bit rate limited for unknown unicast frames with queue priority 2 using advanced APIs
*    10. Sample of ingress rate limited for sampling mode
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
* Sample #1: ingress layer 2 bit rate limited  for ARP frames
********************************************************************************/
MSD_STATUS sample_msdPIRL_ARP(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
	MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    MSD_PIRL_COUNT_MODE countMode;

	MSD_PIRL_DATA pirlData;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	 *    Initialize all IRL Resources for all ports.
	 */
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	/*
	 *    Calculate CBS/EBS/BI/BRF according to user specific target rate, target burst size.
	 *    Get customSetup for next step (to initialize pirl data)
	 */
	tgtRate = 1000; /*rate limit target is 1Mbps*/
	tgtBstSize = 3000; /*target burst size should be larger than maximum frame size, here is 3000 byte*/

	msdMemSet(&customSetup,0,sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT)); /* Set all bit to 0 by default */
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/
    if ((status = msdIRLCustomSetup(devNum, tgtRate, tgtBstSize, countMode, &customSetup)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLCustomSetup returned fail.\n"));
		return status;
	}

	/* Initialize pirl data */
	msdMemSet(&pirlData,0,sizeof(MSD_PIRL_DATA)); /* Set all bit to 0 by default */
	pirlData.bktTypeMask = 1<<7;                /* rate limit for ARP frame */
	pirlData.priAndPt = MSD_FALSE;				/* rate limit for PktType or priority*/
    pirlData.actionMode = MSD_PIRL_ACTION_DROP;     /* discard packets if over rate */
	pirlData.accountQConf = MSD_TRUE;

	pirlData.customSetup.isValid = customSetup.isValid;
	pirlData.customSetup.ebsLimit = customSetup.ebsLimit;
	pirlData.customSetup.cbsLimit = customSetup.cbsLimit;
	pirlData.customSetup.bktIncrement = customSetup.bktIncrement;
	pirlData.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
	pirlData.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;
	pirlData.customSetup.IRLConstant = customSetup.IRLConstant;
	pirlData.customSetup.countMode = customSetup.countMode;
	
	/* Writes resource bucket parameters to the given resource of the port. */
	irlPort = 1;
	irlRes  = 0;
	if ((status = msdIRLResourceWrite(devNum, irlPort, irlRes, &pirlData)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_ARP sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2: ingress layer 2 bit rate limited  for ARP frames using advanced APIs
********************************************************************************/
MSD_STATUS sample_msdPIRL_ARP_Adv(MSD_U8 devNum)
{
    MSD_STATUS status;

    MSD_U32 tgtRate;
    MSD_U32 tgtBstSize;
    MSD_PIRL_COUNT_MODE countMode;
    MSD_PIRL_PKTTYPE_OR_PRI priOrType;

    MSD_LPORT irlPort;
    MSD_U32 irlRes; /* 0~4, each port support 5 resource */

    MSD_QD_DEV *dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSG_PRINT(("devNum %d is NULL. \n", devNum));
        return MSD_FAIL;
    }

    /*
    *    Initialize all IRL Resources for all ports.
    */
    if ((status = msdIRLReset(devNum)) != MSD_OK)
    {
        MSG_PRINT(("msdIRLReset returned fail.\n"));
        return status;
    }

    /* Initialize priOrType structure */
    msdMemSet(&priOrType, 0, sizeof(MSD_PIRL_PKTTYPE_OR_PRI));        /* Set all bit to 0 by default */

    priOrType.bktTypeMask = 1 << 7;                /* rate limit for ARP frame */
    priOrType.priAndPt = MSD_FALSE;				/* rate limit for PktType or priority*/

    tgtRate = 1000; /*rate limit target is 1Mbps*/
    tgtBstSize = 3000; /*target burst size should be larger than maximum frame size, here is 3000 byte*/
    countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/

	irlPort = 1;
	irlRes = 0;

    /* configure ingress rate limit based on the above parameters */
    if ((status = msdIRLAdvConfig(devNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &priOrType)) != MSD_OK)
    {
        MSG_PRINT(("msdIRLAdvConfig return fail.\n"));
        return status;
    }

    MSG_PRINT(("sample_msdPIRL_ARP_Adv sample code success.\n"));
    return MSD_OK;
}

/********************************************************************************
* Sample #3: ingress layer 2 bit rate limited for Frame Priority 2 frames 
********************************************************************************/
MSD_STATUS sample_msdPIRL_FPRI(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
    MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    MSD_PIRL_COUNT_MODE countMode;

	MSD_PIRL_DATA pirlData;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	if (MSD_PERIDOT != dev->devName)
	{
		MSG_PRINT(("device 0x%x not support rate limit for frame priority\n", dev->deviceId));
		return MSD_NOT_SUPPORTED;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	/*
	*    Calculate CBS/EBS/BI/BRF according to user specific target rate, target burst size.
	*    Get customSetup for next step (to initialize pirl data)
	*/
	tgtRate = 1000; /*rate limit target is 1Mbps*/
	tgtBstSize = 3000; /*target burst size should be larger than maximum frame size, here is 3000 byte*/

	msdMemSet(&customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT)); /* Set all bit to 0 by default */
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/
	if ((status = msdIRLCustomSetup(devNum, tgtRate, tgtBstSize, countMode, &customSetup)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLCustomSetup returned fail.\n"));
		return status;
	}

	/* Initialize pirl data */
	msdMemSet(&pirlData, 0, sizeof(MSD_PIRL_DATA)); /* Set all bit to 0 by default */
	pirlData.useFPri = MSD_TRUE;	                  /* Rate Limit for Frame Priority 2 */
	pirlData.priSelect = 1 << 2;                  
	pirlData.bktTypeMask = 0;                     /* don't care packet type */
	pirlData.priAndPt = MSD_FALSE;				  /* rate limit for PktType or priority*/
    pirlData.actionMode = MSD_PIRL_ACTION_DROP;       /* discard packets if over rate */
	pirlData.accountQConf = MSD_TRUE;

	pirlData.customSetup.isValid = customSetup.isValid;
	pirlData.customSetup.ebsLimit = customSetup.ebsLimit;
	pirlData.customSetup.cbsLimit = customSetup.cbsLimit;
	pirlData.customSetup.bktIncrement = customSetup.bktIncrement;
	pirlData.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
	pirlData.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;
	pirlData.customSetup.IRLConstant = customSetup.IRLConstant;
	pirlData.customSetup.countMode = customSetup.countMode;

	/* Writes resource bucket parameters to the given resource of the port. */
	irlPort = 1;
	irlRes = 0;
	if ((status = msdIRLResourceWrite(devNum, irlPort, irlRes, &pirlData)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_FPRI sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #4: ingress layer 2 bit rate limited  for Frame Priority 2 frames
* using advanced APIs
********************************************************************************/
MSD_STATUS sample_msdPIRL_FPRI_Adv(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
	MSD_PIRL_COUNT_MODE countMode;
	MSD_PIRL_PKTTYPE_OR_PRI priOrType;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	if (MSD_PERIDOT != dev->devName)
	{
		MSG_PRINT(("device 0x%x not support rate limit for frame priority\n", dev->deviceId));
		return MSD_NOT_SUPPORTED;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	priOrType.bktTypeMask = 0;				/* don't care packet type */
	priOrType.priAndPt = MSD_FALSE;			/* rate limit for PktType or priority*/
	priOrType.useFPri = MSD_TRUE;			
	priOrType.priSelect = 1 << 2;			/* Rate Limit for Frame Priority 2 */

	tgtRate = 1000;			/*rate limit target is 1Mbps*/
	tgtBstSize = 3000;			/*target burst size should be larger than maximum frame size, here is 3000 byte*/
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/

	irlPort = 1;
	irlRes = 0;

	/* configure ingress rate limit based on the above parameters */
	if ((status = msdIRLAdvConfig(devNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &priOrType)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLAdvConfig return fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_ARP_Adv sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #5: ingress layer 2 bit rate limited for Queue Priority 2 frames
********************************************************************************/
MSD_STATUS sample_msdPIRL_QPRI(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
    MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    MSD_PIRL_COUNT_MODE countMode;

	MSD_PIRL_DATA pirlData;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	/*
	*    Calculate CBS/EBS/BI/BRF according to user specific target rate, target burst size.
	*    Get customSetup for next step (to initialize pirl data)
	*/
	tgtRate = 1000; /*rate limit target is 1Mbps*/
	tgtBstSize = 3000; /*target burst size should be larger than maximum frame size, here is 3000 byte*/

	msdMemSet(&customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT)); /* Set all bit to 0 by default */
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;             /* Rate limit for Layer 2 bit rate*/
	if ((status = msdIRLCustomSetup(devNum, tgtRate, tgtBstSize, countMode, &customSetup)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLCustomSetup returned fail.\n"));
		return status;
	}

	/* Initialize pirl data */
	msdMemSet(&pirlData, 0, sizeof(MSD_PIRL_DATA)); /* Set all bit to 0 by default */
	pirlData.useFPri = MSD_FALSE;	              /* Rate Limit for Queue Priority 2 */
	pirlData.priSelect = 1 << 2;
	pirlData.bktTypeMask = 0;                     /* don't care packet type */
	pirlData.priAndPt = MSD_FALSE;				  /* rate limit for PktType or priority*/
    pirlData.actionMode = MSD_PIRL_ACTION_DROP;       /* discard packets if over rate */
	pirlData.accountQConf = MSD_TRUE;

	pirlData.customSetup.isValid = customSetup.isValid;
	pirlData.customSetup.ebsLimit = customSetup.ebsLimit;
	pirlData.customSetup.cbsLimit = customSetup.cbsLimit;
	pirlData.customSetup.bktIncrement = customSetup.bktIncrement;
	pirlData.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
	pirlData.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;
	pirlData.customSetup.IRLConstant = customSetup.IRLConstant;
	pirlData.customSetup.countMode = customSetup.countMode;

	/* Writes resource bucket parameters to the given resource of the port. */
	irlPort = 1;
	irlRes = 0;
	if ((status = msdIRLResourceWrite(devNum, irlPort, irlRes, &pirlData)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_QPRI sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #6: ingress layer 2 bit rate limited  for Queue Priority 2 frames
* using advanced APIs
********************************************************************************/
MSD_STATUS sample_msdPIRL_QPRI_Adv(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
	MSD_PIRL_COUNT_MODE countMode;
	MSD_PIRL_PKTTYPE_OR_PRI priOrType;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	priOrType.bktTypeMask = 0;				/* don't care packet type */
	priOrType.priAndPt = MSD_FALSE;			/* rate limit for PktType or priority*/
	priOrType.useFPri = MSD_FALSE;			/* Rate Limit for Queue Priority 2 */
	priOrType.priSelect = 1 << 2;			

	tgtRate = 1000;			/*rate limit target is 1Mbps*/
	tgtBstSize = 3000;			/*target burst size should be larger than maximum frame size, here is 3000 byte*/
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/

	irlPort = 1;
	irlRes = 0;

	/* configure ingress rate limit based on the above parameters */
	if ((status = msdIRLAdvConfig(devNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &priOrType)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLAdvConfig return fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_ARP_Adv sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #7: ingress layer 2 bit rate limited for unknown unicast frames with 
* Queue priority 2
********************************************************************************/
MSD_STATUS sample_msdPIRL_UnknownUC_QPRI(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
    MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    MSD_PIRL_COUNT_MODE countMode;

	MSD_PIRL_DATA pirlData;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	/*
	*    Calculate CBS/EBS/BI/BRF according to user specific target rate, target burst size.
	*    Get customSetup for next step (to initialize pirl data)
	*/
	tgtRate = 1000; /*rate limit target is 1Mbps*/
	tgtBstSize = 3000; /*target burst size should be larger than maximum frame size, here is 3000 byte*/

	msdMemSet(&customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT)); /* Set all bit to 0 by default */
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/
	if ((status = msdIRLCustomSetup(devNum, tgtRate, tgtBstSize, countMode, &customSetup)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLCustomSetup returned fail.\n"));
		return status;
	}

	/* Initialize pirl data */
	msdMemSet(&pirlData, 0, sizeof(MSD_PIRL_DATA)); /* Set all bit to 0 by default */
	pirlData.priAndPt = MSD_TRUE ;				  /* rate limit from frame that matches both PktType and specified priority*/
	pirlData.bktTypeMask = 1 << 0;                /* rate limit for unknown unicast frame */
	pirlData.useFPri = MSD_FALSE;                   /* rate limit for queue priority 2*/
	pirlData.priSelect = 1 << 2;
    pirlData.actionMode = MSD_PIRL_ACTION_DROP;       /* discard packets if over rate */
	pirlData.accountQConf = MSD_TRUE;

	pirlData.customSetup.isValid = customSetup.isValid;
	pirlData.customSetup.ebsLimit = customSetup.ebsLimit;
	pirlData.customSetup.cbsLimit = customSetup.cbsLimit;
	pirlData.customSetup.bktIncrement = customSetup.bktIncrement;
	pirlData.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
	pirlData.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;
	pirlData.customSetup.IRLConstant = customSetup.IRLConstant;
	pirlData.customSetup.countMode = customSetup.countMode;

	/* Writes resource bucket parameters to the given resource of the port. */
	irlPort = 1;
	irlRes = 0;
	if ((status = msdIRLResourceWrite(devNum, irlPort, irlRes, &pirlData)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_UnknownUC_FPRI sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #8: ingress layer 2 bit rate limited for unknown unicast frames with 
* Queue priority 2 using advanced APIs
********************************************************************************/
MSD_STATUS sample_msdPIRL_UnknownUC_QPRI_Adv(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_U32 tgtRate;
	MSD_U32 tgtBstSize;
	MSD_PIRL_COUNT_MODE countMode;
	MSD_PIRL_PKTTYPE_OR_PRI priOrType;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	priOrType.bktTypeMask = 1 << 0;			/* rate limit for unknown unicast frame */
	priOrType.priAndPt = MSD_TRUE;			/* rate limit from frame that matches both PktType and specified priority*/
	priOrType.useFPri = MSD_FALSE;
	priOrType.priSelect = 1 << 2;			/* Rate Limit for queue Priority 2 */

	tgtRate = 1000;			/*rate limit target is 1Mbps*/
	tgtBstSize = 3000;			/*target burst size should be larger than maximum frame size, here is 3000 byte*/
	countMode = MSD_PIRL_COUNT_ALL_LAYER2;           /* Rate limit for Layer 2 bit rate*/

	irlPort = 1;
	irlRes = 0;

	/* configure ingress rate limit based on the above parameters */
	if ((status = msdIRLAdvConfig(devNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &priOrType)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLAdvConfig return fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPIRL_ARP_Adv sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #9: Sample of ingress rate limited for sampling mode, only for mirror frames
********************************************************************************/
MSD_STATUS sample_msdPIRL_SAMPLING(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_PIRL_DATA pirlData;

	MSD_LPORT irlPort;
	MSD_U32 irlRes; /* 0~4, each port support 5 resource */

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Initialize all IRL Resources for all ports.
	*/
	if ((status = msdIRLReset(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLReset returned fail.\n"));
		return status;
	}

	msdMemSet(&pirlData, 0, sizeof(MSD_PIRL_DATA)); /* Set all bit to 0 by default */
	pirlData.bktTypeMask = 1 << 13;       /* sampling for policy mirror frame */
	pirlData.priAndPt = MSD_FALSE;         /* sampling for pktType or priority */
	pirlData.samplingMode = MSD_TRUE;      /* Enable sampling mode */

	pirlData.customSetup.countMode = MSD_PIRL_COUNT_FRAME;
	pirlData.customSetup.bktIncrement = 0x1;

	/* Writes resource bucket parameters to the given resource of the port. */
	irlPort = 1;
	irlRes = 0;
	if (dev->devName == MSD_PERIDOT)
	{
		/*
		*    SampleRate = 1 / (CBS_Limit + 1)
		*/
		pirlData.customSetup.ebsLimit = 0;
		pirlData.customSetup.cbsLimit = 0x63;
	}

	if (dev->devName == MSD_TOPAZ || dev->devName == MSD_AGATE || dev->devName == MSD_PEARL)
	{
		MSD_PIRL_DATA pirlDataRd;

		if ((status = msdIRLResourceRead(devNum, irlPort, irlRes, &pirlDataRd)) != MSD_OK)
		{
			MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
			return status;
		}

		/*
		*    SampleRate = CBS_Limit / (CBS_Limit + EBS_Limit)
		*/
		pirlData.customSetup.ebsLimit = 0x63;
		pirlData.customSetup.cbsLimit = 0x1;
		pirlData.customSetup.IRLConstant = pirlDataRd.customSetup.IRLConstant;		/* Topaz, Agate, Pearl need it */
	}

	if ((status = msdIRLResourceWrite(devNum, irlPort, irlRes, &pirlData)) != MSD_OK)
	{
		MSG_PRINT(("msdIRLResourceWrite returned fail.\n"));
		return status;
	}


	MSG_PRINT(("sample_msdPIRL_SAMPLING sample code success.\n"));
	return MSD_OK;
}