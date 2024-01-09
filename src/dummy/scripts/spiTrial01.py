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
spi.max_speed_hz = 1000000

msgs = ["Speed l:+00200 r:+00400\n",
        "Encoder data requested*\n",
        "Encoder data requested*\n",]

for m in msgs: 
    response = bytes(spi.xfer(bytes(m, "utf-8")))
    print('decoded', response.decode('utf-8'))
    time.sleep(1)
      
spi.close()