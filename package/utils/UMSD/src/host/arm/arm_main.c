#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "Wrapper.h"
#include "msdApi.h"
#include "rmuPcap.h"
#ifdef WIN32
#include <Winsock2.h>  
#include <Iptypes.h>  
#include <IPHlpApi.h>
#else
#include   <sys/ioctl.h> 
#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <net/if.h> 
#endif

extern MSD_QD_DEV       *qddev;
extern char    pcap_device_name[512];
extern char    AdapterList[Max_Num_Adapter][1024];
extern int     NumOfNIC;
extern unsigned char  AdapterMACAddr[6];
extern MSD_RMU_MODE gRmuMode;
extern MSD_U32 gEtherTypeValue;
extern MSD_U8 sohoDevNum;

/*Function Declaration*/
MSD_STATUS qdInit(int baseAddr, int bus_interface);
void qdClose();
void clean_stdin(void);
void apiCLI();
int cliCommand();

void sw_ctrl(MSD_BOOL down)
{
	MSD_LPORT port;

	/* shut down all ports */
	for(port = 0; port < 7; port++) {
		if(msdPortStateSet(sohoDevNum, port, down ? MSD_PORT_DISABLE : MSD_PORT_FORWARDING)) {
			printf("%s port %d fail!\n", down?"disable":"enable", port);
		}

		if(port > 4) { /* skip port 5 and 6 */
			continue;
		}

		if(msdPhyPortPowerDown(sohoDevNum, port, down)) {
			printf("%s port %d fail!\n", down?"shut down":"power up", port);
		}
	}
}

/*
 * MCLI down/up/help
 * down - shut down all ports
 * up - turn on all ports
 */
 
int main(int argc, char *argv[])
{
	int command = 0;
    MSD_U32 baseAddr = 0;
    int bus_interface = -1;
    int rmu_flag = 0;
    int fd;
	int ret = 0;

	/* SMI Interface */
	ret = smiOpen();
	if(ret)
	{
		printf("Error: Cannot open smi device\n");
		return 1;
	}    

    if (bus_interface < 0)
        bus_interface = 0;

#if 0
    do
    {
        printf("Interface List:\n");
        printf("\t0 : SMI Interface\n");
        printf("\t1 : SMI Multi-Chip\n");
        printf("\t2 : RMU Interface\n");
        printf("Please select one interface: ");
        scanf(" %c", (char*)(&command));
        clean_stdin();
        switch (command)
        {
            case '1':
                printf("please enter the base address(hex)\n");
                scanf("%x", &baseAddr);
                sohoDevNum = baseAddr;
                clean_stdin();
                bus_interface = 1;
                /*Still need the USB init, not break here*/
            case '0':
            {
                int ret = 0;

		ret = smiOpen();
		if(ret)
		{
			printf("Error: Cannot open smi device\n");
			return 1;
		}    

                if (bus_interface < 0)
                    bus_interface = 0;

                break;
            }
            case '2':
            {
                int nicId = 0;
                int i = 0;

                pcap_rmuGetDeviceList();
                do {
                    printf("\nPlease select NIC Index: ");
                    scanf("%d", &nicId);
                    clean_stdin();
                    strcpy(pcap_device_name, AdapterList[nicId - 1]);
#ifdef WIN32
                    IP_ADAPTER_INFO AdapterInfo[16];
                    DWORD dwBuflen = sizeof(AdapterInfo);

                    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBuflen);
                    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

                    do{
                        if (strstr(pcap_device_name, pAdapterInfo->AdapterName) != 0)
                        {
                            memcpy(AdapterMACAddr, pAdapterInfo->Address, 6);
                            break;
                        }
                        pAdapterInfo = pAdapterInfo->Next;
                    } while (pAdapterInfo);
#else
                    struct   ifreq   ifreq; 
                    int   sock; 
                    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0) 
		    { 
			    perror( "socket "); 
			    return   2; 
		    } 
		    strcpy(ifreq.ifr_name, pcap_device_name); 
		    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0) 
		    { 
			    perror( "ioctl "); 
			    return   3; 
		    } 
		    memcpy(AdapterMACAddr, ifreq.ifr_hwaddr.sa_data, 6);
#endif

                } while (!((nicId > 0) && (nicId <= NumOfNIC)));

                if (pcap_rmuOpenEthDevice() != 0)
                {
                    printf("NIC Open Failed\n");
                    printf("Press any key to exit program...");
                    command = getchar();
                    return -1;
                }
                bus_interface = 2;
                rmu_flag = 1;
                break;
            }
            default:
                break;
        }       
    } while (bus_interface < 0);    

#endif

	if(qdInit(baseAddr, bus_interface) != 0)
	{
		printf("QD Load Failed\n");
		printf("Do you want to exit program(y/n)?");
		command=getchar();
		clean_stdin();
		if ((command=='y')||(command=='Y'))
		{
			return 1;
		}
	}
	else 
	{
		printf("QD Load Success\n");
	}

	if(argc == 2) { 
		if(!strcmp(argv[1], "down")) {
			printf("shut down ports...\n");
			sw_ctrl(MSD_TRUE);
		}
		else if(!strcmp(argv[1], "up")) {
			printf("power up ports...\n");
			sw_ctrl(MSD_FALSE);
		}
		else {
			printf("MCLI usage:\n");
			printf("down - shut down all ports!\n");
			printf("up - turn on all ports!\n");
			printf("other - MCLI usage\n");
			printf("no arguments - enter CLI\n");
		}
	}
	else {
		
		apiCLI();
	//	cliCommand();

	    if (rmu_flag > 0)
		    pcap_rmuCloseEthDevice();
	}

	qdClose();
	return 0;
}
