#ifndef __MREGACCESSLIB_H_H_
#define __MREGACCESSLIB_H_H_
/*********************************************************************************\
 * This is the main header file for MRegLib for Embedded Linux system		      *
 *                                                                                *
 * Author              :        Guangwei Zhou ( zhougw@marvell.com )              *
 * Copyright           :        Marvell Semiconductors Inc. / Datacomm            *
 * Date written        :        June 20, 2014                                     *
 * Modification History:                                                          *
 *    1. Create, version 0.0.1 June 20, 2014                                      *
 *       Only one USB2MI Adapter Connect support								  *
 *       v61xx v5 and v6 USB2MI Adapter support	                                      *
 *       ReadCPLDVersion support									              *
 *       Only single MDIO/XMDIO Read/Write support								  *
 *       											                              *
 \********************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/* Function prototypes */
/*******************************************************************************************
  Prototype    : int InitDefaultUSBConn();
  Description  : Init USB connect, only support one USB2MI connect, will get the usb device
                 handle and create suitable derived class object
                 Init USB must be called before any other operation
  Parameters   : None
  Return Value : 0 on success, negative value on error.
 *******************************************************************************************/
int InitDefaultUSBConn();
/*******************************************************************************************
  Prototype    : int InitOneUSBConn();
  Description  : Init one USB connect, only support one USB2MI connect, will get the usb device
                 handle and create suitable derived class object
                 Init USB must be called before any other operation
  Parameters   : None
  Return Value : 0 on success, negative value on error.
 *******************************************************************************************/
int InitOneUSBConn(char *usb_did);
/*******************************************************************************************
  Prototype    : int ReadCPLDVersion();
  Description  : This function read back the USB2MI CPLD version
  Parameters   : None
  Return Value : Returns the 16-bit CPLD version of the Adapter or negative value on error
 *******************************************************************************************/
int ReadCPLDVersion();
/*******************************************************************************************
  Prototype    : int MRegLibRelease();
  Description  : This function delete the global MRegAccessClass
  Parameters   : None
  Return Value : None
 *******************************************************************************************/
void MRegLibRelease();
/*******************************************************************************************
  Prototype    : int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr);
  Description  : Single MDIO Register Read
  Parameters   : unsigned int SMIPort	:	USB2MI port, range form 0 to 3
                 unsigned int phyAddr	:	16-bit phy address
                 unsigned int regAddr	:	16-bit register address
  Return Value : 16-bit read back register value or negative value on error
 *******************************************************************************************/
int MDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr);
/*******************************************************************************************
  Prototype    : int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data);
  Description  : Single MDIO Register Write
  Parameters   : unsigned int SMIPort	:	USB2MI port, range form 0 to 3
                 unsigned int phyAddr	:	16-bit phy address
                 unsigned int regAddr	:	16-bit register address
                 unsigned int data	:	16-bit data to write
  Return Value : 0 on success, negative value on error
 *******************************************************************************************/
int MDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int regAddr, unsigned int data);
/*******************************************************************************************
  Prototype    : int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr);
  Description  : Single XMDIO Register Read
  Parameters   : unsigned int SMIPort	:	USB2MI port, range form 0 to 3
                 unsigned int phyAddr	:	16-bit phy address
                 unsigned int devAddr	:	16-bit device address
                 unsigned int regAddr	:	16-bit register address
  Return Value : 16-bit read back register value or negative value on error
 *******************************************************************************************/
int XMDIOReadRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr);
/*******************************************************************************************
  Prototype    : int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data);
  Description  : Single XMDIO Register Write
  Parameters   : unsigned int SMIPort	:	USB2MI port, range form 0 to 3
                 unsigned int phyAddr	:	16-bit phy address
                 unsigned int devAddr	:	16-bit device address
                 unsigned int regAddr	:	16-bit register address
                 unsigned int data	:	16-bit data to write
  Return Value : 0 on success, negative value on error
 *******************************************************************************************/
int XMDIOWriteRegister(unsigned int SMIPort, unsigned int phyAddr, unsigned int devAddr, unsigned int regAddr, unsigned int data);
#ifdef __cplusplus
}
#endif
#endif

