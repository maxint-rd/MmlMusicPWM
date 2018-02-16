/*
 * MmlMusicPWM SoundFX
 * MmlMusicPWM library example for ESP8266/ATmega in Arduino IDE
 * see https://github.com/maxint-rd/MusicEngine
 *
 * by MMOLE (@maxint-rd)
 * This example code is in the public domain
 *
 * This example demonstrates how the library can also be used to produce
 * sound effects using a speaker/buzzer on a digital pin (eg. between GPIO14 and GND).
 * The music is played using an ticker-interrupt routine that changes the
 * PWM frequency according the note being played. This means we can do other
 * things while the music keeps playing. In this example we play a series of effects, 
 * produced using the tone() and play() functions.
 *
 *
 *
 */
#include <MmlMusicPWM.h>

// define pin and initialize MmlMusicPWM object
#define BUZ_PIN 14      // pin 4 recommended on Pro Mini since it has perfect distance from GND
MmlMusicPWM music(BUZ_PIN);

// Define a pin for the blinking LED. Pin 2 is used on ESP-07 and ESP-12 modules
// On ESP-01 the LED is on pin 1. Most Arduino boards have the LED on pin 13.
// On ESP the LED is active low, on Arduino is active HIGH
#define LED_BUILTIN 2

void blinkWhilePlaying()
{
    pinMode(LED_BUILTIN, OUTPUT);
    while(music.isPlaying())
    {
        digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (On ESP the LED is active LOW)
        delay(50);
        digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
        delay(100);
    }
}

void soundSiren(int nTimes=10)
{
  for(int nLoop=0; nLoop<nTimes; nLoop++)
  {
    for(int nFreq=100; nFreq<2000; nFreq+=10)
    {
      music.tone(nFreq);
      delay(1);
    }
    for(int nFreq=2000; nFreq>100; nFreq-=10)
    {
      music.tone(nFreq);
      delay(1);
    }
  }
}

void soundNoise(int nLength=30)
{
  srand(analogRead(A0));
  for(int nLoop=0; nLoop<nLength; nLoop++)
  {
    for(int nTimes=1; nTimes<max(400-nLoop*5,0); nTimes++)
    {
      music.tone(random(100, 2000));
      delay(1);
    }
    music.noTone();
    delay(max(200-nLoop*5,0));
  }
}

void soundNoise2(int nLength=20)
{
  srand(analogRead(A0));
  for(int nLoop=0; nLoop<nLength; nLoop++)
  {
    for(int nTimes=1; nTimes<100; nTimes++)
    {
      music.tone(random(100, 2000));
      delay(1);
    }
    delay(100);
  }
  music.noTone();
}


void soundNoise3(int nLength=5)
{
  srand(analogRead(A0));
  for(int nLoop=0; nLoop<nLength; nLoop++)
  {
    for(int nTimes=1; nTimes<200; nTimes++)
    {
      music.tone(random(100, 2000));
      delay(10);
    }
    delay(20);
  }
  music.noTone();
}



void setup() {
  // put your setup code here, to run once:
  music.play("T240 L16 O8 r1 CDEDC"); // give a short blurp
  blinkWhilePlaying();    // blink until done playing

  soundSiren(5);

  music.play("T240 L32 O8 r1 GDECBADGECDEDCr2GDECBADGECDEDCr1GDECBADGECDEDC"); // play a different noise
  blinkWhilePlaying();    // blink until done playing

  soundNoise();
  delay(500);
  soundNoise2();
  delay(500);
  soundNoise3();

  music.play("T240 L2 O2 r1 CDCECBrCDCDCD O1 r CDCDCDrCDCECB O0 r CDCDCDrCDCECB"); // play a different noise
  blinkWhilePlaying();    // blink until done playing
}

void loop() {
  // not much left to do...
  music.play("T240 L16 O8 CDEDC"); // give a short blurp
  blinkWhilePlaying();    // blink until done playing
  delay(1000);
}
