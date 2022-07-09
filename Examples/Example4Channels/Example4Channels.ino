//
/* Example showing the use of ServoPulseTimer1 and ServoPulseTimer2.
* Each ServoPulseTimer will pulse the requested servo value once.
* Pulsing again is left up to schedulling rather than the Timer overflow.
* This way we have a, interrupt imune, best possible resolution for the servo output,
*	at the expense of update frequency jitter.
* As a bonus, we can update the Timer1 and Timer2 outputs at different periods.
*/




#define SERIAL_BAUD_RATE 115200

#include <ServoPulseTimer1.h>
#include <ServoPulseTimer2.h>

const uint32_t UpdatePeriodTimer1 = 10;
const uint32_t UpdatePeriopTimer2 = 20;

const uint32_t TestPeriod = 10000;
const uint32_t DisplayUpdatePeriod = 20;

uint32_t Timer1LastUpdated = 0;
uint32_t Timer2LastUpdated = 0;
uint32_t DisplayLastUpdated = 0;


void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);

	Serial.println(F("Example 4 Channels Start."));
	delay(1000);
}


void loop()
{
	uint32_t timestamp = millis();

	uint16_t progress = ((timestamp % TestPeriod) * UINT16_MAX) / TestPeriod;

	uint32_t progressValue = ServoPulseTimer::OutMinNanos;
	progressValue += (((uint64_t)progress * ServoPulseTimer::OutRangeNanos) / UINT16_MAX);

	if (timestamp - Timer1LastUpdated >= UpdatePeriodTimer1)
	{
		ServoPulseTimer1::Pulse(
			ServoPulseTimer1::GetRawValueFromNanoseconds(progressValue),
			ServoPulseTimer1::GetRawValueFromNanoseconds(progressValue));
		Timer1LastUpdated = timestamp;
	}

	if (timestamp - Timer2LastUpdated >= UpdatePeriopTimer2)
	{
		ServoPulseTimer2::Pulse(
			ServoPulseTimer2::GetRawValueFromNanoseconds(progressValue),
			ServoPulseTimer2::GetRawValueFromNanoseconds(progressValue));
		Timer2LastUpdated = timestamp;
	}

	if (timestamp - DisplayLastUpdated >= DisplayUpdatePeriod)
	{
		Serial.println(progress);
		DisplayLastUpdated = timestamp;
	}
}

