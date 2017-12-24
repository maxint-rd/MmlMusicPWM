# MmlMusicPWM Blink example
This example demonstrates playing some music while blinking a LED.

 Blink the onboard LED on the ESP-07/ESP-12 module while playing a tune.
 The music is played using a ticker-interrupt routine that changes the
 PWM frequency according the note being played. This means we can do other
 things while the music keeps playing. In this example we simply wait.
