#!/bin/bash

link_path=`greadlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in vimrc vim

do
if [ -e $dir/$f ]; then
	rmcmd="rm $HOME/.$f"
	echo $rmcmd
	
	eval $rmcmd
	cmd="ln -s $dir/$f $HOME/.$f"
	echo $cmd

	eval $cmd
	fi
done

