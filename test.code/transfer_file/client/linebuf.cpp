#include "linebuf.h"
#include <assert.h>
#include <unistd.h>

const int MAX_TRY = 100;

CLineBuffer::CLineBuffer(int size) : dequeuePos_(0), size_(size), isEnd_(false) {
	lines_ = new char*[size_];
	for (int i = 0; i < size_; i++) {
		lines_[i] = NULL;
	}
}

CLineBuffer::~CLineBuffer() {
	for (int i = 0; i < size_; i++) {
		delete[] lines_[i];
	}
	delete[] lines_;
}

int CLineBuffer::enqueue(int line, char* msg) {
	assert(line < size_);
	if (lines_[line] != NULL) {
		fprintf(stderr, "enqueue line is not NULL, line=%d\n", line);
	}
	lines_[line] = msg;
}

int CLineBuffer::dequeue(std::vector<char*>& lineVec) {
	int i = 0;
	while (!lines_[dequeuePos_]) {
		if (++i > MAX_TRY) break;
		usleep(1);
	}
	while(lines_[dequeuePos_]) {
		lineVec.push_back(lines_[dequeuePos_++]);
	}
	return dequeuePos_;
}
