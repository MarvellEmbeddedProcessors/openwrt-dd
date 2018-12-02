#include <msdCopyright.h>

/*******************************************************************************
* Pearl_msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Vlan Translation Unit for 802.1Q.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <pearl/include/api/Pearl_msdBrgVtu.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdUtils.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

static MSD_STATUS Pearl_vtuOperationPerform
(
    IN        MSD_QD_DEV           *dev,
    IN      PEARL_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT     PEARL_MSD_VTU_ENTRY        *vtuEntry
);

/*******************************************************************************
* Pearl_gvtuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid VTU entry from the specified VID.
*
* INPUTS:
*       vtuEntry - the VID to start the search.
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
*       Search starts from the VID in the vtuEntry specified by the user.
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT PEARL_MSD_VTU_ENTRY  *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port; 
    MSD_LPORT               lport; 
    PEARL_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Pearl_gvtuGetEntryNext Called.\n"));

	if(vtuEntry->vid > 0xfff )
	{
		MSD_DBG_ERROR(("Bad vid: %d.\n", vtuEntry->vid));
        return MSD_BAD_PARAM;
	}
    entry.vid   = vtuEntry->vid;
    valid = 0;

	retVal = Pearl_vtuOperationPerform(dev, PEARL_GET_NEXT_ENTRY, &valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_vtuOperationPerform PEARL_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrieve the value from the operation */ 

	if ((entry.vid == 0xFFF) && (valid == 0))
	{
		MSD_DBG_INFO(("Pearl_gvtuGetEntryNext Exit, MSD_NO_SUCH\n"));
		return MSD_NO_SUCH;
	}

    vtuEntry->DBNum = entry.DBNum;
    vtuEntry->vid   = entry.vid;

    vtuEntry->vidPolicy = entry.vidPolicy;
    vtuEntry->sid = entry.sid;

    vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
    vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
	
    for(lport=0; lport<dev->numOfPorts; lport++)
    {
        port = MSD_LPORT_2_PORT(lport);
        vtuEntry->memberTagP[lport]=entry.memberTagP[port];
    }

    MSD_DBG_INFO(("Pearl_gvtuGetEntryNext Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvtuFlush
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
MSD_STATUS Pearl_gvtuFlush
(
    IN  MSD_QD_DEV       *dev
)
{
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Pearl_gvtuFlush Called.\n"));

    retVal = Pearl_vtuOperationPerform(dev,PEARL_FLUSH_ALL,NULL,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_vtuOperationPerform PEARL_FLUSH_ALL returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Pearl_gvtuFlush Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvtuAddEntry
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
MSD_STATUS Pearl_gvtuAddEntry
(
    IN  MSD_QD_DEV   *dev,
    IN PEARL_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8           port; 
    MSD_LPORT           lport; 
    PEARL_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Pearl_gvtuAddEntry Called.\n"));

	if(vtuEntry->vid > 0xfff || vtuEntry->sid > 0x3f || vtuEntry->DBNum > 0xff)
	{
		MSD_DBG_ERROR(("Bad vid or sid or DBNum: vid %d sid %d DBNum %d.\n", vtuEntry->vid, vtuEntry->sid, vtuEntry->DBNum));
        return MSD_BAD_PARAM;
	}
	entry.DBNum = vtuEntry->DBNum;
	entry.vid   = vtuEntry->vid;
	entry.vidPolicy = vtuEntry->vidPolicy;
	entry.sid = vtuEntry->sid;
	entry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
	entry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;
		
    valid = 1; /* for load operation */

    for(port=0; port<dev->maxPorts; port++)
    {
        lport = MSD_PORT_2_LPORT(port);
        if(lport == MSD_INVALID_PORT)
        {
            entry.memberTagP[port] = Pearl_NOT_A_MEMBER;
        }
        else
        {
            entry.memberTagP[port] = vtuEntry->memberTagP[lport];
        }
    }

    retVal = Pearl_vtuOperationPerform(dev,PEARL_LOAD_PURGE_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed (vtuOperationPerform returned MSD_FAIL).\n"));
        return retVal;
    }
	    
    MSD_DBG_INFO(("Pearl_gvtuAddEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvtuDelEntry
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
*       MSD_NO_SUCH - if specified address entry does not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuDelEntry
(
    IN  MSD_QD_DEV   *dev,
    IN  PEARL_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    PEARL_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Pearl_gvtuDelEntry Called.\n"));
    
	if(vtuEntry->vid > 0xfff)
	{
		MSD_DBG_ERROR(("Bad vid: %d.\n", vtuEntry->vid));
        return MSD_BAD_PARAM;
	}
    entry.vid   = vtuEntry->vid;
    valid = 0; /* for delete operation */

    retVal = Pearl_vtuOperationPerform(dev,PEARL_LOAD_PURGE_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_vtuOperationPerform delete entry returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    MSD_DBG_INFO(("Pearl_gvtuDelEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvtuGetViolation
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
MSD_STATUS Pearl_gvtuGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT PEARL_MSD_VTU_INT_STATUS *vtuIntStatus
)
{
    MSD_U8               spid;
    MSD_U16               vid;
    MSD_U16               intCause;
    MSD_STATUS           retVal;
    PEARL_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Pearl_gvtuGetViolation Called.\n"));

    /* check Violation occurred */
    retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_GLOBAL_STATUS,5,1,&intCause);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU Violation Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (intCause == 0)
    {
        /* No Violation occurred. */
        vtuIntStatus->vtuIntCause = 0;
		MSD_DBG_INFO(("Pearl_gvtuGetViolation Exit, No Violation occurred.\n"));
        return MSD_OK;
    }

    entry.DBNum = 0;

    retVal = Pearl_vtuOperationPerform(dev,PEARL_SERVICE_VIOLATIONS,NULL, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_vtuOperationPerform PEARL_SERVICE_VIOLATIONS returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,4,3,&intCause);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU OPERATION Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    spid = (MSD_U8)(entry.DBNum & 0xF);
    vid = entry.vid;

    if(spid == 0xF)
    {
        vtuIntStatus->vtuIntCause = PEARL_MSD_VTU_FULL_VIOLATION;
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = 0;
    }
    else
    {
        vtuIntStatus->vtuIntCause = intCause & (PEARL_MSD_VTU_MEMBER_VIOLATION | PEARL_MSD_VTU_MISS_VIOLATION);
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = vid;
    }
    
    MSD_DBG_INFO(("Pearl_gvtuGetViolation Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       vtuEntry - contains the VID to searche for
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
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Pearl_gvtuFindVidEntry
(
IN  MSD_QD_DEV       *dev,
INOUT PEARL_MSD_VTU_ENTRY  *vtuEntry,
OUT MSD_BOOL         *found
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_LPORT            lport;
    PEARL_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Pearl_gvtuFindVidEntry Called.\n"));

	if (NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_BOOL found is NULL. \n"));
		return MSD_BAD_PARAM;
	}

    *found = MSD_FALSE;

    /* Decrement 1 from vid */
    if (vtuEntry->vid == 0)
    {
        entry.vid = 0xFFF;
    }
    else
    {
        entry.vid = vtuEntry->vid - 1;
    }
    valid = 0; /* valid is not used as input in this operation */
    entry.DBNum = vtuEntry->DBNum;

    retVal = Pearl_vtuOperationPerform(dev, PEARL_GET_NEXT_ENTRY, &valid, &entry);
    if (retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Pearl_vtuOperationPerform vtuFindVidEntry returned: %d).\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrive the value from the operation */

    if ((entry.vid != vtuEntry->vid) || (valid != 1))
    {
		MSD_DBG_INFO(("Pearl_gvtuFindVidEntry Exit, MSD_NO_SUCH\n"));
        return MSD_OK;
    }

    vtuEntry->DBNum = entry.DBNum;
    vtuEntry->vid = entry.vid;

    vtuEntry->vidPolicy = entry.vidPolicy;
    vtuEntry->sid = entry.sid;

    vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
    vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
        for (lport = 0; lport<dev->numOfPorts; lport++)
    {
        vtuEntry->memberTagP[lport] = entry.memberTagP[lport];
    }

    *found = MSD_TRUE;

    MSD_DBG_INFO(("Pearl_gvtuFindVidEntry Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Pearl_vtuOperationPerform
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
*
*******************************************************************************/

static MSD_STATUS Pearl_vtuOperationPerform
(
    IN        MSD_QD_DEV           *dev,
    IN      PEARL_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT    PEARL_MSD_VTU_ENTRY        *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* Data to be set into the      */
								/* register.                    */

	msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);
  
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	/* Set the VTU data register    */
	/* There is no need to setup data reg. on flush, get next, or service violation */
	if((vtuOp != PEARL_FLUSH_ALL) && (vtuOp != PEARL_GET_NEXT_ENTRY) && (vtuOp != PEARL_SERVICE_VIOLATIONS))
	{
		/****************** VTU DATA 1 REG *******************/

		/* get data and wirte to QD_REG_VTU_DATA1_REG (ports 0 to 3) */
		data =  (MSD_U16)((entry->memberTagP[0] & 3)     |
				((entry->memberTagP[1] & 3)<<4) | 
				((entry->memberTagP[2] & 3)<<8));
			
		if(dev->maxPorts > 3)
		{
			data |= ((entry->memberTagP[3] & 3)<<12) ;
		}

		retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA1_REG,data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

	  /****************** VTU DATA 2 REG *******************/

		/* get data and wirte to QD_REG_VTU_DATA2_REG (ports 4 to 7) */

		if(dev->maxPorts > 4)
		{
			/* also need to set data register  ports 4 to 6 */

			data =  (MSD_U16)((entry->memberTagP[4] & 3)   |
						((entry->memberTagP[5] & 3) << 4));

			if(dev->maxPorts > 6)
			{
			  data |= ((entry->memberTagP[6] & 3)<<8) ;
			}

			retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA2_REG,data);
			if(retVal != MSD_OK)
			{
			  msdSemGive(dev->devNum, dev->vtuRegsSem);
			  return retVal;
			}
		}


		/****************** VTU DATA 3 REG *******************/

		if(entry->vidExInfo.useVIDFPri == MSD_TRUE)
			data = ((1 << 15) | ((entry->vidExInfo.vidFPri & 0x7) << 12));
		else
			data = 0;

		retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA3_REG,data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}
	}

	/* Set the VID register (QD_REG_VTU_VID_REG) */
	/* There is no need to setup VID reg. on flush and service violation */
	if ((vtuOp != PEARL_FLUSH_ALL) && (vtuOp != PEARL_SERVICE_VIOLATIONS))
	{
		data = 0;

		data |= ( (entry->vid) & 0xFFF ) | ( (*valid) << 12 );
		retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,(MSD_U8)(PEARL_QD_REG_VTU_VID_REG),data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
	}

	/* Set SID, FID, VIDPolicy, if it's Load operation */
	if((vtuOp == PEARL_LOAD_PURGE_ENTRY) && (*valid == 1))
	{
		data= (entry->sid) & 0x3F;
		
		retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,(MSD_U8)(PEARL_QD_REG_STU_SID_REG),data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
		

		data = 0;
		data |= ((entry->vidPolicy) << 12);


		data |= (entry->DBNum & 0xFF);

		retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,(MSD_U8)(PEARL_QD_REG_VTU_FID_REG),data);
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

	data = (MSD_U16)((1 << 15) | (vtuOp << 12));

	retVal = msdSetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	/* only two operations need to go through the mess below to get some data 
	* after the operations -  service violation and get next entry
	*/

	/* If the operation is to service violation operation wait for the response   */
	if(vtuOp == PEARL_SERVICE_VIOLATIONS)
	{
		/* Wait until the VTU in ready. */
		data = 1;
		while(data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,15,1,&data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}


		/* get the Source Port ID that was involved in the violation */
		retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,0,4,&data);
		entry->DBNum = (MSD_U8)(data & 0xF);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}


		/* get the VID that was involved in the violation */
		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_VID_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* Get the vid - bits 0-11 */
		entry->vid   = data & 0xFFF;

	} /* end of service violations */

	/* If the operation is a get next operation wait for the response   */
	if(vtuOp == PEARL_GET_NEXT_ENTRY)
	{
		entry->vidExInfo.useVIDFPri = MSD_FALSE;
		entry->vidExInfo.vidFPri = 0;


		entry->sid = 0;
		entry->vidPolicy = MSD_FALSE;

		/* Wait until the VTU in ready. */
		data = 1;
		while(data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_OPERATION,15,1,&data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}


		/****************** get the vid *******************/

		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_VID_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* the vid is bits 0-11 */
		entry->vid   = data & 0xFFF;

		/* the vid valid is bits 12 */
		*valid   = (MSD_U8)((data >> 12) & 1);

		if (*valid == 0)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return MSD_OK;
		}

		/****************** get the SID *******************/
		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,(MSD_U8)(PEARL_QD_REG_STU_SID_REG),&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
		entry->sid = (MSD_U8)(data & 0x3F);
		
		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,(MSD_U8)(PEARL_QD_REG_VTU_FID_REG),&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}   
		
		entry->vidPolicy = (data >> 12) & 0x1;
		

		entry->DBNum = data & 0xFF;

		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA1_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* get data from data register for ports 0 to 2 */
		entry->memberTagP[0]  =  data & 3 ;
		entry->memberTagP[1]  = (data >> 4) & 3 ;
		entry->memberTagP[2]  = (data >> 8) & 3 ;

		/****************** for the switch more than 3 ports *****************/

		if(dev->maxPorts > 3)
		{
			/* fullsail has 3 ports, clippership has 7 prots */
			entry->memberTagP[3]  = (data >>12) & 3 ;

			/* get data from data register for ports 4 to 6 */
			retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA2_REG,&data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
			entry->memberTagP[4]  = data & 3 ;
			entry->memberTagP[5]  = (data >> 4) & 3 ;

			if(dev->maxPorts > 6)
			{
				entry->memberTagP[6]  = (data >> 8) & 3 ;
			}
		}
		/****************** upto 7 port switch *******************/


		/* get data from data register for ports 4 to 6 */
		retVal = msdGetAnyReg(dev->devNum,  PEARL_GLOBAL1_DEV_ADDR,PEARL_QD_REG_VTU_DATA3_REG,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		if (data & 0x8000)
		{
			entry->vidExInfo.useVIDFPri = MSD_TRUE;
			entry->vidExInfo.vidFPri = (MSD_U8)((data >> 12) & 0x7);
		}
		else
		{
			entry->vidExInfo.useVIDFPri = MSD_FALSE;
			entry->vidExInfo.useVIDFPri = 0;
		}
	} /* end of get next entry */

	msdSemGive(dev->devNum, dev->vtuRegsSem);
	return MSD_OK;
}

MSD_STATUS Pearl_gvtuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	PEARL_MSD_VTU_ENTRY entry;
	int temp;

	msdMemSet(&entry, 0, sizeof(PEARL_MSD_VTU_ENTRY));
	entry.vid = 0xfff;

	/* Print out ATU entry field */
	MSG(("\n-----------------------------------------------------------------------------------\n"));
	MSG(("FID  VID   SID     MEMBERTAGP     VP  USEFPRI  FPRI \n"));
	MSG(("-----------------------------------------------------------------------------------\n"));

	temp = 1;
	while (1 == temp)
	{
		status = Pearl_gvtuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
			break;

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("%-5x % -5x % -5x % x %x %x %x %x %x %x   %-5x % --7x % -x\n", entry.DBNum, entry.vid, entry.sid,
			entry.memberTagP[0], entry.memberTagP[1], entry.memberTagP[2], entry.memberTagP[3], entry.memberTagP[4], entry.memberTagP[5], entry.memberTagP[6],
			entry.vidPolicy, entry.vidExInfo.useVIDFPri, entry.vidExInfo.vidFPri
			));
		if (entry.vid == 0xfff)
			break;

	}

	MSG(("\n"));
	return MSD_OK;
}
