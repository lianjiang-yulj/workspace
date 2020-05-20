#!/bin/sh

link_path=`greadlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig bashrc zshrc zshrc.pre-oh-my-zsh zprofile

do
	if [ -f $dir/$f ]; then
		cmd="ln -s $dir/$f $HOME/.$f"
		echo $cmd
		if [ -f $HOME/.$f ]; then
			rm_cmd="rm -i $HOME/.$f"
			echo $rm_cmd
			eval $rm_cmd
		fi

		eval $cmd
	fi

done

sh $dir/neovim/config_link.sh

#ln -s $HOME/workspace.github/bash_env/vim/vim $HOME/.vim -f
