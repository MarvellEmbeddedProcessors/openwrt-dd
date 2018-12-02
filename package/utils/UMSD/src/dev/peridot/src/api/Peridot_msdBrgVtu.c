#include <msdCopyright.h>

/*******************************************************************************
* Peridot_msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Vlan Translation Unit for 802.1Q.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <peridot/include/api/Peridot_msdBrgVtu.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

static MSD_STATUS Peridot_vtuOperationPerform
(
    IN        MSD_QD_DEV         *dev,
    IN      PERIDOT_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT     PERIDOT_MSD_VTU_ENTRY      *vtuEntry
);

/*******************************************************************************
* Peridot_gvtuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid VTU entry from the specified VID.
*
* INPUTS:
*       vtuEntry - the VID and VTUPage to start the search.
*
* OUTPUTS:
*       vtuEntry - match VTU  entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from the VID and VTU Page (13th vid) in the vtuEntry specified by the user.
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT PERIDOT_MSD_VTU_ENTRY  *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port; 
    MSD_LPORT               lport; 
    PERIDOT_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Peridot_gvtuGetEntryNext Called.\n"));

	if(vtuEntry->vid > 0xfff )
	{
		MSD_DBG_ERROR(("Bad vid: %d.\n", vtuEntry->vid));
        return MSD_BAD_PARAM;
	}
    entry.vid   = vtuEntry->vid;
	entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;
    valid = 0;

    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_GET_NEXT_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform PERIDOT_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrieve the value from the operation */ 

	if ((entry.vid == 0xFFF) && (valid == 0))
	{
		MSD_DBG_INFO(("Peridot_gvtuGetEntryNext Exit. MSD_NO_SUCH\n"));
		return MSD_NO_SUCH;
	}

    vtuEntry->DBNum = entry.DBNum;
    vtuEntry->vid   = entry.vid;

    vtuEntry->vidPolicy = entry.vidPolicy;
    vtuEntry->sid = entry.sid;

	vtuEntry->vidExInfo.useVIDQPri = entry.vidExInfo.useVIDQPri;
    vtuEntry->vidExInfo.vidQPri = entry.vidExInfo.vidQPri;
    vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
    vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
	vtuEntry->vidExInfo.dontLearn = entry.vidExInfo.dontLearn;
	vtuEntry->vidExInfo.filterUC = entry.vidExInfo.filterUC;
	vtuEntry->vidExInfo.filterBC = entry.vidExInfo.filterBC;
	vtuEntry->vidExInfo.filterMC = entry.vidExInfo.filterMC;
	vtuEntry->vidExInfo.vtuPage = entry.vidExInfo.vtuPage;

    for(lport=0; lport<dev->numOfPorts; lport++)
    {
        port = MSD_LPORT_2_PORT(lport);
        vtuEntry->memberTagP[lport]=entry.memberTagP[port];
    }

    MSD_DBG_INFO(("Peridot_gvtuGetEntryNext Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID and VTU Page, it will return the entry, if found, 
*       along with its associated data 
*
* INPUTS:
*       vtuEntry - contains the VID and VTU Page to search for.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       vtuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuFindVidEntry
(
    IN  MSD_QD_DEV       *dev,
    INOUT PERIDOT_MSD_VTU_ENTRY  *vtuEntry,
    OUT MSD_BOOL         *found
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port;
    MSD_LPORT            lport;
    PERIDOT_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Peridot_gvtuFindVidEntry Called.\n"));

	if (NULL == found || NULL == vtuEntry)
	{
		MSD_DBG_ERROR(("Input pointer param MSD_BOOL found or vtuentry is NULL.\n"));
		return MSD_BAD_PARAM;
	}

    *found = MSD_FALSE;

	valid = 0; /* valid is not used as input in this operation */
	if ((1 == vtuEntry->vidExInfo.vtuPage) && (0 == vtuEntry->vid))
	{
		entry.vid = 0xfff;
		entry.vidExInfo.vtuPage = 0;
	}
	else if ((0 == vtuEntry->vidExInfo.vtuPage) && (0 == vtuEntry->vid))
	{
		entry.vid = 0xfff;
		entry.vidExInfo.vtuPage = 1;
	}
	else 
	{
		/* Decrement 1 from vid    */
		entry.vid   = vtuEntry->vid-1;	
		entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;
	}
	
    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_GET_NEXT_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform vtuFindVidEntry returned: %d).\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrive the value from the operation */ 

	if( (entry.vid !=vtuEntry->vid) || (entry.vidExInfo.vtuPage != vtuEntry->vidExInfo.vtuPage) || (valid !=1) )
    {
		MSD_DBG_INFO(("Peridot_gvtuFindVidEntry Exit.\n"));
        return MSD_OK;
    }

    vtuEntry->DBNum = entry.DBNum;
    vtuEntry->vid   = entry.vid;

    vtuEntry->vidPolicy = entry.vidPolicy;
    vtuEntry->sid = entry.sid;

	vtuEntry->vidExInfo.useVIDQPri = entry.vidExInfo.useVIDQPri;
    vtuEntry->vidExInfo.vidQPri = entry.vidExInfo.vidQPri;
    vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
    vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
	vtuEntry->vidExInfo.dontLearn = entry.vidExInfo.dontLearn;
	vtuEntry->vidExInfo.filterUC = entry.vidExInfo.filterUC;
	vtuEntry->vidExInfo.filterBC = entry.vidExInfo.filterBC;
	vtuEntry->vidExInfo.filterMC = entry.vidExInfo.filterMC;
	vtuEntry->vidExInfo.vtuPage = entry.vidExInfo.vtuPage;

    for(lport=0; lport<dev->numOfPorts; lport++)
    {
        port = MSD_LPORT_2_PORT(lport);
        vtuEntry->memberTagP[lport]=entry.memberTagP[port];
    }

    *found = MSD_TRUE;

    MSD_DBG_INFO(("Peridot_gvtuFindVidEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuFlush
*
* DESCRIPTION:
*       This routine removes all entries from VTU Table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuFlush
(
    IN  MSD_QD_DEV       *dev
)
{
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Peridot_gvtuFlush Called.\n"));

    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_FLUSH_ALL,NULL,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform PERIDOT_FLUSH_ALL returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gvtuFlush Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuAddEntry
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       vtuEntry - vtu entry to insert to the VTU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuAddEntry
(
    IN  MSD_QD_DEV   *dev,
    IN PERIDOT_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8           port; 
    MSD_LPORT           lport; 
    PERIDOT_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Peridot_gvtuAddEntry Called.\n"));

	if(vtuEntry->vid > 0xfff || vtuEntry->sid > 0x3f || vtuEntry->DBNum > 0xfff)
	{
		MSD_DBG_ERROR(("Bad vid or sid or DBNum: vid %d sid %d DBNum %d.\n", vtuEntry->vid, vtuEntry->sid, vtuEntry->DBNum));
        return MSD_BAD_PARAM;
	}
	entry.DBNum = vtuEntry->DBNum;
	entry.vid   = vtuEntry->vid;
	entry.vidPolicy = vtuEntry->vidPolicy;
	entry.sid = vtuEntry->sid;
	entry.vidExInfo.useVIDQPri = vtuEntry->vidExInfo.useVIDQPri;
	entry.vidExInfo.vidQPri = vtuEntry->vidExInfo.vidQPri;
	entry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
	entry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;
	entry.vidExInfo.dontLearn = vtuEntry->vidExInfo.dontLearn;
	entry.vidExInfo.filterUC = vtuEntry->vidExInfo.filterUC;
	entry.vidExInfo.filterBC = vtuEntry->vidExInfo.filterBC;
	entry.vidExInfo.filterMC = vtuEntry->vidExInfo.filterMC;
	entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;

    valid = 1; /* for load operation */

    for(port=0; port<dev->maxPorts; port++)
    {
        lport = MSD_PORT_2_LPORT(port);
        if(lport == MSD_INVALID_PORT)
        {
            entry.memberTagP[port] = Peridot_NOT_A_MEMBER;
        }
        else
        {
            entry.memberTagP[port] = vtuEntry->memberTagP[lport];
        }
    }

    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_LOAD_PURGE_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform load entry returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gvtuAddEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuDelEntry
*
* DESCRIPTION:
*       Deletes VTU entry specified by user.
*
* INPUTS:
*       vtuEntry - the VTU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuDelEntry
(
    IN  MSD_QD_DEV   *dev,
    IN PERIDOT_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    PERIDOT_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Peridot_gvtuDelEntry Called.\n"));
    
	if(vtuEntry->vid > 0xfff)
	{
		MSD_DBG_ERROR(("Bad vid: %d.\n", vtuEntry->vid));
        return MSD_BAD_PARAM;
	}
    entry.vid   = vtuEntry->vid;
	entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;

	valid = 0; /* for delete operation */

    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_LOAD_PURGE_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform delete entry returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    MSD_DBG_INFO(("Peridot_gvtuDelEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuSetMode
*
* DESCRIPTION:
*       Set VTU Mode
*
* INPUTS:
*       PERIDOT_MSD_VTU_MODE - VTU Mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuSetMode
(
    IN  MSD_QD_DEV      *dev,
    IN  PERIDOT_MSD_VTU_MODE    mode
)
{
	MSD_STATUS	retVal;
	MSD_U16		data;
	
    MSD_DBG_INFO(("Peridot_gvtuSetMode Called.\n"));

    if (mode < Peridot_VTU_MODE_0 || mode > Peridot_VTU_MODE_3)
    {
        MSD_DBG_ERROR(("Bad mdoe: %d\n", mode));
        return MSD_BAD_PARAM;
    }

	msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);
		
	data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
			MSD_DBG_ERROR(("Read VTU Operation register busy bit returned: %s", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

	data = (MSD_U16)mode;
	retVal = msdSetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,10,2,data);
	if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	msdSemGive(dev->devNum, dev->vtuRegsSem);

	MSD_DBG_INFO(("Peridot_gvtuSetMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuGetMode
*
* DESCRIPTION:
*       Get VTU Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       PERIDOT_MSD_VTU_MODE - VTU Mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuGetMode
(
    IN  MSD_QD_DEV     *dev,
    OUT PERIDOT_MSD_VTU_MODE   *mode
)
{
	MSD_STATUS	retVal;
	MSD_U16		data;
	
    MSD_DBG_INFO(("Peridot_gvtuGetMode Called.\n"));
	msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);

	data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,10,2,&data);
	if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	switch(data)
	{
		case 0:
			*mode = Peridot_VTU_MODE_0;
			break;
		case 1:
			*mode = Peridot_VTU_MODE_1;
			break;
		case 2:
			*mode = Peridot_VTU_MODE_2;
			break;
		case 3:
			*mode = Peridot_VTU_MODE_3;
			break;
		default:
			MSD_DBG_ERROR(("Get VTU MODE, data: %d\n", data));
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return MSD_FAIL;
	}
	msdSemGive(dev->devNum, dev->vtuRegsSem);

	MSD_DBG_INFO(("Peridot_gvtuGetMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_gvtuGetViolation
*
* DESCRIPTION:
*       Get VTU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Peridot_gvtuGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT PERIDOT_MSD_VTU_INT_STATUS *vtuIntStatus
)
{
    MSD_U8               spid;
    MSD_U16              vid;
	MSD_U8				page;
    MSD_U16              intCause;
    MSD_STATUS           retVal;
    PERIDOT_MSD_VTU_ENTRY        entry;
	MSD_U16				tmp;

    MSD_DBG_INFO(("Peridot_gvtuGetViolation Called.\n"));

    /* check Violation occurred */
    retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_GLOBAL_STATUS,5,1,&intCause);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU Violation Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (intCause == 0)
    {
        /* No Violation occurred. */
        vtuIntStatus->vtuIntCause = 0;
		MSD_DBG_INFO(("Peridot_gvtuGetViolation Exit, No Violation occurred.\n"));
        return MSD_OK;
    }

    entry.DBNum = 0;

    retVal = Peridot_vtuOperationPerform(dev,PERIDOT_SERVICE_VIOLATIONS,NULL, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_vtuOperationPerform PERIDOT_SERVICE_VIOLATIONS returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,&tmp);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU OPERATION Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	intCause = (MSD_U16)((tmp >> 4) & 0x7);
    spid = (MSD_U8)(tmp & 0x1F);
    vid = entry.vid;
	page = entry.vidExInfo.vtuPage;

    if(spid == 0x1F)
    {
        vtuIntStatus->vtuIntCause = PERIDOT_MSD_VTU_FULL_VIOLATION;
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = 0;
    }
    else
    {
        vtuIntStatus->vtuIntCause = intCause & (PERIDOT_MSD_VTU_MEMBER_VIOLATION | PERIDOT_MSD_VTU_MISS_VIOLATION);
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = vid|((page & 0x1)<<12);
    }
    
    MSD_DBG_INFO(("Peridot_gvtuGetViolation Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Peridot_vtuOperationPerform
*
* DESCRIPTION:
*       This function is used by all VTU control functions, and is responsible
*       to write the required operation into the VTU registers.
*
* INPUTS:
*       vtuOp       - The VTU operation bits to be written into the VTU
*                     operation register.
*       DBNum       - DBNum where the given vid belongs to
*       vid         - vlan id
*       valid       - valid bit
*       vtuData     - VTU Data with memberTag information
*
* OUTPUTS:
*       DBNum       - DBNum where the given vid belongs to
*       vid         - vlan id
*       valid       - valid bit
*       vtuData     - VTU Data with memberTag information
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*		None.
*
*******************************************************************************/

static MSD_STATUS Peridot_vtuOperationPerform
(
    IN        MSD_QD_DEV         *dev,
    IN      PERIDOT_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT    PERIDOT_MSD_VTU_ENTRY       *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* Data to be set into the      */
								/* register.                    */

	msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);
  
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	/* Set the VTU data register    */
	/* There is no need to setup data reg. on flush, Peridot_get next, or service violation */
	if((vtuOp != PERIDOT_FLUSH_ALL) && (vtuOp != PERIDOT_GET_NEXT_ENTRY) && (vtuOp != PERIDOT_SERVICE_VIOLATIONS))
	{
		/****************** VTU DATA 1 REG *******************/

		/* Peridot_get data and wirte to PERIDOT_QD_REG_VTU_DATA1_REG (ports 0 to 3) */
		data = (MSD_U16) ((entry->memberTagP[0] & 3)|((entry->memberTagP[1] & 3)<<2)
			|((entry->memberTagP[2] & 3)<<4)|((entry->memberTagP[3] & 3)<<6)
			|((entry->memberTagP[4] & 3)<<8)|((entry->memberTagP[5] & 3)<<10)
			|((entry->memberTagP[6] & 3)<<12)|((entry->memberTagP[7] & 3)<<14));

		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_DATA1_REG,data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

	    /****************** VTU DATA 2 REG *******************/
		if(dev->maxPorts > 8)
		{
			
			data =  (MSD_U16)((entry->memberTagP[8] & 3)|((entry->memberTagP[9] & 3)<<2)
				|((entry->memberTagP[10] & 3)<<4));

			if(entry->vidExInfo.useVIDQPri == MSD_TRUE)
				data |= ((1 << 15) | ((entry->vidExInfo.vidQPri & 0x7) << 12));

			if(entry->vidExInfo.useVIDFPri == MSD_TRUE)
				data |= ((1 << 11) | ((entry->vidExInfo.vidFPri & 0x7) << 8));

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_DATA2_REG,data);
			if(retVal != MSD_OK)
			{
			  msdSemGive(dev->devNum, dev->vtuRegsSem);
			  return retVal;
			}
		}
	}

	/* Set the VID register (PERIDOT_QD_REG_VTU_VID_REG) */
	/* There is no need to setup VID reg. on flush and service violation */
	if((vtuOp != PERIDOT_FLUSH_ALL) && (vtuOp != PERIDOT_SERVICE_VIOLATIONS) )
	{
		data = 0;

		data |= (((entry->vidExInfo.vtuPage & 0x1)<<13) | ((entry->vid) & 0xFFF ) | ( (*valid) << 12 ));
		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,(MSD_U8)(PERIDOT_QD_REG_VTU_VID_REG),data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
	}

	/* Set SID, FID, VIDPolicy, if it's Load operation */
	if((vtuOp == PERIDOT_LOAD_PURGE_ENTRY) && (*valid == 1))
	{
		data= (entry->sid) & 0x3F;
		if(entry->vidExInfo.dontLearn)
			data |= 0x8000;
		if(entry->vidExInfo.filterUC)
			data |= 0x4000;
		if(entry->vidExInfo.filterBC)
			data |= 0x2000;
		if(entry->vidExInfo.filterMC)
			data |= 0x1000;

		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,(MSD_U8)(PERIDOT_QD_REG_STU_SID_REG),data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
		

		data = 0;
		data= (MSD_U16)((entry->vidPolicy) << 12);


		data |= (entry->DBNum & 0xFFF);

		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,(MSD_U8)(PERIDOT_QD_REG_VTU_FID_REG),data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
	}

	/* Start the VTU Operation by defining the DBNum, vtuOp and VTUBusy    */
	/* 
	 * Flush operation will skip the above two setup (for data and vid), and 
	 * come to here directly
	*/

	retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR, PERIDOT_QD_REG_VTU_OPERATION, &data);	
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	data &= 0xc00;
	data |= (1 << 15) | (vtuOp << 12);

	retVal = msdSetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	/* only two operations need to go through the mess below to Peridot_get some data 
	* after the operations -  service violation and Peridot_get next entry
	*/

	/* If the operation is to service violation operation wait for the response   */
	if(vtuOp == PERIDOT_SERVICE_VIOLATIONS)
	{
		/* Wait until the VTU in ready. */
		data = 1;
		while(data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,15,1,&data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}

		/* Peridot_get the VID that was involved in the violation */
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_VID_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* Get the vid - bits 0-11 */
		entry->vid   = data & 0xFFF;

		/* Get the page vid - bit 12 */
		entry->vidExInfo.vtuPage  = (MSD_U8)((data & 0x2000) >> 13);

	} /* end of service violations */

	/* If the operation is a Peridot_get next operation wait for the response   */
	if(vtuOp == PERIDOT_GET_NEXT_ENTRY)
	{
		entry->vidExInfo.useVIDQPri = MSD_FALSE;
		entry->vidExInfo.vidQPri = 0;
		entry->vidExInfo.useVIDFPri = MSD_FALSE;
		entry->vidExInfo.vidFPri = 0;
		entry->vidExInfo.vtuPage = 0;

		entry->sid = 0;
		entry->vidPolicy = MSD_FALSE;

		/* Wait until the VTU in ready. */
		data = 1;
		while(data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_OPERATION,15,1,&data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}


		/****************** Peridot_get the vid *******************/

		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_VID_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* the vid is bits 0-11 */
		entry->vid   = data & 0xFFF;

		entry->vidExInfo.vtuPage = (MSD_U8)((data >> 13) & 1);

		/* the vid valid is bits 12 */
		*valid   = (MSD_U8)((data >> 12) & 1);

		if (*valid == 0)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return MSD_OK;
		}

		/****************** Peridot_get the SID *******************/
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,(MSD_U8)(PERIDOT_QD_REG_STU_SID_REG),&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
		entry->sid = (MSD_U8)(data & 0x3F);
		
		entry->vidExInfo.dontLearn = (data & 0x8000)>>15;
		entry->vidExInfo.filterUC = (data & 0x4000)>>14;
		entry->vidExInfo.filterBC = (data & 0x2000)>>13;
		entry->vidExInfo.filterMC = (data & 0x1000)>>12;

		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,(MSD_U8)(PERIDOT_QD_REG_VTU_FID_REG),&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}   
		
		entry->vidPolicy = (data >> 12) & 0x1;
		

		entry->DBNum = data & 0xFFF;

		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_DATA1_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* Peridot_get data from data register for ports 0 to 7 */
		entry->memberTagP[0]  =  data & 3 ;
		entry->memberTagP[1]  = (data >> 2) & 3 ;
		entry->memberTagP[2]  = (data >> 4) & 3 ;
		entry->memberTagP[3]  = (data >> 6) & 3 ;
		entry->memberTagP[4]  = (data >> 8) & 3 ;
		entry->memberTagP[5]  = (data >> 10) & 3 ;
		entry->memberTagP[6]  = (data >> 12) & 3 ;
		entry->memberTagP[7]  = (data >> 14) & 3 ;

		/* Peridot_get data from data register for ports 8 to 10 */
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_GLOBAL1_DEV_ADDR,PERIDOT_QD_REG_VTU_DATA2_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}
		entry->memberTagP[8]  = data & 3 ;
		entry->memberTagP[9]  = (data >> 2) & 3 ;
		entry->memberTagP[10]  = (data >> 4) & 3 ;

		if (data & 0x8000)
		{
			entry->vidExInfo.useVIDQPri = MSD_TRUE;
			entry->vidExInfo.vidQPri = (MSD_U8)((data >> 12) & 0x7);
		}
		else
		{
			entry->vidExInfo.useVIDQPri = MSD_FALSE;
			entry->vidExInfo.useVIDQPri = 0;
		}
		
		if (data & 0x800)
		{
			entry->vidExInfo.useVIDFPri = MSD_TRUE;
			entry->vidExInfo.vidFPri = (MSD_U8)((data >> 8) & 0x7);
		}
		else
		{
			entry->vidExInfo.useVIDFPri = MSD_FALSE;
			entry->vidExInfo.useVIDFPri = 0;
		}
	} /* end of Peridot_get next entry */

	msdSemGive(dev->devNum, dev->vtuRegsSem);
	return MSD_OK;
}
MSD_STATUS Peridot_gvtuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	PERIDOT_MSD_VTU_ENTRY entry;
	int temp;

	msdMemSet(&entry, 0, sizeof(PERIDOT_MSD_VTU_ENTRY));
	entry.vidExInfo.vtuPage = 1;
	entry.vid = 0xfff;

	/* Print out ATU entry field */
	MSG(("\n-----------------------------------------------------------------------------------------------------\n"));
	MSG(("FID  VID  SID  MEMBERTAGP              VP USEFPRI FPRI USEQPRI QPRI DONTLEARN FILTERUC FILTERBC FILTERMC\n"));
	MSG(("-----------------------------------------------------------------------------------------------------\n"));

	temp = 1;
	while (1 == temp)
	{
		status = Peridot_gvtuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
			break;

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("%-5x%-5x%-5x%x %x %x %x %x %x %x %x %x %x %x   %-3x%--8x%-5x%-8x%-5x%-10x%-9x%-9x%-9x\n", entry.DBNum, (entry.vid | (entry.vidExInfo.vtuPage << 12)), entry.sid,
			entry.memberTagP[0], entry.memberTagP[1], entry.memberTagP[2], entry.memberTagP[3], entry.memberTagP[4], entry.memberTagP[5],
			entry.memberTagP[6], entry.memberTagP[7], entry.memberTagP[8], entry.memberTagP[9], entry.memberTagP[10], entry.vidPolicy, 
			entry.vidExInfo.useVIDFPri, entry.vidExInfo.vidFPri, entry.vidExInfo.useVIDQPri, entry.vidExInfo.vidQPri,
			entry.vidExInfo.dontLearn, entry.vidExInfo.filterUC, entry.vidExInfo.filterBC, entry.vidExInfo.filterMC));

		if (entry.vid == 0xfff && entry.vidExInfo.vtuPage == 1)
			break;

	}

	MSG(("\n"));
	return MSD_OK;
}