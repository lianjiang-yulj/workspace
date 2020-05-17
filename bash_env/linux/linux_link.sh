#!/bin/sh

link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig gdbinit profile bashrc zshrc zshrc.pre-oh-my-zsh

do
if [ -f $dir/$f ]; then
	#rmcmd="rm $HOME/.$f"
	#echo $rmcmd
	
	#eval $rmcmd
	cmd="ln -s $dir/$f $HOME/.$f -f"
	echo $cmd

	eval $cmd
	fi
done

