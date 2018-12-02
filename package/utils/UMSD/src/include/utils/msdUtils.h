#include <msdCopyright.h>
/*******************************************************************************
*              Copyright 2017, MARVELL SEMICONDUCTOR, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* msdUtils.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Utils.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdUtils_h
#define __msdUtils_h

#include <msdApiTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Common Utils Types			 			                                   */
/****************************************************************************/
#define MSD_INVALID_PHY             0xFF
#define MSD_INVALID_PORT            0xFF
#define MSD_INVALID_PORT_VEC        0xFFFFFFFF

#define MSD_UNUSED_PARAM(_a)	(_a)=(_a)

void msdDbgPrint(char* format, ...);
#define MSG(x)             msdDbgPrint x      /* macro for message dump, like atu/vtu/mib...dump */
typedef void(*MSD_STRING_PRINT_CALLBACK)(const char *string);
void msdSetStringPrintCallback(MSD_STRING_PRINT_CALLBACK callback);

#ifdef MSD_DEBUG_QD
typedef enum 
{
	MSD_DBG_OFF_LVL = 0,
	MSD_DBG_ERR_LVL,
	MSD_DBG_INF_LVL,
	MSD_DBG_ALL_LVL
} MSD_DBG_LEVEL;

extern MSD_DBG_LEVEL dbg_level;
/*******************************************************************************
* msdDbgPrint
*
* DESCRIPTION:
*       This function prints out debug message depending on input debug_level.
*
* INPUTS:
*       debug_level 
*          MSD_DBG_OFF_LVL - prints nothing
*          MSD_DBG_ERR_LVL - for error messages
*          MSD_DBG_INF_LVL - for trace messages
*          MSD_DBG_ALL_LVL - for all messages
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

#define MSD_DBG_ERROR(x)       if(dbg_level >= MSD_DBG_ERR_LVL) \
                               msdDbgPrint("(ERROR ): "); \
                           if(dbg_level >= MSD_DBG_ERR_LVL) \
                               msdDbgPrint x  /* macro for error messages */
#define MSD_DBG_INFO(x)        if(dbg_level >= MSD_DBG_INF_LVL) \
                               msdDbgPrint("(TRACE ): "); \
                           if(dbg_level >= MSD_DBG_INF_LVL) \
	                           msdDbgPrint x  /* macro for trace messages */
#define MSD_DBG(x)             if(dbg_level >= MSD_DBG_ALL_LVL) \
                               msdDbgPrint x  /* macro for informational messages */

#else
#define MSD_DBG_ERROR(x)
#define MSD_DBG_INFO(x)
#define MSD_DBG(x)
#endif /* MSD_DEBUG */

char* msdDisplayStatus(MSD_STATUS status);

/****************************************************************************/
/* Exported Common Utils Functions		 			                                   */
/****************************************************************************/

#define MSD_IS_PORT_SET(_portVec, _port)    \
            ((_portVec) & (0x1 << (_port)))

/* The following macro converts a binary    */
/* value (of 1 bit) to a boolean one.       */
/* 0 --> MSD_FALSE                           */
/* 1 --> MSD_TRUE                            */
#define MSD_BIT_2_BOOL(binVal,boolVal)                                  \
            (boolVal) = (((binVal) == 0) ? MSD_FALSE : MSD_TRUE)

/* The following macro converts a boolean   */
/* value to a binary one (of 1 bit).        */
/* MSD_FALSE --> 0                           */
/* MSD_TRUE --> 1                            */
#define MSD_BOOL_2_BIT(boolVal,binVal)                                  \
            (binVal) = (((boolVal) == MSD_FALSE) ? 0 : 1)

/* The following macro converts a binary    */
/* value (of 1 bit) to a boolean one.       */
/* 0 --> MSD_TRUE                            */
/* 1 --> MSD_FALSE                           */
#define MSD_BIT_2_BOOL_R(binVal,boolVal)                                  \
            (boolVal) = (((binVal) == 0) ? MSD_TRUE : MSD_FALSE)

/* The following macro converts a boolean   */
/* value to a binary one (of 1 bit).        */
/* MSD_FALSE --> 1                           */
/* MSD_TRUE --> 0                            */
#define MSD_BOOL_2_BIT_R(boolVal,binVal)                                  \
            (binVal) = (((boolVal) == MSD_TRUE) ? 0 : 1)

/* This macro checks for an broadcast mac address     */
#define MSD_IS_BROADCAST_MAC(mac) (((mac).arEther[0] == 0xFF) && ((mac).arEther[1] == 0xFF) && ((mac).arEther[2] == 0xFF) && ((mac).arEther[3] == 0xFF) && ((mac).arEther[4] == 0xFF) && ((mac).arEther[5]


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
);

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
);


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
);

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
);

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
void msdDelay
(
    IN const unsigned int delayTime
);

MSD_U32 msd_htonl
(
	MSD_U32 hostlong
);

MSD_U16 msd_htons
(
	MSD_U16 hostshort
);

MSD_U32 msd_ntohl
(
	MSD_U32 netlong
);

MSD_U16 msd_ntohs
(
	MSD_U16 netshort
);

/*******************************************************************************
* msdLong2Char
*
* DESCRIPTION:
*       This function converts long value to char value
*
* INPUTS:
*       data - long value

* OUTPUTS:
*       MSD_U8 - char value
*        
* RETURNS:
*       logical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdLong2Char
(
	MSD_U32 data
);

/*******************************************************************************
* msdShort2Char
*
* DESCRIPTION:
*       This function converts short value to char value
*
* INPUTS:
*       data - short value

* OUTPUTS:
*       MSD_U8 - char value
*        
* RETURNS:
*       logical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdShort2Char
(
	MSD_U16 data
);


#ifdef __cplusplus
}
#endif

#endif /* __msdUtils_h */
