#include <msdCopyright.h>

/********************************************************************************
* msdUtils.c
*
* DESCRIPTION:
*       Collection of Utility functions
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdUtils.h>

#define SWAP16(a) (((((a) >> 0 ) & 0xff) << 8 ) | \
	((((a) >> 8) & 0xff) << 0))

#define SWAP32(a) (((((a) >> 0 ) & 0xff) << 24) | \
	((((a) >> 8) & 0xff) << 16) | \
	((((a) >> 16) & 0xff) << 8) | \
	((((a) >> 24) & 0xff) << 0))

/*******************************************************************************
* msdMemSet
*
* DESCRIPTION:
*       Set a block of memory
*
* INPUTS:
*       start  - start address of memory block for setting
*       simbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * msdMemSet
(
    IN void * start,
    IN int    symbol,
    IN MSD_U32 size
)
{
    MSD_U32 i;
    char* buf;
    
    buf = (char*)start;
        
    for(i=0; i<size; i++)
    {
        *buf++ = (char)symbol;
    }

    return start;
}

/*******************************************************************************
* msdMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * msdMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN MSD_U32       size
)
{
    MSD_U32 i;
    char* buf;
    char* src;
    
    buf = (char*)destination;
    src = (char*)source;
        
    for(i=0; i<size; i++)
    {
        *buf++ = *src++;
    }

    return destination;
}

/*******************************************************************************
* msdMemCmp
*
* DESCRIPTION:
*       Compares given memories.
*
* INPUTS:
*       src1 - source 1
*       src2 - source 2
*       size - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0, if equal.
*        negative number, if src1 < src2.
*        positive number, if src1 > src2.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int msdMemCmp
(
    IN char src1[],
    IN char src2[],
    IN MSD_U32 size
)
{
    MSD_U32 i;
    int value;

    for(i=0; i<size; i++)
    {
        if((value = (int)(src1[i] - src2[i])) != 0)
            return value; 
    }

    return 0;
}

/*******************************************************************************
* msdStrlen
*
* DESCRIPTION:
*       Determine the length of a string
* INPUTS:
*       source  - string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       size    - number of characters in string, not including EOS.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_U32 msdStrlen
(
    IN const void * source
)
{
    MSD_U32 i = 0;
    char* src;
    
    src = (char*)source;
        
    while(*src++) i++;

    return i;
}

MSD_BOOL msd_isBigEndian
(
)
{
	MSD_U32 data = 0x1;
	return ((MSD_U8*)(&data) == 0);
}

MSD_U32 msd_htonl
(
	MSD_U32 hostlong
)
{
	if (msd_isBigEndian())
		return hostlong;
	else
		return SWAP32(hostlong);
}

MSD_U16 msd_htons
(
	MSD_U16 hostshort
)
{
	if (msd_isBigEndian())
		return hostshort;
	else
		return SWAP16(hostshort);;
}

MSD_U32 msd_ntohl
(
	MSD_U32 netlong
)
{
	return msd_htonl(netlong);
}

MSD_U16 msd_ntohs
(
	MSD_U16 netshort
)
{
	return msd_htons(netshort);
}


/*******************************************************************************
* msdDelay
*
* DESCRIPTION:
*       Wait for the given uSec and return.
*        Current Switch devices with Gigabit Ethernet Support require 250 uSec
*        of delay time for PPU to be disabled.
*        Since this function is System and/or OS dependent, it should be provided
*        by each MSD user.
*
* INPUTS:
*       delayTime - delay in uSec.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/

static MSD_U8 msd32_2_8[256] = {
0,1,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,
50,51,52,53,54,55,56,57,58,59,
60,61,62,63,64,65,66,67,68,69,
70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,
90,91,92,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,107,108,109,
110,111,112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127,128,129,
130,131,132,133,134,135,136,137,138,139,
140,141,142,143,144,145,146,147,148,149,
150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,
170,171,172,173,174,175,176,177,178,179,
180,181,182,183,184,185,186,187,188,189,
190,191,192,193,194,195,196,197,198,199,
200,201,202,203,204,205,206,207,208,209,
210,211,212,213,214,215,216,217,218,219,
220,221,222,223,224,225,226,227,228,229,
230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,
250,251,252,253,254,255};

MSD_U8 msdLong2Char(MSD_U32 data)
{
    return msd32_2_8[data&0xff];
}

MSD_U8 msdShort2Char(MSD_U16 data)
{
    MSD_U32 dataL = data;
    return msd32_2_8[dataL&0xff];
}

