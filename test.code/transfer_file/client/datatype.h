#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <stdint.h>

const int MAX_ITEMS = 4096;

struct SRequestItem {
	uint16_t channelId;
};

struct SRequestPacket {
	uint8_t packetCount;
	SRequestItem* requestItems[MAX_ITEMS];
};

struct SResponseItem {
	bool valid;
	uint8_t channelId;
	uint8_t msglen;
	int linenum;
	char* msg;
};

struct SResponsePacket {
	uint8_t packetCount;
	SResponseItem** responseItems;
};

#endif

