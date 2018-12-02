#include <stdio.h>
#include <string.h>
#include "cyusb.h"
#include "MRegAccessClass.h"

int MRegAccess_v6::MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr)
{
	int data = 0;
	unsigned char outData[6] = {0};
	// MDIO ST=01
	uint st_code = 0x1, smi_mode = 0, io_port = SMIPort, burst_len = 1;
	// Write CFG2 Register (uint st_code, uint smi_mode, uint io_port, uint burst_len)---bit[4] for (io_port & 0x8) <<1
	outData[0] = (((smi_mode & 0x1) << 3) + (io_port & 0x7) + ((io_port & 0x8) << 1));
	outData[1] = (WRITE_CFG2_REGISTER + ((st_code & 0x3) << 3));
	outData[2] = (burst_len & 0x00FF);
	outData[3] = ((burst_len & 0xFF00) >> 8);
	// read SMIMode0 
	outData[4] = (((phyAddr & 0x7) << 5) + (regAddr & 0x1F));
	outData[5] = (SMI_READ + MDIO_READ + ((phyAddr & 0x1F) >> 3));
	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 6, &len, _timeout))
		return -1;
    
	// Get the 16-bit binary data from Bulk Endpoint 6
	unsigned char inData[2];
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 2, &len, _timeout))
		return -2;

	data = ((int)inData[1] << 8) + (int)inData[0];

	return data;
}
int MRegAccess_v6::MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data)
{
	// MDIO ST=01
	unsigned char outData[8] = {0};
	uint st_code = 0x1, smi_mode = 0, io_port = SMIPort, burst_len = 1;
	outData[0] = (((smi_mode & 0x1) << 3) + (io_port & 0x7) + ((io_port & 0x8) << 1));
	outData[1] = (WRITE_CFG2_REGISTER + ((st_code & 0x3) << 3));
	outData[2] = (burst_len & 0x00FF);
	outData[3] = ((burst_len & 0xFF00) >> 8);
	//write SMIMode0 (uint type, uint phy_addr, uint reg_addr, uint data)
	outData[4] = (((phyAddr & 0x7) << 5) + (regAddr & 0x1F));
	outData[5] = (SMI_WRITE + MDIO_WRITE + ((phyAddr & 0x1F) >> 3));
	outData[6] = (data & 0x00FF);
	outData[7] = ((data & 0xFF00) >> 8);

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 8, &len, _timeout))
		return -1;

	return 0;
}

int MRegAccess_v6::XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr)
{
	int data = 0;
	// set CFG2: XMDIO ST=00, Burst_Length = 2
	unsigned char outData[10] = {0};
	uint st_code = 0, smi_mode = 0, io_port = SMIPort, burst_len = 2;
	//WriteCFG2Register(st_code, smi_mode, io_port, burst_len)
	outData[0] = (((smi_mode & 0x1) << 3) + (io_port & 0x7) + ((io_port & 0x8) << 1));
	outData[1] = (WRITE_CFG2_REGISTER + ((st_code & 0x3) << 3));
	outData[2] = (burst_len & 0x00FF);
	outData[3] = ((burst_len & 0xFF00) >> 8);
	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[4] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[5] = (SMI_WRITE + XMDIO_ADDRESS + ((phyAddr & 0x1F) >> 3));
	outData[6] = (regAddr & 0x00FF);
	outData[7] = ((regAddr & 0xFF00) >> 8);
	//readSMIMode0(uint type, uint phy_addr, uint reg_addr)           
	outData[8] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[9] = (SMI_READ + XMDIO_READ + ((phyAddr & 0x1F) >> 3));
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
int MRegAccess_v6::XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data)
{
	// set CFG2: XMDIO ST=00, Burst_Length = 2
	unsigned char outData[12] = {0};
	uint st_code = 0, smi_mode = 0, io_port = SMIPort, burst_len = 2;
	//WriteCFG2Register(uint st_code, uint smi_mode, uint io_port, uint burst_len)
	outData[0] = (((smi_mode & 0x1) << 3) + (io_port & 0x7) + ((io_port & 0x8) << 1));
	outData[1] = (WRITE_CFG2_REGISTER + ((st_code & 0x3) << 3));
	outData[2] = (burst_len & 0x00FF);
	outData[3] = ((burst_len & 0xFF00) >> 8);
	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[4] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[5] = (SMI_WRITE + XMDIO_ADDRESS + ((phyAddr & 0x1F) >> 3));
	outData[6] = (regAddr & 0x00FF);
	outData[7] = ((regAddr & 0xFF00) >> 8);
	//writeSMIMode0(uint type, uint phy_addr, uint reg_addr, uint data)
	outData[8] = (((phyAddr & 0x7) << 5) + (devAddr & 0x1F));
	outData[9] = (SMI_WRITE + XMDIO_WRITE + ((phyAddr & 0x1F) >> 3));
	outData[10] = (data & 0x00FF);
	outData[11] = ((data & 0xFF00) >> 8);

	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 12, &len, _timeout))
			return -1;

	return 0;
}

