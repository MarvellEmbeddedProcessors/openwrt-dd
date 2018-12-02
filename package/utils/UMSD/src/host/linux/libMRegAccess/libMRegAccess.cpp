#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cyusb.h"
#include "libMRegAccess.h"
#include "MRegAccessClass.h"
static MRegAccessBaseClass *regAccess = NULL;
static cyusb_handle *g_handle = NULL;
static const int g_interface = 0;
static const unsigned short vid = 0x1286;
static const unsigned short pid = 0x1fa4;

int InitDefaultUSBConn()
{
	int status = 0;
	int count = 0;
	int cpldversion = 0;

	count = cyusb_open(vid, pid);
	
	if(count == 1){
		g_handle = cyusb_gethandle(0);
		if(cyusb_kernel_driver_active(g_handle, 0) == 0){		
			cpldversion = ReadCPLDVersion();
			if(cpldversion >= 0x6100 && cpldversion < 0x6200){
				regAccess = new MRegAccess_v61xx(g_handle);
			}
			else if(cpldversion >= 0x6000 && cpldversion < 0x6100){
				regAccess = new MRegAccess_v6(g_handle);
			}
			else if(cpldversion >= 0x5000 && cpldversion < 0x6000){
				regAccess = new MRegAccess_v5(g_handle);
			}
			else{
				regAccess = new MRegAccessBaseClass(g_handle);
			}
		}
		else{
			status = -3;
		}
	}
	else{
		status = count;
	}

	return status;
}

int InitOneUSBConn(char *usb_did)
{
	int status = -1;
	int count = 0;
	int cpldversion = 0;
        int index;
        unsigned short bcdDevice;
        unsigned short temp;
        char did[8] = {};

        strncpy(did, usb_did, 6);
        temp = (unsigned short)strtol(did, NULL, 0);

	count = cyusb_open(vid, pid);
	
	for(index = 0; index < count; index++){
		g_handle = cyusb_gethandle(index);
                bcdDevice = cyusb_getdevice(g_handle);
                if(bcdDevice == temp){
			if(cyusb_kernel_driver_active(g_handle, 0) == 0){		
				cpldversion = ReadCPLDVersion();
				if(cpldversion >= 0x6100 && cpldversion < 0x6200){
					regAccess = new MRegAccess_v61xx(g_handle);
				}
				else if(cpldversion >= 0x6000 && cpldversion < 0x6100){
					regAccess = new MRegAccess_v6(g_handle);
				}
				else if(cpldversion >= 0x5000 && cpldversion < 0x6000){
					regAccess = new MRegAccess_v5(g_handle);
				}
				else{
					regAccess = new MRegAccessBaseClass(g_handle);
				}
                                status = 0;
			}
			else{
				status = -3;
			}
                 }
	}

	return status;
}


int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr)
{
	int data = 0;
	if(regAccess == NULL || g_handle == NULL)
		return -1;
	cyusb_claim_interface(g_handle, g_interface);
	data = regAccess->MDIOReadRegister(SMIPort, phyAddr, regAddr);
	cyusb_release_interface(g_handle, g_interface);
	return data;
}
int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data)
{
	int status = 0;
	if(regAccess == NULL || g_handle == NULL)
		return -1;
	cyusb_claim_interface(g_handle, g_interface);
	status = regAccess->MDIOWriteRegister(SMIPort, phyAddr, regAddr, data);
	cyusb_release_interface(g_handle, g_interface);

	return status;
}

int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr)
{
	int data = 0;
	if(regAccess == NULL || g_handle == NULL)
		return -1;
	cyusb_claim_interface(g_handle, g_interface);
	data = regAccess->XMDIOReadRegister(SMIPort, phyAddr, devAddr, regAddr);
	cyusb_release_interface(g_handle, g_interface);
	return data;
}
int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data)
{
	int status = 0;
	if(regAccess == NULL || g_handle == NULL)
		return -1;
	cyusb_claim_interface(g_handle, g_interface);
	status = regAccess->XMDIOWriteRegister(SMIPort, phyAddr, devAddr, regAddr, data);
	cyusb_release_interface(g_handle, g_interface);

	return status;
}



int ReadCPLDVersion()
{
	if(g_handle == NULL)
		return -1;
	
	int len, version;
	unsigned char data[2] = {0x00, 0x60};
	unsigned char refdata[4];
	
	cyusb_claim_interface(g_handle, g_interface);
	cyusb_bulk_transfer(g_handle, 0x02, data, 2, &len, 2000);
	cyusb_bulk_transfer(g_handle, 0x86, refdata, 4, &len, 2000);
	cyusb_release_interface(g_handle, g_interface);
	
	version = refdata[1] << 8 | refdata[0];
	
	return version;
}

void MRegLibRelease()
{
	cyusb_close();
	if(regAccess != NULL)
		delete regAccess;
}

