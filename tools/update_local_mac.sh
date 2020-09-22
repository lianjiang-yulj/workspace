#!/bin/sh

brew upgrade

brew update

omz update

conda update conda
conda update anaconda
conda update anaconda-navigator

conda update --all
conda update --prefix /opt/anaconda3 --update-all
conda update --prefix /opt/anaconda3 anaconda
