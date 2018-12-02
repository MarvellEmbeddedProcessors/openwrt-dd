#include <stdio.h>
#include <string.h>
#include "cyusb.h"
#include "MRegAccessClass.h"

int MRegAccess_v5::MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr)
{
	int data = 0;
	unsigned char outData[2];
	outData[0] = 0x00;
	outData[1] = 0xEC;
	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	//sending out reading cmd
	outData[0] = (((phyAddr & 0x7) << 5) + regAddr);
	outData[1] = (SMI_READ + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// Get the 16-bit binary data from Bulk Endpoint 6
	unsigned char inData[4];
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;

	data = ((int)inData[1] << 8) + (int)inData[0];

	return data;
}
int MRegAccess_v5::MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data)
{
	//set CFG2
	unsigned char outData[4] = {0};
	outData[0] = 0x00;
	outData[1] = 0xEA;
	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	//sending out writing cmd
	outData[0] = (((phyAddr & 0x7) << 5) + regAddr);
	outData[1] = (SMI_WRITE + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	outData[2] = (data & 0xff);
	outData[3] = (data >> 8);
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 4, &len, _timeout))
	return -1;

	unsigned char inData[4];
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;
	return 0;
}

int MRegAccess_v5::XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr)
{
	int data = 0;
	//set CFG2
	// startcode = 00(bin), operationCode=00(bin)
	unsigned char outData[4] = {0};
	unsigned char inData[4] = {0};
	outData[0] = 0x00;
	outData[1] = 0xE0;
	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// step1: Send Write regAddr cmd
	outData[0] = (((phyAddr & 0x7) << 5) + devAddr);
	outData[1] = (SMI_WRITE + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	outData[2] = (regAddr & 0xff);
	outData[3] = (regAddr >> 8);
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 4, &len, _timeout))
		return -1;

	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;

	// step2: Send Read regAddr cmd
	// startcode = 00(bin), operationCode=11(bin)
	outData[0] = 0x00;
	outData[1] = 0xE6;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// construct regAddr read data command
	outData[0] = (((phyAddr & 0x7) << 5) + devAddr);
	outData[1] = (SMI_READ + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// step3: Get the 16-bit binary data from Bulk Endpoint 6
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;
	
	data = ((int)inData[1] << 8) + (int)inData[0];
	return data;
}
int MRegAccess_v5::XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data)
{
	//set CFG2
	// startcode = 00(bin), operationCode=00(bin)
	unsigned char outData[4] = {0};
	unsigned char inData[4] = {0};
	outData[0] = 0x00;
	outData[1] = 0xE0;
	int len;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// step1: Send Write regAddr cmd
	outData[0] = (((phyAddr & 0x7) << 5) + devAddr);
	outData[1] = (SMI_WRITE + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	outData[2] = (regAddr & 0xff);
	outData[3] = (regAddr >> 8);
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 4, &len, _timeout))
		return -1;

	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;

	// step2: Send Read regAddr cmd
	// startcode = 00(bin), operationCode=11(bin)
	outData[0] = 0x00;
	outData[1] = 0xE2;
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 2, &len, _timeout))
		return -1;

	// construct regAddr read data command
	//sending out writing cmd
	outData[0] = (((phyAddr & 0x7) << 5) + devAddr);
	outData[1] = (SMI_WRITE + (SMIPort << 2) + ((phyAddr & 0x18) >> 3));
	outData[2] = (data & 0xff);
	outData[3] = (data >> 8);
	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT2, outData, 4, &len, _timeout))
		return -1;

	if(cyusb_bulk_transfer(_usb_handle, ENDPOINT6, inData, 4, &len, _timeout))
		return -2;

	return 0;
}

