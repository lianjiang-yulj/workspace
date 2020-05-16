#!/bin/bash

link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in vimrc vim

do
if [ -e $dir/$f ]; then
	#rmcmd="rm $HOME/.$f"
	#echo $rmcmd
	
	#eval $rmcmd
	cmd="ln -s $dir/$f $HOME/.$f -f"
	echo $cmd

	eval $cmd
	fi
done

#ln -s $HOME/workspace.github/bash_env/vim/vim $HOME/.vim -f
