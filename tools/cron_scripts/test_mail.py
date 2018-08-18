#!/usr/bin/python
# -*- coding: utf-8 -*- 
# Import smtplib for the actual sending function
import smtplib,os

# Import the email modules we'll need
from email.mime.text import MIMEText

# Open a plain text file for reading.  For this example, assume that
# the text file contains only ASCII characters.
# Create a text/plain message
#msg = MIMEText("prefetch1.wtw.cm8")

# me == the sender's email address
# you == the recipient's email address
me='lianjiang.yulj@prefetch1.wtw.cm8.tbsite.net'
#you='lianjiang.yulj@taobao.com'
#you='alimama.wtw-algo@list.alibaba-inc.com'
you='lianjiang.yulj@taobao.com'

# Send the message via our own SMTP server, but don't include the
# envelope header.
custtype={'1':'B2C','5':'Travel','9':'Game','12':'O2O','13':'Edu'}

def format(rec):
    rec=rec.strip()
    array=rec.split('\001')
    ret=custtype[array[0]] + ('(%s)' % array[0]) + '    ' + array[1] + '            '
    if array[1] is '0':
        pass
    else:
        for i in range(int(array[1])):
            ad=array[i+2].split('\002')
            info=','.join(ad)
            ret+=' | '+info

    return ret + '\n'


if __name__ == '__main__':
    path=os.path.dirname(os.path.realpath(__file__))
    f=open(path+'/tair_data','r')
    s=open(path+'/tair_status','r')
    status={}
    for l in s.readlines():
        l=l.strip()
        array=l.split('\001')
        status[array[0]]=array[1];

    my_msg = ''
    all_data = '1. 所有行业在线状态信息\n' + '行业名称  Tair中的广告数  每个广告信息（adgroupTagId, custType, adgroupId, cpm, ppc, ctr）\n'
    #all_data=all_data.decode('utf-8')
    print all_data
    print status
    for l in f.readlines():
        all_data += format(l)
        l=l.strip()
        array=l.split('\001')
        key=array[0]
        value=array[1]
        print array
        if value != '0':
            value = '1'
        if value != status[key]:
            status[key]=value
            my_msg += "行业 [" + custtype[key] + "] 在线状态变更，"
            if value is '1':
                my_msg += "由[下线]变为[上线]\n"
            else:
                my_msg += "由[上线]变为[下线]\n"


    f.close()
    s.close()
    my_msg = 'test'
    if my_msg:	
        print my_msg
        #my_msg = all_data.encode('utf-8')
        msg = MIMEText(my_msg,'plain','utf-8')
        msg['Subject'] = 'The changed status of prefetch tair : %s' % 'prefetch1.wtw.cm8'
        msg['From'] = me
        msg['To'] = you
        s = smtplib.SMTP('localhost')
        s.sendmail(me, you.split(','), msg.as_string())
        s.quit()
