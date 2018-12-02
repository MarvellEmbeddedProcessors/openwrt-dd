#include <msdCopyright.h>

/*******************************************************************************
* Peridot_msdTCAM.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdTCAM.h>
#include <msdSem.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdHwAccess.h>
/****************************************************************************/
/* Internal TCAM structure declaration.                                    */
/****************************************************************************/

/*
 *  typedef: struct PERIDOT_MSD_TCAM_PG_DATA
 *
 *  Description: data required by Ingress TCAM entry page 
 *
 *  Fields:
 *      
 */
typedef struct
{
	MSD_U16  frame[26];  /* key&mask, 0-21 bytes of frame */
					  
} PERIDOT_MSD_TCAM_PG_DATA;

/*
 *  typedef: struct PERIDOT_MSD_TCAM_DATA_HW
 *
 *  Description: data required by Ingress TCAM (Ternary Content Addressable Memory) Operation
 *
 *  Fields:
 *      pg0 - page 0 registers  of TCAM
 *      pg1 - page 1 registers  of TCAM
 *      pg2 - page 2 registers  of TCAM
 */
typedef struct 
{
	/* Pg0 registers */
	PERIDOT_MSD_TCAM_PG_DATA pg0;
	/* Pg1 registers */
	PERIDOT_MSD_TCAM_PG_DATA pg1;
	/* Pg2 registers */
	PERIDOT_MSD_TCAM_PG_DATA pg2;
} PERIDOT_MSD_TCAM_DATA_HW;

/*
 *  typedef: struct PERIDOT_MSD_TCAM_EGR_DATA_HW
 *
 *  Description: data required by Egress TCAM Action Page - TCAM entry page 3
 *
 *  Fields:
 *      
 */
typedef struct
{
	MSD_U16  frame[3];  /* action part */

} PERIDOT_MSD_TCAM_EGR_DATA_HW;

typedef struct
{
    MSD_U32    tcamEntry;
    PERIDOT_MSD_TCAM_DATA_HW    tcamDataP; 
} PERIDOT_MSD_TCAM_OP_DATA;

typedef struct 
{
	MSD_U8  port;
	MSD_U32	  tcamEntry;
	PERIDOT_MSD_TCAM_EGR_DATA_HW	tcamDataP;
} PERIDOT_MSD_TCAM_EGR_OP_DATA;

typedef enum
{
	Peridot_TCAM_FLUSH_ALL = 0x1,
	Peridot_TCAM_FLUSH_ENTRY = 0x2,
	Peridot_TCAM_LOAD_ENTRY = 0x3,
	Peridot_TCAM_PURGE_ENTRY = 0x3,
	Peridot_TCAM_GET_NEXT_ENTRY = 0x4,
	Peridot_TCAM_READ_ENTRY = 0x5
} PERIDOT_MSD_TCAM_OPERATION;

/****************************************************************************/
/* TCAM operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Peridot_tcamOperationPerform
(
    IN   MSD_QD_DEV             *dev,
    IN   PERIDOT_MSD_TCAM_OPERATION    tcamOp,
    INOUT PERIDOT_MSD_TCAM_OP_DATA    *opData
);

static MSD_STATUS Peridot_tcamEgrOperationPerform
(
    IN   MSD_QD_DEV             *dev,
    IN   PERIDOT_MSD_TCAM_OPERATION    tcamOp,
    INOUT PERIDOT_MSD_TCAM_EGR_OP_DATA    *opData
);
static MSD_STATUS Peridot_setTcamHWData
(
    IN	PERIDOT_MSD_TCAM_DATA	*iData,
	OUT	PERIDOT_MSD_TCAM_DATA_HW	*oData
);
static MSD_STATUS Peridot_getTcamHWData
(
	IN	PERIDOT_MSD_TCAM_DATA_HW	*iData,
	OUT	PERIDOT_MSD_TCAM_DATA	*oData
);
static MSD_STATUS Peridot_setTcamEgrHWData
(
	IN	PERIDOT_MSD_TCAM_EGR_DATA	*iData,
	OUT	PERIDOT_MSD_TCAM_EGR_DATA_HW	*oData
);
static MSD_STATUS Peridot_getTcamEgrHWData
(
	IN	PERIDOT_MSD_TCAM_EGR_DATA_HW	*iData,
	OUT	PERIDOT_MSD_TCAM_EGR_DATA	*oData
);
static void displayTcamData(PERIDOT_MSD_TCAM_DATA *tcamData);
static void mappingIpv4ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, MSD_TCAM_ENT_IPV4_UNT *pIpv4Data, MSD_TCAM_ENT_IPV4_UNT *pIpv4Mask);
static void mappingUdpToTcam(int startIndex, PERIDOT_MSD_TCAM_DATA *tcamData, MSD_TCAM_ENT_UDP_UNT *pUdpData, MSD_TCAM_ENT_UDP_UNT *pUdpMask);
static void mappingActionToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_ENT_ACT *actionPtr);
static void mappingIpv6ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_IPV6_UNT *pIpv6, MSD_TCAM_ENT_IPV6_UNT *pIpv6Mask);
static void mappingTcpOverIpv4ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_TCP_UNT *pTcpData, MSD_TCAM_ENT_TCP_UNT *pTcpMask);
static void mappingTcpOverIpv6ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_TCP_UNT *pTcpData, MSD_TCAM_ENT_TCP_UNT *pTcpMask);
static MSD_BOOL checkEntry2Used(PERIDOT_MSD_TCAM_DATA *tcamData2);

/*******************************************************************************
* Peridot_gtcamFlushAll
*
* DESCRIPTION:
*       This routine is to flush all entries. A Flush All command will initialize 
*       TCAM Pages 0 and 1, offsets 0x02 to 0x1B to 0x0000, and TCAM Page 2 offset
*       0x02 to 0x1B to 0x0000 for all TCAM entries with the exception that TCAM 
*       Page 0 offset 0x02 will be initialized to 0x00FF.
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamFlushAll
(
    IN  MSD_QD_DEV     *dev
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamFlushAll Called.\n"));

    /* Program Tuning register */
    op = Peridot_TCAM_FLUSH_ALL;
    retVal = Peridot_tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamFlushAll Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single entry. A Flush a single TCAM entry command 
*       will write the same values to a TCAM entry as a Flush All command, but it is
*       done to the selected single TCAM entry only.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
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
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamFlushEntry
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamFlushEntry Called.\n"));

    /* check if the given pointer is valid */
    if (tcamPointer >= PERIDOT_MAX_IGR_TCAM_ENTRY)
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u. It should be within [0, %d].\n", (unsigned int)tcamPointer, (int)(PERIDOT_MAX_IGR_TCAM_ENTRY - 1)));
        return MSD_BAD_PARAM;
	}

    /* Program Tuning register */
    op = Peridot_TCAM_FLUSH_ENTRY;
    tcamOpData.tcamEntry = tcamPointer;
    retVal = Peridot_tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamFlushEntry Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
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
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamLoadEntry
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    IN  PERIDOT_MSD_TCAM_DATA        *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamLoadEntry Called.\n"));

    /* check if the given pointer is valid */
	if ((tcamPointer >= PERIDOT_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL.\n", (unsigned int)tcamPointer));
        return MSD_BAD_PARAM;
	}

    /* Program Tuning register */
    op = Peridot_TCAM_LOAD_ENTRY;
    tcamOpData.tcamEntry = tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
	Peridot_setTcamHWData(tcamData, &tcamOpData.tcamDataP);
	retVal = Peridot_tcamOperationPerform(dev, op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamLoadEntry Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine reads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamReadTCAMData
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    OUT PERIDOT_MSD_TCAM_DATA        *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamReadTCAMData Called.\n"));


    /* check if the given pointer is valid */
	if ((tcamPointer >= PERIDOT_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL.\n", (unsigned int)tcamPointer));
		return MSD_BAD_PARAM;
	}

    /* Program Tuning register */
    op = Peridot_TCAM_READ_ENTRY;
    tcamOpData.tcamEntry = tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
	retVal = Peridot_tcamOperationPerform(dev, op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	Peridot_getTcamHWData(&tcamOpData.tcamDataP,tcamData);

    MSD_DBG_INFO(("Peridot_gtcamReadTCAMData Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamGetNextTCAMData
*
* DESCRIPTION:
*       This routine  finds the next higher TCAM Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry 
*       register (bits 6:0) is used as the TCAM entry to start from. To find 
*       the lowest number TCAM Entry that is valid, start the Get Next operation 
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamGetNextTCAMData
(
    IN     MSD_QD_DEV     *dev,
    INOUT  MSD_U32        *tcamPointer,
    OUT    PERIDOT_MSD_TCAM_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamGetNextTCAMData Called.\n"));

    /* check if the given pointer is valid */
	if (tcamPointer == NULL || *tcamPointer >= PERIDOT_MAX_IGR_TCAM_ENTRY || tcamData == NULL)
	{
		MSD_DBG_ERROR(("tcamPointer is NULL or Bad tcamPointer value %u out of range OR tcamData is NULL.\n", (unsigned int)(*tcamPointer)));
        return MSD_BAD_PARAM;
	}

    /* Program Tuning register */
    op = Peridot_TCAM_GET_NEXT_ENTRY;
    tcamOpData.tcamEntry = *tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
    retVal = Peridot_tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal == MSD_NO_SUCH)
	{
		MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
		MSD_DBG_INFO(("Peridot_gtcamGetNextTCAMData Exit.\n"));
		return retVal;
	}	
	else if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    *tcamPointer = tcamOpData.tcamEntry;
	Peridot_getTcamHWData(&tcamOpData.tcamDataP,tcamData);
    MSD_DBG_INFO(("Peridot_gtcamGetNextTCAMData Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       tcamPointer - the tcam entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       tcamData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Peridot_gtcamFindEntry
(
    IN	MSD_QD_DEV     *dev,
    IN	MSD_U32        tcamPointer,
    OUT	PERIDOT_MSD_TCAM_DATA  *tcamData,
	OUT	MSD_BOOL		  *found
)
{
    MSD_STATUS       retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamFindEntry Called.\n"));

    /* check if the given pointer is valid */
	if (tcamPointer >= PERIDOT_MAX_IGR_TCAM_ENTRY || tcamData == NULL || found == NULL)
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL or found is NULL.\n", (unsigned int)tcamPointer));
        return MSD_BAD_PARAM;
	}

	*found = MSD_FALSE;

    /* Program Tuning register */
    op = Peridot_TCAM_GET_NEXT_ENTRY;
	tcamOpData.tcamEntry = (tcamPointer == 0 ? 0xff : (tcamPointer - 1));
    retVal = Peridot_tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal == MSD_NO_SUCH )
	{
		MSD_DBG_INFO(("Not found this entry, Peridot_gtcamFindEntry Exit.\n"));
		return MSD_OK;
	}	

	if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    } 

	if(tcamOpData.tcamEntry != tcamPointer)
	{
		MSD_DBG_INFO(("Not found this entry, Peridot_gtcamFindEntry Exit.\n"));
		return MSD_OK;
	}

	*found = MSD_TRUE;
	Peridot_getTcamHWData(&tcamOpData.tcamDataP,tcamData);
    MSD_DBG_INFO(("Peridot_gtcamFindEntry Exit.\n"));
    return MSD_OK;
}
/*******************************************************************************
* Peridot_gtcamSetMode
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*       Note: Do not chage the value while frames are flowing into this port,
*             You must first put the port in the Disable Port State, then change
*             TCAM mode, and then re-enable the port's Port State again.
*
* INPUTS:
*       portNum  - the logical port number
*       mode     - tcam mode to be set
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
*       None
*******************************************************************************/
MSD_STATUS Peridot_gtcamSetMode
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN PERIDOT_MSD_TCAM_MODE mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Peridot_gtcamSetMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    if (mode < PERIDOT_MSD_TCAM_MODE_DISABLE || mode > PERIDOT_MSD_TCAM_MODE_ENABLE_96)
    {
        MSD_DBG_ERROR(("Failed (Bad Tcam Mode).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, 0x0d, 0, 2, mode);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamSetMode Exit.\n"));
    return MSD_OK;
}
/*******************************************************************************
* Peridot_gtcamGetMode
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*
* INPUTS:
*       portNum  - the logical port number
*
* OUTPUTS:
*       mode     - get back tcam mode
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Peridot_gtcamGetMode
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    OUT PERIDOT_MSD_TCAM_MODE *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Peridot_gtcamGetMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the DefaultForward. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, 0x0d, 0, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read PERIDOT_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    *mode = data;

    MSD_DBG_INFO(("Peridot_gtcamGetMode Exit.\n"));
    return MSD_OK;
}
/*******************************************************************************
* Peridot_gtcamAddEntryPort
*
* DESCRIPTION:
*       Add a port to an existing tcam entry. If the tcam entry does not exist
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM (Peridot:0 ~ 255, Topaz:0~127)
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Peridot_gtcamAddEntryPort
(
    IN MSD_QD_DEV *dev,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
)
{
    MSD_STATUS retVal;
    MSD_BOOL found;
    PERIDOT_MSD_TCAM_DATA  tcamData;
    MSD_U8 hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Peridot_gtcamAddEntryPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    
    retVal = Peridot_gtcamFindEntry(dev, tcamPointer, &tcamData, &found);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_gtcamFindEntry in Peridot_gtcamAddEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (found == MSD_FALSE)
    {
        MSD_DBG_ERROR(("Do not find the tcam entry with tcamPointer:%d\n", (int)tcamPointer));
        return MSD_NO_SUCH;
    }

    tcamData.spvMask = tcamData.spvMask & (~(1 << hwPort));
    tcamData.spv = tcamData.spv & (~(1 << hwPort));

    retVal = Peridot_gtcamLoadEntry(dev, tcamPointer, &tcamData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_gtcamLoadEntry in Peridot_gtcamAddEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamAddEntryPort Exit.\n"));
    return retVal;
}
/*******************************************************************************
* Peridot_gtcamRemoveEntryPort
*
* DESCRIPTION:
*       Remove a port to an existing tcam entry. If the tcam entry does not exist
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM (Peridot:0 ~ 255, Topaz:0~127)
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Peridot_gtcamRemoveEntryPort
(
    IN MSD_QD_DEV *dev,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
)
{
    MSD_STATUS retVal;
    MSD_BOOL found;
    PERIDOT_MSD_TCAM_DATA  tcamData;
    MSD_U8 hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Peridot_gtcamRemoveEntryPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Peridot_gtcamFindEntry(dev, tcamPointer, &tcamData, &found);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_gtcamFindEntry in Peridot_gtcamRemoveEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (found == MSD_FALSE)
    {
        MSD_DBG_ERROR(("Do not find the tcam entry with tcamPointer:%d\n", (int)tcamPointer));
        return MSD_NO_SUCH;
    }

    tcamData.spvMask = tcamData.spvMask | (1 << hwPort);
    tcamData.spv = tcamData.spv & (~(1 << hwPort));

    retVal = Peridot_gtcamLoadEntry(dev, tcamPointer, &tcamData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_gtcamLoadEntry in Peridot_gtcamRemoveEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamRemoveEntryPort Exit.\n"));
    return retVal;
}


/*******************************************************************************
* Peridot_gtcamEgrFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for a particular port. A
*       Flush a single Egress TCAM entry command will write the same values to a 
*       Egress TCAM entry as a Flush All command, but it is done to the selected 
*		single egress TCAM entry of the selected single port only.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamEgrFlushEntry
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_EGR_OP_DATA     tcamOpData;
	MSD_U8			hwPort;

    MSD_DBG_INFO(("Peridot_gtcamEgrFlushEntry Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == MSD_INVALID_PORT)
    {
	MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)port));
	return MSD_BAD_PARAM;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0x3F) || (tcamPointer == 0))
    {
	MSD_DBG_ERROR(("Bad tcamPointer: %u. It should be within (0,63].\n", (unsigned int)tcamPointer));
        return MSD_BAD_PARAM;
    }

    /* Program Tuning register */
    op = Peridot_TCAM_FLUSH_ENTRY;
    tcamOpData.port = hwPort;
    tcamOpData.tcamEntry = tcamPointer;
    retVal = Peridot_tcamEgrOperationPerform(dev, op, &tcamOpData);
    if(retVal != MSD_OK)
    {
	MSD_DBG_ERROR(("Peridot_tcamEgrOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamEgrFlushEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gtcamEgrFlushEntryAllPorts
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for all switch ports. 
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamEgrFlushEntryAllPorts
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_EGR_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Peridot_gtcamEgrFlushEntryAllPorts Called.\n"));

    /* check if the given pointer is valid */
    if ((tcamPointer > 0x3F) || (tcamPointer == 0))
    {
	MSD_DBG_ERROR(("Bad tcamPointer: %u. It should be within (0,63].\n", (unsigned int)tcamPointer));
	return MSD_BAD_PARAM;
    }

    /* Program Tuning register */
    op = Peridot_TCAM_FLUSH_ENTRY;
	tcamOpData.port = 0x1F;
    tcamOpData.tcamEntry = tcamPointer;
    retVal = Peridot_tcamEgrOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
	MSD_DBG_ERROR(("Peridot_tcamEgrOperationPerform FLUSH_ENTRY for all returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamEgrFlushEntryAllPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gtcamEgrLoadEntry
*
* DESCRIPTION:
*       This routine loads a single egress TCAM entry for a specific port.
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*       tcamData    - Tcam entry Data
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
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamEgrLoadEntry
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer,
    IN  PERIDOT_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_EGR_OP_DATA     tcamOpData;
    MSD_U8			hwPort;

    MSD_DBG_INFO(("Peridot_gtcamEgrLoadEntry Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0x3F)|| (tcamPointer == 0) || (tcamData==NULL))
    {
        MSD_DBG_ERROR(("tcamPointer: %u out of range (0,63] OR tcamData is NULL.\n", (unsigned int)tcamPointer));
       return MSD_BAD_PARAM;
    }

    /* Program Tuning register */
    op = Peridot_TCAM_LOAD_ENTRY;
	tcamOpData.port = hwPort;
    tcamOpData.tcamEntry = tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
	Peridot_setTcamEgrHWData(tcamData,&tcamOpData.tcamDataP);
    retVal = Peridot_tcamEgrOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
	MSD_DBG_ERROR(("Peridot_tcamEgrOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gtcamEgrLoadEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gtcamEgrGetNextTCAMData
*
* DESCRIPTION:
*       This routine finds the next higher Egress TCAM Entry number that is valid 
*       (i.e.,any entry that is non-zero). The TCAM Entry register (bits 5:0) is
*       used as the Egress TCAM entry to start from. To find the lowest number
*       Egress TCAM Entry that is valid, start the Get Next operation with Egress
*       TCAM Entry set to 0x3F.
*
* INPUTS:
*		 port		 - switch port
*        tcamPointer - pointer to start search TCAM 
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamEgrGetNextTCAMData
(
    IN     MSD_QD_DEV     *dev,
    IN	   MSD_LPORT      port,
    INOUT  MSD_U32        *tcamPointer,
    OUT    PERIDOT_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_EGR_OP_DATA     tcamOpData;
    MSD_U8			hwPort;

    MSD_DBG_INFO(("Peridot_gtcamEgrGetNextTCAMData Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    /* check if the given pointer is valid */
    if (tcamPointer == NULL || (*tcamPointer > 0x3F) || (tcamData==NULL))
    {
        MSD_DBG_ERROR(("tcamPointer is NULL OR tcamPointer value: %u larger than 63 OR tcamData is NULL.\n", (unsigned int)(*tcamPointer)));
        return MSD_BAD_PARAM;
    }

    /* Program Tuning register */
    op = Peridot_TCAM_GET_NEXT_ENTRY;
    tcamOpData.port = hwPort;
    tcamOpData.tcamEntry = *tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
    retVal = Peridot_tcamEgrOperationPerform(dev,op, &tcamOpData);
    if(retVal == MSD_NO_SUCH)
    {
        MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
        MSD_DBG_INFO(("Peridot_gtcamEgrGetNextTCAMData Exit.\n"));
        return retVal;
    }	
    else if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_tcamEgrOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    *tcamPointer = tcamOpData.tcamEntry;
    Peridot_getTcamEgrHWData(&tcamOpData.tcamDataP,tcamData);

    MSD_DBG_INFO(("Peridot_gtcamEgrGetNextTCAMData Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gtcamEgrReadTCAMData
*
* DESCRIPTION:
*       This routine reads a single Egress TCAM entry for a specific port.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamEgrReadTCAMData
(
    IN  MSD_QD_DEV     *dev,
	IN	MSD_LPORT	  port,
    IN  MSD_U32        tcamPointer,
    OUT PERIDOT_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    PERIDOT_MSD_TCAM_OPERATION    op;
    PERIDOT_MSD_TCAM_EGR_OP_DATA     tcamOpData;
	MSD_U8			hwPort;

    MSD_DBG_INFO(("Peridot_gtcamEgrReadTCAMData Called.\n"));

	/* check if the given port is valid */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    /* check if the given pointer is valid */
	if ((tcamPointer > 0x3F) || (tcamPointer == 0) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("tcamPointer: %u out of range (0,63] OR tcamData is NULL.\n", (unsigned int)tcamPointer));
		return MSD_BAD_PARAM;
	}

    /* Program Tuning register */
    op = Peridot_TCAM_READ_ENTRY;
	tcamOpData.port = hwPort;
    tcamOpData.tcamEntry = tcamPointer;
    /*tcamOpData.tcamDataP = tcamData;*/
    retVal = Peridot_tcamEgrOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Peridot_tcamEgrOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	Peridot_getTcamEgrHWData(&tcamOpData.tcamDataP,tcamData);

    MSD_DBG_INFO(("Peridot_gtcamEgrReadTCAMData Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gtcamDump
*
* DESCRIPTION:
*       Finds all valid TCAM entries and print it out.
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamDump
(
    IN  MSD_QD_DEV *dev
)
{
    MSD_STATUS status;
    MSD_U32    tcamPointer;
    PERIDOT_MSD_TCAM_DATA  tcamData;

    MSD_DBG_INFO(("Peridot_gtcamDump Called.\n"));
    tcamPointer = 0xff;

    while ((status = Peridot_gtcamGetNextTCAMData(dev, &tcamPointer, &tcamData)) == MSD_OK)
    {
        MSG(("find the next valid TCAM entry number is %d\n", (int)tcamPointer));
        displayTcamData(&tcamData);
        if ((tcamPointer & 0x00ff) == 0xff)
        {
            break;
        }
    }

    MSD_DBG_INFO(("Peridot_gtcamDump Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Peridot_gtcamAdvConfig
*
* DESCRIPTION:
*       This routine sets tcam entry to match standard IPv4/IPv6, TCP/UDP packets
*       with the given header data and mask.
*
* INPUTS:
*       devNum  - physical devie number
*       pktType - Enumeration of TCAM mapping packet type
*       tcamPointer  - tcam entry number
*       tcamPointer2 - tcam entry2 number for cascade
*       keyMaskPtr   - mask pointer for TCAM entry key
*       keyPtr       - data pointer for TCAM entry key
*       maskPtr      - mask pointer for packet unit
*       patternPtr   - data pointer for packet unit
*       actionPtr    - action pionter
*
* OUTPUTS:
*       entry2Used - MSD_TRUE: two tcam entries been used, MSD_FALSE: only tcamPointer been used, tcamPointer2 not used.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gtcamAdvConfig
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_TCAM_PKT_TYPE pktType,
    IN  MSD_U32 tcamPointer,
    IN  MSD_U32 tcamPointer2,
    IN  PERIDOT_MSD_TCAM_ENT_KEY *keyMaskPtr,
    IN  PERIDOT_MSD_TCAM_ENT_KEY *keyPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *maskPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *patternPtr,
    IN  PERIDOT_MSD_TCAM_ENT_ACT *actionPtr,
    OUT MSD_BOOL *entry2Used
)
{
    MSD_STATUS retVal;
    PERIDOT_MSD_TCAM_DATA  tcamData;
    PERIDOT_MSD_TCAM_DATA  tcamData2;
    int i;

    MSD_DBG_INFO(("Peridot_gtcamAdvConfig Called.\n"));

    msdMemSet(&tcamData, 0, sizeof(PERIDOT_MSD_TCAM_DATA));
    msdMemSet(&tcamData2, 0, sizeof(PERIDOT_MSD_TCAM_DATA));

    /*key mask*/
    tcamData.frameType = keyPtr->frameType;
    tcamData.spv = keyPtr->spv;
    tcamData.ppri = keyPtr->ppri;
    tcamData.pvid = keyPtr->pvid;
    tcamData.frameTypeMask = keyMaskPtr->frameType;
    tcamData.spvMask = keyMaskPtr->spv;
    tcamData.ppriMask = keyMaskPtr->ppri;
    tcamData.pvidMask = keyMaskPtr->pvid;

    /*DA SA VlanTag*/
    for (i = 0; i < 6; i++)
    {
        tcamData.frameOctet[i] = patternPtr->da.arEther[i];
        tcamData.frameOctet[i + 6] = patternPtr->sa.arEther[i];
        tcamData.frameOctetMask[i] = maskPtr->da.arEther[i];
        tcamData.frameOctetMask[i + 6] = maskPtr->sa.arEther[i];
    }
    tcamData.frameOctet[12] = (patternPtr->vlanTag >> 24) & 0xff;
    tcamData.frameOctet[13] = (patternPtr->vlanTag >> 16) & 0xff;
    tcamData.frameOctet[14] = (patternPtr->vlanTag >> 8) & 0xff;
    tcamData.frameOctet[15] = (patternPtr->vlanTag) & 0xff;
    tcamData.frameOctetMask[12] = (maskPtr->vlanTag >> 24) & 0xff;
    tcamData.frameOctetMask[13] = (maskPtr->vlanTag >> 16) & 0xff;
    tcamData.frameOctetMask[14] = (maskPtr->vlanTag >> 8) & 0xff;
    tcamData.frameOctetMask[15] = (maskPtr->vlanTag) & 0xff;

    tcamData.frameOctet[16] = (patternPtr->etherType >> 8) & 0xff;
    tcamData.frameOctet[17] = (patternPtr->etherType) & 0xff;
    tcamData.frameOctetMask[16] = (maskPtr->etherType >> 8) & 0xff;
    tcamData.frameOctetMask[17] = maskPtr->etherType & 0xff;

    /*Protcol setting*/
    switch (pktType)
    {
    case MSD_TCAM_TYPE_IPV4_TCP:
        mappingIpv4ToTcam(&tcamData, &(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4));
        mappingTcpOverIpv4ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv4Tcp.tcp), &(maskPtr->pktUnit.ipv4Tcp.tcp));
        break;
    case MSD_TCAM_TYPE_IPV4_UDP:
        mappingIpv4ToTcam(&tcamData, &(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4));
        mappingUdpToTcam(38, &tcamData, &(patternPtr->pktUnit.ipv4Udp.udp), &(maskPtr->pktUnit.ipv4Udp.udp));
        break;
    case MSD_TCAM_TYPE_IPV6_TCP:
        mappingIpv6ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6));
        mappingTcpOverIpv6ToTcam(&tcamData2, &(patternPtr->pktUnit.ipv6Tcp.tcp), &(maskPtr->pktUnit.ipv6Tcp.tcp));
        break;
    case MSD_TCAM_TYPE_IPV6_UDP:
        mappingIpv6ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6));
        mappingUdpToTcam(10, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.udp), &(maskPtr->pktUnit.ipv6Udp.udp));
        break;
    default:
        MSD_DBG_ERROR(("Input MSD_TCAM_PKT_TYPE error. \n"));
        return MSD_BAD_PARAM;
    }
    
    if (checkEntry2Used(&tcamData2) == MSD_TRUE)
    {
        tcamData.continu = 1;
        tcamData.nextId = (MSD_U8)tcamPointer2;
        tcamData2.pvid = (MSD_U16)tcamPointer2;
        tcamData2.pvidMask = 0xff;
        mappingActionToTcam(&tcamData2, actionPtr);

        retVal = Peridot_gtcamLoadEntry(dev, tcamPointer, &tcamData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Peridot_gtcamLoadEntry for tcamData returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
        retVal = Peridot_gtcamLoadEntry(dev, tcamPointer2, &tcamData2);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Peridot_gtcamLoadEntry for tcamData2 returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
        *entry2Used = MSD_TRUE;
    }
    else
    {
        mappingActionToTcam(&tcamData, actionPtr);
        retVal = Peridot_gtcamLoadEntry(dev, tcamPointer, &tcamData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Peridot_gtcamLoadEntry for tcamData only returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
        *entry2Used = MSD_FALSE;
    }

    MSD_DBG_INFO(("Peridot_gtcamAdvConfig Exit.\n"));
    return MSD_OK;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Peridot_setTcamHWData
(
	IN	PERIDOT_MSD_TCAM_DATA	*iData,
	OUT	PERIDOT_MSD_TCAM_DATA_HW	*oData
)
{
	int i;
	
	msdMemSet(oData,0,sizeof(PERIDOT_MSD_TCAM_DATA_HW));

	/*key*/
	MSD_BF_SET(oData->pg0.frame[0],iData->frameTypeMask, 14, 2);
	MSD_BF_SET(oData->pg0.frame[0],iData->frameType, 6, 2);
	MSD_BF_SET(oData->pg0.frame[0],((iData->spvMask&0x700)>>8), 8, 3);
	MSD_BF_SET(oData->pg0.frame[0],((iData->spv&0x700)>>8), 0, 3);

	MSD_BF_SET(oData->pg0.frame[1],(iData->spvMask&0xff), 8, 8);
	MSD_BF_SET(oData->pg0.frame[1],(iData->spv&0xff), 0, 8);

	MSD_BF_SET(oData->pg0.frame[2],iData->ppriMask, 12, 4);
	MSD_BF_SET(oData->pg0.frame[2],iData->ppri, 4, 4);
	MSD_BF_SET(oData->pg0.frame[2],((iData->pvidMask&0xf00)>>8), 8, 4);
	MSD_BF_SET(oData->pg0.frame[2],((iData->pvid&0xf00)>>8), 0, 4);

	MSD_BF_SET(oData->pg0.frame[3],(iData->pvidMask&0xff), 8, 8);
	MSD_BF_SET(oData->pg0.frame[3],(iData->pvid&0xff), 0, 8);

	/*Page 0 (0~21) bytes*/
	for(i=4; i<26; i++)
	{
		MSD_BF_SET(oData->pg0.frame[i],iData->frameOctetMask[i-4],8,8);
		MSD_BF_SET(oData->pg0.frame[i],iData->frameOctet[i-4],0,8);
	}

	/*Page 1 (22~47) bytes*/
	for(i=0; i<26; i++)
	{
		MSD_BF_SET(oData->pg1.frame[i],iData->frameOctetMask[22+i],8,8);
		MSD_BF_SET(oData->pg1.frame[i],iData->frameOctet[22+i],0,8);
	}

	/*Action*/
	MSD_BF_SET(oData->pg2.frame[0],iData->continu, 15, 1);
	MSD_BF_SET(oData->pg2.frame[0],iData->interrupt, 14, 1);
	MSD_BF_SET(oData->pg2.frame[0],iData->IncTcamCtr, 13, 1);
	MSD_BF_SET(oData->pg2.frame[0],(MSD_U16)(iData->vidOverride), 12, 1);
	MSD_BF_SET(oData->pg2.frame[0],iData->vidData, 0, 12);

	MSD_BF_SET(oData->pg2.frame[1],iData->nextId, 8, 8);
	MSD_BF_SET(oData->pg2.frame[1],(MSD_U16)(iData->qpriOverride), 7, 1);
	MSD_BF_SET(oData->pg2.frame[1],iData->qpriData, 4, 3);
	MSD_BF_SET(oData->pg2.frame[1],(MSD_U16)(iData->fpriOverride), 3, 1);
	MSD_BF_SET(oData->pg2.frame[1],iData->fpriData, 0, 3);

	MSD_BF_SET(oData->pg2.frame[2],iData->dpvSF, 11, 1);
	MSD_BF_SET(oData->pg2.frame[2],(MSD_U16)(iData->dpvData), 0, 11);

	/*
	MSD_BF_SET(oData->pg2.frame[3], 0, 0, 16);
	*/
	oData->pg2.frame[3] = 0;

	MSD_BF_SET(oData->pg2.frame[4],iData->dpvMode, 14, 2);
	MSD_BF_SET(oData->pg2.frame[4],iData->colorMode, 12, 2);
	MSD_BF_SET(oData->pg2.frame[4],(MSD_U16)(iData->vtuPageOverride), 11, 1);
	MSD_BF_SET(oData->pg2.frame[4],iData->vtuPage, 10, 1);
	MSD_BF_SET(oData->pg2.frame[4],iData->unKnownFilter, 8, 2);
	MSD_BF_SET(oData->pg2.frame[4],iData->egActPoint, 0, 6);

	MSD_BF_SET(oData->pg2.frame[5],(MSD_U16)(iData->ldBalanceOverride), 15, 1);
	MSD_BF_SET(oData->pg2.frame[5],iData->ldBalanceData, 12, 3);
	MSD_BF_SET(oData->pg2.frame[5],(MSD_U16)(iData->DSCPOverride), 6, 1);
	MSD_BF_SET(oData->pg2.frame[5],iData->DSCP, 0, 6);

	MSD_BF_SET(oData->pg2.frame[6],(MSD_U16)(iData->factionOverride), 15, 1);
	MSD_BF_SET(oData->pg2.frame[6],iData->factionData, 0, 15);
	
	return MSD_OK;
}

static MSD_STATUS Peridot_getTcamHWData
(
	IN	PERIDOT_MSD_TCAM_DATA_HW	*iData,
	OUT	PERIDOT_MSD_TCAM_DATA	*oData
)
{
	int i;
	/*key*/
	oData->frameTypeMask = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0],14, 2));
	oData->frameType = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0],6, 2));

    oData->spvMask = (MSD_BF_GET(iData->pg0.frame[0],8, 3) << 8) | MSD_BF_GET(iData->pg0.frame[1],8, 8);
	oData->spv = (MSD_BF_GET(iData->pg0.frame[0],0, 3) << 8) | MSD_BF_GET(iData->pg0.frame[1], 0, 8);

	oData->ppriMask = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[2],12, 4));
	oData->ppri = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[2],4, 4));

	oData->pvidMask = (MSD_BF_GET(iData->pg0.frame[2], 8, 4) << 8) | MSD_BF_GET(iData->pg0.frame[3],8, 8);

	oData->pvid = (MSD_BF_GET(iData->pg0.frame[2], 0, 4) << 8) | MSD_BF_GET(iData->pg0.frame[3], 0, 8);

	/*Page 0 (0~21) bytes*/
	for(i=4; i<26; i++)
	{
		oData->frameOctetMask[i-4] = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[i],8, 8));
		oData->frameOctet[i-4] = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[i],0,8));
	}

	/*Page 1 (22~47) bytes*/
	for(i=0; i<26; i++)
	{
		oData->frameOctetMask[22+i] = (MSD_U8)(MSD_BF_GET(iData->pg1.frame[i],8,8));
		oData->frameOctet[22+i] = (MSD_U8)(MSD_BF_GET(iData->pg1.frame[i],0,8));
	}

	/*Action*/
	oData->continu = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[0], 15, 1));
	oData->interrupt = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[0], 14, 1));
	oData->IncTcamCtr = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[0], 13, 1));
	oData->vidOverride = MSD_BF_GET(iData->pg2.frame[0], 12, 1);
	oData->vidData = MSD_BF_GET(iData->pg2.frame[0], 0, 12);

	oData->nextId = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 8, 8));
	oData->qpriOverride = MSD_BF_GET(iData->pg2.frame[1], 7, 1);
	oData->qpriData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 4, 3));
	oData->fpriOverride = MSD_BF_GET(iData->pg2.frame[1], 3, 1);
	oData->fpriData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 0, 3));

	oData->dpvSF = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[2], 11, 1));
	oData->dpvData = MSD_BF_GET(iData->pg2.frame[2], 0, 11);

	oData->dpvMode = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 14, 2));
	oData->colorMode = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 12, 2));
	oData->vtuPageOverride = MSD_BF_GET(iData->pg2.frame[4], 11, 1);
	oData->vtuPage = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 10, 1));
	oData->unKnownFilter = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 8, 2));
	oData->egActPoint = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 0, 6));

	oData->ldBalanceOverride = MSD_BF_GET(iData->pg2.frame[5], 15, 1);
	oData->ldBalanceData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[5], 12, 3));
	oData->DSCPOverride = MSD_BF_GET(iData->pg2.frame[5], 6, 1);
	oData->DSCP = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[5], 0, 6));

	oData->factionOverride = MSD_BF_GET(iData->pg2.frame[6], 15, 1);
	oData->factionData = MSD_BF_GET(iData->pg2.frame[6], 0, 15);

	return MSD_OK;
}

static MSD_STATUS Peridot_setTcamEgrHWData
(
	IN	PERIDOT_MSD_TCAM_EGR_DATA	*iData,
	OUT	PERIDOT_MSD_TCAM_EGR_DATA_HW	*oData
)
{
	msdMemSet(oData,0,sizeof(PERIDOT_MSD_TCAM_EGR_DATA_HW));

	MSD_BF_SET(oData->frame[0],(MSD_U16)(iData->frameModeOverride), 14, 1);
	MSD_BF_SET(oData->frame[0],iData->frameMode, 12, 2);
	MSD_BF_SET(oData->frame[0],(MSD_U16)(iData->tagModeOverride), 10, 1);
	MSD_BF_SET(oData->frame[0],iData->tagMode, 8, 2);
	MSD_BF_SET(oData->frame[0],iData->daMode, 4, 2);
	MSD_BF_SET(oData->frame[0],iData->saMode, 0, 3);

	MSD_BF_SET(oData->frame[1],(MSD_U16)(iData->egVidModeOverride), 14, 1);
	MSD_BF_SET(oData->frame[1],iData->egVidMode, 12, 2);
	MSD_BF_SET(oData->frame[1],iData->egVidData, 0, 12);

	MSD_BF_SET(oData->frame[2],iData->egDSCPMode, 14, 2);
	MSD_BF_SET(oData->frame[2],iData->egDSCP, 8, 6);
	MSD_BF_SET(oData->frame[2],(MSD_U16)(iData->egfpriModeOverride), 6, 1);
	MSD_BF_SET(oData->frame[2],iData->egfpriMode, 4, 2);
	MSD_BF_SET(oData->frame[2],iData->egEC, 3, 1);
	MSD_BF_SET(oData->frame[2],iData->egFPRI, 0, 3);
	
	return MSD_OK;
}

static MSD_STATUS Peridot_getTcamEgrHWData
(
	IN	PERIDOT_MSD_TCAM_EGR_DATA_HW	*iData,
	OUT	PERIDOT_MSD_TCAM_EGR_DATA	*oData
)
{
	oData->frameModeOverride = MSD_BF_GET(iData->frame[0], 14, 1);
	oData->frameMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 12, 2));
	oData->tagModeOverride = MSD_BF_GET(iData->frame[0], 10, 1);
	oData->tagMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 8, 2));
	oData->daMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 4, 2));
	oData->saMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 0, 3));

	oData->egVidModeOverride = MSD_BF_GET(iData->frame[1], 14, 1);
	oData->egVidMode = (MSD_U8)(MSD_BF_GET(iData->frame[1], 12, 2));
	oData->egVidData = MSD_BF_GET(iData->frame[1], 0, 12);

	oData->egDSCPMode = (MSD_U8)(MSD_BF_GET(iData->frame[2], 14, 2));
	oData->egDSCP = (MSD_U8)(MSD_BF_GET(iData->frame[2], 8, 6));
	oData->egfpriModeOverride = MSD_BF_GET(iData->frame[2], 6, 1);
	oData->egfpriMode = (MSD_U8)(MSD_BF_GET(iData->frame[2], 4, 2));
	oData->egEC = (MSD_U8)(MSD_BF_GET(iData->frame[2], 3, 1));
	oData->egFPRI = (MSD_U8)(MSD_BF_GET(iData->frame[2], 0, 3));

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamSetPage0Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<0x1c; i++)
	{
		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataP->pg0.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamSetPage1Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<0x1c; i++)
	{
		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataP->pg1.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamSetPage2Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<9; i++)
	{
		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataP->pg2.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamSetPage3Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_EGR_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<5; i++)
	{
		retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataP->frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamGetPage0Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;


	for(i=2; i<0x1c; i++)
	{
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataP->pg0.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamGetPage1Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;


	for(i=2; i<0x1c; i++)
	{
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataP->pg1.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamGetPage2Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<9; i++)
	{
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataP->pg2.frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_tcamGetPage3Data(MSD_QD_DEV *dev, PERIDOT_MSD_TCAM_EGR_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal;    /* Functions return value */
	int i;

	for(i=2; i<5; i++)
	{
		retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataP->frame[i - 2]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Peridot_waitTcamReady(MSD_QD_DEV           *dev)
{
    MSD_STATUS       retVal;    /* Functions return value */

    MSD_U16          data;     /* temporary Data storage */
    data = 1;
    while(data == 1)
    {
		retVal = msdGetAnyRegField(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_OPERATION, 15, 1, &data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
    }
    return MSD_OK;
}

/*******************************************************************************
* Peridot_tcamOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       tcamOp   - The tcam operation
*       tcamData - address and data to be written into TCAM
*
* OUTPUTS:
*       tcamData - data read from TCAM pointed by address
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Peridot_tcamOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    PERIDOT_MSD_TCAM_OPERATION   tcamOp,
    INOUT PERIDOT_MSD_TCAM_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Peridot_waitTcamReady(dev);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamOp)
	{
		case Peridot_TCAM_FLUSH_ALL:
		{
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (tcamOp << 12));
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Peridot_TCAM_FLUSH_ENTRY:
		{
			/* Wait until the tcam in ready. */
										 retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Peridot_TCAM_LOAD_ENTRY:
		/*    case Peridot_TCAM_PURGE_ENTRY: */
		{
			/* load Page 2 */
			/* Wait until the tcam in ready. */
										retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamSetPage2Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (Peridot_TCAM_LOAD_ENTRY << 12) | (2 << 10) | opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* load Page 1 */
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamSetPage1Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			data = 0;
			data = (MSD_U16)((1 << 15) | (Peridot_TCAM_LOAD_ENTRY << 12) | (1 << 10) | opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			/* load Page 0 */
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamSetPage0Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (Peridot_TCAM_LOAD_ENTRY << 12) | (0 << 10) | opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

		}
		break;

		case Peridot_TCAM_GET_NEXT_ENTRY:
		{
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (opData->tcamEntry)) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			if ((data&0xff)==0xff)
			{
			MSD_U16 data1;
			retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_P0_KEYS_1, &data1);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			if(data1==0x00ff)
			{
				/* No higher valid TCAM entry */
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return MSD_NO_SUCH;
			}
			else
			{
				/* The highest valid TCAM entry found*/
			}
			}

			/* Get next entry and read the entry */
			opData->tcamEntry = data&0xff;

		}
		case Peridot_TCAM_READ_ENTRY:
		{
			tcamOp = Peridot_TCAM_READ_ENTRY;

			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			/* Read page 0 */
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (0 << 10) | opData->tcamEntry) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamGetPage0Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			/* Read page 1 */
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (1 << 10) | opData->tcamEntry) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamGetPage1Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			/* Read page 2 */
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (2 << 10) | opData->tcamEntry) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamGetPage2Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		default:
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}


static MSD_STATUS Peridot_tcamEgrOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    PERIDOT_MSD_TCAM_OPERATION   tcamOp,
    INOUT PERIDOT_MSD_TCAM_EGR_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Peridot_waitTcamReady(dev);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamOp)
	{
		case Peridot_TCAM_FLUSH_ENTRY:
		{
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_PORT, opData->port);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (3<<10) | opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_PORT, 0);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Peridot_TCAM_LOAD_ENTRY:
		{
			/* load Page 2 */
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_PORT, opData->port);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamSetPage3Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((1 << 15) | (Peridot_TCAM_LOAD_ENTRY << 12) | (3 << 10) | (opData->tcamEntry));
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

		}
		break;

		case Peridot_TCAM_GET_NEXT_ENTRY:
		{
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_PORT, opData->port);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			data = (MSD_U16)((tcamOp << 12) | (3 << 10)| (opData->tcamEntry)) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (3 << 10)| (opData->tcamEntry)) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			if ((data&0x3f)==0x3f)
			{
			MSD_U16 data1,data2,data3;
			retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_ACTION_1, &data1);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_ACTION_2, &data2);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_ACTION_3, &data3);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			if(data1==0x0 && data2==0x0 && data3==0x0)
			{
				/* No higher valid TCAM entry */
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return MSD_NO_SUCH;
			}
			else
			{
				/* The highest valid TCAM entry found*/
			}
			}

			/* Get next entry and read the entry */
			opData->tcamEntry = data&0xff;

		}

		case Peridot_TCAM_READ_ENTRY:
		{
			tcamOp = Peridot_TCAM_READ_ENTRY;

			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum,  PERIDOT_TCAM_DEV_ADDR, PERIDOT_QD_REG_TCAM_EGR_PORT, opData->port);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = 0;
			/* Read page 0 */
			data = (MSD_U16)((1 << 15) | (tcamOp << 12) | (3 << 10) | opData->tcamEntry) ;
			retVal = msdSetAnyReg(dev->devNum, PERIDOT_TCAM_DEV_ADDR,PERIDOT_QD_REG_TCAM_OPERATION,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Peridot_tcamGetPage3Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Peridot_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		default:
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}

static void displayTcamData(PERIDOT_MSD_TCAM_DATA *tcamData)
{
	int i;

	MSG(("----- Key & Mask -----\n"));

	MSG(("frameType         : %04x\n", tcamData->frameType));
	MSG(("frameTypeMask     : %04x\n", tcamData->frameTypeMask));
	MSG(("spv               : %04x\n", tcamData->spv));
	MSG(("spvMask           : %04x\n", tcamData->spvMask));
	MSG(("ppri              : %04x\n", tcamData->ppri));
	MSG(("ppriMask          : %04x\n", tcamData->ppriMask));
	MSG(("pvid              : %04x\n", tcamData->pvid));
	MSG(("pvidMask          : %04x\n", tcamData->pvidMask));

	MSG(("----- Ethernet Frame Content -----\n"));

	for (i = 0; i<48; i++)
	{
		MSG(("frame data Octet %2d         : %4x\n", i, tcamData->frameOctet[i]));
		MSG(("frame data OctetMask %2d     : %4x\n", i, tcamData->frameOctetMask[i]));
	}

	MSG(("----- Ingress Action -----\n"));

	MSG(("continu           : %04x\n", tcamData->continu));
	MSG(("interrupt         : %04x\n", tcamData->interrupt));
	MSG(("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr));
	MSG(("vidOverride       : %04x\n", tcamData->vidOverride));
	MSG(("vidData           : %04x\n", tcamData->vidData));

	MSG(("nextId            : %04x\n", tcamData->nextId));
	MSG(("qpriOverride      : %04x\n", tcamData->qpriOverride));
	MSG(("qpriData          : %04x\n", tcamData->qpriData));
	MSG(("fpriOverride      : %04x\n", tcamData->fpriOverride));
	MSG(("fpriData          : %04x\n", tcamData->fpriData));

	MSG(("dpvSF             : %04x\n", tcamData->dpvSF));
	MSG(("dpvData           : %04x\n", (unsigned int)tcamData->dpvData));
	MSG(("dpvMode           : %04x\n", tcamData->dpvMode));

	MSG(("unKnownFilter     : %04x\n", tcamData->unKnownFilter));

	MSG(("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride));
	MSG(("ldBalanceData     : %04x\n", tcamData->ldBalanceData));

	MSG(("factionOverride   : %04x\n", tcamData->factionOverride));
	MSG(("factionData       : %04x\n", tcamData->factionData));
#ifdef PERIDOT_BUILD_IN
	MSG(("colorMode         : %04x\n", tcamData->colorMode));
	MSG(("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride));
	MSG(("vtuPage           : %04x\n", tcamData->vtuPage));
	MSG(("egActPoint        : %04x\n", tcamData->egActPoint));
	MSG(("DSCPOverride      : %04x\n", tcamData->DSCPOverride));
	MSG(("DSCP              : %04x\n", tcamData->DSCP));
#endif

}

static MSD_BOOL checkEntry2Used(PERIDOT_MSD_TCAM_DATA *tcamData2)
{
    MSD_BOOL flag = MSD_FALSE;
    int i;

    for (i = 0; i < 48; i++)
    {
        if (tcamData2->frameOctetMask[i] != 0)
        {
            flag = MSD_TRUE;
            break;
        }
    }
    return flag;
}
static void mappingTcpOverIpv4ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_TCP_UNT *pTcpData, MSD_TCAM_ENT_TCP_UNT *pTcpMask)
{
    tcamData->frameOctet[38] = (pTcpData->srcPort >> 8) & 0xff;
    tcamData->frameOctet[39] = pTcpData->srcPort & 0xff;
    tcamData->frameOctet[40] = (pTcpData->destPort >> 8) & 0xff;
    tcamData->frameOctet[41] = pTcpData->destPort & 0xff;
    tcamData->frameOctet[42] = (pTcpData->seqNum >> 24) & 0xff;
    tcamData->frameOctet[43] = (pTcpData->seqNum >> 16) & 0xff;
    tcamData->frameOctet[44] = (pTcpData->seqNum >> 8) & 0xff;
    tcamData->frameOctet[45] = pTcpData->seqNum & 0xff;
    tcamData->frameOctet[46] = (pTcpData->ackNum >> 24) & 0xff;
    tcamData->frameOctet[47] = (pTcpData->ackNum >> 16) & 0xff;
    tcamData2->frameOctet[0] = (pTcpData->ackNum >> 8) & 0xff;
    tcamData2->frameOctet[1] = (pTcpData->ackNum) & 0xff;
    tcamData2->frameOctet[2] = (pTcpData->offset << 4);
    tcamData2->frameOctet[3] = (pTcpData->flags) & 0x3f;
    tcamData2->frameOctet[4] = (pTcpData->windowSize >> 8) & 0xff;
    tcamData2->frameOctet[5] = (pTcpData->windowSize) & 0xff;
    tcamData2->frameOctet[6] = (pTcpData->chkSum >> 8) & 0xff;
    tcamData2->frameOctet[7] = (pTcpData->chkSum) & 0xff;
    tcamData2->frameOctet[8] = (pTcpData->urgPtr >> 8) & 0xff;
    tcamData2->frameOctet[9] = (pTcpData->urgPtr) & 0xff;

    tcamData->frameOctetMask[38] = (pTcpMask->srcPort >> 8) & 0xff;
    tcamData->frameOctetMask[39] = pTcpMask->srcPort & 0xff;
    tcamData->frameOctetMask[40] = (pTcpMask->destPort >> 8) & 0xff;
    tcamData->frameOctetMask[41] = pTcpMask->destPort & 0xff;
    tcamData->frameOctetMask[42] = (pTcpMask->seqNum >> 24) & 0xff;
    tcamData->frameOctetMask[43] = (pTcpMask->seqNum >> 16) & 0xff;
    tcamData->frameOctetMask[44] = (pTcpMask->seqNum >> 8) & 0xff;
    tcamData->frameOctetMask[45] = pTcpMask->seqNum & 0xff;
    tcamData->frameOctetMask[46] = (pTcpMask->ackNum >> 24) & 0xff;
    tcamData->frameOctetMask[47] = (pTcpMask->ackNum >> 16) & 0xff;
    tcamData2->frameOctetMask[0] = (pTcpMask->ackNum >> 8) & 0xff;
    tcamData2->frameOctetMask[1] = (pTcpMask->ackNum) & 0xff;
    tcamData2->frameOctetMask[2] = (pTcpMask->offset << 4);
    tcamData2->frameOctetMask[3] = (pTcpMask->flags) & 0x3f;
    tcamData2->frameOctetMask[4] = (pTcpMask->windowSize >> 8) & 0xff;
    tcamData2->frameOctetMask[5] = (pTcpMask->windowSize) & 0xff;
    tcamData2->frameOctetMask[6] = (pTcpMask->chkSum >> 8) & 0xff;
    tcamData2->frameOctetMask[7] = (pTcpMask->chkSum) & 0xff;
    tcamData2->frameOctetMask[8] = (pTcpMask->urgPtr >> 8) & 0xff;
    tcamData2->frameOctetMask[9] = (pTcpMask->urgPtr) & 0xff;
}
static void mappingTcpOverIpv6ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_TCP_UNT *pTcpData, MSD_TCAM_ENT_TCP_UNT *pTcpMask)
{
    tcamData2->frameOctet[10] = (pTcpData->srcPort >> 8) & 0xff;
    tcamData2->frameOctet[11] = pTcpData->srcPort & 0xff;
    tcamData2->frameOctet[12] = (pTcpData->destPort >> 8) & 0xff;
    tcamData2->frameOctet[13] = pTcpData->destPort & 0xff;
    tcamData2->frameOctet[14] = (pTcpData->seqNum >> 24) & 0xff;
    tcamData2->frameOctet[15] = (pTcpData->seqNum >> 16) & 0xff;
    tcamData2->frameOctet[16] = (pTcpData->seqNum >> 8) & 0xff;
    tcamData2->frameOctet[17] = pTcpData->seqNum & 0xff;
    tcamData2->frameOctet[18] = (pTcpData->ackNum >> 24) & 0xff;
    tcamData2->frameOctet[19] = (pTcpData->ackNum >> 16) & 0xff;
    tcamData2->frameOctet[20] = (pTcpData->ackNum >> 8) & 0xff;
    tcamData2->frameOctet[21] = (pTcpData->ackNum) & 0xff;
    tcamData2->frameOctet[22] = (pTcpData->offset << 4);
    tcamData2->frameOctet[23] = (pTcpData->flags) & 0x3f;
    tcamData2->frameOctet[24] = (pTcpData->windowSize >> 8) & 0xff;
    tcamData2->frameOctet[25] = (pTcpData->windowSize) & 0xff;
    tcamData2->frameOctet[26] = (pTcpData->chkSum >> 8) & 0xff;
    tcamData2->frameOctet[27] = (pTcpData->chkSum) & 0xff;
    tcamData2->frameOctet[28] = (pTcpData->urgPtr >> 8) & 0xff;
    tcamData2->frameOctet[29] = (pTcpData->urgPtr) & 0xff;

    tcamData2->frameOctetMask[10] = (pTcpMask->srcPort >> 8) & 0xff;
    tcamData2->frameOctetMask[11] = pTcpMask->srcPort & 0xff;
    tcamData2->frameOctetMask[12] = (pTcpMask->destPort >> 8) & 0xff;
    tcamData2->frameOctetMask[13] = pTcpMask->destPort & 0xff;
    tcamData2->frameOctetMask[14] = (pTcpMask->seqNum >> 24) & 0xff;
    tcamData2->frameOctetMask[15] = (pTcpMask->seqNum >> 16) & 0xff;
    tcamData2->frameOctetMask[16] = (pTcpMask->seqNum >> 8) & 0xff;
    tcamData2->frameOctetMask[17] = pTcpMask->seqNum & 0xff;
    tcamData2->frameOctetMask[18] = (pTcpMask->ackNum >> 24) & 0xff;
    tcamData2->frameOctetMask[19] = (pTcpMask->ackNum >> 16) & 0xff;
    tcamData2->frameOctetMask[20] = (pTcpMask->ackNum >> 8) & 0xff;
    tcamData2->frameOctetMask[21] = (pTcpMask->ackNum) & 0xff;
    tcamData2->frameOctetMask[22] = (pTcpMask->offset << 4);
    tcamData2->frameOctetMask[23] = (pTcpMask->flags) & 0x3f;
    tcamData2->frameOctetMask[24] = (pTcpMask->windowSize >> 8) & 0xff;
    tcamData2->frameOctetMask[25] = (pTcpMask->windowSize) & 0xff;
    tcamData2->frameOctetMask[26] = (pTcpMask->chkSum >> 8) & 0xff;
    tcamData2->frameOctetMask[27] = (pTcpMask->chkSum) & 0xff;
    tcamData2->frameOctetMask[28] = (pTcpMask->urgPtr >> 8) & 0xff;
    tcamData2->frameOctetMask[29] = (pTcpMask->urgPtr) & 0xff;
}
static void mappingIpv6ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_DATA *tcamData2, MSD_TCAM_ENT_IPV6_UNT *pIpv6, MSD_TCAM_ENT_IPV6_UNT *pIpv6Mask)
{
    int i;
    MSD_U16 data;

    /*Data for tcamData*/
    data = (pIpv6->version << 12) | (pIpv6->tc << 4) | (pIpv6->flowLbl >> 16);
    tcamData->frameOctet[18] = (data >> 8) & 0xff;
    tcamData->frameOctet[19] = data & 0xff;
    tcamData->frameOctet[20] = (pIpv6->flowLbl >> 8) & 0xff;
    tcamData->frameOctet[21] = (pIpv6->flowLbl) & 0xff;
    tcamData->frameOctet[22] = (pIpv6->payloadLen >> 8) & 0xff;
    tcamData->frameOctet[23] = (pIpv6->payloadLen) & 0xff;
    tcamData->frameOctet[24] = pIpv6->nextHdr;
    tcamData->frameOctet[25] = pIpv6->hopLimit;
    for (i = 0; i < 8; i++)
    {
        tcamData->frameOctet[26 + 2*i] = (pIpv6->sip[i] >> 8) & 0xff;
        tcamData->frameOctet[26 + 2 * i + 1] = (pIpv6->sip[i]) & 0xff;
    }
    for (i = 0; i < 3; i++)
    {
        tcamData->frameOctet[42 + 2 * i] = (pIpv6->dip[i] >> 8) & 0xff;
        tcamData->frameOctet[42 + 2 * i + 1] = (pIpv6->dip[i]) & 0xff;
    }
    /*Data mask for tcamData*/
    data = (pIpv6Mask->version << 12) | (pIpv6Mask->tc << 4) | (pIpv6Mask->flowLbl >> 16);
    tcamData->frameOctetMask[18] = (data >> 8) & 0xff;
    tcamData->frameOctetMask[19] = data & 0xff;
    tcamData->frameOctetMask[20] = (pIpv6Mask->flowLbl >> 8) & 0xff;
    tcamData->frameOctetMask[21] = (pIpv6Mask->flowLbl) & 0xff;
    tcamData->frameOctetMask[22] = (pIpv6Mask->payloadLen >> 8) & 0xff;
    tcamData->frameOctetMask[23] = (pIpv6Mask->payloadLen) & 0xff;
    tcamData->frameOctetMask[24] = pIpv6Mask->nextHdr;
    tcamData->frameOctetMask[25] = pIpv6Mask->hopLimit;
    for (i = 0; i < 8; i++)
    {
        tcamData->frameOctetMask[26 + 2 * i] = (pIpv6Mask->sip[i] >> 8) & 0xff;
        tcamData->frameOctetMask[26 + 2 * i + 1] = (pIpv6Mask->sip[i]) & 0xff;
    }
    for (i = 0; i < 3; i++)
    {
        tcamData->frameOctetMask[42 + 2 * i] = (pIpv6Mask->dip[i] >> 8) & 0xff;
        tcamData->frameOctetMask[42 + 2 * i + 1] = (pIpv6Mask->dip[i]) & 0xff;
    }

    for (i = 0; i < 5; i++)
    {
        tcamData2->frameOctet[0 + 2 * i] = (pIpv6->dip[3 + i] >> 8) & 0xff;
        tcamData2->frameOctet[0 + 2 * i + 1] = (pIpv6->dip[3 + i]) & 0xff;
        tcamData2->frameOctetMask[0 + 2 * i] = (pIpv6Mask->dip[3 + i] >> 8) & 0xff;
        tcamData2->frameOctetMask[0 + 2 * i + 1] = (pIpv6Mask->dip[3 + i]) & 0xff;
    }
}
static void mappingIpv4ToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, MSD_TCAM_ENT_IPV4_UNT *pIpv4Data, MSD_TCAM_ENT_IPV4_UNT *pIpv4Mask)
{
    int i;

    tcamData->frameOctet[18] = (pIpv4Data->version << 4) | pIpv4Data->ihl;
    tcamData->frameOctet[19] = pIpv4Data->tos;
    tcamData->frameOctet[20] = (pIpv4Data->length >> 8) & 0xff;
    tcamData->frameOctet[21] = pIpv4Data->length & 0xff;
    tcamData->frameOctet[22] = (pIpv4Data->id >> 8) & 0xff;
    tcamData->frameOctet[23] = pIpv4Data->id & 0xff;
    tcamData->frameOctet[24] = (((pIpv4Data->flag << 13) | pIpv4Data->fragOffset) >> 8) & 0xff;
    tcamData->frameOctet[25] = ((pIpv4Data->flag << 13) | pIpv4Data->fragOffset) & 0xff;
    tcamData->frameOctet[26] = pIpv4Data->ttl;
    tcamData->frameOctet[27] = pIpv4Data->protocol;
    tcamData->frameOctet[28] = (pIpv4Data->hdrChkSum >> 8) & 0xff;
    tcamData->frameOctet[29] = (pIpv4Data->hdrChkSum) & 0xff;
    for (i = 0; i < 4; i++)
    {
        tcamData->frameOctet[30 + i] = pIpv4Data->sip[i];
        tcamData->frameOctet[34 + i] = pIpv4Data->dip[i];
    }

    tcamData->frameOctetMask[18] = (pIpv4Mask->version << 4) | pIpv4Mask->ihl;
    tcamData->frameOctetMask[19] = pIpv4Mask->tos;
    tcamData->frameOctetMask[20] = (pIpv4Mask->length >> 8) & 0xff;
    tcamData->frameOctetMask[21] = pIpv4Mask->length & 0xff;
    tcamData->frameOctetMask[22] = (pIpv4Mask->id >> 8) & 0xff;
    tcamData->frameOctetMask[23] = pIpv4Mask->id & 0xff;
    tcamData->frameOctetMask[24] = (((pIpv4Mask->flag << 13) | pIpv4Mask->fragOffset) >> 8) & 0xff;
    tcamData->frameOctetMask[25] = ((pIpv4Mask->flag << 13) | pIpv4Mask->fragOffset) & 0xff;
    tcamData->frameOctetMask[26] = pIpv4Mask->ttl;
    tcamData->frameOctetMask[27] = pIpv4Mask->protocol;
    tcamData->frameOctetMask[28] = (pIpv4Mask->hdrChkSum >> 8) & 0xff;
    tcamData->frameOctetMask[29] = (pIpv4Mask->hdrChkSum) & 0xff;
    for (i = 0; i < 4; i++)
    {
        tcamData->frameOctetMask[30 + i] = pIpv4Mask->sip[i];
        tcamData->frameOctetMask[34 + i] = pIpv4Mask->dip[i];
    }
}
static void mappingUdpToTcam(int startIndex, PERIDOT_MSD_TCAM_DATA *tcamData, MSD_TCAM_ENT_UDP_UNT *pUdpData, MSD_TCAM_ENT_UDP_UNT *pUdpMask)
{
    tcamData->frameOctet[startIndex + 0] = (pUdpData->srcPort >> 8) & 0xff;
    tcamData->frameOctet[startIndex + 1] = pUdpData->srcPort & 0xff;
    tcamData->frameOctet[startIndex + 2] = (pUdpData->destPort >> 8) & 0xff;
    tcamData->frameOctet[startIndex + 3] = (pUdpData->destPort) & 0xff;
    tcamData->frameOctet[startIndex + 4] = (pUdpData->length >> 8) & 0xff;
    tcamData->frameOctet[startIndex + 5] = (pUdpData->length) & 0xff;
    tcamData->frameOctet[startIndex + 6] = (pUdpData->chkSum >> 8) & 0xff;
    tcamData->frameOctet[startIndex + 7] = pUdpData->chkSum & 0xff;

    tcamData->frameOctetMask[startIndex + 0] = (pUdpMask->srcPort >> 8) & 0xff;
    tcamData->frameOctetMask[startIndex + 1] = pUdpMask->srcPort & 0xff;
    tcamData->frameOctetMask[startIndex + 2] = (pUdpMask->destPort >> 8) & 0xff;
    tcamData->frameOctetMask[startIndex + 3] = (pUdpMask->destPort) & 0xff;
    tcamData->frameOctetMask[startIndex + 4] = (pUdpMask->length >> 8) & 0xff;
    tcamData->frameOctetMask[startIndex + 5] = (pUdpMask->length) & 0xff;
    tcamData->frameOctetMask[startIndex + 6] = (pUdpMask->chkSum >> 8) & 0xff;
    tcamData->frameOctetMask[startIndex + 7] = pUdpMask->chkSum & 0xff;
}
static void mappingActionToTcam(PERIDOT_MSD_TCAM_DATA *tcamData, PERIDOT_MSD_TCAM_ENT_ACT *actionPtr)
{
    tcamData->interrupt = actionPtr->interrupt;
    tcamData->IncTcamCtr = actionPtr->IncTcamCtr;
    tcamData->vidOverride = actionPtr->vidOverride;
    tcamData->vidData = actionPtr->vidData;
    tcamData->qpriOverride = actionPtr->qpriOverride;
    tcamData->qpriData = actionPtr->qpriData;
    tcamData->fpriOverride = actionPtr->fpriOverride;
    tcamData->fpriData = actionPtr->fpriData;
    tcamData->dpvSF = actionPtr->dpvSF;
    tcamData->dpvData = actionPtr->dpvData;
    tcamData->dpvMode = actionPtr->dpvMode;
    tcamData->colorMode = actionPtr->colorMode;
    tcamData->vtuPageOverride = actionPtr->vtuPageOverride;
    tcamData->vtuPage = actionPtr->vtuPage;
    tcamData->unKnownFilter = actionPtr->unKnownFilter;
    tcamData->egActPoint = actionPtr->egActPoint;
    tcamData->ldBalanceOverride = actionPtr->ldBalanceOverride;
    tcamData->ldBalanceData = actionPtr->ldBalanceData;
    tcamData->DSCPOverride = actionPtr->DSCPOverride;
    tcamData->DSCP = actionPtr->DSCP;
    tcamData->factionOverride = actionPtr->factionOverride;
    tcamData->factionData = actionPtr->factionData;
}