#!/bin/sh

rm -f connection.h connection.cc

ln -s connection$1.h connection.h
ln -s connection$1.cc connection.cc
