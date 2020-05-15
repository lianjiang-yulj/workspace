#include "linebuf.h"
#include <assert.h>
#include <unistd.h>

const int MAX_TRY = 100;

CLineBuffer::CLineBuffer(int size) : dequeuePos_(0), size_(size), isEnd_(false) {
	lines_ = new LineItem[size_];
	for (int i = 0; i < size_; i++) {
		lines_[i].msg = NULL;
	}
}

CLineBuffer::~CLineBuffer() {
	for (int i = 0; i < size_; i++) {
		delete[] lines_[i].msg;
	}
	delete[] lines_;
}

int CLineBuffer::enqueue(int line, char* msg, int len) {
	assert(line < size_);
	if (lines_[line].msg != NULL) {
		fprintf(stderr, "enqueue line is not NULL, line=%d\n", line);
	}
	lines_[line].msg = msg;
	lines_[line].len = len;
}

int CLineBuffer::dequeue(LineItem*& pItem, int& len) {
	int i = 0;
	while (!lines_[dequeuePos_].msg) {
		if (++i > MAX_TRY) break;
		usleep(1);
	}
    int start = dequeuePos_;
    pItem = &lines_[dequeuePos_];
	while(lines_[dequeuePos_].msg) {
        dequeuePos_++;
	}
    len = dequeuePos_ - start;

	return dequeuePos_;
}
