#include <msdCopyright.h>

/******************************************************************************
* Topaz_msdPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <topaz/include/api/Topaz_msdPTP.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

typedef enum
{
    MSD_AVB_BLOCK_PTP = 0x0,
    MSD_AVB_BLOCK_AVB_POLICY = 0x1,
    MSD_AVB_BLOCK_QAV = 0x2,
    MSD_AVB_BLOCK_QBV = 0x3
} MSD_AVB_BLOCK;

/*
* Typedef: enum MSD_PTP_OPERATION
*
* Description: Defines the PTP (Precise Time Protocol) Operation type
*
* Fields:
*      PTP_WRITE_DATA             - Write data to the PTP register
*      PTP_READ_DATA            - Read data from PTP register
*      PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
*      PTP_READ_TIMESTAMP_DATA    - Read timestamp data from PTP register
*                    valid bit will be reset after read
*/
typedef enum
{
    PTP_WRITE_DATA = 0x3,
    PTP_READ_DATA = 0x4,
    PTP_READ_MULTIPLE_DATA = 0x6,
    PTP_READ_TIMESTAMP_DATA = 0x8,
} MSD_PTP_OPERATION;

/*
*  typedef: struct MSD_PTP_OP_DATA
*
*  Description: data required by PTP Operation
*
*  Fields:
*      ptpPort        - physical port of the device
*      ptpBlock    - AVB Block
*      ptpAddr     - register address
*      ptpData     - data for ptp register.
*      ptpMultiData- used for multiple read operation.
*      nData         - number of data to be read on multiple read operation.
*/
typedef struct
{
    MSD_U32    ptpPort;
    MSD_U32    ptpBlock;
    MSD_U32    ptpAddr;
    MSD_U32    ptpData;
    MSD_U32    ptpMultiData[MAX_PTP_CONSECUTIVE_READ];
    MSD_U32    nData;
} MSD_PTP_OP_DATA;


/******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
static MSD_STATUS ptpOperationPerform
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_PTP_OPERATION      ptpOp,
    INOUT MSD_PTP_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32             i;
	MSD_U16          ptpOperation;
	MSD_U16 data;

    msdSemTake(dev->devNum,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
    data = 1;
    while(data == 1)
    {
		retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
            TOPAZ_QD_REG_AVB_COMMAND, 15, 1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    /* Set the PTP Operation register */
    switch (ptpOp)
    {
        case PTP_WRITE_DATA:
		{
            data = (MSD_U16)opData->ptpData;
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

			ptpOperation = PTP_WRITE_DATA;
            ptpOperation &= 3;
            ptpOperation <<=13;
            data = (MSD_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
		}
		break;

        case PTP_READ_DATA:
		{
	        ptpOperation = PTP_READ_DATA;
            ptpOperation &= 3;
            ptpOperation <<=13;
            data = (MSD_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                    TOPAZ_QD_REG_AVB_COMMAND, 15, 1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdGetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_DATA, &data);
			if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpData = (MSD_U32)data;
		}
        break;

        case PTP_READ_MULTIPLE_DATA:
		{
	        ptpOperation = PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3;
            ptpOperation <<=13;
            data = (MSD_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                    TOPAZ_QD_REG_AVB_COMMAND, 15, 1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            for(i=0; i<opData->nData; i++)
            {
                retVal = msdGetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                    TOPAZ_QD_REG_AVB_DATA, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
				opData->ptpMultiData[i] = (MSD_U32)data;
            }
		}
        break;

        case PTP_READ_TIMESTAMP_DATA:
		{
	        ptpOperation = PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3;
            ptpOperation <<=13;
            data = (MSD_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                    TOPAZ_QD_REG_AVB_COMMAND, 15, 1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdGetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_DATA, &data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpMultiData[0] = (MSD_U32)data;

            if (!(data & 0x1))
            {
                /* valid bit is not set */
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            for(i=1; i<opData->nData; i++)
            {
                retVal = msdGetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                    TOPAZ_QD_REG_AVB_DATA, &data);
                opData->ptpMultiData[i] = (MSD_U32)data;
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_DATA, 0);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

	        ptpOperation = PTP_WRITE_DATA;
            ptpOperation &= 3;
            ptpOperation <<=13;
            data = (MSD_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
		}
        break;

        default:
		{    
			msdSemGive(dev->devNum,dev->ptpRegsSem);
            return MSD_FAIL;
		}
    }

    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
            TOPAZ_QD_REG_AVB_COMMAND, 15, 1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    msdSemGive(dev->devNum,dev->ptpRegsSem);
    return retVal;
}

/*******************************************************************************
* gptpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM  - bad parameter
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static MSD_STATUS gptpSetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U32        regOffset,
    IN  MSD_U32        data
)
{
    MSD_STATUS           retVal;
    MSD_U32                hwPort;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA        opData;

    MSD_DBG_INFO(("gptpSetReg Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if ((regOffset > 0x1F) || (data > 0xFFFF))
    {
        MSD_DBG_ERROR(("Failed (Invalid reg offset/data).\n"));
        return MSD_BAD_PARAM;
    }

    op = PTP_WRITE_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpData = data;
    opData.ptpBlock = MSD_AVB_BLOCK_PTP;    /* PTP register space */

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed (ptpOperationPerform).\n"));
        return MSD_FAIL;
    }

    MSD_DBG_INFO(("OK.\n"));
    return MSD_OK;
}

/*******************************************************************************
* gptpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM  - bad parameter
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static MSD_STATUS gptpGetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT	  port,
    IN  MSD_U32        regOffset,
    OUT MSD_U32        *data
)
{
    MSD_STATUS           retVal;
    MSD_U32              hwPort;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA      opData;

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (regOffset > 0x1F)
    {
        MSD_DBG_ERROR(("Failed (Invalid reg offset).\n"));
        return MSD_BAD_PARAM;
    }

    op = PTP_READ_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpBlock = MSD_AVB_BLOCK_PTP;    /* PTP register space */

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed (ptpOperationPerform).\n"));
        return MSD_FAIL;
    }

    *data = opData.ptpData;

    return MSD_OK;
}



MSD_STATUS Topaz_gptpSetIntEnable
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN TOPAZ_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Topaz_gptpSetIntEnable Called.\n"));

    retval = gptpGetReg(dev, port, 0x2, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    data = data & (~3);
    data = data | (interruptEn->ptpArrIntEn ? 1 : 0) | ((interruptEn->ptpDepIntEn ? 1 : 0) << 1);
    retval = gptpSetReg(dev, port, 0x02, data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retval;
    }

    MSD_DBG_INFO(("Topaz_gptpSetIntEnable Exit.\n"));
    return retval;
}
MSD_STATUS Topaz_gptpGetIntEnable
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT TOPAZ_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Topaz_gptpGetIntEnable Called.\n"));

    retval = gptpGetReg(dev, port, 0x2, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    interruptEn->ptpArrIntEn = data & 0x1;
    interruptEn->ptpDepIntEn = (data & 0x02) >> 1;

    MSD_DBG_INFO(("Topaz_gptpGetIntEnable Exit.\n"));
    return retval;
}

MSD_STATUS Topaz_gptpSetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_BOOL  en
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Topaz_gptpSetProtPTPEn Called.\n"));

    retval = gptpGetReg(dev, port, 0x0, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    if (en)
        data &= ~0x1;
    else
        data |= 0x1;

    retval = gptpSetReg(dev, port, 0x0, data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retval;
    }

    MSD_DBG_INFO(("Topaz_gptpSetProtPTPEn Exit.\n"));
    return retval;
}

MSD_STATUS Topaz_gptpGetProtPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_BOOL  *en
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Topaz_gptpGetProtPTPEn Called.\n"));

    retval = gptpGetReg(dev, port, 0x0, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    *en = (data & 0x1) ? MSD_FALSE : MSD_TRUE;

    MSD_DBG_INFO(("Topaz_gptpGetProtPTPEn Exit.\n"));
    return retval;
}

MSD_STATUS Topaz_gptpSetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  TOPAZ_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    MSD_U32 data = 0;
    MSD_DBG_INFO(("Topaz_gptpSetPortConfig Called.\n"));

    if (ptpData->transSpec > 0xF)    /* 4 bits */
        return MSD_BAD_PARAM;

    if (ptpData->etJump > 0x1F)    /* 5 bits */
        return MSD_BAD_PARAM;

    if (ptpData->ipJump > 0x3F)    /* 6 bits */
        return MSD_BAD_PARAM;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    retVal = gptpGetReg(dev, port, 0x0, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }
    data = (ptpData->transSpec << 12) | (data & 0x1) |
        ((ptpData->disTSpec ? 1 : 0) << 11) |
        ((ptpData->disTSOverwrite ? 1 : 0) << 1);

    retVal = gptpSetReg(dev, port, 0x0, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retVal;
    }

    /* setting etJump and ipJump, offset 1 */
    data = (ptpData->ipJump << 8) | ptpData->etJump;
    retVal = gptpSetReg(dev, port, 0x1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retVal;
    }

    /* setting Int, offset 2 */
    retVal = gptpGetReg(dev, port, 0x2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }
    data &= 0x7c;
    data |= (ptpData->ptpArrIntEn ? 1 : 0) |
        ((ptpData->ptpDepIntEn ? 1 : 0) << 1);
    data |= ((ptpData->arrTSMode & 0xff) << 8);
    data |= ((ptpData->filterAct ? 1 : 0) << 7); /* Filter 802.1 Act from LEDs */
    retVal = gptpSetReg(dev, port, 0x2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retVal;
    }

    /* setting Led control, offset 3 */
    data = (ptpData->arrLedCtrl << 8) |
        (ptpData->depLedCtrl);
    retVal = gptpSetReg(dev, port, 0x3, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gptpSetPortConfig Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpGetPortConfig
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT TOPAZ_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    MSD_U32 data = 0;
    MSD_DBG_INFO(("Topaz_gptpGetPortConfig Called.\n"));

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    retVal = gptpGetReg(dev, port, 0x0, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }
    ptpData->transSpec = data >> 12;
    ptpData->disTSpec = ((data >> 11) & 0x1) ? MSD_TRUE : MSD_FALSE;
    ptpData->disTSOverwrite = ((data >> 1) & 0x1) ? MSD_TRUE : MSD_FALSE;

    /* getting MsgIDTSEn, offset 1 */
    retVal = gptpGetReg(dev, port, 0x1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }
    ptpData->ipJump = (data >> 8) & 0x3F;
    ptpData->etJump = data & 0x1F;

    /* getting TSArrPtr, offset 2 */
    retVal = gptpGetReg(dev, port, 0x2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }
    ptpData->ptpDepIntEn = ((data >> 1) & 0x1) ? MSD_TRUE : MSD_FALSE;
    ptpData->ptpArrIntEn = (data & 0x1) ? MSD_TRUE : MSD_FALSE;
    ptpData->arrTSMode = (data & 0xff00) >> 8;
    ptpData->filterAct = ((data >> 7) & 0x1) ? MSD_TRUE : MSD_FALSE;

    /* getting Led control, offset 3 */
    retVal = gptpGetReg(dev, port, 0x3, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }

    ptpData->arrLedCtrl = (data & 0xff00) >> 8;
    ptpData->depLedCtrl = data & 0xff;
    
    MSD_DBG_INFO(("Topaz_gptpGetPortConfig Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpGetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  TOPAZ_PTP_TIME    timeToRead,
    OUT TOPAZ_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    MSD_U8 hwPort;
    MSD_PTP_OPERATION op;
    MSD_PTP_OP_DATA opData;
    MSD_U32 baseReg;

    MSD_DBG_INFO(("Topaz_gptpGetTimeStamp Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    baseReg = 8;

    switch (timeToRead)
    {
    case TOPAZ_PTP_ARR0_TIME:
        opData.ptpAddr = baseReg + 0;
        break;
    case TOPAZ_PTP_ARR1_TIME:
        opData.ptpAddr = baseReg + 4;
        break;
    case TOPAZ_PTP_DEP_TIME:
        opData.ptpAddr = baseReg + 8;
        break;
    default:
        MSD_DBG_ERROR(("Invalid time to be read\n"));
        return MSD_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpBlock = MSD_AVB_BLOCK_PTP;


    op = PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed ptpOperation.\n"));
        return retVal;
    }

    ptpStatus->isValid = (opData.ptpMultiData[0] & 0x1) ? MSD_TRUE : MSD_FALSE;
    ptpStatus->status = (MSD_PTP_INT_STATUS)((opData.ptpMultiData[0] >> 1) & 0x3);
    ptpStatus->timeStamped = TOPAZMSD_PTP_BUILD_TIME(opData.ptpMultiData[2], opData.ptpMultiData[1]);
    ptpStatus->ptpSeqId = opData.ptpMultiData[3];

    MSD_DBG_INFO(("Topaz_gptpGetTimeStamp Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpGetTSValidSt
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
)
{
    MSD_STATUS  retVal;
    MSD_U32 data = 0;
    MSD_U32 baseReg;
    MSD_DBG_INFO(("Topaz_gptpGetTSValidSt Called.\n"));

    baseReg = 8;

    switch (timeToRead)
    {
    case TOPAZ_PTP_ARR0_TIME:
        baseReg = baseReg + 0;
        break;
    case TOPAZ_PTP_ARR1_TIME:
        baseReg = baseReg + 4;
        break;
    case TOPAZ_PTP_DEP_TIME:
        baseReg = baseReg + 8;
        break;
    default:
        MSD_DBG_ERROR(("Invalid time to get valid\n"));
        return MSD_BAD_PARAM;
    }

    retVal = gptpGetReg(dev, port, baseReg, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }

    *isValid = (data & 0x1) ? MSD_TRUE : MSD_FALSE;
    MSD_DBG_INFO(("Topaz_gptpGetTSValidSt Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpResetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToReset
)
{
    MSD_STATUS  retVal;
    MSD_U32 baseReg;
    int index;
    MSD_DBG_INFO(("Topaz_gptpResetTimeStamp Called.\n"));

    switch (timeToReset)
    {
    case TOPAZ_PTP_ARR0_TIME:
        baseReg = 8;
        break;
    case TOPAZ_PTP_ARR1_TIME:
        baseReg = 12;
        break;
    case TOPAZ_PTP_DEP_TIME:
        baseReg = 16;
        break;
    default:
        MSD_DBG_ERROR(("Invalid time to reset\n"));
        return MSD_BAD_PARAM;
    }

    for (index = 0; index < 4; index++)
    {
        MSD_U32 tempAddr = baseReg + index;
        retVal = gptpSetReg(dev, port, tempAddr, 0);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Set the PTP register error\n"));
            return retVal;
        }
    }

    MSD_DBG_INFO(("Topaz_gptpResetTimeStamp Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpSetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    IN  TOPAZ_PTP_GLOBAL_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    MSD_PTP_OPERATION op;
    MSD_PTP_OP_DATA opData;
    MSD_DBG_INFO(("Topaz_gptpResetTimeStamp Called.\n"));

    /* setting PTPEType, offset 0 */
    opData.ptpBlock = MSD_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed writing PTPEType.\n"));
        return retVal;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return retVal;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return retVal;
    }
    
    MSD_DBG_INFO(("Topaz_gptpResetTimeStamp Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpGetGlobalConfig
(
    IN  MSD_QD_DEV* dev,
    OUT TOPAZ_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    MSD_PTP_OPERATION op;
    MSD_PTP_OP_DATA opData;
    MSD_DBG_INFO(("Topaz_gptpGetGlobalConfig Called.\n"));

    opData.ptpBlock = MSD_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed reading PTPEType.\n"));
        return retVal;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return retVal;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return retVal;
    }

    ptpData->tsArrPtr = opData.ptpData;

    MSD_DBG_INFO(("Topaz_gptpGetGlobalConfig Exit.\n"));
    return retVal;
}

MSD_STATUS Topaz_gptpGetIntStatus
(
    IN  MSD_QD_DEV* dev,
    OUT TOPAZ_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    MSD_PTP_OPERATION op;
    MSD_PTP_OP_DATA opData;
    MSD_DBG_INFO(("Topaz_gptpGetIntStatus Called.\n"));

    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_DATA;
    opData.ptpAddr = 8;

    if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading PTP status.\n"));
        return retVal;
    }

    ptpIntSt->ptpIntStPortVect = (opData.ptpData & ((1 << 11) - 1));
    ptpIntSt->ptpTrigGenInt = (opData.ptpData >> 15) & 1;
    ptpIntSt->ptpEventInt = (opData.ptpData >> 14) & 1;

    MSD_DBG_INFO(("Topaz_gptpGetIntStatus Exit.\n"));
    return retVal;
}