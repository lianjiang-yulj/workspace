#!/bin/zsh

#Usage: zsh update_local_brew_conda.sh 0/1/2

#update python3.8.5
#conda install python=3.8.5 -c anaconda
#

echo $1

if [ "x$1" = "x0" ]; then
    echo $PATH
    brew upgrade
    brew update
elif [ "x$1" = "x1" ]; then
    . ~/.zshrc
    omz update
else

    conda init zsh

    conda deactivate

echo "# 先更新 conda，然后更新anaconda"
    conda update conda -y --all
    #conda update -c defaults conda -y --all
    conda update anaconda -y --all
    #conda update anaconda -c defaults -y --all
    conda update anaconda-navigator -y --all
    #conda update anaconda-navigator -c defaults -y --all


echo "# 更新/opt/anaconda3"
    #conda update --prefix /opt/anaconda3 conda -c defaults -y --all
    #conda update --prefix /opt/anaconda3 anaconda -c defaults -y --all
    #conda update --prefix /opt/anaconda3 anaconda-navigator -c defaults -y --all
    #conda update --prefix /opt/anaconda3 --update-all -c defaults -y --all
    conda update --prefix /opt/anaconda3 conda -y --all
    conda update --prefix /opt/anaconda3 anaconda -y --all
    conda update --prefix /opt/anaconda3 anaconda-navigator -y --all
    conda update --prefix /opt/anaconda3 --update-all -y --all

echo "# 然后更新每个环境的包，这里面每个环境并没有安装conda，所以更新无效，可以换成想更新的包"
    #conda update -n base -c defaults conda -y --all

echo "#更新每一个环境的所有包"
    conda update --all -y
    #conda update -n base -c defaults --update-all -y
    conda update -n base --update-all -y
    #conda update -n yundong -c defaults --update-all -y
    conda update -n yundong --update-all -y
fi
