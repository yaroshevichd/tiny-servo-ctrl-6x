import parsy

arithmetic_and_logic = (
    parsy.string('add').result(1) |
    parsy.string('adc').result(1) |
    parsy.string('adiw').result(2) |
    parsy.string('subi').result(1) |
    parsy.string('sub').result(1) |
    parsy.string('sbci').result(1) |
    parsy.string('sbc').result(1) |
    parsy.string('sbiw').result(2) |
    parsy.string('andi').result(1) |
    parsy.string('and').result(1) |
    parsy.string('ori').result(1) |
    parsy.string('or').result(1) |
    parsy.string('eor').result(1) |
    parsy.string('com').result(1) |
    parsy.string('neg').result(1) |
    parsy.string('sbr').result(1) |
    parsy.string('cbr').result(1) |
    parsy.string('inc').result(1) |
    parsy.string('dec').result(1) |
    parsy.string('tst').result(1) |
    parsy.string('clr').result(1) |
    parsy.string('ser').result(1)
)

branch = (
    parsy.string('rjmp').result(2) |
    parsy.string('ijmp').result(2) |
    parsy.string('rcall').result(3) |
    parsy.string('icall').result(3) |
    parsy.string('reti').result(4) |
    parsy.string('ret').result(4) |
    parsy.string('cpse').result((1, 2, 3)) |
    parsy.string('cpc').result(1) |
    parsy.string('cpi').result(1) |
    parsy.string('cp').result(1) |
    parsy.string('sbrc').result((1, 2, 3)) |
    parsy.string('sbrs').result((1, 2, 3)) |
    parsy.string('sbic').result((1, 2, 3)) |
    parsy.string('sbis').result((1, 2, 3)) |
    parsy.string('brbs').result((1, 2)) |
    parsy.string('brbc').result((1, 2)) |
    parsy.string('breq').result((1, 2)) |
    parsy.string('brne').result((1, 2)) |
    parsy.string('brcs').result((1, 2)) |
    parsy.string('brcc').result((1, 2)) |
    parsy.string('brsh').result((1, 2)) |
    parsy.string('brlo').result((1, 2)) |
    parsy.string('brmi').result((1, 2)) |
    parsy.string('brpl').result((1, 2)) |
    parsy.string('brge').result((1, 2)) |
    parsy.string('brlt').result((1, 2)) |
    parsy.string('brhs').result((1, 2)) |
    parsy.string('brhc').result((1, 2)) |
    parsy.string('brts').result((1, 2)) |
    parsy.string('brtc').result((1, 2)) |
    parsy.string('brvs').result((1, 2)) |
    parsy.string('brvc').result((1, 2)) |
    parsy.string('brie').result((1, 2)) |
    parsy.string('brid').result((1, 2))
)

bit_and_bit_test = (
    parsy.string('sbi').result(2) |
    parsy.string('cbi').result(2) |
    parsy.string('lsl').result(1) |
    parsy.string('lsr').result(1) |
    parsy.string('rol').result(1) |
    parsy.string('ror').result(1) |
    parsy.string('asr').result(1) |
    parsy.string('swap').result(1) |
    parsy.string('bset').result(1) |
    parsy.string('bclr').result(1) |
    parsy.string('bst').result(1) |
    parsy.string('bld').result(1) |
    parsy.string('sec').result(1) |
    parsy.string('clc').result(1) |
    parsy.string('sen').result(1) |
    parsy.string('cln').result(1) |
    parsy.string('sez').result(1) |
    parsy.string('clz').result(1) |
    parsy.string('sei').result(1) |
    parsy.string('cli').result(1) |
    parsy.string('ses').result(1) |
    parsy.string('cls').result(1) |
    parsy.string('sev').result(1) |
    parsy.string('clv').result(1) |
    parsy.string('set').result(1) |
    parsy.string('clt').result(1) |
    parsy.string('seh').result(1) |
    parsy.string('clh').result(1)
)

data_transfer = (
    parsy.string('movw').result(1) |
    parsy.string('mov').result(1) |
    parsy.string('ldi').result(1) |
    parsy.string('ldd').result(2) |
    parsy.string('lds').result(2) |
    parsy.string('ld').result(2) |
    parsy.string('std').result(2) |
    parsy.string('sts').result(2) |
    parsy.string('st').result(2) |
    parsy.string('lpm').result(3) |
    parsy.string('spm').result(0) |
    parsy.string('in').result(1) |
    parsy.string('out').result(1) |
    parsy.string('push').result(2) |
    parsy.string('pop').result(2)
)

mcu_control = (
    parsy.string('nop').result(1) |
    parsy.string('sleep').result(1) |
    parsy.string('wdr').result(1) |
    parsy.string('break').result(0)
)

cycles = (
    branch |
    arithmetic_and_logic |
    bit_and_bit_test |
    data_transfer |
    mcu_control
)
