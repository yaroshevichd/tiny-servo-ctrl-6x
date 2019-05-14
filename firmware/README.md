How to control PWM in software mode
===================================

It supposed that the following algorithm will provide enough precision:

* Build array of PWM data which includes the following:
** Timer clock count - 1 when to fire interrup
** Disable PWM mask
* Array must be sorted in the order of PWM channels need to set LOW

Assume timer is clocked with CLK/64 or 8MHz/64=125KHz, so one timer tick equals to 64cc - this is precision
Assume that we will use SG90, which is controlled with 50Hz PWM and 1000-2000us durty cycle.

So we can build array, which lenght is PWM+1, and first element is setup to fire interrupt atleast 125 timer tick, or 1ms.

Interrupt must do the following:

note: interrupt must fire 1 tick prior actual PWM output (so it have 64cc to do other logic)

* setup timer comare register to next array item
* wait for Xcc to send data to output port
* advance pointer
