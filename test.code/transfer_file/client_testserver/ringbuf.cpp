#include "ringbuf.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

CRingBuffer::CRingBuffer(int size) : _size(size), _readPos(0), _isStop(false)
{
	if (pthread_mutex_init(&_mutex,NULL) != 0) {
		perror("pthread_mutex_init error");
	}
	if (pthread_cond_init(&_cond, NULL) != 0) {
		perror("pthread_cond_init error");
	}
	_items = new SBufItem*[_size];
	for (int i = 0; i < _size; ++i) {
		_items[i] = new SBufItem;
		_items[i]->valid = false;
	}
}

CRingBuffer::~CRingBuffer()
{
	if (pthread_mutex_destroy(&_mutex) != 0) {
		perror("pthread_mutex_destroy error");
	}
	if (pthread_cond_destroy(&_cond) != 0) {
		perror("pthread_cond_destroy error");
	}
	for (int i = 0; i < _size; ++i) {
		delete _items[i];
	}
	delete[] _items;
	_items = NULL;
}

int CRingBuffer::readNext(int* pos, SBufItem** item) 
{
	//printf("%d:%d\n", _readPos ,_items[_readPos]->valid);

	//pthread_mutex_lock(&_mutex);
	//while (!_items[_readPos]->valid && !_isStop) {
	//        if (_readPos % 10000 == 0)
	//	printf("%d read wait, valid=%d\n", _readPos, _items[_readPos]->valid);
	//	pthread_cond_wait(&_cond, &_mutex);
	//}
	//pthread_mutex_unlock(&_mutex);

	if (_items[_readPos]->valid) {
		*item = _items[_readPos];
		*pos = _readPos;
		return 0;
	}
	else {
	//	perror("readNext error");
		return 1;
	}
}

int CRingBuffer::next()
{
	_items[_readPos]->valid = false;
	//if (_readPos % 1000 == 0)
		//printf("process readPos=%d\n", _readPos);
	_readPos = (_readPos + 1) % _size;
	return 0;
}

int CRingBuffer::write(int linenum, char* msg, int len)
{
	int writePos = linenum % _size;
	//while (_items[writePos]->valid && !_isStop) {
	//	usleep(1);
		//printf("%d write wait, valid=%d\n", linenum, _items[writePos]->valid);
	//}

	if (!_items[writePos]->valid) {
		memcpy(_items[writePos]->msg, msg, len);
		//printf("write linenum=%d\n", linenum);
		_items[writePos]->linenum = linenum;
		_items[writePos]->msglen = len;
		_items[writePos]->valid = true;

		if (writePos == _readPos) {
			//pthread_mutex_lock(&_mutex);
			//pthread_cond_signal(&_cond);
			//pthread_mutex_unlock(&_mutex);
		}
	}
	else {
	//	printf("readPos=%d, linenum=%d, valid=%d\n", _readPos, linenum, _items[writePos]->valid);
	//	perror("overwrite valid date");
	}
}

void CRingBuffer::finish()
{
	//pthread_mutex_lock(&_mutex);
	_isStop = true;
	//pthread_cond_signal(&_cond);
	//pthread_mutex_unlock(&_mutex);
}

