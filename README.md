# ServoPulseTimer
High precision alternative to Arduino Servo PWM.

Scheduler based Servo PWM generator. Uses Timer1 and Timer2 for up to 4 Servo channels on Arduino AVR 328P.

A Servo PWM consists of a pulse with a duration of [1;2] ms. In a typical Servo output, the PWM is updated every 20 ms.

Using ServoPulseTimer, the update will be done 20ms +-1, without taking into account other functions that might use some loop time. This results in the maximum possible resolution for each timer, at the expense of less consistent update period.

Each pulse must be done individually within a scheduler. Interrupts may affect the update period, but the PWM output is not affected.
This also means the update period is not related to the PWM timing, so the update can actually be done down to 4 ms periods, or up to 100 ms or more.

ServoPulseTimer1 (16 bit timer) can output ~14 bits of effective resolution, with some reducition with F_CPU = 8 MHz and lower.
ServoPulseTimer2 (8 bit timer) can output ~6 bits of effective Servo resolution, same for F_CPU = 8 MHz and F_CPU = 16 MHz.

