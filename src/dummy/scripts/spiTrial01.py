#!/usr/bin/env python3
import spidev
from io import BytesIO
import time

# ===================================
# ==========   SPI   ================
# ===================================

# SPI Connections
# CE0 - Pin 24, BCM 8
# MOSI - Pin 19, BCM 10
# MISO - Pin 21, BCM 9
# SCLK - Pin 23, BCM 11


spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 115200

left_speed = -300
right_speed = 400
auto_manual_status = 1
#msgs = ["5nsp:l:00200,r:00100,am:01\n",]
msg = bytes('5nsp:l:{0:+05d},r:{1:+05d},am:{2:02d}\n'.
                format(left_speed, right_speed, auto_manual_status), 'utf-8')

print ('msg<', msg, '>')
response = spi.xfer(msg)
print("reponse <", bytes(response), ">")      
spi.close()