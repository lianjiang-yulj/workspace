link_path=`readlink -f $0`
echo -e "bash $link_path \n"

dir=`dirname $link_path`
for f in bash_profile gitconfig gdbinit profile

do
cmd="ln -s $dir/$f $HOME/.$f"
echo $cmd

eval $cmd
done
