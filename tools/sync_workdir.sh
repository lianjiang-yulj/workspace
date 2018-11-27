#!/bin/sh
# step1
#sudo yum install python python-devel python-libs zip zip-devel openssl openssl-devel openssl-libs git -bcurrent
# step2
# sh sync_workdir.sh
# step3
# install git-lfs from workspace.github/tools/git-lfs
# step4
# install readline textinfo from workspace.github/cxx_standard_tools/
# step5
# sudo yum install ncurses  ncurses-libs ncurses-devel -bcurrent


if test $# -eq 0 ; then
    echo -e "Usage:\n   sh $0 11.165.228.116 11.165.228.117 11.164.101.225 11.164.101.228 11.164.101.208\n   split by blank space\n"
    exit 1;
fi

ms="$@"
read -ra ADDR <<< "$ms";

path=""
for dir in .cache .conda .keras .linuxbrew .bash_profile .gitconfig .gdbinit .profile .gitignore .bashrc .vimrc .vim
do
    if test -z "$path"; then
        path=$HOME/$dir
    else
        path=$HOME/$dir" "$path
    fi
done

for dir in `ls $HOME/`
do
    if test -z "$path"; then
        path=$HOME/$dir
    else
        path=$HOME/$dir" "$path
    fi
done

if test -z "$path"; then
    echo "rsync dir is empty, exit!"
    exit 1;
fi

user=`whoami`

for m in "${ADDR[@]}";
do
    msg="NOTICE: \n begin to rsync \"$path\" to machine: \"$m\"\n"
    echo -e "$msg"
    read -t 10 -p "---------- make sure?----------- [y/n] " answer
    answer=y
    case $answer in
        Y|y)
            cmd="/usr/bin/rsync -arv $path $user@$m:$HOME/ --delete-after"
            echo $cmd
            #/usr/bin/rsync -arv $path $user@$m:$HOME/ --delete-after
            eval "$cmd"
            #continue;;
            ;;
        N|n)
            echo "";;
        *)
            echo "";;
    esac

done
