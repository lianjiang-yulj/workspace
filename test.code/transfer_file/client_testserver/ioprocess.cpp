#include "ioprocess.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>

#define printf(...)

CIOProcess::CIOProcess() : _sockfd(-1), _pSockBuff(NULL)
{
	if (pthread_mutex_init(&_reqMutex,NULL) != 0) {
		perror("pthread_mutex_init error");
	}
	if (pthread_mutex_init(&_respMutex,NULL) != 0) {
		perror("pthread_mutex_init error");
	}
}

CIOProcess::~CIOProcess()
{
	if (pthread_mutex_destroy(&_reqMutex) != 0) {
		perror("pthread_mutex_destroy error");
	}
	if (pthread_mutex_destroy(&_respMutex) != 0) {
		perror("pthread_mutex_destroy error");
	}
	delete _pSockBuff;
	_pSockBuff = NULL;
}

void CIOProcess::bindSocket(int sockfd)
{
	_sockfd = sockfd;
	_pSockBuff = new SocketBuff(sockfd);
}

int CIOProcess::doRequest(SRequestItem* pReqItem, SResponseItem*& pRespItem)
{
	if (!pReqItem) {
		printf("in putRequest: pReqItem is NULL\n");
		return 1;
	}
	int channelId = pReqItem->channelId;
	_respItems[channelId].valid = false;
	_reqCond.lock();
	for(int i=0;i<1;i++)
	_reqQueue.push(pReqItem);
	printf("push item to _reqQueue, _reqQueue.size=%d\n", _reqQueue.size());
	if (_reqQueue.size() == 1) {
		_reqCond.signal();
	}
	_reqCond.unlock();

	_conds[channelId].lock();
	printf("send request and wait for response, channelId=%d\n", channelId);
	if (!_respItems[channelId].valid) {
		_conds[channelId].wait();
	}
	printf("recv response, channelId=%d\n", channelId);
	pRespItem = &_respItems[channelId];
	_conds[channelId].unlock();

	return 0;
}

int CIOProcess::send()
{
	makeRequest(&_packet);
	sendRequest(_packet);
}

void CIOProcess::makeRequest(SRequestPacket* packet)
{
	//_reqCond.lock();
	//if (_reqQueue.empty()) {
	//	_reqCond.wait();
	//}
	//size_t size = _reqQueue.size();
	//printf("make request, _reqQueue.size=%d", _reqQueue.size());
	size_t size = 10;
	packet->packetCount = (uint8_t)size;
	for (size_t i = 0; i < size; ++i) {
		packet->requestItems[i] = new SRequestItem;
		packet->requestItems[i]->channelId = i;
	}
	//_reqCond.unlock();
}

int CIOProcess::sendRequest(SRequestPacket& packet)
{
	if (packet.packetCount < 1) {
		perror("packet.packetCount < 1");
		return 1;
	}

	char buf[1024];
	char* p = &buf[0];
	uint16_t packetLen = packet.packetCount + sizeof(uint8_t) + sizeof(uint16_t);
	memcpy(p, (char*)&packetLen, sizeof(uint16_t));
	p += sizeof(uint16_t);
	memcpy(p, (char*)&packet.packetCount, sizeof(uint8_t));
	p += sizeof(uint8_t);
	for (int i = 0; i < packet.packetCount; ++i) {
		memcpy(p, (char*)&packet.requestItems[i]->channelId, sizeof(uint8_t));
		p += sizeof(uint8_t);
	}
	*p = '\0';
	int ret = 0;
	char recvbuf[8192];
	while(1) {
		ret = ::send(_sockfd, buf, p - buf, 0);
		if (ret < 0) {
			perror("send error");
			//return 1;
		}
		printf("send packet, len=%d, ret=%d\n", p - buf, ret);
		//assert(ret == 4);
		ret = ::recv(_sockfd, recvbuf, 8192, 0);
		if (ret < 0) {
			perror("recv error");
			//return 1;
		}
		recvbuf[ret] = '\0';
		printf("recv bytes=%d\n", ret);
		/*
		p = &buf[0];
		packet.packetCount = 1;
		packetLen = packet.packetCount + sizeof(uint8_t) + sizeof(uint16_t);
		memcpy(p, (char*)&packetLen, sizeof(uint16_t));
		p += sizeof(uint16_t);
		memcpy(p, (char*)&packet.packetCount, sizeof(uint8_t));
		p += sizeof(uint8_t);
		for (int i = 0; i < packet.packetCount; ++i) {
			memcpy(p, (char*)&packet.requestItems[i]->channelId, sizeof(uint8_t));
			p += sizeof(uint8_t);
		}
		*p = '\0';
		*/

	}

	return 0;
}

int CIOProcess::recv()
{
	//char* buf = NULL;
	int32_t len = 8096;
	char buf[8096];
	while(1) {
	 ::recv(_sockfd, buf, len, 0);
	}


	//bool flag = _pSockBuff->NextPacket(buf, len);
	//if (!flag) {
	//	fprintf(stderr, "_sockBuff.NextPacket return false\n");
	//	return 0;
	//}

	return 0;

	char* p = buf;
	int packetCount = *(uint8_t*)p;
	p += sizeof(uint8_t);
	for (int i = 0; i < packetCount; ++i) {
		int channelId = *(uint8_t*)p;
		SResponseItem* pItem = &_respItems[channelId];
		pItem->channelId = channelId;
		p += sizeof(uint8_t);
		pItem->msglen = *(uint8_t*)p;
		p += sizeof(uint8_t);
		pItem->linenum = *(int*)p;
		p += sizeof(int);
		pItem->msglen = pItem->msglen - sizeof(uint8_t) - sizeof(int);
		pItem->msg = new char[pItem->msglen + 1];
		memcpy(pItem->msg, p, pItem->msglen);
		p += pItem->msglen;
		pItem->msg[pItem->msglen] = '\0';
		
		_conds[channelId].lock();
		_respItems[channelId].valid = true;
		printf("channelId=%d signal, linenum=%d\n", channelId, pItem->linenum);
		_conds[channelId].signal();
		_conds[channelId].unlock();
	}
	printf("recv from server:packetCount=%d,packet_len=%d, process_len=%d\n", packetCount, len, p - buf);
	//assert (len == p - buf);

	return 0;
}
