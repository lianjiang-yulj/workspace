#!/bin/sh

link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
cmd="ln -s $dir/config $HOME/.config"

eval $cmd
