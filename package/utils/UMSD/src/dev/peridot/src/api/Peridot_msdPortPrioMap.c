#include <msdCopyright.h>

/********************************************************************************
* Peridot_msdPortPrioMap.c
*
* DESCRIPTION:
*       API definitions for Port Priority Mapping Rules control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdPortPrioMap.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


/*******************************************************************************
* Peridot_gqosSetPrioMapRule
*
* DESCRIPTION:
*       This routine sets priority mapping rule(TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6, 
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are 
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gqosSetPrioMapRule
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   port,
    IN  MSD_BOOL    mode
)
{
    MSD_U16          data;           /* temporary data buffer */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Peridot_gqosSetPrioMapRule Called.\n"));
    /* translate bool to binary */
	MSD_BOOL_2_BIT(mode, data);
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
   
    /* Set the TagIfBoth.  */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL, 6, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gqosSetPrioMapRule Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine Peridot_gets priority mapping rule (TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6, 
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are 
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gqosGetPrioMapRule
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_BOOL   *mode
)
{
    MSD_U16          data;           /* Used to poll the SWReset bit */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Peridot_gqosGetPrioMapRule Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    
    /* Peridot_get the TagIfBoth.  */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL, 6, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read PERIDOT_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* translate bool to binary */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Peridot_gqosGetPrioMapRule Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            PERIDOT_MSD_INITIAL_PRI_DEFAULT
*               
*            PERIDOT_MSD_INITIAL_PRI_TAG
*                
*            PERIDOT_MSD_INITIAL_PRI_IP
*                
*            PERIDOT_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - PERIDOT_MSD_INITIAL_PRI type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  PERIDOT_MSD_INITIAL_PRI  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16            data;

    MSD_DBG_INFO(("Peridot_gprtSetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    if (mode < PERIDOT_MSD_INITIAL_PRI_DEFAULT || mode > PERIDOT_MSD_INITIAL_PRI_TAG_IP)
    {
        MSD_DBG_ERROR(("Bad mode: %u.\n", (unsigned int)mode));
        return MSD_BAD_PARAM;
    }

    switch(mode)
    {
        case PERIDOT_MSD_INITIAL_PRI_DEFAULT:
            data = 0;
            break;
		case PERIDOT_MSD_INITIAL_PRI_TAG:
			data = 1;
			break;
		case PERIDOT_MSD_INITIAL_PRI_IP:
			data = 2;
			break;
		case PERIDOT_MSD_INITIAL_PRI_TAG_IP:
			data = 3;
			break;
        default:
            {
                MSD_DBG_ERROR(("Failed Bad Mode: %u.\n", (unsigned int)mode));
                return MSD_BAD_PARAM;
            }
    }

    /* Set the PERIDOT_MSD_INITIAL_PRI mode.            */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL, 4, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtSetInitialPri Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine Peridot_gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            PERIDOT_MSD_INITIAL_PRI_DEFAULT
*               
*            PERIDOT_MSD_INITIAL_PRI_TAG
*                
*            PERIDOT_MSD_INITIAL_PRI_IP
*                
*            PERIDOT_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - PERIDOT_MSD_INITIAL_PRI type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT PERIDOT_MSD_INITIAL_PRI  *mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Peridot_gprtGetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    /* Peridot_get the PERIDOT_MSD_INITIAL_PRI type            */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL, 4, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("read PERIDOT_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    switch(data)
    {
        case 0x0:
            *mode = PERIDOT_MSD_INITIAL_PRI_DEFAULT;
            break;
		case 0x1:
			*mode = PERIDOT_MSD_INITIAL_PRI_TAG;
			break;
		case 0x2:
			*mode = PERIDOT_MSD_INITIAL_PRI_IP;
			break;
		case 0x3:
			*mode = PERIDOT_MSD_INITIAL_PRI_TAG_IP;
			break;
        default:
            {
                MSD_DBG_ERROR(("Failed (get the PERIDOT_MSD_INITIAL_PRI type fail).\n"));
                return MSD_FAIL;
            }
    }

    MSD_DBG_INFO(("Peridot_gprtGetInitialPri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_gprtSetDefQpri
*
* DESCRIPTION:
*       This routine Set Default queue priority.
*
* INPUTS:
*       port - the logical port number
*       defque - default queue priority (0~7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetDefQpri
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	MSD_U16		  data;

	MSD_DBG_INFO(("Peridot_gprtSetDefQpri Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    if (defque < 0 || defque>7)
    {
        MSD_DBG_ERROR(("Bad defque: %u.\n", (unsigned int)defque));
        return MSD_BAD_PARAM;
    }

	data = (defque&0x7);

    /* Set the default queue priority.               */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL2, 0, 3, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtSetDefQpri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_gprtGetDefQpri
*
* DESCRIPTION:
*       This routine Get Default queue priority.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       defque - default queue priority structure
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetDefQpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16 data;

    MSD_DBG_INFO(("Peridot_gprtGetDefQpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    /* Set the default queue priority.               */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_PORT_CONTROL2, 0, 3, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("read PERIDOT_QD_REG_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
	*defque = msdShort2Char(data&0x7);

    MSD_DBG_INFO(("Peridot_gprtGetDefQpri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_gprtSetIEEEPrioMap
*
* DESCRIPTION:
*       This API set port IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr		- Pointer to the desired entry of the IEEE Priority  
*						  Mapping table selected above
*       entryStruct	- Mapping data structure for the above selected entry.
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*							PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetIEEEPrioMap
(
    IN  MSD_QD_DEV	*dev,
    IN  MSD_LPORT	port,
    IN  PERIDOT_MSD_IEEE_PRIORITY_TABLE_SET		table,
    IN  MSD_U8							entryPtr,
    IN  PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT   *entryStruct
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data, data1;
    MSD_U16        ptr;
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gprtSetIEEEPrioMap Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    if (table < PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI || table > PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP)
    {
        MSD_DBG_ERROR(("Bad table: %u.\n", (unsigned int)table));
        return MSD_BAD_PARAM;
    }

	ptr = (MSD_U16)(((entryPtr&0x7)<<9) | ((table&0x7)<<12)); 
	data = 0;

	switch(table)
	{
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI:
			MSD_BF_SET(data,entryStruct->ingTbl.ieeeYellow,8,1);
			MSD_BF_SET(data,entryStruct->ingTbl.disIeeeQpri,7,1);
			MSD_BF_SET(data,entryStruct->ingTbl.ieeeQpri,4,3);
			MSD_BF_SET(data,entryStruct->ingTbl.disIeeeFpri,3,1);
			MSD_BF_SET(data,entryStruct->ingTbl.ieeeFpri,0,3);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.disVIDMarking,4,1);
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.disPCPMarking,3,1);
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.framePCP,0,3);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egYelTblPcp.disVIDMarking,4,1);
			MSD_BF_SET(data,entryStruct->egYelTblPcp.disPCPMarking,3,1);
			MSD_BF_SET(data,entryStruct->egYelTblPcp.framePCP,0,3);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.disVIDMarking,4,1);
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.disPCPMarking,3,1);
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.framePCP,0,3);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egGreeTblDscp.enDSCPMarking,6,1);
			MSD_BF_SET(data,entryStruct->egGreeTblDscp.frameDSCP,0,6);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egYelTblDscp.enDSCPMarking,6,1);
			MSD_BF_SET(data,entryStruct->egYelTblDscp.frameDSCP,0,6);
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egAvbTbldscp.enDSCPMarking,6,1);
			MSD_BF_SET(data,entryStruct->egAvbTbldscp.frameDSCP,0,6);
			break;
        default:
            {
                MSD_DBG_ERROR(("Bad table: %u.\n", (unsigned int)table));
                return MSD_BAD_PARAM;
            }
	}

	data |= (ptr);

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
    do    
    {
        retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL, 15, 1, &data1);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            MSD_DBG_ERROR(("read PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
		if((count--)==0) 
		{
            msdSemGive(dev->devNum, dev->tblRegsSem);
            MSD_DBG_ERROR(("Failed(table always busy).\n"));
			return MSD_FAIL;
		}
    } while(data1 == 1);

    data = data | 0x8000;

    retVal = msdSetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL, data);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->tblRegsSem);

    MSD_DBG_INFO(("Peridot_gprtSetIEEEPrioMap Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Peridot_gprtGetIEEEPrioMap
*
* DESCRIPTION:
*       This API ge tport IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr	- Pointer to the desired entry of the IEEE Priority  
*					  Mapping table selected above
*
* OUTPUTS:
*       entryStruct	- Mapping data structure for the above selected entry.
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*						PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetIEEEPrioMap
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT    port,
    IN  PERIDOT_MSD_IEEE_PRIORITY_TABLE_SET      table,
    IN  MSD_U8							entryPtr,
    OUT PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT    *entryStruct
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data;
    MSD_U16        ptr;
    MSD_U8         hwPort;         /* the physical port number     */
	MSD_U8		  phyAddr;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gprtGetIpPrioMap Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_INFO(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    if (table < PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI || table > PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP)
    {
        MSD_DBG_ERROR(("Bad table: %u.\n", (unsigned int)table));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
    do    
    {
		retVal = msdGetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL, &data);
		if(retVal != MSD_OK)
		{	
            MSD_DBG_ERROR(("read PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if((count--)==0)
		{
            MSD_DBG_ERROR(("Read PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while( (data&0x8000) == 0x8000);

	/* Write table + pointer */
	ptr = (MSD_U16)(((entryPtr&0x7)<<9) | ((table&0x7)<<12)); 
	data = (ptr);
    retVal = msdSetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL, data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->tblRegsSem);
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	/* read-back value */
	retVal = msdGetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL, &data);
	if(retVal != MSD_OK)
	{
        msdSemGive(dev->devNum, dev->tblRegsSem);
        MSD_DBG_ERROR(("read PERIDOT_QD_REG_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    msdSemGive(dev->devNum, dev->tblRegsSem);

	switch(table)
	{
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI:			
			entryStruct->ingTbl.ieeeYellow = (MSD_U8)(MSD_BF_GET(data,8,1));
			entryStruct->ingTbl.disIeeeQpri = (MSD_U8)(MSD_BF_GET(data,7,1));
			entryStruct->ingTbl.ieeeQpri = (MSD_U8)(MSD_BF_GET(data,4,3));
			entryStruct->ingTbl.disIeeeFpri = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->ingTbl.ieeeFpri = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP:
			entryStruct->egGreeTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egGreeTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egGreeTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP:
			entryStruct->egYelTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egYelTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egYelTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP:
			entryStruct->egAvbTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egAvbTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egAvbTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP:
			entryStruct->egGreeTblDscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egGreeTblDscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP:
			entryStruct->egYelTblDscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egYelTblDscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
		case PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP:
			entryStruct->egAvbTbldscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egAvbTbldscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
        default:
            {
                MSD_DBG_ERROR(("Bad table: %u.\n", (unsigned int)table));
                return MSD_BAD_PARAM;
            }
	}

    MSD_DBG_INFO(("Peridot_gprtGetIpPrioMap Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gprtRewriteDscp
*
* DESCRIPTION:
*       Re-write ip dscp at egress based on the classification/trust settings in the switch. When this is
*       disabled, the packet is given qos treatment within the switch but the DSCP is not modified at the
*       egress. The L2 cos value gets modified as per the classification/trust settings in the switch
*       irrespective of this setting.
*
* INPUTS:
*       portNum - the logical port number
*       en      ¨C enable/disable dscp re-write at egress on the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtRewriteDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_BOOL    en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Peridot_gprtRewriteDscp Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, PERIDOT_QD_REG_PORT_CONTROL3, 5, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write Peridot_gprtRewriteDscp Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtRewriteDscp Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtSetCosToQueue
*
* DESCRIPTION:
*       Update the COS to egress queue mapping.
*
* INPUTS:
*       portNum - the logical port number
*       cos     - Trusted or assigned cos for the packet
*       queue   - Physical queue to be mapped for trusted or assigned cos
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cos,
    IN  MSD_U8     queue
)
{
    MSD_STATUS       retVal;
    PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT table;

    MSD_DBG_INFO(("Peridot_gprtSetCosToQueue Called.\n"));

    retVal = Peridot_gprtGetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Peridot_gprtSetCosToQueue.\n"));
        return retVal;
    }

    table.ingTbl.disIeeeQpri = 0;
    table.ingTbl.ieeeQpri = queue;

    retVal = Peridot_gprtSetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to SetIEEEPrioMap in Peridot_gprtSetCosToQueue.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtSetCosToQueue Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtGetCosToQueue
*
* DESCRIPTION:
*       Get the physical queue to be mapped for trusted or assigned cos
* INPUTS:
*       portNum - the logical port number
*       cos     - Trusted or assigned cos for the packet
*
* OUTPUTS:
*       queue   - Physical queue to be mapped for trusted or assigned cos
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cos,
    OUT MSD_U8     *queue
)
{
    MSD_STATUS       retVal;
    PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT table;

    MSD_DBG_INFO(("Peridot_gprtGetCosToQueue Called.\n"));

    retVal = Peridot_gprtGetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Peridot_gprtGetCosToQueue.\n"));
        return retVal;
    }

    *queue = table.ingTbl.ieeeQpri;

    MSD_DBG_INFO(("Peridot_gprtGetCosToQueue Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtSetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       portNum - the logical port number
*       cos     - Trusted or assigned cos for the packet
*       dscp    - frame differentiated services code point
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cos,
    IN  MSD_U8      dscp
)
{
    MSD_STATUS       retVal;
    PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT table;

    MSD_DBG_INFO(("Peridot_gprtSetCosToDscp Called.\n"));

    retVal = Peridot_gprtGetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Peridot_gprtSetCosToDscp.\n"));
        return retVal;
    }

    table.egGreeTblDscp.enDSCPMarking = 0x1;
    table.egGreeTblDscp.frameDSCP = dscp;

    retVal = Peridot_gprtSetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to SetIEEEPrioMap in Peridot_gprtSetCosToDscp.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtSetCosToDscp Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtGetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       portNum - the logical port number
*       cos     - Trusted or assigned cos for the packet
*
* OUTPUTS:
*       dscp    - frame differentiated services code point
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cos,
    OUT MSD_U8      *dscp
)
{
    MSD_STATUS       retVal;
    PERIDOT_MSD_IEEE_PRIORITY_TABLE_STRUCT table;

    MSD_DBG_INFO(("Peridot_gprtGetCosToDscp Called.\n"));

    retVal = Peridot_gprtGetIEEEPrioMap(dev, portNum,
        PERIDOT_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP, cos, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Peridot_gprtGetCosToDscp.\n"));
        return retVal;
    }

    *dscp = table.egGreeTblDscp.frameDSCP;
    return retVal;
}

/******************************************************************************
* Peridot_gprtSetDefFpri
*
* Set port default priority.
*
******************************************************************************/
MSD_STATUS Peridot_gprtSetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8  	  defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16		    data;

    MSD_DBG_INFO(("Peridot_gprtSetDefFpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }
    if (defque > 7)
    {
        MSD_DBG_ERROR(("Bad defFPri: %u.\n", (unsigned int)defque));
        return MSD_BAD_PARAM;
    }

    data = (defque & 0x7);

    /* Set the default frame priority.  */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, PERIDOT_QD_REG_PVID, 13, 3, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("set Peridot_QD_REG_PVID Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gprtSetDefFpri Exit.\n"));
    return MSD_OK;
}

/******************************************************************************
* Peridot_gprtGetDefFpri
*
* Get port default priority.
*
******************************************************************************/
MSD_STATUS Peridot_gprtGetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_U8  	   *defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16		    data;

    MSD_DBG_INFO(("Peridot_gprtGetDefFpri Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    /* Get the default frame priority.  */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, PERIDOT_QD_REG_PVID, 13, 3, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("set Peridot_QD_REG_PVID Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    *defque = (MSD_U8)(data & 0x0007);

    MSD_DBG_INFO(("Peridot_gprtGetDefFpri Exit.\n"));
    return MSD_OK;
}