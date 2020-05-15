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

#include "common.h"
#include "command.h"
//#include "atomic.h"
#include "ringbuf.h"
#include "ioprocess.h"
#include "../server/common.h"

#define printf(...)

CIOProcess** gIOProcess = NULL;

CRingBuffer gRingBuffer(1U << 20);

const size_t MMAP_SIZE = 1UL << 4;
const int FILE_EXPEND_STEP = 1U << 2;

bool isStop = false;

void* do_conn_read(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;
	int ret = 0;
	while (!isStop) {
		if ((ret = gIOProcess[pthr_param->id]->recv()) != 0) {
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

	//set_socket_max_buffer(sockfd);
	int size = 1UL << 24;
	if ( setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int)) != 0 ) {perror("setsockopt error");}
	fprintf(stdout, "set sock size=%d\n", size);

	int getsize = 0;
	socklen_t tsize = sizeof(int);
	if ( getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &getsize, &tsize) != 0 ) {perror("getsockopt error");}
	fprintf(stdout, "tid=%ld,sockfd=%d,SO_SNDBUF=%d,tsize=%d\n", pthread_self(), sockfd, getsize, tsize);
	getsize = 0; tsize=sizeof(int);
	if ( getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &getsize, &tsize) != 0 ) {perror("getsockopt error");}
	fprintf(stdout, "tid=%ld,sockfd=%d,SO_RCVBUF=%d,tsize=%d\n", pthread_self(), sockfd, getsize,tsize);

	//set_socket_nodelay(sockfd, 1);
	// bind a socket to dataqueue
	gIOProcess[pthr_param->id]->bindSocket(sockfd);

	pthread_t readpid;
	int ret = 0;
	//if ((ret = pthread_create(&readpid, NULL, do_conn_read, pthr_param)) != 0) {
	//	perror("create do_conn_read thread error");
	//}

	while (!isStop) {
		if ((ret = gIOProcess[pthr_param->id]->send()) != 0) {
			perror("call gDataQueue->send failed");
		}
	}

	pthread_join(readpid, NULL);
	delete pthr_param;
}

void* do_request(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;

	SResponseItem* pRespItem = NULL;
	SRequestItem reqItem;
	reqItem.channelId = pthr_param->id;
	int conn_id = pthr_param->connid;
	int ret = 0;
	while (!isStop) {
		if ((ret = gIOProcess[conn_id]->doRequest(&reqItem, pRespItem)) == 0) {
			printf("channelId=%d, msglen=%d, linenum=%d, msg=%s\n", pRespItem->channelId, pRespItem->msglen, pRespItem->linenum, pRespItem->msg);
			if (pRespItem->linenum < 0) {
				printf("recv linenum=%d, exit do_request\n", pRespItem->linenum);
				break;
			}
		}
	}
	fprintf(stderr, "isStop=%d, exit do_request\n", isStop);
	delete pthr_param;
}

void* do_write(void* arg) {
	thread_param* pthr_param = (thread_param*) arg;
	int pos;
	SBufItem* item = NULL;
	off_t offset = 0;
	off_t maxoffset = 0;
	while (!isStop) {
		if (gRingBuffer.readNext(&pos, &item) == 0) {
			//printf("read data at %d, offset=%ld,filebase=%p\n", pos, offset, pthr_param->filebase);
			if (maxoffset < offset + item->msglen) {
				maxoffset += FILE_EXPEND_STEP;
				if (lseek(pthr_param->fd, maxoffset, SEEK_SET) == (off_t)(-1)) {
					perror("lseek error");
					continue;
				}
				if (write(pthr_param->fd, item->msg, 1) < 0) {
					perror("write error");
					continue;
				}
			}
			//memcpy(pthr_param->filebase + offset, item->msg, item->msglen);
			//memcpy(pthr_param->filebase + 150000, item->msg, item->msglen);
			offset += item->msglen;
			gRingBuffer.next();
		}
	}
	msync(pthr_param->filebase, offset, 0);
}

int main(int argc, char ** argv)
{
	struct timeval start_time;
	if (gettimeofday(&start_time, NULL) < 0) {
		perror("gettimeofday error");
	}
	printf("start time is %d\n", start_time.tv_sec);
	
	struct cmdline_param cmd_param;
	pthread_t conn_pids[MAX_THREAD_CNT];
	pthread_t request_pids[MAX_THREAD_CNT];
	pthread_t write_pids[MAX_THREAD_CNT];

	if (parse_cmd_line(argc, argv, &cmd_param)) {
		return 1;
	}
	
	int fd = open(cmd_param.filename, O_CREAT|O_TRUNC|O_RDWR, 0644);
	if (fd < 0) {
		fprintf(stderr, "open file error : %s\n", strerror(errno));
		return 1;
	}

	char* base = (char*)mmap(0, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); 
	if (base == MAP_FAILED) {
		fprintf(stderr, "mmap error : %s\n", strerror(errno));
		return 3;
	}

	gIOProcess = new CIOProcess*[cmd_param.conn_thread_cnt];
	for (int i = 0;i < cmd_param.conn_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		pthr_param->fd = fd;
		pthr_param->filebase = base;
		pthr_param->host = cmd_param.host;
		pthr_param->port = cmd_param.port;
		pthr_param->id = i;

		gIOProcess[pthr_param->id] = new CIOProcess;
		int ret = pthread_create(&conn_pids[i], NULL, do_conn, pthr_param);
		if (ret) {
			perror("pthread_create error");
			return 2;
		}
	}

	for (int i = 0;i < cmd_param.request_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		pthr_param->fd = fd;
		pthr_param->filebase = base;
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

	for (int i = 0;i < cmd_param.write_thread_cnt; ++i) {
		struct thread_param* pthr_param = new thread_param;
		pthr_param->fd = fd;
		pthr_param->filebase = base;
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

	for (int i = 0;i < cmd_param.request_thread_cnt; ++i) {
		pthread_join(request_pids[i], NULL);
	}

	for (int i = 0;i < cmd_param.write_thread_cnt; ++i) {
		pthread_join(write_pids[i], NULL);
	}
	close(fd);
	
	struct timeval end_time;
	if (gettimeofday(&end_time, NULL) < 0) {
		perror("gettimeofday error");
	}
	printf("end time is %d\n", end_time.tv_sec);
	time_t cost = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec);
	printf("cost=%ld\n", cost);
} 
