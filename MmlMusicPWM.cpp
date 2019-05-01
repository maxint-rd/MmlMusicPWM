// MmlMusicPWM class, inherits from MmlMusic class\
//
// MmlMusic provides a means to play Music Macro Language sequences.
// MmlMusicPWM is the PWM driver for the MmlMusic library for Arduino.
// It implements playing MML music asynchronously on a chosen output pin.
// It generates sound using PWM and timer interrupts.
// On ESP8266/ESP32, the Ticker library is used for tone scheduling. 
// On the ATmega 328/168 it reuses the Timer2 interrupt to combine generating
// PWM sounds with scheduling starting and stopping playing the sound. 
// ATtiny85 is also supported but it has limited resources. 
// On that MCU Timer1 is used.
//
// For more information see:
//    http://www.github.com/maxint-rd/MmlMusic
//    http://www.github.com/maxint-rd/MmlMusicPWM
//

#include "MmlMusicPWM.h"

//#include <Arduino.h>
#if defined (ARDUINO_ARCH_ESP8266)
    #include <cctype>
#endif

MmlMusicPWM::MmlMusicPWM(int pin)		// pin=255 for default constructor without setting _pinPwm
    : MmlMusic()
    , _pinPwm(pin)
{
	if(_pinPwm!=255)
	{
    pinMode(_pinPwm, OUTPUT);
    this->noTone();
  }
}

void MmlMusicPWM::playTone(unsigned int frequency, unsigned long length, uint8_t nTrack, uint8_t nVolume)		// nTrack=0, nVolume=0
{
  if(frequency==0 && length==0)
	{  	
#if defined (ARDUINO_ARCH_ESP8266)
    analogWriteFreq(1000); // Note: analogWriteFreq(0);  gives a spontaneous WDT reset
    analogWrite(_pinPwm, 0);  // default range is 1024, start quiet using pulse-width zero
#elif defined(__AVR_ATtiny85__)
    _noToneTim1();
#else
    _noToneTim2();
#endif
	}
  else if(frequency>0)
  {
#if defined (ARDUINO_ARCH_ESP8266)
		// Set the PWM frequency to that specified by the note being played
		analogWriteFreq(frequency);
		// Note that PWM has lots of harmonics, so volume control using the PWM
		// duty-cycle is not very good, but perhaps better than nothing.
		// The default pwm-range is 1024. A 50% duty-cycle (=512) gives highest volume
		// The volume command Vnnn has a range 0-128, so we multiply by 4 to get the PWM
		// value.
		analogWrite(_pinPwm, nVolume * 4);
		if(length>0)
	    _scheduler.once(length/1000.0, &MmlMusicPWM::musicTickerStopCallback, this);	// MMOLE 190420: schedule when the tone should stop 
#elif defined(__AVR_ATtiny85__)
    _toneTim1(_pinPwm, frequency, length);
#else
    _toneTim2(_pinPwm, frequency, length);
#endif
	}
  else
  {
#if defined (ARDUINO_ARCH_ESP8266)
    _scheduler.once(length/1000.0, &MmlMusicPWM::musicTickerCallback, this);
#elif defined(__AVR_ATtiny85__)
    _waitToneTim1(length);
#else
    _waitToneTim2(length);
#endif
	}
}

#if defined(ARDUINO_ARCH_ESP8266)
void MmlMusicPWM::musicTickerCallback(MmlMusicPWM* __thisMmlMusicPWM)
{
    __thisMmlMusicPWM->continuePlaying();
}
void MmlMusicPWM::musicTickerStopCallback(MmlMusicPWM* __thisMmlMusicPWM)
{
    __thisMmlMusicPWM->noTone();
}
#elif defined(__AVR_ATtiny85__)
/*
On ATtiny85 we use Timer1 to generate the required timer interrupts.
ATtiny85 has two timers. When using the ATtiny "core" by D.A. Mellis, Timer0 is normally used for delay() and millis().
Timer1 is used for PWM using analogWrite(), so no PWM is available when using this library.
*/

// Timer1 interrupt for ATtiny85 (using ATtiny "core" by D.A. Mellis)
// See http://forum.arduino.cc/index.php?topic=75546.0
//
// Use ATtiny85 Timer/Counter1 to call a function every xx ms.
// In setup(), call setupTimer1() and pass it the name of your function which will
// run as part of the interrupt service routine (ISR) every 10ms. This function must take no
// arguments, return nothing, and should run as quickly as possible. The delay() and millis()
// functions will not work while the ISR is running and so should be avoided.
// Works with 1MHz or 8MHz system clock (others may be possible, but I got lazy).
// Scotchware License: If we meet some day, and you think this is worth it, you can buy me a scotch.
//
// Jack Christensen 18Oct2011
// Modified 20171211 by Maxint: use millis() for more accurate timing regardless MCU frequency
//

/*
//Copy code from here down, AND uncomment the following line and move it near the top of your sketch BEFORE the setup() function.
//void setupTimer1(void (*isrPointer)(void));        //function prototype for setupTimer1()
void (*_userISR)(void);              //pointer to the user's ISR

uint16_t _nTimerms=0;
void setupTimer1(uint16_t ms, void (*isrPointer)(void)) {
    _userISR = isrPointer;           //save the pointer to the user's ISR
    _nTimerms=ms;                   //save the time to fire the user's ISR
    //set timer1 up to generate an interrupt every millisecond
    TCCR1 |= _BV(CTC1);             //clear timer1 when it matches the value in OCR1C
    TIMSK |= _BV(OCIE1A);           //enable interrupt when OCR1A matches the timer value
    sei();                          //enable global interrupts
    OCR1A = 124;                    //set the match value for interrupt
    OCR1C = 124;                    //and the same match value to clear the timer
#if F_CPU == 1000000                //1MHz system clock
    TCCR1 |= _BV(CS12);             //set prescaler to divide by 8 (this starts the timer)
#elif F_CPU == 8000000              //8MHz system clock
    TCCR1 |= _BV(CS12) | _BV(CS11) | _BV(CS10);    //set prescaler to divide by 64 (this starts the timer)
#else
    Clock must be 1MHz or 8MHz!     //Error, only 1MHz or 8MHz clock (F_CPU) supported.
#endif
}
*/
		MmlMusicPWM* __thisMmlMusicPWM__;		// TODO: unfortunately I know no better way to call an instance method from the ISR than by using a global reference

void MmlMusicPWM::_toneTim1(uint8_t pin, unsigned int frequency, unsigned long length)
{	// playing a tone
	
	// tune the frequency to compensate for some timing differences that cause frequency to bit 2% too high
	frequency-=frequency/38;			// 38 is the magic number for ATtiny85 @ 8MHz
	
  if (length > 0) _tTim1_time = millis() + length - 1; else _tTim1_time = 0xFFFFFFFF; // Set when the note should end, or play "forever".

  if (_pinMask == 0) { // This gets the port register and bitmap for the pin and sets the pin to output mode.
    _pinMask   = digitalPinToBitMask(pin);                            // Get the port register bitmask for pin.
    _pinOutput = portOutputRegister(digitalPinToPort(pin));           // Get the output port register for pin.
    _pinMode   = (uint8_t *) portModeRegister(digitalPinToPort(pin)); // Get the port mode register for pin.
    *_pinMode |= _pinMask; // Set pin to Output mode.
  }

	// TODO: find out how to implement interrupts in class to get rid of globals and non-privates
	__thisMmlMusicPWM__=this;

	// Tone function, see also http://w8bh.net/avr/TrinketTone.pdf
	// scan through prescalars to find the best fit
	uint32_t ocr = F_CPU/frequency/2;
	uint8_t prescalar = 1;
	while(ocr>255)
	{
		prescalar++;
		ocr /= 2;
	}
  OCR1C = ocr-1;                     // Set the top. OCR1C is tied to CTC mode.
  if (TCNT1 > ocr) TCNT1 = ocr;      // Counter over the top, put within range.
  TCCR1  = 0x90 | prescalar;				 // Set Fast PWM and prescaler
  // Value 0x90 sets bits 7 & 4, which turn on CTC mode and set the output pin OCR1 to toggle on a match, respectively.
  TIMSK |= _BV(OCIE1A);             // Activate the timer interrupt.
}

void MmlMusicPWM::_noToneTim1(void)
{	// stop playing any tone
    TIMSK &= ~_BV(OCIE1A);    // Remove the timer interrupt.
    TCCR1  = 0x90 | 0x02;     // Default clock prescaler of 64 (TODO: check if 2 is indeed correct prescaler for 64_.
    *_pinMode &= ~_pinMask;   // Set pin to INPUT.
    _pinMask = 0; // Flag so we know note is no longer playing.
}

void MmlMusicPWM::_executeCommandTim1(void)
{
  	continuePlaying();
}

void MmlMusicPWM::_waitToneTim1(unsigned long length)
{   // set endtime of tone (or no tone) playing to allow callback at end of tone
    // TODO: find out how to implement interrupts in class to get rid of globals and non-privates
    __thisMmlMusicPWM__=this;		// need to be set in both tone and in wait to be able to call continuePlaying()

    _tTim1_time = millis() + length - 1;
    TIMSK |= _BV(OCIE1A);             // Activate the timer interrupt.
}


ISR(TIMER1_COMPA_vect)
{   //handles the Timer1 Compare Match A interrupt
  if(!__thisMmlMusicPWM__)
  	return;
  if (millis() > __thisMmlMusicPWM__->_tTim1_time)
  {
    __thisMmlMusicPWM__->_noToneTim1(); // Check to see if it's time for the note to end.
    __thisMmlMusicPWM__->_executeCommandTim1();		// execute the next command
  }
  else
	  *(__thisMmlMusicPWM__->_pinOutput) ^= __thisMmlMusicPWM__->_pinMask; // Toggle the pin state.
}
#else
// ATmega Timer2 tone function derived from ToneAC2 library code: https://bitbucket.org/teckel12/arduino-toneac2/

		// #elif defined (__AVR_ATmega328P__) ||  defined (__AVR_ATmega328__) ||  defined (__AVR_ATmega168__)  ||  defined (__AVR_ATmega168P__) 
		// #elif defined (ARDUINO_UNO) ||  defined(ARDUINO_AVR_MEGA2560)
		MmlMusicPWM* __thisMmlMusicPWM__;		// TODO: unfortunately I know no better way to call an instance method from the ISR than by using a global reference

void MmlMusicPWM::_toneTim2(uint8_t pin, unsigned int frequency, unsigned long length)
{	// playing a tone
  long top;
  uint8_t prescaler;

  for (prescaler = 1; prescaler < 8; prescaler++) { // Find the appropriate prescaler
    top = F_CPU / (long) frequency / (long) _tTim2_prescale[prescaler - 1] - 1; // Calculate the top.
    if (top < 256) break; // Fits, break out of for loop.
  }
  if (top > 255) { _noToneTim2(); return; } // Frequency is out of range, turn off sound and return.

  if (length > 0) _tTim2_time = millis() + length - 1; else _tTim2_time = 0xFFFFFFFF; // Set when the note should end, or play "forever".

  if (_pinMask == 0) { // This gets the port register and bitmap for the pin and sets the pin to output mode.
    _pinMask   = digitalPinToBitMask(pin);                            // Get the port register bitmask for pin.
    _pinOutput = portOutputRegister(digitalPinToPort(pin));           // Get the output port register for pin.
    _pinMode   = (uint8_t *) portModeRegister(digitalPinToPort(pin)); // Get the port mode register for pin.
    *_pinMode |= _pinMask; // Set pin to Output mode.
  }

	// TODO: find out how to implement interrupts in class to get rid of globals and non-privates
	__thisMmlMusicPWM__=this;

  OCR2A   = top;                     // Set the top.
  if (TCNT2 > top) TCNT2 = top;      // Counter over the top, put within range.
  TCCR2B  = _BV(WGM22) | prescaler;  // Set Fast PWM and prescaler.
  TCCR2A  = _BV(WGM20) | _BV(WGM21); // Fast PWM and normal port operation, OC2A/OC2B disconnected.
  //TIMSK2 &= ~_BV(OCIE2A);            // Stop timer 2 interrupt while we set the pin states.
  TIMSK2 |= _BV(OCIE2A);             // Activate the timer interrupt.
}

void MmlMusicPWM::_noToneTim2(void)
{	// stop playing any tone
    TIMSK2 &= ~_BV(OCIE2A);     // Remove the timer interrupt.
    TCCR2B  = _BV(CS22);        // Default clock prescaler of 64.
    TCCR2A  = _BV(WGM20);       // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
    *_pinMode &= ~_pinMask;   // Set pin to INPUT.
    _pinMask = 0; // Flag so we know note is no longer playing.
}

void MmlMusicPWM::_executeCommandTim2(void)
{
  	continuePlaying();
}

void MmlMusicPWM::_waitToneTim2(unsigned long length)
{   // set endtime of tone (or no tone) playing to allow callback at end of tone
    // TODO: find out how to implement interrupts in class to get rid of globals and non-privates
    __thisMmlMusicPWM__=this;		// need to be set in both tone and in wait to be able to call continuePlaying()

    _tTim2_time = millis() + length - 1;
    TIMSK2 |= _BV(OCIE2A);             // Activate the timer interrupt.
}

ISR(TIMER2_COMPA_vect)
{ // Timer interrupt vector.
  if(!__thisMmlMusicPWM__)
  	return;
  if (millis() > __thisMmlMusicPWM__->_tTim2_time)
  {
    __thisMmlMusicPWM__->_noToneTim2(); // Check to see if it's time for the note to end.
    __thisMmlMusicPWM__->_executeCommandTim2();		// execute the next command
  }
  else
	  *(__thisMmlMusicPWM__->_pinOutput) ^= __thisMmlMusicPWM__->_pinMask; // Toggle the pin state.
}
#endif
