#!/bin/bash

link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig gdbinit profile gitignore bashrc vimrc zshrc zshrc.pre-oh-my-zsh zprofile

do
if [ -f $dir/mac/$f ]; then
	cmd="ln -s $dir/mac/$f $HOME/.$f"
	echo $cmd

	eval $cmd
fi

done

#ln -s $HOME/workspace.github/tools/vim/vim $HOME/.vim -f
