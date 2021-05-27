#!/bin/zsh

#Usage: zsh update_local_brew_conda.sh 0/1/2

#update python3.8.5
#conda install python=3.8.5 -c anaconda
#

echo $1

if [ "x$1" = "xbrew" ]; then
    echo $PATH
    brew upgrade
    brew update
elif [ "x$1" = "xomz" ]; then
    . ~/.zshrc
    omz update
elif [ "x$1" != "xconda" ]; then
    echo "not support cmd: $1, only support cmd: brew/omz/conda"
    exit 1;

else

    conda init zsh

    conda deactivate

echo "# 先更新 conda，然后更新anaconda"
    conda update conda --all
    #conda update -c defaults conda --all
    conda update anaconda --all
    #conda update anaconda -c defaults --all
    conda update anaconda-navigator --all
    #conda update anaconda-navigator -c defaults --all


echo "# 更新/opt/anaconda3"
    #conda update --prefix /opt/anaconda3 conda -c defaults --all
    #conda update --prefix /opt/anaconda3 anaconda -c defaults --all
    #conda update --prefix /opt/anaconda3 anaconda-navigator -c defaults --all
    #conda update --prefix /opt/anaconda3 --update-all -c defaults --all
    conda update --prefix /opt/anaconda3 conda --all
    conda update --prefix /opt/anaconda3 anaconda --all
    conda update --prefix /opt/anaconda3 anaconda-navigator --all
    conda update --prefix /opt/anaconda3 --update-all --all

echo "# 然后更新每个环境的包，这里面每个环境并没有安装conda，所以更新无效，可以换成想更新的包"
    #conda update -n base -c defaults conda --all

echo "#更新每一个环境的所有包"
    conda update --all
    #conda update -n base -c defaults --update-all
    conda update -n base --update-all
    #conda update -n yundong -c defaults --update-all
    conda update -n yundong --update-all
    conda update -n yundongblock --update-all
fi
