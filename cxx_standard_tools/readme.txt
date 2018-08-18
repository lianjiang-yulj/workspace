gdb:
https://mirrors.ustc.edu.cn/gnu/gdb
http://www.linuxfromscratch.org/blfs/view/svn/general/gdb.html

# add option: --with-python, you can gdb the binary and print stl, support stl automatic
./configure --prefix=$HOME/gdb-xxx/usr --with-system-readline --without-guile --with-python
make
make -C gdb install

PS: install readline, textinfo before
python: ./configure --enable-shared --enable-static --enable-optimizations --enable-unicode=ucs4

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

