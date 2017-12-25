#ifndef __MMLMUSICPWM_H__
#define __MMLMUSICPWM_H__
#include "MmlMusic.h"
//#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <Ticker.h>
#else
// for ATmega we'll use redefinition of Timer2 (on 328/168) or Timer2 (on ATtiny) and portmanipulation
#endif

class MmlMusicPWM : public MmlMusic {
public:
    /** Creates an instance of the MmlMusic
      * @param pin pin used to generate the note frequencies
    */
    MmlMusicPWM(int pin=255); // Constructor
		void playTone(unsigned int frequency, unsigned long length, uint8_t nTrack=0, uint8_t nVolume=0);		// public to allow access from callback

private:
    int _pinPwm;


#if defined (ARDUINO_ARCH_ESP8266)
    Ticker     _scheduler;
    static void musicTickerCallback(MmlMusicPWM*);
#else
    //MsTimer2   _scheduler;
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#elif defined(__AVR_ATtiny85__)
public:
		// Timer1 is used for ATtiny85
		void _toneTim1(uint8_t pin, unsigned int frequency = 0, unsigned long length = 0);
		void _noToneTim1(void);
		void _waitToneTim1(unsigned long length);
		void _executeCommandTim1(void);

		// member variables called by ISR need to be public
		unsigned long _tTim1_time; // Used to track end note with timer when playing note in the background.
		volatile uint8_t *_pinMode;     // Pin mode.
		uint8_t _pinMask = 0;        // Bitmask for pins
		volatile uint8_t *_pinOutput; // Output port register for pin.
#else
// ATmega Timer2 tone function derived from ToneAC2 library code: https://bitbucket.org/teckel12/arduino-toneac2/
public:
		// Timer2 is used for atmega
		void _toneTim2(uint8_t pin, unsigned int frequency = 0, unsigned long length = 0);
		void _noToneTim2(void);
		void _waitToneTim2(unsigned long length);
		void _executeCommandTim2(void);

		// member variables called by ISR need to be public
		unsigned long _tTim2_time; // Used to track end note with timer when playing note in the background.
		volatile uint8_t *_pinMode;     // Pin mode.
		uint8_t _pinMask = 0;        // Bitmask for pins
		volatile uint8_t *_pinOutput; // Output port register for pin.
		const int _tTim2_prescale[7] = { 2, 16, 64, 128, 256, 512, 2048 }; // Prescaler.
#endif

private:
    static const float PERIOD_TABLE[];
};
#endif //__MMLMUSICPWM_H__
