// MxdCppWrapper.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifdef __cplusplus 
    extern "C" {
#endif
extern int openDefaultUSBDriver();
extern int openUSBDriver(char* usbId, int port);
extern void setUSBPort(int usbPort);
extern int readRegister(unsigned int phyAddr,unsigned int regAddr);
extern int writeRegister(unsigned int phyAddr,unsigned int regAddr,unsigned int data);
extern void closeUSBDriver();

extern int semaphoreCreate(int count);
extern int semaphoreWait(unsigned int smid, unsigned long timeout);
extern int semaphoreRelease(unsigned int smid);
extern int semaphoreClose(unsigned int smid);

#ifdef __cplusplus 
    }
#endif
