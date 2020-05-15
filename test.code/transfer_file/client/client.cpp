#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include <vector>
#include <iostream>

#include "common.h"
#include "command.h"
//#include "atomic.h"
#include "linebuf.h"
#include "ioprocess.h"

using namespace std;

#define printf(...)

CIOProcess** gIOProcess = NULL;

//CLineBuffer gLineBuffer(1U << 2);
CLineBuffer gLineBuffer(1U << 28);

volatile bool isStop = false;
bool volatile isThreadStop[256];

void* do_conn_read(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;
	int ret = 0;
	CIOProcess* pProcess = gIOProcess[pthr_param->id];
	while (true) {
		if ((ret = pProcess->recv()) != 0) {
			//fprintf(stdout, "id=%d recv end flag, exit\n", pthr_param->id);
			isThreadStop[pthr_param->id] = true;
	//		perror("call gDataQueue->recv failed");
			break;
		}
	}
}

void* do_conn(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0) {
		perror("open socket error");
		return NULL;
	}
	
	struct sockaddr_in remote_addr;
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(pthr_param->host);
	remote_addr.sin_port = htons(pthr_param->port);

	if (connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr)) < 0) {
		perror("connect error");
		return NULL;
	}
	pthr_param->sockfd = sockfd;
	// bind a socket to dataqueue
	gIOProcess[pthr_param->id]->bindSocket(sockfd);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct sched_param sched;
    pthread_attr_getschedparam(&attr, &sched);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    sched.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &sched);

	pthread_t readpid;
	int ret = 0;
	if ((ret = pthread_create(&readpid, &attr, do_conn_read, pthr_param)) != 0) {
		perror("create do_conn_read thread error");
	}

	CIOProcess* pProcess = gIOProcess[pthr_param->id];
	int id = pthr_param->id;
	while (!isThreadStop[id]) {
		if ((ret = pProcess->send()) != 0) {
			perror("call gDataQueue->send failed");
		}
	}

	//pthread_join(readpid, NULL);
	if (close(sockfd) < 0) {
		perror("close sock error");
	}
	delete pthr_param;
}

void* do_request(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;

	SResponseItem* pRespItem = NULL;
	SRequestItem reqItem;
	reqItem.channelId = pthr_param->id;
	int conn_id = pthr_param->connid;
	int ret = 0;
	CIOProcess* pProcess = gIOProcess[conn_id];
	while (!isThreadStop[conn_id]) {
		if ((ret = pProcess->doRequest(&reqItem)) < 0) {
			perror("gIOProcess->doRequest error");
		}
	}
	printf("isStop=%d, exit do_request\n", isStop);
	delete pthr_param;
}

void* do_write(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;
	
	FILE* fp = NULL;
	if ((fp = fopen(pthr_param->filename, "w+")) == NULL) {
		perror("fopen file error");
		return NULL;
	}
	vector<char*> msgVec;
	while (!isStop) {
		msgVec.clear();
		gLineBuffer.dequeue(msgVec);
		for(vector<char*>::iterator it = msgVec.begin(); it != msgVec.end(); ++it) {
			fputs(*it, fp);
			fputs("\r\n", fp);
		}
	}
	msgVec.clear();
	int rc = gLineBuffer.dequeue(msgVec);
	for(vector<char*>::iterator it = msgVec.begin(); it != msgVec.end(); ++it) {
		fputs(*it, fp);
		fputs("\r\n", fp);
	}

	fclose(fp);
	std::cout << "max line number: " << rc << std::endl;
}

int main(int argc, char ** argv)
{
	struct timeval start_time;
	if (gettimeofday(&start_time, NULL) < 0) {
		perror("gettimeofday error");
	}
	fprintf(stdout, "start time is %d\n", start_time.tv_sec);
	
	struct cmdline_param cmd_param;
	pthread_t conn_pids[MAX_THREAD_CNT];
	pthread_t request_pids[MAX_THREAD_CNT];
	pthread_t write_pids[MAX_THREAD_CNT];

	if (parse_cmd_line(argc, argv, &cmd_param)) {
		return 1;
	}

	for (int i = 0; i < 256; ++i) {
		isThreadStop[i] = false;
	}
	
	gIOProcess = new CIOProcess*[cmd_param.conn_thread_cnt];
    for (int i = 0;i < cmd_param.conn_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		//pthr_param->filename = cmd_param.filename;
		pthr_param->host = cmd_param.host;
		pthr_param->port = cmd_param.port;
		pthr_param->id = i;

		gIOProcess[pthr_param->id] = new CIOProcess(&gLineBuffer);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        struct sched_param sched;
        pthread_attr_getschedparam(&attr, &sched);
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        sched.sched_priority = sched_get_priority_max(SCHED_RR);
        pthread_attr_setschedparam(&attr, &sched);
        //pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);


		int ret = pthread_create(&conn_pids[i], &attr, do_conn, pthr_param);
		if (ret) {
			perror("pthread_create error");
			return 2;
		}
	}

	for (int i = 0;i < cmd_param.request_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		//pthr_param->filename = cmd_param.filename;
		pthr_param->host = cmd_param.host;
		pthr_param->port = cmd_param.port;
		pthr_param->id = i;
		pthr_param->connid = i % cmd_param.conn_thread_cnt;
		int ret = pthread_create(&request_pids[i], NULL, do_request, pthr_param);
		if (ret) {
			perror("pthread_create error");
			return 2;
		}
	}

	cmd_param.write_thread_cnt = 1;
	for (int i = 0;i < cmd_param.write_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		pthr_param->filename = cmd_param.filename;
		pthr_param->host = cmd_param.host;
		pthr_param->port = cmd_param.port;
		pthr_param->id = i;
		int ret = pthread_create(&write_pids[i], NULL, do_write, pthr_param);
		if (ret) {
			perror("pthread_create error");
			return 2;
		}
	}

	for (int i = 0;i < cmd_param.conn_thread_cnt; ++i) {
		pthread_join(conn_pids[i], NULL);
	}

    /*
	for (int i = 0;i < cmd_param.request_thread_cnt; ++i) {
		pthread_join(request_pids[i], NULL);
	}
    */
	isStop = true;

	for (int i = 0;i < cmd_param.write_thread_cnt; ++i) {
		pthread_join(write_pids[i], NULL);
	}
	
	struct timeval end_time;
	if (gettimeofday(&end_time, NULL) < 0) {
		perror("gettimeofday error");
	}
	fprintf(stdout, "end time is %d\n", end_time.tv_sec);
	time_t cost = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;
	fprintf(stdout, "cost=%ld\n", cost);
} 
