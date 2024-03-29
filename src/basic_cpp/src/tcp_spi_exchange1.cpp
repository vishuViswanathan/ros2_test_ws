/******************************************************************************
spitest.cpp
Raspberry Pi SPI interface demo
Byron Jacquot @ SparkFun Electronics>
4/2/2014
https://github.com/sparkfun/Pi_Wedge

A brief demonstration of the Raspberry Pi SPI interface, using the SparkFun
Pi Wedge breakout board.

Resources:

This example makes use of the Wiring Pi library, which streamlines the interface
to the the I/O pins on the Raspberry Pi, providing an API that is similar to the
Arduino.  You can learn about installing Wiring Pi here:
http://wiringpi.com/download-and-install/

The wiringPi SPI API is documented here:
https://projects.drogon.net/raspberry-pi/wiringpi/spi-library/

The init call returns a standard file descriptor.  More detailed configuration
of the interface can be performed using ioctl calls on that descriptor.
See the wiringPi SPI implementation (wiringPi/wiringPiSPI.c) for some examples.
Parameters configurable with ioctl are documented here:
http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/spi/spidev

Hardware connections:

This file interfaces with the SparkFun Serial 7 Segment display:
https://www.sparkfun.com/products/11629

The board was connected as follows:
(Raspberry Pi)(Serial 7 Segment)
GND  -> GND
3.3V -> Vcc
CE1  -> SS (Shift Select)
SCK  -> SCK 
MOSI -> SDI
MISO -> SDO

To build this file, I use the command:
>  g++ spitest.cpp -lwiringPi

Then to run it, first the spi kernel module needs to be loaded.  This can be 
done using the GPIO utility.
> gpio load spi
> ./a.out

This test uses the single-segment mode of the 7 segment display.  It shifts a 
bit through the display characters, lighting a single character of each at a 
time.

Development environment specifics:
Tested on Raspberry Pi V2 hardware, running Raspbian.
Building with GCC 4.6.3 (Debian 4.6.3-14+rpi1)

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include <iostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <unistd.h>

using namespace std;

// channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 1;

void send_receive(unsigned char *data, int len) {
    for (int n = 0; n < len; n++) {
      unsigned char *c = &data[n];
      wiringPiSPIDataRW(CHANNEL, c, 1);
    }
    cout << "buffer:" << (char *)data << endl;
}

int main()
{
    int fd, result;
    unsigned char buffer[100];
    unsigned char read_buffer[100];
    sprintf((char *)read_buffer, "For Getting data requested");
    cout << "Initializing" << endl ;

    // Configure the interface.
    // CHANNEL insicates chip select,
    // 500000 indicates bus speed.
    fd = wiringPiSPISetup(CHANNEL, 500000);

    cout << "Init result: " << fd << endl;

    sleep(1);

    sprintf((char *)buffer, "Encoder data requested*\n");
    send_receive(buffer, 24);
    sleep(1);

    sprintf((char *)buffer, "Speed l:+00300 r:+00600\n");
    send_receive(buffer, 24);
    sleep(1);

    sprintf((char *)buffer, "Encoder data requested*\n");
    send_receive(buffer, 24);
    sleep(1);

    sprintf((char *)buffer, "Encoder data requested*\n");
    send_receive(buffer, 24);
    sleep(1);

}
