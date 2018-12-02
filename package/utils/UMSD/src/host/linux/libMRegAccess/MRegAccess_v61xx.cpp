#include <stdio.h>
#include <string.h>
#include "cyusb.h"
#include "MRegAccessClass.h"

int MRegAccess_v61xx::MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr)
{
	int data = 0;
	//Total EP2 data size 5 16-bit, 10byte
	unsigned char outData[10] = {0};

	//4 bytes CFG2 data
	// MDIO st_code=0x1, smi_mode=0x0, bust_len=1
	unsigned int st_code = 0x01, smi_mode=0, smi_port=SMIPort, burst_len=1;
	outData[0] = SMIControlReg;
	outData[1] = 0x06 << 5;
	outData[2] = (((_last_TA_ext & 0x1) << 4) | ((smi_mode & 0x1) << 3) | (smi_port & 0x7));
	outData[3] = st_code << 3;

	// 4 bytes burstLen data
	outData[4] = BurstLen;
	outData[5] = 0x06 << 5;
	outData[6] = burst_len & 0x00FF;
	outData[7] = (burst_len & 0xFF00) >> 8;

	// 2 bytes SMIRead data
	outData[8] = (((phyAddr & 0x7) << 5) + (regAddr & 0x1F));
	outData[9] = (SMI_READ + MDIO_READ + ((phyAddr & 0x1F) >> 3));

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 10, &len, _timeout))
		return -1;

	// Get the 16-bit binary data from Bulk Endpoint 6
	unsigned char inData[2];
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 2, &len, _timeout))
		return -2;

	data = ((int)inData[1] << 8) + (int)inData[0];

	return data;
}
int MRegAccess_v61xx::MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data)
{
	// total EP2 data size 6 16-bit, 12 bytes
	unsigned char outData[12];

	// 4 bytes CFG2 data
	// MDIO st_code=0x1, smi_mode=0x0, bust_len=1,
	uint st_code = 0x1, smi_mode = 0, smi_port = SMIPort, burst_len = 1;
	outData[0] = SMIControlReg;
	outData[1] = 0x06 << 5;
	outData[2] = (((_last_TA_ext & 0x1) << 4) | ((smi_mode & 0x1) << 3) | (smi_port & 0x7));
	outData[3] = st_code << 3;

	// 4 bytes burstLen data
	outData[4] = BurstLen;
	outData[5] = 0x06 << 5;
	outData[6] = burst_len & 0x00FF;
	outData[7] = (burst_len & 0xFF00) >> 8;

	// 4 bytes SMIRead data
	outData[8] = (((phyAddr & 0x7) << 5) + (regAddr & 0x1F));
	outData[9] = SMI_WRITE + MDIO_WRITE + ((phyAddr & 0x1F) >> 3);
	outData[10] = (data & 0x00FF);
	outData[11] = ((data & 0xFF00) >> 8);

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 12, &len, _timeout))
		return -1;

	return 0;
}

int MRegAccess_v61xx::XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr)
{
	int data = 0;
	// total EP2 data size 7 16-bit, 14 bytes
	unsigned char outData[14];

	// 4 bytes CFG2 data
	// set CFG2: XMDIO ST=00, Burst_Length = 2
	uint st_code = 0, smi_mode = 0, smi_port = SMIPort, burst_len = 2;
	outData[0] = SMIControlReg;
	outData[1] = 0x06 << 5;
	outData[2] = (((_last_TA_ext & 0x1) << 4) | ((smi_mode & 0x1) << 3) | (smi_port & 0x7));
	outData[3] = st_code << 3;

	// 4 bytes burstLen data
	outData[4] = BurstLen;
	outData[5] = 0x06 << 5;
	outData[6] = burst_len & 0x00FF;
	outData[7] = (burst_len & 0xFF00) >> 8;

	// 6 bytes SMIRead data
	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[8] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[9] = (SMI_WRITE + XMDIO_ADDRESS + ((phyAddr & 0x1F) >> 3));
	outData[10] = (regAddr & 0x00FF);
	outData[11] = ((regAddr & 0xFF00) >> 8);
	outData[12] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[13] = (SMI_READ + XMDIO_READ + ((phyAddr & 0x1F) >> 3));

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 14, &len, _timeout))
		return -1;

	// Get the 16-bit binary data from Bulk Endpoint 6
	unsigned char inData[2];
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 2, &len, _timeout))
		return -2;

	data = ((int)inData[1] << 8) + (int)inData[0];
	return data;
}
int MRegAccess_v61xx::XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data)
{
	// total EP2 data size 8 16-bit, 16 bytes
	unsigned char outData[16];

	// 4 bytes CFG2 data
	// set CFG2: XMDIO ST=00, Burst_Length = 2
	uint st_code = 0, smi_mode = 0, smi_port = SMIPort, burst_len = 2;
	outData[0] = SMIControlReg;
	outData[1] = 0x06 << 5;
	outData[2] = (((_last_TA_ext & 0x1) << 4) | ((smi_mode & 0x1) << 3) | (smi_port & 0x7));
	outData[3] = st_code << 3;

	// 4 bytes burstLen data
	outData[4] = BurstLen;
	outData[5] = 0x06 << 5;
	outData[6] = burst_len & 0x00FF;
	outData[7] = (burst_len & 0xFF00) >> 8;

	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[8] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[9] = (SMI_WRITE + XMDIO_ADDRESS + ((phyAddr & 0x1F) >> 3));
	outData[10] = (regAddr & 0x00FF);
	outData[11] = ((regAddr & 0xFF00) >> 8);
	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[12] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[13] = (SMI_WRITE + XMDIO_WRITE + ((phyAddr & 0x1F) >> 3));
	outData[14] = (data & 0x00FF);
	outData[15] = ((data & 0xFF00) >> 8);

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 16, &len, _timeout))
			return -1;

	return 0;
}

