# MmlMusicPWM SoundFX example
This example demonstrates how the MmlMusicPWM library can also be used to produce sound effects using a speaker/buzzer on a digital pin (eg. between GPIO14 and GND). 
The music is played using an ticker-interrupt routine that changes the PWM frequency according the note being played. 
This means we can do other things while the music keeps playing. In this example we play a series of effects, produced using the tone() and play() functions.

