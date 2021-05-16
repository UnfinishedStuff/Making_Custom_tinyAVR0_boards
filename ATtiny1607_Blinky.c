// By default the 1607 has a 20MHz CPU divided by 6 to give 3.3 MHz
#define F_CPU 3300000

// For basic pin operations
#include <avr/io.h>
// For the delay function
#include <util/delay.h>

// Main program starts here
int main(void)
{
  // Set the PORTB pins to work as outputs
  PORTB_DIR = 0xff;
	
  // Do this forever:
  while (1) 
  {
    // Turn all PORTB pins OFF
    PORTB_OUT = 0xff;
    // Wait one second
    _delay_ms(1000);
    // Turn all PORTB pins ON
    PORTB_OUT = 0x00;
    // Wait one second
    _delay_ms(1000);
  }
}
