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
spi.max_speed_hz = 9600

left_speed = -300
right_speed = 400
auto_manual_status = 1

msgs = [bytes('Encoder data requested*\n', 'utf-8'), 
        bytes('Speed l:-00200 r:-00300\n', 'utf-8'),
        bytes('Encoder data requested*\n', 'utf-8'),
        # bytes('Reset encoders*********\n', 'utf-8'),
        bytes('Encoder data requested*\n', 'utf-8'),
    ]
for m in msgs:
    print ('msg<', m, '>')
    response = spi.xfer(m)
    print("reponse <", bytes(response), ">")   
    time.sleep(1)   
spi.close()