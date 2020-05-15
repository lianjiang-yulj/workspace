all: mmapfile mmapfilet
mmapfile: mmapfile.o
	g++ -ggdb -g -c mmapfile.cpp
	g++ -ggdb -g -o mmapfile mmapfile.o
mmapfilet: mmapfilet.o
	g++ -ggdb -g -c mmapfilet.cpp
	g++ -ggdb -g -o mmapfilet mmapfilet.o
clean:
	rm *.o mmapfile mmapfilet

