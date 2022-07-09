// ServoPulseTimer2.h

#ifndef _SERVO_PULSE_TIMER2_h
#define _SERVO_PULSE_TIMER2_h

#if defined(__AVR_atmega328p__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328p__)
#define SERVO_PULSE_TIMER_2_ISR
#endif

#include <ServoPulseTimer.h>

#if defined(SERVO_PULSE_TIMER_2_ISR)
#include <avr/interrupt.h>
#include <avr/power.h>
#endif

/// <summary>
/// Uses Timer 2 to pulse a servo output.
/// Output pins are hardwired to D11 (Channel A) and D3 (Channel B).
/// </summary>
class ServoPulseTimer2
{
public:
	/// <summary>
	/// Channels for ServoPulseTimer2.
	/// Enum value is the Arduino pin number.
	/// </summary>
	enum ChannelEnum
	{
		ChannelA = 11,	// Pin D11.
		ChannelB = 3	// Pin D3.
	};

	/// <summary>
	/// Static calculator for ServoPulseTimer2 raw PWM values.
	/// </summary>
	/// <param name="nanos">Servo Pulse duration [ServoPulseTimer::OutMinNanos ; ServoPulseTimer::OutMaxNanos]</param>
	/// <returns></returns>
	static constexpr uint8_t GetRawValueFromNanoseconds(const uint32_t nanos)
	{
		return (OutMin + (((nanos - ServoPulseTimer::OutMinNanos) * OutRange) / ServoPulseTimer::OutRangeNanos));
	}

private:
	// Prescalar @16 MHz and PrescalerEnum::PRESCALE_128 => Period  2048 us.
	// Prescalar @8 MHz and PrescalerEnum::PRESCALE_64 => Period  2048 us.
	static const uint16_t FullPeriod = 2048;
	static const uint16_t TargetPeriod = 2008;

	static const uint8_t OutMax = ((uint32_t)TargetPeriod * UINT8_MAX) / FullPeriod;
	static const uint8_t OutMin = OutMax / 2;
	static const uint8_t OutRange = OutMax - OutMin;

#if defined(SERVO_PULSE_TIMER_2_ISR)
	enum PrescalerEnum { NO_CLOCK, PRESCALE_1, PRESCALE_8, PRESCALE_32, PRESCALE_64, PRESCALE_128, PRESCALE_256, PRESCALE_1024 };
#if F_CPU == 16000000L
	static const PrescalerEnum Prescaler = PrescalerEnum::PRESCALE_128;
#elif F_CPU == 8000000L
	static const PrescalerEnum Prescaler = PrescalerEnum::PRESCALE_64;
#else
#error CPU frequency not supported.
#endif

	enum {
		SET_A_ON_COMPARE = ServoPulseTimer::Bit(COM2A0) | ServoPulseTimer::Bit(COM2A1),
		SET_B_ON_COMPARE = ServoPulseTimer::Bit(COM2B0) | ServoPulseTimer::Bit(COM2B1)
	};
	static const uint8_t PwmMode = SET_B_ON_COMPARE | SET_A_ON_COMPARE;

	// Fast PWM, top = UINT8_MAX
	static const uint8_t BitMode = ServoPulseTimer::Bit(WGM20) | ServoPulseTimer::Bit(WGM21);
	static const uint8_t ClockMode = 0;

	static const uint8_t TCCR2BMask = ClockMode | Prescaler;

private:
	static void PrepareTimer()
	{
		Stop();

		// Reset Counter.
		TCNT2 = UINT8_MAX;

		power_timer2_enable();

		// Disable timer interrupts.
		TIMSK2 &= ~(ServoPulseTimer::Bit(OCIE2A) | ServoPulseTimer::Bit(OCIE2B));

		// Clear timer interrupts.
		TIFR2 &= ~ServoPulseTimer::Bit(TOV2);

		// Enable pwm interrupt.
		TIMSK2 |= (ServoPulseTimer::Bit(OCIE2A) | ServoPulseTimer::Bit(OCIE2B));

		// Set PWM mode, without setting prescaler yet.
		TCCR2A |= BitMode | PwmMode;

		// Fixed full counter.
		OCR2A = UINT8_MAX;
	}

	static void StartTimer()
	{
		// Start the timer.
		TCCR2B = TCCR2BMask;
	}

public:
	static void Pulse(const uint8_t valueA, const uint8_t valueB)
	{
		PrepareTimer();

		// Set pins as outputs.
		DDRB |= ServoPulseTimer::Bit(DDB3);
		DDRD |= ServoPulseTimer::Bit(DDD3);

		// Set inverted value to PWM register.
		OCR2A = UINT8_MAX - valueA;
		OCR2B = UINT8_MAX - valueB;

		StartTimer();
	}

	static void PulseA(const uint8_t value)
	{
		PrepareTimer();

		// Set inverted value to PWM register.
		OCR2A = UINT8_MAX - value;

		// Set pin as output.
		DDRB |= ServoPulseTimer::Bit(DDB3);

		StartTimer();
	}

	static void PulseB(const uint8_t value)
	{
		PrepareTimer();

		// Set inverted value to PWM register.
		OCR2B = UINT8_MAX - value;

		// Set pin as output.	
		DDRD |= ServoPulseTimer::Bit(DDD3);

		StartTimer();
	}

	static void Stop()
	{
		// Stop Timer and reset all flags.
		TCCR2A = 0;
		TCCR2B = 0;

		power_timer2_disable();
	}
#endif
};
#endif