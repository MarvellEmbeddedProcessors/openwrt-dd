#include <msdCopyright.h>

/********************************************************************************
* Pearl_msdPortPrioMap.c
*
* DESCRIPTION:
*       API definitions for Port Priority Mapping Rules control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdPortPrioMap.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


/*******************************************************************************
* Pearl_gqosSetPrioMapRule
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
MSD_STATUS Pearl_gqosSetPrioMapRule
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

    MSD_DBG_INFO(("Pearl_gqosSetPrioMapRule Called.\n"));
    /* translate bool to binary */
	MSD_BOOL_2_BIT(mode, data);
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
   
    /* Set the TagIfBoth.  */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL, 6, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PEARL_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gqosSetPrioMapRule Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine Pearl_gets priority mapping rule (TagIfBoth bit).
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
MSD_STATUS Pearl_gqosGetPrioMapRule
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

    MSD_DBG_INFO(("Pearl_gqosGetPrioMapRule Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    
    /* Pearl_get the TagIfBoth.  */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL, 6, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("read PEARL_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* translate bool to binary */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Pearl_gqosGetPrioMapRule Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            PEARL_MSD_INITIAL_PRI_DEFAULT
*               
*            PEARL_MSD_INITIAL_PRI_TAG
*                
*            PEARL_MSD_INITIAL_PRI_IP
*                
*            PEARL_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - PEARL_MSD_INITIAL_PRI type
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
MSD_STATUS Pearl_gprtSetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  PEARL_MSD_INITIAL_PRI  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16            data;

    MSD_DBG_INFO(("Pearl_gprtSetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    if (mode < PEARL_MSD_INITIAL_PRI_DEFAULT || mode > PEARL_MSD_INITIAL_PRI_TAG_IP)
    {
        MSD_DBG_ERROR(("Bad mode: %u.\n", (unsigned int)mode));
        return MSD_BAD_PARAM;
    }

    switch(mode)
    {
        case PEARL_MSD_INITIAL_PRI_DEFAULT:
            data = 0;
            break;
		case PEARL_MSD_INITIAL_PRI_TAG:
			data = 1;
			break;
		case PEARL_MSD_INITIAL_PRI_IP:
			data = 2;
			break;
		case PEARL_MSD_INITIAL_PRI_TAG_IP:
			data = 3;
			break;
        default:
            {
                MSD_DBG_ERROR(("Failed Bad Mode: %u.\n", (unsigned int)mode));
                return MSD_BAD_PARAM;
            }
    }

    /* Set the PEARL_MSD_INITIAL_PRI mode.            */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL, 4, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PERIDOT_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gprtSetInitialPri Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine Pearl_gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            PEARL_MSD_INITIAL_PRI_DEFAULT
*               
*            PEARL_MSD_INITIAL_PRI_TAG
*                
*            PEARL_MSD_INITIAL_PRI_IP
*                
*            PEARL_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - PEARL_MSD_INITIAL_PRI type
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
MSD_STATUS Pearl_gprtGetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT PEARL_MSD_INITIAL_PRI  *mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Pearl_gprtGetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    /* Pearl_get the PEARL_MSD_INITIAL_PRI type            */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL, 4, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("read PEARL_QD_REG_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    switch(data)
    {
        case 0x0:
            *mode = PEARL_MSD_INITIAL_PRI_DEFAULT;
            break;
		case 0x1:
			*mode = PEARL_MSD_INITIAL_PRI_TAG;
			break;
		case 0x2:
			*mode = PEARL_MSD_INITIAL_PRI_IP;
			break;
		case 0x3:
			*mode = PEARL_MSD_INITIAL_PRI_TAG_IP;
			break;
        default:
            {
                MSD_DBG_ERROR(("Failed (get the PEARL_MSD_INITIAL_PRI type fail).\n"));
                return MSD_FAIL;
            }
    }

    MSD_DBG_INFO(("Pearl_gprtGetInitialPri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Pearl_gprtSetDefQpri
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
MSD_STATUS Pearl_gprtSetDefQpri
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

	MSD_DBG_INFO(("Pearl_gprtSetDefQpri Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
    if (defque > 3)
    {
        MSD_DBG_ERROR(("Bad defque: %u.\n", (unsigned int)defque));
        return MSD_BAD_PARAM;
    }

	data = (defque&0x3);

    /* Set the default queue priority.               */
    retVal = msdSetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL2, 1, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write PEARL_QD_REG_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gprtSetDefQpri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Pearl_gprtGetDefQpri
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
MSD_STATUS Pearl_gprtGetDefQpri
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

    MSD_DBG_INFO(("Pearl_gprtGetDefQpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    /* Set the default queue priority.               */
    retVal = msdGetAnyRegField(dev->devNum,  phyAddr, PEARL_QD_REG_PORT_CONTROL2, 1, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("read PEARL_QD_REG_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
	*defque = msdShort2Char(data&0x3);

    MSD_DBG_INFO(("Pearl_gprtGetDefQpri Exit.\n"));
	return MSD_OK;
}

/******************************************************************************
* Pearl_gprtSetDefFpri
*
* Set port default priority.
*
******************************************************************************/
MSD_STATUS Pearl_gprtSetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8  	  defFpri
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16		    data;

    MSD_DBG_INFO(("Pearl_gprtSetDefFpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }
    if (defFpri > 7)
    {
        MSD_DBG_ERROR(("Bad defFpri: %u.\n", (unsigned int)defFpri));
        return MSD_BAD_PARAM;
    }

    data = (defFpri & 0x7);

    /* Set the default frame priority.  */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, PEARL_QD_REG_PVID, 13, 3, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("set PEARL_QD_REG_PVID Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gprtSetDefFpri Exit.\n"));
    return MSD_OK;
}

/******************************************************************************
* Pearl_gprtGetDefFpri
*
* Get port default priority.
*
******************************************************************************/
MSD_STATUS Pearl_gprtGetDefFpri
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

    MSD_DBG_INFO(("Pearl_gprtGetDefFpri Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = PEARL_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    /* Get the default frame priority.  */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, PEARL_QD_REG_PVID, 13, 3, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("set PEARL_QD_REG_PVID Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    *defque = (MSD_U8)(data & 0x0007);

    MSD_DBG_INFO(("Pearl_gprtGetDefFpri Exit.\n"));
    return MSD_OK;
}