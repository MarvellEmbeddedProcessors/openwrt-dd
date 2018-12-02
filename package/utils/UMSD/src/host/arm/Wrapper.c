#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 20
#define MAX_CMD_LENGTH 2048
#define MAX_ARGS_LENGTH 200

#define MV_PHY_CMD_REG 0
#define MV_PHY_DATA_REG 1
#define MV_SMIBUSY_BIT    15
#define MV_SMIFUNC_BIT    13
#define MV_SMIFUNC_INT    0
#define MV_SMIFUNC_EXT    1
#define MV_SMIFUNC_SIZE   2
#define MV_SMIMODE_BIT    12
#define MV_SMIOP_BIT      10
#define MV_SMIOP_SIZE     2
#define MV_SMIOP_READ     2
#define MV_SMIOP_WRITE    1
#define MV_DEVAD_BIT      5
#define MV_DEVAD_SIZE     5
#define MV_REGAD_BIT      0
#define MV_REGAD_SIZE     5

#define DEVICE "/dev/mvmdio-uio"

#define MV_MULTI_ADDRMODE 0   /* 0 - For single address mode */
#define MV_PHY_ID 3     /* smi addr of the switch in multi address mode */

static char *CMD_ARGS[MAX_ARGS] = {0};
static int nargs = 0;
static char prompt[100] = "CMD> ";

static int fd;

typedef struct s_mii_data {
	unsigned short phyId;
	unsigned short reg;
	unsigned short data;
} mii_data;

int smiOpen(void)
{
	fd = open(DEVICE, O_RDWR);
	if(!fd)
	{
		printf("Error: Cannot open device: %s, fd:%X\n", DEVICE, fd);
		return -1;
	}
	return 0;
}

void smiClose(void)
{
	if(fd)
		close(fd);
}

#if MV_MULTI_ADDRMODE
int readRegister(
	unsigned short devAddr, 
	unsigned short regAddr)
{
	int retVal;
	mii_data mii;

	/* Write to SMI Command Register */
	mii.phyId = MV_PHY_ID; //set to your phy's ID
	mii.reg   = MV_PHY_CMD_REG; //the register you want to read
	mii.data  = (1 << MV_SMIBUSY_BIT) | (MV_SMIFUNC_INT << MV_SMIFUNC_BIT) | 
		(1 << MV_SMIMODE_BIT) | (MV_SMIOP_READ << MV_SMIOP_BIT) |
		((devAddr & ~(1<<MV_DEVAD_SIZE)) << MV_DEVAD_BIT) | 
		((regAddr & ~(1<<MV_REGAD_SIZE)) << MV_REGAD_BIT) ; 

	retVal = write(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	/* Read from SMI Data Register */
	mii.phyId = MV_PHY_ID;
	mii.reg   = MV_PHY_DATA_REG;
	retVal = read(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}
	
	return (int)mii.data;
}

int writeRegister(
	unsigned short devAddr, 
	unsigned short regAddr, 
	unsigned short data)
{
	int retVal;
	mii_data mii;

	/* Write data to SMI Data Register */
	mii.phyId = MV_PHY_ID;
	mii.reg   = MV_PHY_DATA_REG;
	mii.data = data;
	retVal = write(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	/* Write to SMI Command Register */
	mii.phyId = MV_PHY_ID; //set to your phy's ID
	mii.reg   = MV_PHY_CMD_REG; //the register you want to read
	mii.data  = (1 << MV_SMIBUSY_BIT) | (MV_SMIFUNC_INT << MV_SMIFUNC_BIT) | 
		(1 << MV_SMIMODE_BIT) | (MV_SMIOP_WRITE << MV_SMIOP_BIT) |
		((devAddr & ~(1<<MV_DEVAD_SIZE)) << MV_DEVAD_BIT) | 
		((regAddr & ~(1<<MV_REGAD_SIZE)) << MV_REGAD_BIT) ; 

	retVal = write(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	return retVal;
}
#else
int readRegister(
	unsigned short devAddr, 
	unsigned short regAddr)
{
	int retVal;
	mii_data mii;

	mii.phyId = devAddr;
	mii.reg   = regAddr;
	retVal = read(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	return (int)mii.data;
}

int writeRegister(
	unsigned short devAddr, 
	unsigned short regAddr, 
	unsigned short data)
{
	int retVal;
	mii_data mii;

	/* Write to SMI Command Register */
	mii.phyId = devAddr; 
	mii.reg   = regAddr; 
	mii.data = data;

	retVal = write(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	return retVal;
}
#endif

