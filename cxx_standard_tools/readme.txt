gdb:
https://www.gnu.org/software/gdb/download/
https://mirrors.ustc.edu.cn/gnu/gdb
compile: http://www.linuxfromscratch.org/blfs/view/svn/general/gdb.html

mv gdb-8.1.1 gdb-8.1.1-source
# add option: --with-python, you can gdb the binary and print stl, support stl automatic

"
#export LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBRARY_PATH
#export CPLUS_INCLUDE_PATH=$GCC_HOME/include/c++/8.1.0:$GCC_HOME/include/c++/8.1.0/x86_64-pc-linux-gnu/:$CPLUS_INCLUDE_PATH
#export C_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$C_INCLUDE_PATH
#export OBJCPLUS_INCLUDE_PATH=$LIBRARY_PATH:$CPLUS_INCLUDE_PATH:$OBJCPLUS_INCLUDE_PATH
#export OBJC_INCLUDE_PATH=$OBJCPLUS_INCLUDE_PATH:$OBJC_INCLUDE_PATH
"
cp -r gcc-build-libs/{mpfr, mpc, isl, gmp} gcc-8.1.1-source/
ln -s mpfr-xxx mpfr, .....
./configure --prefix=$HOME/workspace.github/cxx_standard_tools/gdb-8.1.1/usr --with-system-readline --with-python
make -j8
make -C gdb install

PS: install readline(https://tiswww.case.edu/php/chet/readline/rltop.html, http://ftp.gnu.org/gnu/readline/readline-7.0.tar.gz), texinfo(http://ftp.gnu.org/gnu/texinfo/texinfo-6.5.tar.xz) before
sudo yum install zlib-devel zlib -bcurrent
sudo yum install openssl openssl-devel openssl-libs
download python-2.7.15.source code
python: ./configure --enable-shared --enable-static --enable-optimizations --enable-unicode=ucs4 --with-zlib --prefix=/usr; it will install in /usr/bin

gcc:
https://gcc.gnu.org/ -> https://gcc.gnu.org/releases.html -> https://gcc.gnu.org/mirrors.html ->https://www.gnu.org/prep/ftp.html -> https://mirrors.ustc.edu.cn/gnu/
https://mirrors.ustc.edu.cn/gnu/gcc/
https://gcc.gnu.org/wiki/InstallingGCC


method1:
tar xzf gcc-8.1.0.tar.xz
cd gcc-8.1.0
./contrib/download_prerequisites
ps: mpfr, mpc, isl, gmp,  put them into the dir : objdir    isl:http://isl.gforge.inria.fr/
cd ..
mkdir objdir
cd objdir
$PWD/../configure --prefix=$HOME/gcc-8.1.0 --enable-languages=c,c++,go --disable-multilib
make
make install

method2:
optional:
mpfr mpc isl gmp, you can download them from https://ftp.gnu.org/gnu//gmp/

tar -xf gcc-8.2.0.tar.xz
mv gcc-8.2.0 gcc-8.2.0-source
cd gcc-8.2.0-source, ln -s ../gcc-build-libs .
cd ../
see /contrib/download_prerequisites, make sure the libs version and modify the prerequisites.md5 and prerequisites.sha512 if the libs version change
**********NOTE:isl-0.20.tar.xz is not ok, this version will make compile errors!!!!!!!!!!!!!!
modify ${fetch} code block ,not to download the libs in ./contrib/download_prerequisites
modify the libs version from gcc-build-libs in ./contrib/download_prerequisites
cd gcc-8.2.0-source
cp gcc-build-libs/prerequisites.sha512 contrib/
./contrib/download_prerequisites --directory=/home/lianjiang.yulj/workspace.github/cxx_standard_tools/gcc-8.2.0-source/gcc-build-libs/ --sha512

cd gcc-8.2.0-source
mkdir objdir
cd objdir
$PWD/../configure --prefix=$HOME/workspace.github/cxx_standard_tools/gcc-8.2.0 --enable-languages=c,c++,go --disable-multilib

modify bash_profile
"
#export LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBRARY_PATH
#export CPLUS_INCLUDE_PATH=$GCC_HOME/include/c++/8.1.0:$GCC_HOME/include/c++/8.1.0/x86_64-pc-linux-gnu/:$CPLUS_INCLUDE_PATH
#export C_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$C_INCLUDE_PATH
#export OBJCPLUS_INCLUDE_PATH=$LIBRARY_PATH:$CPLUS_INCLUDE_PATH:$OBJCPLUS_INCLUDE_PATH
#export OBJC_INCLUDE_PATH=$OBJCPLUS_INCLUDE_PATH:$OBJC_INCLUDE_PATH
"

make -j16
make check
make install

""
If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the `-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the `LD_RUN_PATH' environment variable
     during linking
   - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to `/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.

""


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
