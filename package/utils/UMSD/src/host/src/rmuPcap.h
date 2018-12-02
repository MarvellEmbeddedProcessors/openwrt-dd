#ifndef __RMUPCAP_H__
#define __RMUPCAP_H__

#define Max_Num_Adapter 10

void pcap_rmuGetDeviceList();
int pcap_rmuOpenEthDevice();
int pcap_rmuSendPacket(unsigned char *packet);
int pcap_rmuReceivePacket(unsigned char **packet, unsigned char id);
int pcap_rmuCloseEthDevice();
int send_and_receive_packet(
		unsigned char *req_packet,
		unsigned int req_pktlen,
		unsigned char **rsp_packet,
		unsigned int *rsp_pktlen);

#endif /*__RMUPCAP_H__*/
