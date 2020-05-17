#!/bin/sh

link_path=`greadlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in gitignore_global

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

