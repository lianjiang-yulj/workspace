all:server client test_buffer


CXXFLAGS=-g -O2
LDFLAGS=-g -O2 -lpthread 


server:server.o Buffer.o server_handler.o fileReader.o protocal.o
	g++ $^ -o $@ ${LDFLAGS}

client:client.o  Buffer.o fileWriter.o connection.o protocal.o bizClient.o
	g++ $^ -o $@ ${LDFLAGS}

test_buffer:test_buffer.o Buffer.o 
	g++ $^ -o $@ ${LDFLAGS}

.cpp.o:
	g++ -c $< -o $@ ${CXXFLAGS}

.PHONY:clean

clean:
	rm -rf server
	rm -rf client
	rm -rf *.o
