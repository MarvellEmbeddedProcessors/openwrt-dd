// QdCppWrapper.cpp : source file that includes just the standard includes

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vcclr.h>
#include <stdlib.h>
#include <string.h>
#include "Wrapper.h"
#using "./MRegLib.dll"
#using "./CyUSB.dll"

//typedef bool BOOLEAN;
//typedef void* HANDLE;

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace MRegLib;

#define INI_RW_MASK  0x00
#define _GCHANDLE_TO_VOIDPTR(x) ((GCHandle::operator System::IntPtr(x)).ToPointer())
#define _VOIDPTR_TO_GCHANDLE(x) (GCHandle::operator GCHandle(System::IntPtr(x)))

HANDLE pDeviceHandle = NULL;
unsigned int  g_USBPort;

int openDefaultUSBDriver()
{
	int err = 0;

	MRegMatlabAccessClass ^ oRegAccObj = gcnew MRegMatlabAccessClass();		
	err = oRegAccObj->InitDefaultUSBConn();
	
	if (0 != err)
		return err;    

	GCHandle gcHandle = GCHandle::Alloc(oRegAccObj);
	pDeviceHandle = _GCHANDLE_TO_VOIDPTR(gcHandle);
	return err;
}
int openUSBDriver(char* usbId, int port = 0)
//bool openUSBDriver(HANDLE* pDeviceHandle, char* usbId, int port = 0)
{
	int err = 0;

	//System::String^ usbId = gcnew System::String("0x0014:Hi-Speed");
	System::String^ usb = gcnew System::String(usbId);
	MRegMatlabAccessClass ^ oRegAccObj = gcnew MRegMatlabAccessClass();		
	err = oRegAccObj->InitOneUSBConn((System::String ^)usb, port);
	
	if (0 != err)
		return err;    
	//*pDeviceHandle = (HANDLE) oRegAccObj->getIntPtr();
	GCHandle gcHandle = GCHandle::Alloc(oRegAccObj);
	pDeviceHandle = _GCHANDLE_TO_VOIDPTR(gcHandle);
	return err;
}
void setUSBPort(int usbPort)
{
	g_USBPort = usbPort;
}

int readRegister(unsigned int phyAddr,unsigned int regAddr)
{
	MRegMatlabAccessClass^ regAccObj = nullptr;
	if(NULL != pDeviceHandle)
	{
		regAccObj = static_cast<MRegMatlabAccessClass^>( _VOIDPTR_TO_GCHANDLE(pDeviceHandle).Target);
	}
	return regAccObj -> MDIOReadRegister(g_USBPort, phyAddr, regAddr);
}

int writeRegister(unsigned int phyAddr,unsigned int regAddr,unsigned int data)
{
	MRegMatlabAccessClass^ regAccObj = nullptr;
	if(NULL != pDeviceHandle)
	{
		regAccObj = static_cast<MRegMatlabAccessClass^>( _VOIDPTR_TO_GCHANDLE(pDeviceHandle).Target);
	}
	return regAccObj -> MDIOWriteRegister(g_USBPort,phyAddr, regAddr, data);
}

int semaphoreCreate(int count)
{
	HANDLE ghSemaphore;

	ghSemaphore = CreateSemaphore(NULL, count, count, NULL);

	if(ghSemaphore == NULL)
	{
		printf("CreateSemaphore error %d\n", GetLastError());
		return -1;
	}
	return (unsigned int)ghSemaphore;
}

int semaphoreWait(unsigned int smid, unsigned long timeout)
{
	int ret = 0;
	HANDLE pSem = (void*)smid;
	DWORD res;
	
	if(timeout == 0)
	{
		timeout = INFINITE;
	}
	res = WaitForSingleObject(pSem, timeout);

	switch(res)
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			ret = -1;
			break;
		default:
			ret = -2;
			break;
	}

	return ret;
}
int semaphoreRelease(unsigned int smid)
{
	int ret = 0;
	HANDLE pSem = (void*)smid;

	if(!ReleaseSemaphore(pSem, 1, NULL))
	{
		printf("ReleaseSemaphore error: %d\n", GetLastError());
		ret = -1;
	}
	return ret;
}
int semaphoreClose(unsigned int smid)
{
	BOOL ret = 0;
	HANDLE pSem = (void*)smid;

	if(!CloseHandle(pSem))
	{
		printf("Close the Semaphore error\n");
		ret = -1;
	}

	return ret;
}
