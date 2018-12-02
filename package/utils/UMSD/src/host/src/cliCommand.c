#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include "msdApi.h"
#include "libcli.h"
#include "apiCLI.h"

#define CLITEST_PORT                8888
#define MAX_LINE_LENGTH             1000

static struct cli_filter_cmds filter_cmds_list[] =
{
    /*ATU entry*/
    { "-macAddr", "atu" },
    { "-lag", "atu" },
    { "-portVec", "atu" },
    { "-fid", "atu" },
    { "-state", "atu" },
    { "-FPri", "atu" },
    { "-QPri", "atu" },

    /*VTU entry*/
    { "-fid", "vlan" },
    { "-vid", "vlan" },
    { "-sid", "vlan" },
    { "-TagP", "vlan" },
    { "-vidPolicy", "vlan" },
    { "-useVIDFPri", "vlan" },
    { "-vidFPri", "vlan" },
    { "-useVIDQPri", "vlan" },
    { "-vidQPri", "vlan" },
    { "-dontLearn", "vlan" },
    { "-filterUC", "vlan" },
    { "-filterBC", "vlan" },
    { "-filterMC", "vlan" },

    /*STU entry*/
    { "-sid", "stu" },
    { "-portState", "stu" },

    /*IRL*/
    { "-bktTypeMask", "irl writeRes" },
    { "-tcamFlows", "irl writeRes" },
    { "-priAndPt", "irl writeRes" },
    { "-useFPri", "irl writeRes" },
    { "-priSelect", "irl writeRes" },
    { "-colorAware", "irl writeRes" },
    { "-accountGrnOverflow", "irl writeRes" },
    { "-accountQConf", "irl writeRes" },
    { "-accountFiltered", "irl writeRes" },
    { "-samplingMode", "irl writeRes" },
    { "-actionMode", "irl writeRes" },
    { "-fcMode", "irl writeRes" },
    { "-fcPriority", "irl writeRes" },
    { "-isValid", "irl writeRes" },
    { "-ebsLimit", "irl writeRes" },
    { "-cbsLimit", "irl writeRes" },
    { "-bktIncrement", "irl writeRes" },
    { "-bktRateFactorGrn", "irl writeRes" },
    { "-bktRateFactorYel", "irl writeRes" },
    { "-countMode", "irl writeRes" },
	{ "-IRLConstant", "irl writeRes" },

    { "-bktTypeMask", "irl advConfig" },
    { "-tcamFlows", "irl advConfig" },
    { "-priAndPt", "irl advConfig" },
    { "-useFPri", "irl advConfig" },
    { "-priSelect", "irl advConfig" },

    /*VCT*/
    { "-mode", "vct" },
    { "-transChanSel", "vct" },
    { "-sampleAvg", "vct" },
    { "-peakDetHyst", "vct" },
    { "-samplePointDist", "vct" },

    /*PTP*/
    { "-ptpArrIntEn", "ptp setIntEn" },
    { "-ptpDepIntEn", "ptp setIntEn" },

    { "-transSpec", "ptp setPortConfig" },
    { "-disTSpec", "ptp setPortConfig" },
    { "-etJump", "ptp setPortConfig" },
    { "-ipJump", "ptp setPortConfig" },
    { "-ptpArrIntEn", "ptp setPortConfig" },
    { "-ptpDepIntEn", "ptp setPortConfig" },
    { "-disTSOverwrite", "ptp setPortConfig" },
    { "-filterAct", "ptp setPortConfig" },
    { "-hwAccPtp", "ptp setPortConfig" },
    { "-kpFramwSa", "ptp setPortConfig" },
    { "-ExthwAcc", "ptp setPortConfig" },
    { "-arrTSMode", "ptp setPortConfig" },
    { "-arrLedCtrl", "ptp setPortConfig" },
    { "-depLedCtrl", "ptp setPortConfig" },

    { "-ptpEType", "ptp setGlobalConfig" },
    { "-msgIdTSEn", "ptp setGlobalConfig" },
    { "-tsArrPtr", "ptp setGlobalConfig" },

    { "-altScheme", "ptp setPtpMode"},
    { "-grandMstr", "ptp setPtpMode" },
    { "-oneStep", "ptp setPtpMode" },
    { "-PTPMode", "ptp setPtpMode" },

    { "-todLoadPoint", "ptp"},
    { "-clkValid", "ptp" },
    { "-domainNumber", "ptp" },
    { "-todNanoseconds", "ptp" },
    { "-todSecondsLow", "ptp" },
    { "-todSecondsHigh", "ptp" },
    { "-Nanoseconds1722Low", "ptp" },
    { "-Nanoseconds1722High", "ptp" },
    { "-todCompensation", "ptp" },

    { "-ptpPulseWidth", "ptp setPtpPulse" },
    { "-ptp1ppsWidthRange", "ptp setPtpPulse" },
    { "-ptp1ppsPhase", "ptp setPtpPulse" },
    { "-ptp1ppsSelect", "ptp setPtpPulse" },

    /*qos*/
    { "-ieeeYellow", "qos" },
    { "-disIeeeQpri", "qos" },
    { "-ieeeQpri", "qos" },
    { "-disIeeeFpri", "qos" },
    { "-ieeeFpri", "qos" },
    { "-disVIDMarkingGreen", "qos" },
    { "-disPCPMarkingGreen", "qos" },
    { "-framePCPGreen", "qos" },
    { "-disVIDMarkingYel", "qos" },
    { "-disPCPMarkingYel", "qos" },
    { "-framePCPYel", "qos" },
    { "-disVIDMarkingAVB", "qos" },
    { "-disPCPMarkingAVB", "qos" },
    { "-framePCPAVB", "qos" },
    { "-enDSCPMarkingGreen", "qos" },
    { "-frameDSCPGreen", "qos" },
    { "-enDSCPMarkingYel", "qos" },
    { "-frameDSCPYel", "qos" },
    { "-enDSCPMarkingAVB", "qos" },
    { "-frameDSCPAVB", "qos" },

    /*TCAM*/
    { "-frameType", "tcam addEntry" },
    { "-frameTypeMask", "tcam addEntry" },
    { "-spv", "tcam addEntry" },
    { "-spvMask", "tcam addEntry" },
    { "-ppri", "tcam addEntry" },
    { "-ppriMask", "tcam addEntry" },
    { "-pvid", "tcam addEntry" },
    { "-pvidMask", "tcam addEntry" },
    { "-frameOctet", "tcam addEntry" },
    { "-frameOctetMask", "tcam addEntry" },
    { "-continu", "tcam addEntry" },
    { "-interrupt", "tcam addEntry" },
    { "-IncTcamCtr", "tcam addEntry" },
    { "-vidOverride", "tcam addEntry" },
    { "-vidData", "tcam addEntry" },
    { "-nextId", "tcam addEntry" },
    { "-qpriOverride", "tcam addEntry" },
    { "-qpriData", "tcam addEntry" },
    { "-fpriOverride", "tcam addEntry" },
    { "-fpriData", "tcam addEntry" },
    { "-dpvSF", "tcam addEntry" },
    { "-dpvData", "tcam addEntry" },
    { "-dpvMode", "tcam addEntry" },
    { "-colorMode", "tcam addEntry" },
    { "-vtuPageOverride", "tcam addEntry" },
    { "-vtuPage", "tcam addEntry" },
    { "-unKnownFilter", "tcam addEntry" },
    { "-egActPoint", "tcam addEntry" },
    { "-ldBalanceOverride", "tcam addEntry" },
    { "-ldBalanceData", "tcam addEntry" },
    { "-DSCPOverride", "tcam addEntry" },
    { "-DSCP", "tcam addEntry" },
    { "-factionOverride", "tcam addEntry" },
    { "-factionData", "tcam addEntry" },

    { "-frameModeOverride", "tcam egrAddEntry" },
    { "-frameMode", "tcam egrAddEntry" },
    { "-tagModeOverride", "tcam egrAddEntry" },
    { "-tagMode", "tcam egrAddEntry" },
    { "-daMode", "tcam egrAddEntry" },
    { "-saMode", "tcam egrAddEntry" },
    { "-egVidModeOverride ", "tcam egrAddEntry" },
    { "-egVidMode", "tcam egrAddEntry" },
    { "-egVidData", "tcam egrAddEntry" },
    { "-egDSCPMode", "tcam egrAddEntry" },
    { "-egDSCP", "tcam egrAddEntry" },
    { "-egfpriModeOverride", "tcam egrAddEntry" },
    { "-egfpriMode", "tcam egrAddEntry" },
    { "-egEC", "tcam egrAddEntry" },
    { "-egFPRI", "tcam egrAddEntry" },

    { "-frameType", "tcam advanceConfig" },
    { "-frameTypeMask", "tcam advanceConfig" },
    { "-spv", "tcam advanceConfig" },
    { "-spvMask", "tcam advanceConfig" },
    { "-ppri", "tcam advanceConfig" },
    { "-ppriMask", "tcam advanceConfig" },
    { "-pvid", "tcam advanceConfig" },
    { "-pvidMask", "tcam advanceConfig" },
    { "-frameOctet", "tcam advanceConfig" },
    { "-frameOctetMask", "tcam advanceConfig" },
    { "-interrupt", "tcam advanceConfig" },
    { "-IncTcamCtr", "tcam advanceConfig" },
    { "-vidOverride", "tcam advanceConfig" },
    { "-vidData", "tcam advanceConfig" },
    { "-qpriOverride", "tcam advanceConfig" },
    { "-qpriData", "tcam advanceConfig" },
    { "-fpriOverride", "tcam advanceConfig" },
    { "-fpriData", "tcam advanceConfig" },
    { "-dpvSF", "tcam advanceConfig" },
    { "-dpvData", "tcam advanceConfig" },
    { "-dpvMode", "tcam advanceConfig" },
    { "-colorMode", "tcam advanceConfig" },
    { "-vtuPageOverride", "tcam advanceConfig" },
    { "-vtuPage", "tcam advanceConfig" },
    { "-unKnownFilter", "tcam advanceConfig" },
    { "-egActPoint", "tcam advanceConfig" },
    { "-ldBalanceOverride", "tcam advanceConfig" },
    { "-ldBalanceData", "tcam advanceConfig" },
    { "-DSCPOverride", "tcam advanceConfig" },
    { "-DSCP", "tcam advanceConfig" },
    { "-factionOverride", "tcam advanceConfig" },
    { "-factionData", "tcam advanceConfig" },

    { "-da", "tcam advanceConfig" },
    { "-sa", "tcam advanceConfig" },
    { "-vlanTag", "tcam advanceConfig" },
    { "-etherType", "tcam advanceConfig" },
    { "-daMask", "tcam advanceConfig" },
    { "-saMask", "tcam advanceConfig" },
    { "-vlanTagMask", "tcam advanceConfig" },
    { "-etherTypeMask", "tcam advanceConfig" },
    { "-version", "tcam advanceConfig" },
    { "-ihl", "tcam advanceConfig" },
    { "-tos", "tcam advanceConfig" },
    { "-ipLength", "tcam advanceConfig" },
    { "-id", "tcam advanceConfig" },
    { "-flag", "tcam advanceConfig" },
    { "-fragOffset", "tcam advanceConfig" },
    { "-ttl", "tcam advanceConfig" },
    { "-protocol", "tcam advanceConfig" },
    { "-hdrChkSum", "tcam advanceConfig" },
    { "-sV4Ip", "tcam advanceConfig" },
    { "-dV4Ip", "tcam advanceConfig" },
    { "-versionMask", "tcam advanceConfig" },
    { "-ihlMask", "tcam advanceConfig" },
    { "-tosMask", "tcam advanceConfig" },
    { "-ipLengthMask", "tcam advanceConfig" },
    { "-idMask", "tcam advanceConfig" },
    { "-flagMask", "tcam advanceConfig" },
    { "-fragOffsetMask", "tcam advanceConfig" },
    { "-ttlMask", "tcam advanceConfig" },
    { "-protocolMask", "tcam advanceConfig" },
    { "-hdrChkSumMask", "tcam advanceConfig" },
    { "-sV4IpMask", "tcam advanceConfig" },
    { "-dV4IpMask", "tcam advanceConfig" },
    { "-tc", "tcam advanceConfig" },
    { "-flowLbl", "tcam advanceConfig" },
    { "-payloadLen", "tcam advanceConfig" },
    { "-nextHdr", "tcam advanceConfig" },
    { "-hopLimit", "tcam advanceConfig" },
    { "-sV6Ip", "tcam advanceConfig" },
    { "-dV6Ip", "tcam advanceConfig" },
    { "-tcMask", "tcam advanceConfig" },
    { "-flowLblMask", "tcam advanceConfig" },
    { "-payloadLenMask", "tcam advanceConfig" },
    { "-nextHdrMask", "tcam advanceConfig" },
    { "-hopLimitMask", "tcam advanceConfig" },
    { "-sV6IpMask", "tcam advanceConfig" },
    { "-dV6IpMask", "tcam advanceConfig" },
    { "-srcPortMask", "tcam advanceConfig" },
    { "-destPortMask", "tcam advanceConfig" },
    { "-seqNumMask", "tcam advanceConfig" },
    { "-ackNumMask", "tcam advanceConfig" },
    { "-tcpOffsetMask", "tcam advanceConfig" },
    { "-tcpFlagsMask", "tcam advanceConfig" },
    { "-windowSizeMask", "tcam advanceConfig" },
    { "-chkSumMask", "tcam advanceConfig" },
    { "-urgPtrMask", "tcam advanceConfig" },
    { "-srcPort", "tcam advanceConfig" },
    { "-destPort", "tcam advanceConfig" },
    { "-seqNum", "tcam advanceConfig" },
    { "-ackNum", "tcam advanceConfig" },
    { "-tcpOffset", "tcam advanceConfig" },
    { "-tcpFlags", "tcam advanceConfig" },
    { "-windowSize", "tcam advanceConfig" },
    { "-chkSum", "tcam advanceConfig" },
    { "-urgPtr", "tcam advanceConfig" },
    { "-udpLength", "tcam advanceConfig" },
    { "-udpLengthMask", "tcam advanceConfig" },

    /*RMU*/
    { "-frameType", "rmu" },
    { "-frameTypeMask", "rmu" },
    { "-spv", "rmu" },
    { "-spvMask", "rmu" },
    { "-ppri", "rmu" },
    { "-ppriMask", "rmu" },
    { "-pvid", "rmu" },
    { "-pvidMask", "rmu" },
    { "-frameOctet", "rmu" },
    { "-frameOctetMask", "rmu" },
    { "-continu", "rmu" },
    { "-interrupt", "rmu" },
    { "-IncTcamCtr", "rmu" },
    { "-vidOverride", "rmu" },
    { "-vidData", "rmu" },
    { "-nextId", "rmu" },
    { "-qpriOverride", "rmu" },
    { "-qpriData", "rmu" },
    { "-fpriOverride", "rmu" },
    { "-fpriData", "rmu" },
    { "-dpvSF", "rmu" },
    { "-dpvData", "rmu" },
    { "-dpvMode", "rmu" },
    { "-colorMode", "rmu" },
    { "-vtuPageOverride", "rmu" },
    { "-vtuPage", "rmu" },
    { "-unKnownFilter", "rmu" },
    { "-egActPoint", "rmu" },
    { "-ldBalanceOverride", "rmu" },
    { "-ldBalanceData", "rmu" },
    { "-DSCPOverride", "rmu" },
    { "-DSCP", "rmu" },
    { "-factionOverride", "rmu" },
    { "-factionData", "rmu" },

    /*qc*/
    { "-len", "qc" },
    { "-queue", "qc" },

    /*port*/
    { "-enable", "port setCutThroughEnable"},
    { "-queue", "port setCutThroughEnable" },

    { NULL, NULL }
};

static struct cli_def *g_cli;
void printCallback(const char* str)
{
    cli_bufprint(g_cli, "%s", str);
}

static void strcatbuf(char *buf, char *command, char *argv[], int argc)
{
	int i;
	
	memset(buf, 0, MAX_LINE_LENGTH);
	strcat(buf, command);
	strcat(buf, " ");
	for(i = 0; i < argc; i++)
	{
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}
}
static int cmd_subcmd_help(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	struct cli_command *p;
	struct cli_command *c;

    for (p = cli->commands; p; p = p->next)
	{
		if (p->children && strstr(cli->commandname, p->command))
		{
            //cli_show_help(cli, p->children);
			for (c = p->children; c; c = c->next)
			{
				if (c->command && c->callback && cli->privilege >= c->privilege &&
					(c->mode == cli->mode || c->mode == MODE_ANY))
				{
					cli_bufprint(cli, "%s", (c->help != NULL ? c->help : ""));
				}
			}
		}
	}

	return CLI_OK;
}

/*Callback function*/
int cmd_operation(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	int index = 0;

    parseCmd_cli(command, argv, argc);
	
	while(externCLICMDList[index].cmd != NULL)
	{
		if(strncmp(command, externCLICMDList[index].cmd, strlen(externCLICMDList[index].cmd)) == 0)
		{
			externCLICMDList[index].callback();
		}
		index++;
	}
	
	return CLI_OK;
}

int cliCommand()
{
	struct cli_command *c, *c1;
	struct cli_def *cli;
	int s, x;
	struct sockaddr_in addr;
	int on = 1;
	int i = 0;
	int index;
	CLI_COMMAND_STRUCT *cliCMDList;
#ifdef WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    int nAddrlen;
#endif

	
	cli = cli_init();
	cli_set_banner(cli, "MCLI test environment");
	cli_set_hostname(cli, "MCLI");
	cli_telnet_protocol(cli, 1);
	
	/*register the commands*/
	while(externCLICMDList[i].cmd != NULL)
	{
		index = 0;
		cliCMDList = externCLICMDList[i].cmdList;
		if(cliCMDList != NULL)
		{
            c = cli_register_command(cli, NULL, externCLICMDList[i].cmd, cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, externCLICMDList[i].desc);
            c1 = cli_register_command(cli, c, "help", cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, externCLICMDList[i].help);
			while(strcmp(cliCMDList[index].cmd, "help") != 0)
			{
				cli_register_command(cli, c1, cliCMDList[index].cmd, 		cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, NULL);
				cli_register_command(cli, c, cliCMDList[index].cmd, 		cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cliCMDList[index].help);
				index++;
			}
		}
        else
        {
            cli_register_command(cli, NULL, externCLICMDList[i].cmd, cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, externCLICMDList[i].desc);
        }
		i++;
	}
    cli_set_filter(filter_cmds_list);

    apiCLI_alloc_buf();
    g_cli = cli;
    setPrintCallback(printCallback);
    msdSetStringPrintCallback(printCallback);

#ifdef WIN32
    /*init the WSA*/
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        printf("init the WSA error\n");
        return 0;
    }

    /*Create the socket*/
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s == INVALID_SOCKET)
    {
        printf("Create socket error\n");
        return -1;
    }

    /*bind the IP and port*/
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLITEST_PORT);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        printf("bind error\n");
        return -1;
    }

    /*Listen*/
    if(listen(s, 1) == SOCKET_ERROR)
    {
        printf("listen error \n");
        return -1;
    }
    printf("Listening on port %d for telnet\n", CLITEST_PORT);

    nAddrlen = sizeof(addr);
    while (1)
    {
        x = accept(s, (SOCKADDR *)&addr, &nAddrlen);
        if(x == INVALID_SOCKET)
        {
            printf("accept error \n");
            continue;
        }
        printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

        cli_loop(cli, x);
        shutdown(x, SD_BOTH);
        _pclose(x);
    }
#else
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return 1;
	}
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(CLITEST_PORT);
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return 1;
	}

	if (listen(s, 1) < 0)
	{
		perror("listen");
		return 1;
	}

	printf("Listening on port %d for telnet\n", CLITEST_PORT);

	while ((x = accept(s, NULL, 0)))
	{
		int pid = fork();
		if (pid < 0)
		{
			perror("fork");
			return 1;
		}

		/* parent */
		if (pid > 0)
		{
			socklen_t len = sizeof(addr);
			if (getpeername(x, (struct sockaddr *) &addr, &len) >= 0)
				printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

			close(x);
			continue;
		}

		/* child */
		close(s);
		cli_loop(cli, x);
		exit(0);
	}
#endif

	apiCLI_free_buf();
	cli_done(cli);
}