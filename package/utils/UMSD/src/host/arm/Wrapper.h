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

int smiOpen(void);
int smiClose(void);
int readRegister(unsigned int phyAddr,unsigned int regAddr);
int writeRegister(unsigned int phyAddr,unsigned int regAddr,unsigned int data);

#ifdef __cplusplus 
    }
#endif
