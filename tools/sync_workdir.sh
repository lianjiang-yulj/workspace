#!/bin/sh

path=""
for dir in .cache .conda .keras .linuxbrew .bash_profile .gitconfig .gdbinit .profile .gitignore .bashrc
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
fi

for m in 10.97.184.58
do
    echo -e "NOTICE: \n begin to rsync \"$path\" to machine: \"$m\"\n"
    #rsync -arv $path
done
