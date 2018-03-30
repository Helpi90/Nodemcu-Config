#include <avr/io.h>
#include <avr/sleep.h>

#define F_CPU 600000UL
#include <util/delay.h>

void sleep_avr()
{
	// sleep mode is set here
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	// System sleeps here
	sleep_mode();
}

int main(void)
{
	// PB3 and PB4 will be output, all others input
	DDRB = 0x18;

	_delay_ms(3000);
	// pull up PB3 and PB4
	PORTB = 0x18;

	sleep_avr();
	while (1);
	return 0;
}
