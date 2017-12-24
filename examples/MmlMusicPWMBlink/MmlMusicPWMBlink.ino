/*
 * MmlMusicPWM Blink
 * MmlMusicPWM library example for ESP8266 and ATmega in Arduino IDE
 * see https://github.com/maxint-rd/MmlMusicPWM
 *
 * by MMOLE (@maxint-rd)
 * Based on ESP8266 Blink by Simon Peter
 * This example code is in the public domain
 *
 * Blink the onboard LED on the ESP-07/ESP-12 module while playing a tune.
 * The music is played using an ticker-interrupt routine that changes the
 * PWM frequency according the note being played. This means we can do other
 * things while the music keeps playing. In this example we simply wait.
 *
 * Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
 * The blue onboard LED on the ESP-01 module is connected to GPIO1
 * (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 * On the ESP-07 and ESP-12 modules the LED is connected to GPIO2.
 * When using the board "Generic ESP8266 Module" LED_BUILTIN needs to be redefined.
 */
#include <MmlMusicPWM.h>

// define the pin used and initialize a MusicEngine object
#define BUZ_PIN 14
MmlMusicPWM music(BUZ_PIN);

#define LED_BUILTIN 2 // pin 2 is used on ESP-07 and ESP-12 modules

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

    // Start playing some music (if impatient use the short tune of the lower line).
    music.play("t112v127l12<dddg2>d2c<ba>g2d4c<ba>g2d4cc-c<a2d6dg2>d2c<ba>g2d4c<ba>g2d4cc-c<"
               "a2d6de4.eb+8&b8a8&g8g&a&b&a6&e&f+4&d6de4.el8b+&ba&g>d.<a16&a2d6d12e4.eb+&ba&gl12g&"
               "a&b&a6&e&f+4>d6dg6fe6dc6<a+a6g>d2.r8<dddg2>d2c<ba>g2d4c<ba>g2d4cc-c<a2d6dg2>d2c<ba>"
               "g2d4c<ba>g2d4cc-c<a2>dg1&g2.gggg8");
    // music.play("T180 L8 CDEC. r CDEC. r EFG. r EFG. r GAGFEC. r GAGFEC. r L4 C<A>C. r C<A>C.");

    // Wait until the music is done playing, show fast blinking while playing
    while (music.isPlaying()) {
        digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is acive low on the ESP module)
        delay(50);
        digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
        delay(200);
    }
}

// the loop function runs over and over again forever
void loop()
{
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (active LOW)
    music.play("T240 O8 E64"); // give a short beep
    delay(1000); // Wait for one second to show the led switched on
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off (output HIGH)
    music.play("T240 O8 G64"); // give a shorter slightly higher beep
    delay(2000); // Wait for two second to show the led switched off
}
