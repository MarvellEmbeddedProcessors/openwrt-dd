#include <msdCopyright.h>
/********************************************************************************
* debug.c
*
* DESCRIPTION:
*       Debug message display routine
*
* DEPENDENCIES:
*       OS Dependent
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <msdApiTypes.h>
#include <msdUtils.h>

static MSD_STRING_PRINT_CALLBACK printCallback = NULL;
void msdSetStringPrintCallback(MSD_STRING_PRINT_CALLBACK callback)
{
    printCallback = callback;
}

#ifdef MSD_DEBUG_QD
MSD_DBG_LEVEL dbg_level = MSD_DBG_OFF_LVL; /* Set default debug level */
#endif
#if defined(LINUX)
 #ifdef __KERNEL__
  #include "kernel.h"
 #else
  #include "stdio.h"
  #include "stdarg.h"
 #endif
#endif

/*******************************************************************************
* msdDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
#if defined(LINUX)
void msdDbgPrint(char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);

    vsprintf(dbgStr, format, argP);
#if defined(LINUX)
 #ifdef __KERNEL__
    if (printCallback != NULL)
        printCallback(dbgStr);
    else
        printk("%s", dbgStr);
 #else
    if (printCallback != NULL)
        printCallback(dbgStr);
    else
        printf("%s", dbgStr);
 #endif
#endif
    return;
}
#else
#include "stdio.h"
#include "stdarg.h"
void msdDbgPrint(char* format, ...)
{
    MSD_UNUSED_PARAM(format);
	va_list argP;
	char dbgStr[1000] = "";

	va_start(argP, format);

	vsprintf_s(dbgStr, 1000, format, argP);
    if (printCallback != NULL)
        printCallback(dbgStr);
    else
        printf("%s", dbgStr);
}
#endif


char* msdDisplayStatus(MSD_STATUS status)
{
	char * str;
	switch (status)
	{
	case MSD_OK:
		str = "MSD_OK";
		break;
	case MSD_FAIL:
		str = "MSD_FAIL";
		break;
	case MSD_BAD_PARAM:
		str = "MSD_BAD_PARAM";
		break;
	case MSD_NO_SUCH:
		str = "MSD_NO_SUCH";
		break;
	case MSD_NOT_SUPPORTED:
		str = "MSD_NOT_SUPPORTED";
		break;
	case MSD_ALREADY_EXIST:
		str = "MSD_ALREADY_EXIST";
		break;
	case MSD_BAD_CPU_PORT:
		str = "MSD_BAD_CPU_PORT";
		break;
	default:
		str = "UnknowStatus";
		break;
	}
	return str;
}

