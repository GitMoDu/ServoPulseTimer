# ServoPulseTimer


High precision alternative to Arduino Servo PWM.

Scheduler and Timer based Servo PWM generator. 

Uses Timer1 and Timer2 for up to 4 hardware Servo channels on Arduino AVR 328P.


![](https://github.com/GitMoDu/ServoPulseTimer/blob/master/Media/demomulti.png?raw=true)

    Timer1ChannelA and Timer2ChannelA being pulsed individually by a simple scheduler, 10 ms for Timer1 and 20 ms for Timer2.


## Context
A Servo PWM consists of a pulse with a duration of [ 1000 ; 2000] us. In a typical Servo output, the PWM is updated every 20 ms. Using timers for the dual purpose of PWM and update, reduces the Servo PWM resolution down to 20 times. This means 8 bit timers aren't practical for Servo PWM.

### Goals
Pulse the Servo outputs once on request, so that the pulsing can be done in a scheduler, independent of the pulse.

Maximize effective Servo PWM resolution, especially for 8 bit Timer2.

Servo PWM output duration must be interrupt tolerant, i.e. don't change duration if an interrupt happens during the pulse.


## Features

- Up to 4 hardware Servo channels.
- Each pulse is done individually within a scheduler pass.
- Each Timer channel-pair can have its own pulse update period.
- Interrupts may affect the update period, but the PWM output is not affected.
- Update period can go as low as 4 ms periods, or up to 100 ms or more.

ServoPulseTimer1 (16 bit timer)
- ~14 bits of effective resolution
- Some reduction in resolution with F_CPU = 8 MHz and lower.

ServoPulseTimer2 (8 bit timer)
- ~6 bits of effective Servo resolution
- Same resolution for F_CPU = 8 MHz and F_CPU = 16 MHz.


## Captures

![](https://github.com/GitMoDu/ServoPulseTimer/blob/master/Media/min.png?raw=true)

    Timer1ChannelA and Timer2ChannelA outputing a 1000 us Servo Pulse.
    
![](https://github.com/GitMoDu/ServoPulseTimer/blob/master/Media/max.png?raw=true)

    Timer1ChannelA and Timer2ChannelA outputing a 2000 us Servo Pulse.



## Operation

A single shot PWM pseudo-mode is done using PWM interrupts for each channel.

This also means the update period is not related to the PWM timing, so the update can actually be done down to 4 ms periods, or up to 100 ms or more.

Using ServoPulseTimer, the update will be done 20ms +-1, without taking into account other functions that might use some loop time. This results in the maximum possible resolution for each timer, at the expense of less consistent update period.


## Usage

Include the ServoTimer for your choice. By including the timer on AVR build, the associated Timer will be used.

    #include <ServoPulseTimer1.h>
    #include <ServoPulseTimer2.h>

Use a static call to pre-calculate the output range.
  
    const uint16_t Min1 = ServoPulseTimer1::GetRawValueFromNanoseconds(ServoPulseTimer::OutMinNanos);
    const uint16_t Max1 = ServoPulseTimer1::GetRawValueFromNanoseconds(ServoPulseTimer::OutMaxNanos);

    const uint8_t Min2 = ServoPulseTimer2::GetRawValueFromNanoseconds(ServoPulseTimer::OutMinNanos);
    const uint8_t Max2 = ServoPulseTimer2::GetRawValueFromNanoseconds(ServoPulseTimer::OutMaxNanos);
  
Use a static call to pulse all or a selected channel, using the precalculated output range and the input scale.

    rawValue1 = map(scale, 0, UINT16_MAX, Min1, Max1);
    ServoPulseTimer1::Pulse(rawValue1)
