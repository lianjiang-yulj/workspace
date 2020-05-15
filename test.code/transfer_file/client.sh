#!/bin/sh
./client_epoll/client -H $1 -p $2 -r 2400 -c 12 -w 0 -o $3
