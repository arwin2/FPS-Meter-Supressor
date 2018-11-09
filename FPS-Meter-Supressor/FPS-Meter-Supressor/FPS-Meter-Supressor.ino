#include <SPI.h>
#include <Wire.h>

#include <io.h>
#include <interrupt.h>
#include <Arduino.h>

#define max_timer_0_overflow 61		// 1 Seconden
#define max_timer_1_overflow 2441	//10 seconden
float timer_0_overflow = 0;
uint16_t timer_1_overflow = 0;
uint16_t counter_value = 0;
float fps = 0;
float rof = 0;

int main(void) {
	init();

	Serial.begin(9600);

	setup_timers();
	
	setup_IO();

	while (true) {

	}
}

void setup_timers() {
	// TIMER 0
	TCCR0A |= (1 << WGM01); // CTC Mode
	
	OCR0A = 0xFF;			// Overflow
	
	TCCR0B &= (0 << CS00);
	TCCR0B &= (0 << CS01);
	TCCR0B &= (0 << CS02);	// Stop timer 0

	TCNT0 = 0;
	
	TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect

	// TIMER 1
	OCR1A = 0xFFFF;				// Timer 1 overflow
	
	TCCR1A &= (0 << WGM10);
	TCCR1A &= (0 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B &= (0 << WGM13);		// Timer 1 Mode 4, CTC on OCR1A

	TCCR1B &= (0 << CS10);
	TCCR1B &= (0 << CS11);
	TCCR1B &= (0 << CS12);		// Timer 1 disabled

	TCNT1 = 0;

	TIMSK1 |= (1 << OCIE1A);	// Timer 1 Set interrupt on compare match
}

void start_timer_0() {
	TCCR0B |= (1 << CS00);
	TCCR0B |= (1 << CS02);		// Start timer 0
}

void stop_timer_0() {
	TCCR0B &= (0 << CS00);
	TCCR0B &= (0 << CS01);
	TCCR0B &= (0 << CS02);	// Stop timer 0
}

void start_timer_1() {
	TCCR1B |= (1 << CS10);
}

void stop_timer_1() {
	TCCR1B &= (0 << CS10);
	TCCR1B &= (0 << CS11);
	TCCR1B &= (0 << CS12);		// Timer 1 disabled
}

void setup_IO() {
	DDRD |= (1 << DDD4);		// Sets bit DDD4 to 1 within register DDRD
								// PD4 is now an output WOOT !!!
	DDRB |= (1 << DDB5);		// Onboard LED

	// Interrupt INT0 PD2
	DDRD &= ~(1 << DDD2);		// Clear pin
	//PORTD |= (1 << PORTD2);	// Pull-up
	EICRA |= (1 << ISC00);
	EICRA |= (1 << ISC01);		// Rising edge
	EIMSK |= (1 << INT0);		// Turn on INT0

	// Interrupt INT1 PD3
	DDRD &= ~(1 << DDD3);		// CLear pin
	//PORTD |= (1 << PORTD3);	// Pull-up
	//EICRA |= (1 << ISC10);
	EICRA |= (1 << ISC11);		// Falling edge
	EIMSK |= (1 << INT1);		// Turn on INT1
}

// Rising edge
ISR(INT0_vect) {
	TCNT1 = 0;
	start_timer_1();			// FPS Timer
	if (timer_0_overflow == 0) {// ROF Timer
		start_timer_0();
	} else {
		stop_timer_0;
		rof = 15625.0 / (TCNT0 + (timer_0_overflow * 255.0));
		Serial.println('R');
		Serial.println(rof);
		TCNT0 = 0;
		timer_0_overflow = 0;
	}
}

// Falling edge
ISR(INT1_vect) {
	stop_timer_1();
	
	fps = 3.280839895013123 / (((TCNT1 - 1) * 10.0) / 16000000.0);
	
	Serial.println('F');
	if ( fps > 0) Serial.println(fps);
	/*Serial.println('R');
	Serial.println(23);*/
}

// timer0 overflow interrupt
ISR(TIMER0_COMPA_vect) {
	timer_0_overflow++;
}

// Timer 1 compare match
ISR(TIMER1_COMPA_vect) {

}

/*display.setCursor(0, 0);
display.setTextColor(WHITE);
display.setTextSize(1);
display.print("FPS");

display.setCursor(30, 11);
display.setTextSize(3);
display.print(360.5);

display.display();*/