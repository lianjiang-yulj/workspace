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
    conda update conda -y
    conda update -c defaults conda -y
    conda update anaconda -y
    conda update anaconda -c defaults -y
    conda update anaconda-navigator -y
    conda update anaconda-navigator -c defaults -y


echo "# 更新/opt/anaconda3"
    conda update --prefix /opt/anaconda3 conda -c defaults -y
    conda update --prefix /opt/anaconda3 anaconda -c defaults -y
    conda update --prefix /opt/anaconda3 anaconda-navigator -c defaults -y
    conda update --prefix /opt/anaconda3 --update-all -c defaults -y

echo "# 然后更新每个环境的包，这里面每个环境并没有安装conda，所以更新无效，可以换成想更新的包"
    conda update -n base -c defaults conda -y
    conda update -n sas -c defaults conda -y
    conda update -n pythonProject -c defaults conda -y
    conda update -n py38 -c defaults conda -y
    conda update -n zipline -c defaults conda -y
    conda update -n quantitative -c defaults conda -y
    conda update -n stock -c defaults conda -y

echo "#更新每一个环境的所有包"
    conda update --all -y
    conda update -n py38 -c defaults --update-all -y
    conda update -n pythonProject -c defaults --update-all -y
    conda update -n zipline -c defaults --update-all -y
    conda update -n base -c defaults --update-all -y
    conda update -n sas -c defaults --update-all -y
    conda update -n quantitative -c defaults --update-all -y
    conda update -n stock -c defaults --update-all -y
fi
