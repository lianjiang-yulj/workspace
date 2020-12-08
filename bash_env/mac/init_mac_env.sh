#!/bin/sh

#please do some pre steps of init_mac_env.txt
#after that, you can do these steps


#1
# do it in your git project, not global config

#git config  core.filemode false
#git config user.name 'lianjiang.yulj'
#git config user.email 'lianjiang.yulj@alibaba-inc.com'
#git-m

#后面一个指令使用了 git-m 命令自动修改 log 信息,获得 git-m 方法：
#remote:   Mac OS X :
#remote:     curl  http://openbase.cn-hangzhou.oss.aliyun-inc.com/git-m -o /usr/local/bin/git-m
#remote:     chmod 775  /usr/local/bin/git-m

#2
#https://blog.bbzhh.com/index.php/archives/141.html
brew install coreutils
#for mac: brew install coreutils, add this in bash_profile
#alias readlink=/usr/local/bin/greadlink

#3
#https://zhuanlan.zhihu.com/p/98384704
#https://www.cnblogs.com/kevingrace/p/6496899.html
brew install tmux
brew install wget
brew install git-lfs
brew install autojump
brew install bat
brew install neovim
brew install htop
brew install maven
brew install coreutils
brew install tree
brew install npm
#brew install node # 安装npm会安装node依赖
brew install yarn
brew install vim


# for install neovim
/opt/anaconda3/bin/pip3.8 install --upgrade pip
/opt/anaconda3/bin/python3.8  -m pip install pynvim

brew install bash #update bash

brew install gcc gdb

conda install seaborn
conda install plotly
conda install pandas
conda install pandocfilters
conda install pandoc
conda install cvxopt
/opt/anaconda3/bin/pip3 install uqer --upgrade

/opt/anaconda3/bin/pip3 install flake8 yapf
#conda install flake8 yapf -c anaconda

/opt/anaconda3/envs/sas/bin/pip3.8 install tushare --upgrade


#install jupyter notebook
conda install jupyter jupyter-kite jupyter_client jupyter_console jupyter_core jupyterlab jupyterlab-server jupyterlab_pygments
# jupyter-kite jupyterlab-server 可能不再conda 的channel里面，安装报错的时候去掉这2个包即可，其中jupyterlab-server会被自动依赖进来

pip3 install pip --upgrade
pip3 install jupyter-kite
jupyter labextension install "@kiteco/jupyterlab-kite"

#https://help.kite.com/article/143-how-to-install-the-jupyterlab-plugin
#pip install --upgrade jupyter-kite
#jupyter labextension update "@kiteco/jupyterlab-kite"

conda create -n sas --clone base

/Library/Frameworks/Python.framework/Versions/3.9/bin/pip3 install pandas numpy




# conda env list 
# base = root, stock = base, clone from base # root环境不可随意变更
# py38 python3.8.5
# pythonProject3.8.5
# quantitative = sas # for 量化股票分析环境
# quantitative = sas # for 量化股票分析环境
# zipline # python=3.5.6, for zipline

#关于QP的量化三大件：pyFolio，zipline，alphalens
conda create -n sas --clone base
conda activate sas

conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes


conda install -c conda-forge alphalens
#pip install alphalens


conda install -c quantopian pyfolio
#pip install pyfolio


# 注意，zipline 依赖python3.5.6, PYTHON < 3.6 VERSION
conda create -n zipline python=3.5
conda install -c quantopian zipline
#pip install zipline

/opt/anaconda3/envs/zipline/bin/pip install backtrader --upgrade
