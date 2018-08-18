all: clean client
client:
	g++ -O2 -DNDEBUG client.cpp req_queue.cpp command.cpp linebuf.cpp ../server_noqueue/socket_buff.cc ioprocess.cpp -lpthread -o client -g
	#g++ -ggdb client.cpp req_queue.cpp command.cpp linebuf.cpp ../server_noqueue/socket_buff.cc ../server_noqueue/common.cc ioprocess.cpp -lpthread -o client -g
clean:
	rm -f client
