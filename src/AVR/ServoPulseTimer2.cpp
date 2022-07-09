#include "ServoPulseTimer2.h"

#if defined(SERVO_PULSE_TIMER_2_ISR)
ISR(TIMER2_COMPA_vect)
{
	OCR2A = UINT8_MAX;
}

ISR(TIMER2_COMPB_vect)
{
	OCR2B = UINT8_MAX;
}
#endif