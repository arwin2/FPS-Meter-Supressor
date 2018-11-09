#include <SPI.h>
#include <Wire.h>

#include <io.h>
#include <interrupt.h>
#include <Arduino.h>

#define max_timer_overflow 2441 //10 seconden
uint16_t timer_overflow = 0;
uint16_t counter_value = 0;
float fps = 0;

int main(void) {
	init();

	Serial.begin(9600);

	setup_timers();
	
	setup_IO();

	while (true) {

	}
}

void setup_timers() {
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
	start_timer_1();
	//Serial.println("Start");
}

// Falling edge
ISR(INT1_vect) {
	stop_timer_1();
	
	fps = 3.280839895013123 / (((TCNT1 - 1) * 10.0) / 16000000.0);
	
	Serial.println('F');
	if ( fps > 0) Serial.println(fps);
	Serial.println('R');
	Serial.println(23);
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