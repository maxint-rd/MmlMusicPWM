/*
 * MmlMusicPWM Serial
 * MmlMusicPWM library example for ESP8266/Atmega in Arduino IDE
 * see https://github.com/maxint-rd/MmlMusicPWM
 *
 * by MMOLE (@maxint-rd)
 * This example code is in the public domain
 *
 * This example uses the serial console to receive music notes to be played
 * using a speaker/buzzer on a digital pin (eg. between GPIO14 and GND).
 * The music is played using an ticker-interrupt routine that changes the
 * PWM frequency according the note being played. This means we can do other
 * things while the music keeps playing. In this example we check the serial
 * line for new input.
 *
 *
 *
 */
#include <MmlMusicPWM.h>

// define pin and initialize MmlMusicPWM object
#define BUZ_PIN 14			// pin 4 recommended on Pro Mini since it has perfect distance from GND
MmlMusicPWM music(BUZ_PIN);

// Reserve a buffer for playing the notes received via the serial console.
// Note that this buffer should remain available while playing.
char szBuf[128]; // serial buffer seems to be only 128 bytes on ESP, only 64 on ATmega

void fnCompleted(void)
{   // callback function to notify completion
    Serial.print(F("\nDone playing.\nPlease type new notes to play something else."));
}

void setup()
{ // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();
    Serial.println(F("=================="));
    Serial.println(F("Serial MmlMusicPWM"));
    Serial.println(F("=================="));
    Serial.println(F("Please type your notes and hit Enter."));
    Serial.println(F("Example tune: T180 L8 CDEC. r CDEC. r EFG. r EFG. r GAGFEC. r GAGFEC. r L4 "
                     "C<A>C. r C<A>C."));
    music.play("T240 L16 O8 rCDEDC"); // give a short blurp
}

void loop()
{ // put your main code here, to run repeatedly:
    while (!Serial.available()) { // Wait for a serial string to be entered.
        // Sound a beep and and print a dot every 5 sec to indicate we're waiting.
        for (int n = 0; n < 50 && !Serial.available(); n++)
            delay(100); // just delay while waiting for input
        Serial.print(".");
        if (!music.isPlaying())
        {
            music.setCompletionCallback(NULL);    // prevent notification after sounding a little beep
            music.play("T250 L64 O7 B"); // give a short tick-like beep when waiting for input
        }
    }

    if (Serial.available() > 0) { // assume the string is valid MML and try to play it
        int nRead = Serial.readBytesUntil('\n', szBuf, sizeof(szBuf) - 1);
        szBuf[nRead] = '\0'; // terminate string
        Serial.println(F("\nPlaying tune:"));
        Serial.println(szBuf);
        music.setCompletionCallback(fnCompleted);   // setup the callback to notify completion
        music.play(szBuf); // the buffer is global to reserve it while playing.
    }
}
