// display test
// PIC18LF4320


#include <htc.h>

#include "led.h"


/* configuration bits */

__CONFIG(1, RCIO );
__CONFIG(2, BORDIS & WDTDIS );


///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
IRCF2=1; IRCF1=1; IRCF0=1; //8MHz RC oscillator

ADCON1=0b1111; //disable analog inputs

led_init();

led_print(1,"1234567");

led_update();

while(1)
	{

	}
}
