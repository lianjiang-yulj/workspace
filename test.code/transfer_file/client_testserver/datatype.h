#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <linux/types.h>

const int MAX_ITEMS = 255;

struct SRequestItem {
	uint8_t channelId;
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

