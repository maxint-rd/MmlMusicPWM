# Examples

These examples are made to demonstrate the MmlMusicPWM library for ESP8266/ATmega within the Arduino IDE.<br>
The examples were tested in the Arduino 1.6.10 IDE using an ESP-12F on a WeMos D1 Mini board with the buzzer connected to GPIO14 and the onboard LED on GPIO2.
With minor pin adjustments, they should also work on Atmel ATmega 328/168 boards such as the Arduino Uno, Nano and Pro-mini. 
In the release of the MmlMusicPWM library additional support was added for the ATtiny85, but one must take into account the resource limits of that MCU.

**WARNING**: Depending the specifications connecting a buzzer directly to an output-pin may draw too much current and can damage your ESP module. Although my ESPs don't seem to mind, yours may fail. If you don't mind taking risks, go ahead; otherwise use a current limiting resistor or drive the buzzer via a transistor.

For more information see the comments in the example code or read the library documentation.

ESP-07 playing music (click to view on YouTube):<br>
[![ESP-07 playing music](https://img.youtube.com/vi/BSmXyXZrRK0/0.jpg)](https://www.youtube.com/watch?v=BSmXyXZrRK0)
