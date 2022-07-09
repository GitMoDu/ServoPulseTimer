
#include "ServoPulseTimer1.h"

#if defined(SERVO_PULSE_TIMER_1_ISR)
ISR(TIMER1_COMPA_vect)
{
	OCR1A = UINT16_MAX;
}

ISR(TIMER1_COMPB_vect)
{
	OCR1B = UINT16_MAX;
}
#endif
