#!/bin/bash
# resys client script deploy script

set -e

function install_rescli()
{
    #may be exists bug to check if a install instance exists
    #proc_name=`basename $0`
    #pid=`ps ux | grep $proc_name | grep -v grep | awk '{print $2}'`
    #pid=`ps ux | grep "$0" | grep -v grep | awk '{print $2}'`
    #arr=(${pid//' '/ })
    #num=${#arr[@]}
    #if [ $num -gt 2 ]; then
    #    echo "another install rescli instance is running!"
    #    exit 3
    #fi

    RESCLI_SUCCESS_FLAG='/home/resys/rescli/.SUCCESS'
    rm -f $RESCLI_SUCCESS_FLAG

    #if [ "$(id -u)" != 0 ]; then
    #    echo -e "You must run as root!!!"
    #    exit 1
    #fi

    #############################################################################
    # global var define
    #############################################################################
    RESYS_HOST="resadm.kgb.cm6.tbsite.net"
    RESYS_PORT=9015
    SCRIPT_HOST="resadm.kgb.cm6.tbsite.net"
    SCRIPT_PORT=8081

    RESYS_ROOT="/home/resys"

    RESCLI_ROOT=$RESYS_ROOT/rescli
    RESCLI_CONF=$RESCLI_ROOT/etc
    RESCLI_TEMP=$RESCLI_ROOT/tmp
    RESCLI_LOGS=$RESCLI_ROOT/logs
    RESCLI_PYTHON=$RESCLI_ROOT/python
    RESCLI_SCRIPT=$RESCLI_ROOT/script

    sudo /bin/mkdir -p $RESCLI_ROOT
    sudo /bin/mkdir -p $RESCLI_CONF
    sudo /bin/mkdir -p $RESCLI_TEMP
    sudo /bin/mkdir -p $RESCLI_LOGS
    sudo /bin/mkdir -p $RESCLI_SCRIPT

    sudo /bin/chown -R ads:users "$RESYS_ROOT"

    APP_ROOT="/home/a/search"

    curl "http://$RESYS_HOST:$RESYS_PORT/file/deployment/setenv.sh" -o "$RESCLI_CONF/setenv.sh"

    sed -i -e "s;%RESYS_HOST%;$RESYS_HOST;g"    "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%RESYS_PORT%;$RESYS_PORT;g"    "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%RESYS_ROOT%;$RESYS_ROOT;g"    "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%RESCLI_ROOT%;$RESCLI_ROOT;g"  "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%SCRIPT_ROOT%;$SCRIPT_ROOT;g"  "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%SCRIPT_PORT%;$SCRIPT_PORT;g"  "$RESCLI_CONF/setenv.sh"
    sed -i -e "s;%APP_ROOT%;$APP_ROOT;g"        "$RESCLI_CONF/setenv.sh"

    #############################################################################
    # install required rpm
    #############################################################################
    #rhel=`rpm -q redhat-release --qf "%{v}"`
    #rsync_location=`curl -s 'http://yum.corp.taobao.com/cgi-bin/rpmfind?name=rsync-2.6.8-3.1.x86_64.rpm&rhel='$rhel`
    #if [ -z "$rsync_location" ]; then
    #    echo "can not find rsync-2.6.8-3.1.x86_64.rpm"
    #    exit 1
    #fi

    #rpm -q rsync-2.6.8-3.1.x86_64 > /dev/null 2>&1 || sudo rpm -ivh "$rsync_location"

    # pre-required rpms
    sudo yum -b current -y install python26-virtualenv \
        python26-setuptools.x86_64 git.x86_64 walle.noarch
    #sudo yum -b test -y install t_provider-0.0.2-3.el5 deploy_express-0.8.0-rc_2

    #############################################################################
    # python virtual env
    #############################################################################
    #python env
    virtualenv $RESCLI_PYTHON

    . "$RESCLI_PYTHON/bin/activate"

    PY_BOOTSTRAP="bootstrap.py"
    PY_REQUIRES="pyapps.txt"
    curl "http://$RESYS_HOST:$RESYS_PORT/file/deployment/python/$PY_BOOTSTRAP" -o "$RESCLI_TEMP/$PY_BOOTSTRAP"
    curl "http://$RESYS_HOST:$RESYS_PORT/file/deployment/python/$PY_REQUIRES" -o "$RESCLI_TEMP/$PY_REQUIRES"

    # required package
    chmod +x $RESCLI_TEMP/$PY_BOOTSTRAP
    pushd . > /dev/null
    cd $RESCLI_TEMP
    $RESCLI_TEMP/$PY_BOOTSTRAP
    popd > /dev/null

    # resclient
    RESYS_PYCLIB="resclient-0.1.17-py2.6.egg"
    curl "http://$RESYS_HOST:$RESYS_PORT/file/deployment/python/$RESYS_PYCLIB" -o "$RESCLI_TEMP/$RESYS_PYCLIB"
    easy_install "$RESCLI_TEMP/$RESYS_PYCLIB"

    #############################################################################
    # deploy user script
    #############################################################################
    if [ -d "$RESCLI_SCRIPT" ]; then
        rm -rf "$RESCLI_SCRIPT"
    fi

    pushd . > /dev/null
    cd $RESCLI_ROOT
    git clone "http://$SCRIPT_HOST:$SCRIPT_PORT/script.git" $RESCLI_SCRIPT/
    popd > /dev/null

    sudo /bin/chown -R ads:users "$RESYS_ROOT"

    touch $RESCLI_SUCCESS_FLAG
}

FLOCK='/tmp/.install_rescli.lock'
( #start subprocess
    # Wait for lock on /var/lock/.myscript.exclusivelock (fd 200) for 10 seconds
    flock -x -w 10 200
    if [ "$?" != "0" ]; then
        echo Cannot lock $FLOCK!; 
        exit 1; 
    fi
    echo $$>$FLOCK #for backward lockdir compatibility, notice this command is executed AFTER command bottom  ) 200>/var/lock/.myscript.exclusivelock.
    install_rescli
    if  [ "$?" != "0" ]; then
        echo "install failed";
        exit 1
    fi
    # Do stuff
    # you can properly manage exit codes with multiple command and process algorithm.
    # I suggest throw this all to external procedure than can properly handle exit X commands

) 200>$FLOCK   #exit subprocess

FLOCKEXIT=$?  #save exitcode status
#do some finish commands
if [ $FLOCKEXIT -ne 0 ]; then
    echo "if not do the install action, maybe a install instance is running. please check"
fi
exit $FLOCKEXIT   #return properly exitcode, may be usefull inside external scripts
