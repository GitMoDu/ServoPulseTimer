// ServoPulseTimer.h

#ifndef _SERVO_PULSE_TIMER_h
#define _SERVO_PULSE_TIMER_h

#include <stdint.h>

class ServoPulseTimer
{
public:
	static const uint32_t OutMaxNanos = 2000000;
	static const uint32_t OutMinNanos = 1000000;
	static const uint32_t OutRangeNanos = OutMaxNanos - OutMinNanos;
	static const uint32_t OutMaxMicros = OutMaxNanos / 1000;
	static const uint32_t OutMinMicros = OutMinNanos / 1000;
	static const uint32_t OutRangeMicros = OutRangeNanos / 1000;

	static const uint32_t LimitNanoseconds(const uint32_t nanos)
	{
		if (nanos > ServoPulseTimer::OutMaxNanos)
		{
			return ServoPulseTimer::OutMaxNanos;
		}
		else if (nanos < ServoPulseTimer::OutMinNanos)
		{
			return ServoPulseTimer::OutMinNanos;
		}
		else
		{
			return nanos;
		}
	}

	static constexpr uint16_t Bit(const uint8_t shift)
	{
		return ((uint16_t)1) << shift;
	}
};
#endif

