#!/bin/bash


#for zsh
sudo yum install htop zsh ncurses  ncurses-libs ncurses-devel -bcurrent

sudo yum install autojump-zsh autojump -bcurrent
if [ $? -ne 0 ]; then
    sudo yum install http://yum.corp.taobao.com/opsx-inc/7/noarch/autojump-fish/autojump-fish-22.3.0-3.alios7.noarch.rpm http://yum.corp.taobao.com/opsx-inc/7/noarch/autojump/autojump-22.3.0-3.alios7.noarch.rpm http://yum.corp.taobao.com/opsx-inc/7/noarch/autojump-zsh/autojump-zsh-22.3.0-3.alios7.noarch.rpm
fi

sudo yum remove ali-jdk --nodeps
sudo yum install ajdk -bcurrent
sudo yum install postfix -bcurrent

sudo yum install git git-config -bcurrent

sudo yum install zlib-devel zlib python-devel python python-libs glibc-debuginfo glibc-debuginfo-common -bcurrent #python-2.7.5 / python-2.7.15

sudo yum install openssl openssl-devel openssl-libs automake -bcurrent

sudo yum install tmux -bcurrent

sudo yum install clang llvm make cmake -bcurrent
sudo yum install perl-ExtUtils-CBuilder perl-ExtUtils-MakeMaker -bcurrent

#for linux nvim
sudo yum install fuse -bcurrent

#for gcc 921
#see http://rpm.corp.taobao.com/find.php?q=alicpp-gcc&t=&d=0&os=
sudo yum install  alicpp-gcc921-gcc-debuginfo alicpp-gcc921-gcc alicpp-gcc921-gdb-debuginfo alicpp-gcc921-gdb  -bcurrent

#install them after install linuxbrew 
~/.linuxbrew/bin/brew install bat htop

#for gdb 9.2
~/.linuxbrew/bin/brew install gdb gcc
