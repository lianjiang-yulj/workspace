#!/usr/bin/env bash
vncserver :1 -geometry 1366x768 -depth 24

[ $? -ne 1 ]|| echo "error"
