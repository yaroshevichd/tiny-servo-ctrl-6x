How to control PWM in software mode
===================================

For given hardware (ATtiny44) the following hardware will be used:

* Timer0 - to generate PWM pulses with frequency equal to 50 Hz. Timer0 interrupts have lower
  priority than Timer1 and this is fine. It's possible to make this frequency configurable in
  the given range from 31 to 255 Hz (because timer will be driven with 1024 presale).
* Timer1 - to control PWM pulse length (from 500us to 2500us). It's possible to make this value
  configurable in range from 8us up to 32128us.
* USI - to communicate with main MCU. MCU will have static base address (0x40) with ability to
  specify subaddress in range from 0 to 6, which is results in ability to connect up to 7 such
  devices.

It supposed that the following algorithm will provide enough precision:

* Build array of PWM data which includes the following:

** Timer1 ticks count relative to previous array record, so that sum of all ticks will produce
   desired pulse length.
** Mask of servo need to be disabled.

* Array sorting order is based on sorted array of desired servos positions, which in turns, must
  be sorted in ascending order.
* Timer0 must be configured to generate interrupt with desired frequency (by default 50 Hz).
* Timer0 COMPA interrupt must do the following:

** Setup pointer to point first element of array
** Set Timer1 registers in accordance to data in first element of array
** Wait for some time, in case of necessity, before set all channels to HIGH and running Timer1

* Timer1 COMPA interrupt must do the following (it must be done faster than 64cc):

** Disable channels specified in data which pointer points to
** Advance pointer to 1 element forward
** If number of ticks equals to zero then stop Timer1
** Set Timer1 registers to count up to data which pointer points to
