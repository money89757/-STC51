#!/usr/bin/python3

import requests
from requests import exceptions
from urllib.request import urlopen
from bs4 import BeautifulSoup
import re
from wxpy import *
import schedule
import time
import itchat

itchat.auto_login(hotReload = True, enableCmdQR = 2)

def sendlogmsg(content):
        name= itchat.search_friends("周勇")
        userName = name[0]['UserName']
        print(userName)
        my_group = itchat.send(content,toUserName = userName)


def job():
	resp = urlopen('http://www.weather.com.cn/weather/101020100.shtml')
	soup = BeautifulSoup(resp,'html.parser')
	tagToday = soup.find('p',class_='tem')
	try:
		temperatureHigh = tagToday.span.string
	except AttributeError as e:
		temperatureHigh = tagToday.find_next('p',class_="tem").span.string

	temperatureLow = tagToday.i.string
	weather = soup.find('p',class_="wea").string
	contents ='上海天气'+ '\n' + '最高温度:' + temperatureHigh + '\n' + '最低温度:' + temperatureLow + '\n' +  '天气:' + weather 

	sendlogmsg(contents)

schedule.every().day.at("15:20").do(job)
while True:
    schedule.run_pending()
    time.sleep(1)

bot.join()
