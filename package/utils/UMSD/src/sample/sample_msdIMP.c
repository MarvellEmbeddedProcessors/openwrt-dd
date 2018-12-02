#include <msdCopyright.h>
/********************************************************************************
* sample_msdIMP.c
*
* DESCRIPTION:
*	This sample will demonstrate how to
*	1. Load EEPROM binary to EEPROM and do reset, fast boot function will be
*	performed
*		after run sample_msdIMPEEPROM, polling IMP IO will print string as following:
*		global 1 register 1: 0
*		global 1 register 1: 11
*		global 1 register 1: 3
*
*	2. load binary to memory and run from specified address
*		after run sample_msdIMPMemory, polling IMP IO will get string as following:
*		1234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'abcdefghijklmnopqrstuvwx
*		yz{|}~?????????????????????????????
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include "msdSample.h"
#include "msdApi.h"
#include <stdlib.h>
#include <stdio.h>
typedef enum {
	S200k = 0,
	S400k = 1,
	S500k = 2,
	S600k = 3,
	S800k = 4,
	S1000k = 5,
	S1200k = 6,
	S1500k = 7
} I2C_SPEED;

static MSD_U16 ReadInt(MSD_U8 *data, MSD_U8 cnt)
{
	MSD_U8 Idx = 0;
	MSD_U16 val = 0;
	cnt = cnt * 2;
	while (cnt > 0)
	{
		if (data[Idx] >= '0' && data[Idx] <= '9')
		{
			val = (val << 4) | (data[Idx] - '0');
			cnt--;
			Idx++;
		}
		else if (data[Idx] >= 'a' && data[Idx] <= 'f')
		{
			val = (val << 4) | (data[Idx] - 'a' + 10);
			cnt--;
			Idx++;
		}
		else if (data[Idx] >= 'A' && data[Idx] <= 'F' + 10)
		{
			val = (val << 4) | (data[Idx] - 'A' + 10);
			cnt--;
			Idx++;
		}
		else
		{
			val = (val << 4) | 0xF;
			cnt--;
			Idx++;
		}

	}
	return val;
}

typedef struct NODE
{
	MSD_U16 addr;
	MSD_U16 len;
	MSD_U8* data;
	struct NODE* next;
}Node;

static void freeNode(struct NODE *head)
{
	while (head != NULL)
	{
		Node* tmp = head->next;
		free(head->data);
		free(head);
		head = tmp;
	}
}

static MSD_U8* Ihx_Parse_EEPROM(MSD_U16 *data_len, MSD_U8* filename, I2C_SPEED speed, 
	MSD_U16 eeprom_addr_src, MSD_U16 ram_dest_addr, MSD_U16 ram_exec_addr)
{
	MSD_U8* ihx_data;
	MSD_U16 ihx_data_len;
	MSD_U16 Idx;
	MSD_U8 Dnum;
	MSD_U16 type;
	MSD_U8 chk;
	MSD_U16 code_start = 0xffff;
	MSD_U16 code_end = 0;
	MSD_U16 addr;
	MSD_U16 code_size;
	MSD_U8 data;
	MSD_U16 i;
	MSD_U8* eeprom_data;

	FILE *pfile = fopen(filename, "rb");

	fseek(pfile, 0, SEEK_END);
	ihx_data_len = (MSD_U16)ftell(pfile);

	ihx_data = (MSD_U8*)malloc(ihx_data_len * sizeof(MSD_U8) + 1);
	rewind(pfile);
	fread(ihx_data, sizeof(MSD_U8), ihx_data_len, pfile);

	/* get code size */
	for (Idx = 0; Idx < ihx_data_len;)
	{
		if (ihx_data[Idx] == ':')
		{
			Idx++;
			Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
			Idx += 2;
			addr = ReadInt(ihx_data + Idx, 2);
			Idx += 4;

			type = ReadInt(ihx_data + Idx, 1);
			if (type == 0)
			{
				if (code_start > addr)
					code_start = addr;
				if (code_end < (addr + Dnum))
					code_end = addr + Dnum;
			}
			else if (type == 1)
				break;
		}
		else
		{
			Idx += 1;
		}
	}

	code_size = code_end - code_start;

	eeprom_data = (unsigned char*)malloc((code_size + eeprom_addr_src)*sizeof(char) + 1);

	for (Idx = 0; Idx < code_size + eeprom_addr_src; Idx++)
	{
		eeprom_data[Idx] = 0xff;
	}

	/* fast boot function */
	eeprom_data[0] = 0xed;

	/* eeprom speed configuration */
	data = (MSD_U8)speed;
	eeprom_data[1] = data;

	/* code size */
	eeprom_data[2] = (MSD_U8)(code_size & 0xff);
	eeprom_data[3] = (MSD_U8)((code_size >> 8) & 0xff);

	/* ram address */
	eeprom_data[4] = (MSD_U8)(ram_dest_addr & 0xff);
	eeprom_data[5] = (MSD_U8)((ram_dest_addr >> 8) & 0xff);

	/* eeprom src address */
	eeprom_data[6] = (MSD_U8)(eeprom_addr_src & 0xff);
	eeprom_data[7] = (MSD_U8)((eeprom_addr_src >> 8) & 0xff);

	eeprom_data[8] = 0xc3;

	/* ram executing address */
	eeprom_data[9] = (MSD_U8)(ram_exec_addr & 0xff);
	eeprom_data[0xa] = (MSD_U8)((ram_exec_addr >> 8) & 0xff);

	type = 0;
	Idx = 0;
	while (type != 1)
	{
		while ((ihx_data[Idx] != ':') && ihx_data[Idx] > 0)
			Idx++;
		if (ihx_data[Idx] != ':')
		{
			/* ihx file format error */
			free(eeprom_data);
			eeprom_data = NULL;
			return eeprom_data;
		}
		Idx++;

		Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
		chk = Dnum;
		Idx += 2;

		addr = ReadInt(ihx_data + Idx, 2);
		chk += (addr & 0xff);
		chk += ((addr >> 8) & 0xff);
		Idx += 4;

		type = ReadInt(ihx_data + Idx, 1);
		chk += type;
		Idx += 2;

		for (i = 0; i < Dnum; i++)
		{
			chk += ReadInt(ihx_data + Idx, 1);
			if (type == 0)
				eeprom_data[addr + eeprom_addr_src + i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);

			Idx += 2;
		}

		if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
		{
			free(eeprom_data);
			eeprom_data = NULL;
			return eeprom_data;
		}
		Idx += 2;

	}
	free(ihx_data);

	*data_len = code_size + eeprom_addr_src;
	return eeprom_data;
}

static Node* Ihx_Parse_Memory(MSD_U8* filename)
{
	MSD_U8* ihx_data;
	MSD_U16 ihx_data_len;
	MSD_U16 Idx;
	MSD_U8 Dnum;
	MSD_U16 type;
	MSD_U8 chk;
	MSD_U16 addr;
	MSD_U16 i;

	Node *head = NULL, *cur = NULL, *ne = NULL;

	FILE *pfile = fopen(filename, "rb");

	fseek(pfile, 0, SEEK_END);
	ihx_data_len = (MSD_U16)ftell(pfile);

	ihx_data = (unsigned char*)malloc(ihx_data_len*sizeof(char) + 1);
	rewind(pfile);
	fread(ihx_data, sizeof(unsigned char), ihx_data_len, pfile);

	type = 0;
	Idx = 0;
	while (type != 1)
	{
		while ((ihx_data[Idx] != ':') && ihx_data[Idx] > 0)
			Idx++;
		if (ihx_data[Idx] != ':')
		{
			/* ihx file format error */
			freeNode(head);
			head = NULL;
			free(ihx_data);
			return NULL;
		}
		Idx++;

		Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
		chk = Dnum;
		Idx += 2;

		addr = ReadInt(ihx_data + Idx, 2);
		chk += (addr & 0xff);
		chk += ((addr >> 8) & 0xff);
		Idx += 4;

		type = ReadInt(ihx_data + Idx, 1);
		chk += type;
		Idx += 2;

		if (type == 0)
		{
			ne = (Node*)malloc(sizeof(Node));
			ne->len = Dnum;
			ne->addr = addr;
			ne->data = (MSD_U8*)malloc(Dnum * sizeof(MSD_U8));
			ne->next = NULL;

			if (head == NULL)
				head = ne;

			if (cur == NULL)
				cur = ne;
			else
			{
				cur->next = ne;
				cur = ne;
			}
		}

		for (i = 0; i < Dnum; i++)
		{
			chk += ReadInt(ihx_data + Idx, 1);
			if (type == 0)
				cur->data[i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);

			Idx += 2;
		}

		Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
		if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
		{
			freeNode(head);
			head = NULL;
			free(ihx_data);
			return NULL;
		}

		Idx += 2;
	}
	free(ihx_data);

	return head;
}

MSD_STATUS sample_msdIMPEEPROM(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U16 data_len = 0;

	MSD_U8 *data = NULL;
	I2C_SPEED speed = S200k;

	MSD_U8* filename = "RegReadWrite.ihx";

	MSD_U16 ram_dest_addr = 0;
	MSD_U16 ram_exec_addr = 0;
	MSD_U16 eeprom_addr_src = 0x10;

	data = Ihx_Parse_EEPROM(&data_len, filename, speed, eeprom_addr_src, ram_dest_addr, ram_exec_addr);

	if (data == NULL)
		return MSD_FAIL;

	/* load data to eeprom */
	status = msdImpLoadToEEPROM(devNum, data_len, data);
	if (status != MSD_OK)
		return MSD_FAIL;

	/* do imp reset, will auto load from eerpom and boot */
	status = msdImpReset(devNum);
	if (status != MSD_OK)
		return MSD_FAIL;

	free(data);

    MSG_PRINT(("sample_msdIMPEEPROM code success.\n"));
	return status;
}

MSD_STATUS sample_msdIMPMemory(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U16 addr = 0xffff;

	MSD_U8* filename = "ConsoleIOAsm.ihx";

	Node* head = Ihx_Parse_Memory(filename);
	Node* node = head;

	if (head == NULL)
		return MSD_FAIL;

	/* load data to memory */
	while (head != NULL)
	{
		status = msdImpLoadToRAM(devNum, head->addr, head->len, head->data);
		if (status != MSD_OK)
			return MSD_FAIL;
		if (addr > head->addr)
			addr = head->addr;
		head = head->next;
	}

	freeNode(node);
	node = NULL;

	status = msdImpRun(devNum, addr);
	if (status != MSD_OK)
		return MSD_FAIL;

    MSG_PRINT(("sample_msdIMPMemory code success.\n"));
	return status;
}
