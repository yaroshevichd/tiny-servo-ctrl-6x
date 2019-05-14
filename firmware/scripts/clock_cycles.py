import sys
import time
from attiny48 import cycles
from parser_asm import parser

# test data
# print(parser.parse('  26:	cf e5       	ldi	r28, 0x5F	; 95'))
# print(parser.parse('  24:	1f be       	out	0x3f, r1	; 63'))
# print(parser.parse('  20:	10 c0       	rjmp	.+32     	; 0x42 <__bad_interrupt>'))
# print(parser.parse('  28:	d1 e0       	ldi	r29, 0x01	; 1'))
# print(parser.parse('  54:	90 91 60 00 	lds	r25, 0x0060	; 0x800060 <_edata>'))
# print(parser.parse('  e4:	90 83       	st	Z, r25'))
# print(parser.parse('  f2:	84 83       	std	Z+4, r24	; 0x04'))
# print(parser.parse('  e8:	02 96       	adiw	r24, 0x02	; 2'))
# print(parser.parse('00000000 <__vectors>:'))
# print(parser.parse('Disassembly of section .text:'))
# print(parser.parse(''))
# sys.exit(0)

asm_file = sys.argv[1]
with open(asm_file) as fp:
    cnt = [parser.parse(asm_line) for asm_line in fp]

try:
    last_fn = None
    last_fn_cc = None
    output_file = sys.argv[2]
    with open(output_file, 'wt') as fp:
        for token in cnt:
            if token[0] == 'junk':
                fp.write(token[1])
                continue
            if token[0] == 'fn':
                if last_fn_cc is not None:
                    fp.write("; %s (%d)\n" % (last_fn, last_fn_cc))
                fp.write("%s <%s>:\n" % (token[1], token[2]))
                last_fn = token[2]
                last_fn_cc = 0
                continue
            if token[0] == 'loc':
                print(token)
                cc = cycles.parse(token[3])
                last_fn_cc += cc[-1] if isinstance(cc, tuple) else cc
                operands = [x for x in [token[4], token[5]] if x is not None]
                old_str = "%4s: %-8s %s" % (token[1], token[3], ", ".join(operands))
                fp.write("%-40s ; %s / %d\n" % (old_str, cc, last_fn_cc))
                continue
            raise NotImplementedError('bad token: %s' % token[0])
except Exception as e:
    print(e)
    time.sleep(4)
    sys.exit(1)
