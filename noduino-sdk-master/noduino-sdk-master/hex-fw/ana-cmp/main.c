#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 600000UL
#include <util/delay.h>

void led_on(void)
{
	// PB3 connected a blue led
	PORTB |= 0x08;
}

void led_off(void)
{
	PORTB &= 0xF7;
}

void reset_esp(void)
{
	// use PB4 to reset esp
	PORTB &= 0xEF;
	_delay_us(200);
	PORTB |= 0x10; 
}

ISR(ANA_COMP_vect)
{
	// we need the all reset
	reset_esp();

	if(ACSR & (1 << ACO)) {
		// switch enter left circle
		led_on();
	} else {
		led_off();
	}
}

void ana_comp_init(void)
{
	/*
	 * Configured the Analog Comparator 
	 * Enable ACIE and set interrupt to 
	 * trigger whenever AC0 go high. That 
	 * is both ACIS1 and ACIS0 are set to 0
	 *
	 * ACIS1 ACIS0
	 * 	0      0     output toggle
	 * 	0      1     output falling
	 * 	1      1     output rising
	 *
	*/
	cli();
	// (1 << ACIS1) | (1 << ACIS0)
	ACSR = (1 << ACIE);

	sei();
}

void flush_led(void)
{
	uint8_t i;
	for(i = 0; i < 5; i++) {
		_delay_ms(100);
		PORTB |= 0x08;
		_delay_ms(100);
		PORTB &= 0xF7;
	}
}

void io_init(void)
{
	// PB4, PB3, PB2 output
	DDRB = 0x1C; 

	/*
	 * PB4 High: Rest need to HIGH
	 * PB3 Low: LED need to disable 
	 */
	PORTB = 0x10;

	flush_led();
}

int main(void)
{
	io_init();
	ana_comp_init();

	while (1)
	{
	//	_delay_ms(1000);
	}
	return 0;
}
