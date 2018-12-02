#ifndef __MREGACCESSCLASS_H_H_
#define __MREGACCESSCLASS_H_H_
#include "cyusb.h"

class MRegAccessBaseClass{
private:
	
protected:
	cyusb_handle *_usb_handle;
	unsigned int _timeout;
	typedef enum _ENDPOINT{
		ENDPOINT2 = 0x02,
		ENDPOINT6 = 0x86,
		ENDPOINT4 = 0x04,
		ENDPOINT8 = 0x88
	}ENPOINT;
	typedef enum _CMD{
		GPIO_WRITE = 0 << 5,
		GPIO_READ = 1 << 5,
		WRITE_USB_CTL_REGISTER = 2 << 5,
		READ_CPLD_VERSION = 3 << 5,
		SMI_WRITE = 4 << 5,
		SMI_READ = 5 << 5,
		WRITE_CFG1_REGISTER = 6 << 5,
		WRITE_CFG2_REGISTER = 7 << 5,
		
		GPIO_SNIFFER_START = 3 << 4,
		GPIO_SNIFFER_STOP = 2 << 4,
		GPIO_SNIFFER_CFG = 6 << 5
	}CMD;
	
	typedef enum _MDIOType{
		MDIO_WRITE = 0 << 2,
		MDIO_READ = 1 << 2,
		XMDIO_ADDRESS = 2 << 2,
		XMDIO_WRITE = 3 << 2,
		XMDIO_READ = 4 << 2,
		XMDIO_READ_INC = 5 << 2
	}MDIOType;
public:
	MRegAccessBaseClass(cyusb_handle *usb_handle){
		_usb_handle = usb_handle;
		_timeout = 2000;
	}
	virtual int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr){};
	virtual int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data){};
	
	virtual int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr){};
	virtual int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data){};
	
	virtual ~MRegAccessBaseClass(){};
};

class MRegAccess_v61xx : public MRegAccessBaseClass {
private:
	unsigned int _last_TA_ext;
	typedef enum _FPGARegMap{
		Config1 = 0,
		Config2,
		Control1,
		Contrl2,
		ClkDiv,
		VDD0,
		PreAmble,
		PostAmble,
		SMIControlReg,
		BurstLen,
		StatusReg,
		GPIORWMask0
	}FPGARegMap;
public:
	MRegAccess_v61xx(cyusb_handle *usb_handle):MRegAccessBaseClass(usb_handle){
		_usb_handle = usb_handle;
		_last_TA_ext = 1;
	}
	int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr);
	int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data);
	
	int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr);
	int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data);
};

class MRegAccess_v6 : public MRegAccessBaseClass {
public:
	MRegAccess_v6(cyusb_handle *usb_handle):MRegAccessBaseClass(usb_handle){
		_usb_handle = usb_handle;
	}
	int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr);
	int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data);
	
	int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr);
	int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data);
};

class MRegAccess_v5 : public MRegAccessBaseClass {
public:
	MRegAccess_v5(cyusb_handle *usb_handle):MRegAccessBaseClass(usb_handle){
		_usb_handle = usb_handle;
	}
	int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr);
	int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data);
	
	int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr);
	int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data);
};

#endif
