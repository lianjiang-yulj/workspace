#ifndef _RING_BUF_H
#define _RING_BUF_H

#include <pthread.h>
#include "common.h"

#define MAX_RINGBUF_ITEM_COUNT 4096

typedef struct SBufItem {
	int  linenum;
	bool valid;
	int msglen;
	char msg[BUF_LINE];
} SBufItem;

class CRingBuffer {
public:
	CRingBuffer(int size);
	virtual ~CRingBuffer();

	int readNext(int* pos, SBufItem** item);
	int next();
	int write(int linenum, char* msg, int len);
	void finish();
private:
	int _size;
	int _readPos;
	SBufItem** _items;
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
	bool _isStop;
};

#endif
