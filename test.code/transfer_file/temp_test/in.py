#!/usr/bin/env python2.6

import os,sys

if __name__=='__main__':
    f=open('./in.txt', 'w')
    f.write("abcdeft\r\n");
    f.write("hello word\r\n");
    f.write("success\r\n");
    f.close()
