#ifndef _DATA_QUEUE_H_
#define _DATA_QUEUE_H_

#include <queue>
#include <pthread.h>
#include "datatype.h"
#include "../server/socket_buff.h"

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
	void signal() {
		pthread_cond_signal(&_cond);
	}
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

class CIOProcess {
public:
	CIOProcess();
	~CIOProcess();

	void bindSocket(int sockfd);

	int doRequest(SRequestItem* pReqItem, SResponseItem*& pRespReqItem);
	int send();
	void makeRequest(SRequestPacket* packet);
	int sendRequest(SRequestPacket& packet);
	int recv();

private:
	std::queue<SRequestItem*> _reqQueue;
	CCondition _reqCond;
	SRequestPacket _packet;
	std::queue<SResponseItem*> _respQueue;
	pthread_mutex_t _reqMutex;
	pthread_mutex_t _respMutex;
	int _sockfd;
	CCondition _conds[MAX_ITEMS];
	SResponseItem _respItems[MAX_ITEMS];
	SocketBuff* _pSockBuff;
};

#endif
