#!/bin/zsh

#Usage: zsh update_local_brew_conda.sh

#update python3.8.5
#conda install python=3.8.5 -c anaconda
#
brew upgrade
brew update

omz update

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
