// QdCppWrapper.cpp : source file that includes just the standard includes

#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "Wrapper.h"
#include "libMRegAccess.h"

HANDLE pDeviceHandle = NULL;
unsigned int  g_USBPort;

#define MAXSEMNUMBER 15
sem_t g_semlist[MAXSEMNUMBER];
static int g_semcount = 1;

int openDefaultUSBDriver()
{
	return InitDefaultUSBConn();
}
int openUSBDriver(char* usbId, int port = 0)
{
	return InitOneUSBConn(usbId);
}
void setUSBPort(int usbPort)
{
	g_USBPort = usbPort;
}

int readRegister(unsigned int phyAddr,unsigned int regAddr)
{
	return MDIOReadRegister(g_USBPort, phyAddr, regAddr);
}

int writeRegister(unsigned int phyAddr,unsigned int regAddr,unsigned int data)
{
	return MDIOWriteRegister(g_USBPort, phyAddr, regAddr, data);
}

void closeUSBDriver()
{ 
	return MRegLibRelease();
}

int semaphoreCreate(int count)
{
	int ret = 0;

	if(g_semcount >= MAXSEMNUMBER)
	{
		printf("Over the Max Semaphore numbers\n");
		return -3;
	}

	if(sem_init(&(g_semlist[g_semcount]), 0, count) != 0)
	{
		printf("Init semapore%d error\n", g_semcount);
		ret = -1;
	}
	else
	{
		ret = g_semcount;
		g_semcount = g_semcount + 1;		
	}

	return ret;
}
int semaphoreWait(unsigned int smid, unsigned long timeout)
{
	int ret = 0;
	struct timespec ts;
	
	if(smid >= MAXSEMNUMBER)
		return -1;

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec = ts.tv_sec + timeout/1000;

	ret = sem_timedwait(&(g_semlist[smid]), &ts);

	return ret;	
}
int semaphoreRelease(unsigned int smid)
{
	int ret = 0;
	
	if(smid >= MAXSEMNUMBER)
		return -1;

	ret = sem_post(&(g_semlist[smid]));

	return ret;
}
int semaphoreClose(unsigned int smid)
{
	int ret = 0;
	
	if(smid >= MAXSEMNUMBER)
		return -1;

	ret = sem_destroy(&(g_semlist[smid]));

	return ret;
}




