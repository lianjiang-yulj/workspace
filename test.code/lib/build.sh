#!/bin/sh

g++ -c liba.cpp libb.cpp lib.cpp

ar crv lib.a  lib.o
ar crv liba.a  liba.o lib.o
ar crv libb.a  libb.o lib.o

g++ -g -fPIC -shared -o lib.so lib.cpp
g++ -g -fPIC -shared -o liba.so liba.cpp lib.cpp
g++ -g -fPIC -shared -o libb.so libb.cpp lib.cpp

g++ -g -o a main.cpp liba.so lib.so
g++ -g -o b main.cpp liba.a lib.so
g++ -g -o b1 main.cpp lib.so liba.a
g++ -g -o c main.cpp liba.a lib.a
g++ -g -o d main.cpp liba.so lib.a
g++ -g -o e main.cpp liba.a
g++ -g -o f main.cpp liba.so
