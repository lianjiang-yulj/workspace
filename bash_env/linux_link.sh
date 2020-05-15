link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig gdbinit profile gitignore bashrc vimrc

do
cmd="ln -s $dir/$f $HOME/.$f"
echo $cmd

eval $cmd
done

ln -s $HOME/workspace.github/tools/vim/vim $HOME/.vim -f
