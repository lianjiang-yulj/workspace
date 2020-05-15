#ifndef _RING_BUF_H
#define _RING_BUF_H

#include <pthread.h>
#include <vector>

#include "common.h"

struct LineItem {
    char* msg;
    int32_t len;
};

class CLineBuffer {
public:
	CLineBuffer(int size);
	virtual ~CLineBuffer();

	int enqueue(int line, char* msg, int len);
	int dequeue(LineItem*& pItem, int& len);
	void stop() { isEnd_ = true; }
	bool isStop() { return isEnd_; }
private:
	int dequeuePos_;
	int size_;
	LineItem* lines_;
	volatile bool isEnd_;
};

#endif
