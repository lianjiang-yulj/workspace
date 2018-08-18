#!/usr/local/bin/python2.6
# -*- coding: utf-8 -*- 
# Import smtplib for the actual sending function

"""
Usage: 
    send_mail.py --html_mail=<html_mail>

Arguments:
    --html_mail=<html_mail>           #mail content
"""

import smtplib,os

# Import the email modules we'll need
from email.mime.text import MIMEText
from email.Header import Header

# Open a plain text file for reading.  For this example, assume that
# the text file contains only ASCII characters.
# Create a text/plain message
#msg = MIMEText("prefetch1.wtw.cm8")

# me == the sender's email address
# you == the recipient's email address
#me='QiJian@odps1.test.cm3'
me='QiJian@odps1.test.cm3.tbsite.net'
#me='lianjiang.yulj@taobao.com'
#you='lianjiang.yulj@taobao.com'
#you='alimama.wtw-algo@list.alibaba-inc.com'
#you='lianjiang.yulj@taobao.com, ali-p4p@list.alibaba-inc.com'
#you='lianjiang.yulj@taobao.com, alimama-wxb-engine@list.alibaba-inc.com, app-fighting@list.alibaba-inc.com, fangzikai.fzk@alibaba-inc.com, wangke.wk@alibaba-inc.com, wb-yanshuyuan@alibaba-inc.com, zishi.jaf@taobao.com, haidong.wanghd@alibaba-inc.com, xiaonan.mengxn@alibaba-inc.com, yuexia.jia@alibaba-inc.com, saiya.hso@taobao.com, longran.wh@alibaba-inc.com'
you='lianjiang.yulj@taobao.com, lianjiang.yulj@alibaba-inc.com'

# Send the message via our own SMTP server, but don't include the
# envelope header.

#host_uri='http://m.proxy.taobao.org/report'
host_uri='http://m.proxy.taobao.org/'
#http://m.proxy.taobao.org/click_cheat-20141122.html

import sys
from docopt import docopt

def SendMail(args):
    mail_msg = 'test'
    msg = MIMEText(mail_msg,'html','utf-8')
    msg['Subject'] = Header('Test', 'utf-8')
    msg['From'] = me
    msg['To'] = you
    s = smtplib.SMTP('localhost')
    s.sendmail(me, you.split(','), msg.as_string())
    s.quit()

if __name__ == '__main__':
    args = docopt(__doc__, help=True)
    SendMail(args)
