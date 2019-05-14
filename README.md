It's a try to make PWM controller on X channel using AT MCU. MCU communication is I2C.

PCA9685
ATtiny44A

ATtiny44A:
top speed: 8MHz
enter/exit interrupt: 8cc
    + Xcc - determine enough cycles


let suppose we want to have resolution at 2 deg, then I need at least 90 interrupt per 1ms or 1 interrupt each 11us which at 8MHz clock speed (0.125us) will be 88cc


low boundary is fixed
t=0             -> all PWM HGH
t=low boundary  -> first interrupt
...     intermediate interrupts
t=high boundary -> last interrupt

10cc to prepare 1 pwm channel data or 60cc for all PWMS + 10cc to track cycles + 1cc to output = 71cc min
