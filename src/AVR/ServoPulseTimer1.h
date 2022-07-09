// ServoPulseTimer1.h

#ifndef _SERVO_PULSE_TIMER1_h
#define _SERVO_PULSE_TIMER1_h

#if defined(__AVR_atmega328p__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328p__)
#define SERVO_PULSE_TIMER_1_ISR
#endif

#include <ServoPulseTimer.h>

#if defined(SERVO_PULSE_TIMER_1_ISR)
#include <avr/interrupt.h>
#include <avr/power.h>
#endif

/// <summary>
/// Uses Timer 1 to pulse a servo output.
/// Output pins are hardwired to D9 (Channel A) and D10 (Channel B).
/// </summary>
class ServoPulseTimer1
{
public:
	/// <summary>
	/// Channels for ServoPulseTimer1.
	/// Enum value is the Arduino pin number.
	/// </summary>
	enum ChannelEnum
	{
		ChannelA = 9,	// Pin D9.
		ChannelB = 10	// Pin D10.
	};

	/// <summary>
	/// Template static calculator for ServoPulseTimer1 raw PWM values.
	/// </summary>
	/// <template param name="CPUFrequency">CPU_FREQUENCY_16 or CPU_FREQUENCY_8</param>
	/// <param name="nanos">Servo Pulse duration [ServoPulseTimer::OutMinNanos ; ServoPulseTimer::OutMaxNanos]</param>
	/// <returns></returns>
	template<const uint32_t CPUFrequency = F_CPU>
	static const uint16_t GetRawValueFromNanoseconds(const uint32_t nanos)
	{
		if (CPUFrequency == ServoPulseTimer::CPU_FREQUENCY_16 || CPUFrequency == ServoPulseTimer::CPU_FREQUENCY_8)
		{
			const uint16_t outMax = ((uint32_t)TargetPeriod * UINT16_MAX) / (FullPeriod16 / (ServoPulseTimer::CPU_FREQUENCY_16 / CPUFrequency));
			const uint16_t outMin = outMax / 2;

			// Scale down by 10 so working value stays under UINT32_MAX, without losing precision on the output.
			//return outMin + ((((uint64_t)(nanos - ServoPulseTimer::OutMinNanos)) * (outMax - outMin)) / ServoPulseTimer::OutRangeNanos);
			return outMin + ((((nanos - ServoPulseTimer::OutMinNanos) / 10) * (outMax - outMin)) / (ServoPulseTimer::OutRangeNanos / 10));
		}
		else
		{
			// Unsupported frequency.
			return 0;
		}
	}

private:
	static const uint16_t TargetPeriod = 2000;

	// Prescalar 1 (@16 MHz) and Top = UINT16_MAX => Period  4096 us.
	static const uint16_t FullPeriod16 = 4096;

	// Prescalar 1 (@8 MHz) and Top = UINT16_MAX => Period  2048 us.
	static const uint16_t FullPeriod8 = FullPeriod16 * 2;

#if defined(SERVO_PULSE_TIMER_1_ISR)
#if F_CPU == 16000000L
	static const uint16_t OutMax = ((uint32_t)TargetPeriod * UINT16_MAX) / FullPeriod16;
#elif F_CPU == 8000000L
	static const uint16_t OutMax = ((uint32_t)TargetPeriod * UINT16_MAX) / FullPeriod8;
#else
#error CPU frequency not supported.
#endif

	static const uint16_t OutMin = OutMax / 2;
	static const uint16_t OutRange = OutMax - OutMin;

	enum PrescalerEnum { NO_CLOCK, PRESCALE_1 };
	static const PrescalerEnum Prescaler = PrescalerEnum::PRESCALE_1;

	enum {
		SET_A_ON_COMPARE = ServoPulseTimer::Bit(COM1A0) | ServoPulseTimer::Bit(COM1A1),
		SET_B_ON_COMPARE = ServoPulseTimer::Bit(COM1B0) | ServoPulseTimer::Bit(COM1B1),
	};
	static const uint8_t PwmMode = SET_A_ON_COMPARE | SET_B_ON_COMPARE;

	// Fast PWM, TOP = ICR1
	static const uint8_t BitMode = ServoPulseTimer::Bit(WGM11);
	static const uint8_t ClockMode = ServoPulseTimer::Bit(WGM12) | ServoPulseTimer::Bit(WGM13);

	static const uint8_t TCCR1AMask = BitMode | PwmMode;
	static const uint8_t TCCR1BMask = ClockMode | Prescaler;

private:
	static void PrepareTimer()
	{
		Stop();

		// Reset Counter.
		TCNT1 = UINT16_MAX;

		power_timer1_enable();

		// Disable timer interrupts.
		TIMSK1 &= ~(ServoPulseTimer::Bit(OCIE1A) | ServoPulseTimer::Bit(OCIE1B));

		// Clear timer interrupts.
		TIFR1 &= ~(ServoPulseTimer::Bit(OCF1A) | ServoPulseTimer::Bit(OCF1B));

		// Enable pwm interrupt.
		TIMSK1 |= ServoPulseTimer::Bit(OCIE1A) | ServoPulseTimer::Bit(OCIE1B);

		// Set TOP (Period) to ~2ms / 500Hz
		ICR1 = OutMax;

		// Set PWM mode, without setting prescaler yet.
		TCCR1B = TCCR1BMask;
	}

	static void StartTimer()
	{
		// Start timer.
		TCCR1A = TCCR1AMask;
	}

public:
	static void Pulse(const uint16_t valueA, const uint16_t valueB)
	{
		PrepareTimer();

		// Set inverted values to PWM registers.
		OCR1A = OutMax - valueA;
		OCR1B = OutMax - valueB;

		// Set pins as outputs.
		DDRB |= ServoPulseTimer::Bit(DDB1) | ServoPulseTimer::Bit(DDB2);

		StartTimer();
	}

	static void PulseA(const uint16_t value)
	{
		PrepareTimer();

		// Set inverted value to PWM register.
		OCR1A = OutMax - value;

		// Set pin as output.
		DDRB |= ServoPulseTimer::Bit(DDB1);

		StartTimer();
	}

	static void PulseB(const uint16_t value)
	{
		PrepareTimer();

		// Set inverted value to PWM register.
		OCR1B = OutMax - value;

		// Set pin as output.
		DDRB |= ServoPulseTimer::Bit(DDB2);

		StartTimer();
	}

	static void Stop()
	{
		// Stop Timer and reset all flags.
		TCCR1A = 0;
		TCCR1B = 0;

		power_timer1_disable();
	}
#endif
};
#endif