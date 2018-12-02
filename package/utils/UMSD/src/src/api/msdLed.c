#include <msdCopyright.h>

/********************************************************************************
* msdLed.c
*
* DESCRIPTION:
*       API definitions for LED control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdLed.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

/*******************************************************************************
*   msdLedForceOn
*
* DESCRIPTION:
*       This routine is to Force Port LED On
*
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       ledNum  - LED Index 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Topaz: 
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility 
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to 
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   msdLedForceOn
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.LEDObj.gledForceOn != NULL)
        return dev->SwitchDevObj.LEDObj.gledForceOn(dev, portNum, ledNum);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
*   msdLedForceOff
*
* DESCRIPTION:
*       This routine is to Force Port LED Off
*
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   msdLedForceOff
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.LEDObj.gledForceOff != NULL)
        return dev->SwitchDevObj.LEDObj.gledForceOff(dev, portNum, ledNum);
    else
        return MSD_NOT_SUPPORTED;
}

/*******************************************************************************
*   msdLedLinkAct
*
* DESCRIPTION:
*       This routine is to set Port LED mode to link/act. That is:
*           off - no link, on - link, blink = activity.
*
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   msdLedLinkAct
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.LEDObj.gledLinkAct != NULL)
        return dev->SwitchDevObj.LEDObj.gledLinkAct(dev, portNum, ledNum);
    else
        return MSD_NOT_SUPPORTED;
}

