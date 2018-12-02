#include <msdCopyright.h>

/*******************************************************************************
* Agate_msdTCAM.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdRMU.h>
#include <msdSem.h>
#include <agate/include/api/Agate_msdApiInternal.h>


static MSD_U32 dump_atuPacket
(
    IN      MSD_U8 delta,
	IN		MSD_U8 *packet,
	IN		MSD_U32 pkt_size,
	OUT	    MSD_U32 *numOfEntry,
	OUT		AGATE_MSD_ATU_ENTRY **atuEntry
);

static MSD_STATUS dump_mibPacket
(
    IN  MSD_U8 delta,
	IN	MSD_U8 *packet,
	IN	MSD_U32 pkt_size,
	OUT	AGATE_MSD_STATS_COUNTER_SET_LEGACY *statsCounterSet
);

static MSD_STATUS dump_mib2Packet
(
    IN  MSD_U8 delta,
	IN	MSD_U8 *packet,
	IN	MSD_U32 pkt_size,
	OUT	AGATE_MSD_STATS_COUNTER_SET *statsCounterSet
);


static MSD_STATUS dump_MultiRWPacket
(
    IN      MSD_U8 delta,
	IN		MSD_U8 *packet,
	IN		MSD_U32 pkt_size,
	INOUT	MSD_RegCmd **regCmdList
);

/*******************************************************************************
* msdRmuGetID
*
* DESCRIPTION:
*       Get the current Device ID.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       id - DUT Id.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuGetID
(
	IN  MSD_QD_DEV *dev,
	OUT MSD_U16 *id
)
{
	MSD_Packet ReqPkt;
	/*Packet RspPkt;*/
	MSD_U8 reqEthPacket[512];
	MSD_U8 rspEthPacket[512];
	MSD_U32 req_pktlen, rsp_pktlen;
	MSD_STATUS retVal = 0;
    MSD_U8 delta;

	MSD_RMU_CMD cmd = MSD_GetID;

	MSD_U8 *rspEthPacketPtr = &(rspEthPacket[0]);

	MSD_DBG_INFO(("Agate_msdRmuGetID Called.\n"));

	if (NULL == id)
	{
		MSD_DBG_ERROR(("input id is NULL.\n"));
		return MSD_BAD_PARAM;
	}

    if (dev->rmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        delta = 0;
    else if (dev->rmuMode == MSD_RMU_DSA_MODE)
        delta = 4;
    else
        delta = 0;
		
	/*Request Packet*/
	retVal = msdRmuReqPktCreate(dev, cmd, &ReqPkt);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuReqPktCreate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	msdMemSet(reqEthPacket, 0, sizeof(reqEthPacket));

	retVal = msdRmuPackEthReqPkt(&ReqPkt, cmd, reqEthPacket);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuPackEthReqPkt returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	req_pktlen = MSD_RMU_PACKET_PREFIX_SIZE - delta + 2;

	/*dump_packet(&reqEthPacket, req_pktlen, "Read Register Request Packet");*/
	retVal = msdRmuTxRxPkt(dev, reqEthPacket, req_pktlen,
		&rspEthPacketPtr, &rsp_pktlen);
	if (retVal != MSD_OK || rsp_pktlen == 0) {
		MSD_DBG_ERROR(("rmu_tx_rx returned: %s with rsp_pktLen %d.\n", msdDisplayStatus(retVal), (int)rsp_pktlen));
		return MSD_FAIL;
	}

	/* Receive Packet */
	if (rsp_pktlen >= req_pktlen) {
		/*dump_packet(&rspEthPacket, req_pktlen,
		//	"Read Register Response Packet");
		//msdRmuParseEthPacket(&rspEthPacket, RspPkt);*/
	}
	else {
		MSD_DBG_ERROR(("response_pktlen [%d] < request_pktlen [%d]\n",
			rsp_pktlen, req_pktlen));
		return MSD_FAIL;
	}

	*id = ((*(rspEthPacketPtr + 24 - delta) & 0xff) << 8) | (*(rspEthPacketPtr + 25 - delta) & 0xff);

	MSD_DBG_INFO(("Agate_msdRmuGetID Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* msdRmuAtuDump
*
* DESCRIPTION:
*       Dump ATU entry from the specified starting address.
*
* INPUTS:
*       startAddr - starting address to search the valid ATU entryies
*
* OUTPUTS:
*       atuEntry - array of atuEntries.
*		numOfEntry - number of returned valued entries
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuAtuDump
(
	IN MSD_QD_DEV *dev,
	INOUT MSD_U32 *startAddr,
	OUT MSD_U32 *numOfEntry,
	OUT AGATE_MSD_ATU_ENTRY **atuEntry
)
{
	MSD_Packet ReqPkt;
	MSD_U8 reqEthPacket[512];
	MSD_U8 rspEthPacket[512];
	MSD_U32 req_pktlen, rsp_pktlen;
	MSD_U32 next_cont_code = 0;
	MSD_STATUS retVal = 0;
    MSD_U8 delta;

	MSD_RMU_CMD cmd = MSD_DumpATU;

	MSD_U8 *rspEthPacketPtr = &(rspEthPacket[0]);

	MSD_DBG_INFO(("Agate_msdRmuAtuDump Called.\n"));

	if (startAddr == NULL || numOfEntry == NULL)
	{
		MSD_DBG_ERROR(("startAddr: %d, numOfEntry: %d.\n", startAddr, numOfEntry));
		return MSD_BAD_PARAM;
	}
	
    if (dev->rmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        delta = 0;
    else if (dev->rmuMode == MSD_RMU_DSA_MODE)
        delta = 4;
    else
        delta = 0;

	/*Request Packet*/
	retVal = msdRmuReqPktCreate(dev, cmd, &ReqPkt);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuReqPktCreate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	ReqPkt.reqData._reqData = (MSD_U16)(*startAddr);

	msdMemSet(reqEthPacket, 0, sizeof(reqEthPacket));
	retVal = msdRmuPackEthReqPkt(&ReqPkt, cmd, reqEthPacket);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuPackEthReqPkt returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	req_pktlen = MSD_RMU_PACKET_PREFIX_SIZE - delta + 2;

	/*dump_packet(&reqEthPacket, req_pktlen, "Read Register Request Packet");*/
	retVal = msdRmuTxRxPkt(dev, reqEthPacket, req_pktlen,
		&rspEthPacketPtr, &rsp_pktlen);
	if (retVal != MSD_OK || rsp_pktlen == 0) {
		MSD_DBG_ERROR(("msdRmu_tx_rx returned: %s with rsp_pktLen %d.\n", msdDisplayStatus(retVal), (int)rsp_pktlen));
		return MSD_FAIL;
	}

	if (rsp_pktlen < req_pktlen) {
		MSD_DBG_ERROR(("response_pktlen [%d] < request_pktlen [%d]\n", rsp_pktlen, req_pktlen));
		return MSD_FAIL;
	}

	/* Receive Packet*/
	next_cont_code = dump_atuPacket(delta, rspEthPacketPtr, rsp_pktlen, numOfEntry, atuEntry);

	*startAddr = next_cont_code;

	MSD_DBG_INFO(("Agate_msdRmuAtuDump Exit.\n"));

	return MSD_OK;
}

/*******************************************************************************
* msdRmuMibDump
*
* DESCRIPTION:
*       Dump MIB from the specified port.
*
* INPUTS:
*       enClearOnRead - indicate dump mib clear or not
*		port - request port
*
* OUTPUTS:
*       statsCounterSet - mib info.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuMibDump
(
	IN MSD_QD_DEV *dev,
	IN MSD_BOOL enClearOnRead,
	IN MSD_LPORT port,
	OUT AGATE_MSD_STATS_COUNTER_SET_LEGACY *statsCounterSet
)
{
	MSD_Packet ReqPkt;
	MSD_U8 reqEthPacket[512];
	MSD_U8 rspEthPacket[512];
	MSD_U32 req_pktlen, rsp_pktlen;
	MSD_STATUS retVal;
    MSD_U8 delta;

	MSD_RMU_CMD cmd = (enClearOnRead ? MSD_DumpMIBClear : MSD_DumpMIB);

	MSD_U8 *rspEthPacketPtr = &(rspEthPacket[0]);

	MSD_DBG_INFO(("Agate_msdRmuMibDump Called.\n"));

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("statsCounterSet is NULL.\n"));
		return MSD_BAD_PARAM;
	}

    if (dev->rmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        delta = 0;
    else if (dev->rmuMode == MSD_RMU_DSA_MODE)
        delta = 4;
    else
        delta = 0;

	/*Request Packet*/
	retVal = msdRmuReqPktCreate(dev, cmd, &ReqPkt);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuReqPktCreate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	ReqPkt.reqData._reqData = (MSD_U16)((enClearOnRead ? 0x8000 : 0x0) | port);

	msdMemSet(reqEthPacket, 0, sizeof(reqEthPacket));
	retVal = msdRmuPackEthReqPkt(&ReqPkt, cmd, reqEthPacket);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuPackEthReqPkt returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	req_pktlen = MSD_RMU_PACKET_PREFIX_SIZE - delta + 2;

	/*dump_packet(&reqEthPacket, req_pktlen, "Read Register Request Packet");*/
	retVal = msdRmuTxRxPkt(dev, reqEthPacket, req_pktlen,
		&rspEthPacketPtr, &rsp_pktlen);
	if (retVal != MSD_OK || rsp_pktlen == 0) {
		MSD_DBG_ERROR(("msdRmu_tx_rx returned: %s with rsp_pktLen %d.\n", msdDisplayStatus(retVal), (int)rsp_pktlen));
		return MSD_FAIL;
	}

	/* Receive Packet*/
	retVal = dump_mibPacket(delta, rspEthPacketPtr, rsp_pktlen, statsCounterSet);

	MSD_DBG_INFO(("Agate_msdRmuMibDump Exit.\n"));

	return retVal;
}

/*******************************************************************************
* msdRmuMib2Dump
*
* DESCRIPTION:
*       Dump MIB from the specified port.
*
* INPUTS:
*       enClearOnRead - indicate dump mib clear or not
*		port - request port
*
* OUTPUTS:
*       statsCounterSet - mib info.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuMib2Dump
(
	IN MSD_QD_DEV *dev,
	IN MSD_BOOL enClearOnRead,
	IN MSD_LPORT port,
	OUT AGATE_MSD_STATS_COUNTER_SET *statsCounterSet
)
{
	MSD_Packet ReqPkt;
	MSD_U8 reqEthPacket[512];
	MSD_U8 rspEthPacket[512];
	MSD_U32 req_pktlen, rsp_pktlen;
	MSD_STATUS retVal;
    MSD_U8 delta;

	MSD_RMU_CMD cmd = (enClearOnRead ? MSD_DumpMIB2Clear : MSD_DumpMIB2);

	MSD_U8 *rspEthPacketPtr = &(rspEthPacket[0]);

	MSD_DBG_INFO(("Agate_msdRmuMib2Dump Called.\n"));

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("statsCounterSet is NULL.\n"));
		return MSD_BAD_PARAM;
	}

    if (dev->rmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        delta = 0;
    else if (dev->rmuMode == MSD_RMU_DSA_MODE)
        delta = 4;
    else
        delta = 0;

	/*Request Packet*/
	retVal = msdRmuReqPktCreate(dev, cmd, &ReqPkt);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuReqPktCreate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	ReqPkt.reqData._reqData = (MSD_U16)((enClearOnRead ? 0x8000 : 0x0) | port);
	ReqPkt.reqFmt = 0x1;
	msdMemSet(reqEthPacket, 0, sizeof(reqEthPacket));
	retVal = msdRmuPackEthReqPkt(&ReqPkt, cmd, reqEthPacket);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuPackEthReqPkt returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    req_pktlen = MSD_RMU_PACKET_PREFIX_SIZE - delta + 2;

	/*dump_packet(&reqEthPacket, req_pktlen, "Read Register Request Packet");*/
	retVal = msdRmuTxRxPkt(dev, reqEthPacket, req_pktlen,
		&rspEthPacketPtr, &rsp_pktlen);
	if (retVal != MSD_OK || rsp_pktlen == 0) {
		MSD_DBG_ERROR(("msdRmu_tx_rx returned: %s with rsp_pktLen %d.\n", msdDisplayStatus(retVal), (int)rsp_pktlen));
		return MSD_FAIL;
	}  

	/* Receive Packet*/
	if (rsp_pktlen >= 0xa0)
		rsp_pktlen = 0x123;
	retVal = dump_mib2Packet(delta, rspEthPacketPtr, rsp_pktlen, statsCounterSet);

	MSD_DBG_INFO(("Agate_msdRmuMib2Dump Exit.\n"));

	return retVal;
}


/*******************************************************************************
* msdRmuReadRegister
*
* DESCRIPTION:
*       single register read.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*
* OUTPUTS:
*       data - return register value.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuReadRegister
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	OUT MSD_U16 *data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Agate_msdRmuReadRegister Called.\n"));

	if (NULL == data)
	{
		MSD_DBG_ERROR(("data is NULL.\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyReg(dev->devNum, devAddr, regAddr, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_msdRmuReadRegister Exit.\n"));

	return retVal;
}

/*******************************************************************************
* msdRmuWriteRegister
*
* DESCRIPTION:
*       single register write.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*		data - register value.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuWriteRegister
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	IN MSD_U16 data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Agate_msdRmuWriteRegister Called.\n"));

	retVal = msdSetAnyReg(dev->devNum, devAddr, regAddr, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_msdRmuWriteRegister Exit.\n"));

	return retVal;
}

/*******************************************************************************
* msdRmuMultiRegAccess
*
* DESCRIPTION:
*       multiple register read/write.
*
* INPUTS:
*       RegCmd - array of Read/Write requests
*		nCmd - number of register access commands
*
* OUTPUTS:
*       RegCmd - update read-back register value if it has register read request
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuMultiRegAccess
(
	IN MSD_QD_DEV *dev,
	INOUT MSD_RegCmd **regCmdList,
	IN MSD_U32 nCmd
)
{
	MSD_Packet ReqPkt;
	MSD_U8 reqEthPacket[512];
	MSD_U8 rspEthPacket[512];
	MSD_U32 req_pktlen, rsp_pktlen;
	MSD_STATUS retVal = 0;
	MSD_U32	i = 0;
    MSD_U8 delta;

	MSD_RMU_CMD cmd = MSD_RegRW;

	MSD_U8 *rspEthPacketPtr = &(rspEthPacket[0]);

	MSD_DBG_INFO(("Agate_msdRmuMultiRegAccess Called.\n"));

	if (nCmd > MSD_RMU_MAX_REGCMDS)
	{
		MSD_DBG_ERROR(("nCmd %u exceeds the maximum supported CMD number %d\n.", (unsigned int)nCmd, MSD_RMU_MAX_REGCMDS));
		return MSD_FAIL;
	}

	if ((nCmd != 0) && (NULL == regCmdList))
	{
		MSD_DBG_ERROR(("regCmdList is NULL with nCmd: %d.\n", nCmd));
		return MSD_FAIL;
	}

    if (dev->rmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        delta = 0;
    else if (dev->rmuMode == MSD_RMU_DSA_MODE)
        delta = 4;
    else
        delta = 0;

	/*Request Packet*/
	retVal = msdRmuReqPktCreate(dev, cmd, &ReqPkt);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuReqPktCreate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	ReqPkt.reqData._regRWData.nCmd = nCmd;
	for (i = 0; i < nCmd; i++)
	{
		ReqPkt.reqData._regRWData.regCmd[i].isWaitOnBit = (*regCmdList)[i].isWaitOnBit;
		ReqPkt.reqData._regRWData.regCmd[i].opCode = (*regCmdList)[i].opCode;
		ReqPkt.reqData._regRWData.regCmd[i].devAddr = (*regCmdList)[i].devAddr;
		ReqPkt.reqData._regRWData.regCmd[i].regAddr = (*regCmdList)[i].regAddr;
		ReqPkt.reqData._regRWData.regCmd[i].data = (*regCmdList)[i].data;
	}

	msdMemSet(reqEthPacket, 0, sizeof(reqEthPacket));
	retVal = msdRmuPackEthReqPkt(&ReqPkt, cmd, reqEthPacket);
	if (retVal != MSD_OK) {
		MSD_DBG_ERROR(("msdRmuPackEthReqPkt returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	req_pktlen = MSD_RMU_PACKET_PREFIX_SIZE -delta + (nCmd + 1)*MSD_RMU_REGCMD_WORD_SIZE;

	/* dump_packet(&reqEthPacket, req_pktlen, "Read Register Request Packet"); */
	retVal = msdRmuTxRxPkt(dev, reqEthPacket, req_pktlen,
		&rspEthPacketPtr, &rsp_pktlen);
	if (retVal != MSD_OK || rsp_pktlen == 0) {
		MSD_DBG_ERROR(("msdRmu_tx_rx returned: %s with rsp_pktLen %d.\n", msdDisplayStatus(retVal), (int)rsp_pktlen));
		return MSD_FAIL;
	}

	if (rsp_pktlen < req_pktlen) {
		MSD_DBG_ERROR(("response_pktlen [%d] < request_pktlen [%d]\n", rsp_pktlen, req_pktlen));
		return MSD_FAIL;
	}

	/* Receive Packet */
	retVal = dump_MultiRWPacket(delta, rspEthPacketPtr, rsp_pktlen, regCmdList);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("dump_MultiRWPacket returned: %s.\n", msdDisplayStatus(retVal)));
		return MSD_FAIL;
	}

	MSD_DBG_INFO(("Agate_msdRmuMultiRegAccess Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Agate_msdRmuRegDump
*
* DESCRIPTION:
*       Dump 0~31 registers for the specified devAddr.
*
* INPUTS:
*       devAddr - device address
*
* OUTPUTS:
*       dataList - 32 registers value for the specified devive address
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_msdRmuRegDump
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8 devAddr,
    OUT MSD_RegList *dataList
)
{
    MSD_STATUS retVal;
    MSD_RegCmd regcmdList[32];
    MSD_RegCmd *pRegCmd = &(regcmdList[0]);
    MSD_U32 nCmd = 32;
    MSD_U8 index;

    MSD_DBG_INFO(("Agate_msdRmuRegDump Called.\n"));

    if (NULL == dataList)
    {
        MSD_DBG_ERROR(("dataList is null.\n"));
        return MSD_BAD_PARAM;
    }

    for (index = 0; index < 32; index++)
    {
        regcmdList[index].isWaitOnBit = MSD_RMU_WAIT_ON_BIT_FALSE;
        regcmdList[index].opCode = MSD_RMU_REQ_OPCODE_READ;
        regcmdList[index].devAddr = devAddr;
        regcmdList[index].regAddr = index;
        regcmdList[index].data = 0;
    }

    if ((retVal = Agate_msdRmuMultiRegAccess(dev, &pRegCmd, nCmd)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Agate_msdRmuMultiRegAccess returned fail.\n"));
        return retVal;
    }

    for (index = 0; index < 32; index++)
    {
        dataList->data[index] = regcmdList[index].data;
    }

    MSD_DBG_INFO(("Agate_msdRmuRegDump Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_U32 dump_atuPacket
(
    IN      MSD_U8 delta,
	IN		MSD_U8 *packet, 
	IN		MSD_U32 pkt_size, 
	OUT	    MSD_U32 *numOfEntry, 
	OUT		AGATE_MSD_ATU_ENTRY **atuEntry
)
{
	MSD_U8 *ptr = packet;
	MSD_U32 n;
	MSD_U32 idx = 0;
	/*AGATE_MSD_ATU_ENTRY atu;*/
	/*static MSD_U32 atu_slno = 0;*/
	MSD_U32 atu_cnt = 0;
	MSD_U32 cont_addr = 0;
	MSD_BOOL flag = MSD_TRUE;

	ptr += MSD_RMU_PACKET_PREFIX_SIZE - delta;
	idx += MSD_RMU_PACKET_PREFIX_SIZE - delta;

	/*if (addr == 0) {
	//	atu_slno = 0;
	//	printf("ATU Entries Dump\n");
	//	printf("+------------------------------------------+\n");
	//	printf("| Sl.No  ES  DPV   Mac Address   PRI  FID  |\n");
	//	printf("+------------------------------------------+\n");
	//}*/

	while (flag) {

		if (idx + 10 > pkt_size)
			break;

		((*atuEntry) + atu_cnt)->entryState = ((*ptr) >> 4) & 0xf;
		if (((*atuEntry) + atu_cnt)->entryState == 0)
			break;

		((*atuEntry) + atu_cnt)->LAG = ((*ptr) >> 3) & 0x01;

		((*atuEntry) + atu_cnt)->portVec = *(ptr + 1) | (((*ptr) & 0x7) << 8);
		ptr += 2;

		/*atu.macAddr = 0x0L;*/
		for (n = 0; n < 6; n++) {
			((*atuEntry) + atu_cnt)->macAddr.arEther[n] = (MSD_U8)(*ptr);/* << 8 * n; */
			ptr++;
		}

		((*atuEntry) + atu_cnt)->exPrio.macFPri = ((*ptr) >> 4) & 0x7;

		((*atuEntry) + atu_cnt)->DBNum = (*(ptr + 1)) | (((*ptr) & 0xf) << 8);
		ptr += 2;
                /*
		MSD_DBG_INFO(("| %05d  %01X   %03X   %012llX   %01X   %03X  |\n", atu_slno++,
			((*atuEntry) + atu_cnt)->entryState, 
			((*atuEntry) + atu_cnt)->portVec, 
			((*atuEntry) + atu_cnt)->macAddr, 
			((*atuEntry) + atu_cnt)->exPrio.macFPri, 
			((*atuEntry) + atu_cnt)->DBNum));
                */
		idx += 10;
		atu_cnt++;
	}

	if (atu_cnt == MSD_RMU_MAX_ATUS) {
		cont_addr = (*(ptr) << 8) | (*(ptr + 1));
	}

	/*
	if (cont_addr == 0)
		MSD_DBG_INFO(("+------------------------------------+\n"));
	*/
	*numOfEntry = atu_cnt;

	return cont_addr;
}

static MSD_STATUS dump_mibPacket
(
IN  MSD_U8 delta,
IN	MSD_U8 *packet,
IN	MSD_U32 pkt_size,
OUT	AGATE_MSD_STATS_COUNTER_SET_LEGACY *statsCounterSet
)
{
    MSD_U8 *ptr = packet;
    /*
MSD_U8 dev_num = 0;
MSD_U8 port_num = 0;
*/
    MSD_U32  temp = 0;

    if (pkt_size < (MSD_U32)(170 - delta)) {
        MSD_DBG_ERROR(("Error: Mib Packet Size [%d] < 170 bytes\n", (int)pkt_size));
        return MSD_FAIL;
    }

    ptr += MSD_RMU_PACKET_PREFIX_SIZE - delta;

    /*dev_num = (*ptr) & 0x1f; */
    ptr++;
    /*port_num = ((*ptr) >> 3) & 0xf; */
    ptr++;

    /*time stamp*/
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;

    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodOctetsLo = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodOctetsHi = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBadOctets = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutFCSErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InUnicasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Deferred = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBroadcasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InMulticasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets64 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets127 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets255 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets511 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets1023 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OctetsMax = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsLo = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsHi = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutUnicasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Excessive = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutMulticasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutBroadcasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Single = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutPause = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InPause = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Multiple = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InUndersize = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InFragments = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InOversize = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InJabber = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InRxErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InFCSErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Collisions = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Late = temp;

    return MSD_OK;
}

static MSD_STATUS dump_mib2Packet
(
IN  MSD_U8 delta,
IN	MSD_U8 *packet,
IN	MSD_U32 pkt_size,
OUT	AGATE_MSD_STATS_COUNTER_SET *statsCounterSet
)
{
    MSD_U8 *ptr = packet;
    /*
MSD_U8 dev_num = 0;
MSD_U8 port_num = 0;
*/
    MSD_U32  temp = 0;

    if (pkt_size < (MSD_U32)(290 - delta)) {
        MSD_DBG_ERROR(("Error: Mib Packet Size [%d] < 290 bytes\n", (int)pkt_size));
        return MSD_FAIL;
    }

    ptr += MSD_RMU_PACKET_PREFIX_SIZE - delta;

    /*dev_num = (*ptr) & 0x1f;  */
    ptr++;
    /*port_num = ((*ptr) >> 3) & 0xf; */
    ptr++;

    /*time stamp*/
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;

    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodOctetsLo = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodOctetsHi = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBadOctets = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutFCSErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InUnicasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Deferred = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBroadcasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InMulticasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets64 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets127 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets255 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets511 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Octets1023 = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OctetsMax = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsLo = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsHi = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutUnicasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Excessive = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutMulticasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutBroadcasts = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Single = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutPause = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InPause = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Multiple = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InUndersize = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InFragments = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InOversize = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InJabber = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InRxErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InFCSErr = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Collisions = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->Late = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InDiscards = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InFiltered = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InAccepted = temp;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBadAccepted = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodAvbClassA = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InGoodAvbClassB = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBadAvbClassA = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InBadAvbClassB = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->TCAMCounter0 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->TCAMCounter1 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->TCAMCounter2 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->TCAMCounter3 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InDroppedAvbA = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InDroppedAvbB = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InDaUnknown = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->InMGMT = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue0 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue1 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue2 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue3 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue4 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue5 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue6 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutQueue7 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutCutThrough = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->reserved_19 = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsA = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutOctetsB = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutYel = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutDroppedYel = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutDiscards = 0;
    temp = 0;
    temp |= ((*ptr) & 0xff) << 24; ptr++;
    temp |= ((*ptr) & 0xff) << 16; ptr++;
    temp |= ((*ptr) & 0xff) << 8; ptr++;
    temp |= ((*ptr) & 0xff) << 0; ptr++;
    statsCounterSet->OutMGMT = 0;
    return MSD_OK;
}

static MSD_STATUS dump_MultiRWPacket
(
    IN      MSD_U8 delta,
    IN		MSD_U8 *packet,
    IN		MSD_U32 pkt_size,
    INOUT	MSD_RegCmd **regCmdList
)
{
	MSD_U8 *ptr = packet;
	MSD_U32  i = 0, size = 0;
	MSD_U16 *ptrr;
	MSD_BOOL flag = MSD_TRUE;
	MSD_BOOL isFoundEnd = MSD_FALSE;

	ptr += MSD_RMU_PACKET_PREFIX_SIZE - delta;
	size += MSD_RMU_PACKET_PREFIX_SIZE - delta;
	ptrr = (MSD_U16*)ptr;

	while (flag) {
		if (size >= pkt_size)
			break;
		if (*ptrr == 0xFFFF && *(ptrr + 1) == 0xFFFF)
		{
			isFoundEnd = MSD_TRUE;
			break;
		}
		ptrr += 1;
		(*regCmdList)[i].data = msd_ntohs((MSD_U16)((*ptrr) & 0xffff)); ptrr = ptrr + 1;
		i++;
		size += 4;
	}

	if (MSD_FALSE == isFoundEnd)
		return MSD_FAIL;

	return MSD_OK;
}
