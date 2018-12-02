#include <msdCopyright.h>

/********************************************************************************
* Peridot_msdSysCtrl.c
*
* DESCRIPTION:
*       API definitions for system global control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdSysCtrl.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>
/*******************************************************************************
* Peridot_gsysSetTrunkMaskTable
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       maskNum - Mask Number (0~7).
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
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
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetTrunkMaskTable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       maskNum,
    IN MSD_U32       trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gsysSetTrunkMaskTable Called.\n"));

	if (maskNum >= PERIDOT_MAX_LAG_MASK_NUM)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %d]\n", (unsigned int)maskNum, (PERIDOT_MAX_LAG_MASK_NUM - 1)));
        return MSD_BAD_PARAM;
    }

    mask = (1 << dev->numOfPorts) - 1;
    if(trunkMask > mask)
    {
        MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkMask: 0x%x). It should be within [0, 0x%x]\n", trunkMask, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_MASK_TBL, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Set related register */
	data &= 0x0800; /* maintain existing HasHash bit.*/
	mask = MSD_LPORTVEC_2_PORTVEC(trunkMask);
    data = (MSD_U16)(0x8000 | data |  (maskNum << 12) | mask);   
	retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gsysSetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysGetTrunkMaskTable
*
* DESCRIPTION:
*       This function Peridot_gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if maskNum > 0x7)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetTrunkMaskTable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       maskNum,
    OUT MSD_U32       *trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gsysGetTrunkMaskTable Called.\n"));

	if (maskNum >= PERIDOT_MAX_LAG_MASK_NUM)
	{
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %d]\n", (unsigned int)maskNum, (PERIDOT_MAX_LAG_MASK_NUM - 1)));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,&data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Set related register */
    data &= 0x0800;
    data = (MSD_U16)(data | (maskNum << 12));
	retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* read-back value */
	retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,&data);
    msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
    }

    mask = (1 << dev->maxPorts) - 1;
    *trunkMask = MSD_PORTVEC_2_LPORTVEC(data & mask);
            
    MSD_DBG_INFO(("Peridot_gsysGetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toPeridot_gether to 
*       select the TrunkMask to use.
*
* INPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
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
MSD_STATUS Peridot_gsysSetHashTrunk
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16            data;

    MSD_DBG_INFO(("Peridot_gsysSetHashTrunk Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,11,1,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gsysSetHashTrunk Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysGetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toPeridot_gether to 
*       select the TrunkMask to use.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetHashTrunk
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_DBG_INFO(("Peridot_gsysGetHashTrunk Called.\n"));

    /* Get related bit */
    retVal = msdGetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,PERIDOT_QD_REG_TRUNK_MASK_TBL,11,1,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Peridot_gsysGetHashTrunk Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetTrunkRouting
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetTrunkRouting
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       trunkId,
    IN MSD_U32       trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;

    int count=0x10;

    MSD_DBG_INFO(("Peridot_gsysSetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0x1f] */
	if (!PERIDOT_IS_TRUNK_ID_VALID(dev, trunkId))
    {
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", (unsigned int)trunkId));
        return MSD_BAD_PARAM;
    }

	/* Check if trunkRoute is our of range [0,0x7ff] */
    mask = (1 << dev->numOfPorts) - 1;
    if(trunkRoute > mask)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkRoute: 0x%x). It should be within [0, 0x%x]\n", trunkRoute, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)((trunkId&0x10)>>4);
	retVal = msdSetAnyRegField(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_MISC, 13, 1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	mask = MSD_LPORTVEC_2_PORTVEC(trunkRoute);
	data = (MSD_U16)(0x8000 | ((trunkId&0xf) << 11) | mask);
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_ROUTING, data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    MSD_DBG_INFO(("Peridot_gsysSetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysGetTrunkRouting
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetTrunkRouting
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       trunkId,
    OUT MSD_U32       *trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gsysGetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0x1f] */
	if (!PERIDOT_IS_TRUNK_ID_VALID(dev, trunkId))
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", (unsigned int)trunkId));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);


	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)((trunkId&0x10)>>4);
	retVal = msdSetAnyRegField(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_MISC, 13, 1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	data = (MSD_U16)((trunkId&0xf) << 11);
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_ROUTING, data);
    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* Read-back value */
	retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_TRUNK_ROUTING, &data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    mask = (1 << dev->maxPorts) - 1;
    *trunkRoute = MSD_PORTVEC_2_LPORTVEC(data & mask);

    MSD_DBG_INFO(("Peridot_gsysGetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       en - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
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
MSD_STATUS Peridot_gsysSetFloodBC
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16            data;

    MSD_DBG_INFO(("Peridot_gsysSetFloodBC Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_MANAGEMENT, 12, 1, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gsysSetFloodBC Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysGetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetFloodBC
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_DBG_INFO(("Peridot_gsysGetFloodBC Called.\n"));

    /* Get related bit */
	retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_MANAGEMENT, 12, 1, &data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);
    MSD_DBG_INFO(("Peridot_gsysGetFloodBC Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetMonitorMgmtCtrl
*
* DESCRIPTION:
*       Set Monitor and mgmt control data. 
*         
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*       data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
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
MSD_STATUS Peridot_gsysSetMonitorMgmtCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8             point,
    IN  MSD_U8            data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U16            tmpData;
    int count=0x10;
    MSD_DBG_INFO(("Peridot_gsysSetMonitorMgmtCtrl Called.\n"));

    if (point >= 64)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM (point: %u). It should be within [0, 63].\n", point));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

	/*Wait busy bit*/
	tmpData = 1;
    while(tmpData ==1)
    {
		retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_MONITOR_MGMT_CTRL, 15, 1, &tmpData);
		if(retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Read G1 monitor&mgmt register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G1 monitor&mgmt register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    }

	tmpData =  (MSD_U16)((1 << 15) | (point << 8) | data);
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_MONITOR_MGMT_CTRL, tmpData);

	msdSemGive(dev->devNum, dev->tblRegsSem);

	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg for G1 monitor&mgmt register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    MSD_DBG_INFO(("Peridot_gsysSetMonitorMgmtCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Peridot_gsysGetMonitorMgmtCtrl
*
* DESCRIPTION:
*       Get Monitor and mgmt control data. 
*         
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*
* OUTPUTS:
*       data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
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
MSD_STATUS Peridot_gsysGetMonitorMgmtCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U16	tmpData;
	int count=0x10;
    MSD_DBG_INFO(("Peridot_gsysGetMonitorMgmtCtrl Called.\n"));

	if (point >= 64)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (point: %u). It should be within [0, 63].\n", point));
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

	do {
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_MONITOR_MGMT_CTRL, &tmpData);
        if(retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Read G1 monitor&mgmt register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G1 monitor&mgmt register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (tmpData&0x8000);

	tmpData =  (MSD_U16)((point << 8) | 0);
	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_MONITOR_MGMT_CTRL, tmpData);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg for G1 monitor&mgmt register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_MONITOR_MGMT_CTRL, &tmpData);
    if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg for G1 monitor&mgmt register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }
	*data = (MSD_U8)(tmpData&0xff);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    MSD_DBG_INFO(("Peridot_gsysGetMonitorMgmtCtrl Exit.\n"));
    return retVal;
}


/*******************************************************************************
* Peridot_gsysSetCPUDest
*
* DESCRIPTION:
*       This routine sets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria 
*       are mapped to the CPU Destination port, overriding where the frame would 
*       normally go. In the case of ARP, the frame will be mapped normally and it 
*       will also Peridot_get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination 
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring 
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       port  - the logical port number.
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
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetCPUDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			data;

    MSD_DBG_INFO(("Peridot_gsysSetCPUDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    /* translate LPORT to hardware port */
    if(port == 0x1F || port == 0x1E)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }

	retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x30,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	data = ((data & 0xE0) | hwPort);
	retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x30, (MSD_U8)data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	
    MSD_DBG_INFO(("Peridot_gsysSetCPUDest Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Peridot_gsysGetCPUDest
*
* DESCRIPTION:
*       This routine Peridot_gets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria 
*       are mapped to the CPU Destination port, overriding where the frame would 
*       normally go. In the case of ARP, the frame will be mapped normally and it 
*       will also Peridot_get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination 
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring 
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetCPUDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8 datac;

    MSD_DBG_INFO(("Peridot_gsysGetCPUDest Called.\n"));

    retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x30, &datac);
    if(retVal != MSD_OK)
	{
	    MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	    return retVal;
	}
	data = (datac&0x1F);

    if(data == 0x1F || data == 0x1E)
    {
        *port = (MSD_LPORT)data;
    }
    else
    {
        *port = MSD_PORT_2_LPORT((MSD_U8)data);
    }

    MSD_DBG_INFO(("Peridot_gsysGetCPUDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetIngressMonitorDest
*
* DESCRIPTION:
*       This routine sets Ingress Monitor Destination Port. Frames that are 
*       tarPeridot_geted toward an Ingress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Ingress Monitor type, and frames received on a Network port that 
*       is enabled to be the Ingress Monitor Source Port.
*       If the Ingress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Ingress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to Peridot_get 
*       to the device that contains the Ingress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.when port = 0x1F,IMD is disabled.
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
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetIngressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Peridot_gsysSetIngressMonitorDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    /* translate LPORT to hardware port */
    if(port == 0x1F || port == 0x1E)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }

	retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x20, (MSD_U8)hwPort);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	
    MSD_DBG_INFO(("Peridot_gsysSetIngressMonitorDest Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Peridot_gsysGetIngressMonitorDest
*
* DESCRIPTION:
*       This routine Peridot_gets Ingress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetIngressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8 datac;

    MSD_DBG_INFO(("Peridot_gsysGetIngressMonitorDest Called.\n"));

    retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x20, &datac);
    if(retVal != MSD_OK)
	{
        MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	    return retVal;
	}
	data = datac;

    if(data == 0x1F || data == 0x1E)
    {
        *port = (MSD_LPORT)data;
    }
    else
    {
        *port = MSD_PORT_2_LPORT((MSD_U8)data);
    }

    MSD_DBG_INFO(("Peridot_gsysGetIngressMonitorDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetEgressMonitorDest
*
* DESCRIPTION:
*       This routine sets Egress Monitor Destination Port. Frames that are 
*       tarPeridot_geted toward an Egress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Egress Monitor type, and frames transmitted on a Network port that 
*       is enabled to be the Egress Monitor Source Port.
*       If the Egress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Egress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to Peridot_get 
*       to the device that contains the Egress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.If port = 0x1F, EMD is disabled
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
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetEgressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Peridot_gsysSetEgressMonitorDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    if(port == 0x1F || port == 0x1E)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }

    retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x21, (MSD_U8)hwPort);
    if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	    return retVal;
	}
	
    MSD_DBG_INFO(("Peridot_gsysSetEgressMonitorDest Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gsysGetEgressMonitorDest
*
* DESCRIPTION:
*       This routine Peridot_gets Egress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetEgressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
	MSD_U8 datac;

    MSD_DBG_INFO(("Peridot_gsysGetEgressMonitorDest Called.\n"));
		
    retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x21, &datac);
    if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	  return retVal;
	}
	data = datac;


    if(data == 0x1F || data == 0x1E)
    {
        *port = (MSD_LPORT)data;
    }
    else
    {
        *port = MSD_PORT_2_LPORT((MSD_U8)data);
    }
    MSD_DBG_INFO(("Peridot_gsysGetEgressMonitorDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysSetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Peridot_gsysSetRsvd2Cpu) is set to a one, 
*       the 16 reserved multicast DA addresses, whose bit in this register are 
*       also set to a one, are treadted as MGMT frames. All the reserved DA's 
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is 
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will 
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       enBits - bit vector of enabled Reserved Multicast.
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
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetRsvd2CpuEnables
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U16        enBits
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */

    MSD_DBG_INFO(("Peridot_gsysSetRsvd2CpuEnables Called.\n"));

	retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x0, (MSD_U8)(enBits&0xff));
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl for point 0 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x1, (MSD_U8)((enBits>>8)&0xff));
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl for point 1 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	    
	MSD_DBG_INFO(("Peridot_gsysSetRsvd2CpuEnables Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gsysGetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Peridot_gsysSetRsvd2Cpu) is set to a one, 
*       the 16 reserved multicast DA addresses, whose bit in this register are 
*       also set to a one, are treadted as MGMT frames. All the reserved DA's 
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is 
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will 
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       enBits - bit vector of enabled Reserved Multicast.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetRsvd2CpuEnables
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_U16      *enBits
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8 data;
	MSD_U16 dataw;

    MSD_DBG_INFO(("Peridot_gsysGetRsvd2CpuEnables Called.\n"));

	retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x0, &data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl for point 0 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	*enBits = data;
	retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x1, &data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl for point 1 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	dataw=data;
	dataw <<=8;
	*enBits |= (dataw&0xff00);

	MSD_DBG_INFO(("Peridot_gsysGetRsvd2CpuEnables Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Peridot_gsysSetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetMGMTPri
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U16        pri
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

    MSD_DBG_INFO(("Peridot_gsysSetMGMTPri Called.\n"));

    if (pri > 0x7)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM (pri: %u). It should be within [0, 7].\n", (unsigned int)pri));
        return MSD_BAD_PARAM;
    }

	retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x30,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Set related bit */
	data = (MSD_U8)((data & 0x1F) | ((pri & 0x7)<<5));
    retVal = Peridot_gsysSetMonitorMgmtCtrl(dev,0x30,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gsysSetMGMTPri Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gsysGetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetMGMTPri
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_U16      *pri
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

    MSD_DBG_INFO(("Peridot_gsysGetMGMTPri Called.\n"));

    /* Get related bit */
	retVal = Peridot_gsysGetMonitorMgmtCtrl(dev,0x30,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	*pri = ((data & 0xE0) >> 5);

    MSD_DBG_INFO(("Peridot_gsysGetMGMTPri Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Peridot_gsysGlobal1RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8 i = 0;
	char desc[32][48] = 
	{
		{ "Switch Global Status(0x0)" },
		{ "ATU FID Register(0x1)" },
		{ "VTU FID Register(0x2)" },
		{ "VTU SID Register(0x3)" },
		{ "Switch Global Control Register(0x4)" },
		{ "VTU Operation Register(0x5)" },
		{ "VTU VID Register(0x6)" },
		{ "VTU/STU Data Register Ports 0 to 7(0x7)" },
		{ "VTU/STU Data Register Ports 8 to 10(0x8)" },
		{ "Reserved(0x9)" },
		{ "ATU Control Register(0xa)" },
		{ "ATU Operation Register(0xb)" },
		{ "ATU Data Register(0xc)" },
		{ "Switch MAC Address Register Bytes 0 & 1(0xd)" },
		{ "Switch MAC Address Register Bytes 2 & 3(0xe)" },
		{ "Switch MAC Address Register Bytes 4 & 5(0xf)" },
		{ "Reserved(0x10)" },
		{ "Reserved(0x11)" },
		{ "Reserved(0x12)" },
		{ "Reserved(0x13)" },
		{ "Reserved(0x14)" },
		{ "Reserved(0x15)" },
		{ "Reserved(0x16)" },
		{ "Reserved(0x17)" },
		{ "Reserved(0x18)" },
		{ "Reserved(0x19)" },
		{ "Monitor & MGMT Control Register(0x1a)" },
		{ "Total Free Counter(0x1b)" },
		{ "Global Control 2(0x1c)" },
		{ "Stats Operation Register(0x1d)" },
		{ "Stats Counter Register Byte 3 & 2(0x1e)" },
		{ "Stats Counter Register Byte 1 & 0(0x1f)" }
	};

	MSD_DBG_INFO(("Peridot_gsysGlobal1RegDump Called.\n"));

	MSG(("\n------------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL1_DEV_ADDR, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-48s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Peridot_gsysGlobal1RegDump Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Peridot_gsysGlobal2RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8 i = 0;
	char desc[32][48] =
	{
		{ "Interrupt Source Register(0x0)" },
		{ "Interupt Mask Register(0x1)" },
		{ "Reserved(0x2)" },
		{ "Reserved(0x3)" },
		{ "Flow Control Delay Register(0x4)" },
		{ "Switch Management Register(0x5)" },
		{ "Device Mapping Table Register(0x6)" },
		{ "LAG Mask Table Register(0x7)" },
		{ "LAG Mapping Register(0x8)" },
		{ "Ingress Rate Command Register(0x9)" },
		{ "Ingress Rate Data Register(0xa)" },
		{ "Cross Chip Port VLAN Addr Register(0xb)" },
		{ "Cross Chip Port VLAN Data Register(0xc)" },
		{ "Switch MAC/WoL/WoF register(0xd)" },
		{ "ATU Stats(0xe)" },
		{ "Priority Override Table(0xf)" },
		{ "Reserved(0x10)" },
		{ "Reserved(0x11)" },
		{ "Energy Management Register(0x12)" },
		{ "IMP Communication/Debug Register(0x13)" },
		{ "EEPROM Command(0x14)" },
		{ "EEPROM Addr(0x15)" },
		{ "AVB/TSN Command Register(0x16)" },
		{ "AVB/TSN Data Register(0x17)" },
		{ "SMI PHY Command for SMI Access(0x18)" },
		{ "SMI PHY Data Register(0x19)" },
		{ "Scratch and Misc(0x1a)" },
		{ "Watch Dog Control Register(0x1b)" },
		{ "QoS Weights Register(0x1c)" },
		{ "Misc Register(0x1d)" },
		{ "Reserved(0x1e)" },
		{ "Cut Through Control Register(0x1f)" }
	};

	MSD_DBG_INFO(("Peridot_gsysGlobal2RegDump Called.\n"));

	MSG(("\n------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-42s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Peridot_gsysGlobal2RegDump Exit.\n"));
	return MSD_OK;
}
