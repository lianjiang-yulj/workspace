#!/bin/sh

#sudo docker run -itd -v /etc/yum.repos.d:/etc/yum.repos.d -v /home/lianjiang.yulj/.ssh:/root/.ssh  -v /home/lianjiang.yulj/workspace:/home/lianjiang.yulj/workspace -v /etc/passwd:/etc/passwd --net=host  --cpuset-cpus="0-63"  reg.docker.alibaba-inc.com/isearch/ha3:3.3.1-rc3 /bin/bash

#sudo docker exec -it 91871593a2a7 /bin/bash
#sudo docker run -itd -v /etc/yum.repos.d:/etc/yum.repos.d -v /home/lianjiang.yulj/.ssh:/root/.ssh  -v /home/lianjiang.yulj/workspace:/home/lianjiang.yulj/workspace -v /etc/passwd:/etc/passwd --net=host  --cpuset-cpus="0-63"  reg.docker.alibaba-inc.com/isearch/ha3:3.3.1-rc3 /bin/bash
image=reg.docker.alibaba-inc.com/hippo/hippo_alios7u2_base:1.5
container_name=lianjiang.yulj
work_dir=/home/lianjiang.yulj
#sudo docker run --net=host -v /home/lianjiang.yulj/workspace/source/:/home/lianjiang.yulj/workspace/source/ -it reg.docker.alibaba-inc.com/hippo/hippo_alios7u2_base:1.5 /bin/bash
sudo docker stop $container_name
sudo docker rm $container_name
sudo docker create  --net=host -w $work_dir --name $container_name -v /home/lianjiang.yulj/workspace/source/:/home/lianjiang.yulj/workspace/source/ -it $image /bin/bash
sudo docker ps -a
#container_id=`sudo docker ps -a | grep Up | awk '{print $1}'`
sudo docker start $container_name
sudo docker exec -it $container_name /bin/bash
sudo useradd lianjiang.yulj
sudo passwd lianjiang.yulj
chmod +w /etc/sudoers
vi /etc/sudoers
add lianjiang.yulj after root
chmod -w /etc/sudoers
#sudo cp .bash_profile .bashrc .gitconfig workspace/source/ 通过挂在目录传递某些文件
cp .gitconfig /root or /home/lianjiang.yulj/  #从宿主机账户拷贝到容器账户
exit #推出容器
sudo chown lianjiang.yulj:users /home/lianjiang.yulj/workspace -R
#sudo docker exec -u lianjiang.yulj -it $container_name /bin/bash
sudo docker exec -it $container_name /bin/bash
#sudo chown lianjiang.yulj:users /home/lianjiang.yulj -R
sudo yum clean all
sudo yum install git -bcurrent
