#!/bin/zsh

#Usage: zsh update_local_brew_conda.sh 0/1/2

#update python3.8.5
#conda install python=3.8.5 -c anaconda
#

echo $1

if [ "x$1" = "x0" ]; then
    brew upgrade
    brew update
elif [ "x$1" = "x1" ]; then
    . ~/.zshrc
    omz update
else

    conda init zsh

    conda deactivate
    conda update -c defaults conda
    conda update -n base -c defaults conda
    conda update conda
    conda update anaconda
    conda update anaconda-navigator

    conda update --all
    conda update --prefix /opt/anaconda3 --update-all
    conda update --prefix /opt/anaconda3 anaconda
fi
