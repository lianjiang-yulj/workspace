#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define MAX_IP_LEN 32
#define MAX_FILENAME_LEN  256

const int MAX_THREAD_CNT = 255;
const int BUF_LINE = 8096;

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// 命令行参数结构
typedef struct cmdline_param {
	int                     request_thread_cnt;
	int                     conn_thread_cnt;
	int                     write_thread_cnt;
	const char*             host;
	uint16_t                port;
	const char*             filename;

	cmdline_param(): request_thread_cnt(1), conn_thread_cnt(1), write_thread_cnt(1), host(NULL), port(80), filename(NULL) {}
} cmdline_param;

typedef struct thread_param {
	int                      id;  // 当前线程编号
	int                      connid;  // 使用的连接线程编号
	const char*              host;
	uint16_t                 port;
	int                      sockfd;
	char*                    filebase;
	FILE*                    fp;
	int                      fd;
} thread_param;

#endif

