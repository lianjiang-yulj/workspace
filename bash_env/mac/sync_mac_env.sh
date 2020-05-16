#!/bin/sh

link_path=`greadlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig gitignore_global gdbinit profile gitignore bashrc vimrc zshrc zshrc.pre-oh-my-zsh zprofile

do
	if [ -f $dir/$f ]; then
		cmd="cp $HOME/.$f $dir/$f"
		echo $cmd

		eval $cmd
	fi

done

