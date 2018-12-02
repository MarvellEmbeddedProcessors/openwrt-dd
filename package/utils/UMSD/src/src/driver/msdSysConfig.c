#include <msdCopyright.h>

/********************************************************************************
* msdSysConfig.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <msdSysConfig.h>
#include <msdDevObj.h>
#include <msdSem.h>
#include <msdHwAccess.h>
#include <msdDevObj.h>
#include <msdUtils.h>

static MSD_QD_DEV sohoDevs[MAX_SOHO_DEVICES];
static MSD_QD_DEV_PTR sohoDevsPtr[MAX_SOHO_DEVICES];

static MSD_BOOL msdRegister(MSD_QD_DEV *qd_dev, MSD_BSP_FUNCTIONS* pBSPFunctions);

/*******************************************************************************
* msdLoadDriver
*
* DESCRIPTION:
*       QuarterDeck Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this QuarterDeck
*       device. This pointer (*dev) is then used for all the API functions. 
*
* INPUTS:
*       cfg  - Holds device configuration parameters provided by system software.
*
* OUTPUTS:
*       dev  - Holds device information to be used for each API call.
*
* RETURNS:
*       MSD_OK               - on success
*       MSD_FAIL             - on error
*       MSD_ALREADY_EXIST    - if device already started
*       MSD_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*     msdUnLoadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
MSD_STATUS msdLoadDriver
(
    IN  MSD_SYS_CONFIG   *cfg
)
{

	MSD_U16 		value;
	MSD_BOOL		flag = MSD_TRUE;
	MSD_QD_DEV	*dev;

    MSD_DBG_INFO(("msdLoadDriver Called.\n"));

    /* Check for parameters validity        */
	/*
	if (dev == NULL)
    {
        MSD_DBG_INFO(("Failed.\n"));
        return MSD_BAD_PARAM;
    }
	*/

    /* Check for parameters validity        */
	if (cfg == NULL)
    {
        MSD_DBG_ERROR(("Input param cfg is NULL.\n"));
        return MSD_BAD_PARAM;
    }

	if (cfg->devNum < 0 || cfg->devNum >= MAX_SOHO_DEVICES)
	{
		MSD_DBG_ERROR(("Input devNum %d out of range. It should be within [0, %d]\n", cfg->devNum, (int)(MAX_SOHO_DEVICES - 1)));
		return MSD_BAD_PARAM;
	}

    /* The initialization was already done. */
	/*
	if (dev->devEnabled)
    {
        MSD_DBG_INFO(("QuarterDeck already started.\n"));
        return MSD_ALREADY_EXIST;
    }
	*/
	if (sohoDevsPtr[cfg->devNum] != NULL && sohoDevsPtr[cfg->devNum]->devEnabled)
	{
		MSD_DBG_INFO(("Soho Device already started.\n"));
		return MSD_ALREADY_EXIST;
	}
  
	/*Initialize dev structure*/
	msdMemSet((void*)(&(sohoDevs[cfg->devNum])), 0, sizeof(MSD_QD_DEV));
	dev = &(sohoDevs[cfg->devNum]);
	sohoDevsPtr[cfg->devNum] = dev;
	dev->devNum = cfg->devNum;
    dev->InterfaceChannel = cfg->InterfaceChannel;
    dev->rmuMode = cfg->rmuMode;
    dev->eTypeValue = cfg->eTypeValue;

	if (msdRegister(dev, &(cfg->BSPFunctions)) != MSD_TRUE)
    {
		MSD_DBG_ERROR(("msdRegister Failed.\n"));
       return MSD_FAIL;
    }

    dev->phyAddr = (MSD_U8)cfg->baseAddr;
    dev->baseRegAddr = 0x0;
	dev->basePhyRegAddr = 0;

	dev->reqSeqNum = 0;

    /* Initialize the MultiAddress Register Access semaphore.    */
	if ((dev->multiAddrSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("multiAddrSem semCreate Failed.\n"));
        msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }
    
    while (flag)
	{
        /* Init the device's config struct.             */
        if ((msdGetAnyReg(dev->devNum,  dev->baseRegAddr, MSD_REG_SWITCH_ID, &value)) != MSD_OK)
        {
			MSD_DBG_ERROR(("Get Device ID Failed.\n"));
            return MSD_FAIL;
        }

            dev->deviceId = value >> 4;
            dev->revision = (MSD_U8)value & 0xF;
	

        /* Initialize dev fields.         */
        dev->cpuPortNum = 0;
        dev->maxPhyNum = 5;
   
        dev->validSerdesVec = 0;
    
        switch (dev->deviceId)
        {
			case MSD_88E6352:
			case MSD_88E6172:
			case MSD_88E6176:
			case MSD_88E6240:
					dev->numOfPorts = 7;
					dev->maxPorts = 7;
					dev->maxPhyNum = 5;
					dev->validPortVec = (1 << dev->numOfPorts) - 1;
					/*dev->validPhyVec = (1 << dev->maxPhyNum) - 1;*/
					dev->validPhyVec = 0x7F;
					dev->basePhyRegAddr = 0;
					dev->devName = MSD_AGATE;
					break;
			case MSD_88E6320:			
			case MSD_88E6321:		
					dev->numOfPorts = 7;
					dev->maxPorts = 7;
					dev->maxPhyNum = 2;
					dev->validPortVec = (1 << dev->numOfPorts) - 1;
					/*dev->validPhyVec = (1 << dev->maxPhyNum) - 1;*/
					dev->validPhyVec = 0x18;
					dev->basePhyRegAddr = 0;
					dev->devName = MSD_PEARL;
					break;
		    case MSD_88E6141:
		    case MSD_88E6341:
                    dev->numOfPorts = 7;
                    dev->maxPorts = 7;
                    dev->maxPhyNum = 5;
                    dev->validPortVec = (1 << dev->numOfPorts) - 1;
                    /*dev->validPhyVec = (1 << dev->maxPhyNum) - 1;*/
				    dev->validPhyVec = 0x1E;
					dev->basePhyRegAddr = 0x10;
					dev->devName = MSD_TOPAZ;
                    break;        
        
            case MSD_88E6390X:
            case MSD_88E6390:
            case MSD_88E6190X:
            case MSD_88E6190:
            case MSD_88E6290:
                    dev->numOfPorts = 11;
                    dev->maxPorts = 11;
                    dev->maxPhyNum = 8;
                    dev->validPortVec = (1 << dev->numOfPorts) - 1;
                    /*dev->validPhyVec = (1 << dev->maxPhyNum) - 1;*/
                    dev->validPhyVec = 0x1FE;/*Internal GPHY Vector List*/
					dev->basePhyRegAddr = 0;
                    dev->devName = MSD_PERIDOT;
                    break;
            default:		
				if ((dev->baseRegAddr == 0x10) || (dev->baseRegAddr == 0x2))
				{
					if (dev->baseRegAddr == 0x3)
					{
						dev->devName = MSD_UNKNOWN;
						MSD_DBG_ERROR(("Unknown Device. Initialization failed.\n"));
						return MSD_FAIL;
					}
					else
						dev->baseRegAddr = 0x2;
				}
                else
                    dev->baseRegAddr = 0x10;
                break;
        }
        if (dev->devName != MSD_UNKNOWN)
            break;
    }

    /*
    dev->cpuPortNum = MSD_PORT_2_LPORT(cfg->cpuPortNum);

	if (dev->cpuPortNum == MSD_INVALID_PORT)
    {
		if (MSD_LPORT_2_PORT((MSD_LPORT)cfg->cpuPortNum) != MSD_INVALID_PORT)
        {
            dev->cpuPortNum = cfg->cpuPortNum;
        }
        else
        {
            return MSD_BAD_CPU_PORT;
        }
    }
    */

    /* Initialize the ATU semaphore.    */
	if ((dev->atuRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("atuRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the VTU semaphore.    */
	if ((dev->vtuRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("vtuRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the STATS semaphore.    */
	if ((dev->statsRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("statsRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the PIRL semaphore.    */
	if ((dev->pirlRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("pirlRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the PTP semaphore.    */
	if ((dev->ptpRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("ptpRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the PTP Mode semaphore.    */
    if ((dev->ptpModeSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
        MSD_DBG_ERROR(("ptpModeSem semCreate Failed.\n"));
        msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the PTP Tod semaphore.    */
    if ((dev->ptpTodSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
        MSD_DBG_ERROR(("ptpTodSem semCreate Failed.\n"));
        msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the Table semaphore.    */
	if ((dev->tblRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("tblRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the EEPROM Configuration semaphore.    */
	if ((dev->eepromRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("eepromRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    /* Initialize the PHY Device Register Access semaphore.    */
	if ((dev->phyRegsSem = msdSemCreate(dev->devNum, MSD_SEM_FULL)) == 0)
    {
		MSD_DBG_ERROR(("phyRegsSem semCreate Failed.\n"));
		msdUnLoadDriver(dev->devNum);
        return MSD_FAIL;
    }

    dev->devEnabled = 1;
    InitDevObj(dev);

    MSD_DBG_INFO(("msdLoadDriver Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* msdUnLoadDriver
*
* DESCRIPTION:
*
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK           - on success
*       MSD_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           msdLoadDriver().
*
*******************************************************************************/
MSD_STATUS msdUnLoadDriver
(
	IN MSD_U8	devNum
)
{
	MSD_QD_DEV* dev;

    MSD_DBG_INFO(("msdUnLoadDriver Called.\n"));

	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

    /* Delete the MultiAddress mode reagister access semaphore.    */
	if (msdSemDelete(devNum, dev->multiAddrSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("multiAddrSem semDelete Failed.\n"));
        return MSD_FAIL;
    }
 
    /* Delete the ATU semaphore.    */
	if (msdSemDelete(devNum, dev->atuRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("atuRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the VTU semaphore.    */
	if (msdSemDelete(devNum, dev->vtuRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("vtuRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the STATS semaphore.    */
	if (msdSemDelete(devNum, dev->statsRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("statsRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the PIRL semaphore.    */
	if (msdSemDelete(devNum, dev->pirlRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("pirlRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the PTP semaphore.    */
	if (msdSemDelete(devNum, dev->ptpRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("ptpRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the PTP Mode semaphore.    */
    if (msdSemDelete(devNum, dev->ptpModeSem) != MSD_OK)
    {
        MSD_DBG_ERROR(("ptpModeSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the PTP Tod semaphore.    */
    if (msdSemDelete(devNum, dev->ptpTodSem) != MSD_OK)
    {
        MSD_DBG_ERROR(("ptpTodSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the Table semaphore.    */
	if (msdSemDelete(devNum, dev->tblRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("tblRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the EEPROM Configuration semaphore.    */
	if (msdSemDelete(devNum, dev->eepromRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("eepromRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

    /* Delete the PHY Device semaphore.    */
	if (msdSemDelete(devNum, dev->phyRegsSem) != MSD_OK)
    {
		MSD_DBG_ERROR(("phyRegsSem semDelete Failed.\n"));
        return MSD_FAIL;
    }

	msdMemSet((void*)dev, 0, sizeof(MSD_QD_DEV));

	MSD_DBG_INFO(("msdUnLoadDriver Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* msdSetDriverInterface
*
* DESCRIPTION:
*       This function set the interface channel.
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*       channel -   interface channel(smi, smi multichip, rmu)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSetDriverInterface
(
    IN MSD_U8 devNum,
    IN MSD_INTERFACE channel
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	MSD_DBG_INFO(("msdSetDriverInterfaceSet Called.\n"));

    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    dev->InterfaceChannel = channel;

	MSD_DBG_INFO(("msdSetDriverInterfaceSet Called.\n"));
    return MSD_OK;
}

/*******************************************************************************
* msdGetDriverInterface
*
* DESCRIPTION:
*       This function get the current interface channel.
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*
* OUTPUTS:
*       channel -   interface channel(smi, smi multichip, rmu)
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdGetDriverInterface
(
    IN  MSD_U8 devNum,
    OUT MSD_INTERFACE *channel
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	MSD_DBG_INFO(("msdSetDriverInterfaceSet Called.\n"));

    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }
    *channel = dev->InterfaceChannel;

	MSD_DBG_INFO(("msdSetDriverInterfaceSet Called.\n"));
        return MSD_OK;
}

/*******************************************************************************
* sohoDevGet
*
* DESCRIPTION:
*       get the soho qdDev by devNumm
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       pointer to the soho qdDev   - on success.
*       NULL                        - on failure.
*
* COMMENTS:
*
*******************************************************************************/
MSD_QD_DEV* sohoDevGet
(
	IN  MSD_U8 devNum
)
{
	/* check range */
	if (devNum >= MAX_SOHO_DEVICES)
	{
		MSD_DBG_ERROR(("Failed. (Not supported devNum %d. It should be within [0, %d])", devNum, (int)(MAX_SOHO_DEVICES - 1)));
		return NULL;
	}
	return sohoDevsPtr[devNum];
}

/*******************************************************************************
* msdlport2port
*
* DESCRIPTION:
*       This function converts logical port number to physical port number
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdlport2port
(
IN MSD_U16    portVec,
IN MSD_LPORT     port
)
{
    MSD_U8    hwPort, tmpPort;

    tmpPort = hwPort = 0;

    while (portVec)
    {
        if (portVec & 0x1)
        {
            if ((MSD_LPORT)tmpPort == port)
                break;
            tmpPort++;
        }
        hwPort++;
        portVec >>= 1;
    }

    if (!portVec)
        hwPort = MSD_INVALID_PORT;

    return hwPort;
}

/*******************************************************************************
* msdport2lport
*
* DESCRIPTION:
*       This function converts physical port number to logical port number
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_LPORT msdport2lport
(
IN MSD_U16    portVec,
IN MSD_U8     hwPort
)
{
    MSD_U8        tmpPort, port;

    port = 0;

    if (hwPort == MSD_INVALID_PORT)
        return (MSD_LPORT)hwPort;

    if (!MSD_IS_PORT_SET(portVec, hwPort))
        return (MSD_LPORT)MSD_INVALID_PORT;

    for (tmpPort = 0; tmpPort <= hwPort; tmpPort++)
    {
        if (portVec & 0x1)
        {
            port++;
        }
        portVec >>= 1;
    }

    return (MSD_LPORT)port - 1;
}

/*******************************************************************************
* msdlportvec2portvec
*
* DESCRIPTION:
*       This function converts logical port vector to physical port vector
*
* INPUTS:
*        portVec - physical port list in vector
*        lVec     - logical port vector
* OUTPUTS:
*        None.
* RETURNS:
*       physical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U32 msdlportvec2portvec
(
IN MSD_U16    portVec,
IN MSD_U32     lVec
)
{
    MSD_U32    pVec, vec;

    pVec = 0;
    vec = 1;

    while (portVec)
    {
        if (portVec & 0x1)
        {
            if (lVec & 0x1)
            {
                pVec |= vec;
            }
            lVec >>= 1;
        }

        vec <<= 1;
        portVec >>= 1;
    }

    if (lVec)
        return MSD_INVALID_PORT_VEC;

    return pVec;
}

/*******************************************************************************
* msdportvec2lportvec
*
* DESCRIPTION:
*       This function converts physical port vector to logical port vector
*
* INPUTS:
*        portVec - physical port list in vector
*        pVec     - physical port vector
* OUTPUTS:
*        None.
* RETURNS:
*       logical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U32 msdportvec2lportvec
(
IN MSD_U16    portVec,
IN MSD_U32     pVec
)
{
    MSD_U32    lVec, vec;

    lVec = 0;
    vec = 1;

    while (portVec)
    {
        if (portVec & 0x1)
        {
            if (pVec & 0x1)
            {
                lVec |= vec;
            }
            vec <<= 1;
        }

        pVec >>= 1;
        portVec >>= 1;
    }

    return lVec;
}

/*******************************************************************************
* msdlport2phy
*
* DESCRIPTION:
*       This function converts logical port number to physical phy number.
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdlport2phy
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port
)
{
    MSD_U8    hwPort;

    if (dev->validPhyVec & (1 << port))
    {
        hwPort = MSD_LPORT_2_PORT(port);
		hwPort += dev->basePhyRegAddr;
    }
    else
		hwPort = (MSD_U8)MSD_INVALID_PHY;
		
    return hwPort;
}

/*******************************************************************************
* msdRegister
*
* DESCRIPTION:
*       BSP should register the following functions:
*        1) MII Read - (Input, must provide)
*            allows QuarterDeck driver to read QuarterDeck device registers.
*        2) MII Write - (Input, must provice)
*            allows QuarterDeck driver to write QuarterDeck device registers.
*        3) Semaphore Create - (Input, optional)
*            OS specific Semaphore Creat function.
*        4) Semaphore Delete - (Input, optional)
*            OS specific Semaphore Delete function.
*        5) Semaphore Take - (Input, optional)
*            OS specific Semaphore Take function.
*        6) Semaphore Give - (Input, optional)
*            OS specific Semaphore Give function.
*        Notes: 3) ~ 6) should be provided all or should not be provided at all.
*
* INPUTS:
*        pBSPFunctions - pointer to the structure for above functions.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_TRUE, if input is valid. MSD_FALSE, otherwise.
*
* COMMENTS:
*       This function should be called only once.
*
*******************************************************************************/
static MSD_BOOL msdRegister(MSD_QD_DEV *dev, MSD_BSP_FUNCTIONS* pBSPFunctions)
{

	dev->rmu_tx_rx = pBSPFunctions->rmu_tx_rx;

    dev->fgtReadMii =  pBSPFunctions->readMii;
    dev->fgtWriteMii = pBSPFunctions->writeMii;
    
    dev->semCreate = pBSPFunctions->semCreate;
    dev->semDelete = pBSPFunctions->semDelete;
    dev->semTake   = pBSPFunctions->semTake  ;
    dev->semGive   = pBSPFunctions->semGive  ;
    
    return MSD_TRUE;
}
