#!/bin/sh

#查看编译过程
#g++ -E c++17.cc
g++ -g -ggdb -o c++17.g++ c++17.cc -std=c++2a

#g++ -g -ggdb -o c++17.g++ c++17.cc -std=c++2a, you can set -std=c++2b to see all the c++1x standards


