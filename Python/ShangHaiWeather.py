#!/usr/bin/python3
# -- coding  utf-8 --

import urllib
import urllib2
import sys
reload(sys)
sys.setdefaultencoding('utf8')
 
import requests
from bs4 import BeautifulSoup
 
def weather_get(url):
    weather_url=url
    web_page=urllib.urlopen(weather_url).read() 
    weather_soup=BeautifulSoup(web_page,'html.parser') 
    weather_value = weather_soup.find(id = "weaLiveInfo").get_text() 
    value_list =  weather_value.split("：") 
    # 获得温度，湿度，气压
    temp,humidity,pressure =value_list[1].split("℃")[0],value_list[2].split("%")[0],value_list[4].split("hPa")[0]
    return temp,humidity,pressure
 
weather_url='http://tianqi.2345.com/suzhou/58357.htm'
temp,humidity,pressure = weather_get(weather_url)
print ("温度：%s,湿度：%s,气压：%s"%(temp,humidity,pressure))
