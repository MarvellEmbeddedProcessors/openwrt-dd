#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msdApi.h"
#include "rmuPcap.h"

static pcap_t *fp_pcap;

char    pcap_device_name[512] = { 0 };
char    AdapterList[Max_Num_Adapter][1024];
int		NumOfNIC = 0;
unsigned char	AdapterMACAddr[6] = { 0 };
extern MSD_RMU_MODE gRmuMode;
extern MSD_U32 gEtherTypeValue;
extern MSD_U8 sohoDevNum;

void pcap_rmuGetDeviceList()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list from the local machine */
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
		exit(1);
	}
	printf("\n================ NIC CARD List ================");
	for (d = alldevs; d != NULL; d = d->next)
	{
		//strcpy(pcap_device_name, d->name);
		//printf("device: %s\n", d->description);
		if (d->name) {
			//if (strstr(d->name, "eth0")) {
			//	memset(pcap_device_name, 0, sizeof(pcap_device_name));
			//	strcpy(pcap_device_name, d->name);
			//	printf("\nDevName: %s", pcap_device_name);
			//	printf("\nDevDesc: %s", d->description);
			//	printf("\n\n");
			//	break;
			//}
			strcpy(AdapterList[i], d->name);
			printf("\n(%d) %s", (i + 1), d->description);
			printf("\n     %s", d->name);
			i++;
		}
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
	}

	NumOfNIC = i;
	/* We don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);
}

int pcap_rmuOpenEthDevice()
{
	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	//char packet_filter[] = "ether proto 0x9100";
    char packet_filter[512] = { 0 };
	struct bpf_program fcode;
	bpf_u_int32 NetMask;
	//dev = pcap_findalldevs_ex(errbuf);
	//dev = pcap_findalldevs(errbuf);
	if ((dev = pcap_lookupdev(errbuf)) == NULL)
	{
		printf("No Device Found: %s\n", errbuf);
		return 1;
	}
	printf("\nUsing Interface: %s\n\n", pcap_device_name /*dev*/);

    if (gRmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
    {
        sprintf(packet_filter, "len <= 512 and ether[12] == 0x%02x and ether[13] == 0x%02x and ether[14] == 0x00 and ether[15] == 0x00 and ether[16] == 0x%02x and ether[17] == 0x00", 
            (gEtherTypeValue >> 8) & 0xff, gEtherTypeValue & 0xff, sohoDevNum & 0x1F);
    }
    else if (gRmuMode == MSD_RMU_DSA_MODE)
    {
        sprintf(packet_filter, "len <= 512 and ether[12] == 0x%02x and ether[13] == 0x00",
            sohoDevNum & 0x1F);
    }

	/* Open the adapter */
	if ((fp_pcap = pcap_open_live(pcap_device_name,//pcap_device_name /*dev*/,  // name of the device
		65536,	           // portion of the packet to capture. It doesn't matter in this case
		1,	               // promiscuous mode (nonzero means promiscuous)
		1000,	           // read timeout
		errbuf	           // error buffer
		)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", dev);
		return 1;
	}

	// Let's do things simpler: we suppose to be in a C class network ;-)
	NetMask = 0xffffff;

	//compile the filter
	if (pcap_compile(fp_pcap, &fcode, packet_filter, 1, NetMask) < 0)
	{
		fprintf(stderr, "\nError compiling filter: wrong syntax.\n");

		pcap_close(fp_pcap);
		return 1;
		/* return FALSE; */
	}

	//set the filter
	if (pcap_setfilter(fp_pcap, &fcode)<0)
	{
		fprintf(stderr, "\nError setting the filter\n");

		pcap_close(fp_pcap);
		return 1;
		/* return FALSE; */
	}

	return 0;
}

int pcap_rmuSendPacket(unsigned char *packet)
{
	if (pcap_sendpacket(fp_pcap,	// Adapter
		packet,				// buffer with the packet
		150					// size
		) != 0)
	{
		fprintf(stderr, "\nError sending the packet: %s\n", pcap_geterr(fp_pcap));
		return 1;
	}
	return 0;
}

int pcap_rmuReceivePacket(unsigned char **packet, unsigned char id)
{
	struct pcap_pkthdr *mypkt_hdr;
	unsigned int exit = 0;
	unsigned int drop_cnt = 0;

	while (exit == 0)
	{
		pcap_next_ex(fp_pcap, &mypkt_hdr, (const u_char **)packet);

		if (mypkt_hdr->len != 0)
		{
			if (*((*packet) + 19) == id)
			{
				exit = 1;
			}
		}

		drop_cnt++;
		if (drop_cnt > 100)
		{
			printf("Packet drop count exceeds limit: %d\n", drop_cnt);
			exit = 1;
		}
	}
	return mypkt_hdr->len;
}

int pcap_rmuCloseEthDevice()
{
	if (fp_pcap != NULL)
	{
		pcap_close(fp_pcap);
	}
	return 0;
}

int send_and_receive_packet(
	unsigned char *req_packet,
	unsigned int req_pktlen,
	unsigned char **rsp_packet,
	unsigned int *rsp_pktlen)
{
	struct pcap_pkthdr *mypkt_hdr;
	unsigned int retVal;
	unsigned int size = 512;
	unsigned int exit = 0;
	unsigned int drop_cnt = 0;
	unsigned int drop_cnt_limit = 100;
    unsigned int seqNumOffset = 19;

	/*Override packet SA = CPU MAC: 0x28, 0xD2, 0x44, 0x8C, 0xF9, 0xF3*/

	*(req_packet + 6) = AdapterMACAddr[0];
	*(req_packet + 7) = AdapterMACAddr[1];
	*(req_packet + 8) = AdapterMACAddr[2];
	*(req_packet + 9) = AdapterMACAddr[3];
	*(req_packet + 10) = AdapterMACAddr[4];
	*(req_packet + 11) = AdapterMACAddr[5];

    if (gRmuMode == MSD_RMU_ETHERT_TYPE_DSA_MODE)
        seqNumOffset = 19;
    else if (gRmuMode == MSD_RMU_DSA_MODE)
        seqNumOffset = 15;

	retVal = pcap_sendpacket(fp_pcap, req_packet, size);
	if (retVal)
	{
		fprintf(stderr, "\nError sending packet: %s\n", pcap_geterr(fp_pcap));
		return retVal;
	}

	while (exit == 0)
	{
		retVal = pcap_next_ex(fp_pcap, &mypkt_hdr, (const u_char **)rsp_packet);
		if(retVal != 1)
			continue;

		if (mypkt_hdr->len != 0)
		{
			// dump_packet(&rsp_packet, mypkt_hdr->len, "Received Packet:");
			if (*(req_packet + seqNumOffset) != *(*(rsp_packet)+seqNumOffset)) // Check sequence number of dsatag
			{
				 printf("Error: DSA tag sequence numbers does not match - req: %X, rsp: %X\n",
                     *(req_packet + seqNumOffset), *((*rsp_packet) + seqNumOffset));

				//exit = 1;
			}
			else
			{
				if (*(req_packet + 6) != *(*(rsp_packet)))
				{

					;
				}
				else
					exit = 1;
			}
		}

		drop_cnt++;
		if (drop_cnt > drop_cnt_limit)
		{
			printf("Packet drop count exceeds limit: %d\n", drop_cnt_limit);
			//exit = 1;
			return -1;
		}
	}
	*rsp_pktlen = mypkt_hdr->len;

	if (*rsp_pktlen < req_pktlen)
	{
		printf("Error: rsp_pktlen - %d < req_pktlen - %d\n", *rsp_pktlen, req_pktlen);
		return -1;
	}

	return 0;
}
