#!/bin/sh

#~/clang+llvm-9.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/clang++ ../c++17.cc -std=c++17 -g -ggdb -o c++17.clang  -L/apsara/alicpp/built/gcc-9.2.1/gcc-9.2.1/lib64/ -v
#~/clang+llvm-9.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/clang++ ../c++17.cc -std=c++17 -g -ggdb -o c++17.clang  -L/apsara/alicpp/built/gcc-9.2.1/gcc-9.2.1/lib64/ -fno-limit-debug-info

clang++ ../c++17.cc -std=c++2a -g -ggdb -o c++17.clang  -L/apsara/alicpp/built/gcc-9.2.1/gcc-9.2.1/lib64/ -fno-limit-debug-info
