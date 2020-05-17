#!/bin/sh
# lianjiang.yulj

app_name="none";
group_name="none";
host_name="none";


# 4. 登录
login_host()
{
    ssh $host_name
}

# 3. 选择机器
select_and_get_host_name()
{
    host_list=`armory -leg $group_name`
    host_array=(${host_list// /})
    host_len=${#host_array[@]}
    # 输出host 列表
    echo
    echo "机器列表："
    for (( j = 0; j < $host_len; j++ )); do
        echo $j". "${host_array[j]}
    done
    #3. 登录机器
    echo
    echo "请输入编号选择机器；输入b返回上一步；输入q退出："
    read host_index

    if [[ "$host_index" == 'q' ]]; then
        exit;
    elif [[ "$host_index" == 'b' ]]; then
        select_and_get_group_name
    else
        if [ $host_index -lt $host_len ]; then
            host_name=${host_array[host_index]}
            login_host
        else
            echo -e "\nwrong host index($host_index)\n"
        fi
    fi
}

# 2. 选择分组
select_and_get_group_name()
{
    group_name_str=`armory --appgroup $app_name --fields nodegroup_name -1`
    group_name_array=(${group_name_str//,/ })
    group_len=${#group_name_array[@]}

    echo "应用分组列表："
    for (( j = 0; j < ${#group_name_array[@]}; j++ )); do
        echo $j". "${group_name_array[j]}
    done

    # 2. 根据应用分组名获取其中一台机器地址
    echo
    echo "请输入编号选择分组；输入b返回上一步；输入q退出："
    read group_index

    if [[ "$group_index" == 'q' ]]; then
        exit;
    elif [[ "$group_index" == 'b' ]]; then
        select_and_get_app_name;
    else
        if [ $group_index -lt $group_len ]; then
            group_name=${group_name_array[group_index]}
            select_and_get_host_name;
        else
            echo -e "\nwrong group index($group_index)\n"
        fi
    fi
}

# 1. 选择应用
select_and_get_app_name(){
    app_name_array=(
    imatch2-dms
    imatch2-rt
    imatch2-sal
    imatch2.build
    imatch2.cf.click
    imatch2.dump
    imatch2.simon
    imatch2.web
    p4p-gateway
    riddle
    imatch-dsp
    p4p-sal4cmserver
    p4p-ops
    )
    length=${#app_name_array[@]}

    # 1. 根据应用名获取应用分组列表
    echo "应用列表："
    for (( i = 0; i < $length; i++ )); do
        echo $i". "${app_name_array[i]}
    done

    echo
    echo "请输入编号选择应用；输入q退出："
    read index
    if [[ "$index" == 'q' ]]; then
        exit;
    else
        if [ $index -lt $length ]; then
            app_name=${app_name_array[index]}
            select_and_get_group_name;
        else
            echo -e "\nwrong app index($index)\n"
            exit 1
        fi
    fi
}

#select_and_get_app_name;

select_machine() {
    machines=(
        login1.su18.alibaba.org
        login1.eu13.alibaba.org
        login1.et2.alibaba.org
        login1.eu13.cloud.alibaba.org
        11.165.228.116
        11.165.228.117
        11.164.101.208 # my dev
        11.164.101.225 # group dev
        11.164.101.228
        11.229.113.196 # online gpu virtual machine
        11.179.217.65  # online cpu physical machine
    )

    machines_info=(
        jump
        jump
        jump
        jump
        dev.private
        dev.private
        dev.private
        dev.share
        dev.private
        online_gpu_virtual_machine
        online_cpu_physical_machine_admin_jump
    )

    length=${#machines[@]}

    # 1. 根据应用名获取应用分组列表
    echo "machine list："
    for (( i = 0; i < $length; i++ )); do
        echo $i".   "${machines[i]}" #------>>>>> "${machines_info[i]}
    done

    echo
    echo "请输入编号选择应用；输入q|quit退出："
    read index
    if [ "$index" -ge 0 ] 2>/dev/null ;then
	if [ $index -lt $length ]; then
            host_name=${machines[index]}
            login_host;
        else
            echo "\nwrong machine index($index)\n"
            exit 1
        fi
    elif [[ "$index" == 'q' || "$index" == 'quit' ]]; then
        exit 1; 
    else
        exit 1;
    fi
}

select_machine

