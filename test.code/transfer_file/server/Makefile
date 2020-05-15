CXX=g++
CXXFLAG=-g -O2 -Wall
LIBS=-lpthread
all: clean server

server:
	${CXX} ${CXXFLAG} -o server server.cc common.cc handle.cc data_queue.cc mmap_mempool64.cc req_queue.cc brick_queue.cc socket_buff.cc ${LIBS}

clean:
	rm -f *.o server
