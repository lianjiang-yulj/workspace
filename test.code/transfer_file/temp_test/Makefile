#This is the makefile of EpollTest

all: clean server client ev_server_single ev_client ev_server_multi multi_async_ev_server

server:
		gcc server.c -o server
client:
		gcc client.c -o client
ev_server_single:
		g++ -g -O2 ev_server_single.cpp Thread.cpp Lock.cpp MMapMempool64.cpp data_queue.cpp -o ev_server_single -I/usr/local/include -L/usr/local/lib -lev -lpthread
ev_server_multi:
		g++ -g -O2 ev_server_multi.cpp Thread.cpp Lock.cpp MMapMempool64.cpp data_queue.cpp -o ev_server_multi -I/usr/local/include -L/usr/local/lib -lev -lpthread
ev_client:
		g++ -g -O2 ev_client.cpp -o ev_client
multi_async_ev_server:
		g++ -g -O2 multi_async_ev_server.cpp Thread.cpp MMapMempool64.cpp data_queue.cpp Lock.cpp -o multi_async_ev_server -I/usr/local/include -L/usr/local/lib -lev -lpthread
clean:
		rm -f server client ev_server_single ev_client ev_server_multi multi_async_ev_server
