CXX=g++
CXXFLAG=-g -O2 -Wall -DNDEBUG
LIBS=-lpthread
all: clean server

server:
	${CXX} ${CXXFLAG} -o server server.cc common.cc handle.cc data_queue.cc mmap_mempool64.cc socket_buff.cc connection.cc epoll_worker.cc ${LIBS}

clean:
	rm -f *.o server
