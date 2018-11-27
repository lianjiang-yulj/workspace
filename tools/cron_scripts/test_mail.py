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
you='lianjiang.yulj@alibaba-inc.com'

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
        print s
