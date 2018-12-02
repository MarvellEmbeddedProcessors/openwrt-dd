#include <msdCopyright.h>
/********************************************************************************
* sample_msdQos.c
*
* DESCRIPTION:
*    These samples only supported by Peridot
*    The sample sample_msdQos_PCPToFpriQpri will demonstrate how to
*    1. Program the Ingress PCP to IEEE_FPRI and IEEE_QPRI table to one port.
*
*    The sample sample_msdQos_GreenFpriToDSCP will demonstrate how to
*    1. Program the Green Frame FPRI to DSCP mapping
*    2. Enable the DSCP rewrite at egress
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Program the Ingress PCP to IEEE_FPRI and IEEE_QPRI table, program the 
*       frame's PCP 0~7 mapping to QPRI as "5 2 7 1 4 3 0 6" and mapping to 
*       FPRI as "7 6 5 4 3 2 1 0" on port 0
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdQos_PCPToFpriQpri(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_IEEE_PRIORITY_TABLE_SET table;
	MSD_IEEE_PRIORITY_TABLE_STRUCT entryStruct;
	MSD_LPORT port = 0;
	MSD_U8 QpriList[8] = { 5, 2, 7, 1, 4, 3, 0, 6 };
	MSD_U8 FpriList[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
	MSD_U8 entryPtr;

	table = MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI;
	for (entryPtr = 0; entryPtr < 8; entryPtr++)
	{
		entryStruct.ingTbl.ieeeYellow = 0;
		entryStruct.ingTbl.disIeeeQpri = 0;
		entryStruct.ingTbl.ieeeQpri = QpriList[entryPtr];
		entryStruct.ingTbl.disIeeeFpri = 0;
		entryStruct.ingTbl.ieeeFpri = FpriList[entryPtr];

		if ((status = msdQosPortPrioMapSet(devNum, port, table, entryPtr, &entryStruct)) != MSD_OK)
		{
			MSG_PRINT(("msdQosPortPrioMapSet returned fail.\n"));
			return status;
		}
	}

	MSG_PRINT(("sample_msdQos success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #1:
*       Program the Green Frame FPRI to DSCP table, program the frame's PCP 0~7 
*       mapping to DSCP value list as "0x38 0x18 0x8 0x0 0x20 0x30 0x10 0x10" and 
*       enable the DSCP rewrite to update DSCP field in frames egressing on port 0
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdQos_GreenFpriToDSCP(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_IEEE_PRIORITY_TABLE_SET table;
	MSD_IEEE_PRIORITY_TABLE_STRUCT entryStruct;
	MSD_LPORT port = 0;
	MSD_U8 entryPtr;
	MSD_U8 dscpList[8] = { 0x38, 0x18, 0x8, 0x0, 0x20, 0x30, 0x10, 0x10 };

	/*Program the Green Frame FPRI to DSCP table on port0*/
	table = MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP;
	for (entryPtr = 0; entryPtr < 8; entryPtr++)
	{
		entryStruct.egGreeTblDscp.enDSCPMarking = 1;
		entryStruct.egGreeTblDscp.frameDSCP = dscpList[entryPtr];

		if ((status = msdQosPortPrioMapSet(devNum, port, table, entryPtr, &entryStruct)) != MSD_OK)
		{
			MSG_PRINT(("msdQosPortPrioMapSet returned fail.\n"));
			return status;
		}
	}

	/*Enable the DSCP rewrite to update the DSCP field in frame*/
	if ((status = msdQosPortRewriteDscp(devNum, port, MSD_TRUE)) != MSD_OK)
	{
		MSG_PRINT(("msdQosPortRewriteDscp returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdQos success.\n"));
	return MSD_OK;
}
