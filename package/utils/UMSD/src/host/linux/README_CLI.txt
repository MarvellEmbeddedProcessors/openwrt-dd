
              Telnet Command Line Interface of UMSD (Unified Marvell SOHO Driver) 
             =====================================================================
Contents:
1) General Introduction
-------------------------------
2) Test Environment
-------------------------------
3) How to - build program
-------------------------------
4) Test Procedure
-------------------------------
5) Test Display on screen
-------------------------------
6) How to - Use CLI command
-------------------------------


1) General Introduction
-------------------------------
CLI program emulates one telnet command line interface for UMSD API verification.
There will be one command for each UMSD API, and it validates both chip and API functions.
The program has been tested in Linux PC(Ubuntu 14.04 LTS 32/64bit) through Marvell USB2SMI adapter board or RMU setup.
This host application is used to verify the USMD APIs and depends on third party library “libusb” and “libpcap”. These third party libraries should be installed in the target system. You can use the following command for X86 Ubuntu system to install the libraries.
Note: If you are using embedded system, you should port the “libusb” and “libpcap” to the embedded system by yourself. 

2) Test Environment
-------------------------------
Hardware:
 <PC Linux> + <Marvell USB2SMI board>/<ethernet cable> + <Marvell evaluation board>.
Software:
 Linux telnet server programs :(In switch/host/linux/MCLI)
 
3) How to - build test program
-------------------------------
	3.1)Install libusb-1.0.9 for SMI or libpcap for RMU on Linux PC 
		Run command <sudo apt-get install libusb-1.0> on Ubuntu 14.04 LTS for example.
		Run command <sudo apt-get install libpcap0.8-dev> for example.
	3.2)Source the global building variables
		Switch to UMSD root folder and run command <source setenv>
	3.3)Build the test program
		Switch to host/linux folder and run command <make>

4) Test Procedure
-------------------------------
	4.1)For SMI:
		Connect PC USB to Marvell USB board through USB and connect Marvell USB board to testing evaluation board.
		For RMU:
		Setup the board as RMU mode and connect the PC with RMU port through ethernet cable.
	4.2)To run the test program using command <sudo ./MCLI>
	
5) Test Display on screen
-------------------------------	
	Interface List:
        0 : SMI Interface
        1 : SMI Multi-Chip
        2 : RMU Interface
	Please select one interface: 0
	Enter USB Port number(1 - 4):1
	Only one USB Adapter connect(y/n)?y
	Driver Load Success
	Listening on port 8888 for telnet

6) How to - Use CLI command
-------------------------------
	Note: Please using "Putty" as the telnet client terminal, more details please reference the "UMSD CLI User Manual.pdf"
	6.1) telnet to the server, then the screen will display as follows in the telnet client
		MCLI test environment

		MCLI>

	6.1) type help to show all the command list
		MCLI> help

		Commands available:
		  help                 Show available commands
		  exit                 Exit from current program
		  history              Show a list of previously run commands
		  atu                  Show atu commands
		  stu                  Show stu commands
		  vlan                 Show vlan commands
		  imp                  Show imp commands
		  phy                  Show phy commands
		  ...
		
	6.2)type command help to show the subcmd list
		MCLI> atu help
		atu help [subcmd]: Display atu command options
		atu addEntry [options] : Creates the new entry in MAC address table.
		atu flush <flushCMD> : flush all or all non-static atu entry
		atu flushInDB <flushCMD> <fid>: flush the atu entry based on the DB number
		atu move <moveCMD> <moveFrom> <moveTo>: move all or all non-static atu entry
		atu moveInDB <moveCMD> <fid> <moveFrom> <moveTo>: move the atu entry based on fid
		...
		
	6.3)type command help subcmd to show the detail information
		MCLI> atu help move
		atu move <moveCMD> <moveFrom> <moveTo>
				moveCMD:
						1: Move All
						2: Move All non static
				moveFrom: port where moving from
				moveTo  : port where moving to
		Example:
				atu move 1 2 3

		MCLI> atu help addEntry
		atu addEntry [options]
		[options]:options for atu entry, format as: -macAddr aabbccddee00 -state 7
				-macAddr    : mac address, format as aabbccddee00
				-lag        : trunk member or LAG
				-portVec    : port vector
				-fid        : ATU MAC Address Database number
				-state      : atu entry state
				-FPri       : MAC frame priority data
				-QPri       : MAC Queue priority data
		Example:
				atu addentry -macaddr 112233445566 -state 0xe -portvec 3 -fid 1
	
	6.4)type the command based on the help 
		MCLI> atu addEntry -state 7 -macAddr aabbccddee00	


	
	