#ifndef _DATA_QUEUE_H_
#define _DATA_QUEUE_H_

#include <vector>
#include <pthread.h>

#include "datatype.h"
#include "req_queue.h"
#include "../server_epoll/socket_buff.h"
#include "linebuf.h"

class CCondition {
public:
	CCondition() {
		if (pthread_mutex_init(&_mutex,NULL) != 0) {
			perror("pthread_mutex_init error");
		}   
		if (pthread_cond_init(&_cond, NULL) != 0) {
			perror("pthread_cond_init error");
		}
	}
	~CCondition() {
		if (pthread_mutex_destroy(&_mutex) != 0) {
			perror("pthread_mutex_destroy error");
		}   
		if (pthread_cond_destroy(&_cond) != 0) {
			perror("pthread_cond_destroy error");
		}   
	}

	void lock() {
		pthread_mutex_lock(&_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&_mutex);
	}
	void wait() {
		pthread_cond_wait(&_cond, &_mutex);
	}
	void timedwait(struct timespec& timer) {
		pthread_cond_timedwait(&_cond, &_mutex, &timer);
	}
	void signal() {
		pthread_cond_signal(&_cond);
	}
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

class CIOProcess {
public:
	CIOProcess(CLineBuffer* lineBuf);
	~CIOProcess();

	void bindSocket(int sockfd);

	int doRequest(SRequestItem* pReqItem);
	int send(char* buf, int len);
	void makeRequest(SRequestPacket* packet);
	int sendRequest(int start, int packetCnt);
	int recv(SocketBuff* pSocketBuff);

private:
	ReqQueue _reqQueue;
	int _sockfd;
	CCondition _conds[MAX_ITEMS];
	volatile bool _respFlags[MAX_ITEMS];
	CLineBuffer* _lineBuffer;
	volatile bool _isEnd;
	CCondition _workerCond;
	std::vector<SRequestItem> _reqVec;
};

#endif
