all: clean client
client:
	g++ client.cpp command.cpp ringbuf.cpp ../server/socket_buff.cc ../server/common.cc ioprocess.cpp -lpthread -o client -g -O2
clean:
	rm -f client
