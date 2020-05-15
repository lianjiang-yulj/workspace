#include "ioprocess.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"

const int MaxPacketCount = 255;

#define printf(...)
//#define fprintf(...)

using std::vector;

CIOProcess::CIOProcess(CLineBuffer* lineBuf) : _sockfd(-1), _lineBuffer(lineBuf), _isEnd(false)
{
}

CIOProcess::~CIOProcess()
{
}

void CIOProcess::bindSocket(int sockfd)
{
	_sockfd = sockfd;
}

int CIOProcess::doRequest(SRequestItem* pReqItem)
{
	if (_isEnd) return -1;
	int channelId = pReqItem->channelId;

	_respFlags[channelId] = false;

	_reqQueue.enqueue(*pReqItem);

	_conds[channelId].lock();
	printf("send request and wait for response, channelId=%d\n", channelId);
	if (!_respFlags[channelId]) {
		_conds[channelId].wait();
	}
		//struct timespec timer;
		//timer.tv_sec = time(NULL) + 1;
		//timer.tv_nsec=0;
		//_conds[channelId].timedwait(timer);
	printf("recv response, channelId=%d\n", channelId);
	_conds[channelId].unlock();

	return 0;
}

int CIOProcess::send(char* buf, int len)
{
	_reqVec.clear();
	_reqQueue.dequeue(_reqVec);
	size_t size = _reqVec.size();

	size_t remainCnt = size;
	char* p = buf;
	while (remainCnt > 0) {
		int packetCnt = MIN(remainCnt, MaxPacketCount);
		uint16_t packetLen = packetCnt * sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t);
		memcpy(p, (char*)&packetLen, sizeof(uint16_t));
		p += sizeof(uint16_t);
		memcpy(p, (char*)&packetCnt, sizeof(uint8_t));
		p += sizeof(uint8_t);
		int start = size - remainCnt;
		int end = start + packetCnt;
		for (int i = start; i < end; ++i) {
			memcpy(p, (char*)&(_reqVec[i].channelId), sizeof(uint16_t));
			p += sizeof(uint16_t);
		}
		remainCnt -= packetCnt;
	}
	assert(p - buf <= len);

	return p - buf;
}

int CIOProcess::sendRequest(int start, int packetCnt)
{
	if (packetCnt < 1) {
		perror("packetCnt < 1");
		return 1;
	}

	char buf[1024];
	char* p = &buf[0];
	uint16_t packetLen = packetCnt * sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t);
	memcpy(p, (char*)&packetLen, sizeof(uint16_t));
	p += sizeof(uint16_t);
	memcpy(p, (char*)&packetCnt, sizeof(uint8_t));
	p += sizeof(uint8_t);
	int end = start + packetCnt;
	for (int i = start; i < end; ++i) {
		memcpy(p, (char*)&(_reqVec[i].channelId), sizeof(uint16_t));
		p += sizeof(uint16_t);
	}
	*p = '\0';
	int ret = 0;
	//fprintf(stderr, "send packet, packet_count=%d, len=%d, buf=%s\n", packetCnt, p - buf, buf + 1);
	ret = ::send(_sockfd, buf, p - buf, 0);
	if (ret < 0) {
		perror("send error");
		return 1;
	}
	//assert(ret == p - buf);
	//assert(ret == packetCnt + sizeof(uint8_t) + sizeof(uint16_t));

	return 0;
}

int CIOProcess::recv(SocketBuff* pSocketBuff)
{
	char* buf = NULL;
	int32_t len = 0;

	while (true) {
		bool flag = pSocketBuff->NextPacket(buf, len);
		if (!flag) {
			//fprintf(stderr, "_sockBuff.NextPacket return false\n");
			return _isEnd ? -1 : 0;
		}

		char* p = buf;
		int packetCount = *(uint8_t*)p;
		p += sizeof(uint8_t);
		char linestr[11];
		for (int i = 0; i < packetCount; ++i) {
			int channelId = *(uint16_t*)p;
			SResponseItem item;
			item.channelId = channelId;
			p += sizeof(uint16_t);
			item.msglen = *(uint8_t*)p;
			p += sizeof(uint8_t);
			item.linenum = *(int*)p;
			p += sizeof(int);
			item.msglen = item.msglen - sizeof(uint8_t) - sizeof(int);
			item.msg = new char[item.msglen + 1 + 10];
			int lineLen = snprintf(linestr, 11, "%d", item.linenum);
			memcpy(item.msg, linestr, lineLen);
			memcpy(item.msg + lineLen, p, item.msglen);
			p += item.msglen;
            item.msglen += lineLen;
			item.msg[item.msglen] = '\0';

			if (item.linenum >= 0) {
				_lineBuffer->enqueue(item.linenum, item.msg, item.msglen);
			}
			else {
                /*
                if (!_isEnd)
                fprintf(stderr, "thread_id=%ld, linenum=%d, exiting...\n", pthread_self(), item.linenum);
                */
				_isEnd = true;
			}
			/*
            if (item.linenum == 31423541) {
                struct timeval end_time;
                if (gettimeofday(&end_time, NULL) < 0) {
                    perror("gettimeofday error");
                }
                fprintf(stderr, "recv end time is %d\n", end_time.tv_sec);
            }
            */
	//		_conds[channelId].lock();
			_respFlags[channelId] = true;
			printf("channelId=%d signal, linenum=%d\n", channelId, item.linenum);
			_conds[channelId].signal();
	//		_conds[channelId].unlock();
		}
		//fprintf(stdout, "recv from server:packetCount=%d,packet_len=%d, process_len=%d\n", packetCount, len, p - buf);
		assert (len == p - buf);
	}

	return _isEnd ? -1 : 0;
}
