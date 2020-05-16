#!/bin/bash

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
