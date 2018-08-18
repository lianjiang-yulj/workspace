all: clean client
client:
	g++ -g -O2 client.cpp req_queue.cpp connection.cc command.cpp linebuf.cpp epoll_worker.cc ../server_epoll/common.cc ../server_epoll/socket_buff.cc ioprocess.cpp -lpthread -o client
	#g++ -O2 -DNDEBUG client.cpp req_queue.cpp connection.cc command.cpp linebuf.cpp epoll_worker.cc ../server_epoll/common.cc ../server_epoll/socket_buff.cc ioprocess.cpp -lpthread -o client

clean:
	rm -f client
