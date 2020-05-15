#ifndef _RING_BUF_H
#define _RING_BUF_H

#include <pthread.h>
#include <vector>

#include "common.h"

class CLineBuffer {
public:
	CLineBuffer(int size);
	virtual ~CLineBuffer();

	int enqueue(int len, char* msg);
	int dequeue(std::vector<char*>& msgVec);
	void stop() { isEnd_ = true; }
	bool isStop() { return isEnd_; }
private:
	int dequeuePos_;
	int size_;
	char** lines_;
	volatile bool isEnd_;
};

#endif
