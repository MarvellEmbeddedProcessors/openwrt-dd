// MxdCppWrapper.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define ACTION_READ 	0
#define ACTION_WRITE 	1
#define ACTION_USB_WRITE 	2
#define ACTION_GPIO_READ    3
#define ACTION_GPIO_WRITE  4
#define ACTION_CPLD_READ    5
#define ACTION_CPLD_WRITE   6
#define ACTION_XREAD 	7
#define ACTION_XWRITE 	8


#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifdef __cplusplus 
    extern "C" {
#endif
//SMI Wrapper, using USB2SMI Adapter
extern int openDefaultUSBDriver();
extern int openUSBDriver(char* usbId, int port);
extern void setUSBPort(int usbPort);
extern int readRegister(unsigned int phyAddr,unsigned int regAddr);
extern int writeRegister(unsigned int phyAddr,unsigned int regAddr,unsigned int data);

//Semphore Wrapper
extern int semaphoreCreate(int count);
extern int semaphoreWait(unsigned int smid, unsigned long timeout);
extern int semaphoreRelease(unsigned int smid);
extern int semaphoreClose(unsigned int smid);

#ifdef __cplusplus 
    }
#endif
