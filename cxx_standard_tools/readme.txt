gdb:
https://mirrors.ustc.edu.cn/gnu/gdb
http://www.linuxfromscratch.org/blfs/view/svn/general/gdb.html

# add option: --with-python, you can gdb the binary and print stl, support stl automatic
./configure --prefix=$HOME/gdb-xxx/usr --with-system-readline --without-guile --with-python
make
make -C gdb install

PS: install readline, textinfo before
sudo yum install zlib-devel zlib -bcurrent
sudo yum install openssl openssl-devel openssl-libs
download python-2.7.15.source code
python: ./configure --enable-shared --enable-static --enable-optimizations --enable-unicode=ucs4 --with-zlib

gcc:
https://mirrors.ustc.edu.cn/gnu/gcc/
https://gcc.gnu.org/wiki/InstallingGCC


tar xzf gcc-8.1.0.tar.gz
cd gcc-8.1.0
./contrib/download_prerequisites
ps: mpfr, mpc, isl, gmp,  put them into the dir : objdir    isl:http://isl.gforge.inria.fr/
cd ..
mkdir objdir
cd objdir
$PWD/../configure --prefix=$HOME/gcc-8.1.0 --enable-languages=c,c++,go --disable-multilib
make
make install



##some problem
1. gdb debuginfo
Q: "Missing separate debuginfos, use: debuginfo-install glibc-2.17-222.alios7.1.x86_64"

A: you can run cmd: "debuginfo-install glibc-2.17-222.alios7.1.x86_64", but you can ignore this warnning, it doesn't matter 
 http://debuginfo.centos.org/7/x86_64/

2. gdb aato-load safe-path

Q:
Starting program: /home/lianjiang.yulj/workspace.github/cxx_standard_tools/a
warning: File "/home/lianjiang.yulj/workspace.github/cxx_standard_tools/gcc-8.1.0/lib64/libstdc++.so.6.0.25-gdb.py" auto-loading has been declined by your `auto-load safe-path' set to "$debugdir:$datadir/auto-load".
To enable execution of this file add
	add-auto-load-safe-path /home/lianjiang.yulj/workspace.github/cxx_standard_tools/gcc-8.1.0/lib64/libstdc++.so.6.0.25-gdb.py
line to your configuration file "/home/lianjiang.yulj/.gdbinit".
To completely disable this security protection add
	set auto-load safe-path /
line to your configuration file "/home/lianjiang.yulj/.gdbinit".
For more information about this security protection see the
"Auto-loading safe path" section in the GDB manual.  E.g., run from the shell:
	info "(gdb)Auto-loading safe path"




A: see /home/lianjiang.yulj/workspace.github/bash_env/gdbinit.gdb-8.1.0
