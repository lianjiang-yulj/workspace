#This is the makefile of EpollTest

all: clean zmq_server zmq_client mtserver mtclient mtzmq_s mtzmq_c test_zs test_zc

zmq_server:
	g++ -g -O2 zmq_server.cpp -o zmq_server -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
zmq_client:
	g++ -g -O0 zmq_client.cpp -o zmq_client -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
mtserver:
	g++ -g -O2 mtserver.cpp -o mtserver -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
mtclient:
	g++ -g -O2 mtclient.cpp -o mtclient -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
mtzmq_s:
	g++ -g -O0 mtzmq_s.cpp ../MMapMempool64.cpp ../data_queue.cpp -o mtzmq_s -I../ -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
mtzmq_c:
	g++ -g -O0 mtzmq_c.cpp ../MMapMempool64.cpp ../data_queue.cpp -o mtzmq_c -I../ -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
test_zs:
	g++ -g -O0 test_zs.cpp -o test_zs -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
test_zc:
	g++ -g -O0 test_zc.cpp -o test_zc -I/usr/local/include -L/usr/local/lib/ -lzmq -lpthread
clean:
	rm -f zmq_server zmq_client mtserver mtclient mtzmq_s mtzmq_c test_zs test_zc

#-I/usr/src/kernels/2.6.18-274.el5-x86_64/include/asm-x86_64 -I/usr/src/kernels/2.6.18-274.el5-x86_64/include/ 
