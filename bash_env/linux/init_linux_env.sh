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

#~/.linuxbrew/bin/brew install bat htop
#brew install gdb gcc
