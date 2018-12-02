#include <msdCopyright.h>
/*******************************************************************************
* Pearl_msdVct.c
*
* DESCRIPTION:
*       API for Marvell Virtual Cable Tester.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <pearl/include/api/Pearl_msdVct.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>

#define PEARL_MSD_LOOKUP_TABLE_ENTRY  128  /* 73 */

#define PEARL_MSD_ADV_VCT_ACCEPTABLE_SHORT_CABLE  11



#define PEARL_MSD_ADV_VCT_CALC(_data)        \
        (((long)(_data)*8333 - 191667)/10000 + (((((long)(_data)*8333 - 191667)%10000) >= 5000)?1:0))

/*#define PEARL_MSD_ADV_VCT_CALC_SHORT(_data)        \
        (((long)(_data)*7143 - 71429)/10000 + (((((long)(_data)*7143 - 71429)%10000) >= 5000)?1:0)) */
#define PEARL_MSD_ADV_VCT_CALC_SHORT(_data) ((long)(_data)*8049/10000)

/* macro to check VCT Failure */
/*#define Pearl_IS_VCT_FAILED(_reg)        \
        (((_reg) & 0xFF) == 0xFF)*/

#define Pearl_IS_VCT_FAILED(_reg)        \
        (((_reg) & 0xFF00) == 0)

/* macro to find out if Amplitude is zero */
#define Pearl_IS_ZERO_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) == 0)

/* macro to retrieve Amplitude */
#define PEARL_GET_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) >> 8)
        
/* macro to find out if Amplitude is positive */
#define Pearl_IS_POSITIVE_AMPLITUDE(_reg)    \
        (((_reg) & 0x8000) == 0x8000)

static  MSD_U8 Pearl_tbl_1680_A2_gain2[PEARL_MSD_LOOKUP_TABLE_ENTRY] = 
                    {  60,  62,  65,  67,  70,  72,  75,  77,  79,  81, 
                       84,  86,  88,  90,  91,  93,  95,  97,  99, 100, 
                      102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 
                      117, 118, 120, 121, 122, 124, 125, 126, 127, 129,
					  130  };

static  MSD_U8 Pearl_tbl_1680_A2_gain1[PEARL_MSD_LOOKUP_TABLE_ENTRY] = 
                    {  14,  18,  22,  25,  29,  32,  35,  39,  42,  45, 
                       47,  50,  53,  55,  58,  60,  62,  65,  67,  69, 
                       71,  73,  75,  77,  79,  81,  83,  85,  86,  88, 
                       90,  91,  93,  95,  96,  98,  99, 101, 102, 103, 
                      105, 106, 108, 109, 110, 111, 113, 114, 115, 116, 
                      118, 119, 120, 121, 122, 123, 124, 126, 127, 128, 
					  129, 130 };

static  MSD_U8 Pearl_tbl_1680_A2_gain0[PEARL_MSD_LOOKUP_TABLE_ENTRY] = 
                    {  0,  1,  2,  4,  5,  7,  8, 10, 11, 12, 
                      14, 15, 16, 18, 19, 20, 21, 23, 24, 25,  
                      26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 
                      37, 38, 39, 40, 41, 41, 42, 43, 44, 45,  
                      46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 
                      54, 55, 56, 57, 57, 58, 59, 60, 60, 61,
					  62, 63  };

/*******************************************************************************
* Pearl_getDetailedAdvVCTResult
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
*       PEARL_MSD_ADV_VCT_STATUS
*
* COMMENTS:
*       This routine assumes test result is not normal nor cross pair short.
*
*******************************************************************************/
static
PEARL_MSD_ADV_VCT_STATUS Pearl_getDetailedAdvVCTResult
(
    IN  MSD_U32  amp,
    IN  MSD_U32  len,
    IN  PEARL_MSD_ADV_VCT_STATUS result
)
{
    PEARL_MSD_ADV_VCT_STATUS vctResult;
    MSD_BOOL    update = MSD_FALSE;

    MSD_DBG_INFO(("Pearl_getDetailedAdvVCTResult Called.\n"));

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
        case PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115:
                if(update)
                    vctResult = PEARL_MSD_ADV_VCT_OPEN;
                else
                    vctResult = result;
                break;
        case PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85:
                if(update)
                    vctResult = PEARL_MSD_ADV_VCT_SHORT;
                else
                    vctResult = result;
                break;
        default:
                vctResult = result;
                break;
    }

	MSD_DBG_INFO(("Pearl_getDetailedAdvVCTResult Exit.\n"));
    return vctResult;
}

/*******************************************************************************
* Pearl_analizeAdvVCTResult
*
* DESCRIPTION:
*       This routine analize the Advanced VCT result.
*
* INPUTS:
*       channel - channel number where test was run
*       crossChannelReg - register values after the test is completed
*       mode    - use formula for normal cable case
*
* OUTPUTS:
*       cableStatus - analized test result.
*
* RETURNS:
*       -1, or distance to fault
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MSD_16 Pearl_analizeAdvVCTNoCrosspairResult
(
    IN  int     channel, 
    IN  MSD_U16 *crossChannelReg, 
    IN  MSD_BOOL isShort,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    int len;
    MSD_16 dist2fault;
    PEARL_MSD_ADV_VCT_STATUS vctResult = PEARL_MSD_ADV_VCT_NORMAL;

    MSD_DBG_INFO(("Pearl_analizeAdvVCTNoCrosspairResult Called.\n"));

    dist2fault = -1;

    /* check if test is failed */
    if(Pearl_IS_VCT_FAILED(crossChannelReg[channel]))
    {
		cableStatus->cableStatus[channel] = PEARL_MSD_ADV_VCT_FAIL;
		MSD_DBG_INFO(("Pearl_analizeAdvVCTNoCrosspairResult Exit.\n"));
       return dist2fault;
    }

    /* Check if fault detected */
    if(Pearl_IS_ZERO_AMPLITUDE(crossChannelReg[channel]))
    {
       cableStatus->cableStatus[channel] = PEARL_MSD_ADV_VCT_NORMAL;
	   MSD_DBG_INFO(("Pearl_analizeAdvVCTNoCrosspairResult Exit.\n"));
       return dist2fault;
    }

    /* find out test result by reading Amplitude */
    if(Pearl_IS_POSITIVE_AMPLITUDE(crossChannelReg[channel]))
    {
       vctResult = PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115;
    }
    else
    {
       vctResult = PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85;
    }

    /* 
     * now, calculate the distance for PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115 and
     * PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85
     */
     if(!isShort)
     {
       len = (int)PEARL_MSD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
     }
     else
     {
       len = (int)PEARL_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
     }
     /*MSD_DBG_INFO(("@@@@ no cross len %d\n", len));*/
     if (len < 0)
      len = 0;
     cableStatus->u[channel].dist2fault = (MSD_16)len;
     vctResult = Pearl_getDetailedAdvVCTResult(
                 PEARL_GET_AMPLITUDE(crossChannelReg[channel]),
                 len,
                 vctResult);
     dist2fault = (MSD_16)len;

     cableStatus->cableStatus[channel] = vctResult;

	 MSD_DBG_INFO(("Pearl_analizeAdvVCTNoCrosspairResult Exit.\n"));
     return dist2fault;
}



static
MSD_16 Pearl_analizeAdvVCTResult
(
    IN  int     channel, 
    IN  MSD_U16 *crossChannelReg, 
    IN  MSD_BOOL isShort,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    int i, len;
    MSD_16 dist2fault;
    PEARL_MSD_ADV_VCT_STATUS vctResult = PEARL_MSD_ADV_VCT_NORMAL;

    /*MSD_DBG_INFO(("analizeAdvVCTResult(Crosspair) chan %d reg data %x\n", channel, crossChannelReg[channel]));*/
    MSD_DBG_INFO(("Pearl_analizeAdvVCTResult Called.\n"));

    dist2fault = -1;

    /* check if test is failed */
    for (i=0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
    {
        if(Pearl_IS_VCT_FAILED(crossChannelReg[i]))
        {
            cableStatus->cableStatus[channel] = PEARL_MSD_ADV_VCT_FAIL;
			MSD_DBG_INFO(("Pearl_analizeAdvVCTResult Exit.\n"));
            return dist2fault;
        }
    }

    /* find out test result by reading Amplitude */
    for (i=0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
    {
        if (i == channel)
        {
            if(!Pearl_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                if(Pearl_IS_POSITIVE_AMPLITUDE(crossChannelReg[i]))
                {
                    vctResult = PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115;
                }
                else
                {
                    vctResult = PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85;
                }
            }
            continue;
        }

        if(Pearl_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            continue;

        vctResult = PEARL_MSD_ADV_VCT_CROSS_PAIR_SHORT;
        break;
    }

    /* if it is cross pair short, check the distance for each channel */
    if(vctResult == PEARL_MSD_ADV_VCT_CROSS_PAIR_SHORT)
    {
        cableStatus->cableStatus[channel] = PEARL_MSD_ADV_VCT_CROSS_PAIR_SHORT;
        for (i=0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
        {
            if(Pearl_IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                cableStatus->u[channel].crossShort.channel[i] = MSD_FALSE;
                cableStatus->u[channel].crossShort.dist2fault[i] = 0;
                continue;
            }
                        
            cableStatus->u[channel].crossShort.channel[i] = MSD_TRUE;
            if(!isShort)
                len = (int)PEARL_MSD_ADV_VCT_CALC(crossChannelReg[i] & 0xFF);
            else
                len = (int)PEARL_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[i] & 0xFF);
            /*MSD_DBG_INFO(("@@@@ len %d\n", len));*/
            if (len < 0)
                len = 0;
            cableStatus->u[channel].crossShort.dist2fault[i] = (MSD_16)len;
            dist2fault = (MSD_16)len;
        }

		MSD_DBG_INFO(("Pearl_analizeAdvVCTResult Exit.\n"));
        return dist2fault;
    }

    /* 
     * now, calculate the distance for PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115 and
     * PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85
     */
    switch (vctResult)
    {
        case PEARL_MSD_ADV_VCT_IMP_GREATER_THAN_115:
        case PEARL_MSD_ADV_VCT_IMP_LESS_THAN_85:
            if(isShort)
                len = (int)PEARL_MSD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
            else
                len = (int)PEARL_MSD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].dist2fault = (MSD_16)len;
            vctResult = Pearl_getDetailedAdvVCTResult(
                            PEARL_GET_AMPLITUDE(crossChannelReg[channel]),
                            len,
                            vctResult);
            dist2fault = (MSD_16)len;
            break;
        default:
            break;
    }

    cableStatus->cableStatus[channel] = vctResult;

	MSD_DBG_INFO(("Pearl_analizeAdvVCTResult Exit.\n"));
    return dist2fault;
}

static 
MSD_STATUS Pearl_runAdvCableTest_1680_set
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort
)
{
    MSD_STATUS retVal;


    MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_set Called.\n"));
#if 0
    /* Set transmit channel */
   if (crosspair!=PEARL_MSD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
    if((retVal = Pearl_msdSetPagedPhyRegField(
        dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,11,3,0x4+channel)) != MSD_OK) 
    {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    /* 
     * start Advanced Virtual Cable Tester
     */
    if((retVal = Pearl_msdSetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
    {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
   }
   else
   {
    if (channel==0)
    {
      if((retVal = Pearl_msdSetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,11,3,0x000)) != MSD_OK)
      {
        MSD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = Pearl_msdSetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
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
      if((retVal = Pearl_msdSetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MSD_OK)
      {
		  MSD_DBG_ERROR(("Writing to paged phy reg returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
      }

#endif

	MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_set Exit.\n"));
	return MSD_OK;
}

static 
MSD_STATUS Pearl_runAdvCableTest_1680_check
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort
)
{
	MSD_STATUS retVal;
    MSD_U16 u16Data, count = 4000;

	MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_check Called.\n"));
    /* 
     * loop until test completion and result is valid
     */
    do {
        if((retVal = Pearl_msdReadPagedPhyReg(
                            dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MSD_OK)
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

	MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_check Exit.\n"));
    return MSD_OK;
}

static 
MSD_STATUS Pearl_runAdvCableTest_1680_get
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL    crosspair,
    IN  PEARL_MSD_ADV_VCT_MOD     advMod,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus,
    OUT MSD_BOOL         *tooShort
)
{
    MSD_STATUS retVal;
    MSD_U16 u16Data;
	MSD_U16 crossChannelReg[PEARL_MSD_MDI_PAIR_NUM];
    int j;
    MSD_16  dist2fault;
    MSD_BOOL         mode;
	MSD_BOOL         localTooShort[PEARL_MSD_MDI_PAIR_NUM];

    Pearl_SW_VCT_REGISTER regList[PEARL_MSD_MDI_PAIR_NUM] = {
                    {5,16},{5,17},{5,18},{5,19} };
    mode = MSD_TRUE;

    MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_get Called.\n"));


    if ((retVal = Pearl_msdReadPagedPhyReg(
                        dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from paged phy reg PEARL_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
    }

    /*MSD_DBG_INFO(("Page 5 of Reg23 after test : %0#x.\n", u16Data));*/

    /*
     * read the test result for the cross pair against selected MDI Pair
     */
    for (j=0; j<PEARL_MSD_MDI_PAIR_NUM; j++)
    {
      if((retVal = Pearl_msdReadPagedPhyReg(
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
	if (advMod == PEARL_MSD_ADV_VCT_OFFSET)
    {
		for (j = 0; j<PEARL_MSD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].offset = (crossChannelReg[j]>>8)&0xff;
      }
		MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_get Exit.\n"));
		return MSD_OK;
    }

    if (advMod==PEARL_MSD_ADV_VCT_SAMPLE_POINT)
    {
      for (j=0; j<PEARL_MSD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].sampleAmp = (crossChannelReg[j]>>8)&0xff;
      }
	  MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_get Exit.\n"));
      return MSD_OK;
    }

    for (j=0; j<PEARL_MSD_MDI_PAIR_NUM; j++)
    {
      if (crosspair!=PEARL_MSD_ADV_VCT_TCS_NO_CROSSPAIR)
        dist2fault = Pearl_analizeAdvVCTResult(j, crossChannelReg, mode&(*tooShort), cableStatus);
      else
        dist2fault = Pearl_analizeAdvVCTNoCrosspairResult(j, crossChannelReg, mode&(*tooShort), cableStatus);

      localTooShort[j]=MSD_FALSE;
      if((mode)&&(*tooShort==MSD_FALSE))
      {
        if ((dist2fault>=0) && (dist2fault<PEARL_MSD_ADV_VCT_ACCEPTABLE_SHORT_CABLE))
        {
           /*MSD_DBG_INFO(("@@@#@@@@ it is too short dist2fault %d\n", dist2fault));
           MSD_DBG_INFO(("Distance to Fault is too Short. So, rerun after changing pulse width\n"));*/
           localTooShort[j]=MSD_TRUE;
/*           *tooShort = MSD_TRUE;
           return MSD_OK;   Should return to stop for loop */
        }
      }
    }
    /* check and decide if length is too short */
    for (j=0; j<PEARL_MSD_MDI_PAIR_NUM; j++)
    {
         if (localTooShort[j]==MSD_FALSE) break;
    }

    if (j==PEARL_MSD_MDI_PAIR_NUM)
          *tooShort = MSD_TRUE;
  
	MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_get Exit.\n"));
    return MSD_OK;
}

static 
MSD_STATUS Pearl_runAdvCableTest_1680
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL   crosspair,
    IN  PEARL_MSD_ADV_VCT_MOD     advMod,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus,
    OUT MSD_BOOL         *tooShort
)
{
  MSD_STATUS retVal;
  /*MSD_32  channel;*/
  
  MSD_DBG_INFO(("Pearl_runAdvCableTest_1680 Called.\n"));

/*  if (mode)
        *tooShort = MSD_FALSE;  

  if (crosspair!=PEARL_MSD_ADV_VCT_TCS_NO_CROSSPAIR)
  {
     channel = crosspair - PEARL_MSD_ADV_VCT_TCS_CROSSPAIR_0;
  }
  else
  {
     channel = 0;
  }*/
/*  for (channel=0; channel<PEARL_MSD_MDI_PAIR_NUM; channel++) */
  {
    /* Set transmit channel */
    if((retVal=Pearl_runAdvCableTest_1680_set(dev,hwPort)) != MSD_OK) 
    {
		MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_set returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }


    /* 
     * loop until test completion and result is valid
     */
      if ((retVal=Pearl_runAdvCableTest_1680_check(dev,hwPort)) != MSD_OK)
      {
		MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_check returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
      }

        /*
         * read the test result for the cross pair against selected MDI Pair
         */
      if((retVal=Pearl_runAdvCableTest_1680_get(dev,hwPort, crosspair,
                     advMod, cableStatus,(MSD_BOOL *)tooShort)) != MSD_OK)
      {
		  MSD_DBG_INFO(("Pearl_runAdvCableTest_1680_get returned: %s.\n", msdDisplayStatus(retVal)));
		  return retVal;
      }
  }
  MSD_DBG_INFO(("Pearl_runAdvCableTest_1680 Exit.\n"));
  return MSD_OK;
}

static 
MSD_STATUS Pearl_getAdvCableStatus_1680
(    
    IN  MSD_QD_DEV          *dev,
    IN  MSD_U8           hwPort,
    IN  PEARL_MSD_ADV_VCT_MODE mode,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    MSD_STATUS retVal;
    MSD_U16 orgPulse, u16Data;
    MSD_BOOL flag, tooShort;
    PEARL_MSD_ADV_VCT_TRANS_CHAN_SEL crosspair;


    MSD_DBG_INFO(("Pearl_getAdvCableStatus_1680 Called.\n"));

    /* flag = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MSD_TRUE:MSD_FALSE; */
	flag = MSD_TRUE;
    crosspair = mode.transChanSel;

	orgPulse = 0;
    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case PEARL_MSD_ADV_VCT_FIRST_PEAK:
        case PEARL_MSD_ADV_VCT_MAX_PEAK:
        case PEARL_MSD_ADV_VCT_OFFSET:
        case PEARL_MSD_ADV_VCT_SAMPLE_POINT:
                break;
        default:
				MSD_DBG_ERROR(("Bad parameter mode: %d.\n", mode.mode));
                return MSD_BAD_PARAM;
    }

    if((retVal = Pearl_msdGetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading paged phy reg PEARL_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
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
    if((retVal = Pearl_msdSetPagedPhyRegField(
                    dev,hwPort,5,PEARL_QD_REG_ADV_VCT_CONTROL_5,0,14,u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing paged phy reg PEARL_QD_REG_ADV_VCT_CONTROL_5 returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
	/* Clear samplePointDist */
    u16Data = 0;
    if(mode.mode==PEARL_MSD_ADV_VCT_SAMPLE_POINT)
    {
      if (mode.samplePointDist) u16Data = (mode.samplePointDist&0x3ff) ;
    }
    if((retVal = Pearl_msdSetPagedPhyRegField(
                  dev,hwPort,5,PEARL_QD_REG_ADV_VCT_SMPL_DIST,0,9,u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing paged phy reg PEARL_QD_REG_ADV_VCT_SMPL_DIST returned: %s.\n", msdDisplayStatus(retVal)));
      return retVal;
    }
    if (flag)
    {
        /* save original Pulse Width */
        if((retVal = Pearl_msdGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,&orgPulse)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Reading paged phy reg to save original Pulse Width returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        /* set the Pulse Width with default value */
        if (orgPulse != 0)
        {
            if((retVal = Pearl_msdSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Writing paged phy reg to set the Pulse Width default value returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }
        }
        tooShort=MSD_FALSE;
    }
    if((retVal=Pearl_runAdvCableTest_1680(dev,hwPort,crosspair, mode.mode,
                cableStatus,&tooShort)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_runAdvCableTest_1680 returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (flag)
    {
        if(tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = Pearl_msdSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Writing paged phy reg to set the Pulse Width with minimum width returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }

            /* run the Adv VCT again */
            MSD_DBG_INFO(("Cable is too short, try again!\n"));
            if((retVal=Pearl_runAdvCableTest_1680(dev,hwPort,crosspair,mode.mode,
                        cableStatus,&tooShort)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Pearl_runAdvCableTest_1680 again returned: %s.\n", msdDisplayStatus(retVal)));
                return retVal;
            }

        }

        /* set the Pulse Width back to the original value */
        if((retVal = Pearl_msdSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,orgPulse)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Writing paged phy reg to set back original Pulse Width returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

	MSD_DBG_INFO(("Pearl_getAdvCableStatus_1680 Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvctGetAdvCableDiag
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
MSD_STATUS Pearl_gvctGetAdvCableDiag
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN  PEARL_MSD_ADV_VCT_MODE mode,
    OUT PEARL_MSD_ADV_CABLE_STATUS *cableStatus
)
{
    MSD_STATUS status;
    MSD_U8 hwPort;
    MSD_U16 u16Data, org0;
    MSD_BOOL            autoNeg;
    MSD_U16            pageReg;

    MSD_DBG_INFO(("Pearl_gvctGetAdvCableDiag Called.\n"));
    hwPort = MSD_LPORT_2_PHY(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %d.\n", hwPort));
        return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);

    if((status= Pearl_msdGetSMIPhyRegField(dev,hwPort,22,0,8,&pageReg)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Read Phy Page Register returned: %s.\n", msdDisplayStatus(status)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return status;
    }
    /*
     * Check the link
     */
    if((status= Pearl_msdReadPagedPhyReg(dev,hwPort,0,17,&u16Data)) != MSD_OK)
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
        if((status= Pearl_msdReadPagedPhyReg(dev,hwPort,0,0,&u16Data)) != MSD_OK)
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
            if((status= Pearl_msdWritePagedPhyReg(dev,hwPort,0,0,u16Data)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Disable the auto-neg returned: %s.\n", msdDisplayStatus(status)));
				msdSemGive(dev->devNum, dev->phyRegsSem);
                return status;
            }

            if((status= Pearl_msdPhyReset(dev, hwPort, 0, 0xFF)) != MSD_OK)
            {
				MSD_DBG_ERROR(("Reset the Phy returned: %s.\n", msdDisplayStatus(status)));
				msdSemGive(dev->devNum, dev->phyRegsSem);
                return status;
            }
            autoNeg = MSD_TRUE;        
        }
    }
	
    /* status = getAdvCableStatus_1116(dev,hwPort,mode,cableStatus); */
	status = Pearl_getAdvCableStatus_1680(dev,hwPort,mode,cableStatus);
	if(status != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_getAdvCableStatus_1680 returned: %s.\n", msdDisplayStatus(status)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return status;
	}
	
    if (autoNeg)
    {
        if((status= Pearl_msdPhyReset(dev,hwPort, 0, org0)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Reset the Phy returned: %s.\n", msdDisplayStatus(status)));
			msdSemGive(dev->devNum, dev->phyRegsSem);
            return status;
        }
    }
	
    if((status= Pearl_msdSetSMIPhyRegField(dev,hwPort,22,0,8,pageReg)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Set back Phy Page Register returned: %s.\n", msdDisplayStatus(status)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return status;
    }
    msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gvctGetAdvCableDiag Exit.\n"));
    return status;    
}


/*******************************************************************************
* Pearl_dspLookup
*
* DESCRIPTION:
*       This routine returns cable length (meters) by reading DSP Lookup table.
*
* INPUTS:
*       regValue - register 21
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
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
MSD_STATUS Pearl_dspLookup
(
    IN  MSD_U16 regValue, 
	IN  MSD_U16 gainIndex,
    OUT MSD_32  *cableLen
)
{
    MSD_U16 startEntry,tableEntry;
    MSD_U8* tbl;

	MSD_DBG_INFO(("Pearl_dspLookup Called.\n"));

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
		startEntry = 29;
		tableEntry = 69;
		tbl = Pearl_tbl_1680_A2_gain2;
	}
	else if(gainIndex == 1)
	{	
		startEntry = 20;
		tableEntry = 81;
		tbl = Pearl_tbl_1680_A2_gain1;
	}
	else if(gainIndex == 0)
	{	
		startEntry = 49;
		tableEntry = 110;
		tbl = Pearl_tbl_1680_A2_gain0;
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

	MSD_DBG_INFO(("Pearl_dspLookup Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_getDSPDistance_1680
*
* DESCRIPTION:
*       This routine returns cable length (meters) from DSP method.
*       This routine is for the 88E1680 like devices.
*
* INPUTS:
*       mdi - pair of each MDI (0..3).
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
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
MSD_STATUS Pearl_getDSPDistance_1680
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  hwPort,
    IN  MSD_U32 mdi,
    OUT MSD_32 *cableLen
)
{
    MSD_U16     data, retryCount, gainIndex;
    MSD_STATUS  retVal;

    MSD_DBG_INFO(("Pearl_getDSPDistance_1680 Called.\n"));

    /* Set the required bits for Cable length register */
    if((retVal = Pearl_msdWritePagedPhyReg(dev,hwPort,0xff,16,(MSD_U16)(0x1118+(0xff&mdi)))) != MSD_OK)
    {
		MSD_DBG_ERROR(("Writing to required bits for Cable len register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retryCount = 1000;

    do
    {
        if(retryCount == 0)
        {
			MSD_DBG_ERROR(("Ready bit of Cable len register is not set.\n"));
            return MSD_FAIL;
        }

        /* Check the ready bit of Cable length register */
        if((retVal = Pearl_msdGetPagedPhyRegField(dev,hwPort,0xff,16,15,1,&data)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Check the ready bit of Cable len register returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        retryCount--;

    } while(!data);

    /* read length of MDI pair */
    if((retVal = Pearl_msdReadPagedPhyReg(dev,hwPort,0xff,18,&data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading len of MDI pair returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	/* read gain Index of MDI pair */
    if((retVal = Pearl_msdGetPagedPhyRegField(dev,hwPort,0xfc, 23, 0, 2, &gainIndex)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading gain len of MDI pair returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	if ((retVal = Pearl_dspLookup(data, gainIndex, cableLen)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Pearl_dspLookup returned %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Pearl_getDSPDistance_1680 Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Pearl_getExStatus
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
static MSD_STATUS Pearl_getExStatus
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8            hwPort,
    OUT PEARL_MSD_ADV_EXTENDED_STATUS *extendedStatus
)
{
    MSD_STATUS retVal;
    MSD_U16 u16Data, i;

	MSD_DBG_INFO(("Pearl_getExStatus Called.\n"));

    extendedStatus->isValid = MSD_FALSE;
	
	for (i = 0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
	{
		if((retVal = Pearl_getDSPDistance_1680(dev,hwPort,i,(MSD_32 *)&extendedStatus->cableLen[i])) != MSD_OK)
		{
			MSD_DBG_ERROR(("Pearl_getDSPDistance_1680 returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}

    /*
     * get data from 21_5 register for pair swap
     */
    if((retVal = Pearl_msdReadPagedPhyReg(
                    dev,hwPort,5,PEARL_QD_REG_PAIR_SWAP_STATUS,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from 21_5 register for pair swap returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* if bit 6 is not set, it's not valid. */
    if (!(u16Data & 0x40))
    {
        MSD_DBG_INFO(("Pearl_getExStatus Exit, Valid Bit is not set (%0#x).\n", u16Data));
        return MSD_OK;
    }

    extendedStatus->isValid = MSD_TRUE;
    
    /* get Pair Polarity */
    for(i=0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
    {
        switch((u16Data >> i) & 0x1)
        {
            case 0:
                extendedStatus->pairPolarity[i] = PEARL_MSD_POSITIVE;
                break;
            default:
                extendedStatus->pairPolarity[i] = PEARL_MSD_NEGATIVE;
            break;
        }
    }

    /* get Pair Swap for Channel A and B */
    if (u16Data & 0x10)
    {
        extendedStatus->pairSwap[0] = PEARL_MSD_CHANNEL_A;
        extendedStatus->pairSwap[1] = PEARL_MSD_CHANNEL_B;
    }
    else
    {
        extendedStatus->pairSwap[0] = PEARL_MSD_CHANNEL_B;
        extendedStatus->pairSwap[1] = PEARL_MSD_CHANNEL_A;
    }

    /* get Pair Swap for Channel C and D */
    if (u16Data & 0x20)
    {
        extendedStatus->pairSwap[2] = PEARL_MSD_CHANNEL_C;
        extendedStatus->pairSwap[3] = PEARL_MSD_CHANNEL_D;
    }
    else
    {
        extendedStatus->pairSwap[2] = PEARL_MSD_CHANNEL_D;
        extendedStatus->pairSwap[3] = PEARL_MSD_CHANNEL_C;
    }

    /*
     * get data from 20_5 register for pair skew
     */
    if((retVal = Pearl_msdReadPagedPhyReg(
                    dev,hwPort,5,PEARL_QD_REG_PAIR_SKEW_STATUS,&u16Data)) != MSD_OK)
    {
		MSD_DBG_ERROR(("Reading from 20_5 register for pair skew returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* get Pair Skew */
    for(i=0; i<PEARL_MSD_MDI_PAIR_NUM; i++)
    {
        extendedStatus->pairSkew[i] = ((u16Data >> i*4) & 0xF) * 8;
    }

	MSD_DBG_INFO(("Pearl_getExStatus Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Pearl_gvctGetAdvExtendedStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable length is also provided.
*
* INPUTS:
*       dev  - pointer to GT driver structure returned from mdLoadDriver
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
*        None
*
*******************************************************************************/
MSD_STATUS Pearl_gvctGetAdvExtendedStatus
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT PEARL_MSD_ADV_EXTENDED_STATUS *extendedStatus
)
{
    MSD_STATUS retVal;
    MSD_U8 hwPort;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gvctGetAdvExtendedStatus Called.\n"));
    hwPort = MSD_LPORT_2_PHY(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %d.\n", hwPort));
        return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);

	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Read Phy Page Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	if((retVal = Pearl_getExStatus(dev,hwPort,extendedStatus)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Getting Extanded Cable Status returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
	{
		MSD_DBG_ERROR(("Set back Phy Page Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

    msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gvctGetAdvExtendedStatus Exit.\n"));
    return retVal;
}

