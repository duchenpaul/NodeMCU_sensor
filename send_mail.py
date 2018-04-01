#! /usr/bin/env python3  
# -*- coding: utf-8 -*-   
import sys 
# reload(sys) 
# sys.setdefaultencoding('gb2312')

import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
import getopt
from os.path import basename

#Usage: python send_an_email_with_attachment.py subject content (-f FILEPATH -r "RECIPIENT_1@gmail.com,RECIPIENT_2@gmail.com")

fromaddr = "qq859755014@126.com"
# fromaddr = "RASPBERRY_PI<qq859755014@126.com>"
toaddr = "qq859755014@126.com"
#toaddr = "724330356@qq.com"

msg = MIMEMultipart()
 
# msg["Accept-Language"]="zh-CN"
# msg["Accept-Charset"]="ISO-8859-1,utf-8"

msg['Subject'] = sys.argv[1]
# body = ''.join(sys.argv[2]).encode('utf-8')
body = ''.join(sys.argv[2])
body = body.replace('\n', '<br />')

#msg['Subject'] = "SUBJECT OF THE EMAIL"
#body = "TEXT YOU WANT TO SEND"

msg.attach(MIMEText('<html><body>'+ body +'</body></html>','html','utf-8'))
#msg.attach(MIMEText(body, 'plain'))

opts, args = getopt.getopt(sys.argv[3:], "f:r:")
for op, VALUE in opts:
	if op == '-f':
		FILE_PATH = VALUE
		filename =  basename(FILE_PATH)
		attachment = open(FILE_PATH, "rb")
		
		part = MIMEBase('application', 'octet-stream')
		part.set_payload((attachment).read())
		encoders.encode_base64(part)
		part.add_header('Content-Disposition', "attachment; filename= %s" % filename)
		
		msg.attach(part)
	elif op == '-r':
		toaddr = VALUE


msg['From'] = fromaddr
msg['To'] = toaddr

print(("sending mail to: %s" % toaddr))

try: 
	server = smtplib.SMTP('smtp.126.com')
	server.starttls()
	server.login(fromaddr, "lrvxmouxswxizkgq")
	text = msg.as_string()
	#print(text)
	server.sendmail(fromaddr, toaddr.split(','), text)
	server.quit()
	#print("Mail sent")
except Exception as e:
	print(('Failed to send mail: '+ str(e)))
	sys.exit(1)

