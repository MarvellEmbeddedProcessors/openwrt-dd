#include <msdCopyright.h>

/********************************************************************************
* msdHwAccess.h
*
* DESCRIPTION:
*       Functions declarations for Hw accessing switch register, smi phy registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __msdHwAccessh
#define __msdHwAccessh

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Definition for Multi Address Mode */
#define MSD_REG_SMI_COMMAND         0x0
#define MSD_REG_SMI_DATA            0x1

/* Bit definition for MSD_REG_SMI_COMMAND */
#define MSD_SMI_BUSY                0x8000
#define MSD_SMI_MODE                0x1000
#define MSD_SMI_MODE_BIT            12
#define MSD_SMI_FUNC_BIT            13
#define MSD_SMI_FUNC_SIZE            2
#define MSD_SMI_OP_BIT              10
#define MSD_SMI_OP_SIZE              2
#define MSD_SMI_DEV_ADDR_BIT         5
#define MSD_SMI_DEV_ADDR_SIZE        5
#define MSD_SMI_REG_ADDR_BIT         0
#define MSD_SMI_REG_ADDR_SIZE        5

#define MSD_SMI_CLAUSE45            0
#define MSD_SMI_CLAUSE22            1

#define MSD_SMI_WRITE_ADDR          0x00
#define MSD_SMI_WRITE               0x01
#define MSD_SMI_READ_22             0x02
#define MSD_SMI_READ_45             0x03
#define MSD_SMI_READ_INC            0x02

/* This macro calculates the mask for partial read /    */
/* write of register's data.                            */
#define MSD_CALC_MASK(fieldOffset,fieldLen,mask)        \
            if((fieldLen + fieldOffset) >= 16)      \
                mask = (0 - (1 << fieldOffset));    \
            else                                    \
                mask = (((1 << (fieldLen + fieldOffset))) - (1 << fieldOffset));


#define MSD_SMI_ACCESS_LOOP        1000
#define MSD_SMI_TIMEOUT            2

/***********************************************************************
*  Internal Phy Clause 45 Register access *
***********************************************************************/
#define   MSD_PHY_MMD_ADDR          0x0000
#define   MSD_PHY_MMD_DATA_NO_INC   0x4000
#define   MSD_PHY_MMD_DATA_RW_INC   0x8000
#define   MSD_PHY_MMD_DATA_WR_INC   0xc000

/****************************************************************************/
/* Switch Registers related functions.                                         */
/****************************************************************************/

/*******************************************************************************
*
*
* DESCRIPTION:
*       This function directly writes to a switch's register.
*
* INPUTS:
*       devAddr - device register.
*       regAddr - The register's address.
*       data    - The data to be written.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSetAnyReg
(
	IN  MSD_U8	 devNum,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U16   data
);

/*******************************************************************************
* msdGetAnyReg
*
* DESCRIPTION:
*       This function directly reads a switch's register.
*
* INPUTS:
*       devAddr - device register.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdGetAnyReg
(
	IN  MSD_U8	 devNum,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT MSD_U16   *data
);

/*******************************************************************************
* msdSetAnyRegField
*
* DESCRIPTION:
*       This function writes to specified field in a switch's register.
*
* INPUTS:
*       devAddr     - Device Address to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
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
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MSD_STATUS msdSetAnyRegField
(
	IN  MSD_U8	 devNum,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
);

/*******************************************************************************
* msdGetAnyRegField
*
* DESCRIPTION:
*       This function reads a specified field from a switch's register.
*
* INPUTS:
*       devAddr     - device address to read the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to read.
*
* OUTPUTS:
*       data        - The read register field.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MSD_STATUS msdGetAnyRegField
(
    IN  MSD_U8	 devNum,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT MSD_U16   *data
);

/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                */
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
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSetSMIC45PhyReg
(
    IN MSD_U8    devNum,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
);

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
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdGetSMIC45PhyReg
(
    IN  MSD_U8   devNum,
    IN  MSD_U8   devAddr,
    IN  MSD_U8   phyAddr,
    IN  MSD_U16  regAddr,
    OUT MSD_U16  *data
);

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
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSetSMIPhyReg
(
    IN  MSD_U8   devNum,
    IN  MSD_U8   devAddr,
    IN  MSD_U8   regAddr,
    IN  MSD_U16  data
);

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
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdGetSMIPhyReg
(
    IN  MSD_U8   devNum,
    IN  MSD_U8   devAddr,
    IN  MSD_U8   regAddr,
    OUT MSD_U16  *data
);


MSD_STATUS msdSetSMIPhyXMDIOReg
(
    IN MSD_U8  devNum,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    IN MSD_U16 data
);

MSD_STATUS msdGetSMIPhyXMDIOReg
(
    IN MSD_U8  devNum,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U8  regAddr,
    OUT MSD_U16 *data
);

/****************************************************************************/
/* RMU related functions.                                         */
/****************************************************************************/

#define MSD_RMU_MAX_REGCMDS		(121-1)	 /* exclusive eof 0xFFFFFFFF */
#define MSD_RMU_MAX_TCAMS		3		
#define MSD_RMU_MAX_ATUS		48
/*
#define RMU_REQ_OPCODE_READ  2
#define RMU_REQ_OPCODE_WRITE 1
#define RMU_WAIT_ON_BIT_FALSE 0
#define RMU_WAIT_ON_BIT_TRUE 1
#define RMU_WAIT_ON_BIT_VAL0 0
#define RMU_WAIT_ON_BIT_VAL1 3
*/

#define MSD_RMU_PACKET_PREFIX_SIZE		28 	/* DA(6)+SA(6)+ETYPE(4)+DSA(4)+Type(2)+ReqFMT(2)+Pad(2)+ReqCode(2) */
#define MSD_RMU_TCAM_PACKET_PREFIX_SIZE 38 /*MSD_RMU_PACKET_PREFIX_SIZE + 10B*/
#define MSD_RMU_TCAM_ENTRY_SIZE			154
#define MSD_RMU_TCAM_ENTRY_OP_SIZE		(MSD_RMU_TCAM_ENTRY_SIZE + 2)
#define MSD_RMU_REGCMD_WORD_SIZE		4

#define MSD_RMU_TCAM_OPCODE_READ	0x5
#define MSD_RMU_TCAM_OPCODE_WRITE	0x3

/*
typedef struct {
	MSD_U8 isWaitOnBit;
	MSD_U8 opCode;
	MSD_U8 devAddr;
	MSD_U8 regAddr;
	MSD_U16 data;
} RegCmd;
*/

typedef struct {
	MSD_RegCmd regCmd[MSD_RMU_MAX_REGCMDS];
	MSD_U32 nCmd;
	MSD_U32 eof;
} MSD_RMU_RegRW_ReqData;

typedef struct {
	MSD_U16	dataArray[MSD_RMU_TCAM_ENTRY_OP_SIZE / 2];
} MSD_RMU_TCAM_RW_ENTRY;

typedef struct {
	MSD_RMU_TCAM_RW_ENTRY tcamEntry[MSD_RMU_MAX_TCAMS];
	MSD_U32	nEntry;
	MSD_U16  eol;
} MSD_RMU_TCAM_RW_ReqData;

typedef struct {
	MSD_U8 DA[6];
	MSD_U8 SA[6];
    MSD_RMU_MODE rmuMode;
	MSD_U32 etherType;	/* 0x91000000 */
	MSD_U32 dsaTag;		/* Trg_Dev + SeqNum */
	MSD_U16 lenType;		/* 0x0800 */
	MSD_U16 reqFmt;		/* 0x0000 or 0x0001 */
	MSD_U16 pad;			/* 0x0000 */
	MSD_U16 reqCode;
	union {
		/*
		GetID
		DumpATU
		DumpMIB
		DumpMIB2
		DumpTCAM
		*/
		MSD_U16 _reqData;
		/*
		Multiple Reg RW
		*/
		MSD_RMU_RegRW_ReqData _regRWData;
		/*
		Multiple TCAM RW
		*/
		MSD_RMU_TCAM_RW_ReqData _tcamRWData;
	} reqData;
} MSD_Packet;

typedef enum {
	MSD_GetID = 0,
	MSD_DumpATU,
	MSD_DumpMIB,
	MSD_DumpMIBClear,
	MSD_DumpMIB2,
	MSD_DumpMIB2Clear,
	MSD_RegRW,
	MSD_DumpTCAM,
	MSD_TCAMRW
} MSD_RMU_CMD;

MSD_STATUS msdRmuReqPktCreate
(
IN	MSD_QD_DEV	*dev,
IN	MSD_RMU_CMD		cmd,
OUT MSD_Packet		*packet
);

/*Convert Packet struct to network byte stream*/
MSD_STATUS msdRmuPackEthReqPkt
(
IN	MSD_Packet	*packet,
IN	MSD_RMU_CMD	cmd,
OUT MSD_U8	*buf
);

MSD_STATUS msdRmuTxRxPkt
(
    IN  MSD_QD_DEV* dev,
	IN  MSD_U8	*reqPkt,
	IN  MSD_U32	reqPktLen,
	OUT MSD_U8	**rspPkt,
	OUT MSD_U32	*rspPktLen
);

#ifdef __cplusplus
}
#endif
#endif /* __msdHwAccess_h */
