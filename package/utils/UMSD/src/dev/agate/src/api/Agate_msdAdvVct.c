#include <msdCopyright.h>
/*******************************************************************************
* Agate_msdAdvVct.c
*
* DESCRIPTION:
*       API for Marvell Virtual Cable Tester.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <agate/include/api/Agate_msdVct.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

#define AGATE_MSD_LOOKUP_TABLE_ENTRY  128  /* 73 */

#define AGATE_MSD_ADV_VCT_ACCEPTABLE_SHORT_CABLE  11



#define AGATE_MSD_ADV_VCT_CALC(_data)        \
        (((long)(_data)*8333 - 191667)/10000 + (((((long)(_data)*8333 - 191667)%10000) >= 5000)?1:0))

/*#define AGATE_MSD_ADV_VCT_CALC_SHORT(_data)        \
        (((long)(_data)*7143 - 71429)/10000 + (((((long)(_data)*7143 - 71429)%10000) >= 5000)?1:0)) */
#define AGATE_MSD_ADV_VCT_CALC_SHORT(_data) ((long)(_data)*8049/10000)

#define AGATE_IS_VCT_FAILED(_reg)        \
        (((_reg) & 0xFF00) == 0)

/* macro to find out if Amplitude is zero */
#define AGATE_IS_ZERO_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) == 0)

/* macro to retrieve Amplitude */
#define AGATE_GET_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) >> 8)
        
/* macro to find out if Amplitude is positive */
#define AGATE_IS_POSITIVE_AMPLITUDE(_reg)    \
        (((_reg) & 0x8000) == 0x8000)

static  MSD_U8 Agate_tbl_1548L_A2_gain2[AGATE_MSD_LOOKUP_TABLE_ENTRY] =
                    {  60,  63,  66,  69,  71,  74,  76,  78,  81,  83,
                       85,  87,  89,  91,  93,  95,  97,  99, 100, 102,
                      104, 106, 107, 109, 110, 112, 113, 115, 116, 118,
                      119, 120, 122, 123, 124, 126, 127, 128, 129, 130 };


static  MSD_U8 Agate_tbl_1548L_A2_gain1[AGATE_MSD_LOOKUP_TABLE_ENTRY] =
                    {  13,  17,  21,  25,  28,  32,  35,  38,  41,  44,
                       47,  50,  53,  55,  58,  60,  63,  65,  67,  69,
                       71,  73,  75,  77,  79,  81,  83,  85,  86,  88,
                       90,  91,  93,  94,  96,  97,  99, 100, 102, 103,
                      104, 106, 107, 108, 110, 111, 112, 113, 115, 116,
                      117, 118, 119, 120, 121, 122, 124, 125, 126, 127,
					  128, 129, 130 };

static  MSD_U8 Agate_tbl_1548L_A2_gain0[AGATE_MSD_LOOKUP_TABLE_ENTRY] =
                    {  0,  1,  2,  3,  5,  6,  8,  9, 10, 11,
                      13, 15, 17, 18, 19, 20, 21, 23, 25, 26,
                      27, 28, 30, 31, 32, 33, 34, 35, 36, 37,
                      38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                      48, 49, 50, 51, 52, 53, 54, 55, 55, 56,
                      57, 58, 59, 59, 60 };


/*******************************************************************************
* Agate_getDetailedAdvVCTResult
*
* DESCRIPTION:
*       This routine differenciate Open/Short from Impedance mismatch.
*
* INPUTS:
*       amp - amplitude
*       len - distance to fault
*       vctResult - test result 
*                   (Impedance mismatch, either > 115 ohms, or < 85 ohms)
*
* OUTPUTS:
*
* RETURNS:
*       AGATE_MSD_ADV_VCT_STATUS
*
* COMMENTS:
*       This routine assumes test result is not normal nor cross pair short.
*
*******************************************************************************/
static
AGATE_MSD_ADV_VCT_STATUS Agate_getDetailedAdvVCTResult
(
    IN  MSD_U32  amp,
    IN  MSD_U32  len,
    IN  AGATE_MSD_ADV_VCT_STATUS result
)
{
    AGATE_MSD_ADV_VCT_STATUS vctResult;
    MSD_BOOL    update = MSD_FALSE;

	MSD_DBG_INFO(("Agate_getDetailedAdvVCTResult Called.\n"));
    if(len < 10)
    {
        if(amp > 90)  
            update = MSD_TRUE;
    }
    else if(len < 50)
    {
        if(amp > 70) 
            update = MSD_TRUE;
    }
    else if(len < 110)
    {
        if(amp > 50)  
            update = MSD_TRUE;
    }
    else if(len < 140)
    {
        if(amp > 40)  
            update = MSD_TRUE;
    }
    else
    {
        if(amp > 30) 
            update = MSD_TRUE;
    }

    switch (result)
    {
        case AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115:
                if(update)
                    vctResult = AGATE_MSD_ADV_VCT_OPEN;
                else
                    vctResult = result;
                break;
        case AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85:
                if(update)
                    vctResult = AGATE_MSD_ADV_VCT_SHORT;
                else
                    vctResult = result;
                break;
        default:
                vctResult = result;
                break;
    }

	MSD_DBG_INFO(("Agate_getDetailedAdvVCTResult Exit.\n"));
    return vctResult;
}

/*******************************************************************************
* Agate_analizeAdvVCTResult
*
* DESCRIPTION:
*       This routine Agate_analize the Advanced VCT result.
*
* INPUTS:
*       channel - channel number where test was run
*       crossChannelReg - register values after the test is completed
*       mode    - use formula for normal cable case
*
* OUTPUTS:
*       cableStatus - Agate_analized test result.
*
* RETURNS:
*       -1, or distance to fault
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MSD_16 Agate_analizeAdvVCTNoCrosspairResult
(
    IN  int     channel, 
    IN  MSD_U16 *crossChannelReg, 
    IN  MSD_BOOL isShort,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    int len;
    MSD_16 dist2fault;
    AGATE_MSD_ADV_VCT_STATUS vctResult = AGATE_MSD_ADV_VCT_NORMAL;

    MSD_DBG_INFO(("Agate_analizeAdvVCTNoCrosspairResult Called.\n"));

    dist2fault = -1;

    /* check if test is failed */
    if(AGATE_IS_VCT_FAILED(crossChannelReg[channel]))
    {
       cableStatus->cableStatus[channel] = AGATE_MSD_ADV_VCT_FAIL;
	   MSD_DBG_INFO(("Agate_analizeAdvVCTNoCrosspairResult Exit.\n"));
       return dist2fault;
    }

    /* Check if fault detected */
    if(AGATE_IS_ZERO_AMPLITUDE(crossChannelReg[channel]))
    {
       cableStatus->cableStatus[channel] = AGATE_MSD_ADV_VCT_NORMAL;
	   MSD_DBG_INFO(("Agate_analizeAdvVCTNoCrosspairResult Exit.\n"));
       return dist2fault;
    }

    /* find out test result by reading Amplitude */
    if(AGATE_IS_POSITIVE_AMPLITUDE(crossChannelReg[channel]))
    {
       vctResult = AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115;
    }
    else
    {
       vctResult = AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85;
    }

    /* 
     * now, calculate the distance for AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115 and
     * AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85
     */
     if(!isShort)
     {
       len = (int)AGATE_MSD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
     }
     else
     {
       len = (int)AGATE_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
     }
     if (len < 0)
      len = 0;
     cableStatus->u[channel].dist2fault = (MSD_16)len;
     vctResult = Agate_getDetailedAdvVCTResult(
				 AGATE_GET_AMPLITUDE(crossChannelReg[channel]),
                 len,
                 vctResult);
     dist2fault = (MSD_16)len;

     cableStatus->cableStatus[channel] = vctResult;

	 MSD_DBG_INFO(("Agate_analizeAdvVCTNoCrosspairResult Exit.\n"));
     return dist2fault;
}



static
MSD_16 Agate_analizeAdvVCTResult
(
    IN  int     channel, 
    IN  MSD_U16 *crossChannelReg, 
    IN  MSD_BOOL isShort,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    int i, len;
    MSD_16 dist2fault;
    AGATE_MSD_ADV_VCT_STATUS vctResult = AGATE_MSD_ADV_VCT_NORMAL;

    MSD_DBG_INFO(("Agate_analizeAdvVCTResult Called.\n"));

    dist2fault = -1;

    /* check if test is failed */
    for (i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
    {
        if(AGATE_IS_VCT_FAILED(crossChannelReg[i]))
        {
            cableStatus->cableStatus[channel] = AGATE_MSD_ADV_VCT_FAIL;
			MSD_DBG_INFO(("Agate_analizeAdvVCTResult Exit.\n"));
            return dist2fault;
        }
    }

    /* find out test result by reading Amplitude */
    for (i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
    {
        if (i == channel)
        {
            if(!AGATE_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                if(AGATE_IS_POSITIVE_AMPLITUDE(crossChannelReg[i]))
                {
                    vctResult = AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115;
                }
                else
                {
                    vctResult = AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85;
                }
            }
            continue;
        }

        if(AGATE_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            continue;

        vctResult = AGATE_MSD_ADV_VCT_CROSS_PAIR_SHORT;
        break;
    }

    /* if it is cross pair short, check the distance for each channel */
    if(vctResult == AGATE_MSD_ADV_VCT_CROSS_PAIR_SHORT)
    {
        cableStatus->cableStatus[channel] = AGATE_MSD_ADV_VCT_CROSS_PAIR_SHORT;
        for (i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
        {
            if(AGATE_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                cableStatus->u[channel].crossShort.channel[i] = MSD_FALSE;
                cableStatus->u[channel].crossShort.dist2fault[i] = 0;
                continue;
            }
                        
            cableStatus->u[channel].crossShort.channel[i] = MSD_TRUE;
            if(!isShort)
                len = (int)AGATE_MSD_ADV_VCT_CALC(crossChannelReg[i] & 0xFF);
            else
                len = (int)AGATE_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[i] & 0xFF);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].crossShort.dist2fault[i] = (MSD_16)len;
            dist2fault = (MSD_16)len;
        }
		MSD_DBG_INFO(("Agate_analizeAdvVCTResult Exit.\n"));
        return dist2fault;
    }

    /* 
     * now, calculate the distance for AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115 and
     * AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85
     */
    switch (vctResult)
    {
        case AGATE_MSD_ADV_VCT_IMP_GREATER_THAN_115:
        case AGATE_MSD_ADV_VCT_IMP_LESS_THAN_85:
            if(!isShort)
                len = (int)AGATE_MSD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
            else
                len = (int)AGATE_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].dist2fault = (MSD_16)len;
            vctResult = Agate_getDetailedAdvVCTResult(
				AGATE_GET_AMPLITUDE(crossChannelReg[channel]),
                            len,
                            vctResult);
            dist2fault = (MSD_16)len;
            break;
        default:
            break;
    }

    cableStatus->cableStatus[channel] = vctResult;

	MSD_DBG_INFO(("Agate_analizeAdvVCTResult Exit.\n"));
    return dist2fault;
}

static 
MSD_STATUS Agate_runAdvCableTest_1548L_set
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort
)
{
    MSD_STATUS retVal;


    MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_set Called.\n"));
#if 0
    /* Set transmit channel */
   if (crosspair!=AGATE_MSD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
    if((retVal = Agate_msdSetPagedPhyRegField(
        dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,11,3,0x4+channel)) != MSD_OK) 
    {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    /* 
     * start Advanced Virtual Cable Tester
     */
    if((retVal = Agate_msdSetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
    {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
   }
   else
   {
    if (channel==0)
    {
      if((retVal = Agate_msdSetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,11,3,0x000)) != MSD_OK)
      {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = Agate_msdSetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
      {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
   }
#else
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = Agate_msdSetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
      {
			MSD_DBG_ERROR(("Writing to paged phy reg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
      }

#endif
	MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_set Exit.\n"));
    return MSD_OK;
}

static 
MSD_STATUS Agate_runAdvCableTest_1548L_check
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort
)
{
	MSD_STATUS retVal;
    MSD_U16 u16Data, count = 4000;

	MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_check Called.\n"));

    /* 
     * loop until test completion and result is valid
     */
    do {
        if((retVal = Agate_msdReadPagedPhyReg(
                            dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Reading from paged phy reg returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
		if((count--) == 0 )
		{
			MSD_DBG_ERROR(("Running advanced VCT failed, check error.\n"));
            return MSD_FAIL;
		}
    } while (u16Data & 0x8000);

	MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_check Exit.\n"));
    return MSD_OK;
}

static 
MSD_STATUS Agate_runAdvCableTest_1548L_get
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  AGATE_MSD_ADV_VCT_TRANS_CHAN_SEL    crosspair,
    IN  AGATE_MSD_ADV_VCT_MOD     advMod,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus,
    OUT MSD_BOOL         *tooShort
)
{
    MSD_STATUS	retVal;
    MSD_U16		u16Data;
    MSD_U16		crossChannelReg[AGATE_MSD_MDI_PAIR_NUM];
    int			j;
    MSD_16		dist2fault;
    MSD_BOOL     mode;
    MSD_BOOL     localTooShort[AGATE_MSD_MDI_PAIR_NUM];

    Agate_SW_VCT_REGISTER regList[AGATE_MSD_MDI_PAIR_NUM] = {
                    {5,16},{5,17},{5,18},{5,19} };
    mode = MSD_TRUE;

    MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_get Called.\n"));


    if ((retVal = Agate_msdReadPagedPhyReg(
                        dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from paged phy reg AGATE_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
    }

    /*MSD_DBG_INFO(("Page 5 of Reg23 after test : %0#x.\n", u16Data));*/

    /*
     * read the test result for the cross pair against selected MDI Pair
     */
    for (j=0; j<AGATE_MSD_MDI_PAIR_NUM; j++)
    {
      if((retVal = Agate_msdReadPagedPhyReg(
                        dev,hwPort,
                        regList[j].page,
                        regList[j].regOffset,
                        &crossChannelReg[j])) != MSD_OK)
      {
		  MSD_DBG_ERROR(("Reading from paged phy reg returned: %s.\n", msdDisplayStatus(retVal)));
         return retVal;
      }
      /*MSD_DBG_INFO(("@@@@@ reg channel %d is %x \n", j, crossChannelReg[j]));*/
    }

    /*
     * analyze the test result for RX Pair
     */
    if (advMod==AGATE_MSD_ADV_VCT_OFFSET)
    {
      for (j=0; j<AGATE_MSD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].offset = (crossChannelReg[j]>>8)&0xff;
      }
	  MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_get Exit.\n"));
      return MSD_OK;
    }

    if (advMod==AGATE_MSD_ADV_VCT_SAMPLE_POINT)
    {
      for (j=0; j<AGATE_MSD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].sampleAmp = (crossChannelReg[j]>>8)&0xff;
      }
	  MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_get Exit.\n"));
      return MSD_OK;
    }

    for (j=0; j<AGATE_MSD_MDI_PAIR_NUM; j++)
    {
      if (crosspair!=AGATE_MSD_ADV_VCT_TCS_NO_CROSSPAIR)
        dist2fault = Agate_analizeAdvVCTResult(j, crossChannelReg, mode&(*tooShort), cableStatus);
      else
        dist2fault = Agate_analizeAdvVCTNoCrosspairResult(j, crossChannelReg, mode&(*tooShort), cableStatus);

      localTooShort[j]=MSD_FALSE;
      if((mode)&&(*tooShort==MSD_FALSE))
      {
        if ((dist2fault>=0) && (dist2fault<AGATE_MSD_ADV_VCT_ACCEPTABLE_SHORT_CABLE))
        {
           /*MSD_DBG_INFO(("@@@#@@@@ it is too short dist2fault %d\n", dist2fault));
           MSD_DBG_INFO(("Distance to Fault is too Short. So, rerun after changing pulse width\n"));*/
           localTooShort[j]=MSD_TRUE;
/*           *tooShort = MSD_TRUE;
           return MSD_OK;   Should return to stop for loop */
        }
      }
    }
    /* check and decide if lenAgate_gth is too short */
	for (j = 0; j<AGATE_MSD_MDI_PAIR_NUM; j++)
    {
         if (localTooShort[j]==MSD_FALSE) break;
    }

    if (j==AGATE_MSD_MDI_PAIR_NUM)
          *tooShort = MSD_TRUE;
  
	MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_get Exit.\n"));
    return MSD_OK;
}

static 
MSD_STATUS  Agate_runAdvCableTest_1548L
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  AGATE_MSD_ADV_VCT_TRANS_CHAN_SEL   crosspair,
    IN  AGATE_MSD_ADV_VCT_MOD     advMod,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus,
    OUT MSD_BOOL         *tooShort
)
{
  MSD_STATUS retVal;

  MSD_DBG_INFO((" Agate_runAdvCableTest_1548L Called.\n"));


	/* Set transmit channel */
	if((retVal= Agate_runAdvCableTest_1548L_set(dev,hwPort)) != MSD_OK) 
	{
		MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_set returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}


	/* 
	 * loop until test completion and result is valid
	 */
	  if ((retVal= Agate_runAdvCableTest_1548L_check(dev,hwPort)) != MSD_OK)
	  {
			MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_check returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
	  }

	/*
	* read the test result for the cross pair against selected MDI Pair
	*/
	if((retVal= Agate_runAdvCableTest_1548L_get(dev,hwPort, crosspair, 
					advMod, cableStatus,(MSD_BOOL *)tooShort)) != MSD_OK)
	{
		MSD_DBG_INFO(("Agate_runAdvCableTest_1548L_get returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO((" Agate_runAdvCableTest_1548L Eixt.\n"));
	return MSD_OK;
}

static 
MSD_STATUS Agate_getAdvCableStatus_1548L
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  AGATE_MSD_ADV_VCT_MODE mode,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    MSD_STATUS retVal;
    MSD_U16 orgPulse, u16Data;
    MSD_BOOL flag, tooShort;
    AGATE_MSD_ADV_VCT_TRANS_CHAN_SEL crosspair;


    MSD_DBG_INFO(("Agate_getAdvCableStatus_1548L Called.\n"));

    /* flag = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MSD_TRUE:MSD_FALSE; */
	flag = MSD_TRUE;
    crosspair = mode.transChanSel;
	
	orgPulse = 0;

    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case AGATE_MSD_ADV_VCT_FIRST_PEAK:
        case AGATE_MSD_ADV_VCT_MAX_PEAK:
        case AGATE_MSD_ADV_VCT_OFFSET:
        case AGATE_MSD_ADV_VCT_SAMPLE_POINT:
                break;
        default:
                MSD_DBG_ERROR(("Bad parameter mode: %d.\n", mode.mode));
                return MSD_BAD_PARAM;
    }

    if((retVal = Agate_msdGetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading paged phy reg AGATE_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
       return retVal;
    }
    u16Data &= 0xc73f; 
/*    u16Data &= 0x603; */
    u16Data |= ((mode.mode<<6) | (mode.transChanSel<<11));
    if (mode.peakDetHyst) 
	{
      u16Data &= 0xffc0;
	  u16Data |= (mode.peakDetHyst);
	}
    if (mode.sampleAvg)
	{
      u16Data &= 0xf8ff;
      u16Data |= (mode.sampleAvg<<8) ;
	}
    if((retVal = Agate_msdSetPagedPhyRegField(
                    dev,hwPort,5,AGATE_QD_REG_ADV_VCT_CONTROL_5,0,14,u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing paged phy reg AGATE_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
	/* Clear samplePointDist */
    u16Data = 0;
    if(mode.mode==AGATE_MSD_ADV_VCT_SAMPLE_POINT)
    {
      if (mode.samplePointDist) u16Data = (mode.samplePointDist&0x3ff) ;
    }
    if((retVal = Agate_msdSetPagedPhyRegField(
                  dev,hwPort,5,AGATE_QD_REG_ADV_VCT_SMPL_DIST,0,9,u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing paged phy reg AGATE_QD_REG_ADV_VCT_SMPL_DIST returned: %s.\n", msdDisplayStatus(retVal)));
      return retVal;
    }
    if (flag)
    {
        /* save original Pulse Width */
        if((retVal = Agate_msdGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,&orgPulse)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Reading paged phy reg to save original Pulse Width returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        /* set the Pulse Width with default value */
        if (orgPulse != 0)
        {
            if((retVal = Agate_msdSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Writing paged phy reg to set the Pulse Width default value returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }
        }
        tooShort=MSD_FALSE;
    }
    if((retVal= Agate_runAdvCableTest_1548L(dev,hwPort,crosspair, mode.mode,
                cableStatus,&tooShort)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_runAdvCableTest_1548L returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (flag)
    {
        if(tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = Agate_msdSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Writing paged phy reg to set the Pulse Width with minimum width returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }

            /* run the Adv VCT again */
            MSD_DBG_INFO(("Cable is too short, try again!\n"));
            if((retVal= Agate_runAdvCableTest_1548L(dev,hwPort,crosspair, mode.mode,
                        cableStatus,&tooShort)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Agate_runAdvCableTest_1548L again returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }

        }

        /* set the Pulse Width back to the original value */
        if((retVal = Agate_msdSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,orgPulse)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Writing paged phy reg to set back original Pulse Width returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

    }

	MSD_DBG_INFO(("Agate_getAdvCableStatus_1548L Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* gvctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       port - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_gvctGetAdvCableDiag
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN  AGATE_MSD_ADV_VCT_MODE mode,
    OUT AGATE_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    MSD_STATUS status;
    MSD_U8 hwPort;
    MSD_U16 u16Data, org0;
    MSD_BOOL            autoNeg;
    MSD_U16            pageReg;

    MSD_DBG_INFO(("Agate_gvctGetCableDiag Called.\n"));
    hwPort = MSD_LPORT_2_PHY(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %d.\n", hwPort));
        return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);

    if((status= Agate_msdGetSMIPhyRegField(dev,hwPort,22,0,8,&pageReg)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Read Phy Page Register returned: %s.\n", msdDisplayStatus(status)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return status;
    }
    /*
     * Check the link
     */
    if((status= Agate_msdReadPagedPhyReg(dev,hwPort,0,17,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Check phy link status returned: %s.\n", msdDisplayStatus(status)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return status;
    }

    autoNeg = MSD_FALSE;
    org0 = 0;
    if (!(u16Data & 0x400))
    {
        /* link is down, so disable auto-neg if enabled */
        if((status= Agate_msdReadPagedPhyReg(dev,hwPort,0,0,&u16Data)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Read the Phy register returned: %s.\n", msdDisplayStatus(status)));
			msdSemGive(dev->devNum, dev->phyRegsSem);
            return status;
        }
        
        org0 = u16Data;

        if (u16Data & 0x1000)
        {
            u16Data = 0x140;

            /* link is down, so disable auto-neg if enabled */
            if((status= Agate_msdWritePagedPhyReg(dev,hwPort,0,0,u16Data)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Disable the auto-neg returned: %s.\n", msdDisplayStatus(status)));
				msdSemGive(dev->devNum, dev->phyRegsSem);
                return status;
            }

			if ((status = Agate_msdPhyReset(dev, hwPort, 0, 0xFF)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Reset the Phy returned: %s.\n", msdDisplayStatus(status)));
				msdSemGive(dev->devNum, dev->phyRegsSem);
                return status;
            }
            autoNeg = MSD_TRUE;        
        }
    }
	
    /* status = Agate_getAdvCableStatus_1116(dev,hwPort,mode,cableStatus); */
	status = Agate_getAdvCableStatus_1548L(dev,hwPort,mode,cableStatus);
	if(status != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_getAdvCableStatus_1548L returned: %s.\n", msdDisplayStatus(status)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return status;
	}
	
    if (autoNeg)
    {
        if((status= Agate_msdPhyReset(dev,hwPort, 0, org0)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Reset the Phy returned: %s.\n", msdDisplayStatus(status)));
			msdSemGive(dev->devNum, dev->phyRegsSem);
            return status;
        }
    }
	
	if ((status = Agate_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Set back Phy Page Register returned: %s.\n", msdDisplayStatus(status)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return status;
    }

	msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Agate_gvctGetCableDiag Exit.\n"));
    return status;    
}


/*******************************************************************************
* Agate_dspLookup
*
* DESCRIPTION:
*       This routine returns cable lenAgate_gth (meters) by reading DSP Lookup table.
*
* INPUTS:
*       regValue - register 21
*
* OUTPUTS:
*       cableLen - cable lenAgate_gth (unit of meters).
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MSD_STATUS Agate_dspLookup
(
    IN  MSD_U16 regValue, 
	IN  MSD_U16 gainIndex,
    OUT MSD_32  *cableLen
)
{
    MSD_U16 startEntry,tableEntry;
    MSD_U8* tbl;

	MSD_DBG_INFO(("Agate_dspLookup Called.\n"));

	tbl = NULL;
	startEntry = 0;
	tableEntry = 0;

	if(gainIndex == 3)
	{
		*cableLen = 130;
		return MSD_OK;
	}
	else if(gainIndex == 2)
	{	
		startEntry = 27;
		tableEntry = 40;
		tbl = Agate_tbl_1548L_A2_gain2;
	}
	else if(gainIndex == 1)
	{	
		startEntry = 18;
		tableEntry = 63;
		tbl = Agate_tbl_1548L_A2_gain1;
	}
	else if(gainIndex == 0)
	{	
		startEntry = 42;
		tableEntry = 55;
		tbl = Agate_tbl_1548L_A2_gain0;
	}

    if (tbl == NULL)
    {
        *cableLen = -1;
        return MSD_OK;
    }

    if (regValue < startEntry)
    {
		*cableLen = tbl[0];
        return MSD_OK;
    }

    if (regValue >= (tableEntry+startEntry-1))
    {
        regValue = tableEntry-1;
    }
    else
    {
		/*
        regValue -= startEntry;
		*/
		regValue = (MSD_U16)(regValue - startEntry);
    }

    *cableLen = (MSD_32)tbl[regValue];

	MSD_DBG_INFO(("Agate_dspLookup Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_getDSPDistance_1548L
*
* DESCRIPTION:
*       This routine returns cable lenAgate_gth (meters) from DSP method.
*       This routine is for the 88E1548L like devices.
*
* INPUTS:
*       mdi - pair of each MDI (0..3).
*
* OUTPUTS:
*       cableLen - cable lenAgate_gth (unit of meters).
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MSD_STATUS Agate_getDSPDistance_1548L
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  hwPort,
    IN  MSD_U32 mdi,
    OUT MSD_32 *cableLen
)
{
    MSD_U16     data, retryCount, gainIndex;
    MSD_STATUS  retVal;

    MSD_DBG_INFO(("Agate_getDSPDistance_1548L Called.\n"));

    /* Set the required bits for Cable lenAgate_gth register */
    if((retVal = Agate_msdWritePagedPhyReg(dev,hwPort,0xff,16,(MSD_U16)(0x1118+(0xff&mdi)))) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing to required bits for Cable lenAgate_gth reg returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retryCount = 2000;

    do
    {
        if(retryCount == 0)
        {
			MSD_DBG_ERROR(("Ready bit of Cable lenAgate_gth resiter is not set.\n"));
            return MSD_FAIL;
        }

        /* Check the ready bit of Cable lenAgate_gth register */
        if((retVal = Agate_msdGetPagedPhyRegField(dev,hwPort,0xff,16,15,1,&data)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Check the ready bit of Cable lenAgate_gth register returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        retryCount--;

    } while(!data);

    /* read lenAgate_gth of MDI pair */
    if((retVal = Agate_msdReadPagedPhyReg(dev,hwPort,0xff,18,&data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading lenAgate_gth of MDI pair returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	/* read gain Index of MDI pair */
    if((retVal = Agate_msdGetPagedPhyRegField(dev,hwPort,0xfc, 23, 0, 2, &gainIndex)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading gain lenAgate_gth of MDI pair returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
	
	if ((retVal = Agate_dspLookup(data, gainIndex, cableLen)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_dspLookup returned %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_getDSPDistance_1548L Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Agate_getExStatus
*
* DESCRIPTION:
*       This routine retrieves Pair Skew, Pair Swap, and Pair Polarity
*        for 1000M phy with multiple page mode
*
* INPUTS:
*       dev - device context.
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - extended cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Agate_getExStatus
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8            hwPort,
    OUT AGATE_MSD_ADV_EXTENDED_STATUS *extendedStatus
)
{
    MSD_STATUS retVal;
    MSD_U16 u16Data, i;

	MSD_DBG_INFO(("Agate_getExStatus Called.\n"));

    extendedStatus->isValid = MSD_FALSE;
	
	for (i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
	{
		if((retVal = Agate_getDSPDistance_1548L(dev,hwPort,i,(MSD_32 *)&extendedStatus->cableLen[i])) != MSD_OK)
		{
			MSD_DBG_ERROR(("Agate_getDSPDistance_1548L returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}

    /*
     * Agate_get data from 21_5 register for pair swap
     */
    if((retVal = Agate_msdReadPagedPhyReg(
                    dev,hwPort,5,AGATE_QD_REG_PAIR_SWAP_STATUS,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from 21_5 register for pair swap returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* if bit 6 is not set, it's not valid. */
    if (!(u16Data & 0x40))
    {
        MSD_DBG_INFO(("Agate_getExStatus Exit, Valid Bit is not set (%0#x).\n", u16Data));
        return MSD_OK;
    }

    extendedStatus->isValid = MSD_TRUE;
    
    /* Agate_get Pair Polarity */
    for(i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
    {
        switch((u16Data >> i) & 0x1)
        {
            case 0:
                extendedStatus->pairPolarity[i] = AGATE_MSD_POSITIVE;
                break;
            default:
                extendedStatus->pairPolarity[i] = AGATE_MSD_NEGATIVE;
            break;
        }
    }

    /* Agate_get Pair Swap for Channel A and B */
    if (u16Data & 0x10)
    {
        extendedStatus->pairSwap[0] = AGATE_MSD_CHANNEL_A;
        extendedStatus->pairSwap[1] = AGATE_MSD_CHANNEL_B;
    }
    else
    {
        extendedStatus->pairSwap[0] = AGATE_MSD_CHANNEL_B;
        extendedStatus->pairSwap[1] = AGATE_MSD_CHANNEL_A;
    }

    /* Agate_get Pair Swap for Channel C and D */
    if (u16Data & 0x20)
    {
        extendedStatus->pairSwap[2] = AGATE_MSD_CHANNEL_C;
        extendedStatus->pairSwap[3] = AGATE_MSD_CHANNEL_D;
    }
    else
    {
        extendedStatus->pairSwap[2] = AGATE_MSD_CHANNEL_D;
        extendedStatus->pairSwap[3] = AGATE_MSD_CHANNEL_C;
    }

    /*
     * Agate_get data from 20_5 register for pair skew
     */
    if((retVal = Agate_msdReadPagedPhyReg(
                    dev,hwPort,5,AGATE_QD_REG_PAIR_SKEW_STATUS,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from 20_5 register for pair skew returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Agate_get Pair Skew */
    for(i=0; i<AGATE_MSD_MDI_PAIR_NUM; i++)
    {
        extendedStatus->pairSkew[i] = ((u16Data >> i*4) & 0xF) * 8;
    }

	MSD_DBG_INFO(("Agate_getExStatus Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* gvctGetAdvExtendedStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable lenAgate_gth is also provided.
*
* INPUTS:
*       dev  - pointer to Agate_gt driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_gvctGetAdvExtendedStatus
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT AGATE_MSD_ADV_EXTENDED_STATUS *extendedStatus
)
{
    MSD_STATUS retVal;
    MSD_U8 hwPort;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Agate_gvctGetAdvExtendedStatus Called.\n"));
    hwPort = MSD_LPORT_2_PHY(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %d.\n", hwPort));
        return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);

	if ((retVal = Agate_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Read Phy Page Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if((retVal = Agate_getExStatus(dev,hwPort,extendedStatus)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Getting Extanded Cable Status returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if ((retVal = Agate_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Set back Phy Page Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Agate_gvctGetAdvExtendedStatus Exit.\n"));
    return retVal;
}

