all: libfile.so libfile.a  testldd testla
libfile.so: filea.o fileb.o
	g++ -g -ggdb -fPIC -shared -o ./lib/libfile.so  filea.o fileb.o
libfile.a: filea.o fileb.o
	ar -cvr ./lib/libfile.a filea.o fileb.o 
filea.o: filea.cpp 
	g++ -g -ggdb -fPIC -c filea.cpp -I.
fileb.o: fileb.cpp
	g++ -g -ggdb -fPIC -c fileb.cpp -I.
testldd: testldd.cpp
	g++ -g -ggdb -o testldd  -L./lib  -lfile  testldd.cpp -I.
testla: testldd.cpp
	g++ -g -ggdb -static -o testla testldd.cpp -L./lib  -lfile -I.
clean: 
	rm *.o  testldd testla

