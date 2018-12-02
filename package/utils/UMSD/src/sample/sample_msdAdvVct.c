#include <msdCopyright.h>
/********************************************************************************
* sample_msdAdvVct.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Advanced TDR VCT test
*    2. Advanced DSP VCT test
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

static
void displayAdvVCTResult
(
    MSD_ADV_CABLE_STATUS *cableStatus,
    int    channel,
    MSD_ADV_VCT_MODE mode
);

/* Advanced VCT (TDR) for same pair */
MSD_STATUS sample_advVctTestSamePair(MSD_U8 devNum)
{
    MSD_STATUS status;
    MSD_LPORT port;
    MSD_ADV_VCT_MODE mode;
    MSD_ADV_CABLE_STATUS advCableStatus;
    int i, j;

    MSD_ADV_VCT_MOD mod[4] = {
        MSD_ADV_VCT_FIRST_PEAK,
        MSD_ADV_VCT_MAX_PEAK,
		MSD_ADV_VCT_OFFSET,
		MSD_ADV_VCT_SAMPLE_POINT
    };

    char modeStr[4][32] = {
        "(Adv VCT First PEAK)",
        "(Adv VCT MAX PEAK)",
		"(ADV VCT OFFSET)",
		"(ADV VCT SAMPLE POINT)"
    };


    port = 3; /* must be 1~8 ,Peridot internal phy; 1~4 ,topaz internal phy; 0~4 ,Agate internal phy; 3~4 ,Pearl internal phy*/

    MSG_PRINT(("!!!! sample adv Cable Test Result for Port %i\n",(int)port));

	if (sohoDevGet(devNum) == NULL)
    {
        MSG_PRINT(("GT driver is not initialized\n"));
        return MSD_FAIL;
    }

    for (j=0; j<4; j++)
    {
        mode.mode = mod[j];
        mode.transChanSel = MSD_ADV_VCT_TCS_NO_CROSSPAIR;
        mode.sampleAvg = MSD_ADV_VCT_SAVG_2;
        mode.peakDetHyst = 0;
        if(mode.mode == MSD_ADV_VCT_SAMPLE_POINT){
        	mode.samplePointDist = 10;  /* sample point mode use only */
        }

        /*
         *    Start and get Cable Test Result
         */
        status = MSD_OK;
        MSG_PRINT(("!!!! 1 sample adv Cable Test Result for Port %i\n",(int)port));

        if((status = msdVctGetAdvCableDiag(devNum,port,mode,&advCableStatus)) != MSD_OK)
        {
            MSG_PRINT(("msdVctGetAdvCableDiag return Failed\n"));
            return status;
        }

        MSG_PRINT(("\nCable Test Result %s for Port %i\n", modeStr[j], (int)port));

        for(i=0; i<MSD_MDI_PAIR_NUM; i++)
        {
            MSG_PRINT(("MDI PAIR %i:\n",i));
            displayAdvVCTResult(&advCableStatus, i, mode);
        }
    }

    MSG_PRINT(("sample_advVctTestSamePair code success.\n"));
    return MSD_OK;
}

/* Advanced VCT (TDR) for cross pair */
MSD_STATUS sample_advVctTestCrossPair(MSD_U8 devNum)
{
    MSD_STATUS status;
    MSD_LPORT port;
    MSD_ADV_VCT_MODE mode;
    MSD_ADV_CABLE_STATUS advCableStatus;
    int j;

    MSD_ADV_VCT_MOD mod[1] = {
        MSD_ADV_VCT_FIRST_PEAK,
    };

    char modeStr[1][32] = {
        "(Adv VCT First PEAK)"
    };

    port = 3; /*  must be 1~8 ,Peridot internal phy; 1~4 ,topaz internal phy*/

    MSG_PRINT(("!!!! sample adv Cable Test Result for Port %i\n",(int)port));

	if (sohoDevGet(devNum) == NULL)
    {
        MSG_PRINT(("GT driver is not initialized\n"));
        return MSD_FAIL;
    }

    for (j=0; j<1; j++)
    {
        mode.mode = mod[j];
        mode.transChanSel = MSD_ADV_VCT_TCS_CROSSPAIR_0;
        mode.sampleAvg = MSD_ADV_VCT_SAVG_2;
        mode.peakDetHyst =0;

        /*
         *    Start and get Cable Test Result
         */
        status = MSD_OK;
        MSG_PRINT(("!!!! 1 sample adv Cable Test Result for Port %i\n",(int)port));

        if((status = msdVctGetAdvCableDiag(devNum,port,mode,&advCableStatus)) != MSD_OK)
        {
            MSG_PRINT(("msdVctGetAdvCableDiag return Failed\n"));
            return status;
        }

        MSG_PRINT(("\nCable Test Result %s for Port %i\n", modeStr[j], (int)port));

        MSG_PRINT(("MDI PAIR %i:\n",mode.transChanSel-4));
        displayAdvVCTResult(&advCableStatus, mode.transChanSel-4, mode);
    }

    MSG_PRINT(("sample_advVctTestCrossPair code success.\n"));
    return MSD_OK;
}

static
void displayAdvVCTResult
(
    MSD_ADV_CABLE_STATUS *cableStatus,
    int    channel,
    MSD_ADV_VCT_MODE mode
)
{
    int i, j=channel;

    if(mode.mode==MSD_ADV_VCT_OFFSET)
    {
        if(cableStatus->u[j].offset==0)
          MSG_PRINT(("Advanced VCT offset pair %x test failed\n", j));
        else if(cableStatus->u[j].offset==0x80)
          MSG_PRINT(("Advanced VCT offset pair %x is 0 \n", j));
        else
          MSG_PRINT(("Advanced VCT offset pair %x is %d \n", j, cableStatus->u[j].offset));

      return;
    }
    if(mode.mode==MSD_ADV_VCT_SAMPLE_POINT)
    {
        if(cableStatus->u[j].sampleAmp&0x80)
          MSG_PRINT(("Advanced VCT sample point return pair %x is -%d \n", j, cableStatus->u[j].sampleAmp&0x7f));
        else
          MSG_PRINT(("Advanced VCT sample point return pair %x is +%d \n", j, cableStatus->u[j].sampleAmp&0x7f));

      return;
    }

    switch(cableStatus->cableStatus[channel])
    {
        case MSD_ADV_VCT_FAIL:
            MSG_PRINT(("Advanced Cable Test Failed\n"));
            break;
        case MSD_ADV_VCT_NORMAL:
            MSG_PRINT(("Cable Test Passed. No problem found.\n"));
            break;
        case MSD_ADV_VCT_IMP_GREATER_THAN_115:
            MSG_PRINT(("Cable Test Passed. Impedance is greater than 115 Ohms.\n"));
            MSG_PRINT(("Approximately %i meters from the tested port.\n",cableStatus->u[channel].dist2fault));
            break;
        case MSD_ADV_VCT_IMP_LESS_THAN_85:
            MSG_PRINT(("Cable Test Passed. Impedance is less than 85 Ohms.\n"));
            MSG_PRINT(("Approximately %i meters from the tested port.\n",cableStatus->u[channel].dist2fault));
            break;
        case MSD_ADV_VCT_OPEN:
            MSG_PRINT(("Cable Test Passed. Open Cable.\n"));
            MSG_PRINT(("Approximately %i meters from the tested port.\n",cableStatus->u[channel].dist2fault));
            break;
        case MSD_ADV_VCT_SHORT:
            MSG_PRINT(("Cable Test Passed. Shorted Cable.\n"));
            MSG_PRINT(("Approximately %i meters from the tested port.\n",cableStatus->u[channel].dist2fault));
            break;
        case MSD_ADV_VCT_CROSS_PAIR_SHORT:
            MSG_PRINT(("Cable Test Passed.\n"));
            for(i=0; i<MSD_MDI_PAIR_NUM; i++)
            {
                if(cableStatus->u[channel].crossShort.channel[i] == MSD_TRUE)
                {
                    MSG_PRINT(("\tCross pair short in channel %i.\n",i));
                    MSG_PRINT(("\tApproximately %i meters from the tested port.\n",
                                    cableStatus->u[channel].crossShort.dist2fault[i]));
                }
            }
            break;
        default:
            MSG_PRINT(("Unknown Test Result.\n"));
            break;
    }
}

/* Advanced DSP VCT */
MSD_STATUS sample_getAdvExtendedStatus(MSD_U8 devNum)
{
    MSD_STATUS status;
    MSD_LPORT port;
    MSD_ADV_EXTENDED_STATUS extendedStatus;
    int i;
    char ch;

    port = 3; /*  must be 1~8 ,Peridot internal phy; 1~4 ,topaz internal phy*/

	if (sohoDevGet(devNum) == NULL)
    {
        MSG_PRINT(("GT driver is not initialized\n"));
        return MSD_FAIL;
    }

    /*
     *     Start getting Extended Information.
     */
	if ((status = msdVctGetAdvExtendStatus(devNum, port, &extendedStatus)) != MSD_OK)
    {
        MSG_PRINT(("msdVctGetAdvExtendStatus return Failed\n"));
        return status;
    }

    if (!extendedStatus.isValid)
    {
        MSG_PRINT(("Not able to get Extended Status.\n"));
        MSG_PRINT(("Please check if 1000B-T Link is established on Port %i.\n",(int)port));
        return status;
    }

    /* Pair Polarity */
    MSG_PRINT(("Pair Polarity:\n"));
    for(i=0; i<MSD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT(("MDI PAIR %i: %s\n",i,
                    (extendedStatus.pairPolarity[i] == MSD_POSITIVE)?"Positive":"Negative"));
    }

    /* Pair Swap */
    MSG_PRINT(("Pair Swap:\n"));
    for(i=0; i<MSD_MDI_PAIR_NUM; i++)
    {
        switch(extendedStatus.pairSwap[i])
        {
            case MSD_CHANNEL_A:
                ch = 'A';
                break;
            case MSD_CHANNEL_B:
                ch = 'B';
                break;
            case MSD_CHANNEL_C:
                ch = 'C';
                break;
            case MSD_CHANNEL_D:
                ch = 'D';
                break;
            default:
                MSG_PRINT(("Error: reported unknown Pair Swap %i\n",extendedStatus.pairSwap[i]));
                ch = 'U';
                break;
        }

        MSG_PRINT(("MDI PAIR %i: Channel %c\n",i,ch));
    }

    /* Pair Skew */
    MSG_PRINT(("Pair Skew:\n"));
    for(i=0; i<MSD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT(("MDI PAIR %i: %ins\n",i,(int)extendedStatus.pairSkew[i]));
    }

    /* Cable Len */
    MSG_PRINT(("Cable Len:\n"));
    for(i=0; i<MSD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT(("MDI PAIR %i: approximately %im\n",i,(int)extendedStatus.cableLen[i]));
    }

    MSG_PRINT(("sample_getAdvExtendedStatus code success.\n"));
    return MSD_OK;
}
