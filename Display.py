#Imports Libraries
import datetime
import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

import requests
import json 
import subprocess

# Raspberry Pi pin configuration:
RST = None     
# Note the following are only used with SPI:
DC = 23
SPI_PORT = 0
SPI_DEVICE = 0


# 128x32 display with hardware I2C:
disp = Adafruit_SSD1306.SSD1306_128_32(rst=RST)

# Initialize library.
disp.begin()

# Clear display.
disp.clear()
disp.display()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
width = disp.width
height = disp.height
image = Image.new('1', (width, height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)


padding = -1
top = padding
bottom = height-padding
x = 5

timeinprogram=0

# Alternatively load a different format image, resize it, and convert to 1 bit color.
start_image = Image.open('dontpanic.jpeg').resize((disp.width, disp.height), Image.ANTIALIAS ).convert('1')
# Display image.
disp.image(start_image)
disp.display()
time.sleep(4.2)
print("Don´t Panic!")
disp.clear()

def astronauts_in_space():
    r = requests.get("http://api.open-notify.org/astros.json")
    peopleinspace = r.json()['number']


def mars_nasa_api():
    f = "https://api.nasa.gov/insight_weather/?api_key=DEMO_KEY&feedtype=json&ver=1.0"
    data = requests.get(f)
    tt = json.loads(data.text)
    for i in tt:
     return tt[i]["Season"]

mars_season= mars_nasa_api().capitalize() #Capatilzes the first letter of the season
while True:

    if timeinprogram==43200:# Checks for Astronauts in Space every 12 hours/ 43200 hours
      astronauts_in_space()
    if timeinprogram ==43200: # Checks for season changes on mars every 12 hours / 43200 seconds
      mars_nasa_api()
      
      
    datetime.datetime.utcnow()
    time.time()

    def pretty_thousands(num):
        pretty_num = "{:,}".format(num)
        pretty_num = pretty_num.replace(',', ',')
        return pretty_num

 #Converting  Earth UTC date to sols
    tai_offset = 37     
    millis = 1000*time.time()
    jd_ut = 2440587.5 + (millis / (8.64*10**7))      
    jd_tt = jd_ut + (tai_offset + 32.184) / 86400
    j2000 = jd_tt - 2451545.0
    mars_sol = (((j2000 - 4.5) / 1.027491252) + 44796.0 - 0.00096)

 #Converting UTC to MTC (Martian Coordinated Time)
    mtc = (24 * mars_sol) % 24
    mtc_hours = int(mtc)
    mtc_minutes = int((mtc - mtc_hours)*60)
    mtc_seconds = int(((mtc - mtc_hours)*60 - mtc_minutes)*60)
    mtc_time = datetime.time(hour=mtc_hours, minute=mtc_minutes, second=mtc_seconds)
    Sol=(pretty_thousands(int(mars_sol)))
    Time=str(mtc_time)[0:8]
  
   # Load default font.
    #font = ImageFont.load_default()
   # Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as the python script!
   # Some other nice fonts to try: "http://www.dafont.com/bitmap.php" 

    font = ImageFont.truetype('Minecraftia-Regular.ttf', 8)


    # Draw a black filled box to clear the image.
    draw.rectangle((0,0,125,31), outline=25, fill=0)

    draw.text((x+10,top)  , str(Time)                 ,font=ImageFont.truetype('Minecraftia-Regular.ttf', 14), fill=255)  # Increased fontsize for Time
    draw.text((x+90,top+6) , "MTC"                    ,font=font, fill=255) 
    draw.text((x+4,top+15), "Sol :"                   ,font=font, fill=255)
    draw.text((x+30,top+16), str(Sol)                 ,font=font, fill=255)
    draw.text((x+80,top+16), str(mars_season)         ,font=font, fill=255)
    draw.text((x+7,top+23), "Astronauts :"            ,font=font, fill=255)
    draw.text((x+80,top+23), str(astronauts_in_space()) ,font=font, fill=255)
    
    
    # Display image.
    disp.image(image)
    disp.display()
    timeinprogram=timeinprogram+1
