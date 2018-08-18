
export APP_ROOT=%APP_ROOT%
export RESYS_HOST=%RESYS_HOST%
export RESYS_PORT=%RESYS_PORT%
export RESCLI_ROOT=%RESCLI_ROOT%

#export RESCLI_ROOT=/home/resys/recli
#export RESCLI_BIN_PATH=$RESCLI_ROOT/bin
#export RESCLI_SCRIPT_PATH=$RESCLI_ROOT/script

. $RESCLI_ROOT/python/bin/activate

. $RESCLI_ROOT/script/sys/update_script.sh
