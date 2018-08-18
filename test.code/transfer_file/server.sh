#!/bin/sh
#./server_noqueue/server $2 $1 0 8192
./server_epoll/server $2 $1 2
