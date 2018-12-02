#include <msdCopyright.h>

/******************************************************************************
* Topaz_msdQav.c
*
* DESCRIPTION:
*       API definitions for Qav logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#include <topaz/include/api/Topaz_msdQav.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>

#define MAX_AVB_CONSECUTIVE_READ    4

typedef enum
{
	MSD_AVB_BLOCK_PTP = 0x0,
	MSD_AVB_BLOCK_AVB_POLICY = 0x1,
	MSD_AVB_BLOCK_QAV = 0x2,
	MSD_AVB_BLOCK_QBV = 0x3
} MSD_AVB_BLOCK;


/*
* Typedef: enum MSD_AVB_OPERATION
*
* Description: Defines the AVB (Audio Video Bridge) Operation type
*
* Fields:
*      AVB_WRITE_DATA           - Write data to the AVBBlock(PTP, AVB Policy, Qav or Qbv) register
*      AVB_READ_DATA            - Read data from AVBBlock(PTP, AVB Policy, Qav or Qbv) register
*      AVB_READ_MULTIPLE_DATA   - Read multiple data from AVBBlock(PTP, AVB Policy, Qav or Qbv) register
*/
typedef enum
{
	AVB_READ_DATA = 0x0,
	AVB_READ_MULTIPLE_DATA = 0x2,
	AVB_WRITE_DATA = 0x3,	
} MSD_AVB_OPERATION;

/*
*  typedef: struct MSD_AVB_OP_DATA
*
*  Description: data required by avb Operation
*
*  Fields:
*      avbPort        - physical port of the device
*      avbBlock    - AVB Block
*      avbAddr     - register address
*      avbData     - data for avb register.
*      avbMultiData- used for multiple read operation.
*      nData         - number of data to be read on multiple read operation.
*/
typedef struct
{
	MSD_U32    avbPort;
	MSD_U32    avbBlock;
	MSD_U32    avbAddr;
	MSD_U32    avbData;
	MSD_U32    avbMultiData[MAX_AVB_CONSECUTIVE_READ];
	MSD_U32    nData;
} MSD_AVB_OP_DATA;


/******************************************************************************
* avbOperationPerform
*
* DESCRIPTION:
*       This function accesses AVB Command Register and Data Register.
*
* INPUTS:
*       avbOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       avbData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
static MSD_STATUS avbOperationPerform
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_AVB_OPERATION      avbOp,
    INOUT MSD_AVB_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32             i;
	MSD_U16          avbOperation;
	MSD_U16 data;

    msdSemTake(dev->devNum,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the avb in ready. */
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
    /* Set the AVB Operation register */
    switch (avbOp)
    {
        case AVB_WRITE_DATA:
		{
            data = (MSD_U16)opData->avbData;
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_AVB_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

			avbOperation = AVB_WRITE_DATA;
			data = (MSD_U16)((1 << 15) | (avbOperation <<= 13) |
                    (opData->avbPort << 8)    |
                    (opData->avbBlock << 5)    |
                    (opData->avbAddr & 0x1F));
            retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR,
                TOPAZ_QD_REG_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
		}
		break;

        case AVB_READ_DATA:
		{
	        avbOperation = AVB_READ_DATA;
			data = (MSD_U16)((1 << 15) | (avbOperation <<= 13) |
                    (opData->avbPort << 8)    |
                    (opData->avbBlock << 5)    |
                    (opData->avbAddr & 0x1F));
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
            opData->avbData = (MSD_U32)data;
		}
        break;

        case AVB_READ_MULTIPLE_DATA:
		{
	        avbOperation = AVB_READ_MULTIPLE_DATA;
			data = (MSD_U16)((1 << 15) | (avbOperation <<= 13) |
                    (opData->avbPort << 8)    |
                    (opData->avbBlock << 5)    |
                    (opData->avbAddr & 0x1F));
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
				opData->avbMultiData[i] = (MSD_U32)data;
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

/******************************************************************************
* Topaz_gqavSetPortQpriXRate
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 rate on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        rate - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Topaz_gqavSetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U8         queue,
    IN  MSD_U32        rate
)
{
    MSD_STATUS           retVal;
    MSD_AVB_OPERATION    op;
    MSD_AVB_OP_DATA      opData;
    MSD_U32              hwPort;

    MSD_DBG_INFO(("Topaz_gqavSetPortQpriXRate Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
	if (queue < 0 || queue >= TOPAZ_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue).\n"));
        return MSD_BAD_PARAM;
    }
    if (rate % 64 != 0)
    {
        MSD_DBG_ERROR(("Failed (Bad rate).\n"));
        return MSD_BAD_PARAM;
    }

    opData.avbBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */
    opData.avbAddr = queue * 2;

    opData.avbPort = hwPort;

    op = AVB_WRITE_DATA;
    opData.avbData = (rate / 64);

    if ((retVal = avbOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("avbOperationPerform for Topaz_gqavSetPortQpriXRate returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gqavSetPortQpriXRate Exit.\n"));
    return MSD_OK;
}

/******************************************************************************
* Topaz_gqavGetPortQpriXRate
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 rate Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        rate - number of credits in bytes .
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Topaz_gqavGetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U32        *rate
)
{
    MSD_STATUS           retVal;
    MSD_AVB_OPERATION    op;
    MSD_AVB_OP_DATA        opData;
    MSD_U32            hwPort;

    MSD_DBG_INFO(("Topaz_gqavGetPortQpriXRate Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
	if (queue < 0 || queue >= TOPAZ_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue).\n"));
        return MSD_BAD_PARAM;
    }

    opData.avbBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */

    opData.avbAddr = queue * 2;
    opData.avbPort = hwPort;

    op = AVB_READ_DATA;

    if ((retVal = avbOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("avbOperationPerform for Topaz_gqavGetPortQpriXRate returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    *rate = opData.avbData * 64;

    MSD_DBG_INFO(("Topaz_gqavGetPortQpriXRate Exit.\n"));
    return MSD_OK;
}

/******************************************************************************
* Topaz_gqavSetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 HiLimit on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Topaz_gqavSetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    IN  MSD_U16        hiLimit
)
{
    MSD_STATUS           retVal;
    MSD_AVB_OPERATION    op;
    MSD_AVB_OP_DATA        opData;
    MSD_U32            hwPort;

    MSD_DBG_INFO(("Topaz_gqavSetPortQpriXHiLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
	if (queue < 0 || queue >= TOPAZ_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue).\n"));
        return MSD_BAD_PARAM;
    }
    if (hiLimit > 0x3fff)
    {
        MSD_DBG_ERROR(("Failed (Bad hiLimit).\n"));
        return MSD_BAD_PARAM;
    }

    opData.avbBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */
    opData.avbAddr = queue * 2 + 1;

    opData.avbPort = hwPort;

    op = AVB_WRITE_DATA;
    opData.avbData = hiLimit & 0x3fff;

    if ((retVal = avbOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("avbOperationPerform for Topaz_gqavSetPortQpriXHiLimit returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gqavSetPortQpriXHiLimit Exit.\n"));
    return MSD_OK;
}
/******************************************************************************
* Topaz_gqavGetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 HiLimit Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        hiLimit - number of credits in bytes .
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Topaz_gqavGetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U16        *hiLimit
)
{
    MSD_STATUS           retVal;
    MSD_AVB_OPERATION    op;
    MSD_AVB_OP_DATA        opData;
    MSD_U32            hwPort;

    MSD_DBG_INFO(("Topaz_gqavGetPortQpriXHiLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
	if (queue < 0 || queue >= TOPAZ_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue).\n"));
        return MSD_BAD_PARAM;
    }

    opData.avbBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */

    opData.avbAddr = queue * 2 + 1;
    opData.avbPort = hwPort;

    op = AVB_READ_DATA;

    if ((retVal = avbOperationPerform(dev, op, &opData)) != MSD_OK)
    {
        MSD_DBG_ERROR(("avbOperationPerform for Topaz_gqavGetPortQpriXHiLimit returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    *hiLimit = (MSD_U16)opData.avbData & 0x3fff;

    MSD_DBG_INFO(("Topaz_gqavGetPortQpriXHiLimit Exit.\n"));
    return MSD_OK;
}
