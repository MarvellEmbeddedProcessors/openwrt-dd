#include <msdCopyright.h>
/********************************************************************************
* msdHwAccess.c
*
* DESCRIPTION:
*       Functions definition for HW accessing PHY, Serdes and Switch registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <msdHwAccess.h>
#include <msdSem.h>
#include <msdUtils.h>


/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                         */
/****************************************************************************/

/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdSetSMIC45PhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
)
{
    unsigned int timeOut; 
    MSD_U16 smiReg;

    MSD_DBG(("(LOG AGATE SMI PHY C45 RW): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, ",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_ERROR(("read SMI Phy AGATE_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);


    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write AGATE_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
    smiReg = MSD_SMI_BUSY | (phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) | 
            (devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_INFO(("read SMI Phy AGATE_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);


    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
    smiReg = MSD_SMI_BUSY | (phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_WRITE << MSD_SMI_OP_BIT) | 
            (devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

	return MSD_OK;
}

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdGetSMIC45PhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
)
{
    unsigned int timeOut;
    MSD_U16 smiReg;
	MSD_U16 tmp;

    MSD_DBG_INFO(("Read Phy register while PPU Enabled\n"));

     /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_INFO(("read AGATE_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);


    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
    smiReg = MSD_SMI_BUSY | (phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) | 
            (devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    smiReg = MSD_SMI_BUSY | (phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_READ_45 << MSD_SMI_OP_BIT) | 
            (devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write TOPAZ_QD_REG_SMI_PHY_CMD register Failed\n"));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);


    if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, &tmp) != MSD_OK)
    {
        MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	
    *data = tmp;

    MSD_DBG(("(LOG AGATE SMI PHY C45 RR): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, ",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}

/*****************************************************************************
* msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdSetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN MSD_U16   data
)
{
    unsigned int timeOut;
    MSD_U16 smiReg;

    MSD_DBG(("(LOG AGATE SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, ",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_INFO(("read AGATE_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);


    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
    smiReg = MSD_SMI_BUSY | (devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_WRITE << MSD_SMI_OP_BIT) | 
            (regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    return MSD_OK;
}

/*****************************************************************************
* msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdGetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT  MSD_U16   *data
)
{
    unsigned int timeOut; 
    MSD_U16 smiReg;

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_ERROR(("Read AGATE_QD_REG_SMI_PHY_CMD Timed Out\n"));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);

    smiReg =  MSD_SMI_BUSY | (devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_SMI_READ_22 << MSD_SMI_OP_BIT) | 
            (regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

    if(msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
        if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read AGATE_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut-- < 1 ) 
        {
            MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
    } while (smiReg & MSD_SMI_BUSY);

    if(msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_SMI_PHY_DATA, &smiReg) != MSD_OK)
    {
        MSD_DBG_INFO(("Reading Phy register Failed\n"));
        return MSD_FAIL;
    }
    *data = smiReg;

    MSD_DBG(("(LOG AGATE SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, ",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}

/*****************************************************************************
* msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdSetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN MSD_U16    data
)
{
    MSD_U16 mask;
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal =  Agate_msdGetSMIPhyReg(dev,devAddr,regAddr,&tmpData);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    MSD_CALC_MASK(fieldOffset,fieldLength,mask);

    /* Set the desired bits to 0.                       */
    tmpData &= ~mask;
    /* Set the given data into the above reset bits.    */
    tmpData |= ((data << fieldOffset) & mask);
    MSD_DBG_INFO(("Write to devAddr(%d): regAddr 0x%x, ",
              devAddr,regAddr));
    MSD_DBG_INFO(("fieldOff %d, fieldLen %d, data 0x%x.\n",fieldOffset,
              fieldLength,data));

    retVal = Agate_msdSetSMIPhyReg(dev,devAddr,regAddr,tmpData);

    return retVal;
}

/*****************************************************************************
* msdGetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_msdGetSMIPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT  MSD_U16   *data
)
{
    MSD_U16 mask;            /* Bits mask to be read */
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal =  Agate_msdGetSMIPhyReg(dev,devAddr,regAddr,&tmpData);
    if (retVal != MSD_OK)
        return retVal;
        
    MSD_CALC_MASK(fieldOffset,fieldLength,mask);

    tmpData = (tmpData & mask) >> fieldOffset;
    *data = tmpData;
    MSD_DBG_INFO(("Read from deviceAddr(%d): regAddr 0x%x, ",
              devAddr,regAddr));
    MSD_DBG_INFO(("fOff %d, fLen %d, data 0x%x.\n",fieldOffset,fieldLength,*data));

    return MSD_OK;
}

MSD_STATUS Agate_msdSetPagedPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
)	
{
	MSD_STATUS retVal;
	retVal = Agate_msdSetSMIPhyReg(dev, portNum, 22, pageNum);
	if(retVal != MSD_OK)
		return retVal;
	retVal = Agate_msdSetSMIPhyRegField(dev, portNum, regAddr, fieldOffset, fieldLength, data);
	return retVal;
}	

MSD_STATUS Agate_msdGetPagedPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   *data
)	
{
	MSD_STATUS retVal;
	retVal = Agate_msdSetSMIPhyReg(dev, portNum, 22, pageNum);
	if(retVal != MSD_OK)
		return retVal;
	retVal = Agate_msdGetSMIPhyRegField(dev, portNum, regAddr, fieldOffset, fieldLength, data);
	return retVal;
}

MSD_STATUS Agate_msdWritePagedPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U16   data
)
{
	
	MSD_STATUS retVal;
	retVal = Agate_msdSetSMIPhyReg(dev, portNum, 22, pageNum);
	if(retVal != MSD_OK)
		return retVal;
	retVal = Agate_msdSetSMIPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Agate_msdReadPagedPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    OUT MSD_U16   *data
)
{
	MSD_STATUS retVal;
	retVal = Agate_msdSetSMIPhyReg(dev, portNum, 22, pageNum);
	if(retVal != MSD_OK)
		return retVal;
	retVal = Agate_msdGetSMIPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Agate_msdSetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    IN MSD_U16 data
)
{
    MSD_STATUS retVal;
    MSD_U16 tmpData;
    MSD_U8 hwPort;

    hwPort = MSD_LPORT_2_PHY(portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port: %d.\n", portNum));
        return MSD_BAD_PARAM;
    }

    /* Set MMD access control address is Address | devAddr */
    tmpData = MSD_PHY_MMD_ADDR | devAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, hwPort, 0, 13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
        return retVal;
    }

    /* Set MMD access Data is regAddr */
    tmpData = regAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, hwPort, 0, 14, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
        return retVal;
    }

    /* Set MMD access control address is Data no inc | devAddr */
    tmpData = MSD_PHY_MMD_DATA_NO_INC | devAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, hwPort, 0, 13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
        return retVal;
    }

    /* Write MMD access Data */
    retVal = Agate_msdWritePagedPhyReg(dev, hwPort, 0, 14, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Write MMD access Data.\n"));
        return retVal;
    }

    return MSD_OK;
}

MSD_STATUS Agate_msdGetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    OUT MSD_U16 *data
)
{
    MSD_STATUS retVal;
    MSD_U16 tmpData;

    /* Set MMD access control address is Address | devAddr */
    tmpData = MSD_PHY_MMD_ADDR | devAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, portNum, 0, 13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
        return retVal;
    }

    /* Set MMD access Data is regAddr */
    tmpData = regAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, portNum, 0, 14, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
        return retVal;
    }

    /* Set MMD access control address is Data no inc | devAddr */
    tmpData = MSD_PHY_MMD_DATA_NO_INC | devAddr;
    retVal = Agate_msdWritePagedPhyReg(dev, portNum, 0, 13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
        return retVal;
    }

    /* Read MMD access Data */
    retVal = Agate_msdReadPagedPhyReg(dev, portNum, 0, 14, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Read MMD access Data.\n"));
        return retVal;
    }

    *data = tmpData;

    return MSD_OK;
}

/*******************************************************************************
* msdPhyReset
*
* DESCRIPTION:
*       This function performs softreset and waits until reset completion.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       u16Data     - data should be written into Phy control register.
*                      if this value is 0xFF, normal operation occcurs (read, 
*                      update, and write back.)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_msdPhyReset
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U8        portNum,
	IN  MSD_U8		 pageNum,
    IN  MSD_U16       u16Data
)
{
    MSD_U16 tmpData;
    MSD_STATUS   retVal;
    MSD_U32 retryCount;
    MSD_BOOL    pd = MSD_FALSE;

    /*if((retVal=msdGetSMIPhyReg(dev,portNum,0,&tmpData)) 
           != MSD_OK)*/
	if((retVal=Agate_msdReadPagedPhyReg(dev, portNum, pageNum, 0,&tmpData)) 
           != MSD_OK)
    {
		MSD_DBG_INFO(("Reading Register failed\n"));
		return retVal;
    }

    if (tmpData & 0x800)
    {
        pd = MSD_TRUE;
    }

    if (u16Data != 0xFF)
    {
        tmpData = u16Data;
    }

    /* Set the desired bits to 0. */
    if (pd)
    {
        tmpData |= 0x800;
    }
    else
    {
	  if(((tmpData&0x4000)==0)||(u16Data==0xFF)) /* setting loopback do not set reset */
        tmpData |= 0x8000;
    }

    /*if((retVal=msdSetSMIPhyReg(dev,portNum,0,tmpData)) 
        != MSD_OK)*/
	if((retVal=Agate_msdWritePagedPhyReg(dev, portNum, pageNum, 0,tmpData)) 
        != MSD_OK)
    {
        MSD_DBG_INFO(("Writing to register failed\n"));
        return retVal;
    }

    if (pd)
    {
        return MSD_OK;
    }

    for (retryCount = 0x1000; retryCount > 0; retryCount--)
    {
        /*if((retVal=msdGetSMIPhyReg(dev,portNum,0,&tmpData)) != MSD_OK)*/
		if((retVal=Agate_msdReadPagedPhyReg(dev, portNum, pageNum, 0,&tmpData)) != MSD_OK)
        {
            MSD_DBG_INFO(("Reading register failed\n"));
            return retVal;
        }
        if ((tmpData & 0x8000) == 0)
            break;
    }

    if (retryCount == 0)
    {
        MSD_DBG_INFO(("Reset bit is not cleared\n"));
        return MSD_FAIL;
    }

    return MSD_OK;
}
